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

#ifndef ORDEREDFAB_H
#define ORDEREDFAB_H

// *wdh*
#include "OvertureDefine.h"

//
// $Id: OrderedFab.H,v 1.1 2004/07/07 10:27:11 dquinlan Exp $
//
#include <iostream.h>

#include <BaseFab.H>

//
//@Man:
//@Memo: A BaseFab Specialized on Ordering between Elements of Type T
/*@Doc: 

  An OrderedFab<T> elaborates the BaseFab<T> concept by adding 
  operations that require an ordering between elements of type T.
  Specifically, an OrderedFab allows the computation of min's, max's,
  minIndex, maxIndex, and the setting of masks.

  The operations are generally overloaded, allowing a choice
  of a very general operation to a easy-to-use very specific operation.
  Note that many operations offer the same choice of overloaded
  signatures.

  This class does NOT provide a copy constructor or assignment operator.
*/

template <class T>
class OrderedFab
    : public virtual BaseFab<T>
{
public:

    /*@ManDoc: Default constructor uses BaseFab default constructor;
               i.e. allocates no memory.
    */
    OrderedFab ();
    //
    //@ManDoc: Allocate OrderedFab for given box and number of components.
    //
    OrderedFab (const Box& b,
                int        n = 1);

    /*@ManDoc: Creates aliasing OrderedFab that uses memory in BaseFab bf.
               See aliasing constructor for BaseFab.
    */
    OrderedFab (BaseFab<T>& bf,
                Box         subb,
                int         ns,
                int         nc);
    //
    //@ManDoc: The (virtual) destructor.
    //
    virtual ~OrderedFab ();
    //
    //@ManDoc: Minimum value of given component.
    //
    T min (int comp = 0) const;
    //
    //@ManDoc: Minimum value of given component in given subbox.
    //
    T min (const Box& subbox,
           int        comp = 0) const;
    //
    //@ManDoc: Maximum value of given component.
    //
    T max (int comp = 0) const;
    //
    //@ManDoc: Maximum value of given component in given subbox.
    //
    T max (const Box& subbox,
           int        comp = 0) const;
    //
    //@ManDoc: Find location of minimum value in given component.
    //
    IntVect minIndex (int comp = 0) const;

    /*@ManDoc: Find location of minimum value in given component in
               given subbox.
    */
    IntVect minIndex (const Box& subbox,
                      int        comp = 0) const;
    //
    //@ManDoc: Find location of maximum value in given component.
    //
    IntVect maxIndex (int comp = 0) const;
 
    /*@ManDoc: Find location of maximum value in given component in given
               subbox.
    */
    IntVect maxIndex (const Box& subbox,
                      int        comp = 0) const;

    /*@ManDoc: Compute mask array with value of 1 in cells where
               OrderedFab as value less than val, 0 otherwise.
               mask is resized by this function.
               The number of cells marked with 1 returned.
    */
    int maskLT (BaseFab<int>& mask,
                T             val,
                int           comp = 0) const;
 
    /*@ManDoc: Same as above except mark cells with value less than
               or equal to val.
    */
    int maskLE (BaseFab<int>& mask,
                T             val,
                int           comp = 0) const;
    //
    //@ManDoc: Same as above except mark cells with value equal to val.
    //
    int maskEQ (BaseFab<int>& mask,
                T             val,
                int           comp = 0) const;
    //
    //@ManDoc: Same as above except mark cells with value greater than val.
    //
    int maskGT (BaseFab<int>& mask,
                T             val,
                int           comp = 0) const;
 
    /*@ManDoc: Same as above except mark cells with value greater than
               or equal to val.
    */
    int maskGE (BaseFab<int>& mask,
                T             val,
                int           comp = 0) const;

private:
    //
    // These are disallowed.
    //
    OrderedFab (const OrderedFab<T>&);
    OrderedFab<T>& operator= (const OrderedFab<T>&);
};

#ifdef BL_AUTO_INSTANTIATE
#include <OrderedFab.C>
#endif

#endif /*ORDEREDFAB_H*/
