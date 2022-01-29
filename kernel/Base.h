#pragma once

#define DEBUG 1

#ifdef DEBUG
#define INLINE inline __attribute__((always_inline))
#else
#define INLINE
#endif
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))
#define FLATTEN __attribute__((flatten))
#define NORETURN __attribute__ ((noreturn))
#define NAKED __attribute__((naked))
#define PACKED __attribute__((packed))

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long uintptr_t;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef unsigned long size_t;

#if defined(_M_X64) || defined(__amd64__)
typedef u64 uX;
#else
typedef u32 uX;
#endif

ALWAYS_INLINE void UNDEFINED() {
    asm volatile ("xchg %%bx, %%bx\n\t"
                  "hlt":::);
}