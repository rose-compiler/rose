#include <rose.h>
#include <BinaryConcolic.h>
#if defined(ROSE_ENABLE_CONCOLIC_TESTING) && defined(ROSE_HAVE_SQLITE3)

#ifndef DB_URL
#define DB_URL "sqlite://testAddressRandomization.db"
#endif


int main() {
    using namespace Rose::BinaryAnalysis::Concolic;
    auto db = Database::create(DB_URL, "withAslr");
    auto withAslr = db->testSuite();
    auto specimen = Specimen::instance("./sampleExecutable");

    {
        auto e01 = TestCase::instance(specimen);
        e01->args(std::vector<std::string>{"--address-randomization=true"});
        db->save(e01);

        auto executor = LinuxExecutor::instance(db);
        executor->useAddressRandomization(true);
        auto result = executor->execute(e01);
        int status = result->exitStatus();
        ASSERT_always_require(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    }

    {
        auto e02 = TestCase::instance(specimen);
        e02->args(std::vector<std::string>{"--address-randomization=false"});
        db->save(e02);

        auto executor = LinuxExecutor::instance(db);
        executor->useAddressRandomization(false);
        auto result = executor->execute(e02);
        int status = result->exitStatus();
        ASSERT_always_require(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    }
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
