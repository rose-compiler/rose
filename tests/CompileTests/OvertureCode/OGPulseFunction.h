#ifndef OG_PULSE_FUNCTION_H
#define OG_PULSE_FUNCTION_H

#include "OGFunction.h"

//===========================================================================================
//
// Define "pulse" functions that are useful for testing AMR applications.
//
// Define a Function and it derivatives
//   This function can be used to define the "exact solution" for
//   an Overlapping Grid Appliciation (aka. TwilightZone Flow)
//
//  
//===========================================================================================
class OGPulseFunction : public OGFunction
{

 public:
  

 //  
 // Create a polynomial with the given degree, number Of Space Dimensions and for
 // a maximum number of components
 // 
  OGPulseFunction(int numberOfDimensions_ = 2,
		  int numberOfComponents_ =1,
		  real a0_ =1., 
		  real a1_ =5.,
		  real c0_ =0.,  
		  real c1_ =0.,
		  real c2_ =0.,
		  real v0_ =1.,
		  real v1_ =1.,
		  real v2_ =1.,
		  real p_  =2.);
  
  OGPulseFunction(const OGPulseFunction & ogp );
  OGPulseFunction & operator=(const OGPulseFunction & ogp );
  ~OGPulseFunction(){}
  
  int setRadius( real radius );
  int setShape( real p_ );
  int setCentre( real c0_ =0.,
                 real c1_ =0.,
		 real c2_ =0. );
  int setVelocity( real v0_ =1.,
                   real v1_ =1.,
		   real v2_ =1. );

  void setParameters(int numberOfDimensions_ = 2,
                     int numberOfComponents_ =1,
		     real a0_ =1., 
		     real a1_ =5.,
		     real c0_ =0.,
		     real c1_ =0.,
		     real c2_ =0.,
		     real v0_ =1.,
		     real v1_ =1.,
		     real v2_ =1.,
		     real p_  =2.);

  // Here are the member functions that you must define
  //     u(x,y,z,n) : function value at x,y,z for component n
  //    ux(x,y,z,n) : partial derivative of u with respect to x
  //      ...etc...

  inline real u(const real x, const real y, const real z, const int n, const real t=0.)
  {
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
    real r;
    if( numberOfDimensions==2 )     
      r= SQR(x-b0)+SQR(y-b1);
    else if( numberOfDimensions==3 )
      r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
    else
      r=SQR(x-b0);
    real rp=pow(r,p);  

    return a0*exp( - a1*rp );
  }
  inline real ut(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
    real r;
    if( numberOfDimensions==2 )     
      r= SQR(x-b0)+SQR(y-b1);
    else if( numberOfDimensions==3 )
      r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
    else
      r=SQR(x-b0);
    real rp=pow(r,p);  
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5
    if( numberOfDimensions==2 )     
      return a0*exp( - a1*rp )*( 2.*a1*p*rp/r*( (x-b0)*v0 + (y-b1)*v1 ));
    else if( numberOfDimensions==3 )
      return a0*exp( - a1*rp )*( 2.*a1*p*rp/r*( (x-b0)*v0 + (y-b1)*v1 +(z-b2)*v2 ));
    else
      return a0*exp( - a1*rp )*( 2.*a1*p*rp/r*( (x-b0)*v0 ) );
  }
  inline real ux(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
  
    real r;
    if( numberOfDimensions==2 )     
      r= SQR(x-b0)+SQR(y-b1);
    else if( numberOfDimensions==3 )
      r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
    else
      r=SQR(x-b0);
    real rp=pow(r,p);  
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5
    return a0*exp(-a1*rp)*( -2.*a1*p*rp/r*( (x-b0)) );
  }
  inline real uy(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    if( numberOfDimensions==1 )
      return 0.;
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
  
    real r;
    if( numberOfDimensions==2 )     
      r= SQR(x-b0)+SQR(y-b1);
    else if( numberOfDimensions==3 )
      r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
    else
      r=SQR(x-b0);
    real rp=pow(r,p);  
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5
    return a0*exp(-a1*rp)*( -2.*a1*p*rp/r*( (y-b1)) );
  }
  inline real uxx(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
  
    real r;
    if( numberOfDimensions==2 )     
      r= SQR(x-b0)+SQR(y-b1);
    else if( numberOfDimensions==3 )
      r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
    else
      r=SQR(x-b0);
      
    real rp=pow(r,p);
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5
    real g = -2.*a1*p*rp/r*(x-b0);
    return a0*exp(-a1*rp)*(g*g-2.*a1*p*rp/r*(1.+2.*(p-1.)/r*SQR(x-b0)));
  }
  
  inline real uxy(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    if( numberOfDimensions==1 )
      return 0.;
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
  
    real r;
    if( numberOfDimensions==2 )     
      r= SQR(x-b0)+SQR(y-b1);
    else if( numberOfDimensions==3 )
      r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
    else
      r=SQR(x-b0);
      
    real rp=pow(r,p);
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5
    real gx = -2.*a1*p*rp/r*(x-b0);
    real gy = -2.*a1*p*rp/r*(y-b1);
    return a0*exp(-a1*rp)*(gx*gy-4.*a1*p*(p-1.)*rp/(r*r)*(x-b0)*(y-b1));
  }
  inline real uyy(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    if( numberOfDimensions==1 )
      return 0.;
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
  
    real r;
    if( numberOfDimensions==2 )     
      r= SQR(x-b0)+SQR(y-b1);
    else if( numberOfDimensions==3 )
      r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
    else
      r=SQR(x-b0);
      
    real rp=pow(r,p);
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5
    real g = -2.*a1*p*rp/r*(y-b1);
    return a0*exp(-a1*rp)*(g*g-2.*a1*p*rp/r*(1.+2.*(p-1.)/r*SQR(y-b1)));
  }
  inline real uz(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    if( numberOfDimensions<=2 )
      return 0.;
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
  
    real r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
    real rp=pow(r,p);  
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5

    return a0*exp(-a1*rp)*( -2.*a1*p*rp/r*( (z-b2)) );
  }
  inline real uxz(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    if( numberOfDimensions<=2 )
      return 0.;
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
  
    real r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
      
    real rp=pow(r,p);
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5
    real gx = -2.*a1*p*rp/r*(x-b0);
    real gz = -2.*a1*p*rp/r*(z-b2);
    return a0*exp(-a1*rp)*(gx*gz-4.*a1*p*(p-1.)*rp/(r*r)*(x-b0)*(z-b2));
  }
  inline real uyz(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    if( numberOfDimensions<=2 )
      return 0.;
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
  
    real r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);
      
    real rp=pow(r,p);
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5
    real gy = -2.*a1*p*rp/r*(y-b1);
    real gz = -2.*a1*p*rp/r*(z-b2);
    return a0*exp(-a1*rp)*(gy*gz-4.*a1*p*(p-1.)*rp/(r*r)*(y-b1)*(z-b2));
  }
  inline real uzz(const real x, const real y, const real z, const int n=0, const real t=0.)
  {
    if( numberOfDimensions<=2 )
      return 0.;
    real b0=c0+v0*t;
    real b1=c1+v1*t;
    real b2=c2+v2*t;
  
    real r= SQR(x-b0)+SQR(y-b1)+SQR(z-b2);

    real rp=pow(r,p);
    r=max(r,REAL_MIN*100.);  // avoid division by zero, note that rp/r*(x-b0) == 0 if r==0, p>.5
    real g = -2.*a1*p*rp/r*(z-b2);
    return a0*exp(-a1*rp)*(g*g-2.*a1*p*rp/r*(1.+2.*(p-1.)/r*SQR(z-b2)));
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

 private:
  int numberOfComponents, numberOfDimensions;
  real a0,a1,c0,c1,c2,c3,v0,v1,v2,p;
  
  void checkArguments(const Index & N);

};


#undef TIME

#endif 
