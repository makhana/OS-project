//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/KSemaphore.hpp"

#include "../h/KConsole.h"


#include "../lib/console.h"

void userMain();
void wrapperUserMain(void *){
    userMain();
}




int main()
{
    MemoryAllocator::initialize();
    KConsole::initialize();

    Riscv::w_stvec((uint64) &Riscv::vectorTable | 1);


    thread_t threads[4];
    threads[0] = TCB::createThread(nullptr, nullptr, true);
    TCB::running = threads[0];

    threads[1] = TCB::createThread(wrapperUserMain, nullptr, false);

//    threads[2] = TCB::createThread(KConsole::consumerFunction, nullptr, true);
//    threads[3] = TCB::createThread(KConsole::producerFunction, nullptr, true);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // dozvoljeni softverski prekidi



    while(!threads[1]->isFinished()){
        thread_dispatch();
    }

    delete threads[1];
//    delete threads[2];
//    delete threads[3];

    return 0;
}




