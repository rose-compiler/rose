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

#ifndef MISC_H
#define MISC_H

//
// $Id: Misc.H,v 1.1 2004/07/07 10:27:10 dquinlan Exp $
//

#include <SPACE.H>

/*@ManDoc:
  Returns a copy of the minimum of the two values.  Will work with
  any type T that supplies a T::operator<(const T, const T), or an
  equivalent construct, as well as a copy constructor.   In
  particular, it will work with any integral or floating-point
  intrinsic type.
*/

template <class T>
inline
T
Min (const T& a,
     const T& b)
{
    return a < b ? a : b;
}

/*@ManDoc:
  Returns a copy of the maximum of the two values.  Will work with
  any type T that supplies a T::operator>(const T, const T), or an
  equivalent construct, as well as a copy constructor.   In
  particular, it will work with any integral or floating-point
  intrinsic type.
*/

template <class T>
inline
T
Max (const T& a,
     const T& b)
{
    return a > b ? a : b;
}

/*@ManDoc:
  Returns a copy of the absolute value of the value.  Will work with
  any type T that can be compared against zero with
  T::operator>(const T, const T), or an equivalent construct, as
  well as a copy constructor, and an T::operator-() or equivalent.
  In particular, it will work with any integral or floating-point
  intrinsic type.
*/

template <class T>
inline
T
Abs (const T& a)
{
    return a > 0 ? a : -a;
}

/*@ManDoc:
  Swaps the two values.  Type T must have a copy constructor and
  an assignment operator.
*/

template <class T>
inline
void
Swap (T& a, T& b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

#endif /*MISC_H*/
