//
// Created by ubuntu on 8/29/21.
//

#ifndef OS_MEMORYMANAGER_H
#define OS_MEMORYMANAGER_H

#include "../Base.h"
#include <RBTree.h>

extern unsigned char memory_map_count;

constexpr u32 MIN_ALLOC_SIZE = 64;

#define BIOS_MEMORY_MAPPING_ADDRESS 0x8804

struct BiosMemoryMapping {
    uint64_t base_address;
    uint64_t length;
    uint32_t region_type;
    uint32_t extended_attributes;
} __attribute__((packed));

class MemoryManager {
public:
    RBTree<u32> tree;

    enum class ChunkFlags {
        MEMORY_USED
    };

    enum class MemoryMappingsOrigin {
        BIOS,
        MULTIBOOT1,
        MULTIBOOT2
    };

    MemoryManager() = default;
    void initialize(MemoryMappingsOrigin);
    void parse_bios_mappings();
    void parse_multiboot_mappings();
    void* alloc(u32);

};


#endif //OS_MEMORYMANAGER_H
