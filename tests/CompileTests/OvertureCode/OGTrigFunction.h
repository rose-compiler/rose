#ifndef OGTrigFunction_h
#define OGTrigFunction_h "OGTrigFunction.h"

#include "OGFunction.h"

//
// Define a Function and it derivatives
//   This function can be used to define the "exact solution" for
//   an Overlapping Grid Appliciation (aka. TwilightZone Flow)
//
//      u_n(x,y,z,t) = a(n)*cos(fx(n)*Pi*(x-gx(n)))
//                         *cos(fy(n)*Pi*(y-gy(n)))
//                         *cos(fz(n)*Pi*(z-gz(n)))
//                         *cos(ft(n)*Pi*(t-gt(n))) + c(n)
//

class OGTrigFunction : public OGFunction
{
  RealArray a,cc;
  RealArray fx,fy,fz,ft;
  RealArray gx,gy,gz,gt;

 public:
  
  OGTrigFunction(const real & fx=1., 
		 const real & fy=1., 
		 const real & fz=0., 
		 const real & ft=0.,
                 const int & maximumNumberOfComponents=10);
  OGTrigFunction(const RealArray & fx, 
                 const RealArray & fy, 
                 const RealArray & fz, 
                 const RealArray & ft);
  OGTrigFunction(const OGTrigFunction & ogp );
  OGTrigFunction & operator=(const OGTrigFunction & ogp );
    
  ~OGTrigFunction(){};
  
  int setAmplitudes(const RealArray & a );
  int setConstants(const RealArray & c );

  int setFrequencies(const RealArray & fx, 
		     const RealArray & fy, 
		     const RealArray & fz, 
		     const RealArray & ft);
  int setShifts(const RealArray & gx, 
		const RealArray & gy, 
		const RealArray & gz, 
		const RealArray & gt);
  

  // Here are the member functions that you must define
  //     u(x,y,z,n,t) : function value at x,y,z for component n
  //    ux(x,y,z,n,t) : partial derivative of u with respect to x
  //      ...etc...

  inline real u(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return cos(fx(n)*(x-gx(n)))*cos(fy(n)*(y-gy(n)))*cos(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n)+cc(n);
  }
  inline real ut(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return -ft(n)*cos(fx(n)*(x-gx(n)))*cos(fy(n)*(y-gy(n)))*cos(fz(n)*(z-gz(n)))*sin(ft(n)*(t-gt(n)))*a(n);
  }
  inline real ux(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return -fx(n)*      sin(fx(n)*(x-gx(n)))*cos(fy(n)*(y-gy(n)))*cos(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n);
  }
  inline real uy(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return -fy(n)*      cos(fx(n)*(x-gx(n)))*sin(fy(n)*(y-gy(n)))*cos(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n);
  }
  inline real uxx(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return -fx(n)*fx(n)*cos(fx(n)*(x-gx(n)))*cos(fy(n)*(y-gy(n)))*cos(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n);
  }
  inline real uxy(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return fx(n)*fy(n)*sin(fx(n)*(x-gx(n)))*sin(fy(n)*(y-gy(n)))*cos(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n);
  }
  inline real uyy(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return -fy(n)*fy(n)*cos(fx(n)*(x-gx(n)))*cos(fy(n)*(y-gy(n)))*cos(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n);
  }
  inline real uz(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return -fz(n)*      cos(fx(n)*(x-gx(n)))*cos(fy(n)*(y-gy(n)))*sin(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n);
  }
  inline real uxz(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return fx(n)*fz(n)*sin(fx(n)*(x-gx(n)))*cos(fy(n)*(y-gy(n)))*sin(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n);
  }
  inline real uyz(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return fy(n)*fz(n)*cos(fx(n)*(x-gx(n)))*sin(fy(n)*(y-gy(n)))*sin(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n);
  }
  inline real uzz(const real x, const real y, const real z, const int n=0, const real t=0. )
  {
    return -fz(n)*fz(n)*cos(fx(n)*(x-gx(n)))*cos(fy(n)*(y-gy(n)))*cos(fz(n)*(z-gz(n)))*cos(ft(n)*(t-gt(n)))*a(n);
  }

  // ========= Here are versions with a new naming convention ===========

  // (default arguments not allowed on operators)
  virtual real operator()(const real , const real , const real);
  virtual real operator()(const real , const real , const real , const int);
  virtual real operator()(const real , const real , const real , const int, const real );

  virtual real t(const real , const real , const real , const int=0, const real=0. );
  virtual real x(const real , const real , const real , const int=0, const real=0. );
  virtual real y(const real , const real , const real , const int=0, const real=0. );
  virtual real xx(const real , const real , const real , const int=0, const real=0. );
  virtual real xy(const real , const real , const real , const int=0, const real=0. );
  virtual real yy(const real , const real , const real , const int=0, const real=0. );
  virtual real z(const real , const real , const real , const int=0, const real=0. );
  virtual real xz(const real , const real , const real , const int=0, const real=0. );
  virtual real yz(const real , const real , const real , const int=0, const real=0. );
  virtual real zz(const real , const real , const real , const int=0, const real=0. );
  virtual real xxx(const real , const real , const real , const int=0, const real=0. );
  virtual real xxxx(const real , const real , const real , const int=0, const real=0. );
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
  
};

#endif 
