#ifndef ROSE_BinaryAnalysis_Architecture_H
#define ROSE_BinaryAnalysis_Architecture_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Architecture/Exception.h>

namespace Rose {
namespace BinaryAnalysis {

/** Information about an architecture.
 *
 *  An @p Architecture class holds information that describes a hardware architecture, such as how to decode instructions, what
 *  registers are available, the natural word size and alignment information, memory byte order, stack characteristics, calling
 *  convention definitions, unparsing characteristics, instruction semantics, etc. These are all defining features of the
 *  architecture, not analysis results.
 *
 *  A new architecture definition can be added to ROSE without modifying the RSOE source code. One does this by deriving a new
 *  architecture class and registering an instance of the class with the library by giving it a unique name. */
namespace Architecture {}

} // namespace
} // namespace

#endif
#endif
