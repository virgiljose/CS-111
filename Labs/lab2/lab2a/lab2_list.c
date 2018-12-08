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

/* sorted list data structures */
SortedList_t * list;
SortedListElement_t * elements;
char charset[62] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/* used for printing data */
char yield_opts[4];
char sync_opts[4];

/* variables for synchronization options (i.e. locks) */
pthread_mutex_t list_mutex;
int spin_lock = 0;

/* specify number of threads and iterations (defaults for both: 1) */
int num_threads = 1;
int num_iterations = 1;

/* variables for time implementation */
struct timespec begin_time;
struct timespec end_time;

/* free dynamically-allocated variables */
void free_ptrs() {
  free(list);
  free(elements);
}

/* print statistics */
void print_stats() {

  long long elapsed_time = (end_time.tv_sec - begin_time.tv_sec) * 1000000000;
  elapsed_time += end_time.tv_nsec;
  elapsed_time -= begin_time.tv_nsec;
  long long num_operations = num_threads * num_iterations * 3;
  long long avg_run_time = elapsed_time/num_operations;
  fprintf(stderr, "%s\n", yield_opts);
  printf("list-%s-%s,%d,%d,1,%lld,%lld,%lld\n", yield_opts, sync_opts, num_threads, num_iterations, num_operations, elapsed_time, avg_run_time);
}

/* Thread function */
void * thread_fctn(void * args) {

  int tid = *(int *) args;
  int i;
  int list_len = 0;
  int num_times = num_iterations * num_threads;

  if(opt_mutex)
    pthread_mutex_lock(&list_mutex);
  if(opt_spin_lock)
    while(__sync_lock_test_and_set(&spin_lock, 1));

  for(i = tid; i < num_times; i += num_threads)
    SortedList_insert(list, &elements[i]);

  list_len = SortedList_length(list);

  if(list_len != num_iterations) {
    fprintf(stderr, "Error inserting node. Thread list length: %d. Expected value: %d\n", list_len, num_iterations);
    exit(2);
  }
  
  SortedListElement_t* t_element;
  for(i = tid; i < num_times; i+= num_threads) {
    if((t_element = SortedList_lookup(list, elements[i].key)) == NULL) {
      fprintf(stderr, "Error looking up element. Expected element with key %s, but none found\n", elements[i].key);
      exit(2);
    }
    else
      if(SortedList_delete(t_element) == 1) {
	fprintf(stderr, "Error deleting element with key %s. Associated node's next and prev pointers are corrupted\n", elements[i].key);
	exit(2);
      }
  }

  if(opt_mutex)
    pthread_mutex_unlock(&list_mutex);
  if(opt_spin_lock)
    __sync_lock_release(&spin_lock);

  pthread_exit(NULL);
  return NULL;
}

/* Creates and runs threads */
void run_threads() {  

  /* initialize variables needed for multithreading implementation */
  pthread_t threads[num_threads];
  int rc, i;
  int tid[num_threads];
  void * status;

  if(opt_mutex)
    pthread_mutex_init(&list_mutex, NULL);

  /* start timer */
  if(clock_gettime(CLOCK_MONOTONIC, &begin_time) == -1) {
    fprintf(stderr, "Error getting time\n");
    exit(1);
  } 
  
  /* run threads */
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
  
  if(clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) {
    fprintf(stderr, "Error getting time\n");
    exit(1);
  }

  return;
}

void list_init() {

  list = malloc(sizeof(SortedList_t));

  /* i implement a circular list - the last element points to the head of the list*/
  list->prev = list;
  list->next = list;
  list->key = NULL;

  int list_size = num_iterations * num_threads;
  elements = malloc(sizeof(SortedListElement_t) * list_size);
  //  thread_elements = malloc(sizeof(SortedListElement_t *) * num_threads);
  
  int i, j, key_size;
  for(i = 0; i < list_size; i++) {
   
    key_size = rand() % 50 + 1; /* key size is from 1 to 50 */
    char * key = malloc(sizeof(char)*key_size);

    for(j = 0; j < key_size - 1; j++) {
      int ran = (rand() % 62);
      key[j] = charset[ran];
    }

    key[key_size - 1] = '\0'; /* key should end in null byte*/
    elements[i].key = key;
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
