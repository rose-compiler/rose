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
// $Id: OrderedFab.C,v 1.1 2004/07/07 10:27:11 dquinlan Exp $
//

#include <iostream.h>
#include <iomanip.h>

#include <Misc.H>
#include <OrderedFab.H>
#include <Looping.H>

#ifndef ORDEREDFAB_C
#define ORDEREDFAB_C

template <class T>
OrderedFab<T>::OrderedFab ()
    : BaseFab<T>()
{}

template <class T>
OrderedFab<T>::OrderedFab (const Box& b,
                           int        n)
    : BaseFab<T>(b,n)
{}

template <class T>
OrderedFab<T>::OrderedFab (BaseFab<T>& bf,
                           Box         subb,
                           int         ns,
                           int         nc)
    : BaseFab<T>(bf,subb,ns,nc)
{}

//
// This isn't inlined as it's virtual.
//

template <class T>
OrderedFab<T>::~OrderedFab ()
{}

//
// ---------------------  Min
//

template <class T>
T
OrderedFab<T>::min (int comp) const
{
    T *_min_row = 0;
    int _X_len = 0;
    ForAllThisCPencil(T,domain,comp,1)
    {
        const T* _row = &thisR;
        if (_min_row == 0)
        {
            _min_row = new T[thisLen];
            if (_min_row == 0)
                BoxLib::OutOfMemory(__FILE__, __LINE__);
            _X_len = thisLen;
            for (int i = 0; i < thisLen; i++)
                _min_row[i] = _row[i];
        }
        else
        {
            for (int i = 0; i < thisLen; i++)
                _min_row[i] = Min(_row[i],_min_row[i]);
        }
    } EndForPencil;

    T _min = _min_row[0];
    for (int i = 1; i < _X_len; i++)
        _min = Min(_min,_min_row[i]);

    delete [] _min_row;

    return _min;
}

template <class T>
T
OrderedFab<T>::min (const Box& subbox,
                    int        comp) const
{
    T *_min_row = 0;
    int _X_len = 0;
    ForAllThisCPencil(T,subbox,comp,1)
    {
        const T* _row = &thisR;
        if (_min_row == 0)
        {
            _min_row = new T[thisLen];
            if (_min_row == 0)
                BoxLib::OutOfMemory(__FILE__, __LINE__);
            _X_len = thisLen;
            for (int i = 0; i < thisLen; i++)
                _min_row[i] = _row[i];
        }
        else
        {
            for (int i = 0; i < thisLen; i++)
                _min_row[i] = Min(_row[i],_min_row[i]);
        }
    } EndForPencil;

    T _min = _min_row[0];
    for (int i = 1; i < _X_len; i++)
        _min = Min(_min,_min_row[i]);

    delete [] _min_row;

    return _min;
}

//
// ---------------------  Max
//

template <class T>
T
OrderedFab<T>::max (int comp) const
{
    T *_max_row = 0;
    int _X_len = 0;
    ForAllThisCPencil(T,domain,comp,1)
    {
        const T* _row = &thisR;
        if (_max_row== 0)
        {
            _max_row = new T[thisLen];
            if (_max_row == 0)
                BoxLib::OutOfMemory(__FILE__, __LINE__);
            _X_len = thisLen;
            for (int i = 0; i < thisLen; i++)
                _max_row[i] = _row[i];
        }
        else
        {
            for (int i = 0; i < thisLen; i++)
                _max_row[i] = Max(_row[i],_max_row[i]);
        }
    } EndForPencil;

    T _max = _max_row[0];
    for (int i = 1; i < _X_len; i++)
        _max = Max(_max,_max_row[i]);

    delete [] _max_row;

    return _max;
}

template <class T>
T
OrderedFab<T>::max (const Box& subbox,
                    int        comp) const
{
    T *_max_row = 0;
    int _X_len = 0;
    ForAllThisCPencil(T,subbox,comp,1)
    {
        const T* _row = &thisR;
        if (_max_row == 0)
        {
            _max_row = new T[thisLen];
            if (_max_row == 0)
                BoxLib::OutOfMemory(__FILE__, __LINE__);
            _X_len = thisLen;
            for (int i = 0; i < thisLen; i++)
                _max_row[i] = _row[i];
        }
        else
        {
            for (int i = 0; i < thisLen; i++)
                _max_row[i] = Max(_row[i],_max_row[i]);
        }
    } EndForPencil;

    T _max = _max_row[0];
    for (int i = 1; i < _X_len; i++)
        _max = Max(_max,_max_row[i]);

    delete [] _max_row;

    return _max;
}

//
// ---------------------  MinIndex
//

template <class T>
IntVect
OrderedFab<T>::minIndex (int comp) const
{
    IntVect _min_loc(domain.smallEnd());
    T _min_val = (*this).operator()(_min_loc,comp);
    ForAllThisCBNN(T,domain,comp,1)
    {
        if (thisR < _min_val)
        {
            _min_val = thisR;
            D_EXPR(_min_loc[0] = iR,
                   _min_loc[1] = jR,
                   _min_loc[2] = kR);
        }
    } EndFor;

    return _min_loc;
}

template <class T>
IntVect
OrderedFab<T>::minIndex (const Box& subbox,
                         int        comp) const
{
    IntVect _min_loc(subbox.smallEnd());
    T _min_val = (*this).operator()(_min_loc,comp);
    ForAllThisCBNN(T,subbox,comp,1)
    {
        if (thisR < _min_val)
        {
            _min_val = thisR;
            D_EXPR(_min_loc[0] = iR,
                   _min_loc[1] = jR,
                   _min_loc[2] = kR);
        }
    } EndFor;

    return _min_loc;
}

//
// ---------------------  MaxIndex
//

template <class T>
IntVect
OrderedFab<T>::maxIndex (int comp) const
{
    IntVect _max_loc(domain.smallEnd());
    T _max_val = (*this).operator()(_max_loc,comp);
    ForAllThisCBNN(T,domain,comp,1)
    {
        if (thisR > _max_val)
        {
            _max_val = thisR;
            D_EXPR(_max_loc[0] = iR,
                   _max_loc[1] = jR,
                   _max_loc[2] = kR);
        }
    } EndFor;

    return _max_loc;
}

template <class T>
IntVect
OrderedFab<T>::maxIndex (const Box& subbox,
                         int        comp) const
{
    IntVect _max_loc(subbox.smallEnd());
    T _max_val = (*this).operator()(_max_loc,comp);
    ForAllThisCBNN(T,subbox,comp,1)
    {
        if (thisR > _max_val)
        {
            _max_val = thisR;
            D_EXPR(_max_loc[0] = iR,
                   _max_loc[1] = jR,
                   _max_loc[2] = kR);
        }
    } EndFor;

    return _max_loc;
}

#ifdef __GNUC__
typedef BaseFab<int> bfi;
#endif

template <class T>
int
OrderedFab<T>::maskLT (BaseFab<int>& mask,
                       T             val,
                       int           comp) const
{
    mask.resize(domain,1);
    mask.setVal(0);

    int *mptr = mask.dataPtr();
    int cnt = 0;

    ForAllThisCBNN(T,domain,comp,1)
    {
        int ix = D_TERM(_i, +_j*_b_len[0], +_k*_b_len[0]*_b_len[1]);
        if (thisR < val)
        {
            mptr[ix] = 1;
            cnt++;
        }
    } EndFor;

    return cnt;
}

template <class T>
int
OrderedFab<T>::maskLE (BaseFab<int>& mask,
                       T             val,
                       int           comp) const
{
    mask.resize(domain,1);
    mask.setVal(0);

    int *mptr = mask.dataPtr();
    int cnt = 0;

    ForAllThisCBNN(T,domain,comp,1)
    {
        int ix = D_TERM(_i, +_j*_b_len[0], +_k*_b_len[0]*_b_len[1]);
        if (thisR <= val)
        {
            mptr[ix] = 1;
            cnt++;
        }
    } EndFor;

    return cnt;
}

template <class T>
int
OrderedFab<T>::maskEQ (BaseFab<int>& mask,
                       T             val,
                       int           comp) const
{
    mask.resize(domain,1);
    mask.setVal(0);

    int *mptr = mask.dataPtr();
    int cnt = 0;

    ForAllThisCBNN(T,domain,comp,1)
    {
        int ix = D_TERM(_i, +_j*_b_len[0], +_k*_b_len[0]*_b_len[1]);
        if (thisR == val)
        {
            mptr[ix] = 1;
            cnt++;
        }
    } EndFor;

    return cnt;
}

template <class T>
int
OrderedFab<T>::maskGT (BaseFab<int>& mask,
                       T             val,
                       int           comp) const
{
    mask.resize(domain,1);
    mask.setVal(0);

    int *mptr = mask.dataPtr();
    int cnt = 0;

    ForAllThisCBNN(T,domain,comp,1)
    {
        int ix = D_TERM(_i, +_j*_b_len[0], +_k*_b_len[0]*_b_len[1]);
        if (thisR > val)
        {
            mptr[ix] = 1;
            cnt++;
        }
    } EndFor;

    return cnt;
}

template <class T>
int
OrderedFab<T>::maskGE(BaseFab<int>& mask,
                      T             val,
                      int           comp) const
{
    mask.resize(domain,1);
    mask.setVal(0);

    int *mptr = mask.dataPtr();
    int cnt = 0;

    ForAllThisCBNN(T,domain,comp,1)
    {
        int ix = D_TERM(_i, +_j*_b_len[0], +_k*_b_len[0]*_b_len[1]);
        if (thisR >= val)
        {
            mptr[ix] = 1;
            cnt++;
        }
    } EndFor;

    return cnt;
}


#endif // ORDEREDFAB_C
