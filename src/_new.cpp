//
// Created by marko on 20.4.22..
//


#include "../h/MemoryAllocator.hpp"

#include "../lib/mem.h"

#include "../lib/hw.h"



//void *operator new(size_t n)
//{
//    return MemoryAllocator::kmem_alloc(n);
//}
//
//void *operator new[](size_t n)
//{
//    return MemoryAllocator::kmem_alloc(n);
//}
//
//void operator delete(void *p) noexcept
//{
//    MemoryAllocator::kmem_free(p);
//}
//
//void operator delete[](void *p) noexcept
//{
//    MemoryAllocator::kmem_free(p);
//}