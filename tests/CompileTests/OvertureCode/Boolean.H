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

#ifndef _BOOLEAN_H_
#define _BOOLEAN_H_

// *wdh*
#include "OvertureDefine.h"

//
// $Id: Boolean.H,v 1.1 2004/07/07 10:27:01 dquinlan Exp $
//

// *wdh
// undef TRUE
// undef FALSE
// undef True
// undef False
// *wdh

#undef true
#undef false
#undef bool

#if !defined(BL_LANG_FORT)
// *wdh if !(defined(__PHOTON) || defined(__KCC) || defined(__GNUG__) || (defined(__sgi) && (__mips==4 || __mips==3)))
#ifndef OV_BOOL_DEFINED
/*@ManDoc:
  bool is typedef'd to an int.  It is defined only for those compilers
  that do not yet support the bool type as specified in the ANSI C++ Draft
  Standard.  It is provided here as a convenience so that all C++ code can
  be written using bool as the bool type, regardless of whether or not the
  compiler truly supports the bool type.
*/
    typedef int bool;

/*@ManDoc:
  true is a constant of type bool.  It is provided here for those compilers
  that do not yet support the bool type as specified in the ANSI C++ Draft
  Standard.
*/
    const bool true = 1;

/*@ManDoc:
  false is a constant of type bool.  It is provided here for those compilers
  that do not yet support the bool type as specified in the ANSI C++ Draft
  Standard.
*/
    const bool false = 0;
#endif /*!(defined(__PHOTON || defined(__GNUG__))*/

#else

#define TRUE   1
#define FALSE  0
#endif

#endif /*_BOOLEAN_H_*/

