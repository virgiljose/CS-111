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
#include <sys/socket.h> /* sockets  */
#include <netinet/in.h> /* also needed for sockets */
#include <netdb.h>
#include <string.h>
#include <zlib.h>

/** MACROS **/
#define BUF_COUNT 2048
#define COMPRESSION_BUF_COUNT 1000

/** VARIABLE DECLARATIONS **/

/* compression flag */
static int compress_flag;

/* holds CR LF */
char crlf[2] = { 0x0D, 0x0A };

/* holds LF  */
char lf[1] = { 0x0A } ;

/* pipes */
int parent_to_child[2];
int child_to_parent[2];

/* child pid  */
pid_t c_pid = -1;

/* poll struct and variable */
struct pollfd polls[2];
int poll_ret;

/* waitpid status  */
int status;
pid_t w;

/* socket variables  */
int srv_sock, cln_sock, portnum;
socklen_t cln_len;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;

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

/* SET UP SOCKET */
void setup_socket() {

  /* create server socket*/
  srv_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(srv_sock < 0) {
    fprintf(stderr, "Error opening socket\r\n");
    exit(1);
  }

  memset((char *) &server_addr, 0, sizeof(server_addr));

  /* define server address */
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portnum);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  
  /* bind socket to specified IP and port */
  if(bind(srv_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    fprintf(stderr, "Error binding\r\n");
    exit(1);
  }

  listen(srv_sock, 5);

  cln_len = sizeof(client_addr);

  cln_sock = accept(srv_sock, (struct sockaddr *) &client_addr, &cln_len);
  if(cln_sock < 0) {
    fprintf(stderr, "Error accepting client\r\n");
    exit(1);
  }

}



/** SET UP PIPES **/
int setup_pipes() {
  if(pipe(parent_to_child) == -1) {
    fprintf(stderr, "Error creating pipe\r\n");
    exit(1);
  }

  if(pipe(child_to_parent) == -1) {
    fprintf(stderr, "Error creating pipe\r\n");
    exit(1);
  }

  return child_to_parent[0];
}



/** --shell: PARENT PROCESS FUNCTIONS **/

/* set up parent's pipes*/
void setup_parent() {

  /* Close unused pipes */
  close(parent_to_child[0]);
  close(child_to_parent[1]);

}

/* parent: read from client */
void parent_readclient() {
  
  char buf[BUF_COUNT];
  int bytes = 0;
  int i;

  memset((char *) &buf, 0, sizeof(char));
  bytes = read(cln_sock, buf, sizeof(char));

  if(bytes < 0)
    fprintf(stderr, "Error reading from socket\r\n");

  if(compress_flag) {

    char compression_buf[BUF_COUNT];
    from_shell.avail_in = bytes;
    from_shell.avail_out = BUF_COUNT;
    from_shell.next_out = (unsigned char *) compression_buf;
    from_shell.next_in = (unsigned char *) buf;


    do {
      inflate(&from_shell, Z_SYNC_FLUSH);
    }
    while (from_shell.avail_in > 0);

    int compressed_bytes = BUF_COUNT - ((int)from_shell.avail_out);

    for(i = 0; i < compressed_bytes; i++) {
      if(compression_buf[i] == 0x04) {
	close(parent_to_child[1]);
	break;
      }
      else if(compression_buf[i] == 0x03) {
	kill(c_pid, SIGINT);
	break;
      }
      else if (compression_buf[i] == 0x0A || compression_buf[i] == 0x0D) {
	//	from_shell.next_in = (unsigned char *) lf;
	write(parent_to_child[1], lf, sizeof(char));
      }	
      else
	write(parent_to_child[1], &compression_buf[i], sizeof(char));
    }
    //    fprintf(stderr, "server read client\r\n");
  }	
  else {
    for(i = 0; i < bytes; i++) {
      if(buf[i] == 0x04)
	close(parent_to_child[1]);
      else if(buf[i] == 0x03)
	kill(c_pid, SIGINT);
      else if (buf[i] == 0x0A || buf[i] == 0x0D)
	write(parent_to_child[1], lf, sizeof(char));
      else
	write(parent_to_child[1], &buf[0], sizeof(char));
    }
  }
}

/* parent: read from shell */
void parent_readchild() {

  char buf[BUF_COUNT];
  int bytes = 0;
  int n;

  memset(buf, 0, BUF_COUNT);

  bytes = read(child_to_parent[0], buf, BUF_COUNT);
  if(bytes > 0) {
    int i; /* offset of buf */

    if(compress_flag) {
      int write_starting_pos;
      int count = 0; /* how many bytes to compress and write out */
      int encounter_lf = 0;

      for(i = 0, write_starting_pos = 0; i < bytes; i++) {

	if(buf[i] == 0x0A || buf[i] == 0x0D) {

	  encounter_lf = 1;
	  /* first, write everything before encountering lf*/
	  char compression_buf[COMPRESSION_BUF_COUNT];
	  to_shell.avail_out = sizeof(compression_buf);
	  to_shell.next_out = (unsigned char *) compression_buf;
	  to_shell.avail_in = count;
	  to_shell.next_in = (unsigned char *) &buf[write_starting_pos];
	  do {
	    deflate(&to_shell, Z_SYNC_FLUSH);
	  }
	  while (to_shell.avail_in > 0);
	  write(cln_sock, compression_buf, sizeof(compression_buf) - to_shell.avail_out);

	  /* then, write cr lf*/
	  char crlf_compression_buf[COMPRESSION_BUF_COUNT];
	  to_shell.next_out = (unsigned char *) crlf_compression_buf;
	  to_shell.avail_out = sizeof(crlf_compression_buf);
	  to_shell.avail_in = 2;
	  to_shell.next_in = (unsigned char *) crlf;
	  do {
	    deflate(&to_shell, Z_SYNC_FLUSH);
	  }
	  while (to_shell.avail_in > 0);
	  write(cln_sock, crlf_compression_buf, sizeof(crlf_compression_buf) - to_shell.avail_out);

	  write_starting_pos += count + 1;
	  count = 0;
	  encounter_lf = 0;
	}
	else
	  count++;
      }
      if(!encounter_lf) {
	  char compression_buf[COMPRESSION_BUF_COUNT];
	  to_shell.avail_out = sizeof(compression_buf);
	  to_shell.next_out = (unsigned char *) compression_buf;
	  to_shell.avail_in = count;
	  to_shell.next_in = (unsigned char *) &buf[write_starting_pos];
	  do {
	    deflate(&to_shell, Z_SYNC_FLUSH);
	  }
	  while (to_shell.avail_in > 0);
	  write(cln_sock, compression_buf, sizeof(compression_buf) - to_shell.avail_out);

      }
      //fprintf(stderr, "server read child\r\n");
    }
    else {
      for(i = 0; i < bytes; i++) {
	if(buf[i] == 0x0A || buf[i] == 0x0D) {
	  n = write(cln_sock, crlf, 2);
	  if(n < 0) {
	    fprintf(stderr, "Error writing to socket\r\n");
	    exit(1);
	  }
	}
	else {
	  n = write(cln_sock, &buf[i], 1);
	  if(n < 0) {
	    fprintf(stderr, "Error writing to socket\r\n");
	    exit(1);
	  }	
	}
      }
    }

  }
  else if(bytes < 0)
    fprintf(stderr, "Error reading from child shell\r\n");
}



/** --shell: CHILD PROCESS  **/
void child_process() {

  close(parent_to_child[1]);
  close(child_to_parent[0]);

  dup2(parent_to_child[0], STDIN_FILENO);
  dup2(child_to_parent[1], STDOUT_FILENO);
  dup2(child_to_parent[1], STDERR_FILENO);

  close(parent_to_child[0]);
  close(child_to_parent[1]);

  char * execvp_argv[2];
  char execvp_fn[] = "/bin/bash";
  execvp_argv[0] = execvp_fn;
  execvp_argv[1] = NULL;
  if(execvp(execvp_fn, execvp_argv) == -1) {
    fprintf(stderr, "Error with execvp()\r\n");
    exit(1);
  }
}

 
/* POLL FUNCTIONS */

/* set up poll */
void setup_poll(int childstdin) {

    polls[0].fd = cln_sock;
    polls[1].fd = childstdin;

    polls[0].events = POLLIN|POLLHUP|POLLERR|WNOHANG;
    polls[1].events = POLLIN|POLLHUP|POLLERR|WNOHANG;

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
      parent_readclient();
    
    if((polls[1].revents&POLLIN) == POLLIN)
      parent_readchild();
    
    if((polls[0].revents&POLLHUP) == POLLHUP) {
      close(child_to_parent[0]);
      break;
    }

    if((polls[1].revents&POLLHUP) == POLLHUP) {
      kill(c_pid, SIGINT);
      break;
    }

    if((polls[0].revents&POLLERR) == POLLERR) {
      fprintf(stderr, "I/0 error\r\n");
      close(child_to_parent[0]);
      break;
    }
     
    if((polls[1].revents&POLLERR) == POLLERR) {
      fprintf(stderr, "I/0 error\r\n");
      kill(c_pid, SIGINT);
      break;
    }
    
  }
}



/* FUNCTION TO HANDLE GETOPT */
void getopt_fctn(int argc, char ** argv) {

  /* getopt options struct */
  int c;
  static struct option long_options[] =
    {
      {"compress", no_argument, &compress_flag, 1},
      {"port", required_argument, 0, 'p'},
      {0, 0, 0, 0}
    };
  int option_index = 0;

  while(1) {
    c = getopt_long(argc, argv, "p:", long_options, &option_index);
    if (c == -1)
      break;    
    switch(c) {
    case 0:
      if(long_options[option_index].flag != 0)
	break;
      break;
    case 'p':
      portnum = atoi(optarg);
      break;
    case '?':
      fprintf(stderr, "Usage: ./lab1b-server --port=portnum (--log=logfile)\r\n");
      exit(1);
      break;
    default:
      fprintf(stderr, "Usage: ./lab1b-server --port=portnum (--log=logfile)\r\n");
      exit(1);
    }   
  }
  if(compress_flag)
    setup_zlib();
}



int main(int argc, char ** argv) {

  getopt_fctn(argc, argv); /* parse options using getopt. call relevant functions. */
  setup_socket();

  int ctp_pipeval = setup_pipes();
  setup_poll(ctp_pipeval);
  c_pid = fork();

  if(c_pid > 0) {
    setup_parent();
    poll_loop();
    w = waitpid(c_pid, &status, WNOHANG|WUNTRACED);
    if(w == -1) {
      fprintf(stderr, "waitpid failure\r\n");
      exit(1);
    }
    else if (WIFEXITED(status))
      fprintf(stderr, "SHELL EXIT SIGNAL=0 STATUS=%d\r\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
      fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n", WTERMSIG(status), WEXITSTATUS(status));
    else if (WIFSTOPPED(status))
      fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n", WSTOPSIG(status), WEXITSTATUS(status));
  }  
  else if (c_pid == 0)
    child_process();
  else {
    fprintf(stderr, "Error creating child process\r\n");
    exit(1);
  }
  if(compress_flag)
    end_zlib();
  exit(0);
}
