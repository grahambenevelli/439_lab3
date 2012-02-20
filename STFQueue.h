#ifndef _STFQUEUE_H_
#define _STFQUEUE_H_

class STFQueue {
	struct thread_list{
		long long finishTime;
		long long startTime;
		thread_list *next;
	};
	struct thread_list **t_array;
	int size;
	static const int MAX_SIZE = 100;	
	int max_ID;	
	float currentTime;

	//synchronization variables
	smutex_t Lock;
	scond_t fullQueue;

	void resizeArray(int id);
	void deleteNodes();
	
	public:
	STFQueue();
	~STFQueue();
	bool enqueue(int flowID, float weight, int lenToSend);
	int dequeue();
	void printQueue();
	void unit();
};

#endif 
