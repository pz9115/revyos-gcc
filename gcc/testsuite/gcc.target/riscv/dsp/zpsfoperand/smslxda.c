
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp_zpsfoperand -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int64_t test_smslxda(int64_t t, uintXLEN_t a, uintXLEN_t b) {
    return __rv__smslxda (t, a, b);
}

/* { dg-final { scan-assembler "\tsmslxda" } }*/

/* { dg-final { cleanup-saved-temps } } */
