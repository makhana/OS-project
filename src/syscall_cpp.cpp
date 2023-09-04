//
// Created by os on 7/31/23.
//

#include "../h/syscall_cpp.hpp"
#include "../h/scheduler.hpp"


void *operator new(size_t size) {
    return mem_alloc(size);
}

void operator delete(void *p) {
    mem_free(p);
}

Thread::Thread(void (*body)(void *), void *arg) {
    this->body = body;
    this->arg = arg;
}

Thread::Thread() {
    myHandle = nullptr;
    body = nullptr;
    arg = nullptr;
}

void Thread::dispatch() {
    thread_dispatch();
}

void Thread::join() {
    thread_join(myHandle);
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

int Thread::start() {
    if(body){
        thread_create(&myHandle, body, arg);
    } else {
        thread_create(&myHandle, &thread_wrapper, this);
    }
    return 0;
}


Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

void Console::putc(char c) {
    ::putc(c);
}

char Console::getc() {
    return ::getc();
}

PeriodicThread::PeriodicThread(time_t period):Thread() {
    PeriodicThread::period = period;
    finished = false;
}

void PeriodicThread::terminate() {
    finished = true;
}


