#ifndef ROSE_Yaml_H
#define ROSE_Yaml_H

#include <Sawyer/Yaml.h>

namespace Rose {

/** YAML parser.
 *
 *  The YAML parser implementation has been moved to the Sawyer support library since it's useful to tools that do not otherwise
 *  depend on librose. When used in those tools, use Sawyer::Yaml; when used in tools that depend on librose, you may use either
 *  Sawyer::Yaml or Rose::Yaml interchangeably. */
namespace Yaml = Sawyer::Yaml;

} // namespace

#endif
