
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int8x8_t test_v_smax8(int8x8_t a, int8x8_t b) {
    return __rv__v_smax8 (a, b);
}

/* { dg-final { scan-assembler "\tsmax8" } }*/

/* { dg-final { cleanup-saved-temps } } */
