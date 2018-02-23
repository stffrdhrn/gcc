/* target code for staffords dummy */

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "df.h"
#include "regs.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "diagnostic-core.h"
#include "output.h"
#include "stor-layout.h"
#include "varasm.h"
#include "calls.h"
#include "expr.h"
#include "builtins.h"

/* This file should be included last.  */
#include "target-def.h"

#define LOSE_AND_RETURN(msgid, x) \
  do					\
    {					\
      smh_operand_lossage (msgid, x);	\
      return;				\
    } while (0)


static void
smh_operand_lossage (const char *msg, rtx op)
{
  debug_rtx (op);
  output_operand_lossage ("%s", msg);
}

/* The TARGET_PRINT_OPERAND_ADDRESS worker.  */
void
smh_print_operand_address (FILE *file, machine_mode m ATTRIBUTE_UNUSED, rtx x)
{
  switch (GET_CODE (x))
    {
    case REG:
      fprintf (file, "(%s)", reg_names[REGNO (x)]);
      break;
    default:
      output_addr_const (file, x);
      break;
    }
}

/* The TARGET_PRINT_OPERAND worker.  */
void
smh_print_operand (FILE *file, rtx x, int code)
{
  rtx operand = x;

  switch (code)
    {
    case 0:
      break;
    default:
      LOSE_AND_RETURN ("invalid operand modifier letter", x);
    }

  switch (GET_CODE (operand))
    {
    case REG:
      if (REGNO (operand) > 7)
	internal_error ("bad register: %d", REGNO (operand));
      fprintf (file, "%s", reg_names[REGNO (operand)]);
      return;
    case MEM:
      output_address (GET_MODE (XEXP (operand, 0)), XEXP (operand, 0));
      return;
    default:
      if (CONSTANT_P (operand))
	{
	  output_addr_const (file, operand);
	  return;
	}
	LOSE_AND_RETURN ("unexpected operand", x);
    }
}

/* Per-function machine data.  */
struct GTY(()) machine_function
{
   /* Number of bytes saved on the stack for callee saved registers.  */
   int callee_saved_reg_size;

   /* Number of bytes saved on the stack for local variables.  */
   int local_vars_size;

   /* The sum of 2 sizes: locals vars and padding byte for saving the
    * registers.  Used in expand_prologue () and expand_epilogue().  */
   int size_for_adjusting_sp;
};

/* Zero initialization is OK for all current fields.  */

static struct machine_function *
smh_init_machine_status (void)
{
  return ggc_cleared_alloc<machine_function> ();
}

/* The TARGET_OPTION_OVERRIDE worker.  */
static void
smh_option_override (void)
{
  /* Set the per-function-data initializer.  */
  init_machine_status = smh_init_machine_status;
}

static void
smh_compute_frame (void)
{
  /* For aligning the local variables.  */
  int stack_alignment = STACK_BOUNDARY / BITS_PER_UNIT;
  int padding_locals;
  int regno;

  /* Padding needed for each element of the frame.  */
  cfun->machine->local_vars_size = get_frame_size ();

  /* Align to the stack alignment.  */
  padding_locals = cfun->machine->local_vars_size % stack_alignment;
  if (padding_locals)
    padding_locals = stack_alignment - padding_locals;

  cfun->machine->local_vars_size += padding_locals;

  cfun->machine->callee_saved_reg_size = 0;

  /* Save callee-saved registers.  */
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (df_regs_ever_live_p (regno) && (! call_used_regs[regno]))
      cfun->machine->callee_saved_reg_size += 4;

  cfun->machine->size_for_adjusting_sp =
    crtl->args.pretend_args_size
    + cfun->machine->local_vars_size
    + (ACCUMULATE_OUTGOING_ARGS
       ? (HOST_WIDE_INT) crtl->outgoing_args_size : 0);
}

void
smh_expand_prologue ()
{
  int regno;
  rtx insn;

  smh_compute_frame ();

  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      /* push $sp, $rN  */
      if (df_regs_ever_live_p (regno) && (!call_used_regs[regno]))
	{
	  insn = emit_insn (gen_movsi_push (gen_rtx_REG (Pmode, regno)));
	  RTX_FRAME_RELATED_P (insn) = 1;
	}
    }

  if (cfun->machine->size_for_adjusting_sp > 0)
    {
      /* ldi.l $r5, -SP_ADJUST  */
      /* add   $sp, $sp, $r5      */
      insn = emit_insn (gen_movsi (gen_rtx_REG (Pmode, SMH_R5),
				   GEN_INT (-cfun->machine->size_for_adjusting_sp)));
      RTX_FRAME_RELATED_P (insn) = 1;
      insn = emit_insn (gen_addsi3 (stack_pointer_rtx,
				    stack_pointer_rtx,
				    gen_rtx_REG (Pmode, SMH_R5)));
      RTX_FRAME_RELATED_P (insn) = 1;
    }
}

void
smh_expand_epilogue ()
{
  int regno;
  rtx insn;

  if (cfun->machine->callee_saved_reg_size > 0)
    {
      /* ldi.l $r5, -REG_SIZE  */
      /* add   $sp, $fp, $r5   */
      /* pop   $rN, $sp        */
      insn = emit_insn (gen_movsi (gen_rtx_REG (Pmode, SMH_R5),
				   GEN_INT (-cfun->machine->callee_saved_reg_size)));
      RTX_FRAME_RELATED_P (insn) = 1;
      insn = emit_insn (gen_addsi3 (stack_pointer_rtx,
				    hard_frame_pointer_rtx,
				    gen_rtx_REG (Pmode, SMH_R5)));
      RTX_FRAME_RELATED_P (insn) = 1;

      for (regno = FIRST_PSEUDO_REGISTER - 1; regno > 0; --regno)
	if (df_regs_ever_live_p(regno) && (! call_used_regs[regno]))
	  {
	    insn = emit_insn (gen_movsi_pop (gen_rtx_REG (Pmode, regno)));
	    RTX_FRAME_RELATED_P (insn) = 1;
	  }
    }
  insn = emit_jump_insn (gen_returner ());
  RTX_FRAME_RELATED_P (insn) = 1;
}

/* Implements the macro INITIAL_ELIMINATION_OFFSET, return the OFFSET.  */

int
smh_initial_elimination_offset (int from, int to)
{
  int ret;

  if ((from) == FRAME_POINTER_REGNUM && (to) == HARD_FRAME_POINTER_REGNUM)
    {
      /* Compute this since we need to use cfun->machine->local_vars_size.  */
      smh_compute_frame ();
      ret = -cfun->machine->callee_saved_reg_size;
    }
  else if ((from) == FRAME_POINTER_REGNUM && (to) == STACK_POINTER_REGNUM)
    ret = 0x100;
  else if ((from) == ARG_POINTER_REGNUM && (to) == FRAME_POINTER_REGNUM)
    ret = 0x600;
  else if ((from) == ARG_POINTER_REGNUM && (to) == STACK_POINTER_REGNUM)
    ret = 0x1000;
  else if ((from) == ARG_POINTER_REGNUM && (to) == HARD_FRAME_POINTER_REGNUM)
    ret = 0x00;
  else
    {
      fprintf (stderr, "Unknown elimination reg pair from:%s to:%s\n", reg_names[from], reg_names[to]);
      ret = 0x00;
      //abort ();
    }

  return ret;
}

/* Return the next register to be used to hold a function argument or
   NULL_RTX if there's no more space.  */

static rtx
smh_function_arg (cumulative_args_t cum_v, machine_mode mode,
		    const_tree type ATTRIBUTE_UNUSED,
		    bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  if (*cum < 8)
    return gen_rtx_REG (mode, *cum);
  else
    return NULL_RTX;
}

#define SMH_FUNCTION_ARG_SIZE(MODE, TYPE)	\
  ((MODE) != BLKmode ? GET_MODE_SIZE (MODE)	\
   : (unsigned) int_size_in_bytes (TYPE))

static void
smh_function_arg_advance (cumulative_args_t cum_v, machine_mode mode,
			    const_tree type, bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  *cum = (*cum < SMH_R5
	  ? *cum + ((3 + SMH_FUNCTION_ARG_SIZE (mode, type)) / 4)
	  : *cum);
}


/* worker function for TARGET_RETURN_IN_MEMORY.  What type of args get returned
 * in memory?  Any value bigger than 64-bits is returned in memory.  */

static bool
smh_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  const HOST_WIDE_INT size = int_size_in_bytes (type);
  return (size == -1 || size > (2 * UNITS_PER_WORD));
}

/* Worker function for TARGET_LEGITIMATE_ADDRESS_P.  */

static bool
smh_legitimate_address_p (machine_mode mode ATTRIBUTE_UNUSED, rtx x,
			  bool strict ATTRIBUTE_UNUSED)
{
  if (REG_P (x) && REGNO_OK_FOR_BASE_P (REGNO (x)))
    return true;
  if (GET_CODE (x) == SYMBOL_REF
      || GET_CODE (x) == LABEL_REF
      || GET_CODE (x) == CONST)
    return true;
  return false;
}

static rtx
smh_function_value (const_tree valtype,
		      const_tree fntype_or_decl ATTRIBUTE_UNUSED,
		      bool outgoing ATTRIBUTE_UNUSED)
{
  machine_mode mode = TYPE_MODE (valtype);
  if (INTEGRAL_TYPE_P (valtype) && GET_MODE_SIZE (mode) < 4)
    mode = SImode;
  return gen_rtx_REG (mode, SMH_R0);
}

/* Define how to find the value returned by a library function.

   We always return values in register $r0 for moxie.  */

static rtx
smh_libcall_value (machine_mode mode,
                     const_rtx fun ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (mode, SMH_R0);
}

/* Handle TARGET_FUNCTION_VALUE_REGNO_P.

   We always return values in register $r0 for moxie.  */

static bool
smh_function_value_regno_p (const unsigned int regno)
{
  return (regno == SMH_R0);
}


/* Initialize the GCC target structure.  */

#undef  TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES	hook_bool_const_tree_true

#undef  TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG		smh_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE	smh_function_arg_advance
#undef	TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY		smh_return_in_memory
#undef	TARGET_MUST_PASS_IN_STACK
#define	TARGET_MUST_PASS_IN_STACK	must_pass_in_stack_var_size
#undef	TARGET_PASS_BY_REFERENCE
#define	TARGET_PASS_BY_REFERENCE	hook_pass_by_reference_must_pass_in_stack

#undef	TARGET_LEGITIMATE_ADDRESS_P
#define	TARGET_LEGITIMATE_ADDRESS_P	smh_legitimate_address_p

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE		smh_function_value
#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE		smh_libcall_value
#undef TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P	smh_function_value_regno_p

#undef TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND		smh_print_operand
#undef TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS	smh_print_operand_address

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE		smh_option_override

#undef TARGET_CONSTANT_ALIGNMENT
#define TARGET_CONSTANT_ALIGNMENT	constant_alignment_word_strings

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-smh.h"
