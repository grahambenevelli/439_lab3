#ifndef _STFQ_NW_SCHEDULER_H_
#define _STFQ_NW_SCHEDULER_H_
#include "NWScheduler.h"
#include "sthread.h"

class STFQueue {
	struct thread_list{
		int flowId;
		long long finishTime;
		long long startTime;
		thread_list * next;
	};
	struct thread_list* root;
	int size;
	int maxsize;
	float lastFinishTag;
	float currentTime;
 public:
	STFQueue();
	~STFQueue();

	bool enqueue(int flowID, float weight, int lenToSend, long long startTime);
	int dequeue();
};

class STFQNWScheduler:public NWScheduler{
	smutex_t lock;
	scond_t	 safeSend;
	long maxBytes;
	long bytesToSend;
	int runOff;
	int nextId; //id of the next thread to run
	STFQueue que;

	bool canSafelySend();
	long long calRunOff(long tran, long max, long long time);

	public:
	STFQNWScheduler(long bytesPerSec);
	~STFQNWScheduler();
	void waitMyTurn(int flowId, float weight, int lenToSend);
	long long signalNextDeadline(long long deadlineMS);
};


#endif 
