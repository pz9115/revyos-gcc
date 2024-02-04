
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp_zpsfoperand -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint64_t test_ukmsr64(uint64_t t, uintXLEN_t a, uintXLEN_t b) {
    return __rv__ukmsr64 (t, a, b);
}

/* { dg-final { scan-assembler "\tukmsr64" } }*/

/* { dg-final { cleanup-saved-temps } } */
