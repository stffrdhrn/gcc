;; Machine description for SMH

(define_attr "length" "" (const_int 2))

(define_constraint "W"
 "A register inderect memory operand"
 (and (match_code "mem")
      (match_test "REG_P (XEXP (op, 0))
		   && REGNO_OK_FOR_BASE_P (REGNO (op))")))

(define_predicate "smh_general_movsrc_operand"
  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const")
{
  if (GET_CODE (op) == MEM && GET_CODE (XEXP (op, 0)) == LABEL_REF)
    return 1;

  return general_operand (op, mode);
})

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	 (plus:SI
	  (match_operand:SI 1 "register_operand" "r")
	  (match_operand:SI 2 "general_operand" "r")))]
  ""
  "add.l\t%0, %1, %2")

;; Move

(define_insn "movsi_push"
  [(set (mem:SI (pre_dec:SI (reg:SI 1)))
	(match_operand:SI 0 "register_operand" "r"))]
  ""
  "push\t$sp, %0")

(define_insn "movsi_pop"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mem:SI (post_inc:SI (reg:SI 1))))]
  ""
  "pop\t%0, $sp")

(define_expand "movsi"
  [(set (match_operand:SI 0 "general_operand" "")
	(match_operand:SI 1 "general_operand" ""))]
  ""
  "
{
  if (! (reload_in_progress || reload_completed))
  {
    if (MEM_P (operands[0]))
      {
	operands[1] = force_reg (SImode, operands[1]);
	if (MEM_P (XEXP (operands[0], 0)))
	  operands[0] = gen_rtx_MEM (SImode, force_reg (SImode, XEXP (operands[0], 0)));
      }
      else
	if (MEM_P (operands[1])
	    && MEM_P (XEXP (operands[1], 0)))
          operands[1] = gen_rtx_MEM (SImode, force_reg (SImode, XEXP (operands[1], 0)));
  }
}")

(define_insn "*loadsi_offset"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mem:SI (plus:SI
	 	  (match_operand:SI 1 "register_operand" "r")
	  	  (match_operand:SI 2 "immediate_operand" "i"))))]
  ""
  "ldo.l\t%0, %2(%1)")

(define_insn "*storesi_offset"
  [(set (mem:SI (plus:SI
	 	  (match_operand:SI 1 "register_operand" "r")
	  	  (match_operand:SI 2 "immediate_operand" "i")))
	(match_operand:SI 0 "register_operand" "r"))]
  ""
  "sto.l\t%2(%1), %0")

(define_insn "*movsi"
  [(set (match_operand:SI 0 "nonimmediate_operand" "=r,r,W,m,r,r")
	(match_operand:SI 1 "smh_general_movsrc_operand" "r,i,r,r,W,m"))]
  "register_operand (operands[0], SImode)
   || register_operand (operands[1], SImode)"
  "@
  mov\t%0, %1
  ldi.l\t%0, %1
  st.l\t%0, %1
  sta.l\t%0, %1
  ld.l\t%0, %1
  lda.l\t%0, %1")

(define_expand "movhi"
  [(set (match_operand:HI 0 "nonimmediate_operand" "")
	(match_operand:HI 1 "general_operand" ""))]
  ""
  "
{
  if (MEM_P (operands[0]))
    operands[1] = force_reg (HImode, operands[1]);
}")

(define_insn "*movhi"
  [(set (match_operand:HI 0 "nonimmediate_operand" "=r")
	(match_operand:HI 1 "general_operand" "r"))]
  "register_operand (operands[0], HImode)
   || register_operand (operands[1], HImode)"
  "mov\t%0, %1")

;; Jump and Call instructions

(define_expand "call"
  [(call (match_operand:QI 0 "memory_operand" "")
	 (match_operand 1 "general_operand" ""))]
  ""
{
  gcc_assert (GET_CODE (operands[0]) == MEM);
})

(define_insn "*call_indirect"
  [(call (mem:QI (match_operand:SI 0 "register_operand" "r"))
	 (match_operand 1 "" ""))]
  ""
  "jsr\t%0")

(define_insn "*call"
  [(call (mem:QI (match_operand:SI 0 "immediate_operand" "i"))
	 (match_operand 1 "" ""))]
  ""
  "jsra\t%0")

(define_expand "call_value"
  [(set (match_operand 0 "" "")
	  (call (match_operand:QI 1 "memory_operand" "")
		(match_operand 2 "" "")))]
  ""
{
  gcc_assert (GET_CODE (operands[1]) == MEM);
})

(define_insn "*call_value_indirect"
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:QI (match_operand:SI 1 "register_operand" "r"))
	      (match_operand 2 "" "")))]
  ""
  "jsr\t%1")

(define_insn "*call_value"
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:QI (match_operand:SI 1 "immediate_operand" "i"))
	      (match_operand 2 "" "")))]
  ""
  "jsra\t%1")

(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "general_operand" "r"))]
  "true"
  "jmp %0")

(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0 "" "")))]
  ""
  "jmp\t%l0%#")

;; Calling conventions

(define_expand "prologue"
  [(clobber (const_int 0))]
  ""
  "
{
  smh_expand_prologue ();
  DONE;
}
")

(define_expand "epilogue"
  [(return)]
  ""
  "
{
  smh_expand_epilogue ();
  DONE;
}
")

(define_insn "returner"
  [(return)]
  "reload_completed"
  "ret")
