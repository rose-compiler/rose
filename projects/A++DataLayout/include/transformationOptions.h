
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

          static const char* getOptionString ( int i );
   };

// endif for TRANSFORMATION_OPTIONS_H
#endif























