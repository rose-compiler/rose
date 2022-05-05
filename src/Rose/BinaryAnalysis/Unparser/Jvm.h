#ifndef ROSE_BinaryAnalysis_Unparser_Jvm_H
#define ROSE_BinaryAnalysis_Unparser_Jvm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

struct JvmSettings: public Settings {};

/** Unparser for the JVM architecture. */
class Jvm: public Base {
    JvmSettings settings_;

protected:
    explicit Jvm(const JvmSettings &settings)
        : settings_(settings) {}

public:
    ~Jvm() {}

    static Ptr instance(const JvmSettings &settings = JvmSettings()) {
        return Ptr(new Jvm(settings));
    }

    Ptr copy() const override {
        return instance(settings());
    }

    const JvmSettings& settings() const override { return settings_; }
    JvmSettings& settings() override { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
