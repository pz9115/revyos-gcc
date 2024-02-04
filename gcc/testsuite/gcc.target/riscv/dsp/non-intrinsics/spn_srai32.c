
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

unsigned long test_srai32 (unsigned long a, unsigned int b) {
return (unsigned long) ((int32x2_t)a >> 1);
}

/* { dg-final { scan-assembler "srai32\t" } }*/

/* { dg-final { cleanup-saved-temps } } */

