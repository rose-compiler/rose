#include <rose.h>
#include <BinaryConcolic.h>

#ifndef DB_URL
#define DB_URL "sqlite://testOpenDb.db"
#endif

using namespace Rose::BinaryAnalysis::Concolic;

int main() {
    auto db = Database::create(DB_URL, "ls-family");
    ASSERT_always_not_null(db);
}
