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
#include <ctype.h> /* used for parsing port # (the isdigits function) */
#include <sys/types.h> /* needed for fork, waitpid, and kill */
#include <sys/wait.h> /* waitpid */
#include <poll.h> /* poll library */
#include <signal.h> /* signals, kill */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_COUNT 1024

/* -------------------------------------------------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* -------------------------------------------------------------------------------------------------------------------- */

/* Variables for Options */

int period = 1; /* sampling interval, in seconds */
int scale = 1; /* temperature scale is 0 if C, and 1 if F */
FILE * logfd = NULL; /* if no logfile is specified, write output to sockfd. otherwise, write to logfile. */
int userID = -1; /* user ID */

/* variables for server connection */
int sockfd = 2;
struct sockaddr_in serv_addr;
struct hostent * server;
int port = -1; /* port number */
char * hostname = NULL; /* name or address of host */

/* context variables for the peripheral devices */
mraa_aio_context temp_sensor;
// mraa_gpio_context button;

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
    
  char time_report[18];

  float temp = get_temp(mraa_aio_read(temp_sensor));
    
  if(ctime[0] < 10)
    sprintf(time_report, "0%d:", ctime[0]);
  else
    sprintf(time_report, "%d:", ctime[0]);

  if(ctime[1] < 10)
    sprintf(&time_report[3], "0%d:", ctime[1]);
  else
    sprintf(&time_report[3], "%d:", ctime[1]);

  if(ctime[2] < 10)
    sprintf(&time_report[6], "0%d ", ctime[2]);
  else
    sprintf(&time_report[6], "%d ", ctime[2]);

  if(can_run)
    sprintf(&time_report[9], "%.1f", temp);
  else
    sprintf(&time_report[9], "SHUTDOWN");

  fprintf(logfd, "%s\n", time_report);
  dprintf(sockfd, "%s\n", time_report);
}


/* Stop Getting Temperature afteer Receiving SIGINT */
void sig_handler() {
  can_run = 0;
  return;
}

/* Parse commands from server */
void parse_command(char buf[], int bytes) {

  char arg[1024]; /* holds argument for the PERIOD command */

  if(strncmp(buf,"SCALE=F", bytes) == 0) {
    fprintf(logfd, "SCALE=F\n");
    scale = 1;
  }
  if(strncmp(buf,"SCALE=C", bytes) == 0) {
    fprintf(logfd, "SCALE=C\n");
    scale = 0;
  }
  if(strncmp(buf,"PERIOD=", 7) == 0) {
    strncpy(arg, &buf[7], bytes - 7);
    period = atoi(arg);
    fprintf(logfd, "PERIOD=%d\n", period);
  }
  if(strncmp(buf,"STOP", bytes) == 0) {
    gen_reports = 0;
    fprintf(logfd, "STOP\n");
  }
  if(strncmp(buf,"START", bytes) == 0) {
    gen_reports = 1;
    fprintf(logfd, "START\n");
  }
  if(strncmp(buf,"LOG", 3) == 0) {
    strncpy(arg, &buf[4], bytes - 4);
    fprintf(logfd, "LOG %s\n", arg);
  }
  if(strncmp(buf,"OFF", bytes) == 0) {
    can_run = 0;
    fprintf(logfd, "OFF\n");
    make_report();
  }

}

/* Get Temperature, Report Output */
void run() {

  /* poll structure needed to properly read from sockfd */
  struct pollfd fds[1];
  int poll_ret;
  fds[0].fd = sockfd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  /* variables for reading from server */
  char buf[BUF_COUNT];
  int bytes;
  memset((char *) &buf, 0, BUF_COUNT);

  /* continuously take samples until button is pressed (or SIGINT) */
  while(can_run) {

    if((poll_ret = poll(fds, 1, 0)) < 0) {
      fprintf(stderr, "Error polling\n");
      exit(1);
    }
      
    if(gen_reports)
        make_report();
      
    /* when we receive input from sockfd ... */
    if(fds[0].revents & POLLIN) {
      /* read bytes into buf */
      bytes = read(sockfd, &buf, BUF_COUNT);
      if(bytes < 0) {
	fprintf(stderr, "Error reading from server\n");
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
  mraa_aio_close(temp_sensor);

  return;
}

void init_connection() {
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to open socket\r\n");
        exit(1);
    }
    
    server = gethostbyname(hostname);
    if(server == NULL) {
        fprintf(stderr, "Error finding host\r\n");
        exit(1);
    }
    
    memset( (char *) &serv_addr, 0, sizeof(serv_addr)); /* use instead of bzero */
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length); /* use instead of bcopy */
    serv_addr.sin_port = htons(port);
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Error connecting\r\n");
        exit(1);
    }
    
    if(logfd != NULL)
        fprintf(logfd, "ID=%d\n", userID);
    dprintf(sockfd, "ID=%d\n", userID);
    
    fprintf(stderr, "init connection successful\n");
    
}

/* Initialize temperature sensor */
void init_sensors() {
  temp_sensor = mraa_aio_init(1);
  if(temp_sensor == NULL) {
    fprintf(stderr, "Failed to initialize AIO (temperature sensor)\n");
    mraa_deinit();
    exit(1);
  }
  return;
} 

/* Parse Arguments */
void parse(int argc, char * argv[]) {
    
  /* parse for port number (the non-option argument) */
  int i;
  for(i = 1; i < argc; i++) {
      int isnumber = 1;
      int arglen = strlen(argv[i]);
      int j;
      for(j = 0; j < arglen; j++) {
          if(!isdigit(argv[i][j])) {
              isnumber = 0;
              break;
          }
      }
      if(isnumber) {
          port = atoi(argv[i]);
          fprintf(stderr, "%d\n", port);
          break;
      }
  }

  fprintf(stderr, "parse successful\n");

  int c;
    
  while(1) {
    static struct option long_options[] = {
      {"period", required_argument, 0, 'p'},
      {"scale", required_argument, 0, 's'},
      {"log", required_argument, 0, 'l'},
        {"id", required_argument, 0, 'i'},
        {"host", required_argument, 0, 'h'},
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
    case 'i':
        if(strlen(optarg) != 9) {
            fprintf(stderr, "ID must be 9 characters long\n");
            exit(1);
        }
        else
            userID = atoi(optarg);
        break;
    case 'h':
        hostname = malloc(strlen(optarg) * sizeof(char));
        strcpy(hostname, optarg);
        fprintf(stderr, "%s\n", hostname);
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

    if(logfd == NULL) {
        fprintf(stderr, "No logfile name given\n");
        exit(1);
    }
    if(userID == -1) {
        fprintf(stderr, "No user ID given \n");
        exit(1);
    }
    if(hostname == NULL) {
        fprintf(stderr, "No hostname given\n");
        exit(1);
    }
    if(port == -1) {
        fprintf(stderr, "No port number given\n");
        exit(1);
    }

  return;
}

int main(int argc, char * argv[]) {
  parse(argc, argv);
  init_sensors();
  init_connection();
  run();
  exit(0);
}
