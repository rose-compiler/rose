#ifndef ROSE_BinaryAnalysis_Unparser_Powerpc_H
#define ROSE_BinaryAnalysis_Unparser_Powerpc_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

/** %Settings specific to the PowerPC unparser. */
struct PowerpcSettings: public Settings {};

/** %Unparser for PowerPC instruction sets. */
class Powerpc: public Base {
    PowerpcSettings settings_;

protected:
    explicit Powerpc(const Architecture::BaseConstPtr&, const PowerpcSettings&);

public:
    ~Powerpc();

public:
    static Ptr instance(const Architecture::BaseConstPtr&, const PowerpcSettings& = PowerpcSettings());

    Ptr copy() const override;

    const PowerpcSettings& settings() const override { return settings_; }
    PowerpcSettings& settings() override { return settings_; }
};

} // namespace
} // namespace
} // namespace

#endif
#endif
