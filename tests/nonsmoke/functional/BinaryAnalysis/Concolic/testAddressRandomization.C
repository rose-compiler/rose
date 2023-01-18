#include <rose.h>
#include <Rose/BinaryAnalysis/Concolic.h>
#if defined(ROSE_ENABLE_CONCOLIC_TESTING) && defined(ROSE_HAVE_SQLITE3)

#ifndef DB_URL
#define DB_URL "sqlite://testAddressRandomization.db"
#endif

#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>

static std::string
howTerminated(const int status) {
    if (WIFEXITED(status)) {
        return "exited with status " + boost::lexical_cast<std::string>(WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        return "terminated by signal (" + boost::to_lower_copy(std::string(strsignal(WTERMSIG(status)))) + ")";
    } else {
        return "";                                      // not terminated yet
    }
}

int main() {
    using namespace Rose::BinaryAnalysis::Concolic;
    auto db = Database::create(DB_URL, "withAslr");
    auto withAslr = db->testSuite();
    auto specimen = Specimen::instance("./sampleExecutable");

    {
        auto e01 = TestCase::instance(specimen);
        e01->args(std::vector<std::string>{"--address-randomization=true"});
        db->save(e01);

        auto executor = I386Linux::ExitStatusExecutor::instance(db);
        executor->useAddressRandomization(true);
        auto result = executor->execute(e01).dynamicCast<I386Linux::ExitStatusResult>();
        int status = result->exitStatus();
        std::cerr <<"specimen " <<howTerminated(status) <<"\n";
        ASSERT_always_require(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    }

    {
        auto e02 = TestCase::instance(specimen);
        e02->args(std::vector<std::string>{"--address-randomization=false"});
        db->save(e02);

        auto executor = I386Linux::ExitStatusExecutor::instance(db);
        executor->useAddressRandomization(false);
        auto result = executor->execute(e02).dynamicCast<I386Linux::ExitStatusResult>();
        int status = result->exitStatus();
        std::cerr <<"specimen " <<howTerminated(status) <<"\n";
        ASSERT_always_require(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    }
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
