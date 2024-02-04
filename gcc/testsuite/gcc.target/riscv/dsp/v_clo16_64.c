
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint16x4_t test_v_clo16(uint16x4_t a) {
    return __rv__v_clo16 (a);
}

/* { dg-final { scan-assembler "\tclo16" } }*/

/* { dg-final { cleanup-saved-temps } } */
