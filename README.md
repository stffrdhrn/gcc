## GCC with SMH Dummy Patches

This is the port of the dummy architecture smh.  This is done following the
21 patches series for ggx which became moxie.

 - See: http://atgreen.github.io/ggx/
 - See: http://stffrdhrn.github.io/software/embedded/openrisc/2018/02/03/openrisc_gcc_rewrite.html
 - See: https://github.com/stffrdhrn/binutils-gdb/tree/smh-port

Here I used the latest GCC so the patches are quite a bit different in
places.  I will try to keep notes of the differences below.

### P11 differences

The original patches for `ggx` are pretty old now and using many deprecated
API's.  This required significant updates, but it was a good exercise in understanding the dependencies
of a GCC back-end.

Also, current GCC builds run a series of self tests which do not allow `abort()`
definitions.  This means the original patch 11 does not build completely. I am
getting a
[failure](https://gist.github.com/stffrdhrn/b40c87a56b7be1c6bd8adae26043adea) I
hope this improves going forward.

 - Using name `SMH` instead of ggx.
 - Update `gcc/config.gcc` too
 - Not using [Old  Constraints](https://gcc.gnu.org/onlinedocs/gcc-4.9.4/gccint/Old-Constraints.html)
in `smh.h`
 - Renovate poison `HARD_REGNO_MODE_OK` to `TARGET_HARD_REGNO_MODE_OK`
 - Renovate poison `HARD_REGNO_NREGS` to `TARGET_...`
 - Renovate poison `MODES_TIEABLE_P` to `TARGET_...`
 - Renovate poison `LEGITIMATE_CONSTANT_P` to `TARGET_...`
 - Renovate poison `FRAME_POINTER_REQUIRED` to `TARGET_...`
 - Removed poison `RETURN_POPS_ARGS`, depend on default
 - Removed poison `TRULY_NOOP_TRUNCATION`, depend on default
 - Redefine `INITIALIZE_TRAMPOLINE` as `TARGET_TRAMPOLINE_INIT`
 - Renovate `GO_IF_MODE_DEPENDENT_ADDRESS` to `TARGET_LEGITIMATE_ADDRESS_P`
 - Define `nop` required
 - Define `ELIMINAL_REGS` required
 - Define `INITIAL_ELIMINATION_OFFSET` required 
 - Don't use `abort()` for `TARGET_` macros.  for now don't define to get default implementation.
 - Remove `CLASS_MAX_NREGS` definition as default implementation is the same.

