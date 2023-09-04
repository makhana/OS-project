//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"

#include "../lib/hw.h"



TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB* TCB::sleepyHead = nullptr;

uint64 TCB::systemSPOffset = OFFSETOF(TCB, systemStackSP);



void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    if(TCB::running->isSystem){
        Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    }
    else{
        Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    }

    Riscv::popSppSpie();
    running->body(running->args);
    thread_exit();
}


TCB *TCB::createThread(TCB::Body body, void* args, bool isSys){
    return new TCB(body, args, isSys);
}

int TCB::tcbThreadExit(){
    TCB* old = running;
    while(TCB* t = running->joinQueue.removeFirst()){
        Scheduler::put(t);
    }
    old->setFinished(true);
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);

    return 0;
}

void TCB::tcbThreadJoin(TCB* handle){
    if(handle == running) return;
    if(handle->isFinished()) return;
    TCB *old = running;
    handle->joinQueue.addLast(running);
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

int TCB::tcbThreadSleep(time_t time) {
    TCB *old = running;


    TCB::running->sleepyTime = time;
    if(sleepyHead == nullptr){
        sleepyHead = running;
        sleepyHead->sleepyNext = nullptr;
        sleepyHead->sleepyPrev = nullptr;
    } else {
        TCB* t = sleepyHead;
        for(; t->sleepyNext != nullptr; t = t->sleepyNext){}
        t->sleepyNext = running;
        running->sleepyPrev = t;
        running->sleepyNext = nullptr;
    }

    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);

    return 0;

}

void *TCB::operator new(size_t size) {
    return MemoryAllocator::kmem_alloc(size);
}


void TCB::operator delete(void *p) {
    MemoryAllocator::kmem_free(p);
}

void *TCB::operator new[](size_t size) {
    return MemoryAllocator::kmem_alloc(size);
}


void TCB::operator delete[](void *p) {
    MemoryAllocator::kmem_free(p);
}

TCB::~TCB() {
    delete stack;
}


