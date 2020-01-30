#include <rose.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/process/search_path.hpp>

#ifndef DB_URL
#define DB_URL "sqlite://testExport.db"
#endif

using namespace Rose::BinaryAnalysis::Concolic;

int main() {
    auto db = Database::create(DB_URL, "exp");
    auto exp = db->testSuite();
    auto specimen = Specimen::instance("./sampleExecutable");

    {
        auto e01 = TestCase::instance(specimen);
        e01->env(std::vector<EnvValue>{EnvValue{"E0", "Hello"}});
        e01->args(std::vector<std::string>{"--env=E0=Hello"});
        db->save(e01);
    }

    {
        auto e02 = TestCase::instance(specimen);
        e02->env(std::vector<EnvValue>{EnvValue{"E0", "Hello"}, EnvValue{"A1", "Test"}});
        e02->args(std::vector<std::string>{"--env=E0=Hello"});
        db->save(e02);
    }

    {
        auto e03 = TestCase::instance(specimen);
        e03->env(std::vector<EnvValue>{EnvValue{"E0", "Howdy"}, EnvValue{"A1", "Test"}});
        e03->args(std::vector<std::string>{"--env=A1=Test", "--env=E0=Howdy"});
        db->save(e03);
    }

    {
        auto e04 = TestCase::instance(specimen);
        e04->env(std::vector<EnvValue>{EnvValue{"E0", "Howdy"}, EnvValue{"A1", "Test"}});
        e04->args(std::vector<std::string>{"--env=E0=Howdy", "--env=A1=Test"});
        db->save(e04);
    }

    // Switch to a different test suite
    auto ts = TestSuite::instance("exp2");
    db->testSuite(ts);

    {
        auto f01 = TestCase::instance(specimen);
        f01->env(std::vector<EnvValue>{EnvValue{"E0", "Hello"}});
        f01->args(std::vector<std::string>{"--env=E0=Howdy"});
        db->save(f01);
    }

    {
        auto f02 = TestCase::instance(specimen);
        f02->args(std::vector<std::string>{"--env=E0=Howdy"});
        db->save(f02);
    }

    // Run the "exp2" test suite concretely, which should all exit with non-zero value
    for (auto testCaseId: db->needConcreteTesting(10)) {
        auto testCase = db->object(testCaseId);
        auto executor = LinuxExecutor::instance();
        auto result = executor->execute(testCase);      // leaked?
        int status = result->exitStatus();
        ASSERT_always_require(WIFEXITED(status) && WEXITSTATUS(status) != 0);
        db->insertConcreteResults(testCase, *result);
    }

    // Run the "exp" test suite concretely, which should all exit with zero
    db->testSuite(exp);
    for (auto testCaseId: db->needConcreteTesting(10)) {
        auto testCase = db->object(testCaseId);
        auto executor = LinuxExecutor::instance();
        auto result = executor->execute(testCase);      // leaked?
        int status = result->exitStatus();
        ASSERT_always_require(WIFEXITED(status) && WEXITSTATUS(status) == 0);
        db->insertConcreteResults(testCase, *result);
    }
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
