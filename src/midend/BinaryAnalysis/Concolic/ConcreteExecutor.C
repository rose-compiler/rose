#include <sage3basic.h>
#include <Concolic/ConcreteExecutor.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Concolic/Database.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

ConcreteExecutor::ConcreteExecutor(const Database::Ptr &db)
    : db_(db) {
    ASSERT_not_null(db);
}

ConcreteExecutor::~ConcreteExecutor() {}

Database::Ptr
ConcreteExecutor::database() const {
    return db_;
}

} // namespace
} // namespace
} // namespace
#endif
