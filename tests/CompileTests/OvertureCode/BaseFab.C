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
// $Id: BaseFab.C,v 1.1 2004/07/07 10:27:00 dquinlan Exp $
//

#include <stdlib.h>
#include <Assert.H>
#include <Box.H>
#include <BoxList.H>
#include <Looping.H>
#include <AliasedDPtr.H>

#ifdef BL_USE_POINTLIB
#include <PointLooping.H>
#endif

template <class T>
BaseFab<T>::BaseFab ()
    : domain(Box()),
      pdomain(Box()),
      nvar(0),
      noffset(0),
      pnvar(0),
      numpts(0)
{ }

template <class T>
BaseFab<T>::BaseFab (const Box& bx,
                     int        n)
    : domain(bx),
      pdomain(bx)
{
    pnvar   = nvar = n;
    pnumpts = numpts = bx.numPts();
    noffset = 0;
    define();
}

#ifdef BL_USE_POINTLIB
#ifndef BL_CRAY_BUG_DEFARG
template <class T>
BaseFab<T>::BaseFab (const PointBaseFab<PointDomain,T>& pbf,
                     const T                            val)
{
    pdomain = domain = pbf.minimalBox();
    pnvar   = nvar = pbf.nComp();
    noffset = 0;
    pnumpts = numpts = domain.numPts();
    resize(domain, nvar);
    setVal(val);
    PointForAllCX(PointDomain,T,pbf)
    {
        this->operator()(ivR,nR) = pbfR;
    } EndPointForAll
}
#endif /*BL_CRAY_BUG_DEFARG*/
#endif /*BL_USE_POINTLIB*/

template <class T>
void
BaseFab<T>::define ()
{
    boxAssert(nvar > 0);
    boxAssert(numpts > 0);
    if (!dptr.ok())
    {
        AliasedDPtr<T>* ptr = new AliasedDPtr<T>(nvar*numpts);
        if (ptr == 0)
            BoxLib::OutOfMemory(__FILE__, __LINE__);
        dptr.setDPtr(ptr);
    }
}

//
// Alias.
//
template <class T>
BaseFab<T>::BaseFab (BaseFab<T>& fr,
                     const Box&  subb,
                     int         ns,
                     int         nc)
    : domain(subb),
      pdomain(fr.box())
{
    pnvar   = fr.nComp();
    pnumpts = fr.pnumpts;
    nvar    = nc;
    noffset = ns;
    numpts  = subb.numPts();

    AliasedDPtr<T>* p = new AliasedDPtr<T>((AliasedDPtr<T>*)fr.dptr.getDPtr(),
                                           nvar*numpts);
    if (p == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);

    dptr.setDPtr(p);
}

//
// This isn't inlined as it's virtual.
//

template <class T>
BaseFab<T>::~BaseFab ()
{}

template <class T>
void
BaseFab<T>::resize (const Box& b,
                    int        n)
{
    pnvar    = nvar   = n;
    pdomain  = domain = b;
    pnumpts  = numpts = domain.numPts();
    int size = nvar*numpts;
    if (!dptr.ok())
        define();
    else
        dptr->resize(size);
}

template <class T>
void
BaseFab<T>::clear ()
{
    if (dptr.ok())
        dptr->clear();
    domain = Box();
    nvar   = 0;
    numpts = 0;
}

template <class T>
void
BaseFab<T>::performCopy (const BaseFab<T>& src,
                         const Box&        srcbox,
                         int               srccomp,
                         const Box&        destbox,
                         int               destcomp,
                         int               numcomp)
{
    boxAssert(src.box().contains(srcbox));
    boxAssert(box().contains(destbox));
    boxAssert(destbox.sameSize(srcbox));
    boxAssert(srccomp >= 0 && srccomp+numcomp <= src.nComp());
    boxAssert(destcomp >= 0 && destcomp+numcomp <= nComp());

    ForAllThisBNNXCBN(T,destbox,destcomp,numcomp,src,srcbox,srccomp)
    {
        thisR = srcR;
    } EndForTX
}

#ifdef BL_USE_POINTLIB
#ifndef BL_CRAY_BUG_DEFARG
template <class T>
void
BaseFab<T>::performCopy (const PointBaseFab<PointDomain,T>& src,
                         const Box&                         srcbox,
                         int                                srccomp,
                         const Box&                         destbox,
                         int                                destcomp,
                         int                                numcomp)
{
    boxAssert(box().contains(destbox));
    boxAssert(destbox.sameSize(srcbox));
    boxAssert(srccomp >= 0 && srccomp+numcomp <= src.nComp());
    boxAssert(destcomp >= 0 && destcomp+numcomp <= nComp());

    destcomp -= srccomp; // Adjust
    Box bx(domain);
    bx &= srcbox;
    bx &= destbox;
    if (bx.ok())
    {
        PointForAllCXBNN(PointDomain,T,src,bx,srccomp,numcomp)
        {
            this->operator()(ivR, destcomp + nR) = srcR;
        } EndPointForAll
    }
}
#endif /*!BL_CRAY_BUG_DEFARG*/
#endif /*BL_USE_POINTLIB*/

template <class T>
BaseFab<T>&
BaseFab<T>::copy (const BaseFab<T>& src,
                  const Box&        srcbox,
                  int               srccomp,
                  const Box&        destbox,
                  int               destcomp,
                  int               numcomp)
{
    boxAssert(srcbox.sameSize(destbox));
    boxAssert(src.box().contains(srcbox));
    boxAssert(domain.contains(destbox));
    boxAssert(srccomp >= 0 && srccomp+numcomp <= src.nComp());
    boxAssert(destcomp >= 0 && destcomp+numcomp <= nvar);
    performCopy(src,srcbox,srccomp,destbox,destcomp,numcomp);
    return *this;
}

template <class T>
BaseFab<T>&
BaseFab<T>::copy (const BaseFab<T>& src)
{
    boxAssert(nvar == src.nvar);
    boxAssert(domain.sameType(src.domain));
    Box overlap(domain);
    overlap &= src.domain;
    if (overlap.ok())
        performCopy(src,overlap,0,overlap,0,nvar);
    return *this;
}

template <class T>
BaseFab<T>&
BaseFab<T>::copy (const BaseFab<T>& src,
                  const Box&        destbox)
{
    boxAssert(nvar == src.nvar);
    boxAssert(domain.contains(destbox));
    Box overlap(destbox);
    overlap &= src.domain;
    if (overlap.ok())
        performCopy(src,overlap,0,overlap,0,nvar);
    return *this;
}

template <class T>
BaseFab<T>&
BaseFab<T>::copy  (const BaseFab<T>& src,
                   int               srccomp,
                   int               destcomp,
                   int               numcomp)
{
    boxAssert(srccomp >= 0 && srccomp + numcomp <= src.nvar);
    boxAssert(destcomp >= 0 && destcomp + numcomp <= nvar);
    Box overlap(domain);
    overlap &= src.domain;
    if (overlap.ok())
        performCopy(src,overlap,srccomp,overlap,destcomp,numcomp);
    return *this;
}

template <class T>
void
BaseFab<T>::getVal  (T*             data,
                     const IntVect& pos,
                     int            n,
                     int            numcomp) const
{
    int loc      = domain.index(pos);
    int size     = domain.numPts();
    const T* ptr = dptr;
    boxAssert(n >= 0 && n + numcomp <= nvar);
    for (int k = 0; k < numcomp; k++)
        data[k] = ptr[loc+(n+k)*size];
}

template <class T>
void
BaseFab<T>::performSetVal (const T    val,
                           const Box& bx,
                           int        ns,
                           int        num)
{
    boxAssert(domain.contains(bx));
    boxAssert(ns >= 0 && ns + num <= nvar);
    ForAllThisBNN(T,bx,ns,num)
    {
        thisR = val;
    } EndFor
}

template <class T>
void
BaseFab<T>::setComplement (const T    x,
                           const Box& b,
                           int        ns,
                           int        num)
{
    BoxList b_lst = boxDiff(domain,b);
    for (BoxListIterator bli(b_lst); bli; ++bli)
        performSetVal(x,bli(),ns,num);
}

