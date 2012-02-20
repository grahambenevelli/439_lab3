#include <stdio.h>
#include <stdlib.h>

STFQueue::STFQueue() {
	max_ID = 10;
	t_array = (thread_list*) malloc(sizeof(thread_list) * max_ID);
	size = 0;
	currentTime = 0;
	currentThreads = 0;

	smutex_init(&Lock);	//init lock
	scond_init(&fullQueue);	//init Condition variable
}

STFQueue::~STFQueue() {
	// get rid of sync variables
	smutex_destroy(&Lock);
	scond_destroy(&fullQueue);
	// get rid of nodes
}


bool STFQueue::enqueue(int flowID, float weight, int lenToSend) {
	smutex_lock(&Lock);
	
	// resize array if needed	

	//check bounds
	while(size == MAX_SIZE) {
		scond_wait(&fullQueue, &Lock);
	}

	//create the new node
	struct thread_list *temp = (thread_list*) malloc(sizeof(thread_list));

	long long prevFinish = 0;
	struct thread_list current = t_array[flowID];
	// find last node in flowID list
	while (current.next == NULL) current = current->next;
	// current is last node
	current.next = temp;
	prevFinish = current.finishTime;

	//calculate starttime
	//calculate finishtime

	smutex_unlock(&Lock);

	return true;
}

int STFQueue::dequeue() {
	// find lowest start time
	// save that node
	// set that slot to next node
	// delete that node
	// decrement currentThreads
	// signal next enqueue
	// return flowID
	return -1;
}








void STFQueue::unit() {
	int test = 1;
	bool allPassed = true;
	bool result;

	// constructor Tests 1
	result = max_ID == 10;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// constructor Tests 2
	result = size == 0;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// constructor Tests 3
	result = currentTime == 0;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// constructor Tests 4
	result = currentThreads == 0;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	if(allPassed)printf("All %d test passed in STFQueue\n", test);
}

