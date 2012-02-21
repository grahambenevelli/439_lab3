
#include "rec_queue.h"



	rec_queue::rec_queue() {
		smutex_init(&lock);		//init lock
		scond_init(&queueEmpty);	//init Condition variable
		root = (rec_node*) malloc(sizeof(rec_node));
	}

	rec_queue::~rec_queue() {
		smutex_destroy(&lock);
		scond_destroy(&queueEmpty);
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


	void rec_queue::enqueue(InputStream *io) {
		smutex_lock(&lock);

		struct rec_node *temp = (rec_node*) malloc(sizeof(rec_node));
		temp->is = io;

		temp->next = NULL;

		rec_node *current = root;
		while(current->next != NULL) {
			current = current->next;
		}

		current->next = temp;

		scond_broadcast(&queueEmpty, &lock);

		smutex_unlock(&lock);
	}


	InputStream* rec_queue::dequeue() {
		smutex_lock(&lock);

		while (root->next == NULL) {
			scond_wait(&queueEmpty, &lock);
		}

		InputStream *out = root->next->is;

		if(root->next->next == NULL)
			root->next = NULL;
		else 
			root->next = root->next->next;

		smutex_unlock(&lock);
		return out;
	}

	bool rec_queue::isEmpty() {
		smutex_lock(&lock);
		bool out = root->next == NULL;
		smutex_unlock(&lock);
		return out;
	}

void rec_queue::unit() {
	int test = 1;
	bool result;
	bool allPassed = true;

	//construtctor test 1 
	result = root != NULL;
	if (!result) {printf("Test %d failed in rec_queue\n", test); allPassed = false;}
	test++;

	//isEmpty test 2
	result = isEmpty();
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;
	
	Stats *stats = new Stats();
	InputStream *io = new InputStream(5000, 2, stats);

	enqueue(NULL);

	//isEmpty test 3
	result = !isEmpty();
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;
	
	//isEmpty test 4
	result = root != NULL;
	if (!result) {printf("Test %d failed in STFQueue\n", test); allPassed = false;}
	test++;

	//isEmpty test 5
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


