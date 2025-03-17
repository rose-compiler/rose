#ifndef ROSE_BinaryAnalysis_CallingConvention_H
#define ROSE_BinaryAnalysis_CallingConvention_H

#include <Rose/BinaryAnalysis/CallingConvention/Analysis.h>
#include <Rose/BinaryAnalysis/CallingConvention/BasicTypes.h>
#include <Rose/BinaryAnalysis/CallingConvention/Declaration.h>
#include <Rose/BinaryAnalysis/CallingConvention/Definition.h>
#include <Rose/BinaryAnalysis/CallingConvention/Exception.h>
#include <Rose/BinaryAnalysis/CallingConvention/StoragePool.h>

namespace Rose {
namespace BinaryAnalysis {

/** Support for binary calling conventions.
 *
 *  This namespace has three main classes and a number of auxiliary classes. The main classes are:
 *
 *  @li @ref Definition describes a particular calling convention based on external documentation of that calling convention.
 *
 *  @li @ref Dictionary is a collection of calling convention definitions.
 *
 *  @li @ref Analysis performs an analysis and can determine which definitions best match the characteristics of some
 *      specified function.
 *
 *  @li @ref Declaration takes a calling convention definition and a C-like function prototype and outputs locations where
 *      each argument would be stored. */
namespace CallingConvention {
}

} // namespace
} // namespace

#endif
