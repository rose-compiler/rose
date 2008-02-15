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

#ifndef _ARENA_H_
#define _ARENA_H_

//
// $Id: Arena.H,v 1.1 2004/07/07 10:27:00 dquinlan Exp $
//

#include <stddef.h>

//
//@Man:
//@Memo: A Virtual Base Class for Dynamic Memory Management
/*@Doc:

  This is a virtual base class for objects that manage their own dynamic
  memory allocation.  Since it is a virtual base class, you have to derive
  something from it to use it.
*/

class Arena
{
public:

    union Word
    {
        void*  p;
        double d;
        long   l;
    };

    /*@ManDoc: A pure virtual function for allocating a dynamic memory arena
               of size sz.  A pointer to this memory should be returned.
               Also the *pt is set to the returned pointer if pt != 0;
    */
    virtual void *alloc (size_t sz, void** pt = 0) = 0;

    /*@ManDoc: A pure virtual function for deleting the arena pointed
               to by pt.
    */
    virtual void free (void* pt) = 0;

    /*@ManDoc: Given a minimum required arena size of sz bytes, this returns
               the next largest arena size that will hold an integral number
               of objects of the largest of the three types void*, long
               and double.
    */
    static size_t align (size_t sz);
};

//
// Inlines.
//

inline
size_t
Arena::align (size_t s)
{
    int x = s + sizeof(Word) - 1;
    x -= x%sizeof(Word);
    return x;
}

#endif /*_ARENA_H_*/
