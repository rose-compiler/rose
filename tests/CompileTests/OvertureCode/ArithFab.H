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

#ifndef ARITHFAB_H
#define ARITHFAB_H

// *wdh*
#include "OvertureDefine.h"

//
// $Id: ArithFab.H,v 1.1 2004/07/07 10:27:00 dquinlan Exp $
//
#include <iostream.h>

#include <BaseFab.H>
#include <REAL.H>

//
//@Man:
//@Memo: A BaseFab<T> Specialized for Numerical Operations
/*@Doc: 

  An ArithFab<T> elaborates the BaseFab concept by adding numerical 
  operations that make sense for arithmetic types T, e.g. integers
  and reals.  For such types we define Fab-wide sums, inverses,
  negations, additions, subtractions, multiplications, divisions,
  linear interpolations, and linear combinations.

  The numerical operations are generally overloaded, allowing a choice
  of a very general operation to a easy-to-use very specific operation.
  Note that many operations offer the same choice of overloaded
  signatures.

  Note that some functions are denoted as scalar operations while other
  functions are denoted as Fab operations.  The difference is that the
  scalar operations take a scalar as one argument, while FAB operations
  utilize two FABS.

  This class does NOT provide a copy constructor or assignment operator.
*/

template < class T >
class ArithFab
    : public virtual BaseFab<T>
{
public:

    /*@ManDoc: Default constructor uses BaseFab default constructor.
               It doesn't allocate any memory.
    */
    ArithFab ();
    //
    //@ManDoc: Allocate ArithFab for given box and number of components.
    //
    ArithFab (const Box& b,
              int        n = 1);

    /*@ManDoc: Creates aliasing ArithFab that uses memory in BaseFab bf.
               See aliasing constructor for BaseFab.
    */
    ArithFab (BaseFab<T>& bf,
              Box         subb,
              int         ns,
              int         nc);
    //
    //@ManDoc: The (virtual) destructor.
    //
    virtual ~ArithFab ();
    //
    //@ManDoc: Fill with a pattern of numbers.
    //
    void patternFill (int mark = 0);

    /*@ManDoc: Copies with index reversal from srcbox region of src
               into destbox region of this FAB.  All components are
               copied and multiplied by corresponding multiplier.
    */
    void copyRev (const Box&         destbox,
                  const ArithFab<T>& src,
                  const Box&         srcbox,
                  int                reversal_index,
                  T*                 multiplier);

#if !defined(BL_CRAY_BUG_DEFARG)

    /*@ManDoc: Copies with a reversal.  The index that is
               reversed is reversal\_index.  Comp is the component copied.
               This is not defined if the macro BL\_CRAY\_BUG\_DEFARG is
               defined.
    */
    friend void copyFABRev (const ArithFab<T>& src,
                            const Box&         srcbox,
                            ArithFab<T>&       dest,
                            const Box&         destbox,
                            int                reversal_index,
                            int                comp,
                            T                  multiplier);
#endif

    //
    //@ManDoc: Compute sum of given component of FAB state vector.
    //
    T sum (int comp,
           int numcomp = 1) const;

    /*@ManDoc: Compute sum of given component of FAB state vector in
               given subbox.
    */
    T sum (const Box& subbox,
           int        comp,
           int        numcomp = 1) const;
    //
    //@ManDoc: Most general version, specify subbox and which components.
    //
    ArithFab<T>& invert (T          v,
                         const Box& subbox,
                         int        comp=0,
                         int        numcomp=1);
    //
    //@ManDoc: As above except on entire domain.
    //
    ArithFab<T>& invert (T   v,
                         int comp,
                         int numcomp=1);
    //
    //@ManDoc: As above except on entire domain, all components.
    //
    ArithFab<T>& invert (T v);
    //
    //@ManDoc: Negate ArithFab, most general.
    //
    ArithFab<T>& negate (const Box& subbox,
                         int        comp=0,
                         int        numcomp=1);
    //
    //@ManDoc: As above, except on entire domain.
    //
    ArithFab<T>& negate (int comp,
                         int numcomp=1);
    //
    //@ManDoc: As above, except on entire domain and all components.
    //
    ArithFab<T>& negate ();
    //
    //@ManDoc: Scalar addition (a[i] <- a[i] + r), most general.
    //
    ArithFab<T>& plus (T          r,
                       const Box& b,
                       int        comp=0,
                       int        numcomp=1);
    //
    //@ManDoc: As above, except on entire domain.
    //
    ArithFab<T>& plus (T   r,
                       int comp,
                       int numcomp=1);
    //
    //@ManDoc: As above, except on entire domain and all components.
    //
    inline ArithFab<T>& plus (T r);

    /*@ManDoc: Addition in place.  This will often be more efficient than
               making new ArithFab for result.
    */
    ArithFab<T>& operator+= (T r);
    //
    //@ManDoc: FAB addition (a[i] <- a[i] + b[i]) in place.
    //
    ArithFab<T>& operator+= (const ArithFab<T>& f);
    //
    //@ManDoc: FAB addition (a[i] <- a[i] + b[i]). The same as += operator.
    //
    inline ArithFab<T>& plus (const ArithFab<T>& src);

    /*@ManDoc: Add src components (srccomp:srccomp+numcomp-1) to
               this FAB's components (destcomp:destcomp+numcomp-1)
               where the two FABs intersect.
    */
    ArithFab<T>& plus (const ArithFab<T>& src,
                       int                srccomp,
                       int                destcomp,
                       int                numcomp=1);

    /*@ManDoc: Same as above except addition is restricted to intersection
               of subbox and src FAB. NOTE: subbox must be contained in this
               FAB.
    */
    ArithFab<T>& plus (const ArithFab<T>& src,
                       const Box&         subbox,
                       int                srccomp,
                       int                destcomp,
                       int                numcomp=1);

    /*@ManDoc: Add srcbox region of src FAB to destbox region of this FAB.
               The srcbox and destbox must be same size.
    */
    ArithFab<T>& plus (const ArithFab<T>& src,
                       const Box&         srcbox,
                       const Box&         destbox,
                       int                srccomp,
                       int                destcomp,
                       int                numcomp=1);

    /*@ManDoc: Scalar subtraction (a[i] <- a[i] - r).
               Note: use plus(-r) for more general operations.
    */
    inline ArithFab<T>& operator-= (T r);
    //
    //@ManDoc: FAB subtraction (a[i] <- a[i] - b[i]), in place.
    //
    ArithFab<T>& operator-= (const ArithFab<T>& f);
    //
    //@ManDoc: FAB subtraction (a[i] <- a[i] - b[i]). The same as -= operator.
    //
    inline ArithFab<T>& minus (const ArithFab<T>& src);

    /*@ManDoc: Subtract src components (srccomp:srccomp+numcomp-1) to
               this FAB's components (destcomp:destcomp+numcomp-1) where
               the two FABs intersect.
    */
    ArithFab<T>& minus (const ArithFab<T>& src,
                        int                srccomp,
                        int                destcomp,
                        int                numcomp=1);

    /*@ManDoc: Same as above except subtraction is restricted to intersection
               of subbox and src FAB.  NOTE: subbox must be contained in
               this FAB.
    */
    ArithFab<T>& minus (const ArithFab<T>& src,
                        const Box&         subbox,
                        int                srccomp,
                        int                destcomp,
                        int                numcomp=1);

    /*@ManDoc: Subtract srcbox region of src FAB from destbox region
               of this FAB. srcbox and destbox must be same size.
    */
    ArithFab<T>& minus (const ArithFab<T>& src,
                        const Box&         srcbox,
                        const Box&         destbox,
                        int                srccomp,
                        int                destcomp,
                        int                numcomp=1);
    //
    //@ManDoc: Scalar multiplication (a[i] <- a[i] * r), in place.
    //
    ArithFab<T>& operator*= (T r);
    //
    //@ManDoc: Scalar multiplication (a[i] <- a[i] * r).  The same as *=.
    //
    inline ArithFab<T>& mult (T r);

    /*@ManDoc: Scalar multiplication, except control which components
               are multiplied.
    */
    ArithFab<T>& mult (T   r,
                       int comp,
                       int numcomp=1);
    //
    //@ManDoc: As above, except specify sub-box.
    //
    ArithFab<T>& mult (T          r,
                       const Box& b,
                       int        comp=0,
                       int        numcomp=1);
    //
    //@ManDoc: FAB multiplication (a[i] <- a[i] * b[i]), in place.
    //
    ArithFab<T>& operator*= (const ArithFab<T>& f);
    //
    //@ManDoc: As above.
    //
    inline ArithFab<T>& mult (const ArithFab<T>& src);

    /*@ManDoc: Multiply src components (srccomp:srccomp+numcomp-1) with
               this FAB's components (destcomp:destcomp+numcomp-1) where
               the two FABs intersect.
    */
    ArithFab<T>& mult (const ArithFab<T>& src,
                       int                srccomp,
                       int                destcomp,
                       int                numcomp=1);

    /*@ManDoc: Same as above except multiplication is restricted to
               intersection of subbox and src FAB.  NOTE: subbox must be
               contained in this FAB.
    */
    ArithFab<T>& mult (const ArithFab<T>& src,
                       const Box&         subbox,
                       int                srccomp,
                       int                destcomp,
                       int                numcomp=1);

    /*@ManDoc: Multiply srcbox region of src FAB with destbox region
               of this FAB. The srcbox and destbox must be same size.
    */
    ArithFab<T>& mult (const ArithFab<T>& src,
                       const Box&         srcbox,
                       const Box&         destbox,
                       int                srccomp,
                       int                destcomp,
                       int                numcomp=1);
    //
    //@ManDoc: Scalar division (a[i] <- a[i] / r), in place.
    //
    ArithFab<T>& operator/= (T r);
    //
    //@ManDoc: Scalar division (a[i] <- a[i] / r), in place.
    //
    inline ArithFab<T>& divide (T r);
    //
    //@ManDoc: As above except specify which components.
    //
    ArithFab<T>& divide (T   r,
                         int comp,
                         int numcomp=1);
    //
    //@ManDoc: As above except specify sub-box.
    //
    ArithFab<T>& divide (T          r,
                         const Box& b,
                         int        comp=0,
                         int        numcomp=1);
    //
    //@ManDoc: FAB division, in place.
    //
    ArithFab<T>& operator/= (const ArithFab<T>& src);
    //
    //@ManDoc: Same as above.
    //
    inline ArithFab<T>& divide (const ArithFab<T>& src);

    /*@ManDoc: This FAB is numerator, src FAB is denominator
               divide src components (srccomp:srccomp+numcomp-1) into
               this FAB's components (destcomp:destcomp+numcomp-1)
               where the two FABs intersect.
    */
    ArithFab<T>& divide (const ArithFab<T>& src,
                         int                srccomp,
                         int                destcomp,
                         int                numcomp=1);

    /*@ManDoc: Same as above except division is restricted to
               intersection of subbox and src FAB.  NOTE: subbox must be
               contained in this FAB.
    */
    ArithFab<T>& divide (const ArithFab<T>& src,
                         const Box&         subbox,
                         int                srccomp,
                         int                destcomp,
                         int                numcomp=1);
 
    /*@ManDoc: destbox region of this FAB is numerator. srcbox regions of
               src FAB is denominator. srcbox and destbox must be same size.
    */
    ArithFab<T>& divide (const ArithFab<T>& src,
                         const Box&         srcbox,
                         const Box&         destbox,
                         int                srccomp,
                         int                destcomp,
                         int                numcomp=1);

    /*@ManDoc: Linear interpolation / extrapolation.
               Result is (t2-t)/(t2-t1)*f1 + (t-t1)/(t2-t1)*f2
               Data is taken from b1 region of f1, b2 region of f2
               and stored in b region of this FAB.
               Boxes b, b1 and b2 must be the same size.
               Data is taken from component comp1 of f1, comp2 of f2,
               and stored in component comp of this FAB.
               This FAB is returned as a reference for chaining.
    */
    ArithFab<T>& linInterp (const ArithFab<T>& f1,
                            const Box&         b1,
                            int                comp1,
                            const ArithFab<T>& f2,
                            const Box&         b2,
                            int                comp2,
                            Real               t1,
                            Real               t2,
                            Real               t,
                            const Box&         b,
                            int                comp,
                            int                numcomp = 1);

    /*@ManDoc: Linear combination.  Result is alpha*f1 + beta*f2.
               Data is taken from b1 region of f1, b2 region of f2
               and stored in b region of this FAB.
               Boxes b, b1 and b2 must be the same size.
               Data is taken from component comp1 of f1, comp2 of f2,
               and stored in component comp of this FAB.
               This FAB is returned as a reference for chaining.
    */
    ArithFab<T>& linComb (const ArithFab<T>& f1,
                          const Box&         b1,
                          int                comp1,
                          const ArithFab<T>& f2,
                          const Box&         b2,
                          int                comp2,
                          Real               alpha,
                          Real               beta,
                          const Box&         b,
                          int                comp,
                          int                numcomp = 1);

private:
    //
    // Disallowed.
    //
    ArithFab (const ArithFab<T>&);
    ArithFab<T>& operator= (const ArithFab<T>&);
};

//
// Inlines.
//

template <class T>
inline
ArithFab<T>&
ArithFab<T>::plus (T r)
{
    return operator+=(r);
}

template <class T>
inline
ArithFab<T>&
ArithFab<T>::plus (const ArithFab<T>& x)
{
    return operator+=(x);
}

template <class T>
inline
ArithFab<T>&
ArithFab<T>::operator-= (T r)
{
    return operator+=(-r);
}

template <class T>
inline
ArithFab<T>&
ArithFab<T>::minus (const ArithFab<T>& x)
{
    return operator-=(x);
}


template <class T>
inline
ArithFab<T>&
ArithFab<T>::mult (T r)
{
    return operator*=(r);
}

template <class T>
inline
ArithFab<T>&
ArithFab<T>::mult (const ArithFab<T>& x)
{
    return operator*=(x);
}

template <class T>
inline
ArithFab<T>&
ArithFab<T>::divide (T r)
{
    return operator/=(r);
}

template <class T>
inline
ArithFab<T>&
ArithFab<T>::divide (const ArithFab<T>& x)
{
    return operator/=(x);
}

#ifdef BL_AUTO_INSTANTIATE
#include <ArithFab.C>
#endif

#endif /*ARITHFAB_H*/
