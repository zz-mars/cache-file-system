#include "thread-pool.h"

static int tp_init(thread_pool_t *this);
static void tp_close(thread_pool_t *this);
static void tp_process_job(thread_pool_t *this, task_func *func,void *arg);
static int tp_get_thread_by_id(thread_pool_t *this, pthread_t id);
static int tp_add_thread(thread_pool_t *this);
static int tp_delete_thread(thread_pool_t *this);
static int tp_get_tp_status(thread_pool_t *this);

/* real work thread */
static void *tp_work_thread(void *tp)
{
	thread_pool_t *this = (thread_pool_t*)tp;

	pthread_t curid = pthread_self();

	/* only called when thread is first created */
	int nseq = this->get_thread_by_id(this,curid);

	if(nseq < 0){
		return;
	}

	thread_info_t * cur_thinfo = &this->thread_info[nseq];

	while(1){
		/* wait for the condition */
		pthread_mutex_lock(&cur_thinfo->thread_lock);
		pthread_cond_wait(&cur_thinfo->thread_cond,&cur_thinfo->thread_lock);
		pthread_mutex_unlock(&cur_thinfo->thread_lock);		

		printf("thread # %u with index of %d woken up and processing job now..\n",curid,nseq);

		//	void *arg = cur_thinfo->arg;
		/* for test */
		void *arg = (void*)nseq;

		/* do the job */
		cur_thinfo->func(arg);

		/* set idle */
		pthread_mutex_lock(&cur_thinfo->thread_lock);		
		cur_thinfo->is_busy = 0;
		pthread_mutex_unlock(&cur_thinfo->thread_lock);

		printf("thread # %u with index of %d  - done\n",curid,nseq);
	}

	return;
}

/* manage thread pool to delete idle thread */
static void *tp_manage_thread(void *tp)
{
	thread_pool_t *this = (thread_pool_t*)tp;

	sleep(MANAGE_INTERVAL);

	while(1) {
		printf("manage_thread now checking thread-pool status...\n");

		prt_tpinfo(this);

		if(this->get_tp_status(this) == 0 ){
			/* need to delete some idle threads */
			printf("manage_thread : neeed to rm sone threads..\n");
			pthread_mutex_lock(&this->tp_lock);

			/* delete idle threads as many as possible */
			while(1) {
				if(this->delete_thread(this)) {
					break;
				}
			}

			pthread_mutex_unlock(&this->tp_lock);
		}

		printf("manage_thread now go to sleep..\n");

		sleep(MANAGE_INTERVAL);
	}
}

/* create a thread pool */
thread_pool_t *creat_thread_pool(int min_num, int max_num)
{
	thread_pool_t *this = (thread_pool_t*)malloc(sizeof(thread_pool_t));	

	if(!this) {
		perror("malloc for thead_pool");
		return NULL;
	}

	memset(this, 0, sizeof(thread_pool_t));

	/* init member function ponter */
	this->init = tp_init;
	this->close = tp_close;
	this->process_job = tp_process_job;
	this->get_thread_by_id = tp_get_thread_by_id;
	this->add_thread = tp_add_thread;
	this->delete_thread = tp_delete_thread;
	this->get_tp_status = tp_get_tp_status;

	/* init member */
	this->min_th_num = min_num;
	this->cur_th_num = this->min_th_num;
	this->max_th_num = max_num;
	pthread_mutex_init(&this->tp_lock, NULL);

	if(this->thread_info){
		free(this->thread_info);
	}

	/* malloc for max num of threads allowed in this thread pool */
	this->thread_info = (thread_info_t*)malloc(sizeof(thread_info_t)*this->max_th_num);
	if(!this->thread_info) {
		perror("malloc for thread_info");
		pthread_mutex_destroy(&this->tp_lock);
		free(this);
		this = NULL;
	}

	return this;
}

/* initialize theads currently reside in the thread pool */
int tp_init(thread_pool_t *this)
{
	int i;
	/* set detached state */
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	/* init condition variable and mutex for thread 
	 * start the thread */
	for(i=0;i<this->cur_th_num;i++) {
		thread_info_t *cur_thinfo = &this->thread_info[i];
		pthread_cond_init(&cur_thinfo->thread_cond, NULL);
		pthread_mutex_init(&cur_thinfo->thread_lock, NULL);

		if(pthread_create(&cur_thinfo->thread_id,&attr,tp_work_thread,this)  != 0) {
			perror("creat work thread");
			return 1;
		}
		printf("tp_init: creat work thread %u\n", cur_thinfo->thread_id);
	}

	/* creat manage thread */
	/*
	if(pthread_create(&this->manage_thread_id, NULL, tp_manage_thread, this) != 0){
		perror("creat manage thread");
		return 1;
	}

	printf("tp_init: creat manage thread %u\n", this->manage_thread_id);
	*/

	return 0;

}

/* close thread pool entirely */
void tp_close(thread_pool_t *this)
{
	int i;
	/* kill all the threads currently in the pool */
	for(i=0;i<this->cur_th_num;i++){
		kill(this->thread_info[i].thread_id, SIGKILL);
		/* destroy mutex and cond */
		pthread_mutex_destroy(&this->thread_info[i].thread_lock);
		pthread_cond_destroy(&this->thread_info[i].thread_cond);
		printf("tp_close: kill work thread %u\n", this->thread_info[i].thread_id);
	}

	/* kill manage thread */
	kill(this->manage_thread_id, SIGKILL);
	printf("tp_close: kill manage thread %u\n", this->manage_thread_id);

	/* destroy tp_lock of this pool */
	pthread_mutex_destroy(&this->tp_lock);

	free(this->thread_info);
}

/* when new task comes,call the tp_process_job 
 * tips : This is the only entrance to do a job */
void tp_process_job(thread_pool_t *this, task_func *func,void *arg)
{
	int i;
	thread_info_t *cur_thinfo;

	/* for debug */
	prt_tpinfo(this);

	/* try to find an idle thread */
	for(i=0;i<this->cur_th_num;i++){

		cur_thinfo = &this->thread_info[i];
		/* lock this thread to do some status check and assignment */
		if(pthread_mutex_lock(&cur_thinfo->thread_lock) != 0 && errno == EINVAL){
			/* when lock fail
			 * This may be caused by delete routine,which has destroyed the mutex */
			break;
		}

		if(!cur_thinfo->is_busy) {

			/* set busy and unlock */
			cur_thinfo->is_busy = 1;
			pthread_mutex_unlock(&cur_thinfo->thread_lock);

			/* tell the thread what & how to do */
			cur_thinfo->func = func;
			cur_thinfo->arg = arg;

			printf("tp_process_job : find an idle thread #%u with index %d : now wake it up..\n",cur_thinfo->thread_id,i);
			/* signal */
			pthread_cond_signal(&cur_thinfo->thread_cond);

			return;
		}else { 
			//			printf("tp_process_job : thread #%u with index %d is busy, now checking next one..\n",cur_thinfo->thread_id,i);
			/* this thread is not idle,unlock and loop to the next */
			pthread_mutex_unlock(&cur_thinfo->thread_lock);		
		}
	}

	printf("tp_process_job : no idle thread found,need to start new one..\n");

	/* no idle thread found,so we need to add a new thread */

	pthread_mutex_lock(&this->tp_lock);

	if(this->add_thread(this) == 0){
		cur_thinfo = &this->thread_info[this->cur_th_num-1];

		/* tell the thread what & how to do */
		cur_thinfo->func = func;
		cur_thinfo->arg = arg;

		printf("tp_process_job : new thread #%u with index %d  added,now wake it up..\n",cur_thinfo->thread_id,this->cur_th_num-1);
		pthread_cond_signal(&cur_thinfo->thread_cond);
	}else {
		printf("tp_process_job : add new thread fail!\n");
	}

	pthread_mutex_unlock(&this->tp_lock);

	return;	
}

/* search the array,return the index for a given thread_id */
/* this function is called when threads are first created */
static int tp_get_thread_by_id(thread_pool_t *this, pthread_t id)
{
	int i;
	for(i=0;i<this->cur_th_num;i++){
		if(id == this->thread_info[i].thread_id)
			return i;
	}
	return -1;
}

/* simply add a new thread into the pool
 * no tp_lock is used */
static int tp_add_thread(thread_pool_t *this)
{
	/* no tp_lock is used here,
	 * because when we call tp_add_thread in tp_process_job,
	 * we use tp_lock out side this function */

	thread_info_t *new_thread;

	if(this->max_th_num <= this->cur_th_num) {
		return 1;
	}

	new_thread = &this->thread_info[this->cur_th_num];

	pthread_cond_init(&new_thread->thread_cond, NULL);
	pthread_mutex_init(&new_thread->thread_lock, NULL);

	/* init status is busy */
	new_thread->is_busy = 1;

	this->cur_th_num++;

	/* set detached state */
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	if(pthread_create(&new_thread->thread_id,&attr,tp_work_thread,this) != 0){
		/* when create fail,undo the above oprations and return 0 */
		this->cur_th_num--;
		pthread_cond_destroy(&new_thread->thread_cond);
		pthread_mutex_destroy(&new_thread->thread_lock);
		return 1;
	}
	printf("tp_add_thread: creat work thread %u\n",new_thread->thread_id);

	return 0;
}

/* only delete last idle thread in the pool */
static int tp_delete_thread(thread_pool_t *this)
{
	/* also,no tp_lock is used */
	if(this->cur_th_num <= this->min_th_num) return 1;

	/* last thread to delete */
	thread_info_t * to_del = &this->thread_info[this->cur_th_num-1];

	pthread_mutex_lock(&to_del->thread_lock);

	/* if last thread is busy, do nothing */
	if(to_del->is_busy){
		printf("delete_thread : last block is busy..return 0 ..\n");
		pthread_mutex_unlock(&to_del->thread_lock);
		return 1;
	}
	/* so now,thread is idle 
	 * What if the tp_process_job is also checking this thread for a new job? */
	/* mark it busy,so it is not going to be asked to do any job */
	printf("delete_thread : last block is not busy..set busy before delete it..\n");
	to_del->is_busy = 1;
	pthread_mutex_unlock(&to_del->thread_lock);

	printf("delete_thread : kill thread # %u \n",to_del->thread_id);
	kill(to_del->thread_id, SIGKILL);

	pthread_mutex_destroy(&to_del->thread_lock);
	pthread_cond_destroy(&to_del->thread_cond);

	this->cur_th_num--;

	return 0;
}

/* return 0 on idle; 1 on normal or busy(don't process) */
static int  tp_get_tp_status(thread_pool_t *this)
{
	float busy_num = 0.0;
	int i;

	for(i=0;i<this->cur_th_num;i++){
		if(this->thread_info[i].is_busy){
			busy_num++;
		}
	}

	if(busy_num/(this->cur_th_num) < BUSY_THRESHOLD){
		return 0;
	}else{
		return 1;
	}
}

void some_work(void * arg)
{
	int i;
	int n = (int)arg;
	for(i=0;i<8;i++) {
		printf("thread # %d in processing -- %d\n",n,i);
		sleep(1);
	}
}

//int main()
//{
//	thread_pool_t * tp = creat_thread_pool(4,10);
//	tp->init(tp);
//	prt_tpinfo(tp);
//
//	int i;
//
//	for(i=0;i<6;i++) {
//		tp->process_job(tp,some_work,NULL);
//	}
//
//	while(1) {
//		printf("main just print something to show i'm still alive..but why are some threads stopped?\n");
//		sleep(1);
//	}
//
//	/*
//	sleep(100);
//
//	tp->close(tp);
//	*/
//
//	return 0;
//}
