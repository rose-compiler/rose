// This set of includes is refactored to support building multiple DSL compilers 
// (each for a different abstraction).  E.g. call to array.h is factored out.

#include "VariableIdMapping.h"

#include "dsl_attribute_support.h"

// This line can maybe be generic in the future, it is here in the case that
// we want to use the DSL abstractions semantics at comile-time to evaluate
// constant expressions.
// #include "array.h"

// #include "dsl_detection.h"
#include "dslSupport.h"

// #include "generated_dsl_attributes_header.C"
// #include "generated_dsl_attributes.C"

#include "dsl_detection.h"
