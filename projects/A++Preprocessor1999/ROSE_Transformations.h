// ROSE_Transformations.h -- header file for the separate transformation 
//                           passes with the ROSE Optimizing Preprocessor

#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

// This header file contains the includes for each ROSE transformation object
// each ROSE transformation is defined as an independent C++ class derived from
// ROSE_TransformationBase (the base class for all ROSE transformations)

// Definition of hand-coded array class grammar
#include "array_class_interface.h"

// definition of bese class for all transformations
#include "TransformationBase.h"

// Specific transformations

// Include file for simple array statement transformations
// include "Transform_1/transformation_1.h"
#include "transformation_2.h"
#include "transformation_3.h"

#endif //ifndef TRANSFORMATIONS_H

