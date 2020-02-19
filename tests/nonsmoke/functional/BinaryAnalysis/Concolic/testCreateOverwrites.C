#include <rose.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/process/search_path.hpp>

#ifndef DB_URL
#define DB_URL "sqlite://testCreateOverwrites.db"
#endif

using namespace Rose::BinaryAnalysis::Concolic;

int main() {
    {
        auto db = Database::create(DB_URL, "ls-family");
        auto ls = Specimen::instance(boost::process::search_path("ls"));

        auto l01 = TestCase::instance(ls);
        l01->args(std::vector<std::string>{"-la", "/"});
        db->save(l01);

        auto l02 = TestCase::instance(ls);
        l02->args(std::vector<std::string>{"-1", "/"});
        db->save(l02);
    }

    {
        auto db = Database::create(DB_URL, "tail-family");
        auto tail = Specimen::instance(boost::process::search_path("tail"));

        auto t03 = TestCase::instance(tail);
        t03->args(std::vector<std::string>{"/etc/passwd"});
        db->save(t03);
    }

    // Test fails if we open the database and find any test suites besides "tail-family"
    auto db = Database::instance(DB_URL);
    ASSERT_always_not_null(db);
    for (auto testSuiteId: db->testSuites()) {
        auto testSuite = db->object(testSuiteId);
        ASSERT_always_not_null(testSuite);
        ASSERT_always_require2(testSuite->name() == "tail-family", testSuite->name());
    }
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
