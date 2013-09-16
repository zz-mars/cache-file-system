#ifndef _QUEUE_H
#define _QUEUE_H

#define INIT_QUEUE_SZ	128

typedef struct {
	void **m;
	int msize;
	int front;
	int rear;
	int elem_n;
} queue_t;

static inline void *queue_rear(queue_t *q,int idx)
{
	if(idx >= q->elem_n) {
		fprintf(stderr,"out of range!\n");
		return NULL;
	}

	return q->m[(q->rear+q->msize-idx-1)%q->msize];
}

static inline void *queue_front(queue_t *q,int idx)
{
	if(idx >= q->elem_n) {
		fprintf(stderr,"out of range!\n");
		return NULL;
	}

	return q->m[(q->front+idx)%q->msize];
}

static inline void print_queue(queue_t *q)
{
	printf("q->msize #%d\n",q->msize);
	printf("q->front #%d\n",q->front);
	printf("q->rear #%d\n",q->rear);
	printf("q->elem_n #%d\n",q->elem_n);
}

static inline queue_t *init_queue(int msize)
{
	if(msize <= 0) {
		fprintf(stderr,"invalid arg!\n");
		return NULL;
	}

	queue_t *q = (queue_t*)malloc(sizeof(queue_t));

	if(!q) {
		perror("malloc for new queue");
		return NULL;
	}

	q->m = (void**)malloc(sizeof(void*)*msize);
	if(!q->m) {
		perror("malloc for queue memory zone");
		free(q);
		return NULL;
	}

	q->msize = msize;
	q->front = q->rear = q->elem_n = 0;

	return q;
}

static inline void destory_queue(queue_t *q)
{
	if(!q) {
		fprintf(stderr,"invalid arg!\n");
		return;
	}

	if(q->m) {
		free(q->m);
	}

	free(q);
}

static inline int queue_empty(queue_t *q)
{
	return (q->front == q->rear);
}

static inline int en_queue(queue_t *q,void *p)
{
	if((q->rear + 1) % q->msize == q->front) {
		/* queue is full,more space needed */
		void **tmp = q->m;
		q->m = (void**)malloc(sizeof(void*)*(q->msize + INIT_QUEUE_SZ));
		if(!q->m) {
			q->m = tmp;
			perror("malloc for more space of queue");
			return 1;
		}

	//	printf("en_queue : rear %d front %d\n",q->rear,q->front);

		if(q->front < q->rear) {
			memcpy(q->m,tmp+q->front,q->elem_n*sizeof(void*));
		}else if(q->front > q->rear) {
			int llen = q->msize - q->front;
	//		printf("en_queue : elem_n %d llen %d rlen %d\n",q->elem_n,llen,q->rear);
			memcpy(q->m,tmp+q->front,llen*sizeof(void*));
			memcpy(q->m+llen,tmp,(q->rear)*sizeof(void*));
		}else {
			fprintf(stderr,"queue with size of 0!\n");
			return 1;
		}

		free(tmp);

		q->msize += INIT_QUEUE_SZ;
		q->front = 0;
		q->rear = q->elem_n;
	}

	q->m[q->rear] = p;
	q->rear = (q->rear + 1) % q->msize;
	q->elem_n++;

	return 0;
}

static inline void *de_queue(queue_t *q)
{
	if(queue_empty(q)) {
		fprintf(stderr,"empty queue!\n");
		return NULL;
	}

	void *p = q->m[q->front];
	q->front = (q->front + 1) % q->msize;
	q->elem_n--;

	return p;
}

#endif
