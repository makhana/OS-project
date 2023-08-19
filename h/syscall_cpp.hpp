//
// Created by os on 7/27/23.
//

#ifndef PROJECT_SYSCALL_CPP_HPP
#define PROJECT_SYSCALL_CPP_HPP


#include "./syscall_c.h"
#include "../h/riscv.hpp"

void* operator new (size_t);
void operator delete (void*);

class Thread {
public:
    Thread (void (*body) (void*), void* arg);
    virtual ~Thread () = default;

    int start();

    void join();

    static void dispatch();
    static int sleep (time_t);

protected:
    Thread();
    virtual void run() {}

private:
    thread_t myHandle;
    void (*body) (void*);
    void* arg;



    static void thread_wrapper(void * arg){
        Thread* thr = (Thread*) arg;
        thr->run();
    }

};

class Semaphore {
public:

    Semaphore (unsigned init = 1);
    virtual ~Semaphore();

    int wait();
    int signal();

private:
    sem_t myHandle;
};

class PeriodicThread: public Thread {
public:
    void terminate();

protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation (){}

    void run() override {
        while(finished){
            periodicActivation();
            sleep(period);
        }

    }

private:
    time_t period;
    bool finished;
};

class Console {
public:
    static char getc();
    static void putc (char);
};

#endif //PROJECT_SYSCALL_CPP_HPP
