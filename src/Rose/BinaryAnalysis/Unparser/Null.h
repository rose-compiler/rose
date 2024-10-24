#ifndef ROSE_BinaryAnalysis_Unparser_Null_H
#define ROSE_BinaryAnalysis_Unparser_Null_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

/** %Settings specific to the null unparser. */
struct NullSettings: public Settings {};

/** %Unparser for null architecture. */
class Null: public Base {
    NullSettings settings_;

protected:
    explicit Null(const Architecture::BaseConstPtr&, const NullSettings&);

public:
    ~Null();

    static Ptr instance(const Architecture::BaseConstPtr&, const NullSettings& = NullSettings());

    Ptr copy() const override;

    const NullSettings& settings() const override { return settings_; }
    NullSettings& settings() override { return settings_; }
};

} // namespace
} // namespace
} // namespace

#endif
#endif
