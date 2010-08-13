#ifndef QUADTREE_H
#define QUADTREE_H 

#include "TrimmedMapping.h"

class TMcurves
// curve and segment index data for quadtree calculations for a TrimmedMapping.
// The indexing is be the same as for the Trimmed Mapping object used; i.e.
// the outer curve is 1 and the inner curves are 2,3...numberOfInnerCurves+1
// Their segments are numbered beginning at 0.
{
public:
  const real& curveDist() const { return cdist; };
  real& curveDist() { return cdist; };
  const int& curvestart() const { return cstart; };
  const int& curvestop() const { return cstop; };
  int& curvestart() { return cstart; };
  int& curvestop() { return cstop; };
  const int& nodestart( const int c ) const { return nstart[c]; };
  const int& nodestop( const int c ) const { return nstop[c]; };
  int& nodestart( const int c ) { return nstart[c]; };
  int& nodestop( const int c ) { return nstop[c]; };

#ifdef OLDSTUFF
  TMcurves( TrimmedMapping& tm, const int curvemin=1,
#else
  TMcurves( TrimmedMapping& tm, const int curvemin=0,
#endif
	    const int curvemax=-1 ) :
    cstart(curvemin), cstop(curvemax)
    {
      int c;
      cdist = 0;
#ifdef OLDSTUFF
      if ( curvemax==-1 ) cstop = tm.getNumberOfInnerCurves()+2;
#else
      if ( curvemax==-1 ) cstop = tm.getNumberOfTrimCurves();
#endif
      nstart = new int[cstop];
      nstop = new int[cstop];
#ifdef OLDSTUFF
      assert(cstart>=1);
      assert(cstop<=tm.getNumberOfInnerCurves()+2);
#else
      assert(cstart>=0);
      assert(cstop<tm.getNumberOfTrimCurves()+1);
#endif
      for ( c=cstart; c<cstop; ++c ) {
	nstart[c] = 0;
	nstop[c]  = (tm.rCurve[c]).getLength(0)-1;
      }
    };
  TMcurves() : cdist(0), cstart(0), cstop(0), nstart(0), nstop(0) {};
  TMcurves( const TMcurves& tmc ) {    // deep copy
    int c;
    cdist = tmc.cdist;
    cstart = tmc.cstart;   cstop = tmc.cstop;
    delete[] nstart;    delete[] nstop;
    nstart = new int[cstop];
    nstop  = new int[cstop];
    for ( c=cstart; c<cstop; ++c ) {
      nstart[c] = tmc.nstart[c];
      nstop[c]  = tmc.nstop[c];
    }
  };
  TMcurves& operator=( const TMcurves& tmc ) {
    int c;
    cdist = tmc.cdist;
    cstart = tmc.cstart;   cstop = tmc.cstop;
    delete[] nstart;    delete[] nstop;
    nstart = new int[cstop];
    nstop  = new int[cstop];
    for ( c=cstart; c<cstop; ++c ) {
      nstart[c] = tmc.nstart[c];
      nstop[c]  = tmc.nstop[c];
    }
    return *this;
  };
  ~TMcurves() {
    delete[] nstart;
    delete[] nstop;
  };

protected:
  real cdist;    // cdist = lower bound for distance from square to curve cstart
  int cstart;
  int cstop;
  int* nstart;
  int* nstop;
};

class TMquad
// A quadtree for use in trimmed mapping computations.
// This has data for one node of the tree, a square, and contains
// its four child squares, if any.
// Squares and corners are numbered:   0  1
//                                     2  3
{
protected:
  static int maxsquares;  // total number of squares made, over all quadtree meshes
  static int nextID;      // next ID number to use, when accumulating data in an array
  static real smallestSquareWidth;
  // ... width of smallest square (2*dx), over all quadtree meshes; used only as a diagnostic

  real centerX;           // a better style would be to use a class Point
  real centerY;
  real dx;                // width of a child square
  TMquad * children;      // length 4, or NULL

  // The following data can principal be deleted for all but the leaf nodes
  // and one level above the leaf nodes:
  int inside;        // 1 if this square (including border) is inside the
  //                    trimmed region, -1 if outside, 0 if mixed or unknown
  TMcurves curves;   // lists all curves and their segments which may be relevent
  // to insideness calculation for this square and its descendants.
  // ... for leaf nodes there will only usually be no curves, or one line segment
  // of one polygonal curve, or one segment from each of two intersecting curves;
  // sometimes more curves if we don't resolve the quadtree far enough

  void remake( TrimmedMapping& tm, const real& centerX, const real& centerY,
               const real& dx_, TMcurves& curves_ );

public:
  // Variables to control stopping the subdivision process.
  // There is no point in using access functions because anything setting them
  // needs to understand the algorithm anyway...
  // Note: it would be a bit better to make dxMinNormal,dxMin2Curve serve as (user-
  // resettable) default values which can be overridden by optional arguments of divide
  static real dxMinNormal;
  // ... A square with smaller dx will not normally be subdivided
  static real dxMin2Curve;
  // ... A square with smaller dx will not be subdivided even if two
  // curves pass through it.  dxMin2Curve <= dxMinNormal

  int totalNumberSquaresMade() const { return maxsquares; };
  int maxSquares() const { return maxsquares; };
  real minSquareWidth() const { return smallestSquareWidth; };
  const real& the_dx() const { return dx; };
  const real& the_centerX() const { return centerX; };
  const real& the_centerY() const { return centerY; };
  int the_inside() const { return inside; };
  const TMcurves& the_curves() const { return curves; };

  bool insideSquare( const real x, const real y,
                     const real x0, const real x1, const real y0, const real y1 ) const {
    if ( x>=x0 && x<=x1 && y>=y0 && y<=y1 ) {
      return true; }
    else {
      return false; }
  };
  TMquad();
  TMquad( TrimmedMapping& tm, const real& centerX_, const real& centerY_,
	  const real dx_ );
  TMquad& operator=( const TMquad& tmq ) {
    // deep copy
    centerX = tmq.centerX;
    centerY = tmq.centerY;
    dx = tmq.dx;
    inside = tmq.inside;
    if ( children != NULL ) delete[] children;
    if ( tmq.children == NULL ) {
      children = NULL;
    }
    else {
      children = new TMquad[4];
      for ( int i=0; i<4; ++i ) children[i] = tmq.children[i];
    };
    curves = tmq.curves;
    return *this;
  };
  ~TMquad() {
    delete[] children;
  };

  real distancePointToSegment( real x, real y,
                               real x0, real y0, real u0, real v0 ) const;
  real distanceBetweenSegments( real x1, real y1, real u1, real v1,
				real x2, real y2, real u2, real v2 ) const;
  real distanceToCurve( int c, TrimmedMapping& tm ) const;

  void divide( TrimmedMapping& tm, int& sizeOfMesh, real& minWidth );
  void plot( PlotStuff & gi, PlotStuffParameters parameters ) const;
  void accumulateCenterPoints( realArray& points,
                               const int startID = nextID ) const;
  void accumulateCenterPoints( realArray& points, realArray& inout,
                               const int startID = nextID ) const;
  //  const TMquad& squareItsIn( Point pt ); ... the way I'd like to do it
  const TMquad* squareItsIn( real pointX, real pointY ) const;
  const TMquad* squareItsIn( real pointX, real pointY, TMquad*& parent ) const;
  bool inThisSquare( real pointX, real pointY ) const;

  // put and get functions: put a description of this quadtree into a database
  // file, and get it out.  There is not a standard virtual get function because
  // TMquad is not an independent object; it belongs to a TrimmedMapping.
  //   Note: More compact and faster would be to save just the minimal information:
  // that is the centerX,centerY,dx for only those squares on which divide was
  // called (at the top level); with the divide parameters for each such call;
  // and of course the static variables.  That would require programming complexities
  // such as another class; so for now we save the entire tree (i.e.,
  // centerX,centerY,dx,and children for each square) but recompute insideness
  // information rather than saving it.
  int TMget( const GenericDataBase & dir, const aString & name,
         TrimmedMapping& tm, TMcurves * curves_ = NULL );
  virtual int put( GenericDataBase & dir, const aString & name) const;
  void getStatics( GenericDataBase & dir ) const;
  void putStatics( GenericDataBase & dir ) const;

private:
  TMquad( const TMquad& tmq ) { cerr << "TMquad copy constructor call unexpected" << endl; };

};

class TMquadRoot : public TMquad
// A special version of TMquad for the root square of a quadtree mesh.
// The only difference is that a TMquadRoot knows some control
// parameters (not yet) and diagnostic indicators.
{
public:
  int sizeOfQuadTreeMesh;  // for diagnostics
  real minQuadTreeMeshDx;  // for diagnostics

  TMquadRoot() :
    TMquad(),  sizeOfQuadTreeMesh(0), minQuadTreeMeshDx(1.)
  {};
  TMquadRoot( TrimmedMapping& tm, const real& centerX_, const real& centerY_,
	      const real dx_ ) :
    TMquad( tm, centerX_, centerY_, dx_ ),
    sizeOfQuadTreeMesh(0), minQuadTreeMeshDx(1.)
  {};
  TMquadRoot& operator=( const TMquadRoot& tmq ) {
    sizeOfQuadTreeMesh = tmq.sizeOfQuadTreeMesh;
    minQuadTreeMeshDx = tmq.minQuadTreeMeshDx;
    this->TMquad::operator=(tmq);
    return *this;
  };
  int TMget( const GenericDataBase & dir, const aString & name,
             TrimmedMapping& tm, TMcurves * curves_ = NULL );
  virtual int put( GenericDataBase & dir, const aString & name) const;

private:
  TMquadRoot( const TMquad& tmq ) { cerr << "TMquad copy constructor call unexpected" << endl; };

};

#endif  
