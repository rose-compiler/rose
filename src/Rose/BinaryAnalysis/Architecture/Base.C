#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Base::Base() {}

Base::~Base() {}

const std::string&
Base::name() const {
    return name_;
}

void
Base::name(const std::string &s) {
    name_ = s;
}

const RegisterDictionary::Ptr&
Base::registerDictionary() const {
    return registerDictionary_;
}

void
Base::registerDictionary(const RegisterDictionary::Ptr &regdict) {
    registerDictionary_ = regdict;
}


} // namespace
} // namespace
} // namespace

#endif
