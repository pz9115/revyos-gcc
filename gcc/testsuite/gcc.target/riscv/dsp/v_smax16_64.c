
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int16x4_t test_v_smax16(int16x4_t a, int16x4_t b) {
    return __rv__v_smax16 (a, b);
}

/* { dg-final { scan-assembler "\tsmax16" } }*/

/* { dg-final { cleanup-saved-temps } } */
