#ifndef ROSE_BinaryAnalysis_Unparser_Jvm_H
#define ROSE_BinaryAnalysis_Unparser_Jvm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

std::string unparseJvmMnemonic(SgAsmJvmInstruction*);
std::string unparseJvmExpression(SgAsmExpression*, const LabelMap*);

/** %Settings specific to the JVM unparser. */
struct JvmSettings: public Settings {};

/** %Unparser for the JVM architecture. */
class Jvm: public Base {
    JvmSettings settings_;

protected:
    explicit Jvm(const Architecture::BaseConstPtr&, const JvmSettings&);

public:
    ~Jvm();

    static Ptr instance(const Architecture::BaseConstPtr&, const JvmSettings& = JvmSettings());

    Ptr copy() const override;

    const JvmSettings& settings() const override { return settings_; }
    JvmSettings& settings() override { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const override;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const override;

private:
    void outputExpr(std::ostream&, SgAsmExpression*, State&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
