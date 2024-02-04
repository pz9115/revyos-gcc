;; Machine description for RISC-V Matrix operations.
;; Copyright (C) 2023 Free Software Foundation, Inc.
;; Contributed by T-HEAD.

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.

;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;; Iterator for hardware-supported integer modes.

(define_mode_iterator MMODES [
  M64QI M32HI M16SI M8DI
  M32HF M16SF M8DF
  M2x32HF M2x16SF M2x8DF])

(define_mode_iterator MMODESX1 [
  M64QI M32HI M16SI M8DI
  M32HF M16SF M8DF])

(define_mode_iterator MIMODES [
  M64QI M32HI M16SI M8DI])

(define_mode_iterator MIMODES1 [
  M64QI M32HI M16SI M8DI])

(define_mode_iterator MFMODES [
  M32HF M16SF M8DF
  M2x32HF M2x16SF M2x8DF])

(define_mode_iterator MFMODES2 [
  M32HF M16SF M8DF
  M2x32HF M2x16SF M2x8DF
  ])

(define_mode_iterator MFMODES1 [
  M32HF M16SF M8DF
  M2x32HF M2x16SF M2x8DF
  ])

(define_mode_iterator MFMODESX1 [
  M32HF M16SF M8DF])

(define_mode_iterator MFMODESX2 [
  M2x32HF M2x16SF M2x8DF])

;; Mode attributes for loads.
(define_mode_attr lsfmt [
  (M64QI "b") (M32HI "h") (M16SI "w") (M8DI "d")
  (M32HF "h") (M16SF "w") (M8DF "d")
  (M2x32HF "h") (M2x16SF "w") (M2x8DF "d")])

(define_mode_attr mfmt [
  (M64QI "b") (M32HI "h") (M16SI "s") (M8DI "d")
  (M32HF "h") (M16SF "s") (M8DF "d")
  (M2x32HF "h") (M2x16SF "s") (M2x8DF "d")])

(define_mode_attr mrnum [
  (M64QI "1") (M32HI "1") (M16SI "1") (M8DI "1")
  (M32HF "1") (M16SF "1") (M8DF "1")
  (M2x32HF "2") (M2x16SF "2") (M2x8DF "2")])

(define_mode_attr MSUBMODE [
  (M64QI "QI") (M32HI "HI") (M16SI "SI") (M8DI "DI")
  (M32HF "HF") (M16SF "SF") (M8DF "DF")
  (M2x32HF "HF") (M2x16SF "SF") (M2x8DF "DF")])

(define_mode_attr mtype [
  (M64QI "int8") (M32HI "int16") (M16SI "int32") (M8DI "int64")
  (M32HF "float16") (M16SF "float32") (M8DF "float64")
  (M2x32HF "float16") (M2x16SF "float32") (M2x8DF "float64")])

(define_int_iterator UNSPEC_MLD_MST_IT [UNSPEC_MLD_MST UNSPEC_MLD_MST_STREAM])
(define_int_attr mld_mst_pattern [(UNSPEC_MLD_MST "") (UNSPEC_MLD_MST_STREAM "s")])

(define_int_iterator UNSPEC_MMAQA_IT [UNSPEC_MMAQASS UNSPEC_MMAQAUU UNSPEC_MMAQAUS UNSPEC_MMAQASU])
(define_int_attr mmaqa_pattern [(UNSPEC_MMAQASS "") (UNSPEC_MMAQAUU "u") (UNSPEC_MMAQAUS "us") (UNSPEC_MMAQASU "su")])

(define_int_iterator UNSPEC_PMMAQA_IT [UNSPEC_PMMAQASS UNSPEC_PMMAQAUU UNSPEC_PMMAQAUS UNSPEC_PMMAQASU])
(define_int_attr pmmaqa_pattern [(UNSPEC_PMMAQASS "") (UNSPEC_PMMAQAUU "u") (UNSPEC_PMMAQAUS "us") (UNSPEC_PMMAQASU "su")])

(define_int_iterator PW_ITERATOR [UNSPEC_MADD UNSPEC_MSUB UNSPEC_MMUL UNSPEC_MMULH])
(define_int_attr pw_pattern [(UNSPEC_MADD "madd") (UNSPEC_MSUB "msub") (UNSPEC_MMUL "mmul") (UNSPEC_MMULH "mmulh")])
(define_int_attr pw_type [(UNSPEC_MADD "madd") (UNSPEC_MSUB "madd") (UNSPEC_MMUL "mmul") (UNSPEC_MMULH "mmul")])

(define_int_iterator CLIP_ITERATOR [UNSPEC_MN4CLIP UNSPEC_MN4CLIPU])
(define_int_attr clip_pattern [(UNSPEC_MN4CLIP "mn4clip") (UNSPEC_MN4CLIPU "mn4clipu")])

;; Matrix cfg

(define_expand "matrix_mcfg"
  [(parallel
    [(set (reg:QI MSIZE_M_REGNUM)
      (truncate:QI
	(match_operand:SI 1 "reg_or_const_int_operand")))
    (set (reg:QI MSIZE_N_REGNUM)
      (truncate:QI
	(lshiftrt:SI
	  (match_dup 1)
	(const_int 8))))
    (set (reg:HI MSIZE_K_REGNUM)
      (truncate:HI
	(lshiftrt:SI
	  (match_dup 1)
	(const_int 16))))
    (set (match_operand:SI 0 "register_operand")
      (unspec_volatile:SI
        [(match_dup 1)]
      UNSPEC_SET_MSIZE))])]
  "TARGET_MATRIX"
  {
    if (!REG_P (operands[1]))
      operands[1] = force_reg (SImode, operands[1]);
  })

(define_insn "*matrix_mcfg"
  [(set (reg:QI MSIZE_M_REGNUM)
    (truncate:QI
      (match_operand:SI 1 "reg_or_const_int_operand" "r")))
  (set (reg:QI MSIZE_N_REGNUM)
    (truncate:QI
      (lshiftrt:SI
	(match_dup 1)
      (const_int 8))))
  (set (reg:HI MSIZE_K_REGNUM)
    (truncate:HI
      (lshiftrt:SI
	(match_dup 1)
      (const_int 16))))
  (set (match_operand:SI 0 "register_operand" "=r")
    (unspec_volatile:SI
      [(match_dup 1)]
    UNSPEC_SET_MSIZE))]
  "TARGET_MATRIX"
  {
    return "mcfg\t%0,%1";
  }
  [(set_attr "type" "mset")
  (set_attr "mode" "none")
  (set_attr "emode" "SI")])

(define_insn "matrix_mcfg_m"
  [(set (reg:QI MSIZE_M_REGNUM)
    (unspec_volatile:QI
      [(match_operand:QI 1 "reg_or_const_int_operand" " r,I")]
    UNSPEC_SET_MSIZE))
  (set (match_operand:SI 0 "register_operand" "=r,r")
    (unspec_volatile:SI
      [(reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return REG_P (operands[1]) ? "mcfgm\t%0,%1" :
    "mcfgmi\t%0,%1";
  }
  [(set_attr "type" "mset")
  (set_attr "mode" "none")
  (set_attr "emode" "SI")])

(define_insn "matrix_mcfg_n"
  [(set (reg:QI MSIZE_N_REGNUM)
    (unspec_volatile:QI
      [(match_operand:QI 1 "reg_or_const_int_operand" " r,I")]
    UNSPEC_SET_MSIZE))
  (set (match_operand:SI 0 "register_operand" "=r,r")
    (unspec_volatile:SI
      [(reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return REG_P (operands[1]) ? "mcfgn\t%0,%1" :
    "mcfgni\t%0,%1";
  }
  [(set_attr "type" "mset")
  (set_attr "mode" "none")
  (set_attr "emode" "SI")])

(define_insn "matrix_mcfg_k"
  [(set (reg:HI MSIZE_K_REGNUM)
    (unspec_volatile:HI
      [(match_operand:HI 1 "reg_or_const_int_operand" " r,I")]
    UNSPEC_SET_MSIZE))
  (set (match_operand:SI 0 "register_operand" "=r,r")
    (unspec_volatile:SI
      [(reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return REG_P (operands[1]) ? "mcfgk\t%0,%1" :
    "mcfgki\t%0,%1";
  }
  [(set_attr "type" "mset")
  (set_attr "mode" "none")
  (set_attr "emode" "SI")])

;; Matrix mrelease

(define_insn "matrix_mrelease"
  [(set (reg:QI MSIZE_M_REGNUM)
    (unspec_volatile:QI
      [(const_int 0)]
    UNSPEC_MRELEASE))
  (set (reg:QI MSIZE_N_REGNUM)
    (unspec_volatile:QI
      [(const_int 0)]
    UNSPEC_MRELEASE))
  (set (reg:HI MSIZE_K_REGNUM)
    (unspec_volatile:HI
      [(const_int 0)]
    UNSPEC_MRELEASE))]
  "TARGET_MATRIX"
  {
    return "mrelease";
  }
  [(set_attr "type" "mset")
  (set_attr "mode" "none")
  (set_attr "emode" "SI")])

(define_expand "matrix_mundefined_<mode>"
  [(clobber (match_operand:MMODES 0 "register_operand"))]
  "TARGET_MATRIX"
{
})

;; Matrix zero

(define_insn "matrix_mzero_<MMODES:mode>"
  [(set (match_operand:MMODES 0 "register_operand" "=xr")
      (unspec_volatile:MMODES
	[(const_int 0)]
      UNSPEC_MZERO))]
  "TARGET_MATRIX"
  {
    if (riscv_matrix_x2_mode (GET_MODE (operands[0])))
      return "mzero\t%0\n\tmzero\t%N0";
    return "mzero\t%0";
  }
  [(set_attr "type" "mmov")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

;; Matrix convert, helper for matrix on rv32.

(define_insn "matrix_mconvert_mm_<MIMODES:mode>_<MIMODES1:mode>"
  [(set (match_operand:MIMODES 0 "register_operand" "=xr")
    (unspec:MIMODES
      [(match_operand:MIMODES1 1 "register_operand" "0")]
    UNSPEC_MCONVERT))]
  "TARGET_MATRIX"
  "")

;; Matrix load

(define_expand "matrix_m<mld_mst_pattern>ld_<MMODESX1:mode>_<P:mode>"
  [(set (match_operand:MMODESX1 0 "register_operand")
    (unspec:MMODESX1
      [(unspec:MMODESX1
	[(mem:MMODESX1 (match_operand:P 1 "register_operand"))
	(match_operand:P 2 "reg_or_imm_operand")]
      UNSPEC_MLD_MST_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    if (!REG_P (operands[2]))
      operands[2] = force_reg (Pmode, operands[2]);
  })

(define_insn "*matrix_m<mld_mst_pattern>ld_<MMODESX1:mode>_<P:mode>"
  [(set (match_operand:MMODESX1 0 "register_operand" "=xr")
    (unspec:MMODESX1
      [(unspec:MMODESX1
	[(match_operand:MMODESX1 1 "memory_operand" "m")
	(match_operand:P 2 "register_operand" "r")]
      UNSPEC_MLD_MST_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return "m<mld_mst_pattern>ld<lsfmt>\t%0,%2,%1";
  }
  [(set_attr "type" "mload")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_m<mld_mst_pattern>ld_x2_<MFMODESX2:mode>_<P:mode>"
  [(set (match_operand:MFMODESX2 0 "register_operand")
    (unspec:MFMODESX2
      [(unspec:MFMODESX2
	[(mem:MFMODESX2 (match_operand:P 1 "register_operand"))
	(mem:MFMODESX2 (match_operand:P 2 "register_operand"))
	(match_operand:P 3 "reg_or_imm_operand")]
      UNSPEC_MLD_MST_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    if (!REG_P (operands[3]))
      operands[3] = force_reg (Pmode, operands[3]);
  })

(define_insn "*matrix_m<mld_mst_pattern>ld_x2_<MFMODESX2:mode>_<P:mode>"
  [(set (match_operand:MFMODESX2 0 "register_operand" "=xr")
    (unspec:MFMODESX2
      [(unspec:MFMODESX2
	[(match_operand:MFMODESX2 1 "memory_operand" "m")
	  (match_operand:MFMODESX2 2 "memory_operand" "m")
	  (match_operand:P 3 "register_operand" "r")]
	UNSPEC_MLD_MST_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return "m<mld_mst_pattern>ld<lsfmt>\t%0,%3,%1\n\tm<mld_mst_pattern>ld<lsfmt>\t%N0,%3,%2";
  }
  [(set_attr "type" "mloadx2")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])


;; Matrix store

(define_expand "matrix_m<mld_mst_pattern>st_<MMODESX1:mode>_<P:mode>"
  [(set (mem:MMODESX1 (match_operand:P 1 "register_operand"))
    (unspec:MMODESX1
      [(unspec:MMODESX1
	[(match_operand:MMODESX1 0 "register_operand")
	(match_operand:P 2 "reg_or_imm_operand")]
      UNSPEC_MLD_MST_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
  UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    if (!REG_P (operands[2]))
      operands[2] = force_reg (Pmode, operands[2]);
  })

(define_insn "*matrix_m<mld_mst_pattern>st_<MMODESX1:mode>_<P:mode>"
  [(set (match_operand:MMODESX1 1 "memory_operand" "+m")
    (unspec:MMODESX1
      [(unspec:MMODESX1
	[(match_operand:MMODESX1 0 "register_operand" "xr")
	(match_operand:P 2 "register_operand" "r")]
      UNSPEC_MLD_MST_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return "m<mld_mst_pattern>st<lsfmt>\t%0,%2,%1";
  }
  [(set_attr "type" "mstore")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_m<mld_mst_pattern>st_x2_<MFMODESX2:mode>_<P:mode>"
  [(parallel
    [(set (mem:MFMODESX2 (match_operand:P 1 "register_operand"))
      (unspec:MFMODESX2
	[(unspec:MFMODESX2
	  [(match_operand:MFMODESX2 0 "register_operand")
	  (match_operand:P 3 "reg_or_imm_operand")]
	UNSPEC_MLD_MST_IT)
	(reg:QI MSIZE_M_REGNUM)
	(reg:QI MSIZE_N_REGNUM)
	(reg:HI MSIZE_K_REGNUM)]
      UNSPEC_USE_MSIZE))
    (set (mem:MFMODESX2 (match_operand:P 2 "register_operand"))
      (unspec:MFMODESX2
	[(unspec:MFMODESX2
	  [(match_dup 0)
	  (match_dup 3)]
	UNSPEC_MLD_MST_IT)
	(reg:QI MSIZE_M_REGNUM)
	(reg:QI MSIZE_N_REGNUM)
	(reg:HI MSIZE_K_REGNUM)]
      UNSPEC_USE_MSIZE))])]
  "TARGET_MATRIX"
  {
    if (!REG_P (operands[3]))
      operands[3] = force_reg (Pmode, operands[3]);
  })

(define_insn "*matrix_m<mld_mst_pattern>st_x2_<MFMODESX2:mode>_<P:mode>"
  [(set (match_operand:MFMODESX2 1 "memory_operand" "+m")
      (unspec:MFMODESX2
	[(unspec:MFMODESX2
	  [(match_operand:MFMODESX2 0 "register_operand" "xr")
	  (match_operand:P 3 "register_operand" "r")]
	UNSPEC_MLD_MST_IT)
	(reg:QI MSIZE_M_REGNUM)
	(reg:QI MSIZE_N_REGNUM)
	(reg:HI MSIZE_K_REGNUM)]
      UNSPEC_USE_MSIZE))
    (set (match_operand:MFMODESX2 2 "memory_operand" "+m")
      (unspec:MFMODESX2
	[(unspec:MFMODESX2
	  [(match_dup 0)
	  (match_dup 3)]
	UNSPEC_MLD_MST_IT)
	(reg:QI MSIZE_M_REGNUM)
	(reg:QI MSIZE_N_REGNUM)
	(reg:HI MSIZE_K_REGNUM)]
      UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return "m<mld_mst_pattern>st<lsfmt>\t%0,%3,%1\n\tm<mld_mst_pattern>st<lsfmt>\t%N0,%3,%2";
  }
  [(set_attr "type" "mstorex2")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])


;; Matrix read mlenb

(define_expand "read_mlenb"
  [(match_operand 0 "register_operand")]
  "TARGET_MATRIX"
{
  rtx imm = gen_int_mode (UNITS_PER_M_REG, Pmode);
  emit_move_insn (operands[0], imm);
  DONE;
})


;; Matrix whole register

(define_expand "mov<mode>"
  [(set (match_operand:MMODES 0 "reg_or_mem_operand")
      (match_operand:MMODES 1 "reg_or_mem_operand"))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*mov<mode>"
  [(set (match_operand:MMODES 0 "reg_or_mem_operand"  "=xr,xr,m")
      (match_operand:MMODES 1 "reg_or_mem_operand"  "xr,m,xr"))]
  "TARGET_MATRIX"
  "@
   mmov.mm\t%0,%1
   mld<mrnum>m<lsfmt>\t%0,%1
   mst<mrnum>m<lsfmt>\t%1,%0"
  [(set_attr "type" "mmov,mload,mstore")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])


;; whole matrix load/store

(define_expand "matrix_mldm_<MMODES:mode>_<P:mode>"
  [(set (match_operand:MMODES 0 "register_operand")
    (unspec:MMODES
	[(mem:MMODES (match_operand:P 1 "register_operand"))]
      UNSPEC_WHOLE_SL))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_mldm_<MMODES:mode>"
  [(set (match_operand:MMODES 0 "register_operand" "=xr")
      (unspec:MMODES
	[(match_operand:MMODES 1 "memory_operand" "m")]
      UNSPEC_WHOLE_SL))]
  "TARGET_MATRIX"
  {
    return "mld<mrnum>m<lsfmt>\t%0,%1";
  }
  [(set_attr "type" "mload")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_mstm_<MMODES:mode>_<P:mode>"
  [(set (mem:MMODES (match_operand:P 1 "register_operand"))
    (unspec:MMODES
	[(match_operand:MMODES 0 "register_operand")]
      UNSPEC_WHOLE_SL))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_mstm_<MMODES:mode>"
  [(set (match_operand:MMODES 1 "memory_operand" "+m")
      (unspec:MMODES
	[(match_operand:MMODES 0 "register_operand" "xr")]
      UNSPEC_WHOLE_SL))]
  "TARGET_MATRIX"
  {
    return "mst<mrnum>m<lsfmt>\t%0,%1";
  }
  [(set_attr "type" "mstore")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])


;; Matrix mov

(define_expand "matrix_mmov_mm_<MMODESX1:mode>"
  [(set (match_operand:MMODESX1 0 "register_operand")
    (unspec:MMODESX1
      [(match_operand:MMODESX1 1 "register_operand")]
    UNSPEC_MMOVE))]
  "TARGET_MATRIX"
 {
 })

(define_insn "*matrix_mmov_mm_<MMODESX1:mode>"
  [(set (match_operand:MMODESX1 0 "register_operand" "=xr")
    (unspec:MMODESX1
      [(match_operand:MMODESX1 1 "register_operand" "xr")]
    UNSPEC_MMOVE))]
  "TARGET_MATRIX"
 {
    return "mmov.mm\t%0,%1";
 }
  [(set_attr "type" "mmov")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_mmov_mv_<MMODESX1:mode>"
  [(set (match_operand:MMODESX1 0 "register_operand")
    (unspec:MMODESX1
      [(match_operand:MMODESX1 1 "register_operand")
      (match_operand:QI 2 "reg_or_imm_operand")]
    UNSPEC_MMOVE))]
  "TARGET_MATRIX"
  {
  })

 (define_insn "*matrix_mmov_mv_<MMODESX1:mode>"
  [(set (match_operand:MMODESX1 0 "register_operand" "=xr,xr")
    (unspec:MMODESX1
      [(match_operand:MMODESX1 1 "register_operand" "xr,xr")
      (match_operand:QI 2 "reg_or_imm_operand" "xi,I")]
    UNSPEC_MMOVE))]
  "TARGET_MATRIX"
  {
    if (immediate_operand (operands[2], GET_MODE (operands[2]))){
      return "mmov.mv.i\t%0,%1[%2]";
    }

    return "mmov.mv.x\t%0,%1[%2]";
  }
  [(set_attr "type" "mmov")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_mmov_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(match_operand:<MSUBMODE> 2 "reg_or_imm_operand")
      (match_operand:QI 3 "reg_or_imm_operand")
      (match_operand:MIMODES 1 "register_operand")]
    UNSPEC_MMOVE))]
  "TARGET_MATRIX"
 {
    if (!REG_P (operands[2]))
      operands[2] = force_reg (<MSUBMODE>mode, operands[2]);

    if (!REG_P (operands[3]))
      operands[3] = force_reg (QImode, operands[3]);
 })

(define_insn "*matrix_mmov_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:MIMODES 0 "register_operand" "=xr")
    (unspec:MIMODES
      [(match_operand:<MSUBMODE> 2 "register_operand")
      (match_operand:QI 3 "register_operand")
      (match_operand:MIMODES 1 "register_operand")]
    UNSPEC_MMOVE))]
  "TARGET_MATRIX"
 {
    return "mmov<MIMODES:lsfmt>.m.x\t%0,%2,%3";
 }
  [(set_attr "type" "mmov")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_mmov_xm_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:<MSUBMODE> 0 "register_operand")
    (unspec:<MSUBMODE>
      [(match_operand:MIMODES 1 "register_operand")
      (match_operand:QI 2 "reg_or_imm_operand")]
    UNSPEC_MMOVE))]
  "TARGET_MATRIX"
 {
    if (!REG_P (operands[1]))
      operands[1] = force_reg (<MSUBMODE>mode, operands[1]);

    if (!REG_P (operands[2]))
      operands[2] = force_reg (QImode, operands[2]);
 })

(define_insn "*matrix_mmov_xm_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:<MSUBMODE> 0 "register_operand" "=r")
    (unspec:<MSUBMODE>
      [(match_operand:MIMODES 1 "register_operand" "xr")
      (match_operand:QI 2 "register_operand" "r")]
    UNSPEC_MMOVE))]
  "TARGET_MATRIX"
 {
    return "mmov<MIMODES:lsfmt>.x.m\t%0,%1,%2";
 }
  [(set_attr "type" "mmov")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])


;; Matrix mmov x2

(define_expand "matrix_mmov_mm_x2_<MFMODESX2:mode>_<MFMODESX1:mode>"
    [(set (match_operand:MFMODESX2 0 "register_operand")
      (unspec:MFMODESX2
	[(match_operand:MFMODESX1 1 "register_operand")
	(match_operand:MFMODESX1 2 "register_operand")]
      UNSPEC_MMOVE))]
  "TARGET_MATRIX"
 {
 })

(define_insn "*matrix_mmov_mm_x2_<MFMODESX2:mode>_<MFMODESX1:mode>"
  [(set (match_operand:MFMODESX2 0 "register_operand" "=xr")
    (unspec:MFMODESX2
      [(match_operand:MFMODESX1 1 "register_operand" "xr")
      (match_operand:MFMODESX1 2 "register_operand" "xr")]
    UNSPEC_MMOVE))]
  "TARGET_MATRIX"
 {
    return "mmov.mm\t%0,%1\n\tmmov.mm\t%N0,%2";
 }
  [(set_attr "type" "mmovx2")
  (set_attr "mode" "none")
  (set_attr "emode" "<MFMODESX2:MSUBMODE>")])


;; Matrix mdup

(define_expand "matrix_mdup_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(match_operand:<MSUBMODE> 1 "reg_or_imm_operand")]
    UNSPEC_MDUP))]
  "TARGET_MATRIX"
 {
    if (!REG_P (operands[1]))
      operands[1] = force_reg (<MSUBMODE>mode, operands[1]);
 })

(define_insn "*matrix_mdup_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:MIMODES 0 "register_operand" "=xr")
    (unspec:MIMODES
      [(match_operand:<MSUBMODE> 1 "register_operand" "r")]
    UNSPEC_MDUP))]
  "TARGET_MATRIX"
 {
    return "mdup<MIMODES:lsfmt>.m.x\t%0,%1";
 }
  [(set_attr "type" "mmov")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

;; Matrix integer pointwise arithmetic

(define_expand "matrix_<pw_pattern>_mm_<MIMODES:mode>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand")
	(match_operand:MIMODES 2 "register_operand")]
      PW_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_<pw_pattern>_mm_<MIMODES:mode>"
  [(set (match_operand:MIMODES 0 "register_operand" "=xr")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand" "xr")
	(match_operand:MIMODES 2 "register_operand" "xr")]
      PW_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
      return "<pw_pattern>.<MIMODES:mfmt>.mm\t%0,%1,%2";
  }
  [(set_attr "type" "<pw_type>")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_<pw_pattern>_mv_<MIMODES:mode>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand")
	(match_operand:MIMODES 2 "register_operand")
	(match_operand:QI 3 "reg_or_imm_operand")]
      PW_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_<pw_pattern>_mv_<MIMODES:mode>"
  [(set (match_operand:MIMODES 0 "register_operand" "=xr,xr")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand" "xr,xr")
	(match_operand:MIMODES 2 "register_operand" "xr,xr")
	(match_operand:QI 3 "reg_or_imm_operand" "xi,I")]
      PW_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    if (immediate_operand (operands[3], GET_MODE (operands[3]))){
      return "<pw_pattern>.<MIMODES:mfmt>.mv.i\t%0,%1,%2[%3]";
    }

    return "<pw_pattern>.<MIMODES:mfmt>.mv.x\t%0,%1,%2[%3]";
  }
  [(set_attr "type" "<pw_type>")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_<pw_pattern>_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand")
	(match_operand:<MSUBMODE> 2 "reg_or_imm_operand")]
      PW_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    if (!REG_P (operands[2]))
      operands[2] = force_reg (<MSUBMODE>mode, operands[2]);
  })

(define_insn "*matrix_<pw_pattern>_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:MIMODES 0 "register_operand" "=xr")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand" "xr")
	(match_operand:<MSUBMODE> 2 "register_operand" "xi")]
      PW_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return "<pw_pattern>.<MIMODES:mfmt>.mx\t%0,%1,%2";
  }
  [(set_attr "type" "<pw_type>")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

;; Matrix sra

(define_expand "matrix_msra_mm_<MIMODES:mode>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand")
	(match_operand:MIMODES 2 "register_operand")]
      UNSPEC_MSRA)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_msra_mm_<MIMODES:mode>"
  [(set (match_operand:MIMODES 0 "register_operand" "=xr")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand" "xr")
	(match_operand:MIMODES 2 "register_operand" "xr")]
      UNSPEC_MSRA)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
      return "msra.<MIMODES:mfmt>.mm\t%0,%1,%2";
  }
  [(set_attr "type" "mshift")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_msra_mv_<MIMODES:mode>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand")
	(match_operand:MIMODES 2 "register_operand")
	(match_operand:QI 3 "reg_or_imm_operand")]
      UNSPEC_MSRA)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  }
  [(set_attr "type" "mshift")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_insn "*matrix_msra_mv_<MIMODES:mode>"
  [(set (match_operand:MIMODES 0 "register_operand" "=xr,xr")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand" "xr,xr")
	(match_operand:MIMODES 2 "register_operand" "xr,xr")
	(match_operand:QI 3 "reg_or_imm_operand" "xi,I")]
      UNSPEC_MSRA)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    if (immediate_operand (operands[3], GET_MODE (operands[3]))){
      return "msra.<MIMODES:mfmt>.mv.i\t%0,%1,%2[%3]";
    }

    return "msra.<MIMODES:mfmt>.mv.x\t%0,%1,%2[%3]";
  }
  [(set_attr "type" "mshift")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_msra_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand")
	(match_operand:<MSUBMODE> 2 "reg_or_imm_operand")]
      UNSPEC_MSRA)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    if (!REG_P (operands[2]))
      operands[2] = force_reg (<MSUBMODE>mode, operands[2]);
  }
  [(set_attr "type" "mshift")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_insn "*matrix_msra_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:MIMODES 0 "register_operand" "=xr")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand" "xr")
	(match_operand:<MSUBMODE> 2 "register_operand" "xi")]
      UNSPEC_MSRA)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return "msra.<MIMODES:mfmt>.mx\t%0,%1,%2";
  }
  [(set_attr "type" "mshift")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])


;; Matrix mn4clip/mn4clipu

(define_expand "matrix_<clip_pattern>_mm_<MIMODES:mode>"
  [(set (match_operand:M64QI 0 "register_operand")
    (unspec:M64QI
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand")
	(match_operand:MIMODES 2 "register_operand")]
      CLIP_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_<clip_pattern>_mm_<MIMODES:mode>"
  [(set (match_operand:M64QI 0 "register_operand" "=xr")
    (unspec:M64QI
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand" "xr")
	(match_operand:MIMODES 2 "register_operand" "xr")]
      CLIP_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
      return "<clip_pattern>.<MIMODES:mfmt>.mm\t%0,%1,%2";
  }
  [(set_attr "type" "mn4clip")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_<clip_pattern>_mv_<MIMODES:mode>"
  [(set (match_operand:M64QI 0 "register_operand")
    (unspec:M64QI
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand")
	(match_operand:MIMODES 2 "register_operand")
	(match_operand:QI 3 "reg_or_imm_operand")]
      CLIP_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_<clip_pattern>_mv_<MIMODES:mode>"
  [(set (match_operand:M64QI 0 "register_operand" "=xr,xr")
    (unspec:M64QI
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand" "xr,xr")
	(match_operand:MIMODES 2 "register_operand" "xr,xr")
	(match_operand:QI 3 "reg_or_imm_operand" "xi,I")]
      CLIP_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    if (immediate_operand (operands[3], GET_MODE (operands[3]))){
      return "<clip_pattern>.<MIMODES:mfmt>.mv.i\t%0,%1,%2[%3]";
    }

    return "<clip_pattern>.<MIMODES:mfmt>.mv.x\t%0,%1,%2[%3]";
  }
  [(set_attr "type" "mn4clip")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

(define_expand "matrix_<clip_pattern>_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:M64QI 0 "register_operand")
    (unspec:M64QI
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand")
	(match_operand:<MSUBMODE> 2 "reg_or_imm_operand")]
      CLIP_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    if (!REG_P (operands[2]))
      operands[2] = force_reg (<MSUBMODE>mode, operands[2]);
  })

(define_insn "*matrix_<clip_pattern>_mx_<MIMODES:mode>_<MIMODES:mtype>"
  [(set (match_operand:M64QI 0 "register_operand" "=xr")
    (unspec:M64QI
      [(unspec:MIMODES
	[(match_operand:MIMODES 1 "register_operand" "xr")
	(match_operand:<MSUBMODE> 2 "register_operand" "xi")]
      CLIP_ITERATOR)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return "<clip_pattern>.<MIMODES:mfmt>.mx\t%0,%1,%2";
  }
  [(set_attr "type" "mn4clip")
  (set_attr "mode" "none")
  (set_attr "emode" "<MSUBMODE>")])

;; Matrix fmmacc

(define_expand "matrix_fmmacc_<MFMODES:mode>_<MFMODES1:mode>_<MFMODES2:mode>"
  [(set (match_operand:MFMODES 0 "register_operand")
    (unspec:MFMODES
      [(unspec:MFMODES
	[(plus:MFMODES
	  (mult:MFMODES
	    (match_operand:MFMODES1 2 "register_operand")
	    (match_operand:MFMODES2 3 "register_operand"))
	  (match_operand:MFMODES 1 "register_operand"))]
	UNSPEC_FMMACC)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_fmmacc_<MFMODES:mode>_<MFMODES1:mode>_<MFMODES2:mode>"
  [(set (match_operand:MFMODES 0 "register_operand" "=&xr")
    (unspec:MFMODES
      [(unspec:MFMODES
	[(plus:MFMODES
	    (mult:MFMODES
	      (match_operand:MFMODES1 2 "register_operand" "xr")
	      (match_operand:MFMODES2 3 "register_operand" "xr"))
	    (match_operand:MFMODES 1 "register_operand" "0"))]
      UNSPEC_FMMACC)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return "fmmacc.<MFMODES:mfmt>\t%0,%3,%2";
  }
  [(set_attr "type" "mfmul")
  (set_attr "mode" "none")
  (set_attr "emode" "<MFMODES:MSUBMODE>")])


;; Matrix fwmmacc

(define_expand "matrix_fwmmacc_<MFMODES:mode>_<MFMODES1:mode>_<MFMODES2:mode>"
  [(set (match_operand:MFMODES 0 "register_operand")
    (unspec:MFMODES
      [(unspec:MFMODES
	[(plus:MFMODES
	  (mult:MFMODES
	    (match_operand:MFMODES1 2 "register_operand")
	    (match_operand:MFMODES2 3 "register_operand"))
	  (match_operand:MFMODES 1 "register_operand"))]
	UNSPEC_FWMMACC)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_fwmmacc_<MFMODES:mode>_<MFMODES1:mode>_<MFMODES2:mode>"
  [(set (match_operand:MFMODES 0 "register_operand" "=&xr")
    (unspec:MFMODES
      [(unspec:MFMODES
	[(plus:MFMODES
	    (mult:MFMODES
	      (match_operand:MFMODES1 2 "register_operand" "xr")
	      (match_operand:MFMODES2 3 "register_operand" "xr"))
	    (match_operand:MFMODES 1 "register_operand" "0"))]
      UNSPEC_FWMMACC)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
    return "fwmmacc.<MFMODES1:mfmt>\t%0,%2,%3";
  }
  [(set_attr "type" "mfmul")
  (set_attr "mode" "none")
  (set_attr "emode" "<MFMODES:MSUBMODE>")])


;; Matrix mmaqa

(define_expand "matrix_mmaqa<mmaqa_pattern>_<MIMODES:mode>_<MIMODES1:mode>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(plus:MIMODES
	    (mult:MIMODES
	      (match_operand:MIMODES1 2 "register_operand")
	      (match_operand:MIMODES1 3 "register_operand"))
	    (match_operand:MIMODES 1 "register_operand"))]
      UNSPEC_MMAQA_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_mmaqa<mmaqa_pattern>_<MIMODES:mode>_<MIMODES1:mode>"
  [(set (match_operand:MIMODES 0 "register_operand" "=&xr")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(plus:MIMODES
	    (mult:MIMODES
	      (match_operand:MIMODES1 2 "register_operand" "xr")
	      (match_operand:MIMODES1 3 "register_operand" "xr"))
	    (match_operand:MIMODES 1 "register_operand" "0"))]
      UNSPEC_MMAQA_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
      return "mmaqa<mmaqa_pattern>.<MIMODES1:mfmt>\t%0,%2,%3";
  }
  [(set_attr "type" "mmul")
  (set_attr "mode" "none")
  (set_attr "emode" "<MIMODES:MSUBMODE>")])


;; Matrix pmmaqa

(define_expand "matrix_pmmaqa<pmmaqa_pattern>_<MIMODES:mode>_<MIMODES1:mode>"
  [(set (match_operand:MIMODES 0 "register_operand")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(plus:MIMODES
	    (mult:MIMODES
	      (match_operand:MIMODES1 2 "register_operand")
	      (match_operand:MIMODES1 3 "register_operand"))
	    (match_operand:MIMODES 1 "register_operand"))]
      UNSPEC_PMMAQA_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
  })

(define_insn "*matrix_pmmaqa<pmmaqa_pattern>_<MIMODES:mode>_<MIMODES1:mode>"
  [(set (match_operand:MIMODES 0 "register_operand" "=&xr")
    (unspec:MIMODES
      [(unspec:MIMODES
	[(plus:MIMODES
	    (mult:MIMODES
	      (match_operand:MIMODES1 2 "register_operand" "xr")
	      (match_operand:MIMODES1 3 "register_operand" "xr"))
	    (match_operand:MIMODES 1 "register_operand" "0"))]
      UNSPEC_PMMAQA_IT)
      (reg:QI MSIZE_M_REGNUM)
      (reg:QI MSIZE_N_REGNUM)
      (reg:HI MSIZE_K_REGNUM)]
    UNSPEC_USE_MSIZE))]
  "TARGET_MATRIX"
  {
      return "pmmaqa<pmmaqa_pattern>.<MIMODES1:mfmt>\t%0,%2,%3";
  }
  [(set_attr "type" "mmul")
  (set_attr "mode" "none")
  (set_attr "emode" "<MIMODES:MSUBMODE>")])
