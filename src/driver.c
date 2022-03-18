#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

#include "libthreadpool/threadpool.h"

#define MAX_THREADS 4
#define MAX_TASKS 100

void sumOfNumbers(void** args) {

    int* num = (int*) args;
    int total = 0;
    for (int i=0; i<*num; i++) {
        total += i;
    }
    sleep(2);
    printf("Sum Of all the numbers upto %d: %d\n", *num, total);
    *num = total;
}

void productOfNumbers(void** args) {
    int* num = (int*) args;
    int total = 1;
    for (int i=1; i<*num; i++) {
        total *= i;
    }
    printf("Product Of all the numbers upto %d: %d\n", *num, total);
    *num = total;
}

int main() {
    threadpool_t* threadpool = initializeThreadpool(MAX_THREADS, MAX_TASKS);
    int sumNumber = 100;
    int prodNumber = 10;

    countdownlatch_t* latch = initializeCountDownLatch(2);

    work_t* work1 = createWork(&sumOfNumbers, (void*) &sumNumber, latch);
    work_t* work2 = createWork(&productOfNumbers, (void*) &prodNumber, latch);

    pushTaskToQueue(threadpool, work1);
    pushTaskToQueue(threadpool, work2);

    latchAwait(latch);
    printf("Awaiting Successful\n");
    printf("Value of sumNumber after awaiting: %d\n", sumNumber);
    printf("Value of prodNumber after awaiting: %d\n", prodNumber);
    free(work1);
    free(work2);
    work1=NULL;
    work2=NULL;
    latch->number = 2;

    sumNumber=5;
    prodNumber=9;

    work1 = createWork(&sumOfNumbers, (void*) &sumNumber, latch);
    work2 = createWork(&productOfNumbers, (void*) &prodNumber, latch);

    pushTaskToQueue(threadpool, work1);
    pushTaskToQueue(threadpool, work2);

    latchAwait(latch);

    return 0;
}
