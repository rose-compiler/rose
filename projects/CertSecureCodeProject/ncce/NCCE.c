/*
 * Copyright (c) 2007 Carnegie Mellon University.
 * All rights reserved.

 * Permission to use this software and its documentation for any purpose is hereby granted,
 * provided that the above copyright notice appear and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that the name of CMU not
 * be used in advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, RISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
 
/*

How to include...?:

DCL05
DCL12
DCL14
EXP08
INT15
STR02
MEM02
MEM10
ERR05


Will not compile (on gcc 3.4.4):

PRE01
PRE30
DCL07
DCL09
EXP36
EXP38
INT01
STR07
ARR31
ARR35
ERR03


Not worthwhile at this time:

INT03
STR08
SIG02
ERR02
POS00
POS02
POS31
POS32
MSC08
MSC13

*/

#include "PRE_NCCE.c"
#include "DCL_NCCE.c"
#include "EXP_NCCE.c"
#include "INT_NCCE.c"
#include "FLP_NCCE.c"
#include "ARR_NCCE.c"
#include "STR_NCCE.c"
#include "MEM_NCCE.c"

#include "ENV_NCCE.c"
#include "SIG_NCCE.c"
#include "ERR_NCCE.c"
#include "MSC_NCCE.c"
#include "POS_NCCE.c"

int main(int argc, char const *argv[], char const *envp[]) {
  PRE();
  DCL();
  EXP();
  INT();
  FLP();
  ARR();
  STR();
  MEM();

  ENV(envp);
  SIG();
  ERR();
  MSC();
  POS();

  return 0;
}
