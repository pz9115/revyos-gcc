
/* { dg-do assemble } */
/* { dg-options "-march=rv32gcp -mabi=ilp32d -O2 --save-temps" } */

#include <riscv-dsp.h>

uintXLEN_t test_clz32(uintXLEN_t a) {
    return __rv__clz32 (a);
}

/* { dg-final { scan-assembler "\tclz32" } }*/

/* { dg-final { cleanup-saved-temps } } */
