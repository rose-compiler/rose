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
// $Id: FabArray.C,v 1.1 2004/07/07 10:27:03 dquinlan Exp $
//

#include <Assert.H>

template <class T, class FAB>
FabArray<T, FAB>::FabArray ()
    : n_grow(0),
      n_comp(0),
      bxskel(0),
      fs(0,PArrayManage)
{}

template <class T, class FAB>
FabArray<T, FAB>::FabArray (const BoxArray& bxs,
                            int             ngrow,
                            FabAlloc        alloc)
    : bxskel(0),
      fs(0,PArrayManage)
{
    define(bxs,ngrow,alloc);
}

template <class T, class FAB>
FabArray<T, FAB>::FabArray (const BoxAssoc& ba,
                            int             ngrow,
                            FabAlloc        alloc)
    : bxskel(0),
      fs(0,PArrayManage)
{
    define(ba,ngrow,alloc);
}

template <class T, class FAB>
FabArray<T, FAB>::FabArray (const BoxArray& bxs,
                            int             nvar,
                            int             ngrow,
               FabAlloc alloc)
    : bxskel(0),
      fs(0, PArrayManage)
{
    define(bxs,nvar,ngrow,alloc);
}

template <class T, class FAB>
FabArray<T, FAB>::FabArray (const BoxAssoc& ba,
                            int             nvar,
                            int             ngrow,
               FabAlloc alloc)
    : bxskel(0),
      fs(0, PArrayManage)
{
    define(ba,nvar,ngrow,alloc);
}


template <class T, class FAB>
FabArray<T, FAB>::~FabArray ()
{
    delete bxskel;
}

template <class T, class FAB>
bool
FabArray<T, FAB>::ok () const
{
    boxAssert(bs.ready());
    bool isok = true;
    if (!fs.defined(0))
        isok = false;
    else
    {
        for (int i = 0, nvar = fs[0].nVar(); i < length() && isok; ++i)
        {
            if (fs.defined(i))
            {
                const FAB &f = fs[i];
                if (f.box() != ::grow(box(i), n_grow))
                    isok = false;
                if (nvar != f.nVar())
                    isok = false;
            }
            else
                isok = false;
        }
    }
    return isok;
}

template <class T, class FAB>
void
FabArray<T, FAB>::define (const BoxArray& bxs,
                          int             ngrow,
                          FabAlloc        alloc)
{
    boxAssert(!bs.ready());
    n_grow = ngrow;
    n_comp = 0;
    bs.define(bxs);
    bxskel = new BoxAssoc(bs, n_grow);
    if (bxskel == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
    int nbox = bs.length();
    fs.resize(nbox);
    if (alloc == Fab_allocate)
    {
        for (int i = 0; i < length(); ++i)
        {
            Box tmp(::grow(bs[i],n_grow));
            FAB* f = new FAB(tmp);
            if (f == 0)
                BoxLib::OutOfMemory(__FILE__, __LINE__);
            fs.set(i,f);
        }
        n_comp = fs[0].nVar();
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::define (const BoxAssoc& ba,
                          int             ngrow,
                          FabAlloc        alloc)
{
    boxAssert(!bs.ready());
    boxAssert(bxskel == 0);
    n_grow = ngrow;
    n_comp = 0;
    bs.define(ba.boxArray());
    bxskel = new BoxAssoc(ba);
    if (bxskel == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
    int nbox = ba.length();
    fs.resize(nbox);
    if (alloc == Fab_allocate)
    {
        for (int i = 0; i < length(); ++i)
        {
            Box tmp(::grow(bs[i],n_grow));
            FAB* f = new FAB(tmp);
            if (f == 0)
                BoxLib::OutOfMemory(__FILE__, __LINE__);
            fs.set(i,f);
        }
        n_comp = fs[0].nVar();
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::define (const BoxArray& bxs,
                          int             nvar,
                          int             ngrow,
                          FabAlloc        alloc)
{
    boxAssert(!bs.ready());
    n_grow = ngrow;
    n_comp = nvar;
    bs.define(bxs);
    bxskel = new BoxAssoc(bs, n_grow);
    if (bxskel == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
    int nbox = bxs.length();
    fs.resize(nbox);
    if (alloc == Fab_allocate)
    {
        for (int i = 0; i < length(); ++i)
        {
            Box tmp(::grow(bs[i],n_grow));
            FAB* f = new FAB(tmp,nvar);
            if (f == 0)
                BoxLib::OutOfMemory(__FILE__, __LINE__);
            fs.set(i,f);
        }
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::define (const BoxAssoc& ba,
                          int             nvar,
                          int             ngrow,
                          FabAlloc        alloc)
{
    boxAssert(!bs.ready());
    boxAssert(bxskel == 0);
    n_grow = ngrow;
    n_comp = nvar;
    bs.define(ba.boxArray());
    bxskel = new BoxAssoc(ba);
    if (bxskel == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);
    int nbox = ba.length();
    fs.resize(nbox);
    if (alloc == Fab_allocate)
    {
        for (int i = 0; i < length(); ++i)
        {
            Box tmp(::grow(bs[i],n_grow));
            FAB* f = new FAB(tmp,nvar);
            if (f == 0)
                BoxLib::OutOfMemory(__FILE__, __LINE__);
            fs.set(i,f);
        }
    }
}


template <class T, class FAB>
void
FabArray<T, FAB>::setFab (int  boxno,
                          FAB* elem)
{
    //
    // Must check it is of the proper size.
    //
    if (n_comp == 0)
        n_comp = elem->nVar();
    boxAssert(n_comp == elem->nVar());
    boxAssert(bs.ready());
    boxAssert(elem->box() == ::grow(bs[boxno],n_grow));
    boxAssert(!fs.defined(boxno));
    fs.set(boxno,elem);
}

template <class T, class FAB>
void
FabArray<T, FAB>::setBndry (T val)
{
    if (n_grow > 0)
    {
        for (int i = 0; i < length(); ++i)
        {
            FAB& fab = fs[i];
            int nv = fab.nVar();
            fab.setComplement(val,bs[i],0,nv);
        }
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::setBndry(T val, int strt_comp, int ncomp)
{
    if (n_grow > 0)
    {
        for (int i = 0; i < length(); ++i)
            fs[i].setComplement(val,bs[i],strt_comp,ncomp);
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::filBndry (const BoxAssoc& ba)
{
    boxAssert( ba.boxArray() == bs );
    for (int i = 0; i < length(); ++i)
    {
        FAB& fab = fs[i];
        for (int j=0; j< ba.nborMax(i); j++)
        {
            int nbor = ba.nborIndex(i,j);
            //
            // For self copy, don't copy into self.
            //
            if (nbor == i)
                continue;
            //
            // Restrict copy to domain of validity of nbor.
            //
            Box destbox(ba[nbor]);
            destbox &= fab.box();
            fab.copy(fs[nbor], destbox);
        }
    }
}


//
// Self copy for given component only.
//

template <class T, class FAB>
void
FabArray<T, FAB>::filBndry (const BoxAssoc& ba,
                            int             scomp,
                            int             num_comp)
{
    boxAssert(ba.boxArray() == bs);
    for (int i = 0; i < length(); ++i)
    {
        FAB& fab = fs[i];
        for (int j=0; j < ba.nborMax(i); j++)
        {
            int nbor = ba.nborIndex(i,j);
            //
            // For self copy, don't copy into self.
            //
            if (nbor == i)
                continue;
            //
            // Restrict copy to domain of validity of nbor.
            //
            Box destbox(ba[nbor]);
            destbox &= fab.box();
            fab.copy(fs[nbor],destbox,scomp,destbox,scomp,num_comp);
        }
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::copy (const FabArray<T, FAB>& fa)
{
    for (int j = 0; j < length(); ++j)
      fa.copy(fs[j]);
}

template <class T, class FAB>
void
FabArray<T, FAB>::copy (const FabArray<T, FAB>& src,
                        int                     src_comp,
                        int                     dest_comp,
                        int                     num_comp,
                        int                     nghost)
{
    boxAssert(src.bs == bs);
    boxAssert(nghost <= n_grow);
    boxAssert(nghost <= src.n_grow);
    for (int j = 0; j < length(); ++j)
    {
        BOX b(grow(bs[j],nghost));
        fs[j].copy(src[j], b, src_comp, b, dest_comp, num_comp);
    }
}

//
// Copies to FABs, note that destination is first arg.
//

template <class T, class FAB>
void
FabArray<T, FAB>::copy (FAB& dest) const
{
    const Box& dbox = dest.box();
    for (int j = 0; j < length(); ++j)
    {
        //
        // Test all distributed objects.
        //
        if (bs[j].intersects(dbox))
        {
            //
            // Restrict copy to domain of validity of source.
            //
            Box destbox(bs[j]);
            destbox &= dbox;
            dest.copy(fs[j],destbox);
        }
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::copy (FAB&       dest,
                        const Box& subbox) const
{
    for (int j = 0; j < length(); ++j)
    {
        //
        // Test all distributed objects.
        //
        if (bs[j].intersects(subbox))
        {
            //
            // Restrict copy to domain of validity of source.
            //
            Box destbox(bs[j]);
            destbox &= subbox;
            dest.copy(fs[j],destbox);
        }
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::copy (FAB& dest,
                        int  src_comp,
                        int  dest_comp,
                        int  num_comp) const
{
    const Box& dbox = dest.box();
    for (int j = 0; j < length(); ++j)
    {
        if (bs[j].intersects(dbox))
        {

            Box destbox(bs[j]);
            destbox &= dbox;
            dest.copy(fs[j],destbox, src_comp, destbox, dest_comp,num_comp);
        }
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::copy (FAB&       dest,
                        const Box& subbox,
                        int        src_comp,
                        int        dest_comp,
                        int        num_comp) const
{
    boxAssert(dest_comp + num_comp <= dest.nVar());
    for (int j = 0; j < length(); ++j)
    {
        //
        // Test all distributed objects.
        //
        if (bs[j].intersects(subbox))
        {
            //
            // Restrict copy to domain of validity of source.
            //
            Box destbox(bs[j]);
            destbox &= subbox;
            dest.copy(fs[j],destbox,src_comp,destbox,dest_comp,num_comp);
        }
    }
}

//
// Fast copy into dest.
//

template <class T, class FAB>
void
FabArray<T, FAB>::copy (FAB&            dest,
                        const BoxAssoc& ba,
                        int             ba_index) const
{
    boxAssert(ba.boxArray() == bs);
    for (int j=0; j < ba.nborMax(ba_index); j++)
    {
        int nbor = ba.nborIndex(ba_index,j);
        //
        // Restrict copy to domain of validity of nbor.
        //
        Box destbox(ba[nbor]);
        destbox &= dest.box();
        dest.copy(fs[nbor], destbox);
    }
}

//
// Fast copy on subbox.
//

template <class T, class FAB>
void
FabArray<T, FAB>::copy (FAB&            dest,
                        const Box&      subbox,
                        const BoxAssoc& ba,
                        int             ba_index) const
{
    boxAssert(ba.boxArray() == bs);
    const Box& dbox = dest.box();
    for (int j=0; j < ba.nborMax(ba_index); j++)
    {
        int nbor = ba.nborIndex(ba_index,j);
        //
        // Restrict copy to domain of validity of nbor.
        //
        Box destbox(ba[nbor]);
        destbox &= dbox;
        destbox &= subbox;
        dest.copy(fs[nbor], destbox);
    }
}

//
// Fast copy but only selected components.
//

template <class T, class FAB>
void
FabArray<T, FAB>::copy (FAB&            dest,
                        const BoxAssoc& ba,
                        int             ba_index,
                        int             src_comp,
                        int             dest_comp,
                        int             num_comp) const
{
    boxAssert(ba.boxArray() == bs);
    const Box& dbox = dest.box();
    for (int j=0; j < ba.nborMax(ba_index); j++)
    {
        int nbor = ba.nborIndex(ba_index,j);
        //
        // Restrict copy to domain of validity of nbor.
        //
        Box destbox(ba[nbor]);
        destbox &= dbox;
        dest.copy(fs[nbor],destbox,src_comp,destbox,dest_comp,num_comp);
    }
}

//
// Fast copy but only selected components in dubbox.
//

template <class T, class FAB>
void
FabArray<T, FAB>::copy (FAB&            dest,
                        const Box&      subbox,
                        const BoxAssoc& ba,
                        int             ba_index,
                        int             src_comp,
                        int             dest_comp,
                        int             num_comp) const
{
    boxAssert(ba.boxArray() == bs);
    boxAssert(dest_comp + num_comp <= dest.nVar());
    const Box& dbox = dest.box();
    for (int j=0; j<ba.nborMax(ba_index); j++)
    {
        int nbor = ba.nborIndex(ba_index,j);
        //
        // Restrict copy to domain of validity of nbor.
        //
        Box destbox(ba[nbor]);
        destbox &= dbox;
        destbox &= subbox;
        dest.copy(fs[nbor],destbox,src_comp,destbox,dest_comp,num_comp);
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::setVal (T val)
{
    for (int i = 0; i < length(); ++i)
        fs[i].setVal(val);
}

template <class T, class FAB>
void
FabArray<T, FAB>::setVal (T   val,
                          int comp,
                          int num_comp,
                          int nghost)
{
    boxAssert(nghost >= 0 && nghost <= n_grow);
    boxAssert(comp+num_comp <= n_comp);
    for (int i = 0; i < length(); ++i)
    {
        Box b(::grow(bs[i],nghost));
        FAB& fab = fs[i];
        for (int k = 0; k < num_comp; k++)
            fab.setVal(val,b,comp+k);
    }
}

template <class T, class FAB>
void
FabArray<T, FAB>::setVal (T          val,
                          const Box& region,
                          int        comp,
                          int        num_comp,
                          int        nghost)
{
    boxAssert(nghost >= 0 && nghost <= n_grow);
    boxAssert(comp+num_comp <= n_comp);
    for (int i = 0; i < length(); ++i)
    {
        Box b(::grow(bs[i],nghost));
        b &= region;
        if (b.ok())
        {
            FAB& fab = fs[i];
            for (int k = 0; k < num_comp; k++)
                fab.setVal(val,b,comp+k);
        }
    }
}
