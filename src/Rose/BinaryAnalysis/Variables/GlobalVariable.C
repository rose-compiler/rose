#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Variables/GlobalVariable.h>

#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

GlobalVariable::GlobalVariable() {}

GlobalVariable::GlobalVariable(Address startingAddress, rose_addr_t maxSizeBytes,
                               const std::vector<InstructionAccess> &definingInstructionVas, const std::string &name)
    : BaseVariable(maxSizeBytes, definingInstructionVas, name), address_(startingAddress) {}

GlobalVariable::~GlobalVariable() {}

Address
GlobalVariable::address() const {
    return address_;
}

void
GlobalVariable::address(Address addr) {
    address_ = addr;
}

const std::string&
GlobalVariable::setDefaultName() {
    ASSERT_not_implemented("[Robb Matzke 2019-12-06]");
}

bool
GlobalVariable::operator==(const GlobalVariable &other) const {
    return address() == other.address() && maxSizeBytes() == other.maxSizeBytes();
}

bool
GlobalVariable::operator!=(const GlobalVariable &other) const {
    return !(*this == other);
}

AddressInterval
GlobalVariable::interval() const {
    return AddressInterval::baseSize(address(), maxSizeBytes());
}

void
GlobalVariable::print(std::ostream &out) const {
    out <<"global-variable";
    if (!name().empty())
        out <<" \"" <<StringUtility::cEscape(name()) <<"\"";
    out <<"(va=" <<StringUtility::addrToString(address()) <<", size=" <<sizeStr(maxSizeBytes()) <<")";
}

std::string
GlobalVariable::toString() const {
    std::ostringstream ss;
    print(ss);
    return ss.str();
}

std::ostream&
operator<<(std::ostream &out, const Rose::BinaryAnalysis::Variables::GlobalVariable &x) {
    x.print(out);
    return out;
}

void
erase(GlobalVariables &gvars, const AddressInterval &where) {
    if (where) {
        ASSERT_forbid(isInconsistent(gvars, mlog[ERROR]));
        gvars.erase(where);

        // If we erased the beginning of a variable, then adjust the variable's starting address and maximum size.
        auto next = gvars.upperBound(where.greatest());
        if (next != gvars.nodes().end()
            && next->key().least() == where.greatest() + 1
            && where.contains(next->value().address())) {
            const uint64_t less = where.greatest() + 1 - next->value().address();
            next->value().address(where.greatest() + 1);
            next->value().maxSizeBytes(next->value().maxSizeBytes() - less);
        }

        // If we erased the middle of a variable, then we should leave only the part that is before `where`.  For now, we'll just
        // erase the following node from the `gvars` map without changing the variable. Next we're going to find the prior entries
        // and adjust the variable's location.
        if (next != gvars.nodes().end()
            && next->key().least() == where.greatest() + 1
            && next->value().address() < where.least()) {
            gvars.erase(next->key());
        }

        // If there's a variable immediately before what we erased and it's size extends into the part we erased, then adjust its
        // size so it doesn't go into the erased area.
        auto prior = gvars.findPrior(where.least());
        if (prior != gvars.nodes().end()
            && prior->key().greatest() + 1 == where.least()
            && prior->value().address() + prior->value().maxSizeBytes() > where.least()) {
            prior->value().maxSizeBytes(where.least() - prior->value().address());
        }
        ASSERT_forbid(isInconsistent(gvars, mlog[ERROR]));
    }
}

AddressInterval
isInconsistent(const GlobalVariables &gvars, Sawyer::Message::Stream &out) {
    for (const auto &node: gvars.nodes()) {
        if (node.key() != AddressInterval::baseSize(node.value().address(), node.value().maxSizeBytes())) {
            if (out) {
                for (const auto &n: gvars.nodes()) {
                    const auto i = AddressInterval::baseSize(node.value().address(), node.value().maxSizeBytes());
                    out <<(n.key() == i ? "    " : "  * ")
                        <<"key = " <<StringUtility::addrToString(n.key())
                        <<" (" <<StringUtility::plural(n.key().size(), "bytes") <<")"
                        <<"; var_location = " <<StringUtility::addrToString(i)
                        <<" (" <<StringUtility::plural(i.size(), "bytes") <<")"
                        <<", variable = " <<n.value() <<"\n";
                }
            }
            return node.key();
        }
    }
    return {};
}

} // namespace
} // namespace
} // namespace

#endif
