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
    vx = VLOAD(VCLASS, SEW, EM, x);                                               \
    vy = vle##SEW##ff_v_##VCLASS##EM(y, NULL, vl);                                             \
    vz = v##OP##_vv_##VCLASS##EM (vx, vy, vl);                                      \
    VSTORE(VCLASS, SEW, EM, x, vz);                                                \
  }

RVV_INT_TEST_ARG(VSLOADSTORE, add)
RVV_UINT_TEST_ARG(VSLOADSTORE, add)
RVV_FLOAT_TEST_ARG(VSLOADSTORE, fadd)

/* { dg-final { scan-assembler-times "vle8ff.v" 14 } } */
/* { dg-final { scan-assembler-times "vle16ff.v" 18 } } */
/* { dg-final { scan-assembler-times "vle32ff.v" 15 } } */
/* { dg-final { scan-assembler-times "vle64ff.v" 12 } } */
