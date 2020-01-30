#include <rose.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/process/search_path.hpp>

#ifndef DB_URL
#define DB_URL "sqlite://testCombined.db"
#endif

using namespace Rose::BinaryAnalysis::Concolic;

int main() {
    auto db = Database::create(DB_URL, "ls");
    auto ts1 = db->testSuite();

    // Add some test cases for "ls"
    auto ls = Specimen::instance(boost::process::search_path("ls"));

    auto t01 = TestCase::instance(ls);
    t01->args(std::vector<std::string>{"-la", "/"});
    db->save(t01);

    auto t02 = TestCase::instance(ls);
    t02->args(std::vector<std::string>{"-l", "--author", "/"});
    db->save(t02);

    // Start a new test suite and add test cases for "tail"
    db->testSuite(TestSuite::instance("tail"));

    auto tail = Specimen::instance(boost::process::search_path("tail"));

    auto t03 = TestCase::instance(tail);
    t03->args(std::vector<std::string>{"Makefile"});
    db->save(t03);

    auto t04 = TestCase::instance(tail);
    t04->args(std::vector<std::string>{"-10", "Makefile"});
    db->save(t04);

    // Switch back to the "ls" suite and run
    db->testSuite(ts1);
    for (auto testCaseId: db->needConcreteTesting(1)) {
        auto testCase = db->object(testCaseId);
        auto executor = LinuxExecutor::instance();
        auto result = executor->execute(testCase);      // leaked?
        int status = result->exitStatus();
        ASSERT_always_require(WIFEXITED(status));
        db->insertConcreteResults(testCase, *result);
    }
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
