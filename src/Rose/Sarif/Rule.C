#include <sage3basic.h>
#include <Rose/Sarif/Rule.h>

#include <Rose/Sarif/Exception.h>
#include <Combinatorics.h>                              // rose

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Rose {
namespace Sarif {

Rule::~Rule() {}

Rule::Rule(const std::string &id, const std::string &description)
    : id_(id), description_(description), uuid_(boost::uuids::random_generator()()) {
    if (id.empty()) {
        Combinatorics::HasherSha256Builtin hasher;
        hasher.insert(description);
        id_ = "R" + hasher.toString().substr(0, 8);
    }
}

Rule::Ptr
Rule::instance(const std::string &id, const std::string &description) {
    return Ptr(new Rule(id, description));
}

const std::string&
Rule::id() const {
    return id_;
}

const std::string&
Rule::name() const {
    return name_;
}

void
Rule::name(const std::string &s) {
    if (s == name_)
        return;
    checkPropertyChange("Rule", "name", name_.empty(), {});
    name_ = s;
    if (isIncremental())
        emitName(incrementalStream(), emissionPrefix());
}

const std::string&
Rule::description() const {
    return description_;
}

void
Rule::description(const std::string &s) {
    if (s == description_)
        return;
    checkPropertyChange("Rule", "description", description_.empty(), {});
    description_ = s;
    if (isIncremental())
        emitDescription(incrementalStream(), emissionPrefix());
}

const std::string&
Rule::helpUri() const {
    return helpUri_;
}

void
Rule::helpUri(const std::string &s) {
    if (s == helpUri_)
        return;
    checkPropertyChange("Rule", "helpUri", helpUri_.empty(), {});
    helpUri_ = s;
    if (isIncremental())
        emitHelpUri(incrementalStream(), emissionPrefix());
}

boost::uuids::uuid
Rule::uuid() const {
    return uuid_;
}

void
Rule::uuid(boost::uuids::uuid id) {
    if (id == uuid_)
        return;
    checkPropertyChange("Rule", "uuid", uuid_.is_nil(), {});
    uuid_ = id;
    if (isIncremental())
        emitUuid(incrementalStream(), emissionPrefix());
}

void
Rule::emitId(std::ostream &out, const std::string &firstPrefix) {
    out <<firstPrefix <<"id: " <<StringUtility::yamlEscape(id_) <<"\n";
}

void
Rule::emitUuid(std::ostream &out, const std::string &firstPrefix) {
    out <<firstPrefix <<"guid: " <<uuid_ <<"\n";
}

void
Rule::emitName(std::ostream &out, const std::string &firstPrefix) {
    if (!name_.empty() && name_ != id_)
        out <<firstPrefix <<"name: " <<StringUtility::yamlEscape(name_) <<"\n";
}

void
Rule::emitDescription(std::ostream &out, const std::string &firstPrefix) {
    out <<firstPrefix <<"fullDescription:\n";
    const std::string pp = makeObjectPrefix(firstPrefix);
    out <<pp <<"text: " <<StringUtility::yamlEscape(description_) <<"\n";
}

void
Rule::emitHelpUri(std::ostream &out, const std::string &firstPrefix) {
    out <<firstPrefix <<"helpUri: " <<StringUtility::yamlEscape(helpUri_) <<"\n";
}

void
Rule::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    emitId(out, firstPrefix);
    const std::string p = makeNextPrefix(firstPrefix);
    emitUuid(out, p);

    if (!name_.empty())
        emitName(out, p);

    if (!description_.empty())
        emitDescription(out, p);

    if (!helpUri_.empty())
        emitHelpUri(out, p);
}

std::string
Rule::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

} // namespace
} // namespace
