#include <stdlib.h> 
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
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
sem_t* teacherSem;
sem_t** sems;
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
            printf("Teacher <%d> starts helping next in line: student %d\n", threadID, current);
            teacherBusy = 1;
            if (chairQ->head == chairQ->tail) chairQ->tail = NULL;
            chairQ->head = chairQ->head->next;
            chairQ->chairsOccupied--;
            chairQ->chairsAvailable++;
            studentData[current]++;
            if (studentData[current] == 2) studentsLeft--;

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
            sem_post(sems[current]);
            printf("Teacher <%d> call sem_post <sems[%d]>\n", threadID, current);
        } else {
            // no students in chairs - work on other things
            printf("Teacher <%d> works on own work\n", threadID);
            pthread_mutex_unlock(&dataMutex);
            printf("Teacher <%d> call mutex_unlock <dataMutex>\n", threadID);

            // wait on student to arrive
            printf("Student <%d> will call sem_wait <teacherSem>\n", threadID);
            int checkError = sem_wait(teacherSem);
            if (checkError == -1) perror("sem_wait");

            // begin helping student
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
            printf("Teacher <%d> finishes helping: student %d\n", threadID, teacherSeeing);
            int copy = teacherSeeing;
            teacherSeeing = -1;
            teacherBusy = 0;
            sem_post(sems[copy]);
            printf("Teacher <%d> call sem_post <sems[%d]>\n", threadID, copy);
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
            // walk into teacher's office
            teacherSeeing = i;
            printf("Student <%d> (number %d) walks in to teacher's office\n", threadID, i);
            pthread_mutex_unlock(&dataMutex);
            printf("Student <%d> call mutex_unlock <dataMutex>\n", threadID);
            sem_post(teacherSem);
            printf("Student <%d> (number %d) call sem_post <teacherSem>\n", threadID, i);

            // get helped by teacher
            printf("Student <%d> will call sem_wait <sems[%d]>\n", threadID, i);
            int checkError = sem_wait(sems[i]);
            if (checkError == -1) perror("sem_wait");
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
            printf("Student <%d> (number %d) sits in a chair\n", threadID, i);
            pthread_mutex_unlock(&dataMutex);
            printf("Student <%d> call mutex_unlock <dataMutex>\n", threadID);
            printf("Student <%d> will call sem_wait <sems[%d]>\n", threadID, i);
            int checkError = sem_wait(sems[i]);
            if (checkError == -1) perror("sem_wait");
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
    sems = malloc(sizeof(sem_t *) * numStudents);
    for (int i = 0; i < numStudents; i++) studentData[i] = 0;

    // define semaphores
    teacherSem = sem_open("sem_teacher", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    if (teacherSem == SEM_FAILED) perror("sem_open");
    for (int i = 0; i < numStudents; i++) {
        char name[8];
        sprintf(name, "sem_%d", i);
        sems[i] = sem_open(name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
        if (sems[i] == SEM_FAILED) perror("sem_open");
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

    // destroy semaphores
    for (int i = 0; i < numStudents; i++) {
        char name[8];
        sprintf(name, "sem_%d", i);
        sem_unlink(name);
    }
    sem_unlink("sem_teacher");

    return 0;
}