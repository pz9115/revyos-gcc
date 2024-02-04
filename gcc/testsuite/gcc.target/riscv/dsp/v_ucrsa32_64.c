
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint32x2_t test_v_ucrsa32(uint32x2_t a, uint32x2_t b) {
    return __rv__v_ucrsa32 (a, b);
}

/* { dg-final { scan-assembler "\tcrsa32" } }*/

/* { dg-final { cleanup-saved-temps } } */
