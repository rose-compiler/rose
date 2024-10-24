#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <Rose/BinaryAnalysis/Unparser/Aarch64.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>

#include <SgAsmExpression.h>
#include <SgAsmVectorType.h>
#include <Cxx_GrammarDowncast.h>

#include <boost/lexical_cast.hpp>
#include <regex>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

Aarch64::~Aarch64() {}

Aarch64::Aarch64(const Architecture::Base::ConstPtr &arch, const Aarch64Settings &settings)
    : Base(arch), settings_(settings) {}

Aarch64::Ptr
Aarch64::instance(const Architecture::Base::ConstPtr &arch, const Aarch64Settings &settings) {
    return Ptr(new Aarch64(arch, settings));
}

Base::Ptr
Aarch64::copy() const {
    return instance(architecture(), settings());
}

void
Aarch64::emitRegister(std::ostream &out, const RegisterDescriptor reg, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitRegister(out, reg, state);
    } else {
        std::string name = state.registerNames()(reg);
        if (SgAsmVectorType *vector = isSgAsmVectorType(notnull(state.currentExpression())->get_type())) {
            ASSERT_not_null(vector->get_elmtType());

            // The qN and vN are different names for the same registers and appear in ARM assembly listings depending on whether
            // their value is being interpreted as a vector. ROSE will try to choose the same name as what is customary in the ARM
            // assembly listings even though this might be confusing when the same physical register appears in two or more
            // instructions and is interpretted different ways.
            std::regex re("q[0-9]+");
            if (std::regex_match(name, re))
                name[0] = 'v';

            name += "." + boost::lexical_cast<std::string>(vector->get_nElmts());
            switch (vector->get_elmtType()->get_nBits()) {
                case 8:
                    name += "b";
                    break;
                case 16:
                    name += "h";
                    break;
                case 32:
                    name += "s";
                    break;
                case 64:
                    name += "d";
                    break;
                case 128:
                    name += "v";
                    break;
                default:
                    ASSERT_not_reachable("invalid vector element size: " +
                                         boost::lexical_cast<std::string>(vector->get_elmtType()->get_nBits()) + " bits");
            }
        }

        out <<name;

        if (settings().insn.operands.showingWidth)
            out <<"[" <<reg.nBits() <<"]";
    }
}

} // namespace
} // namespace
} // namespace

#endif
