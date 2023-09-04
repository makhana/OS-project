//
// Created by os on 7/27/23.
//

#ifndef PROJECT_KSEMAPHORE_HPP
#define PROJECT_KSEMAPHORE_HPP

#include "list.hpp"
#include "tcb.hpp"


class KSemaphore{
public:

    KSemaphore(unsigned init = 1){
        val = init;
    }

    static int ksem_wait(KSemaphore* handle);
    static int ksem_signal(KSemaphore* handle);

    static int ksem_open(KSemaphore** handle, unsigned init);

    static int ksem_close(KSemaphore* handle);
    int getVal() const
    {
        return val;
    }


protected:
    void block();
    void unblock();

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
    int val;
    List<TCB> blockedThreads;

};

#endif //PROJECT_KSEMAPHORE_HPP
