#ifndef _REC_QUEUE_H_2
#define _REC_QUEUE_H_2

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

class rec_queue {
	struct rec_node {
		InputStream *is;
		rec_node *next	;
	} *root;

	//synchronization variables
	smutex_t lock;
	scond_t queueEmpty;
	
	public:
	rec_queue();
	~rec_queue();
	void enqueue(InputStream *io);
	InputStream* dequeue();
	bool isEmpty();
};


#endif 
