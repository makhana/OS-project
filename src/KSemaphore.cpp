//
// Created by os on 8/13/23.
//

#include "../h/KSemaphore.hpp"

void KSemaphore::block() {
    TCB *old = TCB::running;
    if (!old->isFinished()) { blockedThreads.addLast(old); }
    TCB::running = Scheduler::get();

    TCB::contextSwitch(&old->context, &TCB::running->context);
}

void KSemaphore::unblock() {
    TCB* thr = blockedThreads.removeFirst();
    if(thr != nullptr){
        Scheduler::put(thr);
    }
}

int KSemaphore::ksem_wait(KSemaphore *handle) {
    if(!handle) return -1;
    if(--handle->val < 0) handle->block();
    return 0;
}

int KSemaphore::ksem_signal(KSemaphore *handle) {
    if(!handle) return -1;
    if(++handle->val <= 0) handle->unblock();
    return 0;
}

int KSemaphore::ksem_open(KSemaphore **handle, unsigned int init) {
    *handle = new KSemaphore(init);
    return (*handle) ? 0 : -1;
}

int KSemaphore::ksem_close(KSemaphore *handle) {
    if(!handle) return -1;
    while(TCB* thr = handle->blockedThreads.removeFirst()){
        Scheduler::put(thr);
    }
    delete handle;
    return 0;
}
