#ifndef ROSE_Sarif_H
#define ROSE_Sarif_H

#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/BasicTypes.h>
#include <Rose/Sarif/Exception.h>
#include <Rose/Sarif/Location.h>
#include <Rose/Sarif/Log.h>
#include <Rose/Sarif/Node.h>
#include <Rose/Sarif/Result.h>

namespace Rose {

/** Static analysis results interchange format.
 *
 *  This namespace is a simplified ROSE API for the [SARIF](https://sarifweb.azurewebsites.net/) format.
 *
 *  The main class is @ref Rose::Sarif::Log, a log of the results from running some analyses.  A SARIF log contains a description of
 *  the tool being run, the rules that correspond to the analyses, and the results of the rules' application. A result associates a
 *  rule with a physical location within an address space such as the lines of a source file, an address in memory, an offset in a
 *  processor's stack, or a hardware register on a processor.
 *
 *  The SARIF specification requires that the data is formatted as a JSON document with some extra restrictions (such as the
 *  "version" property appearing before all others). This ROSE implementation relaxes this constraint and represents the SARIF data
 *  separately from its on-disk representation. This implementation uses YAML as its preferred file representation, which has a
 *  number of advantages over JSON:
 *
 *  @li YAML doesn't require list termination, which means lists can be output incrementally. Therefore, a long-running analysis
 *  can produce usable intermediate results and the intermediate versions of the output file are syntactically correct files.
 *
 *  @li Multi-line YAML files are more human friendly since they can contain comments and have less obtrusive quoting requirements.
 *
 *  In the interest of incremental output, this API has two modes of operation. Initially, the intermediate representation (IR) of
 *  the SARIF data is built up as a tree structure with the assumption that it will be emitted to a file at a later time. At any
 *  time, the tree can be marked as @ref Sarif::Log::incremental "incremental" in which case the current structure of the tree is
 *  emitted and any subsequent additions to the tree result in immediate output. Obviously there are some restrictions with
 *  immediate output: it's not possible to go back in time and change parts of the output which might have already been emitted and
 *  consumed by another process, so the IR must be modified only in ways that can be represented by emitting additional information,
 *  not changing what was previously emitted. */
namespace Sarif {}

} // namespace
#endif
