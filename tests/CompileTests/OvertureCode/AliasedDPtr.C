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

//
// $Id: AliasedDPtr.C,v 1.1 2004/07/07 10:26:59 dquinlan Exp $
//

#include <AliasedDPtr.H>

#ifndef _ALIASEDDPTR_C_
#define _ALIASEDDPTR_C_

template <class T>
AliasedDPtr<T>::AliasedDPtr (size_t _size)
{
    dptr        = new SimpleDPtr<T>(_size);
    currentsize = _size;
    offset      = 0;

    if (dptr.isNull())
        BoxLib::OutOfMemory(__FILE__, __LINE__);
}

template <class T>
AliasedDPtr<T>::AliasedDPtr (AliasedDPtr<T>* _asdptr,
                             size_t          _size,
                             int             _offset)
{
    dptr        = _asdptr->dptr;
    offset      = _offset;
    currentsize = _size;
}

template <class T>
AliasedDPtr<T>::~AliasedDPtr ()
{}

template <class T>
size_t
AliasedDPtr<T>::size () const
{
    return currentsize;
}

template <class T>
T&
AliasedDPtr<T>::operator[] (long n) const
{
    return (*dptr)[n + offset];
}

template <class T>
void
AliasedDPtr<T>::define (size_t _size)
{
    dptr        = new SimpleDPtr<T>(_size);
    currentsize = _size;
    offset      = 0;

    if (dptr.isNull())
        BoxLib::OutOfMemory(__FILE__, __LINE__);
}

template <class T>
void
AliasedDPtr<T>::resize (size_t _size)
{
    dptr        = new SimpleDPtr<T>(_size);
    currentsize = _size;
    offset      = 0;

    if (dptr.isNull())
        BoxLib::OutOfMemory(__FILE__, __LINE__);
}

template <class T>
void
AliasedDPtr<T>::clear ()
{
    dptr = 0;
}

#endif // _ALIASEDDPTR_C_
