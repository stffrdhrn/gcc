;; Machine description for SMH

(define_attr "length" "" (const_int 2))

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

(define_insn "movsi"
  [(set (match_operand:SI 0 "nonimmediate_operand" "=r")
	(match_operand:SI 1 "general_operand" "r"))]
  ""
  "mov %0, %1")

(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "general_operand" "r"))]
  "true"
  "jmp %0")

(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0 "" "")))]
  ""
  "jmp %l0%#")
