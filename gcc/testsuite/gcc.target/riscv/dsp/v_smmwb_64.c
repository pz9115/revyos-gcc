
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int32x2_t test_v_smmwb(int32x2_t a, int16x4_t b) {
    return __rv__v_smmwb (a, b);
}

/* { dg-final { scan-assembler "\tsmmwb" } }*/

/* { dg-final { cleanup-saved-temps } } */
