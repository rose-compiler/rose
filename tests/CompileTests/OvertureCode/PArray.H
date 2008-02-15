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

#ifndef _PARRAY_H_
#define _PARRAY_H_

// *wdh*
#include "OvertureDefine.h"

//
// $Id: PArray.H,v 1.1 2004/07/07 10:27:11 dquinlan Exp $
//

#include <Array.H>
#include <Boolean.H>

//
//@Man:
//@Memo:
/*@Doc: An enum that controls whether or not class PArray<T>
         manages the pointed to objects, as well as the pointer itself.
         Valid values are PArrayNoManage and PArrayManage.
*/

enum PArrayPolicy
{
    PArrayNoManage = 0,
    PArrayManage   = 1
};

//
//@Man:
//@Memo: An Array of Pointers to Objects of Type T
/*@Doc:

  The PArray<T> class is similar to the Array<T> class except that it
  implements an array of pointers to objects of type T rather than an array
  of the objects themselves.  As with Array<T> objects, PArray<T> objects
  know about their length, can be dynamically resized and have built-in
  automatic bounds checking.

  When a PArray<T> object is destructed, should only the pointers be
  destructed or should the objects to which they point also be deleted?
  The PArray<T> class allows for both scenarios.  By default, it will
  only remove the pointers.  If the array is constructed with the
  PArrayManage argument it will also delete all the objects pointed to
  in the array.

  This is a concrete class, not a polymorphic one.

  PArray<T>s do NOT provide a copy constructor or an assignment operator.
*/

template <class T>
class PArray
{
public:
    //
    // The following two constructors are inlined here because g++ has
    // a problem with defining the first as an inline function
    // after the class declaration and the second in the relevant .C file.
    //

    /*@ManDoc: Constructs a PArray<T> with no elements.  The default memory
               management policy is not to delete the underlying objects
               upon destruction.
    */
    inline PArray (PArrayPolicy _managed = PArrayNoManage);

    /*@ManDoc: Constructs a PArray<T> of length len of null pointers.  
               The default memory management policy is not to delete the
               underlying objects upon destruction.
    */
    PArray (int    len,
            PArrayPolicy _managed = PArrayNoManage);

    /*@ManDoc: The destructor.  If the memory management policy was
               set to Manage upon construction, this destructor will
               also delete all objects referenced by the pointers in
               this array.
    */
    ~PArray ();
    //
    //@ManDoc: Returns true if the i'th element is not the null pointer.
    //
    inline bool defined (int i) const;

    /*@ManDoc: Returns a constant reference to the object pointed to by the
               i'th element in this PArray<T>.  Performs bounds checking
               when the library is compiled in debug mode.
    */
    inline const T& operator[] (int i) const;

    /*@ManDoc: Returns a reference to the object pointed to by the
               i'th element in this PArray<T>.  Performs bounds checking
               when the library is compiled in debug mode.
    */
    inline T& operator[] (int i);

    /*@ManDoc: Returns a constant reference to the object pointed to by the
               i'th element in the PArray.  It is an error if the i'th element
               is the null pointer.
    */
    inline const T& get (int i) const;

    /*@ManDoc: Returns a reference to the object pointed to by the i'th
               element in the PArray.  It is an error if the i'th element
               is the null pointer.
    */
    inline T& get (int i);

    /*@ManDoc: Defines the i'th element of this PArray<T> to the pointer
               value elem.  It is an error if the i'th element previously
               had a value.
    */
    inline void set (int i,
                     T*  elem);
    //
    //@ManDoc: Returns the length of the PArray<T>.
    //
    inline int length () const;

    /*@ManDoc: This function sets each pointer in this PArray<T> to null.
               If the memory management policy was set to Manage
               upon construction, this function will also delete all objects
               referenced by the pointers in this array.
    */
    void clear ();

    /*@ManDoc: The i'th element in this PArray<T> is set to null.
               If the memory management policy was set to Manage
               upon construction, this function will also delete the object
               referenced by that pointer.
    */
    void clear (int i);

    /*@ManDoc: This function changes the size of the array to newsize
               while preserving the value of as many array elements as
               possible.  If newsize is greater than the current length,
               the array is grown and the new elements are null pointers.
               If newsize is less than the current length the array is
               cropped with the remaining elements retaining their
               original pointer values. If the memory management policy
               was set to Manage, any cropped pointers are deleted.
    */
    void resize (int newsize);

    /*@ManDoc: Removes the i'th element from the PArray and returns the
               pointer to the calling function.  Nulls out the pointer
               in the PArray<T>.
    */
    inline T* remove (int i);
    //
    //@ManDoc: Returns true if the array is not empty.
    //
    inline bool ready () const;

protected:
    //
    // The underlying representation.
    //
    Array<void*> vp;
    //
    // The memory management policy.
    //
    PArrayPolicy managed;

private:
    //
    // Disallow these.
    //
    PArray (const PArray<T>&);
    PArray<T> operator= (const PArray<T>&);
};

//
// Inlines.
//

template <class T>
inline
PArray<T>::PArray (PArrayPolicy _managed)
    : vp(),
      managed(_managed)
{}

template <class T>
inline
bool
PArray<T>::defined (int i) const
{
    return vp[i] != 0;
}

template <class T>
inline
const T&
PArray<T>::operator[] (int i) const
{
    boxAssert(vp[i] != 0);
    return *((T*)(vp[i]));
}

template <class T>
inline
T&
PArray<T>::operator[] (int i)
{
    boxAssert(vp[i] != 0);
    return *((T*)(vp[i]));
}

template <class T>
inline
const T&
PArray<T>::get (int i) const
{
    boxAssert(vp[i] != 0);
    return *((T*)(vp[i]));
}

template <class T>
inline
T&
PArray<T>::get (int i)
{
    boxAssert(vp[i] != 0);
    return *((T*)(vp[i]));
}

template <class T>
inline
void
PArray<T>::set (int i,
                T*  elem)
{
    boxAssert(vp[i] == 0);
    vp[i] = elem;
}

template <class T>
inline
int
PArray<T>::length () const
{
    return vp.length();
}

template <class T>
inline
T*
PArray<T>::remove (int i)
{
    boxAssert(vp[i] != 0);
    void* tmp = vp[i];
    vp[i]     = 0;
    return (T*) tmp;
}

template <class T>
inline
bool
PArray<T>::ready () const
{
    return vp.ready();
}

#ifdef BL_AUTO_INSTANTIATE
#include <PArray.C>
#endif

#endif /*_PARRAY_H_*/
