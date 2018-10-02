// NAME: Virgil Jose
// EMAIL: xxxxxxxxx
// ID: xxxxxxxxx

#define _GNU_SOURCE
#include "SortedList.h"
#include <getopt.h> /* getopt library */
#include <pthread.h> /* pthread library */
#include <time.h> /* clock_gettime() library */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* flags for synchronization options */
int opt_yield = 0;
int opt_mutex = 0;
int opt_spin_lock = 0;
int opt_compare_and_swap = 0;

/* sorted list data structures and variables */
SortedList_t ** thread_list;
SortedListElement_t * elements;
char charset[62] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/* used for printing data */
char yield_opts[4];
char sync_opts[4];

/* variables for synchronization options (i.e. locks) */
pthread_mutex_t * list_mutex;
int * spin_lock;

/* specify number of threads and iterations (defaults for both: 1) */
unsigned int num_threads = 1;
unsigned int num_iterations = 1;
unsigned int num_lists = 1;

/* variables for time implementation */
struct timespec begin_time;
struct timespec end_time;
struct timespec mutex_lock_begin;
struct timespec mutex_lock_end;
long long * mutex_time;

long long global_mutex_time = 0;

/* free dynamically-allocated variables */
void free_ptrs() {

  unsigned int i;
  for(i = 0; i < num_lists; i++)
    free(thread_list[i]);
  free(thread_list);
  free(elements);
  if(opt_mutex && mutex_time != NULL)
    free(mutex_time);
  free(list_mutex);
  free(spin_lock);
}

/* print statistics */
void print_stats() {

  long long elapsed_time = (end_time.tv_sec - begin_time.tv_sec) * 1000000000 + (end_time.tv_nsec - begin_time.tv_nsec);
  long long num_operations = num_threads * num_iterations * 3;
  long long avg_run_time = elapsed_time/num_operations;
  long long time_wait_lock;

  if(num_operations == 0 || num_threads == 0 || opt_mutex == 0)
    time_wait_lock = 0;
  else {
    time_wait_lock = global_mutex_time;
    time_wait_lock /= num_operations;
  }

  printf("list-%s-%s,%d,%d,%d,%lld,%lld,%lld,%lld\n", yield_opts, sync_opts, num_threads, num_iterations, num_lists, num_operations, elapsed_time, avg_run_time, time_wait_lock);
}

void list_sync_lock(int list_num) {
  if(opt_mutex) {
    if(clock_gettime(CLOCK_MONOTONIC, &mutex_lock_begin) == -1) {
      fprintf(stderr, "Error getting time\n");
      exit(1);
    }
    pthread_mutex_lock(&list_mutex[list_num]);
    if(clock_gettime(CLOCK_MONOTONIC, &mutex_lock_end) == -1) {
      fprintf(stderr, "Error getting time\n");
      exit(1);
    }
    //    mutex_time[thread_id] += (mutex_lock_end.tv_sec - mutex_lock_begin.tv_sec) * 1000000000 + (mutex_lock_end.tv_nsec - mutex_lock_begin.tv_nsec);
    global_mutex_time += (mutex_lock_end.tv_sec - mutex_lock_begin.tv_sec) * 1000000000 + (mutex_lock_end.tv_nsec - mutex_lock_begin.tv_nsec);
  }
  if(opt_spin_lock)
    while(__sync_lock_test_and_set(&spin_lock[list_num], 1));
}

void list_sync_unlock(int list_num) {
  if(opt_mutex)
    pthread_mutex_unlock(&list_mutex[list_num]);
  if(opt_spin_lock)
    __sync_lock_release(&spin_lock[list_num]);
}

/* Thread function */
void * thread_fctn(void * args) {

  int tid = *(unsigned int *) args;
  unsigned int i, mod;
  unsigned int num_times = num_iterations * num_threads;
  
  /* insert elements into lists */
  for(i = tid; i < num_times; i += num_threads) {
    /* determine which list a given element is inserted into, based on the first character of the element's key */
    mod = (elements[i].key[0] - '0') % num_lists;
    list_sync_lock(mod);
    SortedList_insert(thread_list[mod], &elements[i]);
    list_sync_unlock(mod);
  }
  
  /* look up and delete each element */
  SortedListElement_t* t_element;
  for(i = tid; i < num_times; i+= num_threads) {
    mod = (elements[i].key[0] - '0') % num_lists;
    list_sync_lock(mod);
    if((t_element = SortedList_lookup(thread_list[mod], elements[i].key)) == NULL) {
      fprintf(stderr, "Error looking up element. Expected element with key %s, but none found\n", elements[i].key);
      exit(2);
    }
    else {
      if(SortedList_delete(t_element) == 1) {
	fprintf(stderr, "Error deleting element with key %s. Associated node's next and prev pointers are corrupted\n", elements[i].key);
	exit(2);
      }
    list_sync_unlock(mod);
  }
  }

  pthread_exit(NULL);
  return NULL;
}

/* Creates and runs threads */
void run_threads() {  

  /* initialize variables needed for multithreading implementation */
  pthread_t threads[num_threads];
  unsigned int i;
  int rc;
  unsigned int tid[num_threads];
  void * status;

  mutex_time = (long long *) malloc(sizeof(long long) * num_threads);
  for(i = 0; i < num_threads; i++)
    mutex_time[i] = 0;

  /* start timer */
  if(clock_gettime(CLOCK_MONOTONIC, &begin_time) == -1) {
    fprintf(stderr, "Error getting time\n");
    exit(1);
  } 
  
  /* run thread fctn */
  for(i = 0; i < num_threads; i++) {
    tid[i] = i;
    rc = pthread_create(&threads[i], NULL, thread_fctn, &tid[i]);
    if(rc) {
      fprintf(stderr, "Error: Return code from pthread_create(): %d\n", rc);
      exit(1);
    }
  }

  /* join threads */
  for(i = 0; i < num_threads; i++) {
    rc = pthread_join(threads[i], &status);
    if(rc) {
      fprintf(stderr, "Error: return code from pthread_join(): %d\n", rc);
      exit(1);
    }
  }

  int len = 0;
  for(i = 0; i < num_lists; i++)
    len += SortedList_length(thread_list[i]);

  if(len != 0) {
    fprintf(stderr, "Length nonzero.\n");
    exit(2);
  }
  
  if(clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) {
    fprintf(stderr, "Error getting time\n");
    exit(1);

  }

  return;
}

void list_init() {

  unsigned int i, j, key_size;
  unsigned int list_size = num_iterations * num_threads;

  /* allocate memory for multi-list: each is implemented as a circular list */
  thread_list = (SortedList_t **) malloc(sizeof(SortedList_t *) * num_lists);
  for(i = 0; i < num_lists; i++) {
    thread_list[i] = malloc(sizeof(SortedList_t));
    thread_list[i]->prev = thread_list[i];
    thread_list[i]->next = thread_list[i];
    thread_list[i]->key = NULL;
  }

  /* allocate memory for elements */
  elements = (SortedListElement_t *) malloc(sizeof(SortedListElement_t) * list_size);
 
  /* create each key, and insert them into the appropriate list */
  for(i = 0; i < list_size; i++) {   
    key_size = rand() % 50 + 1; /* key size is from 1 to 50 */
    char * key = (char *) malloc(sizeof(char)*key_size);
    for(j = 0; j < key_size - 1; j++) {
      int ran = (rand() % 62);
      key[j] = charset[ran];
    }
    key[key_size - 1] = '\0'; /* key should end in null byte*/
    elements[i].key = key;
  }

  if(opt_mutex) {
    list_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * num_lists);
    for(i = 0; i < num_lists; i++) {
      pthread_mutex_init(&list_mutex[i], NULL);
    }
  }

  if(opt_spin_lock) {
    spin_lock = (int *) malloc(sizeof(int) * num_lists);
    for(i = 0; i < num_lists; i++)
      spin_lock[i] = 0;
  }
}

/* parse arguments */
void parse(int argc, char * argv[]) {  
  int c;
  
  while(1) {
    static struct option long_options[] = {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", required_argument, 0, 'y'},
      {"sync", required_argument, 0, 's'},
      {"lists", required_argument, 0, 'l'},
      {0, 0, 0, 0}
    };
  
    int option_index = 0;
    c = getopt_long(argc, argv, "t:i:y:s:", long_options, &option_index);
    if(c == -1)
      break;
    unsigned int i;
    switch(c) {
    case 0:
      if(long_options[option_index].flag != 0)
	break;
      break;
    case 'l':
      num_lists = atoi(optarg);
      break;
    case 't':
      num_threads = atoi(optarg); 
      break;
    case 'i':
      num_iterations = atoi(optarg);
      break;
   case 's':
     if(optarg[0] == 'm') {
       opt_mutex = 1;
       strcat(sync_opts, "m");
     }
     else if(optarg[0] == 's') {
       opt_spin_lock = 1;
       strcat(sync_opts, "s");
     }
     else {
       fprintf(stderr, "sync: invalid argument. Options are 'c', 'm', and 's'\n");
       exit(1);
     }
     break;
    case 'y':
      if(strlen(optarg) < 1 || strlen(optarg) > 3) {
	fprintf(stderr, "yield: invalid arguments\n");
	exit(1);
      }
      strcat(yield_opts, optarg);
      for(i = 0; i < strlen(optarg); i++) {
	if(optarg[i] == 'i')
	  opt_yield |= INSERT_YIELD;
	else if(optarg[i] == 'd')
	  opt_yield |= DELETE_YIELD;
	else if(optarg[i] == 'l')
	  opt_yield |= LOOKUP_YIELD;
	else {
	  fprintf(stderr, "yield: invalid arguments\n");
	  exit(1);
	}
      }
      break;
    case '?':
      fprintf(stderr, "Invalid argument\n");
      exit(1);
      break;
    default:
      fprintf(stderr, "Invalid argument\n");
      exit(1);
      break;			
    }		
  }

  if(!opt_yield)
    strcpy(yield_opts, "none");

  if(!opt_mutex && !opt_spin_lock)
    strcpy(sync_opts, "none");

  return;
}

int main(int argc, char * argv[]) {
  parse(argc, argv); /* Parse aguments, and check whether mandatory options are specified */
  list_init(); /* Initialize the list */
  run_threads(); /* Run threads */
  print_stats(); /* Print statistics of tests */
  free_ptrs(); /* Free dynamically allocated variables */
  exit(0);
}
