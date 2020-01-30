#include <rose.h>
#include <BinaryConcolic.h>

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
