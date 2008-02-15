#ifndef OGPolyFunction_h
#define OGPolyFunction_h "OGPolyFunction.h"

#include "OGFunction.h"

//===========================================================================================
// Define a Function and it derivatives
//   This function can be used to define the "exact solution" for
//   an Overlapping Grid Appliciation (aka. TwilightZone Flow)
//
//      f(x,y,z,n,t)=[c(0,0,0,n)+x*(c(1,0,0,n)+x*(c(2,0,0,n)+x*(c(3,0,0,n)+c(4,0,0,n)*x)))
//                             +y*(c(0,1,0,n)+y*(c(0,2,0,n)+y*(c(0,3,0,n)+c(0,4,0,n)*y)))
//                             +z*(c(0,0,1,n)+z*(c(0,0,2,n)+z*(c(0,0,3,n)+c(0,0,4,n)*z)))]
//           *(a(0,n)+t*(a(1,n)+t*(a(2,n)+t*(a(3,n)+t*(a(4,n))))));
//
//    Here n is the component number and t is a parameter "time"
//    The matrices "c" and "a" defining the coefficients can be assigned
//    in the constructor or else default values can be used.
//
//===========================================================================================
class OGPolyFunction : public OGFunction
{

 private:
  int numberOfComponents;
  RealArray cc;
  RealArray a;
  void checkArguments(const Index & N);

 public:
  

 //  
 // Create a polynomial with the given degree, number Of Space Dimensions and for
 // a maximum number of components
 // 
 OGPolyFunction(const int & degreeSpaceOfPolynomial=2, 
                const int & numberOfDimensions=3,
	        const int & numberOfComponents=10,
                const int & degreeOfTimePolynomial=1 );
  
  OGPolyFunction(const OGPolyFunction & ogp );
  OGPolyFunction & operator=(const OGPolyFunction & ogp );
  ~OGPolyFunction(){}
  
  //
  //  Supply coefficients to use
  //   note that arrays should be dimensioned
  //        c(0:4,0:4,0:4,0:nc-1) and a(0:4,0:nc-1)   where nc=numberOfComponents
  // 
  void setCoefficients( const RealArray & c, const RealArray & a );

  // Here are the member functions that you must define
  //     u(x,y,z,n) : function value at x,y,z for component n
  //    ux(x,y,z,n) : partial derivative of u with respect to x
  //      ...etc...

#define TIME(n,t)  ( a(0,n)+t*(a(1,n)+t*(a(2,n)+t*(a(3,n)+t*(a(4,n))))) )


  inline real u(const real x, const real y, const real z, const int n, const real t=0.)
  {
    return (cc(0,0,0,n)
            +x*(cc(1,0,0,n)+y*cc(1,1,0,n)+x*(cc(2,0,0,n)+x*(cc(3,0,0,n)+x*cc(4,0,0,n))))
            +y*(cc(0,1,0,n)+z*cc(0,1,1,n)+y*(cc(0,2,0,n)+y*(cc(0,3,0,n)+y*cc(0,4,0,n))))
  	    +z*(cc(0,0,1,n)+x*cc(1,0,1,n)+z*(cc(0,0,2,n)+z*(cc(0,0,3,n)+z*cc(0,0,4,n))))
           )
           *TIME(n,t);
  }
  inline real ut(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return (cc(0,0,0,n)
            +x*(cc(1,0,0,n)+y*cc(1,1,0,n)+x*(cc(2,0,0,n)+x*(cc(3,0,0,n)+x*cc(4,0,0,n))))
            +y*(cc(0,1,0,n)+z*cc(0,1,1,n)+y*(cc(0,2,0,n)+y*(cc(0,3,0,n)+y*cc(0,4,0,n))))
  	    +z*(cc(0,0,1,n)+x*cc(1,0,1,n)+z*(cc(0,0,2,n)+z*(cc(0,0,3,n)+z*cc(0,0,4,n)))))
           *( a(1,n)+t*(2.*a(2,n)+t*(3.*a(3,n)+t*(4.*a(4,n)))) ); 
  }
  inline real ux(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    return (cc(1,0,0,n)+y*cc(1,1,0,n)+z*cc(1,0,1,n)+x*(2.*cc(2,0,0,n)+x*(3.*cc(3,0,0,n)+x*4.*cc(4,0,0,n)))
           )*TIME(n,t);
  }
  inline real uy(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    return (cc(0,1,0,n)+x*cc(1,1,0,n)+z*cc(0,1,1,n)+y*(2.*cc(0,2,0,n)+y*(3.*cc(0,3,0,n)+y*4.*cc(0,4,0,n)))
           )*TIME(n,t);
  }
  inline real uxx(const real x, const real , const real , const int n=0, const real t=0.)
  {
    return (2.*cc(2,0,0,n)+x*(6.*cc(3,0,0,n)+x*12.*cc(4,0,0,n)))*TIME(n,t);
  }
  inline real uxy(const real , const real , const real , const int n=0, const real t=0.)
  {
    return cc(1,1,0,n)*TIME(n,t);
  }
  inline real uyy(const real , const real y, const real , const int n=0, const real t=0.)
  {
    return (2.*cc(0,2,0,n)+y*(6.*cc(0,3,0,n)+y*12.*cc(0,4,0,n)))*TIME(n,t);
  }
  inline real uz(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    return (cc(0,0,1,n)+x*cc(1,0,1,n)+y*cc(0,1,1,n)+z*(2.*cc(0,0,2,n)+z*(3.*cc(0,0,3,n)+z*4.*cc(0,0,4,n)))
           )*TIME(n,t);
  }
  inline real uxz(const real , const real , const real , const int n=0, const real t=0.)
  {
    return cc(1,0,1,n)*TIME(n,t);
  }
  inline real uyz(const real , const real , const real , const int n=0, const real t=0.)
  {
    return cc(0,1,1,n)*TIME(n,t);
  }
  inline real uzz(const real , const real , const real z, const int n=0, const real t=0.)
  {
    return (2.*cc(0,0,2,n)+z*(6.*cc(0,0,3,n)+z*12.*cc(0,0,4,n)))*TIME(n,t);
  }

  // ========= Here are versions with a new naming convention ===========

  // (default arguments not allowed on operators)
  virtual real operator()(const real , const real , const real);
  virtual real operator()(const real , const real , const real , const int);
  virtual real operator()(const real , const real , const real , const int, const real);

  virtual real t(const real , const real , const real , const int=0, const real=0.);
  virtual real x(const real , const real , const real , const int=0, const real=0.);
  virtual real y(const real , const real , const real , const int=0, const real=0.);
  virtual real xx(const real , const real , const real , const int=0, const real=0.);
  virtual real xy(const real , const real , const real , const int=0, const real=0.);
  virtual real yy(const real , const real , const real , const int=0, const real=0.);
  virtual real z(const real , const real , const real , const int=0, const real=0.);
  virtual real xz(const real , const real , const real , const int=0, const real=0.);
  virtual real yz(const real , const real , const real , const int=0, const real=0.);
  virtual real zz(const real , const real , const real , const int=0, const real=0.);
  virtual real xxx(const real , const real , const real , const int=0, const real=0.);
  virtual real xxxx(const real , const real , const real , const int=0, const real=0.);
  // obtain a general derivative
  virtual real gd(const int & ntd, const int & nxd, const int & nyd, const int & nzd,
                  const real , const real , const real , const int=0, const real=0. );

  virtual RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N);
  virtual RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
/* ---
  RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
				  const Index & I3);
  RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
				  const Index & I3, const int n);
  RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
				  const Index & I3, const int n, const real t,
				  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
----- */

  virtual RealDistributedArray t(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray x(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray y(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray z(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray xx(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray yy(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray zz(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray xy(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray xz(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray yz(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray laplacian(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray xxx(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray xxxx(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) ;
  virtual RealDistributedArray gd(const int & ntd, const int & nxd, const int & nyd, const int & nzd,
				  const MappedGrid & c, const Index & I1, const Index & I2, 
				  const Index & I3, const Index & N, const real t=0.,
				  const GridFunctionParameters::GridFunctionType & centering
				  =GridFunctionParameters::defaultCentering);

  realCompositeGridFunction operator()(CompositeGrid & cg);
  realCompositeGridFunction operator()(CompositeGrid & cg, const Index & N=nullIndex);
  realCompositeGridFunction operator()(CompositeGrid & cg, const Index & N, const real t,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction t(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction x(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction y(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction z(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction xx(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction xy(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction xz(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction yy(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction yz(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction zz(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction laplacian(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction xxx(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction xxxx(CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
           const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);

  realCompositeGridFunction gd(const int & ntd, const int & nxd, const int & nyd, const int & nzd,
			       CompositeGrid & cg, const Index & N=nullIndex, const real t=0.,
			       const GridFunctionParameters::GridFunctionType & centering
                                 =GridFunctionParameters::defaultCentering);
};


#undef TIME

#endif 
