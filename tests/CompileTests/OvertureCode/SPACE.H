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

#ifndef _SPACE_H_
#define _SPACE_H_

//
// $Id: SPACE.H,v 1.1 2004/07/07 10:27:12 dquinlan Exp $
//

#if defined(BL_SPACEDIM) && !defined(SPACEDIM)
#define SPACEDIM BL_SPACEDIM
#elif defined(SPACEDIM) && !defined(BL_SPACEDIM)

/*@ManDoc:
  The BL\_SPACEDIM macro specifies the dimension of the BoxLib library
  to be built.  Acceptable values are either 1, 2, or 3.  This macro
  must be set for every compilation as many of the data structures of
  BoxLib rely on it being set.

  Code that depends on the dimensionality of the problem that you're
  trying to solve should be cast in terms of this macro.
*/
#define BL_SPACEDIM SPACEDIM
#elif defined(BL_SPACEDIM) && defined(SPACEDIM)
#elif BL_SPACEDIM != SPACEDIM
#error SPACEDIM and BL_SPACEDIM must be identically defined
#else
#error At least one of BL_SPACEDIM or SPACEDIM must be defined
#endif

#if  (BL_SPACEDIM != 1 && BL_SPACEDIM !=2 && BL_SPACEDIM != 3)
#error BL_SPACEDIM must be either 1, 2, or 3
#endif

#if !defined(BL_LANG_FORT) || defined(BL_LANG_CC)

/*@ManDoc:
  The SpaceDim C++ integer constant specifies the dimension of the
  BoxLib library to be built.  It is initialized to the value of
  the macro BL\_SPACEDIM.  Hence it will have one of the values 1, 2, or 3.

  C++ code that depends on the dimensionality of the problem that
  you're trying to solve should be cast in terms of this constant,
  instead of the BL\_SPACEDIM macro, if possible, as constants are
  visible to debuggers while macros are not.
*/
   const int SpaceDim = BL_SPACEDIM;
#endif

#if BL_SPACEDIM==1

/*@ManDoc:
  The D\_EXPR(a,b,c) macro expands to a comma expression that will
  evaluate SpaceDim of its arguments, counting from the left; i.e.
  BL\_SPACEDIM==1 implies only `a' is evaluated, BL\_SPACEDIM==2
  implies both `a' and `b', and BL\_SPACEDIM==3 implies all three
  arguments are evaluated.  This macro can be used to enhance
  the portability of code that works with structures whose size
  depends on BL\_SPACEDIM.
           
  For example:

     D\_EXPR(vect[0] *= s, vect[1] *= s, vect[2] *= s);

  More explicitly, this macro evaluates to one of

     ((void)((a),0))

     ((void)((a),(b),0))

     ((void)((a),(b),(c),0))

  corresponding to BL\_SPACEDIM values of 1, 2, or 3.
*/
#  define D_EXPR(a,b,c) ((void)((a),0))

/*@ManDoc:
  The D\_DECL(a,b,c) macro expands to a comma-separated list of
  1, 2, or all 3 of the arguments of the call, depending on the
  value of BL\_SPACEDIM.  This can be used to write portable
  function calls that depend on 1, 2, or 3 arguments, corresponding
  to the value of BL\_SPACEDIM.

  For example:

     return IntVect(D\_DECL(p[0] + s, p[1] + s, p[2] + s));

  More explicitly, this macro evaluates to one of

     a

     a,b

     a,b,c

  corresponding to BL\_SPACEDIM values of 1, 2, or 3.
*/
#  define D_DECL(a,b,c) a

/*@ManDoc:
  The D\_TERM(a,b,c) macro expands to a whitespace-separated list of
  1, 2, or all 3 of the arguments of the call, depending on the value
  of BL\_SPACEDIM.  This can be used to write program logic
  that depend on 1, 2, or 3 arguments, corresponding to
  the value of BL\_SPACEDIM.

  For example:

     return D\_TERM(len[0], *len[1], *len[2]);

  More explicitly, this macro evaluates to one of

     a

     a b

     a b c

  corresponding to BL\_SPACEDIM values of 1, 2, or 3.
*/
#  define D_TERM(a,b,c) a
#elif BL_SPACEDIM==2
#  define D_EXPR(a,b,c) ((void)((a),(b),0))
#  define D_DECL(a,b,c) a,b
#  define D_TERM(a,b,c) a b
#elif BL_SPACEDIM==3
#  define D_EXPR(a,b,c) ((void)((a),(b),(c),0))
#  define D_DECL(a,b,c) a,b,c
#  define D_TERM(a,b,c) a b c
#endif

#endif /*_SPACE_H_*/
