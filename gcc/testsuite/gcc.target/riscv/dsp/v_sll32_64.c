
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint32x2_t test_v_sll32(uint32x2_t a, uint32_t b) {
    return __rv__v_sll32 (a, b);
}

/* { dg-final { scan-assembler "\tsll32" } }*/

/* { dg-final { cleanup-saved-temps } } */
