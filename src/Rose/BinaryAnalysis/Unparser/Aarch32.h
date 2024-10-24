#ifndef ROSE_BinaryAnalysis_Unparser_Aarch32_H
#define ROSE_BinaryAnalysis_Unparser_Aarch32_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32

#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

std::string unparseAarch32Mnemonic(SgAsmAarch32Instruction*);
std::string unparseAarch32Expression(SgAsmExpression*, const LabelMap*);

/** %Settings specific to the ARM AArch32 unparser. */
struct Aarch32Settings: public Settings {};

/** ARM Aarch32 A32/T32 disassembler.
 *
 *  This class is able to unparse ARM AArch32 A32/T32 instructions. */
class Aarch32: public Base {
    Aarch32Settings settings_;

protected:
    explicit Aarch32(const Architecture::BaseConstPtr&, const Aarch32Settings&);

public:
    ~Aarch32();

    static Ptr instance(const Architecture::BaseConstPtr&, const Aarch32Settings& = Aarch32Settings());

    Ptr copy() const override;

    const Aarch32Settings& settings() const override { return settings_; }
    Aarch32Settings& settings() override { return settings_; }
};

} // namespace
} // namespace
} // namespace

#endif
#endif
