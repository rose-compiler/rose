#ifndef ROSE_CommandLine_Version_H
#define ROSE_CommandLine_Version_H
#include <RoseFirst.h>

#include <rosedll.h>

#include <string>

namespace Rose {
namespace CommandLine {

/** Global location for version string.
 *
 *  This is the string that's printed by the --version switch (usually). It defaults to the ROSE library version number, but
 *  can be overridden by tools. When overriding, the tool should change this version string before constructing the
 *  command-line parser.
 *
 *  See also, @ref genericSwitches. */
ROSE_DLL_API extern std::string versionString;          // intentionally non-const so tools can change it

} // namespace
} // namespace

#endif
