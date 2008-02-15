/*
** This software is copyright (C) by the Lawrence Berkeley National
** Laboratory.  Permission is granted to reproduce this software for
** non-commercial purposes provided that this notice is left intact.
**  
** It is acknowledged that the U.S. Government has rights to this
** software under Contract DE-AC03-765F00098 between the U.S.  Department
** of Energy and the University of California.
**  
** This software is provided as a professional and academic contribution
** for joint exchange. Thus it is experimental, is provided ``as is'',
** with no warranties of any kind whatsoever, no support, no promise of
** updates, or printed documentation. By using this software, you
** acknowledge that the Lawrence Berkeley National Laboratory and Regents
** of the University of California shall have no liability with respect
** to the infringement of other copyrights by any part of this software.
**  
** For further information about this software, contact:
** 
**         Dr. John Bell
**         Bldg. 50D, Rm. 129,
**         Lawrence Berkeley National Laboratory
**         Berkeley, CA, 94720
**         jbbell@lbl.gov
*/

#ifndef _BOX_ASSERT_H_
#define _BOX_ASSERT_H_

//
// $Id: Assert.H,v 1.1 2004/07/07 10:27:00 dquinlan Exp $
//

#include <BoxLib.H>

/* *wdh* remove redefine of assert
 ifdef assert
 undef assert
 endif
 */

#if defined(NDEBUG)

/*@ManDoc:

  The macro boxAssert(EX) is a debugging macro, used to test the
  internal consistency of BoxLib, that may also be useful in user
  code.  The argument EX is a C++ expression that you expect to
  always evaluate to true.

  When code is compiled with the DEBUG macro set to TRUE, the
  boxAssert(EX) macro expands to a boolean test of the expression EX.
  If that expression evaluates to true, nothing happens, but if the
  expression evaluates to false, a message is output detailing the
  file and line number of the boxAssert(EX) statement, as well as the
  literal expression EX itself, and then exits via abort() using
  BoxLib::Assert().  The idea is that if the assertion fails, something
  has gone terribly wrong somewhere.

  If the DEBUG macro is not set to TRUE, the boxAssert(EX) call becomes
  a null statement in the code, which will be eliminated by the code
  optimizer.
*/
/* *wdh* define assert(EX) ((void)0) */
#define boxAssert(EX) ((void)0) 

#else
#if defined(__sgi) || defined(__hpux)
//
// The problem here is that some brain-dead compilers cannot deal with a
// void argument to the ternary operator.  Currently SGI's OCC and CC on
// HP-UX 9.0.1 have this problem.
//
/* *wdh* define assert(EX) if ((EX)) ; else BoxLib::Assert( # EX , __FILE__, __LINE__) */
#define boxAssert(EX) if ((EX)) ; else BoxLib::Assert( # EX , __FILE__, __LINE__)
#else
/* wdh define assert(EX) (EX)?((void)0):BoxLib::Assert( # EX , __FILE__, __LINE__) */
#define boxAssert(EX) if ((EX)) ; else BoxLib::Assert( # EX , __FILE__, __LINE__)
#endif
#endif

#endif /*_BOX_ASSERT_H_*/
