
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

unsigned long test_srl32 (unsigned long a, unsigned int b) {
return (unsigned long) ((uint32x2_t)a >> b);
}

/* { dg-final { scan-assembler "srl32\t" } }*/

/* { dg-final { cleanup-saved-temps } } */

