#ifndef ROSE_Yaml_H
#define ROSE_Yaml_H
#include <RoseFirst.h>
#include <Sawyer/Yaml.h>

namespace Rose {

/** Entities for operating on YAML. */
namespace Yaml {

using Sawyer::Yaml::Exception;
using Sawyer::Yaml::InternalException;
using Sawyer::Yaml::ParsingException;
using Sawyer::Yaml::OperationException;
using Sawyer::Yaml::Iterator;
using Sawyer::Yaml::ConstIterator;
using Sawyer::Yaml::Node;
using Sawyer::Yaml::parse;
using Sawyer::Yaml::SerializeConfig;
using Sawyer::Yaml::serialize;

/** Parse JSON to YAML.
 *
 * @{ */
Node parseJson(const boost::filesystem::path&);
Node parseJson(const std::string&);
/** @} */

} // namespace
} // namespace

#endif
