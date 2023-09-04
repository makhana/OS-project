//
// Created by os on 8/10/23.
//

#include "../h/KConsole.h"
#include "../h/syscall_c.h"

KConsole* KConsole::initial = nullptr;

KSemaphore *KConsole::semGetReady = nullptr;
KSemaphore *KConsole::semPutReady = nullptr;


void KConsole::initialize() {
    if(initial == nullptr){
        initial = (KConsole*)MemoryAllocator::kmem_alloc(sizeof(KConsole));
        initial->bufferGET = new BoundedBuffer();
        initial->bufferPUT = new BoundedBuffer();
        KSemaphore::ksem_open(&(initial->semGetReady), 0);
        KSemaphore::ksem_open(&(initial->semPutReady), 0);

        TCB::createThread(initial->consumerFunction, nullptr, true);
        TCB::createThread(initial->producerFunction, nullptr, true);
    }
}

void KConsole::kconsole_handler() {
    int interruptNum = plic_claim();
    if(interruptNum == CONSOLE_IRQ){
        if(* (char * )CONSOLE_STATUS & CONSOLE_RX_STATUS_BIT){
            // getc ready
            KSemaphore::ksem_signal(semGetReady);
            // read data from console register
        }
        if (* (char * )CONSOLE_STATUS & CONSOLE_TX_STATUS_BIT){
            // putc ready
            KSemaphore::ksem_signal(semPutReady);
            // get data for sending to console (write to console register)

        }
        plic_complete(CONSOLE_IRQ);
    }
    plic_complete(interruptNum);
}

void KConsole::kputc(char c) {
    initial->bufferPUT->bufferPut(c);
}

char KConsole::kgetc() {
    return initial->bufferGET->bufferGet();
}


void KConsole::consumerFunction(void *) {
    // thread that does GET operation
    while(1){
        KSemaphore::ksem_wait(semGetReady);
        while(* (char * )CONSOLE_STATUS & CONSOLE_RX_STATUS_BIT){

            if(* (char * )CONSOLE_RX_DATA != '\000'){
                initial->bufferGET->bufferPut(* (char * )CONSOLE_RX_DATA);
            }
        }
    }
}



void KConsole::producerFunction(void *) {
    // thread that does PUT operation
    while(1){
        KSemaphore::ksem_wait(semPutReady);
        while(* (char * )CONSOLE_STATUS & CONSOLE_TX_STATUS_BIT){
            * (char * )CONSOLE_TX_DATA = (uint64)initial->bufferPUT->bufferGet();
        }
    }
}
