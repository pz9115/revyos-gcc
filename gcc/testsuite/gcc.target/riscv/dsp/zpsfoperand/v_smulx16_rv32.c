
/* { dg-do assemble } */
/* { dg-options "-march=rv32gcp_zpsfoperand -mabi=ilp32d -O2 --save-temps" } */

#include <riscv-dsp.h>

int32x2_t test_v_smulx16(int16x2_t a, int16x2_t b) {
    return __rv__v_smulx16 (a, b);
}

/* { dg-final { scan-assembler "\tsmulx16" } }*/

/* { dg-final { cleanup-saved-temps } } */
