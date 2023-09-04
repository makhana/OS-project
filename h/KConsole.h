//
// Created by os on 8/10/23.
//

#ifndef PROJECT_KCONSOLE_H
#define PROJECT_KCONSOLE_H

#include "../h/BoundedBuffer.h"

class KConsole {
public:
    // CONSOLE_TX_STATUS_BIT -> putc
    // CONSOLE_RX_STATUS_BIT -> getc
    // CONSOLE_STATUS -> status register
    // CONSOLE_TX_DATA -> for sending data (putc)
    // CONSOLE_RX_DATA -> for receiving data (getc)

    static void initialize();

    static char kgetc();
    static void kputc(char);

    static void kconsole_handler();

    static void consumerFunction(void*);
    static void producerFunction(void*);


private:
    BoundedBuffer *bufferPUT;
    BoundedBuffer *bufferGET;
    static KSemaphore *semPutReady;
    static KSemaphore *semGetReady;



    static KConsole* initial;

    TCB* consumer;
    TCB* producer;
};

#endif //PROJECT_KCONSOLE_H
