
/* { dg-do assemble } */
/* { dg-options "-march=rv32gcp -mabi=ilp32d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint16x2_t test_v_pktt16(uint16x2_t a, uint16x2_t b) {
    return __rv__v_pktt16 (a, b);
}

/* { dg-final { scan-assembler "\tpktt16" } }*/

/* { dg-final { cleanup-saved-temps } } */
