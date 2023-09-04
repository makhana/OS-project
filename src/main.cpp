//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/KSemaphore.hpp"

#include "../h/KConsole.h"


#include "../lib/console.h"

#include "../h/syscall_cpp.hpp"

void userMain();
void wrapperUserMain(void *){
    userMain();
}

//void testFunction(void*){
//
//    while(1){
//        thread_dispatch();
//    }
//
//}

//class ExampleThread: public PeriodicThread {
//
//public:
//    ExampleThread():PeriodicThread(20) {}
//
//    void periodicActivation() override {
//        putc('a');
//    }
//};


int main()
{
    MemoryAllocator::initialize();
    KConsole::initialize();

    Riscv::w_stvec((uint64) &Riscv::vectorTable | 1);


    thread_t threads[2];
    threads[0] = TCB::createThread(nullptr, nullptr, true);
    TCB::running = threads[0];

    threads[1] = TCB::createThread(wrapperUserMain, nullptr, false);


    Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // dozvoljeni softverski prekidi


//   ExampleThread* thr = new ExampleThread();
//
//   thr->start();
//
//    time_sleep(100);
//
//    thr->terminate();
//

    while(!threads[1]->isFinished()){
        thread_dispatch();
    }

    delete threads[1];


    return 0;
}




