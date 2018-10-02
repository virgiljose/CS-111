// NAME: Virgil Jose
// EMAIL: xxxxxxxxx
// ID: xxxxxxxxx

#include <stdio.h> // standard i/o
#include <stdlib.h>
#include <getopt.h> // parser
#include <unistd.h>
#include <fcntl.h>
#include <signal.h> // signal handling
#include <errno.h> // used for perror
#include <string.h> // used for stderror

// segfault function
void seg_fault() {
  char * dummy = NULL;
  dummy[69] = 'A';
}

// signal handler function
void handler(int signum) {
  if(signum == SIGSEGV) {
    fprintf(stderr, "Segmentation fault\n");
    exit(4);
  }
}

// read to stdin, write to stdout
void readin_writeout() {

  char buf[1000];
  int ibytes, obytes;

  while((ibytes = read(0, buf, 1000)) > 0) { 
      if((obytes = write(1, buf, ibytes)) == -1) {
	perror("Error writing to output");
	exit(-1);
      }
      if(ibytes != obytes) {
	perror("Error writing to output");
	exit(-1);
      }
  }

  if(ibytes == -1) {
      perror("Error reading from input.");
      exit(-1);
    }

  exit(0);
}

// flag used for segfault
static int seg_flag;

int main(int argc, char ** argv) {

  ////////////////////////////////////////////////////////
  /////// PARSE OPTIONS, SAVE OPTIONS TO VARIABLES ///////
  ////////////////////////////////////////////////////////

  // inspiration: 
  // https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
  // https://stackoverflow.com/questions/1052746/getopt-does-not-parse-optional-arguments-to-parameters

  // create structure that holds all possible "long" options and their corresponding "short" options
  // in the gnu.org example, this was declared inside the while-loop below. however, i found that it
  // would be better to declaere it outside the while-loop as declaring it over and over again is
  // redundant. doing so does not affect the behavior of the program.
  static struct option long_options[] = {
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"segfault", no_argument, &seg_flag, 1},
    {"catch", no_argument, 0, 'c'},
    {0, 0, 0, 0}
  };

  // for a given long option/argument, hold the value for the short version of it
  int opt;

  while(1) {

    int opt_index = 0;

    // use getopt_long to get the short version of the argument given
    // also, break out of while-loop if we reached the end of the arguments
    if ((opt = getopt_long(argc, argv, "i:o:c:", long_options, &opt_index)) == -1)
      break;

    // input and output file descriptors
    int ifd, ofd;
    ifd = ofd = 0;

    // do given actions depending on the argument read
    switch (opt) {
    case 0:
      if (long_options[opt_index].flag != 0)
	break;
      break;
    case 'i':
	if ((ifd = open(optarg, O_RDONLY)) >= 0) {
	  close(0);
	  if(dup(ifd) < 0) {
	    perror(strerror(ENOENT));
	    exit(-1);
	  }
	}
	else {
	  fprintf(stderr, "--input: unable to open file %s: ", optarg);
	  perror("");
	  exit(2);
	}
      break;
    case 'o':
	ofd = creat(optarg, 0666);
	if (ofd >= 0) {
	  close(1);
	  if(dup(ofd) < 0) {
	    perror("--output: unable to redirect to output");
	    exit(-1);
	  }
	}
	else {
	  fprintf(stderr, "--output: unable to create file %s: ", optarg);
	  perror("");
	  exit(3);
	}
      break;
    case 'c':
      signal(SIGSEGV, &handler);
      break;
    case '?':
      fprintf(stderr, "Usage: --input=input_file --output=output_file");
      exit(1);
      break;
    default:
      exit(1);
      break;
    }
  }
  if(seg_flag)
    seg_fault();

  ///////////////////////////////////////////
  /////// READ FROM fd0, WRITE TO fd1 ///////
  ///////////////////////////////////////////

  readin_writeout();

  exit(0);

}
