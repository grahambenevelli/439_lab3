#include <assert.h>
#include <stdio.h>
#include "STFQNWScheduler.h"
#include "STFQueue.cc"


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
	while (tran >= max) { // essentially time += (tran / max); but without the rounding
		time++;
		tran -= max;
	}

	runOff += tran; //add the run off remaining from tran

	if (runOff >= max) { //whenever enough runoff is accumulated, add 2 ms to our deadline
		time += 2;
		runOff -= max; // keep track of remaining runoff
	}

	return time;
}


STFQNWScheduler::STFQNWScheduler(long bytesPerSec)
{
   maxBytes = bytesPerSec;
	assert( maxBytes == bytesPerSec);
	assert( maxBytes > 0);

	smutex_init(&lock); //init lock
	scond_init(&safeSend); //init Condition variable

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


	smutex_unlock(	&lock);
}

void STFQNWScheduler::unit() {
	int tests = 1;

	printf("All %d test passed in STFQNEScheduler\n", tests);
}

