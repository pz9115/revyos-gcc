
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int32x2_t test_v_smaqa_su(int32x2_t t, int8x8_t a, uint8x8_t b) {
    return __rv__v_smaqa_su (t, a, b);
}

/* { dg-final { scan-assembler "\tsmaqa.su" } }*/

/* { dg-final { cleanup-saved-temps } } */
