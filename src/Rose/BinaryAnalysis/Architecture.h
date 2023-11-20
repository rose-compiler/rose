#ifndef ROSE_BinaryAnalysis_Architecture_H
#define ROSE_BinaryAnalysis_Architecture_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Architecture/Amd64.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch32.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch64.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/Architecture/Exception.h>
#include <Rose/BinaryAnalysis/Architecture/Intel80286.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8086.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8088.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI386.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI486.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumii.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumiii.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium4.h>
#include <Rose/BinaryAnalysis/Architecture/Mips32.h>
#include <Rose/BinaryAnalysis/Architecture/Motorola68040.h>
#include <Rose/BinaryAnalysis/Architecture/NxpColdfire.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc32.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc64.h>

namespace Rose {
namespace BinaryAnalysis {

/** Information about an architecture.
 *
 *  An @p Architecture class holds information that describes a hardware architecture, such as how to decode instructions, what
 *  registers are available, the natural word size and alignment information, memory byte order, stack characteristics, calling
 *  convention definitions, unparsing characteristics, instruction semantics, etc. These are all defining features of the
 *  architecture, not analysis results.
 *
 *  A new architecture definition can be added to ROSE without modifying the ROSE source code. One does this by deriving a new
 *  architecture class and registering an instance of the class with the library by giving it a unique name. */
namespace Architecture {}

} // namespace
} // namespace

#endif
#endif
