
/* { dg-do assemble } */
/* { dg-options "-march=rv32gcp -mabi=ilp32d -O2 --save-temps" } */

#include <riscv-dsp.h>

uintXLEN_t test_ukaddh(uint32_t a, uint32_t b) {
    return __rv__ukaddh (a, b);
}

/* { dg-final { scan-assembler "\tukaddh" } }*/

/* { dg-final { cleanup-saved-temps } } */
