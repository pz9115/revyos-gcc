/* { dg-do compile } */
/* { dg-options "-march=rv64gcvxtheadvdot -mabi=lp64d" } */

#include <riscv_vector.h>
#include <stddef.h>
#include "../../vector/rvv-common.h"

/* Takes the scalar type STYPE, vector class VCLASS (int or float), and
   the e and m value.  */
#define VPWADD(STYPE, VCLASST, VCLASS, EM, MLEN, WSTYPE, WEM, STYPEC, SEW, WSEW, OP)\
  void v##OP##VCLASS##EM(size_t n, STYPE *x, STYPE *y, WSTYPE *z, word_type vl) {\
    v##VCLASST##EM##_t vx, vy;                                               \
    v##VCLASST##WEM##_t vz;                                                  \
    vx = VLOAD(VCLASS, SEW, EM, x);                                          \
    vy = VLOAD(VCLASS, SEW, EM, y);                                          \
    vz = v##OP##_vv_##VCLASS##WEM (vx, vy, vl);                              \
    VSTORE(VCLASS, WSEW, WEM, z, vz);                                        \
  }                                                                          \
  void v##OP##VCLASS##EM##_s(size_t n, STYPE *x, STYPE y, WSTYPE *z, word_type vl) {\
    v##VCLASST##EM##_t vx, vy;                                               \
    v##VCLASST##WEM##_t vz;                                                  \
    vx = VLOAD(VCLASS, SEW, EM, x);                                          \
    vz = v##OP##_v##STYPEC##_##VCLASS##WEM (vx, y, vl);                      \
    VSTORE(VCLASS, WSEW, WEM, z, vz);                                        \
  }

#define VPWADDU(STYPE, VCLASST, VCLASS, EM, MLEN, WSTYPE, WEM, STYPEC, SEW, WSEW, OP)\
  void v##OP##u##EM(size_t n, STYPE *x, STYPE *y, WSTYPE *z, word_type vl) {         \
    v##VCLASST##EM##_t vx, vy;                                              \
    v##VCLASST##WEM##_t vz;                                                 \
    vx = VLOAD(VCLASS, SEW, EM, x);                                         \
    vy = VLOAD(VCLASS, SEW, EM, y);                                         \
    vz = v##OP##_vv_##VCLASS##WEM (vx, vy, vl);                             \
    VSTORE(VCLASS, WSEW, WEM, z, vz);                                       \
  }                                                                         \
  void v##OP##u##EM##_s(size_t n, STYPE *x, STYPE y, WSTYPE *z, word_type vl) {\
    v##VCLASST##EM##_t vx, vy;                                              \
    v##VCLASST##WEM##_t vz;                                                 \
    vx = VLOAD(VCLASS, SEW, EM, x);                                         \
    vz = v##OP##_v##STYPEC##_##VCLASS##WEM (vx, y, vl);                     \
    VSTORE(VCLASS, WSEW, WEM, z, vz);                                       \
  }

RVV_WINT_TEST_ARG(VPWADD, pwadd)
RVV_WUINT_TEST_ARG(VPWADDU, pwaddu)

/* { dg-final { scan-assembler-times "vpwadd.vv" 15 } } */
/* { dg-final { scan-assembler-times "vpwaddu.vv" 15 } } */
/* { dg-final { scan-assembler-times "vpwadd.vx" 15 } } */
/* { dg-final { scan-assembler-times "vpwaddu.vx" 15 } } */
