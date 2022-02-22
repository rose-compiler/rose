#include <Rose/CommandLine/Version.h>

namespace Rose {
namespace CommandLine {

// Global place to store the string usually printed by the --version command-line switch.  This static variable is initialized
// by the ROSE_INITIALIZE macro called from every ROSE tool's "main" function.
std::string versionString;

} // namespace
} // namespace
