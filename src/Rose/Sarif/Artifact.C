#include <sage3basic.h>
#include <Rose/Sarif/Artifact.h>

#include <Rose/Sarif/Exception.h>

namespace Rose {
namespace Sarif {

Artifact::~Artifact() {}

Artifact::Artifact(const std::string &uri)
    : uri_(uri) {}

Artifact::Ptr
Artifact::instance(const std::string &uri) {
    return Ptr(new Artifact(uri));
}

const std::string&
Artifact::uri() const {
    return uri_;
}

const std::string&
Artifact::mimeType() const {
    return mimeType_;
}

void
Artifact::mimeType(const std::string &s) {
    if (s == mimeType_)
        return;
    if (isFrozen())
        throw IncrementalError::frozenObject("Artifact");
    if (isIncremental()) {
        if (!mimeType_.empty())
            throw IncrementalError::cannotChangeValue("Artifact::mimeType");
    }

    mimeType_ = s;

    if (isIncremental())
        emitMimeType(incrementalStream(), emissionPrefix());
}

const std::string&
Artifact::sourceLanguage() const {
    return sourceLanguage_;
}

void
Artifact::sourceLanguage(const std::string &s) {
    if (s == sourceLanguage_)
        return;
    if (isFrozen())
        throw IncrementalError::frozenObject("Artifact");
    if (isIncremental()) {
        if (!sourceLanguage_.empty())
            throw IncrementalError::cannotChangeValue("Artifact::sourceLanguage");
    }

    sourceLanguage_ = s;

    if (isIncremental())
        emitSourceLanguage(incrementalStream(), emissionPrefix());
}

const std::pair<std::string, std::string>&
Artifact::hash() const {
    return hash_;
}

void
Artifact::hash(const std::string &name, const std::string &value) {
    if (name == hash_.first && value == hash_.second)
        return;
    if (isFrozen())
        throw IncrementalError::frozenObject("Artifact");
    if (isIncremental()) {
        if (!hash_.first.empty() || !hash_.second.empty())
            throw IncrementalError::cannotChangeValue("Artifact::hash");
    }

    hash_ = std::make_pair(name, value);

    if (isIncremental())
        emitHash(incrementalStream(), emissionPrefix());
}

void
Artifact::hash(const std::pair<std::string, std::string> &h) {
    hash(h.first, h.second);
}

const std::string&
Artifact::description() const {
    return description_;
}

void
Artifact::description(const std::string &s) {
    if (s == description_)
        return;
    if (isFrozen())
        throw IncrementalError::frozenObject("Artifact");
    if (isIncremental()) {
        if (!description_.empty())
            throw IncrementalError::cannotChangeValue("Artifact::description");
    }

    description_ = s;

    if (isIncremental())
        emitDescription(incrementalStream(), emissionPrefix());
}

void
Artifact::emitUri(std::ostream &out, const std::string &firstPrefix) {
    if (!uri_.empty()) {
        const std::string pp = makeObjectPrefix(firstPrefix);
        out <<firstPrefix <<"location:\n"
            <<pp <<"uri: " <<StringUtility::yamlEscape(uri_) <<"\n";
    }
}

void
Artifact::emitMimeType(std::ostream &out, const std::string &firstPrefix) {
    if (!mimeType_.empty())
        out <<firstPrefix <<"mimeType: " <<StringUtility::yamlEscape(mimeType_) <<"\n";
}

void
Artifact::emitSourceLanguage(std::ostream &out, const std::string &firstPrefix) {
    if (!sourceLanguage_.empty())
        out <<firstPrefix <<"sourceLanguage: " <<StringUtility::yamlEscape(sourceLanguage_) <<"\n";
}

void
Artifact::emitHash(std::ostream &out, const std::string &firstPrefix) {
    if (!hash_.first.empty() || !hash_.second.empty()) {
        const std::string pp = makeObjectPrefix(firstPrefix);
        out <<firstPrefix <<"hashes:\n"
            <<pp <<StringUtility::yamlEscape(hash_.first) <<": " <<StringUtility::yamlEscape(hash_.second) <<"\n";
    }
}

void
Artifact::emitDescription(std::ostream &out, const std::string &firstPrefix) {
    if (!description_.empty()) {
        const std::string pp = makeObjectPrefix(firstPrefix);
        const std::string ppp = makeObjectPrefix(pp);
        out <<firstPrefix <<"description:\n"
            <<pp <<"message:\n"
            <<ppp <<"text: " <<StringUtility::yamlEscape(description_) <<"\n";
    }
}

void
Artifact::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    emitUri(out, firstPrefix);
    const std::string p = makeNextPrefix(firstPrefix);
    emitMimeType(out, p);
    emitSourceLanguage(out, p);
    emitHash(out, p);
    emitDescription(out, p);
}

std::string
Artifact::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

} // namespace
} // namespace
