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

#ifndef _USECOUNT_H_
#define _USECOUNT_H_

//
// $Id: UseCount.H,v 1.1 2004/07/07 10:27:13 dquinlan Exp $
//

#include <Assert.H>
#include <Boolean.H>
#include <BoxLib.H>

//
//@Man:
//@Memo: A Class Encapsulating Reference Counts
/*@Doc:

  This class encapsulates reference counts.
*/

class UseCount
{
public:
    //
    //@ManDoc: Construct a UseCount initialized to one.
    //
    UseCount ();
    //
    //@ManDoc: The copy constructor -- bumps reference count.
    //
    UseCount (const UseCount& rhs);
 
    /*@ManDoc: The assignment operator.  Increments the reference count on
               rhs, decrements the reference count on this, and then makes
               this and rhs point to the same reference count.
    */
    UseCount& operator= (const UseCount& rhs);
    //
    //@ManDoc: The destructor -- decrements the reference count.
    //
    ~UseCount ();
    //
    //@ManDoc: The UseCount is unique if the reference count == 1.
    //
    bool unique () const;
    //
    //@ManDoc: Returns the reference count.
    //
    int linkCount () const;

private:
    //
    // A pointer to the reference count.
    //
    unsigned int* cnt;
    //
    // Decrement the reference count and delete the reference
    // counter if there are no more references.
    //
    void decrement ();
};

//
// Inlines.
//

inline
UseCount::UseCount ()
    : cnt(new unsigned int(1))
{
    if (cnt == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
}

inline
UseCount::UseCount (const UseCount& rhs)
    : cnt(rhs.cnt)
{
    ++*cnt;
}

inline
bool
UseCount::unique () const
{
    return *cnt == 1;
}

inline
void
UseCount::decrement ()
{
    if (unique())
    {
        delete cnt;
        cnt = 0;
    }
    else
        --*cnt;
}

inline
UseCount&
UseCount::operator= (const UseCount& rhs)
{
    ++*rhs.cnt;
    decrement();
    cnt = rhs.cnt;
    return *this;
}

inline
UseCount::~UseCount ()
{
    decrement();
}

inline
int
UseCount::linkCount () const
{
    return *cnt;
}

#endif /*_USECOUNT_H_*/
