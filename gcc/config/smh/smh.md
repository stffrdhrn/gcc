;; Machine description for SMH

(define_attr "length" "" (const_int 2))

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	 (plus:SI
	  (match_operand:SI 1 "register_operand" "r,r")
	  (match_operand:SI 2 "general_operand" "r,n")))]
  ""
  "add.l  %0, %1, %2")

;; Move

(define_insn "movsi_push"
  [(set:SI (mem:SI (pre_dec:SI (reg:SI 1)))
	   (match_operand:SI 0 "register_operand" "r"))]
  ""
  "push  $sp, %0")

(define_insn "movsi_pop"
  [(set:SI (match_operand:SI 0 "register_operand" "=r")
	   (mem:SI (post_inc:SI (reg:SI 1))))]
  ""
  "pop   $sp, %0")

(define_expand "movsi"
  [(set (match_operand:SI 0 "nonimmediate_operand" "")
	(match_operand:SI 1 "general_operand" ""))]
  ""
  "
{
  if (MEM_P (operands[0]))
    operands[1] = force_reg (SImode, operands[1]);
}")

(define_insn "*movsi"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(match_operand:SI 1 "general_operand" "r,i"))]
  "register_operand (operands[0], SImode)
   || register_operand (operands[1], SImode)"
  "@
  mov	%0, %1
  ldi.l %0, %1")

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
  "mov	%0, %1")

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
  "jsr	%0")

(define_insn "*call"
  [(call (mem:QI (match_operand:SI 0 "immediate_operand" "i"))
	 (match_operand 1 "" ""))]
  ""
  "jsra	%0")

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
  "jsr	%1")

(define_insn "*call_value"
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:QI (match_operand:SI 1 "immediate_operand" "i"))
	      (match_operand 2 "" "")))]
  ""
  "jsra	%1")

(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "general_operand" "r"))]
  "true"
  "jmp %0")

(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0 "" "")))]
  ""
  "jmp %l0%#")

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
