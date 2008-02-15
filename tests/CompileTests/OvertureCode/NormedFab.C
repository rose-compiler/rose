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
// $Id: NormedFab.C,v 1.1 2004/07/07 10:27:11 dquinlan Exp $
//

#include <BoxLib.H>
#include <Looping.H>

template <class T>
NormedFab<T>::NormedFab ()
    : BaseFab<T>()
{}

template <class T>
NormedFab<T>::NormedFab (const Box& b,
                         int        n)
    : BaseFab<T>(b,n)
{}

template <class T>
NormedFab<T>::NormedFab (BaseFab<T>& bf,
                         Box         subb,
                         int         ns,
                         int         nc)
    : BaseFab<T>(bf,subb,ns,nc)
{}

//
// This isn't inlined as it's virtual.
//

template <class T>
NormedFab<T>::~NormedFab()
{}

template <class T>
void
NormedFab<T>::abs ()
{
    ForAllThis(Real)
    {
        thisR = Abs(thisR);
    } EndFor
}

template <class T>
void
NormedFab<T>::abs (int comp,
                   int numcomp)
{
    ForAllThisNN(Real,comp,numcomp)
    {
        thisR = Abs(thisR);
    } EndFor
}

template <class T>
void
NormedFab<T>::abs (const Box& subbox,
                   int        comp,
                   int        numcomp)
{
    ForAllThisBNN(Real,subbox,comp,numcomp)
    {
        thisR = Abs(thisR);
    } EndFor
}

// ----------------------  NORM MEMBERS ------------------------

//
// This isn't inlined as it's virtual.
//

template <class T>
Real
NormedFab<T>::norm (int p,
                    int comp,
                    int numcomp) const
{
    return norm(domain,p,comp,numcomp);
}

template <class T>
Real
NormedFab<T>::norm (const Box& subbox,
                    int        p,
                    int        comp,
                    int        numcomp) const
{
    boxAssert(comp >= 0 && comp+numcomp <= nComp());
    boxAssert(p >= 0);

    Real* tmp  = 0;
    int tmplen = 0;
    Real nrm   = 0;
    if (p == 0)
    {
        ForAllThisCPencil(T,subbox,comp,numcomp)
        {
            const T* row = &thisR;
            if (tmp == 0)
            {
                tmp = new Real[thisLen];
                if (tmp == 0)
                    BoxLib::OutOfMemory(__FILE__, __LINE__);
                tmplen = thisLen;
                for (int i = 0; i < thisLen; i++)
                    tmp[i] = Abs(Real(row[i]));
            }
            else
            {
                for (int i = 0; i < thisLen; i++)
                    tmp[i] = Max(tmp[i],Real(Abs(row[i])));
            }
        } EndForPencil
        nrm = tmp[0];
        for (int i = 1; i < tmplen; i++)
            nrm = Max(nrm, tmp[i]);
    }
    else if (p == 1)
    {
        ForAllThisCPencil(T,subbox,comp,numcomp)
        {
            const T* row = &thisR;
            if (tmp == 0)
            {
                tmp = new Real[thisLen];
                if (tmp == 0)
                    BoxLib::OutOfMemory(__FILE__, __LINE__);
                tmplen = thisLen;
                for (int i = 0; i < thisLen; i++)
                    tmp[i] = Abs(Real(row[i]));
            }
            else
            {
                for (int i = 0; i < thisLen; i++)
                    tmp[i] += Abs(Real(row[i]));
            }
        } EndForPencil
        nrm = tmp[0];
        for (int i = 1; i < tmplen; i++)
            nrm += tmp[i];
    }
    else
      BoxLib::Error("NormedFab::norm(): only p == 0 or p == 1 are supported");

    delete [] tmp;

    return nrm;
}
