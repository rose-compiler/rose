#include <rose.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/process/search_path.hpp>

#ifndef DB_URL
#define DB_URL "sqlite://testConnectNonExisting.db"
#endif

using namespace Rose::BinaryAnalysis::Concolic;

int main() {
    try {
        auto db = Database::instance(DB_URL);
        ASSERT_not_reachable("should not have been able to open non-existing database");
    } catch (...) {
    }
}

#else

#include <iostream>
int main() {
    std::cerr <<"concolic testing is not enabled\n";
}

#endif
