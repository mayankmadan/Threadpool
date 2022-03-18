#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "threadpool.h"

void* leftShiftArray(void** arr, unsigned int length) {
    void* firstElement = arr[0];
    for (int i=0; i<length-1; i++) {
        arr[i] = arr[i+1];
    }
    return firstElement;

}

/*
work_t* createWork(void* executingFunction, int nargs, ...) {
    work_t* work = (work_t*) malloc(sizeof(work_t));
    work->executingFunction = executingFunction;

    void** args = (void**) malloc(sizeof(void*)*nargs);

    va_list aptr;
    va_start(aptr, nargs);

    for (int i=0; i<nargs; i++) {
        args[i] = (void*) va_arg(aptr, void*);
    }

    work->args = args;

    return work;

}
*/
work_t* createWork(void* executingFunction, void* args, countdownlatch_t* latch) {
    work_t* work = (work_t*) malloc(sizeof(work_t));
    work->executingFunction = executingFunction;
    work->args = args;
    work->latch = latch;

    return work;

}

void* startThread(void* args) {
    threadpool_t* threadpool = (threadpool_t*) args;
    while(1) {
        pthread_mutex_lock(&(threadpool->taskQueue_mutex));
        while (threadpool->taskCount == 0) {
            pthread_cond_wait(&(threadpool->taskQueue_cond), &(threadpool->taskQueue_mutex));
        }

        work_t* work = (work_t*) leftShiftArray((void**) threadpool->taskQueue, threadpool->taskCount);
        threadpool->taskCount -= 1;
        pthread_mutex_unlock(&(threadpool->taskQueue_mutex));

        work->executingFunction(work->args);
        pthread_mutex_lock(&(work->latch->latch_mutex));
        work->latch->number-=1;
        pthread_mutex_unlock(&(work->latch->latch_mutex));
        pthread_cond_signal(&(work->latch->latch_cond));

    }

    return NULL;
}

void pushTaskToQueue(threadpool_t* threadpool, work_t* work) {
    pthread_mutex_lock(&(threadpool->taskQueue_mutex));
    threadpool->taskQueue[threadpool->taskCount] = work;
    threadpool->taskCount += 1;
    pthread_mutex_unlock(&(threadpool->taskQueue_mutex));
    pthread_cond_signal(&(threadpool->taskQueue_cond));
}

threadpool_t* initializeThreadpool(unsigned int threads, unsigned int maxTasks)
{
    threadpool_t *threadpool = (threadpool_t*) malloc(sizeof(threadpool_t));
    threadpool->threadCount = threads;
    threadpool->maxTasks = maxTasks;
    threadpool->taskCount = 0;

    // Allocate the work queue
    threadpool->taskQueue = (work_t**) malloc(sizeof(work_t*) * maxTasks);

    // Allocate workers
    threadpool->workers = (pthread_t*) malloc(sizeof(pthread_t) * threads);

    // Initialize mutexes and conditionals
    pthread_mutex_init(&(threadpool->taskQueue_mutex), NULL);
    pthread_cond_init(&(threadpool->taskQueue_cond), NULL);


    for (int i=0; i<threads; i++) {
        pthread_create(&(threadpool->workers[i]), NULL, startThread, threadpool);
    }

    return threadpool;
}

countdownlatch_t* initializeCountDownLatch(unsigned int number) {
    countdownlatch_t* latch = (countdownlatch_t*) malloc(sizeof(countdownlatch_t));
    latch->number = number;
    pthread_mutex_init(&(latch->latch_mutex), NULL);
    pthread_cond_init(&(latch->latch_cond), NULL);

    return latch;
}

void latchAwait(countdownlatch_t* latch) {
    pthread_mutex_lock(&(latch->latch_mutex));
    while (latch->number != 0) {
        pthread_cond_wait(&(latch->latch_cond), &(latch->latch_mutex));
    }
    pthread_mutex_unlock(&(latch->latch_mutex));
}
