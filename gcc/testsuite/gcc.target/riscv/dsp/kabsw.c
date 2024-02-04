
/* { dg-do assemble } */
/* { dg-options "-march=rv64gcp -mabi=lp64d -O2 --save-temps" } */

#include <riscv-dsp.h>

int32_t test_kabsw(int32_t a) {
    return __rv__kabsw (a);
}

/* { dg-final { scan-assembler "\tkabsw" } }*/

/* { dg-final { cleanup-saved-temps } } */
