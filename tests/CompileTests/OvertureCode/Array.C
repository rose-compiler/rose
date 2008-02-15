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
// $Id: Array.C,v 1.1 2004/07/07 10:27:00 dquinlan Exp $
//

#include <Assert.H>
#include <Misc.H>

template <class T>
Array<T>::Array (long     len,
                 const T& initialValue)
{
    boxAssert(len >= 0);
    nelem = len;
    vp    = new T[len];
    if (vp == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
    truesize = nelem;
    for(long i = 0; i < nelem; ++i)
        vp[i] = initialValue;
}

template <class T>
Array<T>::Array (const T* vec,
                 long     len)
{
    boxAssert(len >= 0);
    nelem = len;
    vp    = new T[len];
    if (vp == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
    truesize = nelem;
    for(long i = 0; i < nelem; ++i)
        vp[i] = vec[i];
}

template <class T>
Array<T>::Array (const Array<T>& a)
{
    nelem = a.nelem;
    vp    = new T[nelem];
    if (vp == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
    truesize = nelem;
    for (long i = 0; i < nelem; i++)
        vp[i] = a.vp[i];
}

template <class T>
Array<T>&
Array<T>::operator= (const Array<T>& sa)
{
    if (this != &sa)
    {
        clear();
        vp       = new T[sa.nelem];
        nelem    = sa.nelem;
        truesize = nelem;
        if (vp == 0)
            BoxLib::OutOfMemory(__FILE__, __LINE__);
        for(long i = 0; i < nelem; i++)
            vp[i] = sa.vp[i];
    }
    return *this;
}

template <class T>
void
Array<T>::resize (long newlen)
{
    if (newlen == nelem)
        return;
    if (newlen <= truesize)
    {
        nelem = newlen;
        return;
    }
    T* newvp = new T[newlen];
    long len = Min(newlen,nelem);
    if (newvp == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
    for (long i = 0; i < len; i++)
        newvp[i] = vp[i];
    delete [] vp;
    vp = newvp;
    nelem = newlen;
    truesize = newlen;
}

template <class T>
void Array<T>::resize (long     newlen,
                       const T& initialValue)
{
    if (newlen == nelem)
        return;
    if (newlen <= truesize)
    {
        for(long i = nelem; i < newlen; ++i)
            vp[i] = initialValue;
        nelem = newlen;
        return;
    }
    T* newvp = new T[newlen];
    if (newvp == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
    long len = Min(newlen,nelem);
    long i;
    for (i = 0; i < len; i++)
        newvp[i] = vp[i];
    for(i = len; i < newlen; ++i)
        newvp[i] = initialValue;
    delete [] vp;
    vp = newvp;
    nelem = newlen;
    truesize = newlen;
}

template <class T>
void
Array<T>::reserve (long _truesize)
{
    if (_truesize > truesize)
    {
        T* newvp = new T[_truesize];
        if (newvp == 0)
            BoxLib::OutOfMemory(__FILE__, __LINE__);
        for (long i = 0; i < nelem; i++)
            newvp[i] = vp[i];
        delete [] vp;
        vp = newvp;
        truesize = _truesize;
    }
}

template <class T>
void
Array<T>::shrinkWrap ()
{
    if (nelem != truesize)
    {
        T* newvp = new T[nelem];
        if (newvp == 0)
            BoxLib::OutOfMemory(__FILE__, __LINE__);
        for (long i = 0; i < nelem; i++)
            newvp[i] = vp[i];
        delete [] vp;
        vp = newvp;
        truesize = nelem;
    }
}

template <class T>
bool
Array<T>::operator== (const Array<T>& rhs) const
{
    bool rc = true;
    if (length() != rhs.length())
        rc = false;
    else
    {
        for (long i = 0; i < length() && rc; ++i)
            if (!((*this)[i] == rhs[i]))
                rc = false;
    }
    return rc;
}


//
// Inlines.
//

template <class T>

Array<T>::Array ()
{
    nelem    = 0;
    vp       = new T[1];
    truesize = 1;
    if (vp == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
}

template <class T>

void
Array<T>::clear ()
{
    delete [] vp;
    vp       = 0;
    nelem    = 0;
    truesize = 0;
}

template <class T>

Array<T>::~Array ()
{
    clear();
}

template <class T>

bool
Array<T>::ready () const
{
    return vp != 0 && nelem != 0;
}

template <class T>

long
Array<T>::length () const
{
    return nelem;
}

template <class T>

long
Array<T>::trueSize () const
{
    return truesize;
}

template <class T>

T&
Array<T>::operator[] (long i)
{
    boxAssert(vp != 0);
    boxAssert(i >= 0 && i < nelem);
    return vp[i];
}

template <class T>

const T&
Array<T>::operator[] (long i) const
{
    boxAssert(vp != 0);
    boxAssert(i >= 0 && i < nelem);
    return vp[i];
}

template <class T>

T&
Array<T>::get (long i)
{
    boxAssert(vp != 0);
    boxAssert(i >= 0 && i < nelem);
    return vp[i];
}

template <class T>

const T&
Array<T>::get (long i) const
{
    boxAssert(vp != 0);
    boxAssert(i >= 0 && i < nelem);
    return vp[i];
}

template <class T>

T*
Array<T>::dataPtr ()
{
    return vp;
}

template <class T>

const T*
Array<T>::dataPtr () const
{
    return vp;
}

template <class T>

void
Array<T>::swap (long i,
                long j)
{
    boxAssert(i >= 0 && i < nelem);
    boxAssert(j >= 0 && j < nelem);
    T tmp = vp[i];
    vp[i] = vp[j];
    vp[j] = tmp;
}

template <class T>

bool
Array<T>::operator!= (const Array<T>& rhs) const
{
    return !(operator==(rhs));
}
