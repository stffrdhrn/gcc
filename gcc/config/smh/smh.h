/* Target definitions for smh */

#ifndef GCC_SMH_H
#define GCC_SMH_H

/* Data Types */

#define INT_TYPE_SIZE 32
#define SHORT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64

#define FLOAT_TYPE_SIZE 32
#define DOUBLE_TYPE_SIZE 64
#define LONG_DOUBLE_TYPE_SIZE 64

#define DEFAULT_SIGNED_CHAR 1

/* Registers ..
 *
 * $fp, $sp, $r0 - $r5
 *
 * and
 *
 * $pc
 */

#define SMH_FP 0
#define SMH_SP 1
#define SMH_R0 2
#define SMH_R1 3
#define SMH_R2 4
#define SMH_R3 5
#define SMH_R4 6
#define SMH_R5 7
#define SMH_PC 8

#define REGISTER_NAMES {	\
  "$fp", "$sp", "$r0", "$r1",	\
  "$r2", "$r3", "$r4", "$r5",	\
  "$pc" }

#define FIRST_PSEUDO_REGISTER 9

enum reg_class
{
  NO_REGS,
  GENERAL_REGS,
  SPECIAL_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};

#define N_REG_CLASSES LIM_REG_CLASSES

#define REG_CLASS_CONTENTS \
{ { 0x00000000 },	\
  { (1<<9) - 1 },	\
  { (1<<8) },		\
  { (1<<10) -1 }	\
}

#define REG_CLASS_NAMES { \
	"NO_REGS",	\
	"GENERAL_REGS",	\
	"SPECIAL_REGS",	\
	"ALL_REGS" }

#define FIXED_REGISTERS { \
  1, 1, 0, 0,	\
  0, 0, 0, 0,	\
  1 }

#define CALL_USED_REGISTERS { \
  1, 1, 1, 0,	\
  0, 0, 0, 1,	\
  1 }

#define REGNO_REG_CLASS(regno) ((regno < 9) ? GENERAL_REGS : SPECIAL_REGS)

#define ELIMINABLE_REGS							\
{{ FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM },			\
 { ARG_POINTER_REGNUM,   HARD_FRAME_POINTER_REGNUM }}

#define CLASS_MAX_NREGS(class,mode)	\
  ((GET_MODE_SIZE (mode) + UNITS_PER_WORD - 1)	\
   / UNITS_PER_WORD)

#define PREFERRED_RELOAD_CLASS(x, class) class

/* Framework of an Assembler FIle (gcc/doc/tm.texi) */

#define ASM_COMMENT_START "#"
#define ASM_APP_ON ""
#define ASM_APP_OFF ""

#define ASM_OUTPUT_ALIGN(stream, power)	\
	fprintf (stream, "\t.p2align\t%d\n", power)

#define PRINT_OPERAND(s,x,c) (abort(), 0)
#define PRINT_OPERAND_ADDRESS(s,x) (abort(), 0)

#define GLOBAL_ASM_OP "\t.global\t"

/* Function calling conventions */

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET) (abort(), 0)

#define CUMULATIVE_ARGS int
#define INIT_CUMULATIVE_ARGS(ca,fntype,libname,fndecl,n_named_args) \
  (ca = 0)

#define INITIAL_FRAME_POINTER_OFFSET(DEPTH) (DEPTH) = 0;
#define FIRST_PARM_OFFSET(F) 0

/* Storage Layout */
#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 1
#define WORDS_BIG_ENDIAN 1

#define FUNCTION_BOUNDARY 16
#define BIGGEST_ALIGNMENT 16
#define STRICT_ALIGNMENT 1
#define SLOW_BYTE_ACCESS 1
#define UNITS_PER_WORD 2
#define STACK_BOUNDARY 32
#define PARM_BOUNDARY 32

/* Profiling */
#define FUNCTION_PROFILER(FILE,LABELNO) (abort (), 0)

/* Trampolines, for nested functions */
#define TRAMPOLINE_SIZE (abort (), 0)
#define TRAMPOLINE_ALIGNMENT (abort (), 0)

/* Pointer mode */
#define Pmode	SImode
#define FUNCTION_MODE	QImode
#define STACK_POINTER_REGNUM 1
#define FRAME_POINTER_REGNUM 0

#define HARD_FRAME_POINTER_REGNUM 0

/* The register number of the arg pointer register, which is used to
   access the function's argument list.  */
#define ARG_POINTER_REGNUM 0

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which function arguments are sometimes passed.  */
#define FUNCTION_ARG_REGNO_P(r) (r > 1 && r < 4)

/* A macro whose definition is the name of the class to which a vqalid
   base register must belong.  A base register is one used in an
   address which is the register value plus a displacement.  */
#define BASE_REG_CLASS NO_REGS

#define INDEX_REG_CLASS NO_REGS

/* A C expression which is nonzero if register number NUM is suitable
   for use as a base register in operand addresses.  */
#ifdef REG_OK_STRICT
#define REGNO_OK_FOR_BASE_P(NUM)                 \
  (((NUM < FIRST_PSEUDO_REGISTER)                \
    && ((REGNO_REG_CLASS(NUM) == GENERAL_REGS)   \
        || (NUM == HARD_FRAME_POINTER_REGNUM))))
#else
#define REGNO_OK_FOR_BASE_P(NUM)                 \
  ((REGNO_REG_CLASS(NUM) == GENERAL_REGS)        \
   || (NUM >= FIRST_PSEUDO_REGISTER))
#endif

/* A C expression which is nonzero if register number NUM is suitable
   for use as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(NUM) 0

/* The maximum number of bytes that a single instruction can move
   quickly between memory and registers or between two memory
   locations.  */
#define MOVE_MAX 4

#undef TARGET_FRAME_POINTER_REQUIRED
#define TARGET_FRAME_POINTER_REQUIRED hook_bool_void_true

/* A C expression that is 1 if the RTX X is a constant which is a
   valid address.  */
#define CONSTANT_ADDRESS_P(X) CONSTANT_P(X)

/* A number, the maximum number of registers that can appear in a
   valid memory address.  */
#define MAX_REGS_PER_ADDRESS 1

/* An alias for a machine mode name.  This is the machine mode that
   elements of a jump-table should have.  */
#define CASE_VECTOR_MODE SImode

/* Run-time Target Specification */

#undef TARGET_CPU_CPP_BUILTINS
#define TARGET_CPU_CPP_BUILTINS() \
  { \
    builtin_assert ("cpu=smh"); \
    builtin_assert ("machine=smh"); \
    builtin_define ("__smh__=1"); \
  }

#endif
