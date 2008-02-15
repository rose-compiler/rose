
#include "transformationOptions.h"

// This variable maps the enum values to strings that describe each enum value
// This is helpful in debugging transformations.
struct
   {
     TransformationAssertion::TransformationOption variant;
     char *name;
   } transformationOptionNames[14] = {
          {TransformationAssertion::UnknownTransformationOption,"UnknownTransformationOption"},
          {TransformationAssertion::NoWhereStatementTransformation,"NoWhereStatementTransformation"},
          {TransformationAssertion::WhereStatementTransformation,"WhereStatementTransformation"},
          {TransformationAssertion::NoIndirectAddressing,"NoIndirectAddressing"},
          {TransformationAssertion::IndirectAddressing,"IndirectAddressing"},
          {TransformationAssertion::ConstantStrideAccess,"ConstantStrideAccess"},
          {TransformationAssertion::StrideOneAccess,"StrideOneAccess"},
          {TransformationAssertion::VariableStrideAccess,"VariableStrideAccess"},
          {TransformationAssertion::SameSizeArrays,"SameSizeArrays"},
          {TransformationAssertion::DifferentSizeArrays,"DifferentSizeArrays"},
          {TransformationAssertion::SameParallelDistributions,"SameParallelDistributions"},
          {TransformationAssertion::DifferentParallelDistributions,"DifferentParallelDistributions"},
          {TransformationAssertion::SameIndexObjectUsedInAllOperands,"SameIndexObjectUsedInAllOperands"},
          {TransformationAssertion::LastTransformationOptionTag,"LastTransformationOptionTag"}
        };

char*
// TransformationAssertion::getOptionString ( TransformationOption i )
TransformationAssertion::getOptionString ( int i )
   {
     return transformationOptionNames[i].name;
   }



