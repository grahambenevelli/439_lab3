#include <stdio.h>
#include <stdlib.h>

#define MAX(x,y) (x) > (y) ? (x) : (y)
#define MIN(x,y) (x) < (y) ? (x) : (y)

STFQueue::STFQueue() {
	max_ID = 10;
	t_array = (thread_list**) malloc(sizeof(int) * max_ID);
	int i;
	for (i = 0; i < max_ID; i++) {
		t_array[i] = (thread_list*) malloc(sizeof(thread_list));
	}
	size = 0;
	currentTime = 0;

	//smutex_init(&Lock);	//init lock
	//scond_init(&fullQueue);	//init Condition variable
}

STFQueue::~STFQueue() {
	// get rid of sync variables
	//smutex_destroy(&Lock);
	//scond_destroy(&fullQueue);
	// get rid of nodes
	deleteNodes();
	delete(t_array);
}

void
STFQueue::deleteNodes() {
	int i;
	struct thread_list * temp;
	struct thread_list * delthread;
	for (i = 0; i < max_ID; i++) {
		if(t_array[i] == NULL) continue;
		delthread = t_array[i];
		while(delthread->next != NULL) {
			temp = delthread->next;
			delthread->next = NULL;
			delete(delthread);
			delthread = temp;
			size--;
		}
		delete(delthread);
		t_array[i] = NULL;
	}
	assert(size == 0);
}

void
STFQueue::printQueue() {
	int i = 0;
	int j = 0;
	struct thread_list * temp;
	for(; i < max_ID; i++) {
		if(t_array[i] == NULL) continue;
		temp = t_array[i];
		printf("Thread %d: ", i);
		while(temp->next != NULL) {
			j++;
			temp = temp->next;
			printf("(%d node): %lld\t", j, temp->finishTime);
		}
		printf("\n");
		j = 0;
	}
}


bool
STFQueue::enqueue(int flowID, float weight, int lenToSend) {

	
	// resize array if needed
	if (flowID > max_ID) resizeArray(flowID);	

	//check bounds
	/*while(size == MAX_SIZE) {
		scond_wait(&fullQueue, &Lock);
	}*/
	if(size == MAX_SIZE) return -1;
	assert(size < MAX_SIZE);
	size++;

	//create the new node
	struct thread_list *temp = (thread_list*) malloc(sizeof(struct thread_list));

	struct thread_list *current = t_array[flowID];

	// find last node in flowID list
	while (current->next != NULL) current = current->next;
	assert(current->next == NULL);

	//calculate starttime
	temp->startTime = MAX(current->finishTime, currentTime); // ******************check this

	//calculate finishtime
	temp->finishTime = temp->startTime + (lenToSend/weight + 0.5); // 0.5 to round up to nearest number
	temp->next = NULL;	

	// current is last node
	current->next = temp;

	//update base finishTime
	t_array[flowID]->finishTime = temp->finishTime;

	return true;
}

void
STFQueue::resizeArray(int id) {

}

int STFQueue::dequeue() {

	// find lowest start time
	int i;
	if (size == 0) {
		smutex_unlock(&Lock);
		return -1;
	}

	int min = 0;
	for (i = 1; i < max_ID; i++) {
		if (t_array[min]->next == NULL) {min = i; continue;}
		if (t_array[i]->next != NULL) {
			if (t_array[min]->next->startTime > t_array[i]->next->startTime)
				min = i;
		}
	}
	assert(t_array[min]->next != NULL);

	// save that node
	struct thread_list *temp = t_array[min]->next;

	// set that slot to next node
	t_array[min]->next = t_array[min]->next->next;

	// decrement size
	size--;

	// update currentTime
	currentTime = temp->startTime;	

	// delete that node
	delete(temp);

	// signal next enqueue
	//scond_signal(&fullQueue, &Lock);



	// return flowID
	return min;
}

bool 
STFQueue::queueFull() {
	return size == MAX_SIZE;
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
	result = size == 0;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	enqueue(0, 1, 10);

	// enqueue Tests 5
	result = t_array[0]->next != NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 6
	result = t_array[0]->next->startTime == 0;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 7
	result = t_array[0]->next->finishTime == 10;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 8
	result = t_array[0]->next->next == NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 9
	result = size == 1;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 10
	result = t_array[0]->finishTime == 10;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	enqueue(1, 2, 10);

	// enqueue Tests 11
	result = t_array[1]->next != NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 12
	result = t_array[1]->next->startTime == 0;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 13
	result = t_array[1]->next->finishTime == 5;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 14
	result = t_array[1]->next->next == NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 15
	result = size == 2;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 16
	result = t_array[1]->finishTime == 5;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	enqueue(0, 1, 20);

	// enqueue Tests 17
	result = t_array[0]->next->next != NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 18
	result = t_array[0]->next->next->startTime == 10;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 19
	result = t_array[0]->next->next->finishTime == 30;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 20
	result = t_array[0]->next->next->next == NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 21
	result = size == 3;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// enqueue Tests 22
	result = t_array[0]->finishTime == 30;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	int de = dequeue();

	// dequeue Tests 23
	result = t_array[0]->next->next == NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// dequeue Tests 24
	result = de == 0;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// dequeue Tests 25
	result = t_array[0]->next->startTime == 10;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// dequeue Tests 26
	result = t_array[0]->next->finishTime == 30;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// dequeue Tests 27
	result = t_array[0]->finishTime == 30;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// dequeue Tests 28
	result = currentTime == 0;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	enqueue(4, 3, 10);
	enqueue(5, 3, 10);
	enqueue(1, 3, 10);

	de = dequeue();

	// dequeue Tests 29
	result = de == 1;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	de = dequeue();

	// dequeue Tests 30
	result = de == 4;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	de = dequeue();

	// dequeue Tests 31
	result = de == 5;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	de = dequeue();

	// dequeue Tests 32
	result = de == 1;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	de = dequeue();

	// dequeue Tests 33
	result = de == 0;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	enqueue(4, 3, 10);
	enqueue(5, 3, 10);
	enqueue(1, 3, 10);

	deleteNodes();

	// delete Tests 34
	result = t_array[0] == NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	// delete Tests 35
	result = t_array[1] == NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	if(allPassed)printf("All %d test passed in STFQueue\n", test-1);
}



