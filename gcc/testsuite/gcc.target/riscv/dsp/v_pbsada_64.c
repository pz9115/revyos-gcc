
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint64_t test_v_pbsada(uint64_t t, uint8x8_t a, uint8x8_t b) {
    return __rv__v_pbsada (t, a, b);
}

/* { dg-final { scan-assembler "\tpbsada" } }*/

/* { dg-final { cleanup-saved-temps } } */
