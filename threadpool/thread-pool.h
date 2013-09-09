#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "global.h"

#define BUSY_THRESHOLD 0.5

#define MANAGE_INTERVAL 30	

/* define task function type */
typedef void task_func(void * arg);

typedef struct {
	pthread_t thread_id;
	int is_busy;
	pthread_cond_t thread_cond;	
	pthread_mutex_t	thread_lock;
	task_func * func;
	void * arg;
}thread_info_t;

typedef struct thread_pool_t {
	int (*init)(struct thread_pool_t *this);
	void (*close)(struct thread_pool_t *this);
	void (*process_job)(struct thread_pool_t *this, task_func *func, void *arg);
	int  (*get_thread_by_id)(struct thread_pool_t *this, pthread_t id);
	int (*add_thread)(struct thread_pool_t *this);
	int (*delete_thread)(struct thread_pool_t *this);
	int (*get_tp_status)(struct thread_pool_t *this);
	
	int min_th_num;
	volatile int cur_th_num;
	int max_th_num;
	pthread_mutex_t tp_lock;
	pthread_t manage_thread_id;
	thread_info_t *thread_info;
}thread_pool_t;

thread_pool_t *creat_thread_pool(int min_num, int max_num);

static inline void prt_tpinfo(thread_pool_t *this)
{
	printf("--------------------------------\n");
	printf("tp_min # %d\n",this->min_th_num);
	printf("tp_max # %d\n",this->max_th_num);
	printf("tp_cur # %d\n",this->cur_th_num);
	int i;
	for(i=0;i<this->cur_th_num;i++) {
		if(this->thread_info[i].is_busy) {
			printf("thread[%d] #BUSY\n",i);
		}else {
			printf("thread[%d] #IDLE\n",i);
		}
	}
	printf("--------------------------------\n");
}
#endif
