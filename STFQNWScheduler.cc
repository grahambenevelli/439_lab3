#include <assert.h>
#include "STFQNWScheduler.h"


STFQNWScheduler::STFQNWScheduler(long bytesPerSec)
{
  	maxBytes = bytesPerSec; 
	assert( maxBytes == bytesPerSec);
	assert( maxBytes > 0);

	smutex_init(&lock);	//init lock
	scond_init(&safeSend);	//init Condition variable
	
	runOff = 0;
	bytesToSend = 0;
	nextId = -1;
}

STFQNWScheduler::~STFQNWScheduler() {
	smutex_destroy(&lock);
	scond_destroy(&safeSend);
}

void 
STFQNWScheduler::waitMyTurn(int flowId, float weight, int lenToSend)
{
	smutex_lock(&lock);
	
	while(!canSafelySend() && flowId != nextId) {
		break;
	}


	smutex_unlock(&lock);
}

long long 
STFQNWScheduler::signalNextDeadline(long long prevDeadline)
{
	smutex_lock(&lock);


	smutex_unlock(&lock);
	return 0;
}

bool
STFQNWScheduler::canSafelySend() {
	// we can only safely send if the amount of bytes to send is zero
	// otherwise, another thread is currently sending
	return bytesToSend == 0;
}


long long
STFQNWScheduler::calRunOff(long tran, long max, long long time) {
	while (tran >= max) {   // essentially time += (tran / max); but without the rounding
		time++;
		tran -= max;
	}

	runOff += tran;  //add the run off remaining from tran

	if (runOff >= max) {  //whenever enough runoff is accumulated, add 2 ms to our deadline
		time += 2;
		runOff -= max;  // keep track of remaining runoff
	}

	return time;
}

STFQueue::STFQueue() {
	root = NULL;
	size = 0;
	maxsize = 10;
	lastFinishTag = 0;
	
}

STFQueue::~STFQueue() {
}

bool STFQueue::enqueue(int flowID, float weight, int lenToSend, long long startTime) {
	//check bounds
	if(size == maxsize) return false;

	//calculate priority
	
	
	//create the new node
	struct thread_list * temp = root;
	
	while(temp != NULL) {
		temp = temp->next;
	}
	if(size == 0) {
	  //temp = malloc(sizeof(thread_list));
	}
	return true;
}

int STFQueue::dequeue() {
	return -1;
}
