#ifndef ROSE_Yaml_H
#define ROSE_Yaml_H
#include <RoseFirst.h>
#include <Sawyer/Yaml.h>

namespace Rose {

/** Entities for operating on YAML. */
namespace Yaml {

using Sawyer::Yaml::ConstIterator;
using Sawyer::Yaml::Exception;
using Sawyer::Yaml::InternalException;
using Sawyer::Yaml::Iterator;
using Sawyer::Yaml::Node;
using Sawyer::Yaml::OperationException;
using Sawyer::Yaml::parse;
using Sawyer::Yaml::ParsingException;
using Sawyer::Yaml::serialize;
using Sawyer::Yaml::SerializeConfig;

/**
 * @name JSON to YAML conversion.
 *
 * Parse JSON input using nhlohhman::json and then convert to YAML.
 *
 * @{ */

/**
 *  @param fileName A path to the input JSON.
 * */
Node parseJson(const boost::filesystem::path &fileName);

/**
 *  @param content An input string, treated as JSON.
 * */
Node parseJson(const std::string &content);

/** @} */

/**
 * @name YAML parsing.
 *
 * Parse YAML from various types of inputs. These are basically aliases for Sawyer::Yaml::parse.
 *
 * The @ref parseYaml(const boost::filesystem::path &fileName) overload differs from the other overloads in that it
 * treats the input as a path to the input YAML, while the other overloads all assume the input itself is YAML.
 *
 * @{ */

/**
 * @param fileName A path to the input YAML.
 */
Node parseYaml(const boost::filesystem::path &fileName);

/**
 * @param inputStream An input stream, read as YAML.
 */
Node parseYaml(std::istream &inputStream);

/**
 * @param content An input C++ string, treated as a YAML object.
 */
Node parseYaml(const std::string &content);

/**
 * @param content An input C-style string (null-terminated), treated as a YAML object.
 */
Node parseYaml(const char *content);
/** @} */

} // namespace Yaml
} // namespace Rose

#endif
