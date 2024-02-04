
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint8x8_t test_v_uksub8(uint8x8_t a, uint8x8_t b) {
    return __rv__v_uksub8 (a, b);
}

/* { dg-final { scan-assembler "\tuksub8" } }*/

/* { dg-final { cleanup-saved-temps } } */
