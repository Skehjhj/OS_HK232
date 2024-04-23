#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t *q)
{
    if (q == NULL)
        return 1;
    return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
    /* TODO: put a new process to queue [q] */
    if (q->size < MAX_QUEUE_SIZE) {
        q->proc[q->size++] = proc;

    }
}

struct pcb_t *dequeue(struct queue_t *q)
{
    // If the queue is empty, return NULL or handle it as per your requirements
    if (empty(q))
    {
        return NULL;
    }

    // Remove the process with the highest priority
    struct pcb_t *proc = q->proc[0];
    for (int i = 1; i < q->size; i++)
    {
        q->proc[i - 1] = q->proc[i];
    }
    q->size--;

    return proc;
}
