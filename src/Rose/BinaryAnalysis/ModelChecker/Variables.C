#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/Variables.h>

#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Variables.h>
#include <Rose/StringUtility/NumberToString.h>

#include <sstream>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

FoundVariable::FoundVariable() {}

FoundVariable::~FoundVariable() {}

FoundVariable::FoundVariable(const Variables::GlobalVariable &gvar)
    : where_(gvar.interval()), gvar_(gvar) {
    ASSERT_require(gvar);
}

FoundVariable::FoundVariable(const AddressInterval &where, const Variables::StackVariable &lvar)
    : where_(where), lvar_(lvar) {
    ASSERT_require(where);
    ASSERT_require(lvar);
}

const AddressInterval&
FoundVariable::where() const {
    return where_;
}

const Variables::StackVariable&
FoundVariable::stackVariable() const {
    return lvar_;
}

const Variables::GlobalVariable&
FoundVariable::globalVariable() const {
    return gvar_;
}

Partitioner2::Function::Ptr
FoundVariable::function() const {
    if (lvar_) {
        return lvar_.function();
    } else {
        return {};
    }
}

bool
FoundVariable::isValid() const {
    return !where_.isEmpty();
}

FoundVariable::operator bool() const {
    return isValid();
}

bool
FoundVariable::operator!() const {
    return where_.isEmpty();
}

void
FoundVariable::print(std::ostream &out) const {
    if (gvar_) {
        out <<gvar_;
    } else if (lvar_) {
        out <<lvar_ <<" at " <<StringUtility::addrToString(where_);
    } else {
        out <<"none";
    }
}

std::string
FoundVariable::toString() const {
    std::ostringstream ss;
    ss <<*this;
    return ss.str();
}

std::ostream&
operator<<(std::ostream &out, const FoundVariable &var) {
    var.print(out);
    return out;
}

} // namespace
} // namespace
} // namespace

#endif
