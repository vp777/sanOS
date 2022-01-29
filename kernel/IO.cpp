#include "IO.h"



u8 in8(u16 port)
{
    u8 value;
    asm volatile("inb %1, %0"
    : "=a"(value)
    : "Nd"(port));
    return value;
}

u16 in16(u16 port)
{
    u16 value;
    asm volatile("inw %1, %0"
    : "=a"(value)
    : "Nd"(port));
    return value;
}

u32 in32(u16 port)
{
    u32 value;
    asm volatile("inl %1, %0"
    : "=a"(value)
    : "Nd"(port));
    return value;
}

void out8(u16 port, u8 value) {
    asm volatile("outb %0, %1" ::"a"(value), "Nd"(port));
}

void out16(u16 port, u16 value)
{
    asm volatile("outw %0, %1" ::"a"(value), "Nd"(port));
}

void out32(u16 port, u32 value)
{
    asm volatile("outl %0, %1" ::"a"(value), "Nd"(port));
}

void delay(size_t microseconds)
{
    for (size_t i = 0; i < microseconds; ++i)
        in8(0x80);
}