#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
    // If the queue is full, return an error or handle it as per your requirements
    if (q->size >= MAX_QUEUE_SIZE) {
        return;
    }

    // Find the correct position to insert the process based on its priority
    int i;
    for (i = 0; i < q->size; i++) {
        if (q->proc[i]->priority > proc->priority) {
            break;
        }
    }

    // Shift the elements to make space for the new process
    for (int j = q->size; j > i; j--) {
        q->proc[j] = q->proc[j - 1];
    }

    // Insert the new process
    q->proc[i] = proc;
    q->size++;
}

struct pcb_t * dequeue(struct queue_t * q){
    // If the queue is empty, return NULL or handle it as per your requirements
    if (empty(q)) {
        return NULL;
    }

    // Remove the process with the highest priority
    struct pcb_t * proc = q->proc[0];
    for (int i = 1; i < q->size; i++) {
        q->proc[i - 1] = q->proc[i];
    }
    q->size--;

    return proc;
}

