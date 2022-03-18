#ifndef THREADPOOL_C
#define THREADPOOL_C

#include<pthread.h>

struct countdownlatch_struct {
    unsigned int number;
    pthread_mutex_t latch_mutex;
    pthread_cond_t latch_cond;
};
typedef struct countdownlatch_struct countdownlatch_t;

struct work_struct {
    void (*executingFunction)(void** args);
    void* args;
    countdownlatch_t* latch;

};
typedef struct work_struct work_t;

struct threadpool_struct {
    work_t** taskQueue;
    pthread_t* workers;
    pthread_mutex_t taskQueue_mutex;
    pthread_cond_t taskQueue_cond;
    unsigned int taskCount;
    unsigned int threadCount;
    unsigned int maxTasks;
};

typedef struct threadpool_struct threadpool_t;

threadpool_t* initializeThreadpool(unsigned int threads, unsigned int maxTasks);
void pushTaskToQueue(threadpool_t* threadpool, work_t* work);
//work_t* createWork(void* executingFunction, int nargs, ...);
work_t* createWork(void* executingFunction, void* args, countdownlatch_t* latch);
countdownlatch_t* initializeCountDownLatch(unsigned int number);
void latchAwait(countdownlatch_t* latch);
void joinThreads(threadpool_t *threadpool);

#endif
