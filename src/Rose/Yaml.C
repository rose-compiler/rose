#include <Rose/Yaml.h>

#include <Sawyer/Assert.h>
#include <boost/filesystem.hpp>
#include <nlohmann/json.h>
#include <fstream>

namespace Rose {
namespace Yaml {

using json = nlohmann::json;

static Node
toYaml(const json &j) {
    Yaml::Node y;
    if (j.is_null()) {
        /*void*/
    } else if (j.is_object()) {
        for (auto &item: j.items())
            y[item.key()] = toYaml(item.value());
    } else if (j.is_array()) {
        for (size_t i = 0; i < j.size(); ++i)
            y.pushBack() = toYaml(j[i]);
    } else if (j.is_boolean()) {
        y = j.get<bool>() ? "true" : "false";
    } else if (j.is_string()) {
        y = j.get<std::string>();
    } else if (j.is_number_unsigned()) {
        y = j.get<uint64_t>();
    } else if (j.is_number_float()) {
        y = j.get<double>();
    } else {
        ASSERT_require(j.is_number());
        y = j.get<int64_t>();
    }
    return y;
}

Node
parseJson(const boost::filesystem::path &fileName) {
    std::ifstream f(fileName.c_str());
    return toYaml(json::parse(f));
}

Node
parseJson(const std::string &content) {
    return toYaml(json::parse(content));
}

} // namespace
} // namespace
