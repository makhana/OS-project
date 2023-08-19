//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"
//#include "../h/MemoryAllocator.hpp"

#include "../lib/mem.h"

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

// Thread Control Block
class TCB
{
public:
    ~TCB();

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);

    static TCB *createThread(Body body, void* args, bool isSys);

    static void yield();

    static TCB *running;


    static void dispatch();


    static int tcbThreadExit();

    static void tcbThreadJoin(TCB* handle);

    static int tcbThreadSleep(time_t time);

    static void* operator new(size_t);
    static void operator delete(void*p);
    static void* operator new[](size_t);
    static void operator delete[](void*p);




private:

    TCB(Body b, void* a, bool isSys)
    {
        body = b;
        stack = (uint64*) MemoryAllocator::kmem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64));
        args = a;
        context.ra = body != nullptr ? (uint64) &threadWrapper : 0;
        context.sp = stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0;
        timeSlice = DEFAULT_TIME_SLICE;
        finished = false;
        isSystem = isSys;
        systemStack = (uint64*)MemoryAllocator::kmem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64));
        systemStackSP = (uint64)&systemStack[DEFAULT_STACK_SIZE];
        if (body != nullptr) { Scheduler::put(this); }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };



    Body body;
    uint64 *stack;
    void* args;
    Context context;
    uint64 timeSlice;
    bool finished;

    uint64 sepc;
    uint64 sstatus;


    bool isSystem;

    uint64* systemStack;
    uint64 systemStackSP;

    static TCB* sleepyHead;
    TCB* sleepyNext;
    TCB* sleepyPrev;
    uint64 sleepyTime;


    List<TCB> joinQueue;

    friend class Riscv;
    friend class KSemaphore;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);



    static uint64 timeSliceCounter;
    

};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
