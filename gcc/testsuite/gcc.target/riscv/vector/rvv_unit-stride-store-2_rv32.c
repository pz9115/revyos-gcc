/* { dg-do compile } */
/* { dg-options "-march=rv32gcv -mabi=ilp32d" } */

#include <riscv_vector.h>
#include <stddef.h>
#include "rvv-common.h"

/* Takes the scalar type STYPE, vector class VCLASS (int or float), and
   the e and m value.  */
#define VSLOADSTORE(STYPE, VCLASST, VCLASS, EM, MLEN, STYPEC, SEW, OP)                                   \
  void vsloadstore##VCLASS##EM(size_t n, long stride, STYPE *x,                \
                               STYPE *y, STYPE z, word_type vl) {                            \
    v##VCLASST##EM##_t vx, vy, vz;                                           \
    vbool##MLEN##_t mask;                                                   \
    mask = MSET (MLEN);                                             \
    vx = VLOAD(VCLASS, SEW, EM, x);                                               \
    vy = VLOAD(VCLASS, SEW, EM, y);                                               \
    vz = v##OP##_vv_##VCLASS##EM (vx, vy, vl);                                      \
    vse##SEW##_v_##VCLASS##EM##_m(mask, x, vz, vl);                                   \
  }

RVV_INT_TEST_ARG(VSLOADSTORE, add)
RVV_UINT_TEST_ARG(VSLOADSTORE, add)
RVV_FLOAT_TEST_ARG(VSLOADSTORE, fadd)

/* { dg-final { scan-assembler-times "vse8.v" 14 } } */
/* { dg-final { scan-assembler-times "vse16.v" 18 } } */
/* { dg-final { scan-assembler-times "vse32.v" 15 } } */
/* { dg-final { scan-assembler-times "vse64.v" 12 } } */
