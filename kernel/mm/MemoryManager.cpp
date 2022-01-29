#include "MemoryManager.h"



void MemoryManager::parse_bios_mappings(){
    struct BiosMemoryMapping *bmm = (struct BiosMemoryMapping*)BIOS_MEMORY_MAPPING_ADDRESS;
    for(int i=0;i<memory_map_count;i++,bmm++){
        if(bmm->base_address>=0x100000 && bmm->region_type == 1) {
            asm volatile ("mov %0, %%eax\n\t"
                          "mov %1, %%ebx\n\t"
                          "xchg %%bx, %%bx"
                          ::"r"(bmm->base_address),"r"(bmm->length)
                          :"eax", "ebx");
            tree.insert(bmm->length, (void*)bmm->base_address);
        }
    }
}

void MemoryManager::parse_multiboot_mappings(){
    UNDEFINED();
}

void MemoryManager::initialize(MemoryMappingsOrigin mappings_origin) {
    if (mappings_origin == MemoryMappingsOrigin::BIOS) {
        parse_bios_mappings();
    } else if (mappings_origin == MemoryMappingsOrigin::MULTIBOOT1) {
        parse_multiboot_mappings();
    } else {
        UNDEFINED();
    }
}

//todo: very simplistic, basic splitting functionality
void* MemoryManager::alloc(u32 size) {
    size = (size+4)&~7;
    RBTree<u32>::Node* node = tree.find_smallest_not_below(size+sizeof(u32));

    if (!node)
        return nullptr;

    tree.remove_by_node(node);


    //chunk splitting in case req size<received chunk size
    if (node->value-size>=MIN_ALLOC_SIZE){
        tree.insert(node->value-size, (char*)node+size);
    } else {
        //assume that received chunk size is multiple of 4
        size = node->value;
    }

    *(u32*)node = size+(1<<(u8)(ChunkFlags::MEMORY_USED));
    return (void*)((char*)node+sizeof(u32));

}