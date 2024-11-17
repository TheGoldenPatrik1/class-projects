#include <stdlib.h> 
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include "mytime.h"

// set up chairs queue
struct chairs {
    int chairsOccupied;
    int chairsAvailable;
    struct chair* head;
    struct chair* tail;
};
struct chair {
    int index;
    struct chair* next;
};

// define global variables
struct chairs *chairQ;
int studentsLeft;
int* studentData;
int teacherBusy = 0;
int teacherSeeing = -1;
pthread_mutex_t dataMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t teacherLock;
pthread_cond_t teacherCond;
pthread_mutex_t* locks;
pthread_cond_t* conds;
int leftBound;
int rightBound;

// teacher thread method
void *teacherHandler(void *arg) {
    // set up variables
    int duration;
    int threadID = (int)(intptr_t)pthread_self();

    // repeat until all students gone
    while (studentsLeft > 0) {
        // take data lock
        printf("Teacher <%d> will call mutex_lock <dataMutex>\n", threadID);
        pthread_mutex_lock(&dataMutex);

        // check chair queue
        if (chairQ->chairsOccupied > 0) {
            // students in chairs - handle next one FIFO
            int current = chairQ->head->index;
            teacherBusy = 1;
            printf("Teacher <%d> starts helping next in line: student %d\n", threadID, current);
            if (chairQ->head == chairQ->tail) chairQ->tail = NULL;
            chairQ->head = chairQ->head->next;
            chairQ->chairsOccupied--;
            chairQ->chairsAvailable++;
            studentData[current]++;
            if (studentData[current] == 2) studentsLeft--;
            printf("Teacher <%d> will call mutex_lock <locks[%d]>\n", threadID, current);
            pthread_mutex_lock(&locks[current]);

            // release data lock
            pthread_mutex_unlock(&dataMutex);
            printf("Teacher <%d> call mutex_unlock <dataMutex>\n", threadID);

            // help student (ie sleep thread)
            duration = mytime(leftBound, rightBound);
            printf("Teacher <%d> to sleep %d sec\n", threadID, duration);
            sleep(duration);
            printf("Teacher <%d> wake up\n", threadID);

            // post to student thread
            printf("Teacher <%d> finishes helping next in line: student %d\n", threadID, current);
            teacherBusy = 0;
            pthread_cond_signal(&conds[current]);
            printf("Teacher <%d> call cond_signal <conds[%d]>\n", threadID, current);

            // unlock mutex
            pthread_mutex_unlock(&locks[current]);
            printf("Teacher <%d> call mutex_unlock <locks[%d]>\n", threadID, current);
        } else {
            // no students in chairs - work on other things
            printf("Teacher <%d> works on own work\n", threadID);
            pthread_mutex_unlock(&dataMutex);
            printf("Teacher <%d> call mutex_unlock <dataMutex>\n", threadID);
            
            // wait on student to arrive
            printf("Teacher <%d> will call mutex_lock <teacherLock>\n", threadID);
            pthread_mutex_lock(&teacherLock);
            printf("Teacher <%d> will call cond_wait <teacherCond>\n", threadID);
            while (teacherSeeing == -1) pthread_cond_wait(&teacherCond, &teacherLock);
            pthread_mutex_unlock(&teacherLock);
            printf("Teacher <%d> call mutex_unlock <teacherLock>\n", threadID);

            // begin helping student
            printf("Teacher <%d> will call mutex_lock <locks[%d]>\n", threadID, teacherSeeing);
            pthread_mutex_lock(&locks[teacherSeeing]);
            teacherBusy = 1;
            printf("Teacher <%d> starts helping: student %d\n", threadID, teacherSeeing);
            studentData[teacherSeeing]++;
            if (studentData[teacherSeeing] == 2) studentsLeft--;

            // help student (ie sleep thread)
            duration = mytime(leftBound, rightBound);
            printf("Teacher <%d> to sleep %d sec\n", threadID, duration);
            sleep(duration);
            printf("Teacher <%d> wake up\n", threadID);

            // finish helping student
            int copy = teacherSeeing;
            teacherSeeing = -1;
            teacherBusy = 0;
            printf("Teacher <%d> finishes helping: student %d\n", threadID, copy);
            pthread_cond_signal(&conds[copy]);
            printf("Teacher <%d> call cond_signal <conds[%d]>\n", threadID, copy);

            // unlock mutex
            pthread_mutex_unlock(&locks[copy]);
            printf("Teacher <%d> call mutex_unlock <locks[%d]>\n", threadID, copy);
        }
    }
    return NULL;
}

// student thread method
void *studentHandler(void *arg) {
    // set up variables
    int i = (long long int) arg;
    int duration;
    int threadID = (int)(intptr_t)pthread_self();

    // initial sleep
    duration = mytime(leftBound, rightBound);
    printf("Student <%d> to sleep %d sec\n", threadID, duration);
    sleep(duration);
    printf("Student <%d> wake up\n", threadID);

    // repeat until helped by teacher twice
    while (studentData[i] < 2) {
        // take data lock
        printf("Student <%d> will call mutex_lock <dataMutex>\n", threadID);
        pthread_mutex_lock(&dataMutex);

        // check if teacher available (ie doing own work)
        if (chairQ->chairsOccupied == 0 && teacherBusy == 0 && teacherSeeing == -1) {
            // walk into office
            teacherSeeing = i;
            printf("Student <%d> (number %d) walks in to teacher's office\n", threadID, i);
            pthread_mutex_unlock(&dataMutex);
            printf("Student <%d> call mutex_unlock <dataMutex>\n", threadID);

            // wait for teacher thread to wake up
            printf("Student <%d> will call mutex_lock <teacherLock>\n", threadID);
            pthread_mutex_lock(&teacherLock);
            printf("Student <%d> will call mutex_lock <locks[%d]>\n", threadID, i);
            pthread_mutex_lock(&locks[i]);
            pthread_cond_signal(&teacherCond);
            printf("Student <%d> call cond_signal <teacherCond>\n", threadID);
            pthread_mutex_unlock(&teacherLock);
            printf("Student <%d> call mutex_unlock <teacherLock>\n", threadID);

            // wait on teacher to finish help
            printf("Student <%d> will call cond_wait <conds[%d]>\n", threadID, i);
            int goal = studentData[i] + 1;
            while (goal != studentData[i]) pthread_cond_wait(&conds[i], &locks[i]);
            pthread_mutex_unlock(&locks[i]);
            printf("Student <%d> call mutex_unlock <locks[%d]>\n", threadID, i);
            continue;
        }
        // teacher is busy with another student - sit in chair
        if (chairQ->chairsAvailable == 0) {
            // no chairs - go study (ie sleep thread)
            printf("Student <%d> (number %d) cannot find a chair and leaves for a time\n", threadID, i);
            pthread_mutex_unlock(&dataMutex);
            printf("Student <%d> call mutex_unlock <dataMutex>\n", threadID);
            duration = mytime(leftBound, rightBound);
            printf("Student <%d> to sleep %d sec\n", threadID, duration);
            sleep(duration);
            printf("Student <%d> wake up\n", threadID);
        } else {
            // avaiable chairs - sit in one FIFO
            printf("Student <%d> will call mutex_lock <locks[%d]>\n", threadID, i);
            pthread_mutex_lock(&locks[i]);
            struct chair *newChair = malloc(sizeof(struct chair));
            newChair->index = i;
            newChair->next = NULL;
            if (chairQ->chairsOccupied == 0) {
                // list is empty - insert at head
                chairQ->head = newChair;
                chairQ->tail = newChair;
            } else {
                // list isn't empty - insert at tail
                chairQ->tail->next = newChair;
                chairQ->tail = newChair;
            }
            chairQ->chairsAvailable--;
            chairQ->chairsOccupied++;

            // wait to be seen by teacher
            pthread_mutex_unlock(&dataMutex);
            printf("Student <%d> (number %d) sits in a chair\n", threadID, i);
            printf("Student <%d> call mutex_unlock <dataMutex>\n", threadID);
            printf("Student <%d> will call cond_wait <conds[%d]>\n", threadID, i);
            int goal = studentData[i] + 1;
            while (goal != studentData[i]) pthread_cond_wait(&conds[i], &locks[i]);
            pthread_mutex_unlock(&locks[i]);
            printf("Student <%d> call mutex_unlock <locks[%d]>\n", threadID, i);
        }
    }
    return NULL;
}

// main method
int main(int argc, char** argv) {
    // verify input
    if (argc != 5) {
        printf("Incorrect arguments!\nUsage: ./a.out <number of students> <number of chairs> <left time interval> <right time interval>\n");
        return 1;
    }

    // parse arguments
    int numStudents = atoi(argv[1]);
    int numChairs = atoi(argv[2]);
    leftBound = atoi(argv[3]);
    rightBound = atoi(argv[4]);

    // set up random number generator
    srand(time(NULL));

    // set up chair queue
    chairQ = malloc(sizeof(struct chairs));
    chairQ->chairsOccupied = 0;
    chairQ->chairsAvailable = numChairs;
    chairQ->head = NULL;
    chairQ->tail = NULL;

    // set up global variables
    studentsLeft = numStudents;
    studentData = malloc(sizeof(int) * numStudents);
    locks = malloc(sizeof(pthread_mutex_t) * numStudents);
    conds = malloc(sizeof(pthread_cond_t) * numStudents);
    for (int i = 0; i < numStudents; i++) studentData[i] = 0;

    // initialize locks & conditions
    pthread_mutex_init(&teacherLock, NULL);
    pthread_cond_init(&teacherCond, NULL);
    for (int i = 0; i < numStudents; i++) {
        pthread_mutex_init(&locks[i], NULL);
        pthread_cond_init(&conds[i], NULL);
    }

    // set up threads
    pthread_t students[numStudents];
    pthread_t teacher;
    pthread_create(&teacher, NULL, teacherHandler, (void *) (long long int) -1);
    for (int i = 0; i < numStudents; i++) {
        pthread_create(&students[i], NULL, studentHandler, (void *) (long long int) i);
    }
    
    // join threads
    pthread_join(teacher, NULL);
    for (int i = 0; i < numStudents; i++) {
        pthread_join(students[i], NULL);
    }

    // close teacher thread
    pthread_cancel(teacher); 

    // destroy mutex
    pthread_mutex_destroy(&dataMutex);

    // destroy locks & conditions
    for (int i = 0; i < numStudents; i++) {
        pthread_mutex_destroy(&locks[i]);
        pthread_cond_destroy(&conds[i]);
    }
    pthread_mutex_destroy(&teacherLock);
    pthread_cond_destroy(&teacherCond);

    return 0;
}