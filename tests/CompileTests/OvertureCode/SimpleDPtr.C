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
// $Id: SimpleDPtr.C,v 1.1 2004/07/07 10:27:12 dquinlan Exp $
//

#include <BArena.H>

template <class T>
SimpleDPtr<T>::SimpleDPtr (size_t _size,
                           Arena* _arena)
    : DPtrRep<T>(_arena),
      dp(0)
{
    if (arena == 0)
    {
        static BArena _builtin_BArena;
        arena = &_builtin_BArena;
    }
    define(_size);
}

template <class T>
SimpleDPtr<T>::~SimpleDPtr ()
{
    clear();
}

template <class T>
void
SimpleDPtr<T>::clear ()
{
    if (arena)
        arena->free(dp);
    dp = 0;
    currentsize = 0;
}

template <class T>
void
SimpleDPtr<T>::resize (size_t _size)
{
    if (_size > currentsize)
    {
        T *g = (T*) arena->alloc(_size * sizeof(T));
        for (size_t i = 0; i < currentsize; ++i)
            g[i] = dp[i];
        delete [] dp;
        dp = g;
        currentsize = _size;
    }
}

template <class T>
void
SimpleDPtr<T>::define (size_t _size)
{
    boxAssert(dp == 0);
    dp = (T*) arena->alloc(_size*sizeof(T));
    currentsize = _size;
    boxAssert(dp != 0);
}

template <class T>
T&
SimpleDPtr<T>::operator[] (long n) const
{
    boxAssert(n >= 0 && n < currentsize);
    return dp[n];
}

template <class T>
size_t
SimpleDPtr<T>::length ()
{
    return currentsize;
}

template <class T>
size_t
SimpleDPtr<T>::size () const
{
    return currentsize;
}
