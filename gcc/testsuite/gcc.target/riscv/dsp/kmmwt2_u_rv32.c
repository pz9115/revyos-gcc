
/* { dg-do assemble } */
/* { dg-options "-march=rv32gcp -mabi=ilp32d -O2 --save-temps" } */

#include <riscv-dsp.h>

intXLEN_t test_kmmwt2_u(intXLEN_t a, uintXLEN_t b) {
    return __rv__kmmwt2_u (a, b);
}

/* { dg-final { scan-assembler "\tkmmwt2.u" } }*/

/* { dg-final { cleanup-saved-temps } } */
