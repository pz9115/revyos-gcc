
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uint32x2_t test_v_clz32(uint32x2_t a) {
    return __rv__v_clz32 (a);
}

/* { dg-final { scan-assembler "\tclz32" } }*/

/* { dg-final { cleanup-saved-temps } } */
