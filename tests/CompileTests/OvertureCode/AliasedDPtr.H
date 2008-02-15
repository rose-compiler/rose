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

#ifndef _ALIASEDDPTR_H_
#define _ALIASEDDPTR_H_

//
// $Id: AliasedDPtr.H,v 1.1 2004/07/07 10:27:00 dquinlan Exp $
//

#include <Assert.H>
#include <stddef.h>
#include <DPtr.H>
#include <SimpleDPtr.H>
#include <Pointers.H>

// *wdh*
#include "OvertureDefine.h"

//
//@Man:
//@Memo: A Shared and Counted DPtr
/*@Doc:

  This class uses a `LnClassPtr<SimpleDPtr<T>>' to allow several Dptrs
  to address the same area of memory.

  This class does NOT provide a copy constructor or assignment operator.
 */

template <class T>
class AliasedDPtr
    : public DPtrRep<T>
{
public:

    /*@ManDoc: Construct a new internal LnClassPtr<SimpleDPtr<T>> of
               the specifed size.
    */
    AliasedDPtr (size_t size);

    /*@ManDoc: Construct a new object referencing the same internal
               representation as the old one.  Store size as the current
               size regardless of the size of *dptr, and have this object
               point to a location in memory offset units (the size of the
               type T) from the location to which the internal object points.
    */
    AliasedDPtr (AliasedDPtr<T>* dptr,
                 size_t          size,
                 int             offset = 0);
    //
    //@ManDoc: The destructor.
    //
    virtual ~AliasedDPtr ();

    /*@ManDoc: Smart array access, taking into account the current memory
               offset.
    */
    virtual T& operator[] (long) const;

    /*@ManDoc: Decrement the count on the internal object, remove it if this
               is the last alias, and set this object to point to null.
    */
    virtual void clear ();

    /*@ManDoc: Disassociate this object from the current internal
               representation and then create a new one of the specified
               size.
    */
    virtual void resize (size_t newsize);

    /*@ManDoc: Create in initial internal object.  The size is specified
               as the number of objects of type T that the arena will hold.
    */
    virtual void define (size_t size);
    //
    //@ManDoc: Returns the current arena size, in units of the size of type T.
    //
    virtual size_t size () const;
protected:
    LnClassPtr< SimpleDPtr<T> > dptr;
    int                         offset;
    size_t                      currentsize;
private:
    //
    // These are disallowed.
    //
    AliasedDPtr (const AliasedDPtr<T> &);
    AliasedDPtr<T>& operator= (const AliasedDPtr<T>&);
};

#ifdef BL_AUTO_INSTANTIATE
#include <AliasedDPtr.C>
#endif 

#endif /*_ALIASEDDPTR_H_*/
