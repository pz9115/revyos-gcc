
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

intXLEN_t test_smaqa(intXLEN_t t, uintXLEN_t a, uintXLEN_t b) {
    return __rv__smaqa (t, a, b);
}

/* { dg-final { scan-assembler "\tsmaqa" } }*/

/* { dg-final { cleanup-saved-temps } } */
