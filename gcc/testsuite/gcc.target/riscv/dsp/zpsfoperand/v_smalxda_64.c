
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp_zpsfoperand -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int64_t test_v_smalxda(int64_t t, int16x4_t a, int16x4_t b) {
    return __rv__v_smalxda (t, a, b);
}

/* { dg-final { scan-assembler "\tsmalxda" } }*/

/* { dg-final { cleanup-saved-temps } } */
