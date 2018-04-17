// THIS FILE IS SPECIFIC TO P++ WITH THE A++PREPROCESSOR

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// These functions need to be seen when the users source file is compiled
// however we want to avoid having them multiplely defined (so we make them 
// inline functions).
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

// This should already be included within the users applications so 
// we don't have to include it again!
// Include A++.h so that we can compile the target template and transformation template code
// include <A++.h>

// If we can use the restrict pointers then we can generate FORTRAN 
// performance transformations using only the simple transformations
// The restrict keyword allows us to generate FORTRAN 77 performance using C++ code
// But restrict is not a standard part of C or C++, thus it is not available everywhere (e.g. DEC)
// define USE_RESTRICT_POINTERS_IN_ROSE_TRANSFORMATIONS TRUE

// The goal here is to provide a means of defining optimizations within object-oriented
// frameworks.  The definition of such transformations is entended to be as general
// as possible.

// if you change this line please change corresponding line in TransformationBase.h
#include "supported_dimensions.h"

#if USE_RESTRICT_POINTERS_IN_ROSE_TRANSFORMATIONS
#define RESTRICT restrict
#else
#define RESTRICT
#endif

#define ROSE_DUMMY_STATEMENT (void) NULL;

#ifdef USE_ROSE
// 1-3D subscript function declarations (in the compilation of the transformation 
// this will be a MACRO (when USE_ROSE is not defined), but when we build the 
// transformation (when ROSE is called and the preprocessor macro USE_ROSE is 
// defined) it will be built as a function call (to a function defined only 
// during ROSE preprocessing).  Internally we grab the bodies of these functions
// (and the program trees that they represent) and use then to avoid the use
// of a compositional model for the construction of the parts that go into 
// a transformation.  It is in general far easier for the user to specify and
// debug the code that a is represented by a transformation than to build the
// transformation directly using the Sage II implementation of the C++ grammar
// (the C++ grammar is far to complex to build meaningful transformations directly).

// ROSE:FUNCTION("ROSE_SUBSCRIPT_COMPUTATION_1D"):DEFINITION
inline int ROSE_SUBSCRIPT_COMPUTATION_1D (int i, int i_base, int i_bound) 
   {
     return i-i_base;
   }

// ROSE:FUNCTION("ROSE_SUBSCRIPT_COMPUTATION_1D"):USE
int x1 = ROSE_SUBSCRIPT_COMPUTATION_1D(0,0,0);

// ROSE:FUNCTION("ROSE_SUBSCRIPT_COMPUTATION_2D"):DEFINITION
inline int ROSE_SUBSCRIPT_COMPUTATION_2D (int i, int i_base, int i_bound, int j, int j_base, int j_bound)
   {
     return (j-j_base)*(i_bound-i_base+1)
			 +(i-i_base);
   }

// ROSE:FUNCTION("ROSE_SUBSCRIPT_COMPUTATION_2D"):USE
int x2 = ROSE_SUBSCRIPT_COMPUTATION_2D(0,0,0,0,0,0);

// ROSE:FUNCTION("ROSE_SUBSCRIPT_COMPUTATION_3D"):DEFINITION
inline int ROSE_SUBSCRIPT_COMPUTATION_3D (int i, int i_base, int i_bound, int j, int j_base, int j_bound, int k, int k_base, int k_bound)
   {
     return (k-k_base)*(j_bound-j_base+1)*(i_bound-i_base+1)
			 +(j-j_base)*(i_bound-i_base+1)
			 +(i-i_base);
   }

// ROSE:FUNCTION("ROSE_SUBSCRIPT_COMPUTATION_3D"):USE
int x3 = ROSE_SUBSCRIPT_COMPUTATION_3D(0,0,0,0,0,0,0,0,0);

// ROSE:FUNCTION("ROSE_SUBSCRIPT_COMPUTATION_3D"):DEFINITION
inline int ROSE_SUBSCRIPT_COMPUTATION_8D(
	int i, int i_base, int i_bound,
	int j, int j_base, int j_bound,
	int k, int k_base, int k_bound,
	int l, int l_base, int l_bound,
	int m, int m_base, int m_bound,
	int n, int n_base, int n_bound,
	int o, int o_base, int o_bound,
	int p, int p_base, int p_bound)
{
	// CW: it's not important what is returned, since
	// the function will never be used. It is only used as a code
	// template for ROSE
	return 0;
}

// ROSE:FUNCTION("ROSE_SUBSCRIPT_COMPUTATION_3D"):USE
int x8 = ROSE_SUBSCRIPT_COMPUTATION_8D(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

#else
// Not being used by the preprocessor (i.e. is being processed 
// by the C++ compiler (or some other preprocessor)

#define ROSE_SUBSCRIPT_COMPUTATION_1D(i,i_base,i_bound) \
	(i)-(i_base)

#define ROSE_SUBSCRIPT_COMPUTATION_2D(i,i_base,i_bound,j,j_base,j_bound) \
	((j)-(j_base))*((i_bound)-(i_base)+1) \
	+ ((i)-(i_base))

#define ROSE_SUBSCRIPT_COMPUTATION_3D(i,i_base,i_bound,j,j_base,j_bound,k,k_base,k_bound) \
	((k)-(k_base))*((j_bound)-(j_base)+1)*((i_bound)-(i_base)+1) \
	+((j)-(j_base))*((i_bound)-(i_base)+1) \
	+((i)-(i_base))

#define ROSE_SUBSCRIPT_COMPUTATION_8D(\
	i,i_base,i_bound,j,j_base,j_bound,k,k_base,k_bound,l,l_base,l_bound,\
	m,m_base,m_bound,n,n_base,n_bound,o,o_base,o_bound,p,p_base,p_bound)\
	((((((((p)-(p_base))*((o_bound)-(o_base)+1) \
	+((o)-(o_base)))*((n_bound)-(n_base)+1) \
	+((n)-(n_base)))*((m_bound)-(m_base)+1) \
	+((m)-(m_base)))*((l_bound)-(l_base)+1) \
	+((l)-(l_base)))*((k_bound)-(k_base)+1) \
	+((k)-(k_base)))*((j_bound)-(j_base)+1) \
	+((j)-(j_base)))*((i_bound)-(i_base)+1) \
	+((i)-(i_base))


// if USE_ROSE
#endif

// Define a dummy class that can hold these functions
class SimpleArrayAssignment
   {
     public:
          void target();
          void transformationTemplate();
   };

// *************************************************************************
// Define the target of our transformations for "SimpleArrayAssignment"
// *************************************************************************

void 
SimpleArrayAssignment::target ()
   {
  // example: A = B + C
  // This code (template) is not used but was originally intended 
  // to be used as a sort of target for the optimization that 
  // follows in the next function.

  // We require enough declarations to make the target code properly defined
     doubleArray A(1);
     doubleArray B(1);

// Use this to debug case of multiple pragmas
// #pragma rose testA
#pragma rose transformTarget on
     A = B;   // transform this line only
#pragma rose transformTarget off
     ROSE_DUMMY_STATEMENT
   }

// *************************************************************************
// Define the transformation template for "SimpleArrayAssignment"
// *************************************************************************


void
SimpleArrayAssignment::transformationTemplate ()
   {
  // example: A = B + C loop structure
  // We can make this more complex later

  // We require enough declarations to make the target code properly defined
  // This code is not part of the transformation
     doubleArray LHS_ARRAY(1);
     doubleArray RHS_ARRAY(1);

#pragma rose transformTemplate on
     ROSE_DUMMY_STATEMENT
  // This code represents the transformation
#pragma rose transformTemplate uniqueCode on
     ROSE_DUMMY_STATEMENT
     int GLOBAL_INDEX_NAME[ROSE_MAX_ARRAY_DIMENSION];
     int rose_stride[ROSE_MAX_ARRAY_DIMENSION];
     int rose_base[ROSE_MAX_ARRAY_DIMENSION];
     int rose_bound[ROSE_MAX_ARRAY_DIMENSION];
  // Call P++ specific functions to setup the serial array objects
  // This code is only required once for all the operands in the same scope
#pragma rose transformTemplate uniqueCode off
	ROSE_DUMMY_STATEMENT

#pragma rose transformTemplate LhsOperandSpecificCode on
  // This is code that is required once for the Lhs operand
  // double* RESTRICT LHS_ARRAY_DATA_POINTER = LHS_ARRAY.getDataPointer();
     double* RESTRICT LHS_ARRAY_DATA_POINTER = LHS_ARRAY.getSerialArray().getDataPointer();
#pragma rose transformTemplate LhsOperandSpecificCode off
     ROSE_DUMMY_STATEMENT

#pragma rose transformTemplate operandSpecificCode on
     ROSE_DUMMY_STATEMENT
	// This code is required for each operand We really want to get a
	// pointer to the data in B that can be used with the index for A
	// (i.e. "i") But this requires a new feature in A++ so skip it for
	// now double* RESTRICT RHS_ARRAY_DATA_POINTER =
	// RHS_ARRAY.getDataPointer(A);
  // double* RESTRICT RHS_ARRAY_DATA_POINTER = RHS_ARRAY.getDataPointer();
     double* RESTRICT RHS_ARRAY_DATA_POINTER = RHS_ARRAY.getSerialArray().getDataPointer();
#pragma rose transformTemplate operandSpecificCode off
     ROSE_DUMMY_STATEMENT

#pragma rose transformTemplate loopConstructionCode_1D on
	ROSE_DUMMY_STATEMENT
	// This code is required only once for this transformation
	assert(LHS_ARRAY.numberOfDimensions()==1);
	rose_base[0]=LHS_ARRAY.getLocalBase(0);
	rose_bound[0]=LHS_ARRAY.getLocalBound(0);
	rose_stride[0]=LHS_ARRAY.getLocalStride(0);
	for(GLOBAL_INDEX_NAME[0] = rose_base[0];
		GLOBAL_INDEX_NAME[0] <= rose_bound[0];
		GLOBAL_INDEX_NAME[0] += rose_stride[0])
	{
// We will replace only a single statement so this transformation 
// must have only one statement in its inner loop body
// pragma rose transformTemplate innerloop on
		// This code would be modified with the edited user inner loop statement 
		// The first statement will be replaced within the transformation process
		LHS_ARRAY_DATA_POINTER[ROSE_SUBSCRIPT_COMPUTATION_1D(GLOBAL_INDEX_NAME[0],0,0)] = RHS_ARRAY_DATA_POINTER[0];
// pragma rose transformTemplate innerloop off

	}
#pragma rose transformTemplate loopConstructionCode_1D off
     ROSE_DUMMY_STATEMENT

#pragma rose transformTemplate loopConstructionCode_2D on
	ROSE_DUMMY_STATEMENT
	assert(LHS_ARRAY.numberOfDimensions()==2);
	// This code is required only once for this transformation
	rose_base[0]   = LHS_ARRAY.getBase(0);
	rose_bound[0]  = LHS_ARRAY.getBound(0);
	rose_stride[0] = LHS_ARRAY.getStride(0);
	rose_base[1]   = LHS_ARRAY.getBase(1);
	rose_bound[1]  = LHS_ARRAY.getBound(1);
	rose_stride[1] = LHS_ARRAY.getStride(1);
	for(GLOBAL_INDEX_NAME[1] = rose_base[1];
		GLOBAL_INDEX_NAME[1] <= rose_bound[1];
		GLOBAL_INDEX_NAME[1] += rose_stride[1])
	{
		for(GLOBAL_INDEX_NAME[0] = rose_base[0];
			GLOBAL_INDEX_NAME[0] <= rose_bound[0];
			GLOBAL_INDEX_NAME[0] += rose_stride[0])
		{
// pragma rose transformTemplate innerloop on
			// This code would be modified with the edited user inner loop statement 
			// The first statement will be replaced within the transformation process
			LHS_ARRAY_DATA_POINTER = RHS_ARRAY_DATA_POINTER;
// pragma rose transformTemplate innerloop off
		}
	}
#pragma rose transformTemplate loopConstructionCode_2D off
	ROSE_DUMMY_STATEMENT

#pragma rose transformTemplate loopConstructionCode_3D on
	ROSE_DUMMY_STATEMENT
	assert(LHS_ARRAY.numberOfDimensions()==3);
	rose_base[0]   = LHS_ARRAY.getBase(0);
	rose_bound[0]  = LHS_ARRAY.getBound(0);
	rose_stride[0] = LHS_ARRAY.getStride(0);
	rose_base[1]   = LHS_ARRAY.getBase(1);
	rose_bound[1]  = LHS_ARRAY.getBound(1);
	rose_stride[1] = LHS_ARRAY.getStride(1);
	rose_base[2]   = LHS_ARRAY.getBase(2);
	rose_bound[2]  = LHS_ARRAY.getBound(2);
	rose_stride[2] = LHS_ARRAY.getStride(2);
	for(GLOBAL_INDEX_NAME[2] = rose_base[2];
		GLOBAL_INDEX_NAME[2] <= rose_bound[2];
		GLOBAL_INDEX_NAME[2] += rose_stride[2])
	{
		for(GLOBAL_INDEX_NAME[1] = rose_base[1];
			GLOBAL_INDEX_NAME[1] <= rose_bound[1];
			GLOBAL_INDEX_NAME[1] += rose_stride[1])
		{
			for(GLOBAL_INDEX_NAME[0] = rose_base[0];
				GLOBAL_INDEX_NAME[0] <= rose_bound[0];
				GLOBAL_INDEX_NAME[0] += rose_stride[0])
			{
// pragma rose transformTemplate innerloop on
				// This code would be modified with the edited user inner loop statement 
				// The first statement will be replaced within the transformation process
				LHS_ARRAY_DATA_POINTER = RHS_ARRAY_DATA_POINTER;
// pragma rose transformTemplate innerloop off
			}
		}
	}
#pragma rose transformTemplate loopConstructionCode_3D off
	ROSE_DUMMY_STATEMENT
	
#pragma rose transformTemplate loopConstructionCode_8D on
	ROSE_DUMMY_STATEMENT
	assert(LHS_ARRAY.numberOfDimensions()<=8);
	rose_base[0]   = LHS_ARRAY.getBase(0);
	rose_bound[0]  = LHS_ARRAY.getBound(0);
	rose_stride[0] = LHS_ARRAY.getStride(0);
	rose_base[1]   = LHS_ARRAY.getBase(1);
	rose_bound[1]  = LHS_ARRAY.getBound(1);
	rose_stride[1] = LHS_ARRAY.getStride(1);
	rose_base[2]   = LHS_ARRAY.getBase(2);
	rose_bound[2]  = LHS_ARRAY.getBound(2);
	rose_stride[2] = LHS_ARRAY.getStride(2);
	rose_base[3]   = LHS_ARRAY.getBase(3);
	rose_bound[3]  = LHS_ARRAY.getBound(3);
	rose_stride[3] = LHS_ARRAY.getStride(3);
	rose_base[4]   = LHS_ARRAY.getBase(4);
	rose_bound[4]  = LHS_ARRAY.getBound(4);
	rose_stride[4] = LHS_ARRAY.getStride(4);
	rose_base[5]   = LHS_ARRAY.getBase(5);
	rose_bound[5]  = LHS_ARRAY.getBound(5);
	rose_stride[5] = LHS_ARRAY.getStride(5);
	rose_base[6]   = LHS_ARRAY.getBase(6);
	rose_bound[6]  = LHS_ARRAY.getBound(6);
	rose_stride[6] = LHS_ARRAY.getStride(6);
	rose_base[7]   = LHS_ARRAY.getBase(7);
	rose_bound[7]  = LHS_ARRAY.getBound(7);
	rose_stride[7] = LHS_ARRAY.getStride(7);
	for(GLOBAL_INDEX_NAME[7] = rose_base[7];
		GLOBAL_INDEX_NAME[7] <= rose_bound[7];
		GLOBAL_INDEX_NAME[7] += rose_stride[7])
	{
		for(GLOBAL_INDEX_NAME[6] = rose_base[6];
		GLOBAL_INDEX_NAME[6] <= rose_bound[6];
		GLOBAL_INDEX_NAME[6] += rose_stride[6])
		{
			for(GLOBAL_INDEX_NAME[5] = rose_base[5];
			GLOBAL_INDEX_NAME[5] <= rose_bound[5];
			GLOBAL_INDEX_NAME[5] += rose_stride[5])
			{
				for(GLOBAL_INDEX_NAME[4] = rose_base[4];
				GLOBAL_INDEX_NAME[4] <= rose_bound[4];
				GLOBAL_INDEX_NAME[4] += rose_stride[4])
				{
					for(GLOBAL_INDEX_NAME[3] = rose_base[3];
					GLOBAL_INDEX_NAME[3] <= rose_bound[3];
					GLOBAL_INDEX_NAME[3] += rose_stride[3])
					{
						for(GLOBAL_INDEX_NAME[2] = rose_base[2];
						GLOBAL_INDEX_NAME[2] <= rose_bound[2];
						GLOBAL_INDEX_NAME[2] += rose_stride[2])
						{
							for(GLOBAL_INDEX_NAME[1] = rose_base[1];
								GLOBAL_INDEX_NAME[1] <= rose_bound[1];
								GLOBAL_INDEX_NAME[1] += rose_stride[1])
							{
								for(GLOBAL_INDEX_NAME[0] = rose_base[0];
									GLOBAL_INDEX_NAME[0] <= rose_bound[0];
									GLOBAL_INDEX_NAME[0] += rose_stride[0])
								{
// pragma rose transformTemplate innerloop on
				// This code would be modified with the edited user inner loop statement 
				// The first statement will be replaced within the transformation process
									LHS_ARRAY_DATA_POINTER = RHS_ARRAY_DATA_POINTER;
// pragma rose transformTemplate innerloop off
								}
							}
						}
					}
				}
			}
		}
	}
#pragma rose transformTemplate loopConstructionCode_8D off
	ROSE_DUMMY_STATEMENT


#pragma rose transformTemplate off
	ROSE_DUMMY_STATEMENT
}








