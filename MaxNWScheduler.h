#ifndef _MAX_NW_SCHEDULER_H_
#define _MAX_NW_SCHEDULER_H_
#include <stdlib.h>
#include <signal.h>
#include "NWScheduler.h"
#include "sthread.h"
#include "AlarmThread.h"

class MaxNWScheduler:public NWScheduler{
  /*
   * TBD: Fill this in
   */
 public:
  	MaxNWScheduler(long bytesPerSec);
	~MaxNWScheduler();
 	void waitMyTurn(int flowId, float weight, int lenToSend);
 	long long signalNextDeadline(long long deadlineMS);
	bool canSafelySend();
 private:
 	long maxBytes;
	long totalTransmittedBytes;
	long long deadline;
	bool dlmet;
	smutex_t Lock;
	scond_t SafeSend;
	scond_t DeadLineMet;
	int threads;
};
#endif 
