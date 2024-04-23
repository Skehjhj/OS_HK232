
#include "queue.h"
#include "sched.h"
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
static struct queue_t ready_queue;
static struct queue_t run_queue;
static pthread_mutex_t queue_lock;

#ifdef MLQ_SCHED
static struct queue_t mlq_ready_queue[MAX_PRIO];
#endif

int queue_empty(void) {
#ifdef MLQ_SCHED
	unsigned long prio;
	for (prio = 0; prio < MAX_PRIO; prio++)
		if(!empty(&mlq_ready_queue[prio])) 
			return -1;
#endif
	return (empty(&ready_queue) && empty(&run_queue));
}

void init_scheduler(void) {
#ifdef MLQ_SCHED
	int i ;

	for (i = 0; i < MAX_PRIO; i ++){
		mlq_ready_queue[i].size = 0;
		mlq_ready_queue[i].slot = MAX_PRIO - i;
	}
#endif
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}

#ifdef MLQ_SCHED
/* 
 *  Stateful design for routine calling
 *  based on the priority and our MLQ policy
 *  We implement stateful here using transition technique
 *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
 */
struct pcb_t * get_mlq_proc(void) {
	static int curr_prio = 0;
	struct pcb_t * proc = NULL;
	/*TODO: get a process from PRIORITY [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */


	
	//usleep(10);
	//Kiểm tra ready queue hiện tại có còn slot không
    if (mlq_ready_queue[curr_prio].slot > 0) {
		// Nếu ready queue hiện tại không rỗng
		if(!empty(&mlq_ready_queue[curr_prio])){ 
			pthread_mutex_lock(&queue_lock);
        	proc = dequeue(&mlq_ready_queue[curr_prio]);
			mlq_ready_queue[curr_prio].slot--;
			pthread_mutex_unlock(&queue_lock);
		}
		// Nếu ready queue hiện tại rỗng
		else{
			//Tìm ready queue tiếp theo không rỗng và vẫn còn slot
			pthread_mutex_lock(&queue_lock);
			int temp = (curr_prio + 1) % MAX_PRIO;
			while (temp != curr_prio) {
				if (!empty(&mlq_ready_queue[temp]) && mlq_ready_queue[temp].slot > 0) {
					curr_prio = temp; // Chuyển sang queue khác
					proc = dequeue(&mlq_ready_queue[curr_prio]);
					mlq_ready_queue[temp].slot--;
					break;
				} else {
					temp = (temp + 1) % MAX_PRIO;
				}
			}
			pthread_mutex_unlock(&queue_lock);
		}
    } else {
        // ready_queue hiện tại đã sử dụng hết slot, reset giá trị và chuyển sang queue tiếp theo
        mlq_ready_queue[curr_prio].slot = MAX_PRIO - curr_prio;

        //Tìm ready queue tiếp theo không rỗng và vẫn còn slot
		pthread_mutex_lock(&queue_lock);
        int temp = (curr_prio + 1) % MAX_PRIO;
        while (temp != curr_prio) {
            if (!empty(&mlq_ready_queue[temp]) && mlq_ready_queue[temp].slot > 0) {
                curr_prio = temp; // Chuyển sang queue khác
                proc = dequeue(&mlq_ready_queue[curr_prio]);
				mlq_ready_queue[curr_prio].slot--;
                break;
            } else {
                temp = (temp + 1) % MAX_PRIO;
            }
        }
		pthread_mutex_unlock(&queue_lock);
    }
	return proc;	
}

void put_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);	
}

struct pcb_t * get_proc(void) {
	return get_mlq_proc();
}

void put_proc(struct pcb_t * proc) {
	return put_mlq_proc(proc);
}

void add_proc(struct pcb_t * proc) {
	return add_mlq_proc(proc);
}
#else
struct pcb_t * get_proc(void) {
	struct pcb_t * proc = NULL;
	/*TODO: get a process from [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */
	return proc;
}

void put_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&run_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&ready_queue, proc);
	pthread_mutex_unlock(&queue_lock);	
}
#endif


