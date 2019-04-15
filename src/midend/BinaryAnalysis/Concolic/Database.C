#include <sage3basic.h>
#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

Database::Ptr
Database::instance(const std::string &url) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

Database::Ptr
Database::create(const std::string &url, const std::string &testSuiteName) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

std::vector<Database::TestSuiteId>
Database::testSuites() {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

TestSuite::Ptr
Database::testSuite() {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

Database::TestSuiteId
Database::testSuite(const TestSuite::Ptr&) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

std::vector<Database::SpecimenId>
Database::specimens() {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

std::vector<Database::TestCaseId>
Database::testCases() {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

TestSuite::Ptr
Database::object(TestSuiteId, Update::Flag update) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

TestCase::Ptr
Database::object(TestCaseId, Update::Flag update) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

Specimen::Ptr
Database::object(SpecimenId, Update::Flag update) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

Database::TestSuiteId
Database::id(const TestSuite::Ptr &testSuite, Update::Flag update) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

Database::TestCaseId
Database::id(const TestCase::Ptr &testCase, Update::Flag update) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

Database::SpecimenId
Database::id(const Specimen::Ptr &specimen, Update::Flag update) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

bool
Database::rbaExists(Database::SpecimenId) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
Database::saveRbaFile(const boost::filesystem::path&, Database::SpecimenId) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
Database::extractRbaFile(const boost::filesystem::path&, Database::SpecimenId) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
Database::eraseRba(Database::SpecimenId) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

} // namespace
} // namespace
} // namespace
