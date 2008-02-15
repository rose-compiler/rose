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

#ifndef _TUPLE_H_
#define _TUPLE_H_

// *wdh*
#include "OvertureDefine.h"

//
// $Id: Tuple.H,v 1.1 2004/07/07 10:27:13 dquinlan Exp $
//

#include <Assert.H>
#include <stdlib.h>

//
//@Man:
//@Memo: Ordered Tuples for Types T
/*@Doc:

  This class represents ordered tuples of some user-specified concrete
  type T for N > 0. The type T must have a default constructor.  If the
  non-default constructor, copy constructor, or copy assignment operator
  are used is used, T must also have a copy constructor.
*/

template <class T, size_t N>
class Tuple
{
public:
 
    /*@ManDoc: The default constructor.  For user-defined types T, the
               default constructor for T will be run on each of the N
               objects in the Tuple.  For builtin (intrinsic) types,
               the values in the Tuple will be garbage.
    */
    inline Tuple ();

    /*@ManDoc: Constructs a Tuple, initializing the elements in the Tuple
               with the corresponding elements in the vector v.  This assumes
               that v contains at least N elements of type T -- an assumption
               that is NOT checked.  For user-defined types, T must have a
               well-defined and accessible copy constructor.
    */
    Tuple (const T* v);
    //
    // The copy constructor.
    //
    Tuple (const Tuple& rhs);
    //
    // The copy assignment operator.
    //
    Tuple& operator= (const Tuple& rhs);
 
    /*@ManDoc: Returns a reference to the i'th element in the Tuple,
               counting from zero.  Performs range checking when the library
               is compiled in debug mode.
    */
    inline T& operator[] (int i);
 
    /*@ManDoc: Returns a constant reference to the i'th element in the Tuple,
               counting from zero.  Performs range checking when the library
               is compiled in debug mode.
    */
    inline const T& operator[] (int i) const;
 
    /*@ManDoc: Returns the address of the underlying vector of T
               representation.  This should ONLY be used when interfacing
               to Fortran as it breaks the encapsulation of the class.
    */
    inline operator const T* () const;

protected:
    //
    // The underlying vector of T representing the Tuple.
    //
    T vect[N];
};

//
// Inlines.
//

template <class T, size_t N>
inline
Tuple<T,N>::Tuple()
{}

template <class T, size_t N>
inline
T&
Tuple<T,N>::operator[] (int i)
{
    boxAssert(0 <= i && i < N);
    return vect[i];
}

template <class T, size_t N>
inline
const T&
Tuple<T,N>::operator[] (int i) const
{
    boxAssert(0 <= i && i < N);
    return vect[i];
}

template <class T, size_t N>
inline
Tuple<T,N>::operator const T* () const
{
    return &vect[0];
}

#ifdef BL_AUTO_INSTANTIATE
#include <Tuple.C>
#endif

#endif /*_TUPLE_H_*/
