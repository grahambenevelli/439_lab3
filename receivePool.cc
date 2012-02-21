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
	rec_queue *RQ = new rec_queue();
	int ii;
	stats = new Stats();

	if (argc < 2) {
	fprintf(stderr,
	    "(RECEIVEPOOL): Invoke as  'receiver socknum [totalMax] [weight0 weight1 weight2 ...]'\n");
	exit(1);
	}

	nflows = 0;

  	socket_listen = setup_listen(atoi(argv[1]));

	for(ii = 0; ii < NUM_WORKERS; ii++){
		sthread_t *thread = (sthread_t *)malloc(sizeof(sthread_t));
		sthread_create(thread, (void*(*)(void*))worker, (void*)RQ);
	}
	acceptor(RQ);
	return 1;
}


void 
acceptor(rec_queue * RQ) //enqueue inputstream
  {
	int socket_recv;

    	while(1){

	    socket_recv = saccept(socket_listen); 
	    if (socket_recv < 0) {
	      fprintf(stderr, "An error occured in the server; a connection\n");
	      fprintf(stderr, "failed because of ");
	      perror("Saccept failed");
	      exit(1);
	    }
	    nflows++;
	    InputStream *is = new InputStream(socket_recv, nflows, stats);
    	    printf("InpSt add: %d\n", (int) is);

	    RQ->enqueue(is);
	    
  	}
	exit(42);
  }


void 
worker(rec_queue * RQ)//dequeue inputstream
  {
	int got;
	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	InputStream *is;
	
    while(1){
	RQ->printQueue();
	is = RQ->dequeue();
	
	got = is->read(buffer, BUFFER_SIZE);
	if(got <= 01){
		free(is); continue;
	}
	RQ->enqueue(is);
    }
	exit(42);
  }
