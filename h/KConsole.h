//
// Created by os on 8/10/23.
//

#ifndef PROJECT_KCONSOLE_H
#define PROJECT_KCONSOLE_H

#include "../h/BoundedBuffer.h"

class KConsole {
public:
    // status 0 -> putc
    // status 5 -> getc
    // CONSOLE_STATUS -> statusni registar
    // CONSOLE_TX_DATA -> za prijem podataka (getc)
    // CONSOLE_RX_DATA -> za slanje podataka (putc)

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
