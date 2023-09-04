//
// Created by marko on 20.4.22.
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../lib/hw.h"
#include "../h/KConsole.h"


#include "../h/KSemaphore.hpp"

#include "../lib/mem.h"

#include "../test/printing.hpp"



void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}



void Riscv::handleSupervisorTrap(int code, void* args)
{
    uint64 scause = r_scause();

    size_t *argsF = (size_t*) args;

    void* ret = nullptr;
    if(scause == 0x0000000000000009UL || scause == 0x0000000000000008UL) {

        switch(code){
            case MEM_ALLOC: {
                Riscv::w_sepc(r_sepc() + 4);
                size_t size = (size_t) (argsF[0]) * MEM_BLOCK_SIZE;
                ret = MemoryAllocator::kmem_alloc(size);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case MEM_FREE: {
                Riscv::w_sepc(r_sepc() + 4);
                ret = (void*)((size_t)MemoryAllocator::kmem_free(args));
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case THREAD_CREATE: {
                Riscv::w_sepc(r_sepc() + 4);
                TCB::Body routine = (TCB::Body) argsF[1];


                TCB* thread = TCB::createThread(routine, (void*)argsF[2], false);
                TCB** handle = (TCB**) argsF[0];
                *handle = thread;
                ret = (*handle ? (void*)0 : (void*)-1);

                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case THREAD_DISPATCH: {
                TCB::running->sepc = Riscv::r_sepc() + 4;
                TCB::running->sstatus = Riscv::r_sstatus();
                TCB::dispatch();
                Riscv::w_sstatus(TCB::running->sstatus);
                Riscv::w_sepc(TCB::running->sepc);
                break;
            }
            case THREAD_EXIT: {
                TCB::running->sepc = Riscv::r_sepc() + 4;
                TCB::running->sstatus = Riscv::r_sstatus();
                TCB::tcbThreadExit();
                Riscv::w_sstatus(TCB::running->sstatus);
                Riscv::w_sepc(TCB::running->sepc);
                break;
            }
            case THREAD_JOIN: {
                TCB::running->sepc = Riscv::r_sepc() + 4;
                TCB::running->sstatus = Riscv::r_sstatus();
                TCB::tcbThreadJoin((TCB*)argsF[0]);
                Riscv::w_sstatus(TCB::running->sstatus);
                Riscv::w_sepc(TCB::running->sepc);
                break;
            }
            case SEM_OPEN: {
                Riscv::w_sepc(r_sepc() + 4);
                ret = (void* ) (size_t)KSemaphore::ksem_open((KSemaphore**) argsF[0], (int)argsF[1]);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case SEM_CLOSE: {
                Riscv::w_sepc(r_sepc() + 4);
                ret = (void* ) (size_t)KSemaphore::ksem_close((KSemaphore*) argsF[0]);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case SEM_WAIT: {
                TCB::running->sepc = Riscv::r_sepc() + 4;
                TCB::running->sstatus = Riscv::r_sstatus();
                ret = (void* ) (size_t)KSemaphore::ksem_wait((KSemaphore*) argsF[0]);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                Riscv::w_sstatus(TCB::running->sstatus);
                Riscv::w_sepc(TCB::running->sepc);
                break;
            }
            case SEM_SIGNAL: {
                Riscv::w_sepc(r_sepc() + 4);
                ret = (void* ) (size_t)KSemaphore::ksem_signal((KSemaphore*) argsF[0]);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case GETC: {
                TCB::running->sepc = Riscv::r_sepc() + 4;
                TCB::running->sstatus = Riscv::r_sstatus();
                ret = (void* ) ((size_t)KConsole::kgetc());
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                Riscv::w_sstatus(TCB::running->sstatus);
                Riscv::w_sepc(TCB::running->sepc);
                break;
            }
            case PUTC: {
                TCB::running->sepc = Riscv::r_sepc() + 4;
                TCB::running->sstatus = Riscv::r_sstatus();
                KConsole::kputc((char)argsF[0]);
                Riscv::w_sstatus(TCB::running->sstatus);
                Riscv::w_sepc(TCB::running->sepc);
                break;
            }
            case TIME_SLEEP: {
                TCB::running->sepc = Riscv::r_sepc() + 4;
                TCB::running->sstatus = Riscv::r_sstatus();
                if((time_t ) argsF[0] == 0){

                } else {
                    ret = (void*)(size_t)TCB::tcbThreadSleep((time_t ) argsF[0]);
                    __asm__ volatile("mv a0, %0" : : "r" (ret));
                }
                Riscv::w_sstatus(TCB::running->sstatus);
                Riscv::w_sepc(TCB::running->sepc);
                break;
            }
        }
    }

    else {
        while(true){
            // if illegal action happens end up here
        }
    }
}

void Riscv::handleExternalTrap() {
    KConsole::kconsole_handler();
}

void Riscv::handleSoftwareTrap() {
    //interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)

    // sleepy threads
    TCB* t = TCB::sleepyHead;
    for(; t!= nullptr; t = t->sleepyNext){
        t->sleepyTime -= 1;
        if(t->sleepyTime == 0){
            if(t->sleepyPrev){
                t->sleepyPrev->sleepyNext = t->sleepyNext;
                if(t->sleepyNext)  t->sleepyNext->sleepyPrev = t->sleepyPrev;
            } else {
                TCB::sleepyHead = t->sleepyNext;
                if(TCB::sleepyHead != nullptr)  TCB::sleepyHead->sleepyPrev = nullptr;
            }

            t->sleepyNext = nullptr;
            t->sleepyPrev = nullptr;
            Scheduler::put(t);
        }
    }

    TCB::timeSliceCounter++;
    if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
    {
        TCB::running->sepc = Riscv::r_sepc();
        TCB::running->sstatus = Riscv::r_sstatus();
        TCB::timeSliceCounter = 0;
        TCB::dispatch();
        Riscv::w_sstatus(TCB::running->sstatus);
        Riscv::w_sepc(TCB::running->sepc);
    }
    mc_sip(SIP_SSIP); // software interrupt processed
}

