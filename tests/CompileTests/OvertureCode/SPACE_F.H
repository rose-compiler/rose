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

#ifndef _SPACE_F_H_
#define _SPACE_F_H_

//
// $Id: SPACE_F.H,v 1.1 2004/07/07 10:27:12 dquinlan Exp $
//

#include <SPACE.H>

#ifdef BL_LANG_FORT
#  if (BL_SPACEDIM == 1)

/*@ManDoc:
  The DIM\_V() macro can be used to enhance the portability of Fortran
  code.  It can be used whenever the BL\_LANG\_FORT preprocessor macro
  is defined.  It is used to specify the dimensionality of Fortran
  arrays, corresponding to BL\_SPACEDIM.  `lo' and `hi' are
  BL\_SPACEDIM-dimensional integer arrays that specify the low and
  high range of the array being specified.

  More explicitly, the macro evaluates to one of


     lo(1):hi(1)

     lo(1):hi(1),lo(2):hi(2)

     lo(1):hi(1),lo(2):hi(2),lo(3):hi(3)

  corresponding to BL\_SPACEDIM values of 1, 2, or 3.
*/
#    define DIM_V(lo,hi) lo(1):hi(1)
#  elif (BL_SPACEDIM == 2)
#    define DIM_V(lo,hi) lo(1):hi(1),lo(2):hi(2)
#  elif (BL_SPACEDIM == 3)
#    define DIM_V(lo,hi) lo(1):hi(1),lo(2):hi(2),lo(3):hi(3)
#  endif
#endif

#endif /*_SPACE_F_H_*/
