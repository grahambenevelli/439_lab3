#ifndef _MAX_NW_SCHEDULER_H_
#define _MAX_NW_SCHEDULER_H_
#include <stdlib.h>
#include <signal.h>
#include "NWScheduler.h"
#include "sthread.h"
#include "AlarmThread.h"

class MaxNWScheduler:public NWScheduler{
 public:
  	MaxNWScheduler(long bytesPerSec);
	~MaxNWScheduler();
 	void waitMyTurn(int flowId, float weight, int lenToSend);
 	long long signalNextDeadline(long long deadlineMS);
	void unitTest();
 private:
	bool canSafelySend();
	long long calRunOff(long tran, long max, long long time);

 	long maxBytes;
	long BytesToSend;
	int runOff;

	//synchronization variables
	smutex_t Lock;
	scond_t SafeSend;
};
#endif 
