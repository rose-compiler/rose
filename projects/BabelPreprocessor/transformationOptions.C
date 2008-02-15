
// We need this header file so that we can use the mechanisms within ROSE to build a preprocessor
#include "rose.h"

#include "transformationOptions.h"

// This variable maps the enum values to strings that describe each enum value
// This is helpful in debugging transformations.
struct
   {
     TransformationAssertion::TransformationOption variant;
     char *name;
   } transformationOptionNames[14] = {
          {TransformationAssertion::UnknownTransformationOption,"UnknownTransformationOption"},
          {TransformationAssertion::LastTransformationOptionTag,"LastTransformationOptionTag"}
        };

char*
// TransformationAssertion::getOptionString ( TransformationOption i )
TransformationAssertion::getOptionString ( int i )
   {
     return transformationOptionNames[i].name;
   }



