//
// Created by os on 7/27/23.
//

#include "../h/syscall_c.h"
#include "../lib/hw.h"
#include "../h/riscv.hpp"
#include "../lib/console.h"


void* callSystem (int code, size_t* args){
    __asm__ volatile ("ecall");
    void * retval;
    __asm__ volatile("mv a0, %[ret]" : [ret] "=r" (retval));
    return retval;
}

void* mem_alloc (size_t size){
    size_t blockNum = (size + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE;
    size_t args[1] = {blockNum};
    return callSystem(MEM_ALLOC, args);
}
int mem_free (void* address){
    void* retval = callSystem(MEM_FREE, (size_t*)address);
    return retval ? -1 : 0;
}

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg){

    size_t args[] = {(size_t)handle, (size_t)start_routine, (size_t)arg};
    void* retval = callSystem(THREAD_CREATE, args);
    return retval ? -1 : 0;
}

void thread_dispatch (){
    callSystem(THREAD_DISPATCH, nullptr);
}

int thread_exit (){
    callSystem(THREAD_EXIT, nullptr);
    return 0;
}

void thread_join (thread_t handle){
    size_t args[] = {(size_t)handle};
    callSystem(THREAD_JOIN, args);
}


int sem_open (sem_t* handle, unsigned int val){
    size_t args[] = {(size_t)handle, (size_t)val};
    void* retval = callSystem(SEM_OPEN, args);
    return retval ? -1 : 0;
}
int sem_close (sem_t handle){
    size_t args[] = {(size_t)handle};
    void* retval = callSystem(SEM_CLOSE, args);
    return retval ? -1 : 0;
}
int sem_wait (sem_t id){
    size_t args[] = {(size_t)id};
    void* retval = callSystem(SEM_WAIT, args);
    return retval ? -1 : 0;
}
int sem_signal (sem_t id){
    size_t args[] = {(size_t)id};
    void* retval = callSystem(SEM_SIGNAL, args);
    return retval ? -1 : 0;
}



int time_sleep (time_t time){
    size_t args[] = {(size_t)time};
    void* retval = callSystem(TIME_SLEEP, args);
    return retval ? -1 : 0;
}


char getc(){

    //return __getc();
    void* retval = callSystem(GETC, nullptr);
    return (char)((uint64)retval);
}
void putc(char c){
    //__putc(c);
    size_t args[] = {(size_t)c};
    callSystem(PUTC, args);
}



