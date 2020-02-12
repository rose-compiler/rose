#include <rose.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/process/search_path.hpp>

#ifndef DB_URL
#define DB_URL "sqlite://testDefineFail.db"
#endif

using namespace Rose::BinaryAnalysis::Concolic;

int main() {
    auto db = Database::create(DB_URL, "ls-family");

    auto ls = Specimen::instance(boost::process::search_path("ls"));

    auto t01 = TestCase::instance(ls);
    t01->args(std::vector<std::string>{"-la"});
    db->save(t01);

    try {
        Specimen::instance("./ls");  // does not exist
        ASSERT_not_reachable("Specimen::instance should have failed");
    } catch (...) {
    }
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
