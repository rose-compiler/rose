#include <rose.h>
#include <Rose/BinaryAnalysis/Concolic.h>
#if defined(ROSE_ENABLE_CONCOLIC_TESTING) && defined(ROSE_HAVE_SQLITE3)

#include <boost/process/search_path.hpp>

#ifndef DB_URL
#define DB_URL "sqlite://testConnect.db"
#endif

using namespace Rose::BinaryAnalysis::Concolic;

int main() {
    {
        auto db = Database::create(DB_URL, "ls-family");
        auto ls = Specimen::instance(boost::process::search_path("ls"));
        auto l01 = TestCase::instance(ls);
        db->save(l01);
    }

    // Test passes if there's only a single test suite named "ls-family" and it's current
    auto db = Database::instance(DB_URL);
    ASSERT_always_not_null(db);
    auto testSuite = db->testSuite();
    ASSERT_always_not_null(testSuite);

    for (auto testSuiteId: db->testSuites())
        ASSERT_always_require(db->testSuite() == db->object(testSuiteId));
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
