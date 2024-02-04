
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint8x8_t test_v_srl8_u(uint8x8_t a, uint32_t b) {
    return __rv__v_srl8_u (a, b);
}

/* { dg-final { scan-assembler "\tsrl8.u" } }*/

/* { dg-final { cleanup-saved-temps } } */
