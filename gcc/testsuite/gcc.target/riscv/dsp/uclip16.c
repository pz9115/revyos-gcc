
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

uintXLEN_t test_uclip16(uintXLEN_t a, const uint32_t b) {
    return __rv__uclip16 (a, 1);
}

/* { dg-final { scan-assembler "\tuclip16" } }*/

/* { dg-final { cleanup-saved-temps } } */
