// define the keyword COLLECTION_FUNCTION to be GridCollectionFunction or CompositeGridFunction
// define the keyword INT_COLLECTION_FUNCTION to be intGridCollectionFunction or intCompositeGridFunction
// define the keyword INTEGRAL_TYPE to be one of int, float double


  // define a few useful operators
  COLLECTION_FUNCTION & operator+ ( const COLLECTION_FUNCTION & cgf ) const;
  COLLECTION_FUNCTION & operator- ( const COLLECTION_FUNCTION & cgf ) const;
  COLLECTION_FUNCTION & operator* ( const COLLECTION_FUNCTION & cgf ) const;
  COLLECTION_FUNCTION & operator/ ( const COLLECTION_FUNCTION & cgf ) const;
  COLLECTION_FUNCTION & operator+=( const COLLECTION_FUNCTION & cgf );
  COLLECTION_FUNCTION & operator-=( const COLLECTION_FUNCTION & cgf );
  COLLECTION_FUNCTION & operator*=( const COLLECTION_FUNCTION & cgf );
  COLLECTION_FUNCTION & operator/=( const COLLECTION_FUNCTION & cgf );

  COLLECTION_FUNCTION & operator+ () const;
  COLLECTION_FUNCTION & operator- () const;

  COLLECTION_FUNCTION & operator= ( const INTEGRAL_TYPE value );
  COLLECTION_FUNCTION & operator+ ( const INTEGRAL_TYPE value ) const;
  COLLECTION_FUNCTION & operator- ( const INTEGRAL_TYPE value ) const;
  COLLECTION_FUNCTION & operator* ( const INTEGRAL_TYPE value ) const;
  COLLECTION_FUNCTION & operator/ ( const INTEGRAL_TYPE value ) const;
  COLLECTION_FUNCTION & operator+=( const INTEGRAL_TYPE value );
  COLLECTION_FUNCTION & operator-=( const INTEGRAL_TYPE value );
  COLLECTION_FUNCTION & operator*=( const INTEGRAL_TYPE value );
  COLLECTION_FUNCTION & operator/=( const INTEGRAL_TYPE value );

  // comparison operators return an intGridCollectionFunction
  INT_COLLECTION_FUNCTION operator <  ( const COLLECTION_FUNCTION & cgf ) const;
  INT_COLLECTION_FUNCTION operator <= ( const COLLECTION_FUNCTION & cgf ) const;
  INT_COLLECTION_FUNCTION operator >  ( const COLLECTION_FUNCTION & cgf ) const;
  INT_COLLECTION_FUNCTION operator >= ( const COLLECTION_FUNCTION & cgf ) const;
  INT_COLLECTION_FUNCTION operator == ( const COLLECTION_FUNCTION & cgf ) const;
  INT_COLLECTION_FUNCTION operator != ( const COLLECTION_FUNCTION & cgf ) const;

  friend COLLECTION_FUNCTION & operator+ (const INTEGRAL_TYPE value, const COLLECTION_FUNCTION & cgf);
  friend COLLECTION_FUNCTION & operator- (const INTEGRAL_TYPE value, const COLLECTION_FUNCTION & cgf);
  friend COLLECTION_FUNCTION & operator* (const INTEGRAL_TYPE value, const COLLECTION_FUNCTION & cgf);
  friend COLLECTION_FUNCTION & operator/ (const INTEGRAL_TYPE value, const COLLECTION_FUNCTION & cgf);


  friend INTEGRAL_TYPE max( const COLLECTION_FUNCTION & cgf );
  friend INTEGRAL_TYPE min( const COLLECTION_FUNCTION & cgf );


// Now define derivative functions for gridCollectionFunction and compositeGridFunctions

private:
  void derivativeError() const;
  void boundaryConditionError() const;

public:


// Macro to define a typical function call
#define FUNCTION(type) \
COLLECTION_FUNCTION type(\
                         const Index & C1 = nullIndex,      \
			 const Index & C2 = nullIndex,      \
			 const Index & C3 = nullIndex,      \
			 const Index & C4 = nullIndex,      \
			 const Index & C5 = nullIndex ) const; \
COLLECTION_FUNCTION type(\
                         const GridFunctionParameters & gfType, \
                         const Index & C1 = nullIndex,      \
			 const Index & C2 = nullIndex,      \
			 const Index & C3 = nullIndex,      \
			 const Index & C4 = nullIndex,      \
			 const Index & C5 = nullIndex ) const;
     

#define FUNCTION_COEFFICIENTS(type) \
  COLLECTION_FUNCTION type(const Index & C0 = nullIndex,      \
                           const Index & C1 = nullIndex,      \
			   const Index & C2 = nullIndex,      \
			   const Index & C3 = nullIndex ) const; \
  COLLECTION_FUNCTION type(const GridFunctionParameters & gfType, \
                           const Index & C0 = nullIndex,      \
			   const Index & C1 = nullIndex,      \
			   const Index & C2 = nullIndex,      \
			   const Index & C3 = nullIndex ) const;
     


  // parametric derivatives in the r1,r2,r3 directions
  FUNCTION(r1)
  FUNCTION_COEFFICIENTS(r1Coefficients)
  FUNCTION(r2)
  FUNCTION_COEFFICIENTS(r2Coefficients)
  FUNCTION(r3)
  FUNCTION_COEFFICIENTS(r3Coefficients)
  FUNCTION(r1r1)
  FUNCTION_COEFFICIENTS(r1r1Coefficients)
  FUNCTION(r1r2)
  FUNCTION_COEFFICIENTS(r1r2Coefficients)
  FUNCTION(r1r3)
  FUNCTION_COEFFICIENTS(r1r3Coefficients)
  FUNCTION(r2r2)
  FUNCTION_COEFFICIENTS(r2r2Coefficients)
  FUNCTION(r2r3)
  FUNCTION_COEFFICIENTS(r2r3Coefficients)
  FUNCTION(r3r3)
  FUNCTION_COEFFICIENTS(r3r3Coefficients)

  // FUNCTIONs in the x,y,z directions
  FUNCTION(x)
  FUNCTION_COEFFICIENTS(xCoefficients)
  FUNCTION(y)
  FUNCTION_COEFFICIENTS(yCoefficients)
  FUNCTION(z)
  FUNCTION_COEFFICIENTS(zCoefficients)
  FUNCTION(xx)
  FUNCTION_COEFFICIENTS(xxCoefficients)
  FUNCTION(xy)
  FUNCTION_COEFFICIENTS(xyCoefficients)
  FUNCTION(xz)
  FUNCTION_COEFFICIENTS(xzCoefficients)
  FUNCTION(yy)
  FUNCTION_COEFFICIENTS(yyCoefficients)
  FUNCTION(yz)
  FUNCTION_COEFFICIENTS(yzCoefficients)
  FUNCTION(zz)
  FUNCTION_COEFFICIENTS(zzCoefficients)

  // other forms of derivatives

 // compute face-centered variable from cell-centered variable 
  FUNCTION(cellsToFaces)

  //compute (u.grad)u (convective derivative)
  FUNCTION(convectiveDerivative)

  // compute contravariant velocity from either cell-centered or face-centered input velocity
  FUNCTION(contravariantVelocity)

  FUNCTION(div)
  FUNCTION_COEFFICIENTS(divCoefficients)

  //returns cell-centered divergence given normal velocities
  FUNCTION(divNormal)

  // compute faceArea-weighted normal velocity from either cell-centered or 
  // face-centered input velocity (this is just an alias for contravariantVelocity)
  FUNCTION(normalVelocity)

  FUNCTION(grad)
  FUNCTION_COEFFICIENTS(gradCoefficients)

  FUNCTION(identity)
  FUNCTION_COEFFICIENTS(identityCoefficients)

  FUNCTION(laplacian)
  FUNCTION_COEFFICIENTS(laplacianCoefficients)

  FUNCTION(vorticity)

#undef FUNCTION  
#undef FUNCTION_COEFFICIENTS
  // ******* derivatives in non-standard  form  ***********

  //compute (u.grad)w (convective derivative of passive variable(s))
  COLLECTION_FUNCTION convectiveDerivative(const COLLECTION_FUNCTION &w,
                                           const Index & C1 = nullIndex,
					   const Index & C2 = nullIndex,
					   const Index & C3 = nullIndex
					   ) const;   

  COLLECTION_FUNCTION convectiveDerivative (const GridFunctionParameters & gfType,
					    const COLLECTION_FUNCTION &w,
					    const Index & C1 = nullIndex,
					    const Index & C2 = nullIndex,
					    const Index & C3 = nullIndex
					    ) const;

#define SCALAR_FUNCTION(type) \
COLLECTION_FUNCTION type(\
                         const COLLECTION_FUNCTION & s,    \
			 const Index & C1 = nullIndex,      \
			 const Index & C2 = nullIndex,      \
			 const Index & C3 = nullIndex,      \
			 const Index & C4 = nullIndex ) const; \
COLLECTION_FUNCTION type(\
                         const GridFunctionParameters & gfType, \
                         const COLLECTION_FUNCTION & s,         \
                         const Index & C1 = nullIndex,      \
			 const Index & C2 = nullIndex,      \
			 const Index & C3 = nullIndex,      \
			 const Index & C4 = nullIndex ) const; 

#define SCALAR_FUNCTION_COEFFICIENTS(type) \
  COLLECTION_FUNCTION type(const COLLECTION_FUNCTION & s,    \
                           const Index & C0 = nullIndex,      \
			   const Index & C1 = nullIndex,      \
			   const Index & C2 = nullIndex,      \
			   const Index & C3 = nullIndex ) const; \
  COLLECTION_FUNCTION type(const GridFunctionParameters & gfType, \
                           const COLLECTION_FUNCTION & s,    \
                           const Index & C0 = nullIndex,      \
			   const Index & C1 = nullIndex,      \
			   const Index & C2 = nullIndex,      \
			   const Index & C3 = nullIndex ) const;
     


  // div(s grad(u)), s=scalar field
  SCALAR_FUNCTION(divScalarGrad)
  SCALAR_FUNCTION_COEFFICIENTS(divScalarGradCoefficients)

  SCALAR_FUNCTION(divInverseScalarGrad)
  SCALAR_FUNCTION_COEFFICIENTS(divInverseScalarGradCoefficients)

  SCALAR_FUNCTION(scalarGrad)
  SCALAR_FUNCTION_COEFFICIENTS(scalarGradCoefficients)

  SCALAR_FUNCTION(divVectorScalar)
  SCALAR_FUNCTION_COEFFICIENTS(divVectorScalarCoefficients)

#undef SCALAR_FUNCTION
#undef SCALAR_FUNCTION_COEFFICIENTS

COLLECTION_FUNCTION derivativeScalarDerivative(
                         const COLLECTION_FUNCTION & s,    
                         const int & direction1, 
                         const int & direction2, 
			 const Index & C1 = nullIndex,      
			 const Index & C2 = nullIndex,      
			 const Index & C3 = nullIndex,      
			 const Index & C4 = nullIndex ) const; 
COLLECTION_FUNCTION derivativeScalarDerivative(
                         const GridFunctionParameters & gfType, 
		         const COLLECTION_FUNCTION & s,    
                         const int & direction1, 
                         const int & direction2, 
                         const Index & C1 = nullIndex,      
			 const Index & C2 = nullIndex,      
			 const Index & C3 = nullIndex,      
			 const Index & C4 = nullIndex ) const; 

COLLECTION_FUNCTION derivativeScalarDerivativeCoefficients(const COLLECTION_FUNCTION & s,    
                           const int & direction1, 
                           const int & direction2, 
                           const Index & C0 = nullIndex,      
			   const Index & C1 = nullIndex,      
			   const Index & C2 = nullIndex,      
			   const Index & C3 = nullIndex ) const; 

COLLECTION_FUNCTION derivativeScalarDerivativeCoefficients(const GridFunctionParameters & gfType, 
                           const COLLECTION_FUNCTION & s,    
                           const int & direction1, 
                           const int & direction2, 
                           const Index & C0 = nullIndex,      
			   const Index & C1 = nullIndex,      
			   const Index & C2 = nullIndex,      
			   const Index & C3 = nullIndex ) const;
     




  //returns face-centered gradients
  COLLECTION_FUNCTION FCgrad (
                                 const int c0 = 0,
				 const int c1 = 0,
				 const int c2 = 0,
				 const int c3 = 0,
				 const int c4 = 0
				 ) const;
  COLLECTION_FUNCTION FCgrad (const GridFunctionParameters & gfType,
                                 const int c0 = 0,
				 const int c1 = 0,
				 const int c2 = 0,
				 const int c3 = 0,
				 const int c4 = 0
				 ) const;



  // ********************************************************************
  // ------------- Here we define the Boundary Conditions ---------------
  // ********************************************************************

  void applyBoundaryConditions(
                                       const real & time = 0.,
				       const int & grid =forAll);
  // fill in coefficients for the boundary conditions
  void assignBoundaryConditionCoefficients(
                                                   const real & time = 0.,
						   const int & grid =forAll);
  // new BC interface:
  void applyBoundaryCondition(const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType=BCTypes::dirichlet,
                              const int & boundaryCondition = BCTypes::allBoundaries,
                              const real & forcing = 0.,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());
  

  void applyBoundaryCondition(const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const RealArray & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());

  void applyBoundaryCondition(const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const COLLECTION_FUNCTION & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());

#ifdef USE_PPP
  void applyBoundaryCondition(const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const RealDistributedArray & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());

#endif
  // fix corners and periodic update:
  void finishBoundaryConditions(const BoundaryConditionParameters & bcParameters= Overture::defaultBoundaryConditionParameters(),
                                const Range & C0=nullRange);
  
  void applyBoundaryConditionCoefficients(const Index & Equation,
                                          const Index & Components,
					  const BCTypes::BCNames & boundaryConditionType=BCTypes::dirichlet,
					  const int & boundaryCondition = BCTypes::allBoundaries,
					  const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());
  
