#ifndef ROSE_BinaryAnalysis_Unparser_Null_H
#define ROSE_BinaryAnalysis_Unparser_Null_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

struct NullSettings: public Settings {};

/** Unparser for null architecture. */
class Null: public Base {
    NullSettings settings_;

protected:
    explicit Null(const NullSettings &settings)
        : settings_(settings) {}

public:
    ~Null() {}

    static Ptr instance(const NullSettings &settings = NullSettings()) {
        return Ptr(new Null(settings));
    }

    Ptr copy() const override {
        return instance(settings());
    }

    const NullSettings& settings() const override { return settings_; }
    NullSettings& settings() override { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
