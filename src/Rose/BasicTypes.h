#ifndef ROSE_BasicTypes_H
#define ROSE_BasicTypes_H

// Recursively includes all basic types. These are mostly forward declarations. Try to keep other things to a bare minimum.

#include <Rose/BinaryAnalysis/BasicTypes.h>

namespace Rose {

class Exception;

class Location;

class Progress;
using ProgressPtr = Sawyer::SharedPointer<Progress>;

class SourceLocation;

} // namespace
#endif
