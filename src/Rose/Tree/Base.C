#include <Rose/Tree/Base.h>

#include <limits>

namespace Rose {
namespace Tree {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ReverseEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
ReverseEdgeAccess::resetParent(ReverseEdge &e) {
    e.reset();
}

void
ReverseEdgeAccess::setParent(ReverseEdge &e, Base &parent) {
    e.set(parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ReverseEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ReverseEdge::~ReverseEdge() {
    ASSERT_require(parent_ == nullptr);
}

ReverseEdge::ReverseEdge(Base &child)
    : child_(child) {}

BasePtr
ReverseEdge::operator()() const {
    if (parent_) {
        return parent_->pointer();
    } else {
        return {};
    }
}

BasePtr
ReverseEdge::operator->() const {
    ASSERT_not_null(parent_);
    return parent_->pointer();
}

bool
ReverseEdge::operator==(const BasePtr &ptr) const {
    return ptr.get() == parent_;
}

bool
ReverseEdge::operator!=(const BasePtr &ptr) const {
    return ptr.get() != parent_;
}

bool
ReverseEdge::operator==(const ReverseEdge &other) const {
    return parent_ == other.parent_;
}

bool
ReverseEdge::operator!=(const ReverseEdge &other) const {
    return parent_ != other.parent_;
}

void
ReverseEdge::reset() {
    parent_ = nullptr;
}

void
ReverseEdge::set(Base &parent) {
    parent_ = &parent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Base::Base()
    : parent(*this) {}

BasePtr
Base::pointer() {
    return shared_from_this();
}

std::string
Base::childName(size_t i) {
    return findChild(i).name;
}

Base::Ptr
Base::child(size_t i) {
    return findChild(i).value;
}

size_t
Base::nChildren(size_t i) {
    return findChild(std::numeric_limits<size_t>::max()).i;
}

Base::ChildDescriptor
Base::findChild(size_t i) const {
    return ChildDescriptor{0, "", nullptr};
}

} // namespace
} // namespace
