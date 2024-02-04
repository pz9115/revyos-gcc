
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp_zpsfoperand -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int64_t test_kmar64(int64_t t, intXLEN_t a, intXLEN_t b) {
    return __rv__kmar64 (t, a, b);
}

/* { dg-final { scan-assembler "\tkmar64" } }*/

/* { dg-final { cleanup-saved-temps } } */
