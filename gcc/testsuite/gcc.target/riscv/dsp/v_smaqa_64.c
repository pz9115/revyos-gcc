
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int32x2_t test_v_smaqa(int32x2_t t, int8x8_t a, int8x8_t b) {
    return __rv__v_smaqa (t, a, b);
}

/* { dg-final { scan-assembler "\tsmaqa" } }*/

/* { dg-final { cleanup-saved-temps } } */
