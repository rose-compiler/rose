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
// $Id: Pointers.C,v 1.1 2004/07/07 10:27:11 dquinlan Exp $
//

#if 0
//
// This stuff is commented out for now.  If someone really uses it,
// we'll uncomment it as well as document it.
//

template<class T>
VcClassPtr<T>::VcClassPtr (const VcClassPtr<T>& _a)
    :  ptr(_a.isNull() ?  0 : _a.ptr->clone())
{}

template<class T>
VcClassPtr<T>&
VcClassPtr<T>::operator= (T* _ptr)
{
    delete ptr;
    ptr = _ptr;
    return *this;
}

template<class T>
VcClassPtr<T>&
VcClassPtr<T>::operator= (const VcClassPtr<T>& _r)
{
    if (ptr != _r.ptr)
    {
        delete ptr;
        ptr = _r.isNull() ? 0 : _r.ptr->clone();
    }
    return *this;
}

template<class T>
T*
VcClassPtr<T>::release()
{
    T* old = ptr;
    ptr = 0;
    return old;
}
#endif
