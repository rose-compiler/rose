#include <rose.h>
#include <BinaryConcolic.h>
#if defined(ROSE_ENABLE_CONCOLIC_TESTING) && defined(ROSE_HAVE_SQLITE3)

#include <boost/process/search_path.hpp>

#ifndef DB_URL
#define DB_URL "sqlite://testRun1.db"
#endif

using namespace Rose::BinaryAnalysis::Concolic;

int main() {
    auto db = Database::create(DB_URL, "ls-family");

    // Add some test cases for "ls"
    auto ls = Specimen::instance(boost::process::search_path("ls"));

    auto t01 = TestCase::instance(ls);
    t01->args(std::vector<std::string>{"-la", "/"});
    db->save(t01);

    auto t02 = TestCase::instance(ls);
    t02->args(std::vector<std::string>{"-l", "--author", "/"});
    db->save(t02);

    // Run all tests
    for (auto testCaseId: db->needConcreteTesting(1)) {
        auto testCase = db->object(testCaseId);
        auto executor = LinuxExecutor::instance(db);
        auto result = executor->execute(testCase);      // leaked?
        int status = result->exitStatus();
        ASSERT_always_require(WIFEXITED(status));
        db->saveConcreteResult(testCase, result);
    }
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
