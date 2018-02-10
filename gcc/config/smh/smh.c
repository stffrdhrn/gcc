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

  *cum = (*cum < 7
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


struct gcc_target targetm = TARGET_INITIALIZER;

