
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint16x4_t test_v_ucrsa16(uint16x4_t a, uint16x4_t b) {
    return __rv__v_ucrsa16 (a, b);
}

/* { dg-final { scan-assembler "\tcrsa16" } }*/

/* { dg-final { cleanup-saved-temps } } */
