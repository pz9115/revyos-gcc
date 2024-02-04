
/* { dg-do assemble } */
/* { dg-options "-march=rv32gcp -mabi=ilp32d -O2 --save-temps" } */

#include <riscv-dsp.h>

uintXLEN_t test_uraddw(uint32_t a, uint32_t b) {
    return __rv__uraddw (a, b);
}

/* { dg-final { scan-assembler "\turaddw" } }*/

/* { dg-final { cleanup-saved-temps } } */
