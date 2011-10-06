#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thread_pool.h"

/* Constants */
#define MAX_THREADS 200
#define MIN_THREADS 5

/* Macros */
#define DEBUGP(x, y...) \
		printf("%s@%d:%s_LOG_" x "\n", __FILE__, \
		__LINE__, __func__, ##y)

/* Structures */
typedef void (*dispatch_fn)(void *);

typedef struct task {
	void (*routine) (void *);
	void *arg;
	struct task* next;
} task;

typedef struct thread_pool { 
	int num_threads;
	int queue_size; //Should this be tracked here?
	int shutdown;
	int dont_accept;
	pthread_t *threads; /* pointer to threads */
	task *queue_head;
	task *queue_tail;
	pthread_mutex_t queue_lock;
	pthread_cond_t queue_not_empty;
	pthread_cond_t queue_empty;
} thread_pool;

/* Global Variables */

/* Function Declarations */

void perror_exit(const char *str);
void *do_work(thread_pool *pool);
thread_pool *create_thread_pool(int num_threads_in_pool);
void dispatch(thread_pool *pool, void (*func)(void *), void *arg);
int delete_thread_pool(thread_pool *pool);

/* Functions */
void
perror_exit(const char *str)
{
	perror(str);
	exit(-1);
}

void *
do_work(thread_pool *pool)
{
	task *current;
	while (1) {
		pthread_mutex_lock(&(pool->queue_lock));
		while (0 == pool->queue_size) {
			pthread_mutex_unlock(&(pool->queue_lock));
			if (pool->shutdown) 
				pthread_exit(NULL);//hmm perror_exit for my implementation?
			pthread_cond_wait(&(pool->queue_not_empty),&(pool->queue_lock));
			if (pool->shutdown)
				pthread_exit(NULL); //unlock again?	
		}
		current = pool->queue_head;
		
		pool->queue_size--;
		if (0 == pool->queue_size) {
			pool->queue_head = NULL;
			pool->queue_tail = NULL;
		} else pool->queue_head = current->next;
		if ((0 == pool->queue_size) && !pool->shutdown) 
			pthread_cond_signal(&(pool->queue_empty)); //signal...
		pthread_mutex_unlock(&(pool->queue_lock));
		(current->routine)(current->arg);
		free(current);
	}
	DEBUGP("YAY");
}

void
dispatch(thread_pool *pool, void (*func)(void *), void *arg)
{ 
	task *current;
	int temp = pool->queue_size;
	current = (task *) malloc(sizeof(task));
	if(NULL == current)
		perror_exit("Out of memory creating a task\n");
	current->routine = *func;
	current->arg = arg;
	current->next = NULL;
	pthread_mutex_lock(&(pool->queue_lock));
	if (pool->dont_accept) {
		free(current);
		return;
	}
	if (0 == pool->queue_size) {
		pool->queue_head = current;
		pool->queue_tail = current;
		pthread_cond_signal(&(pool->queue_not_empty));
	} else {
		pool->queue_tail->next = current;
		pool->queue_tail = current;
	}
	pool->queue_size++;
	pthread_mutex_unlock(&(pool->queue_lock));
}

thread_pool *
create_thread_pool(int num_threads_in_pool)
{	
	if ((0 >= num_threads_in_pool) || (MAX_THREADS < num_threads_in_pool)) 
		perror_exit("Threads not equivalent");
	thread_pool *pool;
	pool = (thread_pool *) malloc(sizeof(thread_pool));
	if (NULL == pool)
		perror_exit("Out of memory creating a new threadpool\n");
	
	pool->threads = (pthread_t *) malloc(sizeof(pthread_t) * num_threads_in_pool);
	if(!pool->threads)
		perror_exit("Out of memory creating a new threadpool\n");
	pool->num_threads = num_threads_in_pool;
	pool->queue_size = 0;
	pool->queue_head = NULL;
	pool->queue_tail = NULL;
	pool->shutdown = 0;
	pool->dont_accept = 0;
	
	return pool;
}

int
delete_thread_pool(thread_pool *pool)
{	
	free(pool->threads);
	pthread_mutex_destroy(&(pool->queue_lock));
	pthread_cond_destroy(&(pool->queue_empty));
	pthread_cond_destroy(&(pool->queue_not_empty));
	return 0;
}

void 
print_test(void *arg) 
{
	char *temp = (char *)arg;
	printf("%s\n", temp);
}

int
main(void) 
{
	/* Test Create */
	thread_pool *pool;
	int i;
	pool = create_thread_pool(10);
	printf("Pool initialized\n");
	/* Test dispatch */
	void (*func)(void *) = NULL;
	func = print_test;
	void *arg;
	arg = "Hello World";
	
	dispatch(pool, func, arg);
	
	/* Test do_work */
	printf("%i\n",pool->queue_size);
	do_work(pool);	
	printf("%i\n",pool->queue_size);
	
	/* Test Delete */
	/*
	if(!(0 > delete_thread_pool(&pool))) 
		printf("Pool deleted\n");
	*/
	printf("Thread pool test success\n");
}
