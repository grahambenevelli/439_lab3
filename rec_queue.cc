
#include "rec_queue.h"



	rec_queue::rec_queue() {
		smutex_init(&lock);	//init lock
		scond_init(&queueEmpty);	//init Condition variable
		root = NULL;
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


	void rec_queue::enqueue(InputStream *io) {
		smutex_lock(&lock);

		rec_node *temp = (rec_node*) malloc(sizeof(rec_node));
		if (root == NULL) {
			root = temp;
			return;
		}
		rec_node *current = root;
		while(current->next != NULL) {
			current = current->next;
		}
		//assert(current->next == NULL);
		current->next = temp;

		scond_signal(&queueEmpty, &lock);

		smutex_unlock(&lock);
	}


	InputStream* rec_queue::dequeue() {
		smutex_lock(&lock);

		while (root == NULL) {
			scond_wait(&queueEmpty, &lock);
		}

		InputStream *out = root->is;
		rec_node *temp = root;
		root = root->next;
		delete(temp);

		smutex_unlock(&lock);

		return out;
	}

	bool rec_queue::isEmpty() {
		return root != NULL;
	}


