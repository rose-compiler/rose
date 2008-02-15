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

#ifndef _ARRAYLIM_H_
#define _ARRAYLIM_H_

//
// $Id: ArrayLim.H,v 1.1 2004/07/07 10:27:00 dquinlan Exp $
//

#if !defined(BL_LANG_FORT)

//
// The #ifdef ... #endif subterfuge here is to force doc++ to properly
// document BL_USE_ARLIM :-(
//
#ifdef BL_USE_ARLIM
#undef BL_USE_ARLIM
/*@ManDoc:
  The macro BL\_USE\_ARLIM indicates that the Fortran compiler allows array
  limits to be passed as integer vectors.  The default for Standard Fortran
  is to expand the integer vector into its respective components.  This
  macro is used to control the expansion of the macros ARLIM\_P(x) and
  ARLIM(x).
*/
#define BL_USE_ARLIM 1

/*@ManDoc:
  The macro ARLIM\_P(x) should be used in C++ code when prototyping a Fortran
  function in an extern "C" statement (or block) that takes a
  BL\_SPACEDIM-dimensional array and a BL\_SPACEDIM-dimensional integer vector
  indicating the upper and lower dimensions of the array.

  For example:

    extern "C" void f (Real* array, ARLIM\_P(ivec));

  When BL\_USE\_ARLIM is defined, this macro expands to

    const int *

  When BL\_USE\_ARLIM is not defined, this macro expands to one of

    const int \&

    const int \&, const int \&

    const int \&, const int \&, const int \&

  depending on the value of BL\_SPACEDIM.
*/
# define ARLIM\_P(x)  const int*

/*@ManDoc:
  The macro ARLIM(x) should be used in C++ code when passing an integer vector
  specifying the dimensions of an array, to a Fortran routine that has
  previously been prototyped with ARLIM\_P(x).

  For example:

     f(array, ARLIM(ivec));

  When BL\_USE\_ARLIM is defined, this macro expands to

    x

  When BL\_USE\_ARLIM is not defined, this macro expands to one of

    x[0]

    x[0],x[1]

    x[0],x[1],x[2]

  depending on the value of BL\_SPACEDIM.
*/
# define ARLIM(x)  x
#else
#   if   BL_SPACEDIM==1
#       define  ARLIM_P(x)  const int&
#       define  ARLIM(x)  x[0]
#   elif BL_SPACEDIM==2
#       define  ARLIM_P(x)  const int&,const int&
#       define  ARLIM(x)  x[0],x[1]
#   elif BL_SPACEDIM==3
#       define  ARLIM_P(x)  const int&,const int&,const int&
#       define  ARLIM(x)  x[0],x[1],x[2]
#   endif
#endif
#endif

#endif /*_ARRAYLIM_H_*/
