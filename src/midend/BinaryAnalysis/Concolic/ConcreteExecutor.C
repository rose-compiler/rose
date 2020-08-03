#include <sage3basic.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

ConcreteExecutor::ConcreteExecutor(const Database::Ptr &db)
    : db_(db) {
    ASSERT_not_null(db);
}

Database::Ptr
ConcreteExecutor::database() const {
    return db_;
}

} // namespace
} // namespace
} // namespace
#endif
