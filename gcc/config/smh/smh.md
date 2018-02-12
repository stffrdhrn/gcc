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
  "*
  abort();")

(define_expand "movsi"
  [(set (match_operand:SI 0 "nonimmediate_operand" "")
	(match_operand:SI 1 "general_operand" ""))]
  ""
  "
{
  if (GET_CODE (operands[0]) == MEM)
    operands[1] = force_reg (SImode, operands[1]);
}")

(define_insn "*movsi"
  [(set (match_operand:SI 0 "nonimmediate_operand" "=r,r")
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
  if (GET_CODE (operands[0]) == MEM)
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

(define_insn "epilogue"
  [(return)]
  ""
  "ret")
