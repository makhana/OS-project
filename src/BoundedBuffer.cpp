//
// Created by os on 8/10/23.
//

#include "../h/BoundedBuffer.h"

BoundedBuffer::BoundedBuffer() {
    KSemaphore::ksem_open(&spaceAvailable, bufferSpace);
    KSemaphore::ksem_open(&itemAvailable, 0);
    KSemaphore::ksem_open(&mutex, 1);
    head = tail = 0;
}

void BoundedBuffer::bufferPut(const char c) {
    KSemaphore::ksem_wait(spaceAvailable);
    KSemaphore::ksem_wait(mutex);
    buffer[tail] = c;
    tail = (tail+1) % bufferSpace;
    KSemaphore::ksem_signal(mutex);
    KSemaphore::ksem_signal(itemAvailable);
}

char BoundedBuffer::bufferGet() {
    KSemaphore::ksem_wait(itemAvailable);
    KSemaphore::ksem_wait(mutex);
    char c = buffer[head];
    head = (head+1) % bufferSpace;
    KSemaphore::ksem_signal(mutex);
    KSemaphore::ksem_signal(spaceAvailable);
    return c;
}
