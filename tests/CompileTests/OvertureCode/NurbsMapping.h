#ifndef NURBS_MAPPING_H
#define NURBS_MAPPING_H 

#include "Mapping.h"
  
class IgesReader;  // forward declaration
class GenericGraphicsInterface;
class GraphicsParameters;

//-------------------------------------------------------------
//  Define a Nurbs Curve or Surface
//-------------------------------------------------------------
class NurbsMapping : public Mapping
{

  friend class PlotStuff;
  
public:

  enum NurbsConstantsEnum
  {
    maximumOrder=15 // maximum order for nurbs (p1+1) <= maximumOrder
  };
  
  enum FileFormat
  {
    xxww,  // ascii file format is x1,x2,... y1,y2,... z1,z2,..., w1,w2,...
    xwxw,  // ascii file format is x1,y1,z1,w1, x2,y2,z2,w2,...
    cheryl // cheryl's format
  };

  NurbsMapping();
  NurbsMapping(const int & domainDimension , const int & rangeDimension_);  // make a default NURBS

  // Copy constructor is deep by default
  NurbsMapping( const NurbsMapping &, const CopyType copyType=DEEP );

  ~NurbsMapping();

  NurbsMapping & operator =( const NurbsMapping & X0 );

  int binomial(const int m, const int n);  // compute m! /( (m-n)! n! )

  // split a curve at corners into sub-curves.
  int buildSubCurves( real angle = 60. );
  
  // build a new Nurbs curve  that matches a coordinate line on the surface
  int buildCurveOnSurface( NurbsMapping & curve, real r0, real r1=-1. );

  int circle(realArray & o,
             realArray & x, 
	     realArray & y, 
	     real r,
	     real startAngle=0.,
	     real endAngle=1. );

  int line( RealArray &p1, RealArray &p2 );

  int elevateDegree(const int increment);

  int getOrder( int axis=0 ) const;  // get order p
  int getNumberOfKnots( int axis=0 ) const;  // m+1
  int getNumberOfControlPoints( int axis=0 ) const;  // n+1

  // get uKnot or vKnot, the knots in the first or second direction.
  const realArray & getKnots( int direction=0 ) const;

  const realArray & getControlPoints() const;

  // insert a knot
  int insertKnot(const real & uBar, const int & numberOfTimesToInsert=1 );

  // split the nurb into two, return the pieces but do not alter the original nurb
  int split(real uSplit, NurbsMapping &c1,  NurbsMapping&c2);

  // make a nurb that passes through given points, optionally pass parameterization, optionally get parameterization
  void interpolate(const realArray & x, 
                   const int & option     = 0 ,
		   realArray & parameterization  =Overture::nullRealDistributedArray(),
                   int degree = 3 );
  

  // merge (join) two nurbs's
  int merge(NurbsMapping & nurbs  );
  // merge two nurbs, add a straight line segment if they do not match up closely
  int forcedMerge(NurbsMapping & nurbs  );
  // force a nurbs mapping to be periodic, moving the last control point if neccessary...
  int forcePeriodic();

  int moveEndpoint( int end, const RealArray &endPoint );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
		    MappingParameters & params =Overture::nullMappingParameters() );

  virtual void display( const aString & label=blankString) const;

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  int put( FILE *file, const FileFormat & fileFormat=xxww );  // save basic NURBS data to a readable ascii file.
  int put(const aString & fileName, const FileFormat & fileFormat=xxww);

  int get( FILE *file, const FileFormat & fileFormat=xxww );  // read from an ascii file.
  int get(const aString & fileName, const FileFormat & fileFormat=xxww);


  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return NurbsMapping::className; }


  int parametricSplineSurface(int mu, int mv, realArray & u, realArray & v, realArray & poly );

  int removeKnot(const int & index, 
                 const int & numberOfTimesToRemove, 
                 int & numberRemoved,
		 const real &tol = 100.*FLT_EPSILON);

  // rescale u and v
  int reparameterize(const real & uMin, 
                     const real & uMax,
                     const real & vMin=0.,
                     const real & vMax=1.);
  
  int setDomainInterval(const real & r1Start  =0., 
			const real & r1End    =1.,
			const real & r2Start  =0., 
			const real & r2End    =1.,
			const real & r3Start  =0., 
			const real & r3End    =1. );

  int readFromIgesFile( IgesReader & iges, const int & item );

  // if the Nurb is formed by merging a sequence of Nurbs then function will return that number.
  int numberOfSubCurves() const;
  // Here is the sub curve.
  NurbsMapping& subCurve(const int & subCurveNumber);

  // Indicate that this nurbs is a parametric curve on another nurbs.
  int parametricCurve(const NurbsMapping & nurbs,
                      const bool & scaleParameterSpace=TRUE );

  // rotate about a given axis
  int rotate( const int & axis, const real & theta );

  // transform using a 3x3 matrix
  int matrixTransform( const RealArray & r );
  
  // scale the NURBS
  int scale(const real & scalex=1., 
	    const real & scaley=1., 
	    const real & scalez=1. );

  // shift in space
  int shift(const real & shiftx=0., 
	    const real & shifty=0., 
	    const real & shiftz=0. );

  // specify a curve in 2D or 3D
  int specify(const int &  m,
	      const int & n,
	      const int & p,
	      const realArray & uKnot,
	      const realArray & cPoint,
              const int & rangeDimension=3,
              bool normalizeTheKnots=true );
  // specify a NURBS with domain dimension = 2
  int specify(const int & n1, 
	      const int & n2,
	      const int & p1, 
	      const int & p2, 
	      const realArray & uKnot, 
	      const realArray & vKnot,
	      const realArray & controlPoint,
	      const int & rangeDimension =3,
              bool normalizeTheKnots=true );

  virtual const realArray& getGrid(MappingParameters & params=Overture::nullMappingParameters());
  virtual int update( MappingInformation & mapInfo );

  int plot(GenericGraphicsInterface & gi, GraphicsParameters & parameters, bool plotControlPoints = FALSE );

 protected:
  void initialize();

  int normalizeKnots();

  int intersect3DLines( realArray & p0, realArray &  t0, 
			realArray & p1, realArray & t1,
			real & alpha0, real & alpha1,
			realArray & p2) ;
  
  real distance4D( const realArray & x, const realArray & y );
  // vectorized version of map
  virtual void mapVector(const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
                         MappingParameters & params =Overture::nullMappingParameters() );

  void setBounds(); // set bounds for plotting etc.
  
  void privateInterpolate( const realArray & x, const realArray *uBar );

 private:

  aString className;
  int n1,m1,p1;   
  int n2,m2,p2;
  realArray uKnot,vKnot,cPoint;
  bool initialized;
  bool nonUniformWeights; // true if the weights are not all constant
  real uMin,uMax,vMin,vMax;  // original knot extent (they are scaled to [0,1])
  int nurbsIsPeriodic[3];  // for reparameterization, remember original periodicity.
  real rStart[3], rEnd[3];  // for reparameterization

  bool mappingNeedsToBeReinitialized;

  // if the Nurb is formed by merging a sequence of Nurbs then we may keep the
  // Nurbs that were merged for later use.
  int numberOfCurves;
  NurbsMapping **subCurves;

 public:
  static bool useScalarEvaluation;

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((NurbsMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((NurbsMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new NurbsMapping(*this, ct); }




  int findSpan(const int & n ,
               const int & p,
	       const real & u,
	       const realArray & uKnot );
  void basisFuns(const int & i,
                 const real & u,
		 const int & p,
		 const realArray & uKnot,
		 realArray & basis );
  void dersBasisFuns(const int & i,
                     const real & u,
		     const int & p,
		     const int & order,
		     const realArray & uKnot,
		     real *ders );
  
  // vectorized versions
  void findSpan(const int & n ,
		const int & p,
		const Index & I,
		const realArray & u,
		const realArray & knot,
		intArray & span );

  void dersBasisFuns(const Index & I,
                     const intArray & ia,
		     const realArray & u,
		     const int & p,
		     const int & order,
		     const realArray & knot,
		     realArray & ders );
};


#endif  
