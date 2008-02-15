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

#ifndef _ARRAY_H_
#define _ARRAY_H_

// *wdh*
#include "OvertureDefine.h"

//
// $Id: Array.H,v 1.1 2004/07/07 10:27:00 dquinlan Exp $
//

#include <iostream.h>

#include <Boolean.H>
#include <Assert.H>
#include <BoxLib.H>

template <class T> class Array;

//
//@Man:
//@Memo: An Array of Objects of Type T
/*@Doc: 

   This class implements an array of objects of the parameterized type
   T.  In contrast with the predefined C++ array type, an `Array<T>'
   object knows its length, can be dynamically resized, and provides
   automatic bounds checking.  The bounds checking can be turned off by
   specifying the -DNDEBUG flag on the command line when compiling the
   BOXLIB library.  The main reason for defining the ARRAY class is that
   it is used, either by composition or inheritance, to implement many of
   the other classes in the BOXLIB library.

   The `Array<T>' class works by storing copies of the objects it
   contains.  If the objects are large, such as `FARRAYBOX's it is
   probably better to use the `PArray' class which is an array class that
   stores pointers to the objects (avoiding expensive copies).
   The `Array<T>' class destructs all objects in the array when it is 
   itself destructed.  If this is not the desired action, the `PArray' class
   should be used.

   In the Array<T> class, there are two different concepts of size: the amount
   of space allocated, and the amount of space actually used.  Obviously, the 
   allocated space must be larger than the used space.  We separate these two
   concepts to allow the user to optionally avoid memory allocation costs.
   Rather than construct and destroy a temporary Array<T> many times, it may 
   be less expensive to allocate it once with enough memory to handle all uses,
   and resize the Array<T> for each particular use.  See the member functions
   `reserve', `shrinkWrap', and `resize'. 

   Note that care must be taken when deriving classes from `Array<T>'.  It is
   a concrete class, not a polymorphic one.

   This class does NOT provide an assignment operator for assigning an integer
   to an Array<T>.
*/

template <class T>
class Array
{
public:
    //
    //@ManDoc: Constructs an `Array<T>' with no elements
    //
     Array ();

    /*@ManDoc: Constructs an `Array<T>' of length len with the value of each
      element defined by the default constructor for `T'.
    */
    Array (long len)
    {
        //
        // This is inlined here as OCC won't have it any other way :-(
        //
        boxAssert(len >= 0);
        nelem    = len;
        vp       = new T[len];
        truesize = nelem;
        if (vp == 0)
            BoxLib::OutOfMemory(__FILE__, __LINE__);
    }

    /*@ManDoc: Constructs an `Array<T>' of length len with the value of each
               elements given by initialvalue.
    */
    Array (long     len,
           const T& initialvalue);

    /*@ManDoc: Constructs an `Array<T>' of length len in which the K'th
               value is a copy of vec[K].
    */
    Array (const T* vec,
           long     len);
    //
    //@ManDoc: The copy constructor.
    //
    Array (const Array<T>& rhs);

    /*@ManDoc: This operator deletes the current `Array<T>' and replaces it 
               with a copy of rhs.
    */
    Array<T>& operator= (const Array<T>& rhs);
    //
    //@ManDoc: Deletes the `Array<T>' and all objects it contains.
    //
     ~Array ();

    /*@ManDoc: Destructs each element in this `Array<T>'.  The resulting array
               has length zero.
    */
     void clear ();
    //
    //@ManDoc: Returns true if this `Array<T>' is not empty.
    //
     bool ready  () const;
 
    /*@ManDoc: Reserve space for future expansion of memory.  You still must
               `resize' to address the memory.
    */
    void reserve (long _truesize);
 
    /*@ManDoc: Shrink allocated memory to be just enough for elements in
               Array.  This is useful if you allocated a lot of memory to
               avoid memory allocation delays as you add elements.  Once all
               the elements are added, you can reduce memory usage to
               the minimum by calling this function.
    */
    void shrinkWrap ();
 
    /*@ManDoc: This function changes the size of this `Array<T>' to newlen
               while preserving the value of as many array elements as
               possible.  If newlen is greater than the current length, the
               array is grown and the new elements have the value given by
               the default constructor for `T'.  If newlen is less than the
               current length the array is cropped with the remaining
               elements retaining their original values.
    */
    void resize (long newlen);
 
    /*@ManDoc: This function changes the size of this `Array<T>' to newlen
               while preserving the value of as many array elements as
               possible.  If newlen is greater than the current length, the
               array is grown and the new elements have the value given by
               initialvalue.  If newlen is less than the current length the
               array is cropped with the remaining elements retaining their
               original values.
    */
    void resize (long     newlen,
                 const T& initialvalue);
    //
    //@ManDoc: Return number of elements in the array.
    //
     long length () const;
 
    /*@ManDoc: Return the maximum number of elements the array can hold
               without doing a `resize'.
    */
     long trueSize () const;
 
    /*@ManDoc: Returns a reference to the K'th element in this `Array<T>'.
               The element can be modified through this reference.  The
               result can be used as an L-value.
    */
     T& operator[] (long K);
    //
    //@ManDoc: Same as above, except acts on const Array's.
    //
     const T& operator[] (long K) const;
    //
    //@ManDoc: Different syntax for operator[] (long i).
    //
     T& get (long i);
    //
    //@ManDoc: Different syntax for const operator[] (long i).
    //
     const T& get (long i) const;
 
    /*@ManDoc: Returns pointer to vector of data.  This function breaks object
               encapsulation and should only be used for interfacing to
               Fortran subroutines.
    */
     T* dataPtr ();
    //
    //@ManDoc: Same as above for constant arrays.
    //
     const T* dataPtr () const;
    //
    //@ManDoc: Changes the i'th element of this `Array<T>' to elem.
    //
     void set (long     i,
                     const T& elem)
    {
        //
        // This is inlined here as OCC won't have it any other way :-(
        //
        boxAssert(vp != 0);
        boxAssert(i >= 0 && i < nelem);
        vp[i] = elem;
    }
    //
    //@ManDoc: This function swaps the i'th and j'th element of the array.
    //
     void swap (long i,
                      long j);
    //
    //@ManDoc: Test for equality.
    //
    bool operator== (const Array<T>& rhs) const;
    //
    //@ManDoc: Test for inequality.
    //
     bool operator!= (const Array<T>& rhs) const;

protected:
    //
    // The true size of the array.
    //
    long truesize;
    //
    // The number of elements in the array.
    //
    long nelem;
    //
    // The array itself.
    //
    T* vp;

private:
    //
    // This is disallowed.
    //
    Array<T>& operator= (int);
};

/* -----
//
// Inlines.
//

template <class T>
inline
Array<T>::Array ()
{
    nelem    = 0;
    vp       = new T[1];
    truesize = 1;
    if (vp == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
}

template <class T>
inline
void
Array<T>::clear ()
{
    delete [] vp;
    vp       = 0;
    nelem    = 0;
    truesize = 0;
}

template <class T>
inline
Array<T>::~Array ()
{
    clear();
}

template <class T>
inline
bool
Array<T>::ready () const
{
    return vp != 0 && nelem != 0;
}

template <class T>
inline
long
Array<T>::length () const
{
    return nelem;
}

template <class T>
inline
long
Array<T>::trueSize () const
{
    return truesize;
}

template <class T>
inline
T&
Array<T>::operator[] (long i)
{
    boxAssert(vp != 0);
    boxAssert(i >= 0 && i < nelem);
    return vp[i];
}

template <class T>
inline
const T&
Array<T>::operator[] (long i) const
{
    boxAssert(vp != 0);
    boxAssert(i >= 0 && i < nelem);
    return vp[i];
}

template <class T>
inline
T&
Array<T>::get (long i)
{
    boxAssert(vp != 0);
    boxAssert(i >= 0 && i < nelem);
    return vp[i];
}

template <class T>
inline
const T&
Array<T>::get (long i) const
{
    boxAssert(vp != 0);
    boxAssert(i >= 0 && i < nelem);
    return vp[i];
}

template <class T>
inline
T*
Array<T>::dataPtr ()
{
    return vp;
}

template <class T>
inline
const T*
Array<T>::dataPtr () const
{
    return vp;
}

template <class T>
inline
void
Array<T>::swap (long i,
                long j)
{
    boxAssert(i >= 0 && i < nelem);
    boxAssert(j >= 0 && j < nelem);
    T tmp = vp[i];
    vp[i] = vp[j];
    vp[j] = tmp;
}

template <class T>
inline
bool
Array<T>::operator!= (const Array<T>& rhs) const
{
    return !(operator==(rhs));
}
--- */

#ifdef BL_AUTO_INSTANTIATE
#include <Array.C>
#endif

#endif /*_ARRAY_H_*/
