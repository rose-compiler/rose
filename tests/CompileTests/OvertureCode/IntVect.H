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

#ifndef _INTVECT_H_
#define _INTVECT_H_

//
// $Id: IntVect.H,v 1.1 2004/07/07 10:27:06 dquinlan Exp $
//

#include <SPACE.H>

#include <iostream.h>
#include <stdlib.h>

#include <Boolean.H>
#include <Misc.H>
#include <Array.H>
#include <Assert.H>

//
//@Man:
//@Memo: An Integer Vector in SPACEDIM-Dimensional Space
/*@Doc:

  The class IntVect is an implementation of an integer vector in a
  SPACEDIM-dimensional space.  It represents a point in a discrete space.
  IntVect values are accessed using the operator[] function, as for a normal
  C++ array.  In addition, the basic arithmetic operators have been overloaded
  to implement scaling and translation operations.
*/

class IntVect
{

public:
    //
    //@ManDoc: Construct an IntVect whose components are all zero.
    //
    IntVect ();

    /*@ManDoc: Construct an IntVect given the specific values for its
               coordinates.  D\_DECL is a macro that sets the constructor
               to take BL\_SPACEDIM arguments.
    */
    IntVect (D_DECL(int i, int j, int k));

    /*@ManDoc: Construct an IntVect setting the coordinates to the
               corresponding values in the integer array a.
    */
    IntVect (const int* a);

    /*@ManDoc: Construct an IntVect from an Array<int>.  It is an error if
               the Array<int> doesn' t have the same dimension as this
               IntVect.
    */
    IntVect (const Array<int>& a);
    //
    //@ManDoc: The copy constructor.
    //
    IntVect (const IntVect& rhs);
    //
    //@ManDoc: The assignment operator.
    //
    IntVect& operator= (const IntVect& rhs);
    //
    //@ManDoc: Returns a reference to the i'th coordinate of the IntVect.
    //
    int& operator[] (int i);
    //
    //@ManDoc: Returns the i'th coordinate of the IntVect.
    //
    int operator[] (int i) const;
    //
    //@ManDoc: Set i'th coordinate of IntVect to val.
    //
    IntVect& setVal (int i,
                     int val);

    /*@ManDoc: Returns a const pointer to an array of coordinates of the
               IntVect.  Useful for arguments to FORTRAN calls.
    */
    const int* getVect () const;

    /*@ManDoc: Returns a const pointer to an array of coordinates of the
               IntVect.  Useful for arguments to FORTRAN calls.
    */
    operator const int* () const;

    /*@ManDoc: Returns true of IntVect is equivalent to argument IntVect.
               All comparisons between analogous components must be satisfied.
    */
    bool operator== (const IntVect& p) const;

    /*@ManDoc: Returns true of IntVect is different from argument IntVect.
               All comparisons between analogous components must be satisfied.
    */
    bool operator!= (const IntVect& p) const;

    /*@ManDoc: Returns true if IntVect is less than argument IntVect.
               All comparisons between analogous components must be satisfied.
    */
    bool operator< (const IntVect& p) const;

    /*@ManDoc: Returns true if IntVect is less than or equal to argument
               IntVect.  All comparisons between analogous components must
               be satisfied.
    */
    bool operator<= (const IntVect& p) const;

    /*@ManDoc: Returns true if IntVect is greater than argument IntVect.
               All comparisons between analogous components must be satisfied.
    */
    bool operator> (const IntVect& p) const;

    /*@ManDoc: Returns true if IntVect is greater than or equal to argument
               IntVect.  All comparisons between analogous components must
               be satisfied.
    */
    bool operator>= (const IntVect& p) const;

    /*@ManDoc: Returns true if this IntVect is lexically less than the
               argument.
    */
    bool lexLT (const IntVect& s) const;

    /*@ManDoc: Returns true if this IntVect is lexically greater than the
               argument.
    */
    bool lexGT (const IntVect& s) const;
    //
    //@ManDoc: Unary plus -- for completeness.
    //
    IntVect operator+ ();
    //
    //@ManDoc: Unary Minus -- negates all components.
    //
    IntVect operator- ();
    //
    //@ManDoc: Modifies IntVect by addition of a scaler to each component.
    //
    IntVect& operator+= (int s);
    //
    //@ManDoc: Modifies IntVect by component-wise addition with argument.
    //
    IntVect& operator+= (const IntVect& p);

    /*@ManDoc: Modifies IntVect by multiplication of a scaler to each
               component.
    */
    IntVect& operator*= (int s);
    //
    //@ManDoc: Modifies IntVect by component-wise multiplication with argument.
    //
    IntVect& operator*= (const IntVect& p);
    //
    //@ManDoc: Modifies IntVect by division by a scaler to each component.
    //
    IntVect& operator/= (int s);
    //
    //@ManDoc: Modifies IntVect by component-wise division with argument.
    //
    IntVect& operator/= (const IntVect& p);
    //
    //@ManDoc: Modifies IntVect by subtraction of a scaler to each component.
    //
    IntVect& operator-= (int s);
    //
    //@ManDoc: Modifies IntVect by component-wise subtraction with argument.
    //
    IntVect& operator-= (const IntVect& p);
    //
    //@ManDoc: Returns component-wise sum of IntVect and argument.
    //
    IntVect operator+ (const IntVect& p) const;
    //
    //@ManDoc: Return an IntVect that is this IntVect + s.
    //
    IntVect operator+ (int s) const;
    //
    //@ManDoc: Returns p + s.
    //
    friend inline IntVect operator+ (int            s,
                                     const IntVect& p);
    //
    //@ManDoc: Returns component-wise difference of IntVect and argument.
    //
    IntVect operator- (const IntVect& p) const;
    //
    //@ManDoc: Return an IntVect that is this IntVect - s.
    //
    IntVect operator- (int s) const;
    //
    //@ManDoc: Returns -p + s.
    //
    friend inline IntVect operator- (int            s,
                                     const IntVect& p);
    //
    //@ManDoc: Returns component-wise product of IntVect and argument.
    //
    IntVect operator* (const IntVect& p) const;
    //
    //@ManDoc: Returns component-wise product of IntVect and s.
    //
    IntVect operator* (int s) const;
    //
    //@ManDoc: Returns p * s.
    //
    friend inline IntVect operator* (int            s,
                                     const IntVect& p);
    //
    //@ManDoc: Returns component-wise division of IntVect by argument.
    //
    IntVect operator/ (const IntVect& p) const;
    //
    //@ManDoc: Returns component-wise division of IntVect by s.
    //
    IntVect operator/ (int s) const;
    //
    //@ManDoc: Modifies IntVect by taking component-wise min with argument.
    //
    IntVect& min (const IntVect& p);
    //
    //@ManDoc: Modifies IntVect by taking component-wise max with argument.
    //
    IntVect& max (const IntVect& p);
    //
    //@ManDoc: Modify IntVect by multiplying each coordinate by s.
    //
    IntVect& scale (int s);

    /*@ManDoc: Modify IntVect by reflecting it in the plane defined by
               the index ref\_ix and with normal in the direction of idir.
    */
    IntVect& reflect (int ref_ix,
                      int idir);
    //
    //@ManDoc: Modify IntVect by adding s to given coordinate.
    //
    IntVect& shift (int coord,
                    int s);
    //
    //@ManDoc: Modify IntVect by adding s to each coordinate.
    //
    IntVect& diagShift (int s);
    //
    //@ManDoc: Modify IntVect by component-wise integer projection.
    //
    IntVect& coarsen (const IntVect& p);
    //
    //@ManDoc: Modify IntVect by component-wise integer projection.
    //
    IntVect& coarsen (int p);
    //
    //@ManDoc: Print an IntVect to the ostream.
    //
    void printOn (ostream& os = cout) const;
    //
    //@ManDoc: Print an IntVect to the ostream a bit more verbosely.
    //
    void dumpOn (ostream& os = cout) const;

    /*@ManDoc:
      This static member function returns a reference to a constant IntVect
      object, all of whose BL\_SPACEDIM arguments are set to zero (0).
      Figuratively, it is the zero vector in BL\_SPACEDIM-dimensional space.
      It is provided as a convenient way to specify the zero vector.
      */
    static const IntVect& TheZeroVector ();

    /*@ManDoc:
      This static member function returns a reference to a constant IntVect
      object, all of whose BL\_SPACEDIM arguments are set to one (1).
      Figuratively, it is the unit vector in BL\_SPACEDIM-dimensional space.
      It is provided as a convenient way to specify the unit vector.
      */
    static const IntVect& TheUnitVector ();

    /*@ManDoc:
      This static member function returns a reference to a constant IntVect
      object, all of whose BL\_SPACEDIM arguments are set to IndexType::NODE.
      It is provided as a convenience to our users when defining Boxes.
      */
    static const IntVect& TheNodeVector ();

    /*@ManDoc: 
      This static member function returns a reference to a constant IntVect
      object, all of whose BL\_SPACEDIM arguments are set to IndexType::CELL.
      It is provided as a convenience to our users when defining Boxes.
      */
    static const IntVect& TheCellVector ();
    /*@ManDoc: Returns a basis vector in the given coordinate direction;
               eg.  IntVect3d BASISV(1) == (0,1,0).  Note that the coordinate
               directions are zero based.
    */
    friend inline IntVect BASISV (int dir);

    /*@ManDoc: Returns the IntVect that is the component-wise minimum of two
               argument IntVects.
    */
    friend inline IntVect min (const IntVect& p1,
                               const IntVect& p2);

    /*@ManDoc: Returns the IntVect that is the component-wise minimum of two
               argument IntVects.
    */
    friend inline IntVect max (const IntVect& p1,
                               const IntVect& p2);

    /*@ManDoc: Returns a IntVect obtained by multiplying each of the
               components of this IntVect by s.
    */
    friend inline IntVect scale (const IntVect& p,
                                 int            s);

    /*@ManDoc: Returns an IntVect that is the reflection of input in the
               plane which passes through ref\_ix and normal to the
               coordinate direction idir.
    */
    friend inline IntVect reflect(const IntVect& a,
                                  int            ref_ix,
                                  int            idir);

    /*@ManDoc: Returns IntVect obtained by adding s to each of the
               components of this IntVect.
    */
    friend inline IntVect diagShift (const IntVect& p,
                                     int            s);

    /*@ManDoc: Returns an IntVect that is the component-wise integer
               projection of p by s.
    */
    friend inline IntVect coarsen (const IntVect& p,
                                   int            s);

    /*@ManDoc: Returns an IntVect which is the component-wise integer
               projection of IntVect p1 by IntVect p2.
    */
    friend inline IntVect coarsen (const IntVect& p1,
                                   const IntVect& p2);
    //
    //@ManDoc: Print the IntVect to given output stream in ASCII.
    //
    friend ostream& operator<< (ostream&       os,
                                const IntVect& iv);
    //
    //@ManDoc: Read next IntVect from given input stream.
    //
    friend istream& operator>> (istream& os,
                                IntVect& iv);

protected:
    //
    // Box is a friend of ours.
    //
    friend class Box;

    int vect[SpaceDim];
};

//
//@ManDoc: A typedef to IntVect provided for backward compatibility. 
//
typedef IntVect INTVECT;

//
// For backward compatibility.
//
#ifdef BL_BACKWARD_COMPAT
#define ZEROV IntVect::TheZeroVector()
#define UNITV IntVect::TheUnitVector()
#define NODEV IntVect::TheNodeVector()
#define CELLV IntVect::TheCellVector()
#endif

//
// Inlines.
//

inline
IntVect::IntVect ()
{
    D_EXPR(vect[0] = 0, vect[1] = 0, vect[2] = 0);
}

inline
IntVect::IntVect (D_DECL(int i, int j, int k))
{
    D_EXPR(vect[0] = i, vect[1] = j, vect[2] = k);
}

inline
IntVect::IntVect (const int *a)
{
    D_EXPR(vect[0] = a[0], vect[1] = a[1], vect[2] = a[2]);
}

inline
IntVect::IntVect (const Array<int> &a)
{
    boxAssert(a.length() == SpaceDim);
    D_EXPR(vect[0] = a[0], vect[1] = a[1], vect[2] = a[2]);
}

inline
IntVect::IntVect (const IntVect &iv)
{
    D_EXPR(vect[0]=iv.vect[0], vect[1]=iv.vect[1], vect[2]=iv.vect[2]);
}

inline
IntVect&
IntVect::operator= (const IntVect &iv)
{
    D_EXPR(vect[0]=iv.vect[0], vect[1]=iv.vect[1], vect[2]=iv.vect[2]);
    return *this;
}

inline
int&
IntVect::operator[] (int i)
{
    boxAssert(i>=0 && i < SpaceDim);
    return vect[i];
}

inline
int
IntVect::operator[] (int i) const
{
    boxAssert(i>=0 && i < SpaceDim);
    return vect[i];
}

inline
IntVect&
IntVect::setVal (int i,
                 int val)
{
    boxAssert(i >=0 && i < SpaceDim);
    vect[i] = val;
    return *this;
}

inline
const int*
IntVect::getVect () const
{
    return vect;
}

inline
IntVect::operator const int* () const
{
    return vect;
}

inline
bool
IntVect::operator== (const IntVect& p) const
{
    return D_TERM(vect[0] == p[0], && vect[1] == p[1], && vect[2] == p[2]);
}

inline
bool
IntVect::operator!= (const IntVect& p) const
{
    return D_TERM(vect[0] != p[0], || vect[1] != p[1], || vect[2] != p[2]);
}

inline
bool
IntVect::operator< (const IntVect& p) const
{
    return D_TERM(vect[0] < p[0], && vect[1] < p[1], && vect[2] < p[2]);
}

inline
bool
IntVect::operator<= (const IntVect& p) const
{
    return D_TERM(vect[0] <= p[0], && vect[1] <= p[1], && vect[2] <= p[2]);
}

inline
bool
IntVect::operator> (const IntVect& p) const
{
    return D_TERM(vect[0] > p[0], && vect[1] > p[1], && vect[2] > p[2]);
}

inline
bool
IntVect::operator>= (const IntVect& p) const
{
    return D_TERM(vect[0] >= p[0], && vect[1] >= p[1], && vect[2] >= p[2]);
}

inline
bool
IntVect::lexLT (const IntVect &s) const
{
#define LLT0 (vect[0] < s[0])
#define LLT1 ((vect[0] == s[0]) && (vect[1] < s[1]))
#define LLT2 ((vect[1] == s[1]) && (vect[2] < s[2]))
#if   BL_SPACEDIM == 1
    return LLT0;
#elif BL_SPACEDIM == 2
    return LLT0 || LLT1;
#elif BL_SPACEDIM == 3
    return LLT0 || (vect[0]==s[0] && ((vect[1] < s[1] || LLT2)));
#endif
#undef LLT0
#undef LLT1
#undef LLT2
}

inline
bool
IntVect::lexGT (const IntVect& s) const
{
#define LGT0 (vect[0] > s[0])
#define LGT1 ((vect[0] == s[0]) && (vect[1] > s[1]))
#define LGT2 ((vect[1] == s[1]) && (vect[2] > s[2]))
#if   BL_SPACEDIM == 1
   return LGT0;
#elif BL_SPACEDIM == 2
   return LGT0 || LGT1;
#elif BL_SPACEDIM == 3
   return LGT0 || (vect[0] == s[0] && ((vect[1] > s[1] || LGT2)));
#endif
#undef LGT0
#undef LGT1
#undef LGT2
}

inline
IntVect
IntVect::operator+ ()
{
    return IntVect(*this);
}

inline
IntVect
IntVect::operator- ()
{
    return IntVect(D_DECL(-vect[0], -vect[1], -vect[2] ));
}

inline
IntVect&
IntVect::operator+= (int s)
{
    D_EXPR(vect[0] += s, vect[1] += s, vect[2] += s);
    return *this;
}

inline
IntVect&
IntVect::operator+= (const IntVect& p)
{
    D_EXPR(vect[0] += p[0], vect[1] += p[1], vect[2] += p[2]);
    return *this;
}

inline
IntVect&
IntVect::operator*= (int s)
{
    D_EXPR(vect[0] *= s, vect[1] *= s, vect[2] *= s);
    return *this;
}

inline
IntVect&
IntVect::operator*= (const IntVect &p)
{
    D_EXPR(vect[0] *= p[0], vect[1] *= p[1], vect[2] *= p[2]);
    return *this;
}

inline
IntVect&
IntVect::operator/= (int s)
{
    D_EXPR(vect[0] /= s, vect[1] /= s, vect[2] /= s);
    return *this;
}

inline
IntVect&
IntVect::operator/= (const IntVect& p)
{
    D_EXPR(vect[0] /= p[0], vect[1] /= p[1], vect[2] /= p[2]);
    return *this;
}

inline
IntVect&
IntVect::operator-= (int s)
{
    D_EXPR(vect[0] -= s, vect[1] -= s, vect[2] -= s);
    return *this;
}

inline
IntVect&
IntVect::operator-= (const IntVect& p)
{
    D_EXPR(vect[0] -= p[0], vect[1] -= p[1], vect[2] -= p[2]);
    return *this;
}

inline
IntVect
IntVect::operator+ (const IntVect& p) const
{
    return IntVect(D_DECL(vect[0] + p[0], vect[1] + p[1], vect[2] + p[2]));
}

inline
IntVect
IntVect::operator+ (int s) const
{
    return IntVect(D_DECL(vect[0] + s, vect[1] + s, vect[2] + s));
}

inline
IntVect
IntVect::operator- (const IntVect& p) const
{
    return IntVect(D_DECL(vect[0] - p[0], vect[1] - p[1], vect[2] - p[2]));
}

inline
IntVect
IntVect::operator- (int s) const
{
    return IntVect(D_DECL(vect[0] - s, vect[1] - s, vect[2] - s));
}

inline
IntVect
IntVect::operator* (const IntVect& p) const
{
    return IntVect(D_DECL(vect[0] * p[0], vect[1] * p[1], vect[2] * p[2]));
}

inline
IntVect
IntVect::operator* (int s) const
{
    return IntVect(D_DECL(vect[0] * s, vect[1] * s, vect[2] * s));
}

inline
IntVect
IntVect::operator/ (const IntVect& p) const
{
    return IntVect(D_DECL(vect[0] / p[0], vect[1] / p[1], vect[2] / p[2]));
}

inline
IntVect
IntVect::operator/ (int s) const
{
    return IntVect(D_DECL(vect[0] / s, vect[1] / s, vect[2] / s));
}

inline
IntVect&
IntVect::min (const IntVect& p)
{
    D_EXPR(vect[0] = Min(vect[0], p.vect[0]),
           vect[1] = Min(vect[1], p.vect[1]),
           vect[2] = Min(vect[2], p.vect[2]));
    return *this;
}

inline
IntVect&
IntVect::max (const IntVect& p)
{
    D_EXPR(vect[0] = Max(vect[0], p.vect[0]),
           vect[1] = Max(vect[1], p.vect[1]),
           vect[2] = Max(vect[2], p.vect[2]));
    return *this;
}

inline
IntVect&
IntVect::scale (int s)
{
    D_EXPR(vect[0] *= s, vect[1] *= s, vect[2] *= s);
    return *this;
}

inline
IntVect&
IntVect::reflect (int ref_ix,
                  int idir)
{
    boxAssert(idir >= 0 && idir < SpaceDim);
    vect[idir] = -vect[idir] + 2*ref_ix;
    return *this;
}

inline
IntVect&
IntVect::shift (int coord,
                int s)
{
    boxAssert(coord >= 0 && coord < SpaceDim);
    vect[coord] += s;
    return *this;
}

inline
IntVect&
IntVect::diagShift (int s)
{
    D_EXPR(vect[0] += s, vect[1] += s, vect[2] += s);
    return *this;
}

inline
IntVect
operator+ (int            s,
           const IntVect& p)
{
    return IntVect(D_DECL(p[0] + s, p[1] + s, p[2] + s));
}

inline
IntVect
operator- (int            s,
           const IntVect& p)
{
    return IntVect(D_DECL(s - p[0], s - p[1], s - p[2]));
}

inline
IntVect
operator* (int            s,
           const IntVect& p)
{
    return IntVect(D_DECL(s * p[0], s * p[1], s * p[2]));
}

inline
IntVect
scale (const IntVect& p,
       int            s)
{
    return IntVect(D_DECL(s * p[0], s * p[1], s * p[2]));
}

inline
IntVect
diagShift (const IntVect &p, int s)
{
    return IntVect(D_DECL(p[0] + s, p[1] + s, p[2] + s));
}

inline
IntVect
min (const IntVect& p1,
     const IntVect& p2)
{
    IntVect p(p1);
    return p.min(p2);
}

inline
IntVect
max (const IntVect& p1,
     const IntVect& p2)
{
    IntVect p(p1);
    return p.max(p2);
}

inline
IntVect
BASISV (int dir)
{
    boxAssert(dir >= 0 && dir < SpaceDim);
    IntVect tmp;
    tmp.vect[dir] = 1;
    return tmp;
}

inline
IntVect
reflect (const IntVect& a,
         int            ref_ix,
         int            idir)
{
    boxAssert(idir >= 0 && idir < SpaceDim);
    IntVect b(a);
    b.vect[idir] = -b.vect[idir] + 2*ref_ix;
    return b;
}

inline
IntVect
coarsen (const IntVect& p,
         int            s)
{
    boxAssert(s > 0);
    return IntVect(
        D_DECL((p.vect[0]<0) ? -abs(p.vect[0]+1)/s-1 : p.vect[0]/s ,
               (p.vect[1]<0) ? -abs(p.vect[1]+1)/s-1 : p.vect[1]/s ,
               (p.vect[2]<0) ? -abs(p.vect[2]+1)/s-1 : p.vect[2]/s ));
}

inline
IntVect
coarsen (const IntVect& p1,
         const IntVect& p2)
{
    boxAssert(p2 > IntVect::TheZeroVector());
    return IntVect(
        D_DECL(
        (p1.vect[0]<0)?-abs(p1.vect[0]+1)/p2.vect[0]-1:p1.vect[0]/p2.vect[0],
        (p1.vect[1]<0)?-abs(p1.vect[1]+1)/p2.vect[1]-1:p1.vect[1]/p2.vect[1],
        (p1.vect[2]<0)?-abs(p1.vect[2]+1)/p2.vect[2]-1:p1.vect[2]/p2.vect[2])
    );
}

inline
IntVect&
IntVect::coarsen (int s)
{
    boxAssert(s > 0);
    for (int i = 0; i < SpaceDim; ++i)
        vect[i] = ((vect[i]<0) ? -abs(vect[i]+1)/s-1 : vect[i]/s);
    return *this;
}

inline
IntVect&
IntVect::coarsen (const IntVect& p)
{
    boxAssert(p > IntVect::TheZeroVector());
    for (int i = 0; i <SpaceDim; ++i)
    {
        const int s = p.vect[i];
        vect[i] = ((vect[i]<0) ? -abs(vect[i]+1)/s-1 : vect[i]/s);
    }
    return *this;
}

#endif /*_INTVECT_H_*/

