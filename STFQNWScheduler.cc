#include <assert.h>
#include <stdio.h>
#include "STFQNWScheduler.h"
#include "STFQueue.cc"
#include "util.h"



STFQNWScheduler::STFQNWScheduler(long bytesPerSec)
{
   maxBytes = bytesPerSec;
	assert( maxBytes == bytesPerSec);
	assert( maxBytes > 0);
	//que = new STFQueue();
	//que = STFQueue();

	smutex_init(&lock); //init lock
	scond_init(&safeSend); //init Condition variable
	scond_init(&deadline);
	scond_init(&queueFull);

	runOff = 0;
	bytesToSend = -1;
	nextId = -1;
}

STFQNWScheduler::~STFQNWScheduler() {
	smutex_destroy(&lock);
	scond_destroy(&safeSend);
	scond_destroy(&deadline);
	scond_destroy(&queueFull);
	//free(que);
}





void
STFQNWScheduler::waitMyTurn(int flowId, float weight, int lenToSend)
{
	smutex_lock(&lock);
	
	// add to queue, wait if full
	while(que.queueFull()) {
		scond_wait(&queueFull, &lock);
	}
	que.enqueue(flowId, weight, lenToSend);

	// wait to be signaled
	while((!canSafelySend(flowId)) || (flowId != nextId && nextId != -1)) {// || (flowId == nextId))) {
		scond_wait(&safeSend, &lock);
	}
	
	nextId = -1;	

	//assert(flowId == nextId);
	assert(canSafelySend(flowId));

	// update bytes to send
	bytesToSend = lenToSend*1000;

	// signal that there is a deadline to update
	scond_signal(&deadline, &lock);

	smutex_unlock(&lock);
}




long long
STFQNWScheduler::signalNextDeadline(long long prevDeadline)
{
	smutex_lock(&lock);
	//printf("deadline()\n");
	long long nextDL = prevDeadline;
	if(prevDeadline == 0) { // if this is first pass through, upadte to current time.
		nextDL += nowMS() + 1;
	}

	// if bytes to send == 0 wait
	while(bytesToSend == 0) {
		scond_wait(&deadline, &lock);
	}

	nextDL = calRunOff(bytesToSend, maxBytes, nextDL);
	assert(nextDL >= prevDeadline);
	bytesToSend = 0;			//reset amount of bytes to send, allows other threads to continue.

	// get next id to send
	nextId = que.dequeue();

	// singal next thread
     	scond_broadcast(&safeSend, &lock);
     	scond_broadcast(&queueFull, &lock);
	

	smutex_unlock(&lock);

	return nextDL;
}

bool
STFQNWScheduler::canSafelySend(int flowId) {
	// we can only safely send if the amount of bytes to send is zero
	// otherwise, another thread is currently sending
	return (bytesToSend == 0);
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


void STFQNWScheduler::unit() {
	int test = 1;
	bool result;
	bool allPassed = true;

	//que.printQueue();

	nextId = 0;
	bytesToSend = 0;
	waitMyTurn(0, 1, 10);	

	// Test 1
	result = bytesToSend == 10000;
	if (!result) {printf("Test %d failed in STFQNWScheduler\n", test); allPassed = false;}
	test++;

	long long nextDL = signalNextDeadline(0);
	long long time = nowMS();
	
	// Test 2
	result = nextDL <= time + 1 && nextDL >= time -1;
        if (!result) {printf("Test %d failed in STFQNWScheduler\n", test); allPassed = false;}
	test++;

	// Test 3
	nextId = 0;
	bytesToSend = 0;
	waitMyTurn(0, 1, 10);

	// Test 4
	result = bytesToSend == 10000;
        if (!result) {printf("Test %d failed in STFQNWScheduler\n", test); allPassed = false;}
	test++;

	nextDL = signalNextDeadline(0);
	time = nowMS();

        // test 5                                                                                      
        result = nextDL <= time + 1 && nextDL >= time -1;
	if (!result) {printf("Test %d failed in STFQNWScheduler\n", test); allPassed = false;}
	test++;

        // Test 6
        result = nextId == 0;
        if (!result) {printf("Test %d failed in STFQNWScheduler\n", test); allPassed = false;}
        test++;

        result = nextId == 0;
        if (!result) {printf("Test %d failed in STFQNWScheduler\n", test); allPassed = false;}
        test++;

	nextId = 4;
	bytesToSend = 0;
	waitMyTurn(4, 4, 1024);

	// Test 7
	result = bytesToSend == 1024000;
        if (!result) {printf("Test %d failed in STFQNWScheduler\n", test); allPassed = false;}
	test++;

        nextDL = signalNextDeadline(nextDL);

        // Test 8
        result = nextDL <= time + 5 && nextDL >= time - 5;
	if (!result) {printf("Test %d failed in STFQNWScheduler\n", test); allPassed = false;}
	test++;

	// Test 9
        result = nextId == 4;
        if (!result) {printf("Test %d failed in STFQNWScheduler\n", test); allPassed = false;}
        test++;


	if (allPassed) printf("All %d test passed in STFQNEScheduler\n", test-1); 
}




