// NAME: Virgil Jose
// EMAIL: xxxxxxxxx
// ID: xxxxxxxxx

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

/** VARIABLE DECLARATIONS **/

/* flag used for getopt_long*/
static int shell_flag;

/* macro that holds size of buffer */
#define BUF_COUNT 100

/* holds CR LF */
char crlf[2] = { 0x0D, 0x0A };

/* holds LF  */
char lf[1] = { 0x0A } ;

/* termios structs */
struct termios t_org; /* stores original attribute */
struct termios t_new; /* stores new sttributes */

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

/** SIGNAL HANDLER FUNCTION  **/
//static void sig_handler(int signum) {
//  if(signum == SIGPIPE) {
//    fprintf(stderr, "SIGPIPE detected. Exiting with exit code 1\n");
//    exit(1);
//  }
//  if(signum == SIGINT) {
//    fprintf(stderr, "SIGINT detected. Exiting with exit code 0\n");
//    exit(1);
//  }
// }



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



/** NO ARGUMENT: READ FROM FD0 AND WRITE TO FD1  **/
void rin_wout() {

  char buf[BUF_COUNT];
  int bytes;

  while( (bytes = read(0, buf, BUF_COUNT) ) > 0) { 
    int i;
    for(i = 0; i < bytes; i++) {
      if (buf[i] == 0x04) {
	tcsetattr(0, TCSANOW, &t_org);
	//	fprintf(stderr, "^D detected. Exiting program\r\n");
	exit(0);
      }
      else if (buf[i] == 0x0A || buf[i] == 0x0D)
	write(STDOUT_FILENO, crlf, 2);
      else
	write(STDOUT_FILENO, &buf[i], 1);
    }
  }
   tcsetattr(0, TCSANOW, &t_org);
}



/** --shell: PARENT PROCESS FUNCTIONS **/

/* set up parent's pipes*/
void setup_parent() {

  /* Close unused pipes */
  close(parent_to_child[0]);
  close(child_to_parent[1]);

}

/* parent: read from stdin */
void parent_readstdin() {
  
  char buf[BUF_COUNT];
  int bytes = 0;
  /* Read from STDIN, write to child and STDOUT*/
  bytes = read(STDIN_FILENO, buf, BUF_COUNT);

  if(bytes > 0) {
    int i;
    for (i = 0; i < bytes; i++) {

        if(buf[i] == 0x04) {
	  //	  fprintf(stderr, "^D encountered\r\n");
	  close(parent_to_child[1]);
	  //	  tcsetattr(STDIN_FILENO, TCSANOW, &t_org);
	  break;
	}
	else if(buf[i] == 0x03) {
	  //	  fprintf(stderr, "^C encountered, SIGINT sent to process %d\r\n", c_pid);
	  kill(c_pid, SIGINT);
	  //	  write(STDOUT_FILENO, &buf[i], 1);
	  //	  write(parent_to_child[1], &buf[i], 1);
	  //	  close(parent_to_child[1]);
	  break;
	}
	else if (buf[i] == 0x0A || buf[i] == 0x0D) {
	  write(STDOUT_FILENO, crlf, 2);
	  write(parent_to_child[1], lf, 1);
	}
	else {
	  write(STDOUT_FILENO, &buf[i], 1);
	  write(parent_to_child[1], &buf[i], 1);
	}
    }
  }
}

/* parent: read from child */
void parent_readchild() {

  char buf[BUF_COUNT];
  int bytes = 0;
  /* Read from (child) shell process, write to STDOUT */
  bytes = read(child_to_parent[0], buf, BUF_COUNT);
  if(bytes > 0) {
    int i;
    for(i = 0; i < bytes; i++) {
      if(buf[i] == 0x0A || buf[i] == 0x0D)
	write(STDOUT_FILENO, crlf, 2);
      else
	write(STDOUT_FILENO, &buf[i], 1);
    }
  }
}



/** --shell: CHILD PROCESS  **/
void child_process() {

  //  signal(SIGPIPE, sig_handler);

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
void setup_poll(int child_stdout) {

    polls[0].fd = STDIN_FILENO;
    polls[1].fd = child_stdout;

    polls[0].events = POLLIN|POLLHUP|POLLERR;
    polls[1].events = POLLIN|POLLHUP|POLLERR;

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
      parent_readstdin();
    
    if((polls[1].revents&POLLIN) == POLLIN)
      parent_readchild();
    
    if((polls[0].revents&POLLHUP) == POLLHUP) {
      //      fprintf(stderr, "POLLHUP detected\r\n");
      tcsetattr(STDIN_FILENO, TCSANOW, &t_org);
      break;
      //      kill(c_pid, SIGKILL);
      //  exit(0);
    }
      
    if((polls[1].revents&POLLHUP) == POLLHUP) {
      //      fprintf(stderr, "POLLHUP detected\r\n");
      tcsetattr(STDIN_FILENO, TCSANOW, &t_org);
      break;
      //      kill(c_pid, SIGKILL);
      //      exit(0);
    }
      
    if((polls[0].revents&POLLERR) == POLLERR) {
      fprintf(stderr, "I/0 error\r\n");
      tcsetattr(STDIN_FILENO, TCSANOW, &t_org);
      break;
      //      kill(c_pid, SIGKILL);
      //      exit(1);
    }
     
    if((polls[0].revents&POLLERR) == POLLERR) {
      fprintf(stderr, "I/0 error\r\n");
      tcsetattr(STDIN_FILENO, TCSANOW, &t_org);
      break;
      //      kill(c_pid, SIGKILL);
      //      exit(1);
    }
    
  }

}



/* FUNCTION TO HANDLE GETOPT */
void getopt_fctn(int argc, char ** argv) {
  /* getopt options struct */
  int c;
  static struct option long_options[] =
    {
      {"shell", no_argument, &shell_flag, 1},
      {0, 0, 0, 0}
    };
  int option_index = 0;
  while(1) {
    c = getopt_long(argc, argv, ":", long_options, &option_index);
    if (c == -1)
      break;    
    switch(c) {
    case 0:
      if(long_options[option_index].flag != 0)
	break;
      //      printf("option %s", long_options[option_index]);
      break;
    case '?':
      fprintf(stderr, "Usage: ./lab1a (--shell)\r\n");
      exit(1);
      break;
    default:
      fprintf(stderr, "Usage: ./lab1a (--shell)\r\n");
      exit(1);
    }   
  }
}



int main(int argc, char ** argv) {

  getopt_fctn(argc, argv); /* parse options using getopt. call relevant functions. */
  t_set(); /* set up terminal attributes */

  if(shell_flag) {

    int ctp_pipeval = setup_pipes();
    setup_poll(ctp_pipeval);
    c_pid = fork();

    if(c_pid > 0) {
      //      fprintf(stderr, "child process #: %d\r\n", c_pid);
      setup_parent();
      poll_loop();
      w = waitpid(c_pid, &status, WUNTRACED);
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
      tcsetattr(STDIN_FILENO, TCSANOW, &t_org);
      exit(1);
    }
  }
  else
    rin_wout();

  exit(0);
}
