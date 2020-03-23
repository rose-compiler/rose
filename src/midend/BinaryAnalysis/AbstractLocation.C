#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "AbstractLocation.h"

namespace Rose {
namespace BinaryAnalysis {

bool
AbstractLocation::mayAlias(const AbstractLocation &other, const SmtSolverPtr &solver) const {
    if (isRegister() && other.isRegister()) {
        return reg_ == other.reg_;
    } else if (isAddress() && other.isAddress()) {
        return addr_->may_equal(other.addr_, solver);
    } else if (!isValid() && !other.isValid()) {
        return true;
    } else {
        return false;
    }
}

bool
AbstractLocation::mustAlias(const AbstractLocation &other, const SmtSolverPtr &solver) const {
    if (isRegister() && other.isRegister()) {
        return reg_ == other.reg_;
    } else if (isAddress() && other.isAddress()) {
        return addr_->must_equal(other.addr_, solver);
    } else if (!isValid() && !other.isValid()) {
        return true;
    } else {
        return false;
    }
}

void
AbstractLocation::print(std::ostream &out, const RegisterDictionary *regdict,
                        InstructionSemantics2::BaseSemantics::Formatter &fmt) const {
    if (isRegister()) {
        out <<RegisterNames()(reg_, regdict ? regdict : regdict_);
    } else if (isAddress()) {
        out <<(*addr_+fmt);
        if (nBytes_>0)
            out <<"+" <<StringUtility::plural(nBytes_, "bytes");
    } else {
        out <<"NO_LOCATION";
    }
}

std::ostream&
operator<<(std::ostream &out, const AbstractLocation &aloc)
{
    aloc.print(out);
    return out;
}

} // namespace
} // namespace

#endif
