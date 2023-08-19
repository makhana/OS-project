#include "../lib/hw.h"




class MemoryAllocator {
public:
    static void* kmem_alloc(size_t size);
    static int kmem_free(void*);
    static void initialize();

private:
    struct BlockHeader {
        size_t size;
        BlockHeader *prev;
        BlockHeader* next;
        bool free;
    };

    BlockHeader* freeMemHead;

    static MemoryAllocator* initial;



    static int tryToJoin(BlockHeader* arg);

    MemoryAllocator();


};