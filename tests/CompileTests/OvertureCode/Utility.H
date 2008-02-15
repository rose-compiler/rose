/* wdh: add this next to define Boxlib define's as a function of Overture define's */
#include "OvertureBL.h"

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

#ifndef _UTILITY_H_
#define _UTILITY_H_

// *wdh*
#include "OvertureDefine.h"

//
// $Id: Utility.H,v 1.1 2004/07/07 10:27:13 dquinlan Exp $
//

#include <iostream.h>

#include <Boolean.H>

//
//@Man:
//@Memo: Useful C++ Utility Functions
/*@Doc:

  This data-less class is a poor-person's namespace of utility functions.
  Since we can't assume the existence of namespaces, and we don't
  like global functions, we put them into a data-less class as
  static member functions.
*/

class Utility
{
public:
    /*@ManDoc: Returns the total user and system time used by
               the calling process up to the point of the call.
               If t != 0, it also stores the result in t.
    */
    static double second (double* t = 0);

    /*@ManDoc: Returns the current time as the number of microseconds
               relative to some epoch in the past.  If t != 0, it also
               stores the result in t.
    */
    static double wsecond (double* t = 0);
    //
    //@ManDoc: Return true if argument is a non-zero length string of digits.
    //
    static bool is_integer (const char* str);
};

#ifdef BL_BACKWARD_COMPAT
inline double second (double* t = 0)
{
    return Utility::second(t);
}

inline double wsecond (double* t = 0)
{
    return Utility::wsecond(t);
}

inline bool is_integer (const char* str)
{
    return Utility::is_integer(str);
}
#endif /*BL_BACKWARD_COMPAT*/

//
// The following #ifdef ... #endif block is simply a bit of subterfuge
// to force doc++ to document BL_BACKWARD_COMPAT.  I wasn't able to get
// the explicit doc++ directives to do what I wanted :-(
//
#ifdef BL_BACKWARD_COMPAT
#undef BL_BACKWARD_COMPAT
/*@ManDoc:
  A number of changes have been made to this version of BoxLib that aren't
  completely backward compatible with earlier releases.  The macro
  BL\_BACKWARD\_COMPAT can be turned on when porting old code to the new
  version of the library to gloss over these changes.  Eventually, code
  should probably be updated to use the new way of doing things, as
  this macro may go away in future releases of BoxLib.

  A simple way to turn on this macro is to add the following to your
  make command line:

    XTRADEFS+=-DBL\_BACKWARD\_COMPAT

  Here's an enumeration of BoxLib changes that turning on this macro will
  gloss over:

    second()               -> Utility::second()

    wsecond()              -> Utility::wsecond()

    is\_integer()          -> Utility::is\_integer()

    FABioPrecision         -> FABio::Precision

    FABioFormat            -> FABio::Format

    FABioOrdering          -> FABio::Ordering

    FAB\_FLOAT             -> FABio::FAB\_FLOAT

    FAB\_DOUBLE            -> FABio::FAB\_DOUBLE

    FAB\_ASCII             -> FABio::FAB\_ASCII

    FAB\_IEEE              -> FABio::FAB\_IEEE

    FAB\_NATIVE            -> FABio::FAB\_NATIVE

    FAB\_8BIT              -> FABio::FAB\_8BIT

    FAB\_NORMAL\_ORDER     -> FABio::FAB\_NORMAL\_ORDER

    FAB\_REVERSE\_ORDER    -> FABio::FAB\_REVERSE\_ORDER

    FAB\_REVERSE\_ORDER\_2 -> FABio::FAB\_REVERSE\_ORDER\_2

    UNITBOX                -> Box::TheUnitBox()

    ZEROV                  -> IntVect::TheZeroVector()

    UNITV                  -> IntVect::TheUnitVector()

    NODEV                  -> IntVect::TheNodeVector()

    CELLV                  -> IntVect::TheCellVector()

    CELLTYPE               -> IndexType::TheCellType()

    NODETYPE               -> IndexType::TheNodeType()
*/
#define BL_BACKWARD_COMPAT 1
#endif /*BL_BACKWARD_COMPAT*/

//
// I'm going to document right here all the BL macros that aren't documented
// anywhere else.  Note that all there #ifdef ... #endif blocks are necessary
// to get doc++ to properly document the macros.
//

#ifdef BL_ARCH_CRAY
#undef BL_ARCH_CRAY
/*@ManDoc:
  The macro BL\_ARCH\_CRAY is defined only when compiling on a Cray
  architecture.  The make subsystem automatically sets this if it
  detects that a compilation is taking place on a Cray system.
*/
#define BL_ARCH_CRAY 1
#endif /*BL_ARCH_CRAY*/

#ifdef BL_ARCH_IEEE
#undef BL_ARCH_IEEE
/*@ManDoc:
  The macro BL\_ARCH\_IEEE is defined only when compiling on a Cray architecture.
  The make subsystem automatically sets this if it detects that a compilation
  is taking place on a non-Cray system.  That is to say, we assume all
  architectures, with the exception of Cray, are IEEE.
*/
#define BL_ARCH_IEEE 1
#endif /*BL_ARCH_IEEE*/

#ifdef BL_USE_FLOAT
#undef BL_USE_FLOAT
/*@ManDoc:
  The macro BL\_USE\_FLOAT indicates that C++ floating-point calculations should
  use "float" variables and Fortran floating-point calculations should use
  "real*4" variables.  One of BL\_USE\_FLOAT or BL\_USE\_DOUBLE must always be
  defined when compiling and using BoxLib.
*/
#define BL_USE_FLOAT 1
#endif /*BL_USE_FLOAT*/

#ifdef BL_USE_DOUBLE
#undef BL_USE_DOUBLE
/*@ManDoc:
  The macro BL\_USE\_DOUBLE indicates that C++ floating-point calculations
  should use "double" variables and Fortran floating-point calculations
  should use "real*8" variables.  One of BL\_USE\_FLOAT or BL\_USE\_DOUBLE must
  always be defined when compiling and using BoxLib.  This macro is not
  allowed on Cray architectures; i.e. only BL\_USE\_FLOAT is allowed on Cray
  architectures.
*/
#define BL_USE_DOUBLE 1
#endif /*BL_USE_DOUBLE*/

#ifdef BL_LANG_FORT
#undef BL_LANG_FORT
/*@ManDoc:
  The macro BL\_LANG\_FORT indicates that Fortran code is being compiled.
*/
#define BL_LANG_FORT 1
#endif /*BL_LANG_FORT*/

#ifdef BL_LANG_CC
#undef BL_LANG_CC
/*@ManDoc:
  The macro BL\_LANG\_CC indicates that C++ code is being compiled.
*/
#define BL_LANG_CC 1
#endif /*BL_LANG_CC*/

#ifdef BL_AUTO_INSTANTIATE
#undef BL_AUTO_INSTANTIATE
/*@ManDoc:
  The macro BL\_AUTO\_INSTANTIATE indicates that the C++ compiler is not
  able to find a template source file (.C file) given the template declaration
  file (.H file).  This macro instructs the compiler to automatically include
  the .C file, whenever it includes the .H file.
*/
#define BL_AUTO_INSTANTIATE 1
#endif /*BL_AUTO_INSTANTIATE*/


#ifdef BL_FORT_USE_UNDERSCORE
#undef BL_FORT_USE_UNDERSCORE
/*@ManDoc:
  The macro BL\_FORT\_USE\_UNDERSCORE indicates that C++ code should call
  Fortran routines by appending an underscore to the name of the Fortran
  routine.  This is set automatically by the make subsystem.  It is the
  default for all BoxLib-style programs except on Cray architectures.

  For example, if the Fortran routine is named abcxyx, then it will
  be called in C++ code as abcxyz\_.
*/
#define BL_FORT_USE_UNDERSCORE 1
#endif /*BL_LANG_CC*/

#ifdef BL_FORT_USE_UPPERCASE
#undef BL_FORT_USE_UPPERCASE
/*@ManDoc:
  The macro BL\_FORT\_USE\_UPPERCASE indicates that C++ code should call
  Fortran routines using uppercase letters for all the letters in the
  routine.  This is set automatically by the make subsystem.  Currently it
  is only set when compiling on Cray architectures.

  For example, if the Fortran routine is named abcxyx, then it will
  be called in C++ code as ABCXYZ.
*/
#define BL_FORT_USE_UPPERCASE 1
#endif /*BL_FORT_USE_UPPERCASE*/

/*@ManDoc:
  BL\_IGNORE\_MAX is a macro that expands to the literal value 100000.  It is
  defined when compiling either Fortran or C++ code; i.e. when either
  BL\_LANG\_CC or BL\_LANG\_FORT is defined.  It is used in calls to
  istream::ignore() in the BoxLib code when reading in characters from an
  istream.  We use this macro instead of the more proper INT\_MAX from
  <limits.h> since at least one compiler didn't work properly when
  istream::ignore() was passed INT\_MAX.
*/
#define BL_IGNORE_MAX 100000


#endif /*_UTILITY_H_*/
