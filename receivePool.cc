/**
 * receivePool.cc
 *
 * The current version receives at full speed from whatever
 * senders contact it.
 * In the future, this program should be modified
 * so that each receiver socket can be throttled to
 * receive at a specified rate via ScheduledInputThread.
 */


//#include "receivePool2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include "common.h"
#include "InputStream.h"
#include "ScheduledInputStream.h"
#include "SocketLibrary/socklib.h"
#include "Stats.h"
#include "sthread.h"
#include "util.h"
#include "rec_queue.h"

void acceptor(rec_queue * RQ);
void worker(rec_queue * RQ);

Stats *stats;
int nflows;
static const int NUM_WORKERS = 10;
int socket_listen;

int
main(int argc, char **argv)
{	
	// create queue for our pending input streams
	rec_queue *RQ = new rec_queue();

	nflows = 0;
	int ii;

	// stats for input streams
	stats = new Stats();

	if (argc < 2) {
	fprintf(stderr,
	    "(RECEIVEPOOL): Invoke as  'receiver socknum [totalMax] [weight0 weight1 weight2 ...]'\n");
	exit(1);
	}

  	socket_listen = setup_listen(atoi(argv[1]));

	// create worker threads
	for(ii = 0; ii < NUM_WORKERS; ii++){
		sthread_t *thread = (sthread_t *)malloc(sizeof(sthread_t));
		sthread_create(thread, (void*(*)(void*))worker, (void*)RQ);
	}

	// call accepter, this parent becomes accepter threads
	acceptor(RQ);
	return 1;
}


void 
acceptor(rec_queue * RQ) //enqueue inputstream
  {
	int socket_recv;

	// loop continuously
    	while(1){

	    // listen for an incoming inputstream
	    socket_recv = saccept(socket_listen); 
	    if (socket_recv < 0) {
	      fprintf(stderr, "An error occured in the server; a connection\n");
	      fprintf(stderr, "failed because of ");
	      perror("Saccept failed");
	      exit(1);
	    }

	    // got stream of sending data, create input stream for them
	    nflows++;
	    InputStream *is = new InputStream(socket_recv, nflows, stats);

	    // add to queue
	    RQ->enqueue(is);
	    
  	}
	exit(42);
  }


void 
worker(rec_queue * RQ)//dequeue inputstream
  {
	// set up variables
	int got;
	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	InputStream *is;
	
    while(1){
	// get an input stream from queue
	is = RQ->dequeue();
	
	got = is->read(buffer, BUFFER_SIZE); // read from input stream
	if(got <= 01){
		// if it is done, don't stop thread but free the input stream and continue
		free(is); continue;
	}
	// if the inputstream isn't dead add it back to the queue
	RQ->enqueue(is);
    }
	exit(42);
  }
