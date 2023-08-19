#include "../h/MemoryAllocator.hpp"


MemoryAllocator* MemoryAllocator::initial = nullptr;

void MemoryAllocator::initialize(){
    if(initial == nullptr){
        initial = (MemoryAllocator*) HEAP_START_ADDR;
        initial->freeMemHead = (BlockHeader*) ((char*) HEAP_START_ADDR + sizeof(MemoryAllocator));
        initial->freeMemHead->size = (char*) HEAP_END_ADDR - (char*) HEAP_START_ADDR - sizeof(MemoryAllocator) - sizeof(BlockHeader);
        initial->freeMemHead->prev = nullptr;
        initial->freeMemHead->next = nullptr;
        initial->freeMemHead->free = true;
    }
}

void* MemoryAllocator::kmem_alloc(size_t size){

    size_t newSize = ((size + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE;
    if(size == 0 || initial->freeMemHead == nullptr) return nullptr;
    BlockHeader *prev = nullptr;
    BlockHeader* cur = initial->freeMemHead;
    for(; cur != nullptr; prev = cur, cur = cur->next){
        if(cur->size >= newSize) break;
    }
    if(cur == nullptr) return nullptr;
    size_t remainingSize = cur->size - newSize;

    if(remainingSize >= sizeof(BlockHeader) + MEM_BLOCK_SIZE){
        // fragment remains
        cur->size = newSize;
        size_t offset = newSize + sizeof(BlockHeader);
        BlockHeader* newSeg = (BlockHeader*)((char*)cur + offset);
        if(prev) {
            prev->next = newSeg;
            newSeg->prev = prev;
        }
        else {
            initial->freeMemHead = newSeg;
            newSeg->prev = nullptr;
        }
        if(cur->next){
            cur->next->prev = newSeg;
        }
        newSeg->next = cur->next;
        newSeg->free = true;
        newSeg->size = remainingSize - sizeof(BlockHeader);
    } else {
        // allocate whole fragment
        if(prev) {
            prev->next = cur->next;
            if(cur->next) cur->next->prev = prev;
        }
        else {
            initial->freeMemHead = cur->next;
            initial->freeMemHead->prev = nullptr;
            if(initial->freeMemHead->next) initial->freeMemHead->next->prev = initial->freeMemHead;
        }

    }

    cur->next = nullptr;
    cur->free = false;
    return (char*)cur + sizeof(BlockHeader);

}

int MemoryAllocator::kmem_free(void* addr){
    if(!addr || (char*)addr < (char*)HEAP_START_ADDR + sizeof(MemoryAllocator) + sizeof(BlockHeader) || addr >= HEAP_END_ADDR) return -1;

    BlockHeader* newSeg = (BlockHeader*)((char*)addr - sizeof(BlockHeader));
    if(!newSeg || newSeg->free == true) return -2;

    BlockHeader* cur = nullptr;
    BlockHeader* prev = nullptr;

    if(!initial->freeMemHead){
        // insert new segment as freeMemHead
        newSeg->next = initial->freeMemHead;
        initial->freeMemHead = newSeg;
    } else if(addr < (char*)initial->freeMemHead){
        // insert new segment before freeMemHead
        initial->freeMemHead->prev = newSeg;
        newSeg->next = initial->freeMemHead;
        initial->freeMemHead = newSeg;
    } else {
        // insert new segment after prev before cur
        cur = initial->freeMemHead;
        for(; cur != nullptr && addr > (char*) (cur); prev = cur, cur = cur->next);
        newSeg->prev = prev;
        newSeg->next = cur;
        cur->prev = newSeg;
        prev->next = newSeg;
    }

    newSeg->free = true;

    // try to merge with previous and next segments
    tryToJoin(newSeg);
    tryToJoin(prev);

    return 0;

}

int MemoryAllocator::tryToJoin(BlockHeader* arg){
    if(!arg) return 0;
    if(arg->next && (char*)arg + arg->size + sizeof(BlockHeader) == (char*)(arg->next)){
        // remove cur->next segment
        arg->size += arg->next->size + sizeof(BlockHeader);
        arg->next = arg->next->next;
        if(arg->next) arg->next->prev = arg;
        return 1;
    } else {
        return 0;
    }
}

