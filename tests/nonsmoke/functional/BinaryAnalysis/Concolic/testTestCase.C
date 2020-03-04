#include <rose.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

using namespace Rose::BinaryAnalysis::Concolic;

static TestCaseId
createDatabase(const std::string &dbUrl) {
    Database::Ptr db = Database::create(dbUrl);
    ASSERT_always_not_null(db);

    Specimen::Ptr specimen = Specimen::instance("/bin/cat");
    ASSERT_always_not_null(specimen);
    db->save(specimen);

    TestCase::Ptr testCase = TestCase::instance(specimen);
    ASSERT_always_not_null(testCase);
    testCase->args(std::vector<std::string>(1, "\377\177"));
    return db->id(testCase);
}

static void
checkDatabase(const std::string &dbUrl, const TestCaseId &testCaseId) {
    Database::Ptr db = Database::instance(dbUrl);
    TestCase::Ptr tc1 = db->object(testCaseId);
    ASSERT_always_not_null(tc1);
    std::vector<std::string> args = tc1->args();
    ASSERT_always_require2(args.size() == 1, boost::lexical_cast<std::string>(args.size()));
    ASSERT_always_require2(args[0] == "\377\177", "\"" + Rose::StringUtility::cEscape(args[0]) + "\"");
}

int
main() {
    boost::filesystem::path dbName = "testTestCase.db";
    boost::system::error_code ec;
    boost::filesystem::remove(dbName, ec);
    std::string dbUrl = "sqlite3://" + dbName.string();

    TestCaseId testCaseId = createDatabase(dbUrl);
    checkDatabase(dbUrl, testCaseId);
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
