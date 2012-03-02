// A few functions to take care of annoying file naming and creation issues.

#ifndef H_SATIRE_FILE_UTILS
#define H_SATIRE_FILE_UTILS

#include <vector>
#include <string>
#include <fstream>

namespace SATIrE {

// Split a path name: "a//b/c" -> ["a", "b", "c"]
// special case: leading / is kept as separate component
std::vector<std::string>
pathNameComponents(std::string pathName);

// Construct path name from components: ["a", "b", "c"] -> "a/b/c"
std::string
pathName(const std::vector<std::string> &components);

// Prefix a file name: "a/b", "prx" -> "a/prx_b"; as a special case,
// prefixing with the empty string does nothing.
std::string
prefixedFileName(std::string fileName, std::string prefix);

// Create a possibly nested directory.
void
createDirectoryHierarchy(const std::vector<std::string> &components);

// Open a file for writing; if the file name refers to a subdirectory that
// does not exist, this creates all required directories (true on success).
bool
openFileForWriting(std::ofstream &file, std::string fileName);

} // namespace SATIrE

#endif
