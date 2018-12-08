// NAME: Virgil Jose
// EMAIL: xxxxxxxxx
// ID: xxxxxxxxx



/** HEADER FILES **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h> /* getopt library */
#include <sys/types.h> /* needed for fork, waitpid, and kill */
#include <sys/wait.h> /* waitpid */
#include <termios.h> /* termios library */
#include <poll.h> /* poll library */
#include <signal.h> /* signals, kill */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h> /* used for mem functions (memset, memcpy) */
#include <zlib.h> /* zlib */


/** MACROS **/

/* size of buffer */
#define BUF_COUNT 2048
#define COMPRESSION_BUF_COUNT 1000


/** GLOBAL VARIABLES **/

/* flag for --compress (getopt_long) */
static int compress_flag;

/* holds CR LF */
char crlf[2] = { 0x0D, 0x0A };

/* holds LF  */
char lf[1] = { 0x0A };

/* holds CR (newline) */
char cr[1] = { 0x0D };

/* termios structs */
struct termios t_org; /* stores original attribute */
struct termios t_new; /* stores new sttributes */

/* poll struct and variable */
struct pollfd polls[2];
int poll_ret;

/* socket variables */
int sockfd, portnum;
char * hostname;
struct sockaddr_in serv_addr;
struct hostent * server;

/* log file */
FILE * logfile = NULL;
int logfilefd;

/* zlib variables */
z_stream to_shell;
z_stream from_shell;



/** SETUP ZLIB **/
void setup_zlib() {

  to_shell.zalloc = Z_NULL;
  to_shell.zfree = Z_NULL;
  to_shell.opaque = Z_NULL;

  deflateInit(&to_shell, Z_DEFAULT_COMPRESSION);

  from_shell.zalloc = Z_NULL;
  from_shell.zfree = Z_NULL;
  from_shell.opaque = Z_NULL;

  inflateInit(&from_shell);

}

void end_zlib() {

 deflateEnd(&to_shell);
 inflateEnd(&from_shell);

}



/** LOGFILE: WRITE TO STDOUT  **/
void write_log(char buf[], int bytes, int received_or_sent) {

  if(logfile == NULL)
    return;

  if(received_or_sent == 0) {
    fprintf(logfile, "RECEIVED %d bytes: ", bytes);
  }
  else if(received_or_sent == 1) {
    fprintf(logfile, "SENT %d bytes: ", bytes);
  }

  fwrite(buf, sizeof(char), bytes, logfile);
  fprintf(logfile, "\n");

  return;

}

/** SET UP SOCKET **/
void setup_socket() {

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "Failed to open socket\r\n");
    exit(1);
  }

  server = gethostbyname("localhost");
  if(server == NULL) {
    fprintf(stderr, "Error finding host\r\n");
    exit(1);
  }

  memset( (char *) server->h_addr, 0, sizeof(serv_addr)); /* use instead of bzero */
  serv_addr.sin_family = AF_INET;
  memcpy( (char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length); /* use instead of bcopy */
  serv_addr.sin_port = htons(portnum);
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error connecting\r\n");
    exit(1);
  }

}



/** GET AND SET TERMIOS ATTRIBUTES **/
void t_set() {

  /* get termios attributes and store them in the struct*/
  tcgetattr(STDIN_FILENO, &t_org);
  tcgetattr(STDIN_FILENO, &t_new);

  /* change the following termios attributes: */
  t_new.c_iflag = ISTRIP; /* lower 7 bits */
  t_new.c_oflag = 0; /* don't process */
  t_new.c_lflag = 0; /* don't process */

  /* set those attributes and store them in the t_new struct: */
  tcsetattr(STDIN_FILENO, TCSANOW, &t_new);
}

void t_restore_err() {
  tcsetattr(STDIN_FILENO, TCSANOW, &t_org);
  exit(1);
}



/** READ FROM STDIN **/
void read_stdin() {
  
  char buf[BUF_COUNT];
  int bytes = 0;

  memset((char *) &buf, 0, BUF_COUNT); /* use instead of bcopy */
  bytes = read(STDIN_FILENO, buf, BUF_COUNT);

  if(bytes > 0) {

    int i;
    for (i = 0; i < bytes; i++) {
      if(buf[i] == 0x0A || buf[i] == 0x0D)
	write(STDOUT_FILENO, crlf, 2);
      else
	write(STDOUT_FILENO, buf, 1);
    }

    if(compress_flag) {
      char compression_buf[COMPRESSION_BUF_COUNT];
      to_shell.avail_in = bytes;
      to_shell.next_in = (unsigned char *) buf;
      to_shell.avail_out = COMPRESSION_BUF_COUNT;
      to_shell.next_out = (unsigned char *) compression_buf;      
      do {
	deflate(&to_shell, Z_SYNC_FLUSH);
      }
      while (to_shell.avail_in > 0);
      write_log(compression_buf, COMPRESSION_BUF_COUNT - to_shell.avail_out, 1);
      write(sockfd, compression_buf, COMPRESSION_BUF_COUNT - to_shell.avail_out);
      //      fprintf(stderr, "client read from stdin\r\n");
    }
    else {
	write(sockfd, buf, bytes);
	write_log(buf, bytes, 1);
    }

  }
  else if(bytes < 0) {
    fprintf(stderr, "Error reading from socket\r\n");
    t_restore_err();
  }
}



/* READ FROM SERVER */
void read_server() {

  char buf[BUF_COUNT];
  int bytes = 0;

  memset( (char *) &buf, 0, BUF_COUNT); /* use instead of bcopy */
  bytes = read(sockfd, buf, BUF_COUNT);

  if(bytes > 0) {
    write_log(buf, bytes, 0);
    if(compress_flag) {
      char decompression_buf[BUF_COUNT];
      from_shell.avail_in = bytes;
      from_shell.avail_out = BUF_COUNT;
      from_shell.next_out = (unsigned char *) decompression_buf;
      from_shell.next_in = (unsigned char *) buf;
      do {
	inflate(&from_shell, Z_SYNC_FLUSH);
      }
      while (from_shell.avail_in > 0);
      write(STDOUT_FILENO, decompression_buf, BUF_COUNT - from_shell.avail_out);
      //  fprintf(stderr, "client read from server\r\n");
    }
      
    else
	write(STDOUT_FILENO, buf, bytes);
  }
  else if(bytes == 0) {
    tcsetattr(STDIN_FILENO, TCSANOW, &t_org);
    exit(0);
  }
  else {
    fprintf(stderr, "Error reading from socket\r\n");
    t_restore_err();
  }
}



/* POLL FUNCTIONS */

/* set up poll */
void setup_poll() {

  polls[0].fd = STDIN_FILENO;
  polls[1].fd = sockfd;

  polls[0].events = POLLIN;
  polls[1].events = POLLIN|POLLHUP|POLLERR;

  polls[0].revents = 0;
  polls[1].revents = 0;

}

/* poll loop */
void poll_loop() {

  while(1) {
    poll_ret = poll(polls, (unsigned long)2, -1);
    if(poll_ret < 0) {
      fprintf(stderr, "Error using poll\r\n");
      exit(1);
    }

    if((polls[0].revents&POLLIN) == POLLIN)
      read_stdin();

    if((polls[1].revents&POLLIN) == POLLIN)
      read_server();

    if((polls[1].revents&POLLHUP) == POLLHUP) {
      tcsetattr(STDIN_FILENO, TCSANOW, &t_org);
      if(logfile != NULL)
	fclose(logfile);
      break;
    }

    if((polls[1].revents&POLLERR) == POLLERR) {
      fprintf(stderr, "I/0 error\r\n");
      if(logfile != NULL)
	fclose(logfile);
      break;
    }
  }
}



/* FUNCTION TO HANDLE GETOPT */
void getopt_fctn(int argc, char ** argv) {

  int port_specified = 0; /* check whether user specified a port number */

  /* getopt options struct */
  int c;
  static struct option long_options[] =
    {
      {"compress", no_argument, &compress_flag, 1},
      {"port", required_argument, 0, 'p'},
      {"log", required_argument, 0, 'l'},
      {0, 0, 0, 0}
    };

  int option_index = 0;
  while(1) {
    c = getopt_long(argc, argv, "p:l", long_options, &option_index);
    if (c == -1)
      break;    
    switch(c) {
    case 0:
      if(long_options[option_index].flag != 0)
	break;
      break;
    case 'p':
      portnum = atoi(optarg);
      port_specified = 1;
      break;
    case 'l':
      logfile = fopen(optarg, "w");
      if(logfile == NULL) {
	fprintf(stderr, "Unable to open file\n");
	exit(1);
      }	
      logfilefd = fileno(logfile);
      break;
    case '?':
      fprintf(stderr, "Usage: ./lab1b-client --port=portnum\r\n");
      exit(1);
      break;
    default:
      fprintf(stderr, "Usage: ./lab1b-client --port=portnum\r\n");
      exit(1);
      break;
    }   
  } 

  if(!port_specified) {
    fprintf(stderr, "Port not specified. Usage: ./lab1b-client --port=portnum\r\n");
    exit(1);
  }
  if(compress_flag) {
    setup_zlib();
    fprintf(stderr, "setup\n");
  }
}



int main(int argc, char ** argv) {

  getopt_fctn(argc, argv); /* parse options using getopt. call relevant functions, declare relevant variables based on options set. */
  setup_socket(); /* set up socket */
  setup_poll(); /* setup poll */
  t_set(); /* set up terminal attributes */
  poll_loop(); /* handle read/write */
  end_zlib();
  exit(0);

}
