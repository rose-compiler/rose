#ifndef ROSE_BinaryAnalysis_UnparserNull_H
#define ROSE_BinaryAnalysis_UnparserNull_H
#include <featureTests.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BinaryUnparserBase.h>

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

    Ptr copy() const ROSE_OVERRIDE {
        return instance(settings());
    }

    const NullSettings& settings() const ROSE_OVERRIDE { return settings_; }
    NullSettings& settings() ROSE_OVERRIDE { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
