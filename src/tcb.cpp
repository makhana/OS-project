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
//uint64 TCB::userSPOffset = OFFSETOF(TCB, userStackSP);

void TCB::yield()
{
    TCB::dispatch();
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    //Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
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

//
//
//void Resourse::take(int num_of_instances, int time_ticks){
//    int t = time_ticks;
//    int num = num_of_instances;
//    for(int i = 0; i< num_of_instances; i++){
//        sem_wait(semMutex);
//    }
//    TCB::running->setTimeTicks(t);
//    TCB::running->setResources(num);
//    count++;
//    if(t == 0){
//        TCB::running->setDontTake(true);
//    } else {
//        TCB::running->setDontTake(false);
//    }
//    threadsWithResources.addLast(TCB::running);
//    printString("Nit ");
//    printInt(TCB::running->getId());
//    printString(" uzima ");
//    printInt(num_of_instances);
//    printString(" resursa\n");
//}
//
//int Resourse::give_back(int num_of_instances) {
//    if(TCB::running->getResources() == 0){
//        return 0;
//    }
//    int n = 0;
//    for(int i = 0; i< num_of_instances; i++){
//        sem_signal(semMutex);
//        n++;
//    }
//
//    printString("Nit ");
//    printInt(TCB::running->getId());
//    printString(" vraca ");
//    printInt(num_of_instances);
//    printString(" resursa\n");
//    TCB::running->setResources(TCB::running->getResources() - n);
//    return n;
//}
//
//Resourse::Resourse(int N): PeriodicThread(1) {
//    numOfInstances = N;
//    sem_open(&(semMutex), N);
//    count = 0;
//}
//
//void Resourse::periodicActivation() {
//
//        int cnt = 0;
//        while(TCB* t = threadsWithResources.removeFirst()){
//            if(t == nullptr){
//                break;
//            }
//            cnt++;
//
//            if(t->isDontTake()){
//                threadsWithResources.addFirst(t);
//            } else {
//                int time = t->getTimeTicks();
//                time--;
//                t->setTimeTicks(time);
//                if(time == 0){
//                    int num = t->getResources();
//                    while(1){
//                        int r = give_back(t->getResources());
//                        if(r == t->getResources()){
//                            t->setResources(0);
//                            break;
//                        } else {
//                            t->setResources(t->getResources() - r);
//                        }
//                    }
//                    printString("Niti ");
//                    printInt(t->getId());
//                    printString(" oduzeto ");
//                    printInt(num);
//                    printString(" resursa\n");
//                } else {
//                    threadsWithResources.addFirst(t);
//                }
//
//            }
//
//            if(cnt == count){
//                break;
//            }
//        }
//
//}
//


