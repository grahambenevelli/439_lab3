/**
 * receivePool.cc
 *
 * The current version receives at full speed from whatever
 * senders contact it.
 * In the future, this program should be modified
 * so that each receiver socket can be throttled to
 * receive at a specified rate via ScheduledInputThread.
 */

#ifndef _REC_QUEUE_H_
#define _REC_QUEUE_H_

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

class receivePool2 {

static const int NUM_WORKERS = 10;
private:
	int socket_listen;
	int nflows;
	smutex_t lock;
	scond_t fullQueue;
	Stats *stats;

public:
	receivePool2(int argc, char **argv);
	~receivePool2();
	void worker(int * n);
	//void acceptor(int * n);
	rec_queue * RQ;
};

#endif 
