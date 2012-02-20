#ifndef _STFQUEUE_H_
#define _STFQUEUE_H_

class STFQueue {
	struct thread_list{
		int flowId;
		long long finishTime;
		long long startTime;
		thread_list *next;
	};
	struct thread_list **t_array;
	int size;
	static const int MAX_SIZE = 10;	
	int max_ID;	
	float currentTime;
	int currentThreads;

	//synchronization variables
	smutex_t Lock;
	scond_t fullQueue;
	
	public:
	STFQueue();
	~STFQueue();
	bool enqueue(int flowID, float weight, int lenToSend);
	int dequeue();
	void unit();
};

#endif 
