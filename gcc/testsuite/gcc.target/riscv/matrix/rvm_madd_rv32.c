/* { dg-do compile } */
/* { dg-options "-march=rv32gcxtheadmatrix -mabi=ilp32d -O2" } */

#include <riscv_matrix.h>
#include <stddef.h>
#include "rvm-common.h"

#define MTEST(TYPE_T, STYPE, TYPE)  \
  void test_madd_##TYPE(m##TYPE_T *temp1, m##TYPE_T *temp2, m##TYPE_T *temp3, m##TYPE_T *temp4, m##TYPE_T x, m##TYPE_T y, TYPE_T z, uint8_t stride) {  \
    *temp1 = madd_m##STYPE(x, y);  \
    *temp2 = madd_m##STYPE##_m##STYPE##v(x, y, stride);  \
    *temp3 = madd_m##STYPE##_m##STYPE##v(x, y, 2);  \
    *temp4 = madd_m##STYPE##_##STYPE(x, z);  \
  }

RVM_INT_SD_TYPE_TEST(MTEST)


/* { dg-final { scan-assembler-times "madd.s.mm\t" 1 } } */
/* { dg-final { scan-assembler-times "madd.s.mv.x\t" 1 } } */
/* { dg-final { scan-assembler-times "madd.s.mv.i\t" 1 } } */
/* { dg-final { scan-assembler-times "madd.s.mx\t" 1 } } */
/* { dg-final { scan-assembler-times "madd.d.mm\t" 2 } } */
/* { dg-final { scan-assembler-times "madd.d.mv.x\t" 1 } } */
/* { dg-final { scan-assembler-times "madd.d.mv.i\t" 1 } } */
