/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

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
    vx = vlse##SEW##_v_##VCLASS##EM(x, stride, vl);                                      \
    vy = VLOAD(VCLASS, SEW, EM, y);                                               \
    vz = v##OP##_vv_##VCLASS##EM (vx, vy, vl);                                      \
    VSTORE(VCLASS, SEW, EM, x, vz);                                                \
  }

RVV_INT_TEST_ARG(VSLOADSTORE, add)
RVV_UINT_TEST_ARG(VSLOADSTORE, add)
RVV_FLOAT_TEST_ARG(VSLOADSTORE, fadd)

/* { dg-final { scan-assembler-times "vlse8.v" 14 } } */
/* { dg-final { scan-assembler-times "vlse16.v" 18 } } */
/* { dg-final { scan-assembler-times "vlse32.v" 15 } } */
/* { dg-final { scan-assembler-times "vlse64.v" 12 } } */
