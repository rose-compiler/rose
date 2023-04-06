#include <Rose/Tree/Exception.h>
#include <Rose/Tree/Base.h>

namespace Rose {
namespace Tree {

Exception::~Exception() {}

Exception::Exception(const std::string &mesg, const Base::Ptr &vertex)
    : Rose::Exception(mesg), vertex(vertex) {}

InsertionError::~InsertionError() {}

InsertionError::InsertionError(const Base::Ptr &vertex)
    : Exception("vertex is already attached to a tree", vertex) {}

CycleError::~CycleError() {}

CycleError::CycleError(const Base::Ptr &vertex)
    : Exception("insertion of vertex would cause a cycle in the tree", vertex) {}

} // namespace
} // namespace
