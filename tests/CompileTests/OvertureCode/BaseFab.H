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

#ifndef _BASEFAB_H_
#define _BASEFAB_H_

// *wdh*
#include "OvertureDefine.h"

//
// $Id: BaseFab.H,v 1.1 2004/07/07 10:27:00 dquinlan Exp $
//

#include <Assert.H>
#include <Boolean.H>
#include <Box.H>
#include <DPtr.H>
#include <PArray.H>

#ifdef BL_USE_POINTLIB
#ifndef BL_CRAY_BUG_DEFARG
#include <PointBaseFab.H>
#include <PointDomain.H>
#endif /*BL_CRAY_BUG_DEFARG*/
#endif /*BL_USE_POINTLIB*/

template <class T> class BaseFab;

//
//@Man:
//@Memo: A Fortran Array-like Object
/*@Doc:

  BaseFab emulates the Fortran array concept.  
  Useful operations can be performed upon
  BaseFab's in C++, and they provide a convenient interface to
  Fortran when it is necessary to retreat into that language.
        
  `BaseFab' is a template class.  Through use of the
  template, a `BaseFab' may be based upon any class.  So far at least,
  most applications have been based upon simple types like `integer''s,
  `real*4''s, or `real*8''s.  Most applications do not use BaseFab's 
  directly, but utilize specialized classes derived from BaseFab.
        
  It will be easier to use a `BaseFab' if you understand the following
  concepts.  `BaseFab' objects depend on the dimensionality of space
  (indirectly through the DOMAIN `Box' member).  It is
  typical to define the macro `SPACEDIM' to be 1, 2, or 3 to indicate
  the dimension of space.  See the discussion of class `Box' for more
  information.  A `BaseFab' contains a `Box' DOMAIN, which indicates the
  integer indexing space over which the array is defined.  A `BaseFab'
  also has NVAR components.  By components, we mean that for each
  point in the rectangular indexing space, there are NVAR values
  associated with that point.  A Fortran array corresponding to a
  `BaseFab' would have (SPACEDIM+1) dimensions.
        
  By design, the array layout in a `BaseFab' mirrors that of a
  Fortran array.  The first index (x direction for example) varies
  most rapidly, the next index (y direction), if any, varies next
  fastest. The component index varies last, after all the spatial
  indices.
        
  It is sometimes convenient to be able to treat a sub-array within an
  existing `BaseFab' as a `BaseFab' in its own right.  This is often
  referred to as 'aliasing' the `BaseFab'.  Note that when aliasing is 
  used, the BaseFab's domain will not, in general, be the same as the 
  parent BaseFab's domain, nor will the number of components.
  BaseFab is a dimension dependent class, so SPACEDIM must be 
  defined as either 1, 2, or 3 when compiling.

  This class does NOT provide a copy constructor or assignment operator.
*/

template <class T>
class BaseFab
{
public:

    /*@ManDoc: Construct an invalid `BaseFab'.  The domain is invalid, the
               number of components is zero, and no actually array memory is
               allocated. An invalid `BaseFab' must be resized
               (see `BaseFab::resize') before use.
    */
    BaseFab ();
    //
    //@ManDoc: Make BaseFab with desired domain and number of components.
    //
    BaseFab (const Box& bx,
             int        n = 1);

    /*@ManDoc: Make a BaseFab which aliases the data in the argument BaseFab
               f. The new domain is subb, number of components is nc. The
               first component of new BaseFab is same as ns'th component of f.
    */
    BaseFab (BaseFab<T>& f,
             const Box&  subb,
             int         ns,
             int         nc);

#ifdef BL_USE_POINTLIB
#ifndef BL_CRAY_BUG_DEFARG

    /*@ManDoc: This constructor constructs a BaseFab based on the points in a
               PointBaseFab f. This constructor is only defined
               if BL\_CRAY\_BUG\_DEFARG not defined and if BL\_USE\_POINTLIB
               is defined.
    */
    BaseFab (const PointBaseFab<PointDomain,T>& f,
             const T                            v);
#endif
#endif
    //
    //@ManDoc: The (virtual) destructor deletes the array memory.
    //
    virtual ~BaseFab ();

    /*@ManDoc: This function resizes a `BaseFab' so it covers the `Box' B
               with N components.  The precise resize action is dependent
               on your implementation of class DPtr, but the default action
               is that under resizing, the memory allocated for the `BaseFab'
               only grows and never shrinks.  This function is particularly
               useful when a `BaseFab' is used as a temporary space which
               must be a different size whenever it is used.  Resize'ing a
               temp will often be faster than re-allocating a `BaseFab'
               because memory allocation can often be avoided.
    */
    void resize (const Box& b,
                 int        N = 1);

    /*@ManDoc: The function returns the `BaseFab' to the invalid state.  (See
               comments for constructors above.)  The memory is freed.
    */
    void clear ();
    //
    //@ManDoc: Returns the number of components (same as nComp()).
    //
    inline int nVar () const;
    //
    //@ManDoc: Returns the number of components (same as nVar()).
    //
    inline int nComp () const;
    //
    //@ManDoc: Returns the domain (box) where the array is defined.
    //
    inline const Box& box () const;

    /*@ManDoc:  Returns the domain of the PARENT `BaseFab'.  If this
                is not an alias `BaseFab', this is identical to the
                function "box".
    */
    inline const Box& pBox () const;

    /*@ManDoc: Returns a pointer to an array of SPACEDIM integers
               giving the length of the domain in each direction.
    */
    inline const int* length () const;

    /*@ManDoc: Returns a pointer to an array of SPACEDIM integers
               giving the length of the parent's domain in each direction.
               If this is not an alias `BaseFab', this is identical to the
               function "length"
    */
    inline const int* pLength () const;

    /*@ManDoc:  Returns true if the domain of the
                `BaseFab' is the same as the domain of the parent `BaseFab',
                and if the number of components is the same as the number
                of components of the parent `BaseFab'.  If the
                `BaseFab' object does not have a parent, this returns true
                (the `BaseFab' is its own parent).  Note that this does
                not unequivocally imply that a FAB is NOT aliased.
    */
    inline bool isFilled () const;
 
    /*@ManDoc: Returns the lower corner of the domain.  See
               class `Box' for analogue.
    */
    inline const IntVect& smallEnd () const;
 
    /*@ManDoc:  Returns the upper corner of the domain.  See
                class `Box' for analogue.
    */
    inline const IntVect& bigEnd () const;

    /*@ManDoc: Returns the lower corner of the domain.  Instead of
               returning them in the form of INTVECT's, as in smallEnd and
               bigEnd, it returns the values as a pointer to an array of
               constant integers.  This is useful when interfacing to
               Fortran subroutines.
    */
    inline const int* loVect () const;
 
    /*@ManDoc: Returns the upper corner of the domain.  Instead of
               returning them in the form of INTVECT's, as in smallEnd and
               bigEnd, it returns the values as a pointer to an array of
               constant integers.  This is useful when interfacing to
               Fortran subroutines.
    */
    inline const int* hiVect () const;
 
    /*@ManDoc: This is analogous to loVect, except that
               it reports on the parent's domain.
    */
    inline const int* pLoVect () const;
 
    /*@ManDoc: Thisis analogous to hiVect, except that
               it reports on the parent's domain.
    */
    inline const int* pHiVect () const;
 
    /*@ManDoc: Returns true if the domain of fab is totally contained within
               the domain of this `BaseFab'.
    */
    inline bool contains (const BaseFab<T>& fab) const;
 
    /*@ManDoc: Returns true if bx is totally contained
               within the domain of this `BaseFab'.
    */
    inline bool contains (const Box& bx) const;
 
    /*@ManDoc: Returns a pointer to an object of type T that is the
               value of the Nth component associated with the cell at the
               low end of the domain.  This is commonly used to get a pointer
               to data in the array which is then handed off to a Fortran
               subroutine.  Remember that data is stored in Fortran array
               order, with the component index coming last.   In other words,
               `dataPtr' returns a pointer to all the Nth components.
    */
    inline T* dataPtr (int N = 0);
    //
    //@ManDoc: Same as above except works on const FAB's.
    //
    inline const T* dataPtr (int N = 0) const;

    /*@ManDoc: Returns a reference to the Nth component value
               defined at position p in the domain.  This operator may be
               inefficient if the C++ compiler is unable to optimize the
               C++ code.
    */
    inline T& operator() (const IntVect& p,
                          int            N);
    //
    //@ManDoc: Same as above, except returns component 0.
    //
    inline T& operator() (const IntVect& p);
    //
    //@ManDoc: Same as above except works on const FAB's.
    //
    inline const T& operator() (const IntVect& p,
                                int            N) const;
    //
    //@ManDoc: Same as above, except returns component 0.
    //
    inline const T& operator() (const IntVect& p) const;

    /*@ManDoc: This function puts numcomp component values, starting at
               component N, from position pos in the domain into array data,
               that must be allocated by the user.
    */
    void getVal (T*             data,
                 const IntVect& pos,
                 int            N,
                 int            numcomp) const;

    /*@ManDoc: Same as above, except that starts at component 0 and
               copies ALL comps.
    */
    inline void getVal (T*             data,
                        const IntVect& pos) const;

    /*@ManDoc: The setVal functions set subregions in the `BaseFab' to a
               constant value.  This most general form specifies the sub-box,
               the starting component number, and the number of components
               to be set.
    */
    inline void setVal (const T    x,
                        const Box& bx,
                        int        nstart,
                        int        ncomp);

    /*@ManDoc: Same as above, except the number of modified components is one.
               N is the component to be modified.
    */
    inline void setVal (const T    x,
                        const Box& bx,
                        int        N);
    //
    //@ManDoc: Same as above, except the subbox defaults to the entire domain.
    //
    inline void setVal (const T x,
                        int     N);
    //
    //@ManDoc: Same as above, except ALL components are set.
    //
    inline void setVal (const T x);

    /*@ManDoc: This function is analogous to the fourth form of
               setVal above, except that instead of setting values on the
               `Box' b, values are set on the complement of b in the domain.
    */
    void setComplement (const T    x,
                        const Box& b,
                        int        ns,
                        int        num);

    /*@ManDoc: The `copy' functions copy the contents of one `BaseFab' into
               another.  The destination `BaseFab' is always the object which
               invokes the function.  This, the most general form of copy,
               specifies the contents of any sub-box srcbox in `BaseFab' src
               may be copied into a (possibly different) destbox in the
               destination `BaseFab'.  Note that although the srcbox and the
               destbox may be disjoint, they must be the same size and shape.
               If the sizes differ, the copy is undefined and a runtime error
               results.  This copy function is the only one of the copy
               functions to allow a copy between differing boxes. The user
               also specifies how many components are copied, starting at
               component srccomp in src and stored starting at component
               destcomp. Note that the actual copy is made by the
               function `performCopy' of this class.
    */
    BaseFab<T>& copy (const BaseFab<T>& src,
                      const Box&        srcbox,
                      int               srccomp,
                      const Box&        destbox,
                      int               destcomp,
                      int               numcomp);

    /*@ManDoc: As above, except the destination `Box' and the source `Box'
               are taken to be the entire domain of the destination.   A copy
               of the intersecting region is performed.  Note that the actual
               copy is made by the function `performCopy' of this
               class.
    */
    BaseFab<T>& copy (const BaseFab<T>& src,
                      int               srccomp,
                      int               destcomp,
                      int               numcomp = 1);

    /*@ManDoc: As above, except that the destination `Box' is specified,
               but the source `Box' is taken to the equal to the source
               `Box', and all components of the destination `BaseFab' are
               copied.  Note that the actual copy is made by the
               function `performCopy' of this class.
    */
    BaseFab<T>& copy (const BaseFab<T>& src,
                      const Box&        destbox);

    /*@ManDoc: As above, except that the destbox defaults to the entire domain
               of the destination BaseFab, and all components are copied.
               Note that the actual copy is made by the function
               `performCopy' of this class.
    */
    BaseFab<T>& copy (const BaseFab<T>& src);

    /*@ManDoc: Perform shifts upon the domain of the `BaseFab'. They are
               completely analogous to the corresponding `Box' functions.
               There is no effect upon the array memory.
    */
    inline BaseFab<T>& shift (const IntVect& v);

    /*@ManDoc: Perform shifts upon the domain of the `BaseFab'.  They are
               completely analogous to the corresponding `Box' functions.
               There is no effect upon the array memory.
    */
    inline BaseFab<T>& shift (int idir,
                              int n_cell);

    /*@ManDoc: Perform shifts upon the domain of the `BaseFab'.  They are
               completely analogous to the corresponding `Box' functions.
               There is no effect upon the array memory.
    */
    inline BaseFab<T>& shiftHalf (int dir,
                                  int num_halfs);

    /*@ManDoc: Perform shifts upon the domain of the `BaseFab'. They are
               completely analogous to the corresponding `Box' functions.
               There is no effect upon the array memory.
    */
    inline BaseFab<T>& shiftHalf (const IntVect& num_halfs);

protected:
    //
    // Allocates memory for the `BaseFab<T>'.
    //
    void define ();
    //
    // The function called by BaseFab copy operations.
    //
    void performCopy (const BaseFab<T>& src,
                      const Box&        srcbox,
                      int               srccomp,
                      const Box&        destbox,
                      int               destcomp,
                      int               numcomp);
#ifdef BL_USE_POINTLIB
#ifndef BL_CRAY_BUG_DEFARG
    //
    // This function is called by the `PointBaseFab' copy
    // operations.  This function is only
    // defined if BL\_CRAY\_BUG\_DEFARG is NOT defined and if
    // BL_USE_POINTLIB is defined.
    //
    void performCopy (const PointBaseFab<PointDomain,T>& src,
                      const Box&                         srcbox,
                      int                                srccomp,
                      const Box&                         destbox,
                      int                                destcomp,
                      int                                numcomp);
#endif
#endif
    //
    // This virtual function is called by the `BaseFab' setVal operations.
    //
    void performSetVal (const T    x,
                        const Box& bx,
                        int        nstart,
                        int        numcomp);
private:
    //
    // These functions are made private to prevent use of the default 
    // functions provided by the C++ compiler.
    //
    BaseFab<T>& operator= (const BaseFab<T>&);
    BaseFab (const BaseFab<T>&);
protected:
    Box  domain;   // My index space.
    Box  pdomain;  // Index space of a parent (may be same as domain).
    int  nvar;     // Number components.
    int  noffset;  // Offset to apply to dptr component.
    int  pnvar;    // Number of components in parent.
    long numpts;   // Cached number of points in FAB.
    long pnumpts;  // Cached number of points in parent.
    DPtr<T> dptr;  // Data pointer, a `smart' pointer.
};

//
// Inlines.
//

template <class T>
inline
int
BaseFab<T>::nVar () const
{
    return nvar;
}

template <class T>
inline
int
BaseFab<T>::nComp () const
{
    return nvar;
}

template <class T>
inline
const Box&
BaseFab<T>::box () const
{
    return domain;
}

template <class T>
inline
const Box&
BaseFab<T>::pBox () const
{
    return pdomain;
}

template <class T>
inline
const int*
BaseFab<T>::length () const
{
    return domain.length();
}

template <class T>
inline
const int*
BaseFab<T>::pLength () const
{
    return pdomain.length();
}

template <class T>
inline
bool
BaseFab<T>::isFilled () const
{
    return domain == pdomain && nvar == pnvar;
}

template <class T>
inline
const IntVect&
BaseFab<T>::smallEnd () const
{
    return domain.smallEnd();
}

template <class T>
inline
const IntVect&
BaseFab<T>::bigEnd () const
{
    return domain.bigEnd();
}

template <class T>
inline
const int*
BaseFab<T>::loVect () const
{
    return domain.loVect();
}

template <class T>
inline
const int*
BaseFab<T>::hiVect () const
{
    return domain.hiVect();
}

template <class T>
inline
const int*
BaseFab<T>::pLoVect () const
{
    return pdomain.loVect();
}

template <class T>
inline
const int*
BaseFab<T>::pHiVect () const
{
    return pdomain.hiVect();
}

template <class T>
inline
bool
BaseFab<T>::contains (const BaseFab<T>& fab) const
{
    return box().contains(fab.box()) && nvar <= fab.nvar;
}

template <class T>
inline
bool
BaseFab<T>::contains (const Box& bx) const
{
    return box().contains(bx);
}

template <class T>
inline
T*
BaseFab<T>::dataPtr (int n)
{
    return &dptr[(n+noffset)*pnumpts];
}

template <class T>
inline
const T*
BaseFab<T>::dataPtr (int n) const
{
    return &dptr[(n+noffset)*pnumpts];
}

template <class T>
inline
T&
BaseFab<T>::operator() (const IntVect& p,
                        int            n)
{
    return dptr[pdomain.index(p)+(n+noffset)*pnumpts];
}

template <class T>
inline
T&
BaseFab<T>::operator() (const IntVect& p)
{
    return dptr[pdomain.index(p)+(noffset)*pnumpts];
}

template <class T>
inline
const T&
BaseFab<T>::operator() (const IntVect& p,
                        int            n) const
{
    return dptr[pdomain.index(p)+(n+noffset)*pnumpts];
}

template <class T>
inline
const T&
BaseFab<T>::operator() (const IntVect& p) const
{
    return dptr[pdomain.index(p)+(noffset)*pnumpts];
}

template <class T>
inline
void
BaseFab<T>::getVal (T*             data,
                    const IntVect& pos) const
{
    getVal(data,pos,0,nvar);
}


template <class T>
inline
BaseFab<T>&
BaseFab<T>::shift (const IntVect& v)
{
    domain  += v;
    pdomain += v;
    return *this;
}

template <class T>
inline
BaseFab<T>&
BaseFab<T>::shift (int idir,
                   int n_cell)
{
    domain.shift(idir,n_cell);
    pdomain.shift(idir,n_cell);
    return *this;
}

template <class T>
inline
BaseFab<T> &
BaseFab<T>::shiftHalf (const IntVect& v)
{
    domain.shiftHalf(v);
    pdomain.shiftHalf(v);
    return *this;
}

template <class T>
inline
BaseFab<T> &
BaseFab<T>::shiftHalf (int idir,
                       int n_cell)
{
    domain.shiftHalf(idir,n_cell);
    pdomain.shiftHalf(idir,n_cell);
    return *this;
}

template <class T>
inline
void
BaseFab<T>::setVal (const T val)
{
    performSetVal(val,box(), 0, nvar);
}

template <class T>
inline
void
BaseFab<T>::setVal (const T    x,
                    const Box& bx,
                    int        n)
{
    performSetVal(x,bx,n,1);
}

template <class T>
inline
void
BaseFab<T>::setVal (const T x,
                    int     n)
{
    performSetVal(x,domain,n,1);
}

template <class T>
inline
void
BaseFab<T>::setVal (const T    x,
                    const Box& b,
                    int        ns,
                    int        num)
{
    performSetVal(x,b,ns,num);
}

#ifdef BL_AUTO_INSTANTIATE
#include <BaseFab.C>
#endif

#endif /*_BASEFAB_H_*/
