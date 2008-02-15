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
// $Id: PArray.C,v 1.1 2004/07/07 10:27:11 dquinlan Exp $
//

template <class T>
PArray<T>::PArray (int    len,
                   PArrayPolicy _managed)
    : vp(len),
      managed(_managed)
{
    for (int i = 0; i < length(); i++)
        vp[i] = 0;
}

template <class T>
void
PArray<T>::clear ()
{
    if (managed)
    {
        for (int i = 0; i < length(); ++i)
            delete ((T*)(vp[i]));
    }
    for (int i = 0; i < length(); ++i)
        vp[i] = 0;
}


template <class T>
PArray<T>::~PArray ()
{
    clear();
}

template <class T>
void
PArray<T>::resize (int newlen)
{
    void **ovp = vp.ready() ? &vp[0] : 0;
    int onelem = vp.length();
    vp.resize(newlen);
    for (int i = onelem; i < newlen; ++i)
        vp[i] = 0;
    if (managed)
    {
        for (int i = newlen; i < onelem; i++)
        {
            delete ((T*)(ovp[i]));
            ovp[i]  = 0;
        }
    }
}

template <class T>
void
PArray<T>::clear (int n)
{
    if (managed)
        delete ((T*)(vp[n]));
    vp[n] = 0;
}
