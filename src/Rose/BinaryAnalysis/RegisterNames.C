#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/RegisterNames.h>

#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {

RegisterNames::RegisterNames() {}

RegisterNames::RegisterNames(const RegisterDictionary::Ptr &regdict)
    : dictionary(regdict) {}

RegisterNames&
RegisterNames::operator=(const RegisterDictionary::Ptr &regdict) {
    dictionary = regdict;
    return *this;
}

std::string
RegisterNames::operator()(RegisterDescriptor reg) const {
    return (*this)(reg, dictionary);
}

std::string
RegisterNames::operator()(RegisterDescriptor reg, const RegisterDictionary::Ptr &regdict_) const {
    if (reg.isEmpty())
        return prefix + (prefix == "" ? "" : "_") + "NONE";

    if (RegisterDictionary::Ptr regdict = regdict_ ? regdict_ : dictionary) {
        std::string name = regdict->lookup(reg);
        if (!name.empty())
            return name;
    }

    std::ostringstream ss;
    ss <<prefix <<reg.majorNumber() <<"." <<reg.minorNumber();
    if (showOffset > 0 || (showOffset < 0 && reg.offset() != 0))
        ss <<offsetPrefix <<reg.offset() <<offsetSuffix;
    if (showSize > 0 || (showSize < 0 && reg.offset() != 0))
        ss <<sizePrefix <<reg.nBits() <<sizeSuffix;
    ss <<suffix;
    return ss.str();
}

} // namespace
} // namespace

#endif
