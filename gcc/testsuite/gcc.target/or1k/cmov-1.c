/* { dg-do compile } */
/* { dg-options "-mclass2 -O2" } */

int cond (int a, int b) {
  return a > b;
}

/* { dg-final { scan-assembler "l.cmov" } } */
