#ifndef ROSE_Sarif_H
#define ROSE_Sarif_H

#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Artifact.h>
#include <Rose/Sarif/BasicTypes.h>
#include <Rose/Sarif/Exception.h>
#include <Rose/Sarif/Location.h>
#include <Rose/Sarif/Log.h>
#include <Rose/Sarif/Node.h>
#include <Rose/Sarif/Result.h>
#include <Rose/Sarif/Rule.h>

namespace Rose {

/** Static analysis results interchange format.
 *
 *  This namespace is a simplified ROSE API for the [SARIF](https://sarifweb.azurewebsites.net/) format. ROSE's intermediate
 *  representation (IR) of the major components of a SARIF @ref Rose::Sarif::Log "log" forms a tree. The tree is unparsed to create
 *  the at-rest file representation of the SARIF information.
 *
 *  A SARIF tree is rooted at a @ref Sarif::Log object that contains information about zero or more @ref Sarif::Analysis "analyses",
 *  each of which is run individually or as part of a larger tool. Each analysis object contains a list of @ref Sarif::Rule "rules",
 *  @ref Sarif::Artifact "artifacts", and @ref Sarif::Result "results". The artifacts describe files produced by an analysis and
 *  each result is an individual finding from an analysis.  Each result can refer to a rule and has a list of @ref Sarif::Location
 *  "locations" that refer to parts of artifacts. The SARIF IR uses the Sawyer @ref Sawyer::Tree::Vertex "Tree API" which has
 *  numerous benefits over the older ROSETTA-based IR used by most of the rest of ROSE. Some of these benefits are:
 *
 *  @li No generated code. All SARIF IR classes are plain C++ class defintions.
 *  @li Automatic parent pointers. It is impossible to mess up parent/child relationships.
 *  @li Immediately enforced tree invariants. No need to call a separate tree consistency checker.
 *  @li Reference counted vertices. Use of smart pointers for clear ownership and safe deletion.
 *  @li Simple traversals. Only two traversal functions upon which all others can be implemented.
 *  @li Event driven. Users can register callbacks for tree structure changes.
 *  @li Exception safe. Trees will always be in a consistent state even when exceptions are thrown.
 *  @li Disjoint class hierarchies. The SARIF class hierarchy can be unrelated to hierarchies for other features.
 *  @li Well tested. We strive for 100% code coverage by the Sawyer Tree unit tests.
 *
 *  For example, the binary model checker tool, in a single run, can check for null pointer dereferences, static buffer overflows,
 *  and uninitialized local variables. A single run of the tool itself can be represented as a @ref Sarif::Log that contains three
 *  @ref Sarif::Analysis objects.  Each of those analysis objects contains a single @ref Sarif::Rule, such as "a dereferenced
 *  pointer must not have a value between zero and 1023, inclusive". When a null pointer dereference is detected, a new @ref
 *  Sarif::Result object is attached to the tree as a child of the @ref Sarif::Analysis. The result refers to the aforementioned
 *  rule, the binary artifact that is being analyzed, and a list of locations corresponding to the execution path leading to the
 *  null dereference.  The result might also have @ref Sarif::Artifact objects that describe any additional outputs for the result,
 *  such as an optional memory slice file.
 *
 *  The SARIF IR can be unparsed in two ways: either the whole tree can be unparsed at once to create a SARIF file, or the tree can
 *  be unparsed incrementally as it is modified.  The latter approach works well for tools that run for an undetermined length of
 *  time or tools that run the risk of being killed for using too many resources. The former approach works well for tools that need
 *  to modify the IR extensively before emitting it. Both unparsing mechanisms support building the SARIF tree from top-down or
 *  bottom-up.
 *
 *  A SARIF tree is placed in incremental mode by calling @ref Sarif::Log::incremental and supplying a @c std::ostream as an
 *  argument.  Once incremental mode is activated, the existing tree is emitted and then parts of the tree become read-only. An
 *  incremental tree enforces certain constraints on the order that things can be added to the tree. For instance, because SARIF
 *  represents artifacts and results as disjoint lists, it is not possible to incrementally emit both artifacts and results--the
 *  implementation requires that all artifacts are emitted before any result is emitted. Violating the ordering constraints will
 *  result in a @ref Sarif::IncrementalError being thrown. Furthermore, because every stage of the incremental output must be a
 *  syntactically valid file, this implementation emits SARIF as YAML instead of JSON even though the SARIF standard requires that
 *  the at-rest representation is JSON. Numerous free tools are available to convert YAML to JSON.
 *
 *  Here's an example from the SARIF unit testing that shows how to create a log with a single analysis result refering to a single
 *  artifact and having no results.
 *
 *  @snippet{trimleft} sarifUnitTests.C log_with_artifact
 *
 *  Here's an example from the SARIF unit testing that shows how to create a log with a single analysis that has a single location
 *  refering to an address in virtual memory.
 *
 *  @snippet{trimleft} sarifUnitTests.C log_with_location */
namespace Sarif {}

} // namespace
#endif
