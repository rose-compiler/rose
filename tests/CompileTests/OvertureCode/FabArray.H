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

#ifndef _FABARRAY_H_
#define _FABARRAY_H_

// *wdh*
#include "OvertureDefine.h"

//
// $Id: FabArray.H,v 1.1 2004/07/07 10:27:03 dquinlan Exp $
//

#include <Assert.H>
#include <PArray.H>
#include <Array.H>

#include <Boolean.H>
#include <Box.H>
#include <BoxLib.H>
#include <BoxArray.H>
#include <BoxAssoc.H>
#include <FArrayBox.H>

/*@ManDoc: An enumumeration that controls whether or not the memory for a FAB
           will actually be allocated on construction of a FabArray.
           Possible values are: Fab\_noallocate and Fab\_allocate.
*/

enum FabAlloc
{
    Fab_noallocate = 0,
    Fab_allocate
};

//
//@Man:
//@Memo: A Collection of Fortran Array-like Objects
/*@Doc:

  The FabArray<T,FAB> class implements a collection (stored as an array) of
  Fortran array-like objects.  The parameterized type FAB is intended to be
  any class derived from BaseFab<T>.  For example, FAB may be a BaseFab of
  integers, so we could write:

    FabArray< int,BaseFab<int> > int\_fabs;

  Then int\_fabs is a FabArray that can hold a collection of BaseFab<int>
  objects.

  FabArray is not just a general container class for Fortran arrays.  It is
  intended to hold "grid" data for use in finite difference calculations in
  which the data is defined on a union of (usually disjoint) rectangular
  regions embedded in a uniform index space.  This region, called the valid
  region, is represented by a BoxArray.  For the purposes of this discussion,
  the Kth Box in the BoxArray represents the interior region of the Kth grid.

  Since the intent is to be used with finite difference calculations a
  FabArray also includes the notion of a boundary region for each grid.  The
  boundary region is specified by the ngrow parameter which tells the FabArray
  to allocate each FAB to be ngrow cells larger in all directions than the
  underlying Box.  The larger region covered by the union of all the FABs is
  called the region of definition.  The underlying notion is that the valid
  region contains the grid interior data and the region of definition includes
  the interior region plus the boundary areas.

  Operations are available to copy data from the valid regions into these
  boundary areas where the two overlap.  The number of components, that is,
  the number of values that can be stored in each cell of a FAB, is either
  given as an argument to the constructor or is inherent in the definition of
  the underlying FAB.  Each FAB in the FabArray will have the same number of
  components.  Finally, an optional  BoxAssoc may be defined to construct a
  nearest-neighbor list for each grid so that copy-on-intersect operations
  can be efficiently implemented.

  In summary, a FabArray is an array of FABs.  The Kth element contains a FAB
  that holds the data for the Kth grid, a Box that defines the valid region
  of the Kth grid, and, optionally, a BoxAssoc object that maintains a list of
  the neighbor grids for fast access in copy operations.

  A typical use for a FabArray would be to hold the solution vector or
  right-hand-side when solving a linear system of equations on a union of
  rectangular grids.  The copy operations would be used to copy data from the
  valid regions of neighboring grids into the boundary regions after each
  relaxation step of the iterative method.  If a multigrid method is used, a
  FabArray could be used to hold the data at each level in the multigrid
  hierarchy.

  This class is a concrete class not a polymorphic one.

  This class does NOT provide a copy constructor or assignment operator.
*/

template <class T, class FAB>
class FabArray
{
public:
    //
    //@ManDoc: Constructs an empty FabArray<T,FAB>.
    //
    FabArray ();

    /*@ManDoc: Construct a FabArray<T,FAB> with a valid region defined by bxs
               and a region of definition defined by the grow factor ngrow.
               If mem\_mode is defined to be Fab\_allocate then FABs are
               allocated for each Box in the BoxArray.  The size of the Kth
               FAB is given by bxs[K] grown by ngrow.  If mem\_mode is defined
               to be Fab\_noallocate, then no FABs are allocated at this time,
               but can be defined later.  The number of components in each
               FAB is not specified and is expected to be implicit in the
               definition of the FAB class.  That is, the FAB constructor will
               take only a Box argument.  No BoxAssoc is defined, but one can
               be defined later.  Call this constructor number two.
    */
    FabArray (const BoxArray& bxs,
              int             ngrow,
              FabAlloc        mem_mode = Fab_allocate);

    /*@ManDoc: This constructor is similar to constructor number two except
               that it also specifies a BoxAssoc.  The valid region is
               defined by the BoxArray contained in the BoxAssoc.  Call
               this constructor number three.
    */
    FabArray (const BoxAssoc& ba,
              int             ngrow,
              FabAlloc        mem_mode = Fab_allocate);

    /*@ManDoc: This constructor is similar to constructor number two except
               that it also specifies the number of components for each FAB.
               The FAB constructor used here will take both a Box and nvar
               (the number of components) as arguments.
    */
    FabArray (const BoxArray& bxs,
              int             nvar,
              int             ngrow,
              FabAlloc        mem_mode = Fab_allocate);

    /*@ManDoc: This constructor is similar to constructor number three except
               except that it also specifies the number of components (nvar)
               for each FAB.
    */
    FabArray (const BoxAssoc& ba,
              int             nvar,
              int             ngrow,
              FabAlloc        mem_mode = Fab_allocate);
    //
    //@ManDoc: The (virtual) destructor -- deletes all FABs in the array.
    //
    virtual ~FabArray ();

    /*@ManDoc: Define this FabArray identically to that performed by
               the constructor having an analogous function signature.
               This is only valid if this FabArray was defined using
               the default constructor.
    */
    void define (const BoxArray& bxs,
                 int             ngrow,
                 FabAlloc        mem_mode);

    /*@ManDoc: Define this FabArray identically to that performed by
               the constructor having an analogous function signature.
               This is only valid if this FabArray was defined using
               the default constructor.
    */
    void define (const BoxAssoc& ba,
                 int             ngrow,
                 FabAlloc        mem_mode);

    /*@ManDoc: Define this FabArray identically to that performed by
               the constructor having an analogous function signature.
               This is only valid if this FabArray was defined using
               the default constructor.
    */
    void define (const BoxArray& bxs,
                 int             nvar,
                 int             ngrow,
                 FabAlloc        mem_mode);

    /*@ManDoc: Define this FabArray identically to that performed by
               the constructor having an analogous function signature.
               This is only valid if this FabArray was defined using
               the default constructor.
    */
    void define (const BoxAssoc& ba,
                 int             nvar,
                 int             ngrow,
                 FabAlloc        mem_mode);

    /*@ManDoc: This function builds a BoxAssoc (if necessary) based on the
               value of cachewidth.  The BoxAssoc then becomes the default
               for this FabArray.  The nearest-neighbor list in the BoxAssoc
               is used to enable faster access for communication-intensive
               routines such as iterative solvers.
    */
    inline void setCacheWidth (int cw);

    /*@ManDoc: Returns true if the FabArray is well-defined.  That is,
               if FABs are allocated for each Box in the BoxArray and the
               sizes of the FABs and the number of components are consistent
               with the definition of the FabArray.
    */
    bool ok () const;
    //
    //@ManDoc: Returns the grow factor that defines the region of definition.
    //
    inline int nGrow () const;

    /*@ManDoc: Returns a constant reference to the BoxArray that defines the
               valid region associated with this FabArray.
    */
    inline const BoxArray& boxArray () const;

    /*@ManDoc: Returns a constant reference to the Kth Box in the BoxArray.
               That is, the valid region of the Kth grid.
    */
    inline const Box& box (int K) const;
    //
    //@ManDoc: Returns the length of this FabArray; i.e. the number of FABs.
    //
    inline int length () const;

    /*@ManDoc: Returns a constant reference to the FAB associated with the
               Kth element.
    */
    inline const FAB& operator[] (int K) const;
    //
    //@ManDoc: Returns a reference to the FAB associated with the Kth element.
    //
    inline FAB& operator[] (int K);
    //
    //@ManDoc: Returns number of variables associated with each point (nvar).
    //
    inline int nVar () const;
    //
    //@ManDoc: Returns number of variables associated with each point (nvar).
    //
    inline int nComp () const;
    //
    //@ManDoc: Explicitly set the Kth FAB in the FabArray to point to elem.
    //
    void setFab (int  K,
                 FAB* elem);

    /*@ManDoc: Removes the Kth FAB from the FabArray and returns it to the
               caller.  It leaves the Kth element undefined.  It is the
               callers responsibility to delete the pointer when no
               longer needed.
    */
    inline FAB* remove (int K);

    /*@ManDoc: Delete the Kth element in the FabArray.  It leaves the Kth
               element in the FabArray undefined.
    */
    inline void clear (int K);

    /*@ManDoc: Set all components in the valid region of each FAB in the
               FabArray to val.
    */
    void setVal (T val);

    /*@ManDoc: Set the value of num\_comp components in the valid region of
               each FAB in the FabArray, starting at component comp to val.
               Also set the value of nghost boundary cells.
    */
    void setVal (T   val,
                 int comp,
                 int num_comp,
                 int nghost = 0);

    /*@ManDoc: Set the value of num\_comp components in the valid region of
               each FAB in the FabArray, starting at component comp, as well
               as nghost boundary cells, to val, provided they also intersect
               with the Box region.
    */
    void setVal (T          val,
                 const Box& region,
                 int        comp,
                 int        num_comp,
                 int        nghost = 0);

    /*@ManDoc: Set all components in the valid region of each FAB in the
               FabArray to val, including nghost boundary cells.
    */
    inline void setVal (T   val,
                        int nghost);

    /*@ManDoc: Set all components in the valid region of each FAB in the
               FabArray to val, including nghost boundary cells, that also
               intersect the Box region.
    */
    inline void setVal (T          val,
                        const Box& region,
                        int        nghost);
    //
    //@ManDoc: Set all values in the boundary region to val.
    //
    void setBndry (T val);

    /*@ManDoc: Set ncomp values in the boundary region, starting at
               start\_comp to val.
    */
    void setBndry (T   val,
                   int strt_comp,
                   int ncomp);

    /*@ManDoc: Copy on intersection within FabArray.  Data is copied from
               valid regions to intersecting regions of definition.  The
               purpose is to fill in the boundary regions of each FAB in
               the FabArray.  Uses the local BoxAssoc.
    */
    inline void filBndry ();

    /*@ManDoc: Copy on intersection within FabArray.  Data is copied from
               valid regions to intersecting regions of definition.  The
               purpose is to fill in the boundary regions of each FAB in
               the FabArray.  Uses the local BoxAssoc, but only copies
               num\_comp components starting at start\_comp.
    */
    inline void filBndry (int start_comp,
                          int num_comp);

    /*@ManDoc: Copy on intersection within FabArray.  Data is copied from
               valid regions to intersecting regions of definition.  The
               purpose is to fill in the boundary regions of each FAB in
               the FabArray.  Uses the supplied BoxAssoc.
    */
    void filBndry (const BoxAssoc& ba);

    /*@ManDoc: Copy on intersection within FabArray.  Data is copied from
               valid regions to intersecting regions of definition.  The
               purpose is to fill in the boundary regions of each FAB in
               the FabArray.  Uses the supplied BoxAssoc,  but only copies
               num\_comp components starting at start\_comp.
    */
    void filBndry (const BoxAssoc& ba,
                   int             start_comp,
                   int             num_comp);

    /*@ManDoc: This function copies data from fa to this FabArray.  Each FAB
               in fa is intersected with all FABs in this FabArray and a copy
               is performed on the region of intersection.  The intersection
               is restricted to the valid region of each FAB.
    */
    void copy (const FabArray<T,FAB>& fa);

    /*@ManDoc: This function copies data from src to this FabArray.  Each FAB
               in src is intersected with all FABs in this FabArray and a copy
               is performed on the region of intersection.  The intersection
               is restricted to the num\_comp components starting at src\_comp
               in the FabArray src, with the destination components in this
               FabArray starting at dest\_comp.  nghost boundary zones are
               included in the intersection as well.  This assumes that
               the source and destination FabArray have identical valid
               regions.

    */
    void copy (const FabArray<T,FAB>& src,
               int                    src_comp,
               int                    dest_comp,
               int                    num_comp,
               int                    nghost=0);

    /*@ManDoc: Copies the values contained in the intersection of the
               valid region of this FabArray with the FAB dest into dest.
    */
    void copy (FAB& dest) const;

    /*@ManDoc: Copies the values contained in the intersection of the
               valid region of this FabArray with the FAB dest and the Box
               subbox into that subregion of dest.
    */
    void copy (FAB&       dest,
               const Box& subbox) const;

    /*@ManDoc: Copies the values contained in the intersection of the
               num\_comp component valid region of this FabArray, starting at
               component src\_comp, with the FAB dest into dest, starting at
               component dest\_comp in dest.
    */
    void copy (FAB& dest,
               int  src_comp,
               int  dest_comp,
               int  num_comp) const;

    /*@ManDoc: Copies the values contained in the intersection of the
               num\_comp component valid region of this FabArray, starting at
               component src\_comp, with the FAB dest and the Box subbox, into
               dest, starting at component dest\_comp in dest.
    */
    void copy (FAB&       dest,
               const Box& subbox,
               int        src_comp,
               int        dest_comp,
               int        num_comp) const;

    /*@ManDoc: Copies from the FabArray into the FAB dest.  The search for
               FABs in the FabArray that intersect dest is restricted to the
               nearest neighbor list of the Kth FAB given in the BoxAssoc ba.
               The intention here is that dest is a FAB associated with the
               Kth region and a fast copy is required.
    */
    void copy (FAB&            dest,
               const BoxAssoc& ba,
               int             K) const;

    /*@ManDoc: Copies from the FabArray into the FAB dest.  The search for
               FABs in the FabArray that intersect dest is restricted to the
               nearest neighbor list of the Kth FAB given in the BoxAssoc ba.
               The intention here is that dest is a FAB associated with the
               Kth region and a fast copy is required.  The copy is further
               restricted to the intersection of Box subbox with dest.
    */
    void copy (FAB&            dest,
               const Box&      subbox,
               const BoxAssoc& ba,
               int             K) const;

    /*@ManDoc: Copies from the FabArray into the FAB dest.  The search for
               FABs in the FabArray that intersect dest is restricted to the
               nearest neighbor list of the Kth FAB given in the BoxAssoc ba.
               The intention here is that dest is a FAB associated with the
               Kth region and a fast copy is required.  The copy is further
               restricted to the num\_comp component long region starting at
               src\_comp in this FabArray and going into dest\_comp component
               in dest.
    */
    void copy (FAB&            dest,
               const BoxAssoc& ba,
               int             K,
               int             src_comp,
               int             dest_comp,
               int             num_comp) const;

    /*@ManDoc: Copies from the FabArray into the FAB dest.  The search for
               FABs in the FabArray that intersect dest is restricted to the
               nearest neighbor list of the Kth FAB given in the BoxAssoc ba.
               The intention here is that dest is a FAB associated with the
               Kth region and a fast copy is required.  The copy is further
               restricted to the intersection of Box subbox with dest and
               the num\_comp component long region starting at src\_comp in
               this FabArray and going into dest\_comp component in dest.
    */
    void copy (FAB&            dest,
               const Box&      subbox,
               const BoxAssoc& ba,
               int             K,
               int             src_comp,
               int             dest_comp,
               int             num_comp) const;

    /*@ManDoc: Identical to `void copy (FAB dest, const BoxAssoc ba,
               int K) const' where the BoxAssoc is the one assiciated with
               this FabArray.
    */
    inline void copy (FAB& dest,
                      int  K) const;

    /*@ManDoc: Identical to `void copy (FAB dest, const Box subbox,
               const BoxAssoc ba, int K) const' where the BoxAssoc is the
               one assiciated with this FabArray.
    */
    inline void copy (FAB&       dest,
                      const Box& subbox,
                      int        K) const;

    /*@ManDoc: Identical to `void copy (FAB dest, const BoxAssoc ba,
               int K, int src\_comp, int dest\_comp, int num\_comp) const'
               where the BoxAssoc is the one assiciated with this FabArray.
    */
    inline void copy (FAB& dest,
                      int  K,
                      int  src_comp,
                      int  dest_comp,
                      int  num_comp) const;

    /*@ManDoc: Identical to `void copy (FAB dest, const Box subbox,
               const BoxAssoc ba, int K, int src\_comp,
               int dest\_comp, int num\_comp) const' where the BoxAssoc is
               the one assiciated with this FabArray.
    */
    inline void copy (FAB&       dest,
                      const Box& subbox,
                      int        K,
                      int        src_comp,
                      int        dest_comp,
                      int        num_comp) const;

protected:
    int         n_grow;
    int         n_comp;
    BoxArray    bs;
    BoxAssoc*   bxskel;
    PArray<FAB> fs;

private:
    //
    // These are disallowed.
    //
    FabArray (const FabArray<T,FAB>&);
    FabArray<T,FAB>& operator= (const FabArray<T,FAB>&);
};

#ifdef BL_USE_LPARX_CONSTRUCTS
#undef BL_USE_LPARX_CONSTRUCTS
/*@ManDoc:
  The macro BL\_USE\_LPARX\_CONSTRUCTS enables the use of the "forallthis",
  "forall", "pforallthis" and "pforall" macros in some earlier versions
  of BoxLib.  The BL\_USE\_LPARX\_CONSTRUCTS macro is not turned on by default.
  If you use any of the four above macros in your code, you should probably
  replace them with the equivalent macro expansion, as the
  BL\_USE\_LPARX\_CONSTRUCTS macro will probably go away in the future.
*/
#define BL_USE_LPARX_CONSTRUCTS 1
#endif /*BL_USE_LPARX_CONSTRUCTS*/

#ifdef BL_USE_LPARX_CONSTRUCTS
#define forallthis(i) {                             \
    for (int i = 0; i < length(); i++) {

#define forall(i, fabarray) {                       \
    for (int i = 0; i < (fabarray).length(); i++) {
#define endforall }}

#define pforallthis(i) {                            \
    for (int i = 0; i < length(); i++) {

#define pforall(i, fabarray) {                      \
    for (int i = 0; i < (fabarray).length(); i++) {
#define pendforallnosync }}
#define pendforall pendforallnosync
#endif

//
// Inlines.
//

template <class T, class FAB>
inline
void
FabArray<T,FAB>::setCacheWidth (int cw)
{
    boxAssert(bs.ready());
    if (bxskel == 0)
    {
        bxskel = new BoxAssoc(bs,cw);
        if (bxskel == 0)
            BoxLib::OutOfMemory(__FILE__, __LINE__);
    }
    else
        bxskel->setCacheWidth(cw);
}

template <class T, class FAB>
inline
int
FabArray<T,FAB>::nGrow () const
{
    return n_grow;
}

template <class T, class FAB>
inline
const BoxArray&
FabArray<T,FAB>::boxArray () const
{
    return bs;
}

template <class T, class FAB>
inline
const Box&
FabArray<T,FAB>::box (int K) const
{
    return bs[K];
}

template <class T, class FAB>
inline
int
FabArray<T,FAB>::length () const
{
    return bs.length();
}

template <class T, class FAB>
inline
const FAB&
FabArray<T,FAB>::operator[] (int K) const
{
    return fs[K];
}

template <class T, class FAB>
inline
FAB&
FabArray<T,FAB>::operator[] (int K)
{
    return fs[K];
}

template <class T, class FAB>
inline
int
FabArray<T,FAB>::nVar () const
{
    return n_comp;
}

template <class T, class FAB>
inline
int
FabArray<T,FAB>::nComp () const
{
    return n_comp;
}

template <class T, class FAB>
inline
FAB*
FabArray<T,FAB>::remove (int K)
{
    return fs.remove(K);
}

template <class T, class FAB>
inline
void
FabArray<T,FAB>::clear (int K)
{
    fs.clear(K);
}

template <class T, class FAB>
inline
void
FabArray<T,FAB>::setVal (T   val,
                         int nghost)
{
    setVal(val,0,n_comp,nghost);
}

template <class T, class FAB>
inline
void
FabArray<T,FAB>::setVal (T          val,
                         const Box& region,
                         int        nghost)
{
    setVal(val,region,0,n_comp,nghost);
}

template <class T, class FAB>
inline
void
FabArray<T,FAB>::filBndry ()
{
    boxAssert(bxskel != 0);
    filBndry(*bxskel);
}

template <class T, class FAB>
inline
void
FabArray<T,FAB>::filBndry (int start_comp,
                           int num_comp)
{
    boxAssert(bxskel != 0);
    filBndry(*bxskel,start_comp,num_comp);
}

template <class T, class FAB>
inline
void
FabArray<T,FAB>::copy (FAB& dest,
                       int  K) const
{
    boxAssert(bxskel != 0);
    copy(dest,*bxskel,K);
}

template <class T, class FAB>
inline
void
FabArray<T,FAB>::copy (FAB&       dest,
                       const Box& subbox,
                       int        K) const
{
    boxAssert(bxskel != 0);
    copy(dest,subbox,*bxskel,K);
}

template <class T, class FAB>
inline
void
FabArray<T,FAB>::copy (FAB& dest,
                       int  K,
                       int  src_comp,
                       int  dest_comp,
                       int  num_comp) const
{
    boxAssert(bxskel != 0);
    copy(dest,*bxskel,K,src_comp,dest_comp,num_comp);
}

template <class T, class FAB>
inline
void
FabArray<T,FAB>::copy (FAB&       dest,
                       const Box& subbox,
                       int        K,
                       int        src_comp,
                       int        dest_comp,
                       int        num_comp) const
{
    boxAssert(bxskel != 0);
    copy(dest,subbox,*bxskel,K, src_comp,dest_comp,num_comp);
}

#ifdef BL_AUTO_INSTANTIATE
#include <FabArray.C>
#endif

#endif
