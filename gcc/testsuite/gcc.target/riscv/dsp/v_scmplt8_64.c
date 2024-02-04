
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint8x8_t test_v_scmplt8(int8x8_t a, int8x8_t b) {
    return __rv__v_scmplt8 (a, b);
}

/* { dg-final { scan-assembler "\tscmplt8" } }*/

/* { dg-final { cleanup-saved-temps } } */
