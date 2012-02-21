
#include "rec_queue.h"


	// constructor
	rec_queue::rec_queue() {
		smutex_init(&lock);				//init lock
		scond_init(&queueEmpty);			//init Condition variable
		root = (rec_node*) malloc(sizeof(rec_node));	// allocate a base node
	}
	
	// destructor
	rec_queue::~rec_queue() {
		// out of mutex and scond
		smutex_destroy(&lock);
		scond_destroy(&queueEmpty);

		// delete nodes
		rec_node *next;
		rec_node *current;
		current = root;
		if (current != NULL) {
			while(current->next != NULL) {
				next = current->next;
				delete(current);
				current = next;
			}
			delete(current);
		}
	}

	// printQueue -- not part of project
	// helpful when debugging
	void rec_queue::printQueue() {
		int j =0;
		rec_node *temp = root;
		if(root == NULL) { printf("Nothing\n"); return; }
		while(temp != NULL) {
			printf("(%d) %d",j,(int)temp->is);
			temp = temp->next;
			j++;
		}
		printf("\n");
	}

	// put new input stream on queue
	void rec_queue::enqueue(InputStream *io) {
		smutex_lock(&lock);

		// allocate node for 
		struct rec_node *temp = (rec_node*) malloc(sizeof(rec_node));
		temp->is = io;

		temp->next = NULL;

		// find end of list
		rec_node *current = root;
		while(current->next != NULL) {
			current = current->next;
		}
		
		// set to end of list
		current->next = temp;

		// tell other threads there is something to do
		scond_broadcast(&queueEmpty, &lock);

		smutex_unlock(&lock);
	}

	// get an input stream
	InputStream* rec_queue::dequeue() {
		smutex_lock(&lock);

		// wait if list is empty
		while (root->next == NULL) {
			scond_wait(&queueEmpty, &lock);
		}

		// get next input stream
		InputStream *ret = root->next->is;

		// got node, took out of list and deleted unused node
		struct rec_node *temp = root->next;
		if(root->next->next == NULL)
			root->next = NULL;
		else 
			root->next = root->next->next;
		delete(temp);

		smutex_unlock(&lock);

		// return input stream
		return ret;
	}



void rec_queue::unit() {
	int test = 1;
	bool result;
	bool allPassed = true;

	//construtctor test 1 
	result = root != NULL;
	if (!result) {printf("Test %d failed in rec_queue\n", test); allPassed = false;}
	test++;
	
	Stats *stats = new Stats();
	InputStream *io = new InputStream(5000, 2, stats);

	enqueue(NULL);
	
	//isEmpty test 2
	result = root != NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	//isEmpty test 3
	result = root->next->next == NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	enqueue(io);
	enqueue(io);
	enqueue(io);
	enqueue(io);

	printQueue();

	dequeue();

	printQueue();
	
	delete io;
	delete stats;
	if (allPassed) printf("All %d tests passed in rec_queue\n", test-1);
}


