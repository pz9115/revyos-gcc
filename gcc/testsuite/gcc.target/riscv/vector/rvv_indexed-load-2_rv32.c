/* { dg-do compile } */
/* { dg-options "-march=rv32gcv -mabi=ilp32d" } */

#include <riscv_vector.h>
#include <stddef.h>
#include "rvv-common.h"

/* Takes the scalar type STYPE, vector class VCLASS (int or float), and
   the e and m value.  */
#define VSLOADSTORE(STYPE, VCLASST, VCLASS, EM, MLEN, SEW, IEM, ISEW, OP)                   \
  void vsloadstore##ISEW##_##VCLASS##EM(size_t n, long stride, STYPE *x,                \
                                        STYPE *y, STYPE z, uint##ISEW##_t *index, word_type vl) {     \
    v##VCLASST##EM##_t vx, vy, vz;                                                      \
    vuint##IEM##_t vindex;                                                              \
    vbool##MLEN##_t mask;                                                               \
    mask = MSET (MLEN);                                                             \
    vindex = VULOAD(ISEW, IEM, index);                                                       \
    vy = VLOAD(VCLASS, SEW, EM, y);                                                         \
    vx = vloxe##i##ISEW##_v_##VCLASS##EM##_m(mask, vy, x, vindex, vl);                       \
    vz = v##OP##_vv_##VCLASS##EM (vx, vy, vl);                                                  \
    VSTORE(VCLASS, SEW, EM, x, vz);                                                         \
  }

RVV_INT_INDEX_TEST_ARG_RV32(VSLOADSTORE, add)
RVV_UINT_INDEX_TEST_ARG_RV32(VSLOADSTORE, add)
RVV_FLOAT_INDEX_TEST_ARG_RV32(VSLOADSTORE, fadd)

/* { dg-final { scan-assembler-times "vloxei8.v" 26 } } */
/* { dg-final { scan-assembler-times "vloxei16.v" 45 } } */
/* { dg-final { scan-assembler-times "vloxei32.v" 49 } } */
