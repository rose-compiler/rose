#ifndef TYPETRAITS_H
#define TYPETRAITS_H

// #include "config.h"
#include "rose.h"

//! Is a type default constructible?  This may not quite work properly.
bool isDefaultConstructible(SgType* type);

//! Is a type copy constructible?  This may not quite work properly.
bool isCopyConstructible(SgType* type);

//! Does a type have a trivial (built-in) destructor?
bool hasTrivialDestructor(SgType* t);

//! Is this type a non-constant reference type?
bool isNonconstReference(SgType* t);

//! Is this type a const or non-const reference type?
bool isReferenceType(SgType* t);

//! Is this a const type?
bool isConstType(SgType* t);

#endif // TYPETRAITS_H
