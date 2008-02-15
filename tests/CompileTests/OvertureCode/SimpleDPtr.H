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

#ifndef _SIMPLEDPTR_H_
#define _SIMPLEDPTR_H_

// *wdh*
#include "OvertureDefine.h"

//
// $Id: SimpleDPtr.H,v 1.1 2004/07/07 10:27:12 dquinlan Exp $
//

#include <Assert.H>
#include <stddef.h>
#include <DPtr.H>

//
//@Man:
//@Memo: A Simple Smart Data Pointer
/*@Doc:

  This is the simplest class derived from DPtrRep, supplying DPtr function
  using a fixed memory arena provided by BArena.

  This class does NOT provide a copy constructor or assignment operator.
*/

template <class T>
class SimpleDPtr
    : public DPtrRep<T>
{
public:
 
    /*@ManDoc: Construct a new object using the given arena, if supplied,
               or a BArena otherwise.  size is the number of objects of
               type T that the arena will hold.  Calls define(size).
    */
    SimpleDPtr (size_t size,
                Arena* arena = 0);
    //
    //@ManDoc: The virtual destructor -- calls clear().
    //
    virtual ~SimpleDPtr ();
    //
    //@ManDoc: Free the arena space.
    //
    virtual void clear ();
    //
    //@ManDoc: Change the size of the arena to newsize.
    //
    virtual void resize (size_t newsize);

    /*@ManDoc: Set the initial arena size.  The size is specified as the
               number of objects of type T that the arena will hold.
               It is illegal to call this once the size of the arena
               has already been set; i.e. don't call this more than once.
    */
    virtual void define (size_t size);
 
    /*@ManDoc: Smart array access.  This will abort if the pointer is null
               or if the argument is out of range.
    */
    virtual T& operator[] (long) const;
    //
    //@ManDoc: Return the arena size in units the size of type T.
    //
    virtual size_t length ();
    //
    //@ManDoc: Return the arena size in units the size of type T.
    //
    virtual size_t size () const;

protected:
    //
    // The default constructor.  Here so we can be a parent class.
    //
    inline SimpleDPtr ();
    //
    // The arena pointer.
    //
    T* dp;
    //
    // The size of the arena.
    //
    size_t currentsize;

private:
    //
    // Disallow these.
    //
    SimpleDPtr (const SimpleDPtr<T> &);
    SimpleDPtr<T>& operator= (const SimpleDPtr<T>&);
};

//
// Inlines.
//

template <class T>
inline
SimpleDPtr<T>::SimpleDPtr ()
        : dp(0),
          currentsize(0)
{}

#ifdef BL_AUTO_INSTANTIATE
#include <SimpleDPtr.C>
#endif

#endif /*_SIMPLEDPTR_H_*/
