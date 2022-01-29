#include "interrupts/PIC.h"
#include "interrupts/InterruptController.h"
#include "mm/MemoryManager.h"

unsigned char memory_map_count;

extern "C" void kernel_main() {
    volatile char *mon=(volatile char*)0xb8000;
    int v=66;


    InterruptController ic{};
    PIC pic{};
    pic.enable_vector(1);
    MemoryManager mm{};

    mm.initialize(MemoryManager::MemoryMappingsOrigin::BIOS);
    
    while(1){
        mon[0]=v++;
        mon[2]=v++;
        mon[4]=v++;
        if (v==77) v=66;
        if (v==77) v=66;
        if (v==77) v=66;
    }
}
