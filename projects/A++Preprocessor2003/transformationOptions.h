
#ifndef TRANSFORMATION_OPTIONS_H
#define TRANSFORMATION_OPTIONS_H

class TransformationAssertion
   {
     public:
          enum TransformationOption
             {
            // WhereStatementTransformation     = 0x00000001,
               UnknownTransformationOption      =  0,
               NoWhereStatementTransformation   =  1,
               WhereStatementTransformation     =  2,
               NoIndirectAddressing             =  3,
               IndirectAddressing               =  4,
               ConstantStrideAccess             =  5,
               StrideOneAccess                  =  6,
               VariableStrideAccess             =  7,
               SameSizeArrays                   =  8,
               DifferentSizeArrays              =  9,
               SameParallelDistributions        = 10,
               DifferentParallelDistributions   = 11,
               SameIndexObjectUsedInAllOperands = 12,
               LastTransformationOptionTag      = 13
             };


       // Provide a way to specify more than one option at a type (option values must be specified explicitly)
#if 1
          TransformationAssertion ( TransformationOption a );
          TransformationAssertion ( TransformationOption a, TransformationOption b );
          TransformationAssertion ( TransformationOption a, TransformationOption b, TransformationOption c );
          TransformationAssertion ( TransformationOption a, TransformationOption b, 
                                    TransformationOption c, TransformationOption d );
          TransformationAssertion ( TransformationOption a, TransformationOption b, 
                                    TransformationOption c, TransformationOption d, TransformationOption e );
          TransformationAssertion ( TransformationOption a, TransformationOption b, 
                                    TransformationOption c, TransformationOption d, 
                                    TransformationOption e, TransformationOption f );
#else
       // Or put them all into a single functions and use default initializers so that only a single
       // constructor need be defined
          TransformationAssertion ( TransformationOption a,
                                    TransformationOption b = UnknownTransformationOption, 
                                    TransformationOption c = UnknownTransformationOption,
                                    TransformationOption d = UnknownTransformationOption, 
                                    TransformationOption e = UnknownTransformationOption ,
                                    TransformationOption f = UnknownTransformationOption );
#endif

       // If specified before an array statement this function call communicates to the preprocessor
       // that the transformaation can be made specific to the dimension specified as an integer
       // parameter (parameter should be an integer value (not a variable reference)).
       // void specifyArrayDimension( const baseArray & X, int dimension );

#if 0
       // Example member functions to call
       // The invocation of member functions can also form a mechanism to communicate with the preprocessor
          void nonAliased (intArray A, intArray B);
          void nonAliased (intArray A, intArray B, intArray C);
#endif

       // static char* getOptionString ( TransformationOption i );
          static char* getOptionString ( int i );
   };

// endif for TRANSFORMATION_OPTIONS_H
#endif























