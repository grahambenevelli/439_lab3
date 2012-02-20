#ifndef _STFQ_NW_SCHEDULER_H_
#define _STFQ_NW_SCHEDULER_H_
#include "NWScheduler.h"
#include "sthread.h"
#include "STFQueue.h"

class STFQNWScheduler:public NWScheduler{
	smutex_t lock;
	scond_t safeSend;
	scond_t deadline;
	scond_t queueFull;
	long maxBytes;
	long bytesToSend;
	int runOff;
	int nextId; //id of the next thread to run
	STFQueue que;
	bool canSafelySend(int flowId);
	long long calRunOff(long tran, long max, long long time);


	public:
	STFQNWScheduler(long bytesPerSec);
	~STFQNWScheduler();
	void waitMyTurn(int flowId, float weight, int lenToSend);
	long long signalNextDeadline(long long deadlineMS);
	void unit();
};


#endif 
