#ifndef OG_FUNCTION_H
#define OG_FUNCTION_H "OGFunction.h"

#include "Overture.h"

//
// Define a Function and it derivatives
//   This function can be used to define the "exact solution" for
//   an Overlapping Grid Appliciation (aka. TwilightZone Flow)
//
// This is the base Class from which particular functions are defineed
// by derivativation. See OGTrigFunction.h for example

class OGFunction
{

 public:
  

  OGFunction( ){}
  ~OGFunction(){}
  
  // Here are the (inline) member functions that you must define
  //     u(x,y,z,n,t) : function value at x,y,z for component n
  //    ux(x,y,z,n,t) : partial derivative of u with respect to x
  //      ...etc...
  inline real u(const real , const real , const real , const int=0, const real=0. );

  inline real ut(const real , const real , const real , const int=0, const real=0. );

  inline real ux(const real , const real , const real , const int=0, const real=0. );

  inline real uy(const real , const real , const real , const int=0, const real=0. );

  inline real uxx(const real , const real , const real , const int=0, const real=0. );
  
  inline real uxy(const real , const real , const real , const int=0, const real=0. );
  
  inline real uyy(const real , const real , const real , const int=0, const real=0. );
  
  inline real uz(const real , const real , const real , const int=0, const real=0. );
  
  inline real uxz(const real , const real , const real , const int=0, const real=0. );
  
  inline real uyz(const real , const real , const real , const int=0, const real=0. );

  inline real uzz(const real , const real , const real , const int=0, const real=0. );

  
  // (default arguments not allowed on operators)
  virtual real operator()(const real , const real , const real) = 0;
  virtual real operator()(const real , const real , const real , const int) = 0;
  virtual real operator()(const real , const real , const real , const int, const real ) = 0;

  virtual real t(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real x(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real y(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real xx(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real xy(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real yy(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real z(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real xz(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real yz(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real zz(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real xxx(const real , const real , const real , const int=0, const real=0. ) = 0;
  virtual real xxxx(const real , const real , const real , const int=0, const real=0. ) = 0;
  // obtain a general derivative
  virtual real gd(const int & ntd, const int & nxd, const int & nyd, const int & nzd,
                  const real , const real , const real , const int=0, const real=0. ) = 0;

  virtual RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
					  const Index & I3, const Index & N) = 0;

  virtual RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
					  const Index & I3, const Index & N, const real t,
					  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;

  virtual RealDistributedArray t(const MappedGrid & c, const Index & I1, const Index & I2, 
                                 const Index & I3, const Index & N, const real t=0., 
				 const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray x(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray y(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray z(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray xx(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray yy(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray zz(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray xy(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray xz(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray yz(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;

  virtual RealDistributedArray laplacian(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;

  virtual RealDistributedArray xxx(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;
  virtual RealDistributedArray xxxx(const MappedGrid & c, const Index & I1, const Index & I2, 
              const Index & I3, const Index & N, const real t=0.,const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering) = 0 ;

  virtual RealDistributedArray gd(const int & ntd, const int & nxd, const int & nyd, const int & nzd,
				  const MappedGrid & c, const Index & I1, const Index & I2, 
				  const Index & I3, const Index & N, const real t=0.,
				  const GridFunctionParameters::GridFunctionType & centering
				  =GridFunctionParameters::defaultCentering) = 0 ;

  // ------------- These functions take an int for the component number -----  
  //               (and call the above routines)

  RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
				  const Index & I3);
  RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
				  const Index & I3, const int n);
  RealDistributedArray operator()(const MappedGrid & c, const Index & I1, const Index & I2, 
				  const Index & I3, const int n, const real t,
				  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray t(const MappedGrid & c, const Index & I1, const Index & I2, 
                         const Index & I3, const int n=0, const real t=0.,
			 const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray x(const MappedGrid & c, const Index & I1, const Index & I2, 
                         const Index & I3, const int n=0, const real t=0.,
			 const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray y(const MappedGrid & c, const Index & I1, const Index & I2, 
                         const Index & I3, const int n=0, const real t=0.,
			 const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray z(const MappedGrid & c, const Index & I1, const Index & I2, 
                         const Index & I3, const int n=0, const real t=0.,
			 const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray xx(const MappedGrid & c, const Index & I1, const Index & I2, 
			  const Index & I3, const int n=0, const real t=0.,
			  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray yy(const MappedGrid & c, const Index & I1, const Index & I2, 
			  const Index & I3, const int n=0, const real t=0.,
			  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray zz(const MappedGrid & c, const Index & I1, const Index & I2, 
			  const Index & I3, const int n=0, const real t=0.,
			  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray xy(const MappedGrid & c, const Index & I1, const Index & I2, 
			  const Index & I3, const int n=0, const real t=0.,
			  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray xz(const MappedGrid & c, const Index & I1, const Index & I2, 
			  const Index & I3, const int n=0, const real t=0.,
			  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray yz(const MappedGrid & c, const Index & I1, const Index & I2, 
			  const Index & I3, const int n=0, const real t=0.,
			  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );

  RealDistributedArray laplacian(const MappedGrid & c, const Index & I1, const Index & I2, 
			  const Index & I3, const int n=0, const real t=0.,
			  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );

  RealDistributedArray xxx(const MappedGrid & c, const Index & I1, const Index & I2, 
                           const Index & I3, const int n=0, const real t=0.,
			   const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );
  RealDistributedArray xxxx(const MappedGrid & c, const Index & I1, const Index & I2, 
			    const Index & I3, const int n=0, const real t=0.,
			    const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );

  RealDistributedArray gd(const int & ntd, const int & nxd, const int & nyd, const int & nzd,
			  const MappedGrid & c, const Index & I1, const Index & I2, 
			  const Index & I3, const int n=0, const real t=0.,
			  const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering );

  realCompositeGridFunction operator()(CompositeGrid & cg);
  realCompositeGridFunction operator()(CompositeGrid & cg, const Index & N = nullIndex);
  realCompositeGridFunction operator()(CompositeGrid & cg, const Index & N, const real t,
                                       const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction t(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
			      const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction x(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
			      const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction y(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
			      const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction z(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
			      const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction xx(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
                               const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction xy(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
                               const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction xz(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
                               const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction yy(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
                               const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction yz(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
                               const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction zz(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
                               const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);

  realCompositeGridFunction laplacian(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
                               const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);

  realCompositeGridFunction xxx(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
				const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);
  realCompositeGridFunction xxxx(CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
                                 const GridFunctionParameters::GridFunctionType & centering=GridFunctionParameters::defaultCentering);

  realCompositeGridFunction gd(const int & ntd, const int & nxd, const int & nyd, const int & nzd,
                               CompositeGrid & cg, const Index & N = nullIndex, const real t=0.,
			       const GridFunctionParameters::GridFunctionType & centering
                                 =GridFunctionParameters::defaultCentering);




  //=================================================================================
  //  Assign a grid Function with values defined from the OGFunction f
  //
  //=================================================================================
  void assignGridFunction(realCompositeGridFunction & v, const real t=0.);

  // ***** to add:  *******************************************************************
  // void assignGridFunction(realMappedGridFunction & v, const real t=0.);

  // utility routine:
  void getCoordinates(const GridFunctionParameters::GridFunctionType & centering, 
		      const int & numberOfDimensions,
		      const MappedGrid & c, 
		      const Index & I1, const Index & I2, const Index & I3,
		      RealDistributedArray & x, 
		      RealDistributedArray & y, 
		      RealDistributedArray & z );

};

#endif 
