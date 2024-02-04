
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp_zpsfoperand -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int16x4_t test_v_smul8(int8x4_t a, int8x4_t b) {
    return __rv__v_smul8 (a, b);
}

/* { dg-final { scan-assembler "\tsmul8" } }*/

/* { dg-final { cleanup-saved-temps } } */
