
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

unsigned long test_uadd32 (unsigned long a, unsigned long b) {
return (unsigned long) ((int32x2_t)a + (int32x2_t)b);
}

/* { dg-final { scan-assembler "add32\t" } }*/

/* { dg-final { cleanup-saved-temps } } */

