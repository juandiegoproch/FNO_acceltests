#ifndef CHIPYARD_COMPATIBILITY_H
#define CHIPYARD_COMPATIBILITY_H

#include <stdint.h>
#include <limits.h>

/* Tipos Gemmini-compatible */
typedef int8_t  elem_t;
typedef int32_t acc_t;

/* Saturación */
#define elem_t_max INT8_MAX
#define elem_t_min INT8_MIN

/* RV32 safe mcycle */
static inline uint64_t read_cycles(void)
{
    uint32_t hi, lo, hi2;

    asm volatile ("rdcycleh %0" : "=r"(hi));
    asm volatile ("rdcycle  %0" : "=r"(lo));
    asm volatile ("rdcycleh %0" : "=r"(hi2));

    while (hi != hi2) {
        hi = hi2;
        asm volatile ("rdcycle  %0" : "=r"(lo));
        asm volatile ("rdcycleh %0" : "=r"(hi2));
    }

    return ((uint64_t)hi << 32) | lo;
}

/* OPTIONAL:
 * comentar si el core no soporta instret
 */
static inline uint64_t read_instret(void)
{
    uint32_t hi, lo, hi2;

    asm volatile ("rdinstreth %0" : "=r"(hi));
    asm volatile ("rdinstret  %0" : "=r"(lo));
    asm volatile ("rdinstreth %0" : "=r"(hi2));

    while (hi != hi2) {
        hi = hi2;
        asm volatile ("rdinstret  %0" : "=r"(lo));
        asm volatile ("rdinstreth %0" : "=r"(hi2));
    }

    return ((uint64_t)hi << 32) | lo;
}

#endif