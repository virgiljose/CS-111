// NAME: Virgil Jose
// EMAIL: xxxxxxxxx
// ID: xxxxxxxxx

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

/* variables for synchronization options (i.e. locks) */
pthread_mutex_t add_mutex;
int spin_lock = 0;

/* specify number of threads and iterations */
int num_threads = 1;
int num_iterations = 1;

/* variables for time implementation */
struct timespec begin_time;
struct timespec end_time;

/* counter used for add */
long long counter = 0;

/* print statistics */
void print_stats() {

  long long elapsed_time = (end_time.tv_sec - begin_time.tv_sec) * 1000000000;
  elapsed_time += end_time.tv_nsec;
  elapsed_time -= begin_time.tv_nsec;
  long long num_operations = num_threads * num_iterations * 2;
  long long avg_run_time = elapsed_time/num_operations;

  char test_name[20];

  if(opt_yield) {
    if(opt_mutex)
      strcpy(test_name, "add-yield-m");
    else if(opt_spin_lock)
      strcpy(test_name, "add-yield-s");
    else if(opt_compare_and_swap)
      strcpy(test_name, "add-yield-c");
    else
      strcpy(test_name, "add-yield-none");
  }
  else {
    if(opt_mutex)
      strcpy(test_name, "add-m");
    else if(opt_spin_lock)
      strcpy(test_name, "add-s");
    else if(opt_compare_and_swap)
      strcpy(test_name, "add-c");
    else
      strcpy(test_name, "add-none");
  }

  printf("%s,%d,%d,%lld,%lld,%lld,%lld\n", test_name, num_threads, num_iterations, num_operations, elapsed_time, avg_run_time, counter);
}

/* Add function */
void add(long long * pointer, long long value) {

  /* lock/yield: about to enter critical section */
  if(opt_mutex)
    pthread_mutex_lock(&add_mutex);
  if(opt_spin_lock)
    while(__sync_lock_test_and_set(&spin_lock, 1));

  if(opt_compare_and_swap) {
    long long prev, sum;
    do {
      prev = *pointer;
      sum = prev + value;
      if(opt_yield)
	sched_yield();
    } while(__sync_val_compare_and_swap(pointer, prev, sum) != prev);
    return;
  }

  /* begin critical section */
  long long sum = *pointer + value;

  if(opt_yield)
    sched_yield();

  *pointer = sum;

  /* end critical section: unlock */
  if(opt_mutex)
    pthread_mutex_unlock(&add_mutex);
  if(opt_spin_lock)
    __sync_lock_release(&spin_lock);
}

/* (Multi-threaded) Add wrapper function */
void * add_wrapper(void * counter_ptr) {

  long long * ptr = (long long *) counter_ptr;
  int i;

  for(i = 0; i < num_iterations; i++)
    add(ptr, 1);
  for(i = 0; i < num_iterations; i++)
    add(ptr, -1);

  pthread_exit(NULL);
  return NULL;
}

/* Creates and runs threads */
void run_threads() {  

  /* initialize variables needed for multithreading implementation */
  pthread_t threads[num_threads];
  int rc, i;
  void * status;

  /* create and run threads */
  //  get_time(begin_time);

  
  if(clock_gettime(CLOCK_MONOTONIC, &begin_time) == -1) {
    fprintf(stderr, "Error getting time\n");
    exit(1);
  } 
  

  for(i = 0; i < num_threads; i++) {
    //    fprintf(stderr, "In main: Creating thread %d\n", i);
    rc = pthread_create(&threads[i], NULL, add_wrapper, &counter);
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
    //    fprintf(stderr, "Main: completed join with thread %d having status %ld\n", i, (long) status);
  }
  //  get_time(end_time);
  
   if(clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) {
    fprintf(stderr, "Error getting time\n");
    exit(1);
    } 
  
  //  fprintf(stderr, "value of counter: %lld\n", counter);
  return;
}

/* parse arguments */
void parse(int argc, char * argv[]) {
  
  int c;
  
  while(1) {
    static struct option long_options[] = {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", no_argument, 0, 'y'},
      {"sync", required_argument, 0, 's'},
      {0, 0, 0, 0}
    };
  
    int option_index = 0;
    c = getopt_long(argc, argv, "t:i:y:s:", long_options, &option_index);
    if(c == -1)
      break;
 
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
    case 'y':
      opt_yield = 1;
      break;
   case 's':
     if(strcmp(optarg, "m") == 0)
       opt_mutex = 1;
     else if(strcmp(optarg, "s") == 0)
       opt_spin_lock = 1;
     else if(strcmp(optarg, "c") == 0)
       opt_compare_and_swap = 1;
     else {
       fprintf(stderr, "sync: invalid argument. Options are 'c', 'm', and 's'\n");
       exit(1);
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
  return;
}

int main(int argc, char * argv[]) {
  parse(argc, argv); /* Parse aguments, and check whether mandatory options are specified */
  run_threads(); /* Run threads */
  print_stats(); /* Print statistics of tests */
  exit(0);
}
