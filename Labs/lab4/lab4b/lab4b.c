// NAME: Virgil Jose
// EMAIL: xxxxxxxxx
// ID: xxxxxxxxx

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <mraa.h>
#include <time.h>
#include <poll.h>
#include <getopt.h>
#include <string.h>

#define BUF_COUNT 1024

/* -------------------------------------------------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* -------------------------------------------------------------------------------------------------------------------- */

/* Variables for Options */
int period = 1; /* sampling interval, in seconds */
int scale = 1; /* temperature scale is 0 if C, and 1 if F */
FILE * logfd = NULL; /* if no logfile is specified, write output to STDOUT. otherwise, write to logfile. */

/* context variables for the peripheral devices */
mraa_aio_context temp_sensor;
mraa_gpio_context button;

/* constants for temperature sensor */
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k

/* variable to check whether to continue running program */
sig_atomic_t volatile can_run = 1;

/* variable to check whether to generate reports (i.e. can be turned off by using STOP command) */
int gen_reports = 1;

/* -------------------------------------------------------------------------------------------------------------------- */
/* IMPLEMENTATION */
/* -------------------------------------------------------------------------------------------------------------------- */

/* Get temperature */
float get_temp(int temp) {

  if(temp == -1) {
    fprintf(stderr, "Error reading AIO (temperature sensor)\n");
    exit(1);
  }

  int a = temp;
  float R = 1023.0/a-1.0;
  R = R0*R;
  float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
  if(scale) /* if we want to convert to fahrenheit */
    temperature = (temperature * 9/5) + 32;

  return temperature;

}

/* Get time */
void get_time(int hms[]) {
  time_t curr_time = time(NULL);
  struct tm *tm_struct = localtime(&curr_time);
  hms[0] = tm_struct->tm_hour;
  hms[1] = tm_struct->tm_min;
  hms[2] = tm_struct->tm_sec;
  return;
}

void make_report() {

  int ctime[3];
  get_time(ctime);

  float temp = get_temp(mraa_aio_read(temp_sensor));
  
  if(ctime[0] < 10) {
    if(logfd != NULL)
      fprintf(logfd, "0%d:", ctime[0]);
    else
      fprintf(stdout, "0%d:", ctime[0]);
  }
  else {
    if(logfd != NULL)
      fprintf(logfd, "%d:", ctime[0]);
    else
      fprintf(stdout, "%d:", ctime[0]);
  }

  if(ctime[1] < 10) {
    if(logfd != NULL)
      fprintf(logfd, "0%d:", ctime[1]);
    else
      fprintf(stdout, "0%d:", ctime[1]);
  }
  else {
    if(logfd != NULL)
      fprintf(logfd, "%d:", ctime[1]);
    else
      fprintf(stdout, "%d:", ctime[1]);
  }

  if(ctime[2] < 10) {
    if(logfd != NULL)
      fprintf(logfd, "0%d", ctime[2]);
    else
      fprintf(stdout, "0%d", ctime[2]);
  }
  else {
    if(logfd != NULL)
      fprintf(logfd, "%d", ctime[2]);
    else
      fprintf(stdout, "%d", ctime[2]);
  }

  if(can_run) {
    if(logfd != NULL)
      fprintf(logfd, " %.1f\n", temp);
    else
      fprintf(stdout, " %.1f\n", temp);
  }
  else {
    if(logfd != NULL)
      fprintf(logfd, " SHUTDOWN\n");
    else
      fprintf(stdout, " SHUTDOWN\n");
  }
}


/* Stop Getting Temperature afteer Receiving SIGINT */
void sig_handler() {
  can_run = 0;
  return;
}

/* Parse commands from STDIN */
void parse_command(char buf[], int bytes) {

  char arg[1024]; /* holds argument for the PERIOD command */

  if(strncmp(buf,"SCALE=F", bytes) == 0) {
    if(logfd != NULL)
      fprintf(logfd, "SCALE=F\n");
    else
      fprintf(stdout, "SCALE=F\n");
    scale = 1;
  }
  if(strncmp(buf,"SCALE=C", bytes) == 0) {
    if(logfd != NULL)
      fprintf(logfd, "SCALE=C\n");
    else
      fprintf(stdout, "SCALE=C\n"); 
    scale = 0;
  }
  if(strncmp(buf,"PERIOD=", 7) == 0) {
    strncpy(arg, &buf[7], bytes - 7);
    period = atoi(arg);
    if(logfd != NULL)
      fprintf(logfd, "PERIOD=%d\n", period);
    else
      fprintf(stdout, "PERIOD=%d\n", period); 
  }
  if(strncmp(buf,"STOP", bytes) == 0) {
    if(logfd != NULL)
      fprintf(logfd, "STOP\n");
    else
      fprintf(stdout, "STOP\n"); 
    gen_reports = 0;
  }
  if(strncmp(buf,"START", bytes) == 0) {
    if(logfd != NULL)
      fprintf(logfd, "START\n");
    else
      fprintf(stdout, "START\n"); 
    gen_reports = 1;
  }
  if(strncmp(buf,"LOG", 3) == 0) {
    strncpy(arg, &buf[4], bytes - 4);
    if(logfd != NULL)
      fprintf(logfd, "LOG %s\n", arg);
    else
      fprintf(stdout, "LOG %s\n", arg); 
    
  }
  if(strncmp(buf,"OFF", bytes) == 0) {
    if(logfd != NULL)
      fprintf(logfd, "OFF\n");
    else
      fprintf(stdout, "OFF\n"); 
    can_run = 0;
  }

}

/* Get Temperature, Report Output */
void run() {

  /* poll structure needed to properly read from stdin */
  struct pollfd fds[1];
  int poll_ret;
  fds[0].fd = STDIN_FILENO;
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  /* variables for reading from stdin*/
  char buf[BUF_COUNT];
  int bytes;
  memset((char *) &buf, 0, BUF_COUNT);

  /* continuously take samples until button is pressed (or SIGINT) */
  while(can_run) {

    if(gen_reports)
      make_report();

    if((poll_ret = poll(fds, 1, 0)) < 0) {
      fprintf(stderr, "Error polling\n");
      exit(1);
    }
    /* when we receive input from STDIN... */
    if(fds[0].revents & POLLIN) {
      /* read bytes into buf */
      bytes = read(STDIN_FILENO, &buf, BUF_COUNT);
      if(bytes < 0) {
	fprintf(stderr, "Error reading from STDIN\n");
	exit(1);
      }
      /* separate buf into smaller strings, each of which are commands delimited by '\n' */
      char * command;
      command = strtok(buf, "\n");
      /* process each token (i.e. each command) */
      while(command != NULL && bytes > 0) {
	parse_command(command, strlen(command));
	command = strtok(NULL, "\n");
      }
    }
    sleep(period);
  }
    make_report();
  mraa_aio_close(temp_sensor);
  mraa_gpio_close(button);

  return;
}

/* Initialize button and temperature sensor */
void init() {

  /* Initialize temperature sensor */
  temp_sensor = mraa_aio_init(1);
  if(temp_sensor == NULL) {
    fprintf(stderr, "Failed to initialize AIO (temperature sensor)\n");
    mraa_deinit();
    exit(1);
  }

  /* Initialize button */
  button = mraa_gpio_init(62);
  if(button == NULL) {
    fprintf(stderr, "Failed to initialize GPIO (button)\n");
    mraa_deinit();
    exit(1);
  }
  mraa_gpio_dir(button, MRAA_GPIO_IN);
  mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, sig_handler, NULL);
  signal(SIGINT, sig_handler);

  return;
} 

/* Parse Arguments */
void parse(int argc, char * argv[]) {
  int c;

  while(1) {
    static struct option long_options[] = {
      {"period", required_argument, 0, 'p'},
      {"scale", required_argument, 0, 's'},
      {"log", required_argument, 0, 'l'},
      {0, 0, 0, 0}
    };
    int option_index = 0;
    c = getopt_long(argc, argv, "p:l:s:", long_options, &option_index);
    if(c == -1)
      break;

    switch(c) {
    case 0:
      if(long_options[option_index].flag != 0)
        break;
      break;
    case 'p':
      period = atoi(optarg);
      break;
    case 'l':
      logfd = fopen(optarg, "w" );
      break;
    case 's':
      if(strcmp(optarg, "F") == 0)
	scale = 1;
      if(strcmp(optarg, "C") == 0)
	scale = 0;
      else {
	fprintf(stderr, "scale: invalid argument. Usage: scale=F or scale=C\n");
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
  parse(argc, argv);
  init();
  run();
  exit(0);
}
