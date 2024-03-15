#ifndef ROSE_BasicTypes_H
#define ROSE_BasicTypes_H
#include <RoseFirst.h>

#include <Sawyer/SharedPointer.h>

// Recursively includes all basic types. These are mostly forward declarations. Try to keep other things to a bare minimum.
// #include's for subdirectories are at the *end* of this header file.

namespace Rose {

class Exception;

class Location;

class Progress;
using ProgressPtr = Sawyer::SharedPointer<Progress>;

class SourceLocation;

} // namespace

// #include's for subdirectories
#include <Rose/BinaryAnalysis/BasicTypes.h>

#endif
