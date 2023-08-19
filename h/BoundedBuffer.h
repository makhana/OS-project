//
// Created by os on 8/10/23.
//

#ifndef PROJECT_BOUNDEDBUFFER_H
#define PROJECT_BOUNDEDBUFFER_H

#include "../lib/hw.h"
#include "../h/KSemaphore.hpp"


class BoundedBuffer {
public:
    BoundedBuffer ();
    void bufferPut(const char c);
    char bufferGet ();

    static void *operator new(size_t n)
    {
        return MemoryAllocator::kmem_alloc(n);
    }

    static void *operator new[](size_t n)
    {
        return MemoryAllocator::kmem_alloc(n);
    }

    static void operator delete(void *p) noexcept
    {
        MemoryAllocator::kmem_free(p);
    }

    static void operator delete[](void *p) noexcept
    {
        MemoryAllocator::kmem_free(p);
    }
private:
    KSemaphore *spaceAvailable, *itemAvailable, *mutex;
    static constexpr uint64 bufferSpace = 4096;
    char buffer[bufferSpace];
    int head, tail;

};
#endif //PROJECT_BOUNDEDBUFFER_H
