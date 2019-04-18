#include <sage3basic.h>
#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

TestCase::Ptr
TestCase::instance(const Specimen::Ptr &specimen) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

std::string
TestCase::name() const {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
TestCase::name(const std::string&) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

Specimen::Ptr
TestCase::specimen() const {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
TestCase::specimen(const Specimen::Ptr&) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

} // namespace
} // namespace
} // namespace
