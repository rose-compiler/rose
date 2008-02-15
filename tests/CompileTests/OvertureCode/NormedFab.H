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

#ifndef NORMEDFAB_H
#define NORMEDFAB_H

// *wdh*
#include "OvertureDefine.h"

//
// $Id: NormedFab.H,v 1.1 2004/07/07 10:27:11 dquinlan Exp $
//
#include <iostream.h>

#include <BaseFab.H>
#include <REAL.H>

//
//@Man:
//@Memo: A BaseFab with Norms
/*@Doc:

  This class provides various L-p norms to a BaseFab.

  This class does NOT provide a copy constructor or assignment operator.
 */

template <class T>
class NormedFab
    : public virtual BaseFab<T>
{
public:
    //
    //@ManDoc: The default constructor.
    //
    NormedFab ();
    //
    //@ManDoc: Construct a NormedFab from a Box.
    //
    NormedFab (const Box& b,
               int        n = 1);
    //
    //@ManDoc: Construct a NormedFab from a BaseFab.
    //
    NormedFab (BaseFab<T>& bf,
               Box         subb,
               int         ns,
               int         nc);
    //
    //@ManDoc: The (virtual) destructor.
    //
    virtual ~NormedFab();

    /*@ManDoc: Compute the Lp-norm of this FAB using components
               (comp : comp+numcomp-1).
               p < 0  -> ERROR
               p = 0  -> infinity norm (max norm)
               p = 1  -> sum of ABS(FAB)
    */
    virtual Real norm (int p,
                       int comp=0,
                       int numcomp=1) const;
    //
    //@ManDoc: Same as above except only on given subbox.
    //
    virtual Real norm (const Box& subbox,
                       int        p,
                       int        comp=0,
                       int        numcomp=1) const;
    //
    //@ManDoc:  Compute absolute value for all components of this FAB.
    //
    void abs ();
    //
    //@ManDoc: Same as above except only for components (comp: comp+numcomp-1)
    //
    void abs (int comp,
              int numcomp=1);
    //
    //@ManDoc: Calculate abs() on subbox for given component range.
    //
    void abs (const Box& subbox,
              int        comp = 0,
              int        numcomp=1);
private:
    //
    // These are not allowed.
    //
    NormedFab (const NormedFab<T>&);
    NormedFab<T>& operator= (const NormedFab<T>&);
};

#ifdef BL_AUTO_INSTANTIATE
#include <NormedFab.C>
#endif

#endif /*NORMEDFAB_H*/
