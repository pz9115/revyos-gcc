
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int16x4_t test_v_kcras16(int16x4_t a, int16x4_t b) {
    return __rv__v_kcras16 (a, b);
}

/* { dg-final { scan-assembler "\tkcras16" } }*/

/* { dg-final { cleanup-saved-temps } } */
