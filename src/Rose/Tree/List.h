#ifndef ROSE_Tree_List_H
#define ROSE_Tree_List_H
#include <Rose/Tree/Base.h>

namespace Rose {
namespace Tree {

/** Tree vertex that points to an ordered sequence of indexable children.
 *
 *  This vertex acts like an @c std::vector except that inserting and erasing children also adjusts the child's parent pointer. */
template<class T>
class List: public Base {
private:
    using EdgeVector = std::vector<std::unique_ptr<Edge<T>>>;

public:
    /** Shared-ownership pointer to nodes of this type. */
    using Ptr = std::shared_ptr<List>;

    /** Type of child. */
    using Child = T;

    /** Type of child pointer. */
    using ChildPtr = std::shared_ptr<T>;

    // These are for compatibility with std::vector
    using value_type = Edge<T>;                                   /**< Type of values stored in this class. */
    using size_type = typename EdgeVector::size_type;             /**< Size type. */
    using difference_type = typename EdgeVector::difference_type; /**< Distance between elements. */
    using reference = value_type&;                                /**< Reference to value. */
    using const_reference = const value_type&;                    /**< Reference to cons value. */
    using pointer = value_type*;                                  /**< Pointer to value. */
    using const_pointer = const value_type*;                      /**< Pointer to const value. */

private:
    EdgeVector elmts_;

public:
    /** Random access iterator to non-const edges.
     *
     *  Iterators are invalidated in the same situations as for @c std::vector. */
    class iterator {
        friend class List;
        typename EdgeVector::iterator base_;
        iterator() = delete;
        iterator(typename EdgeVector::iterator base)
            : base_(base) {}

    public:
        /** Cause iterator to point to the next edge.
         *
         * @{ */
        iterator& operator++() {
            ++base_;
            return *this;
        }
        iterator operator++(int) {
            auto temp = *this;
            ++base_;
            return temp;
        }
        /** @} */

        /** Cause iterator to point to previous edge.
         *
         * @{ */
        iterator& operator--() {
            --base_;
            return *this;
        }
        iterator operator--(int) {
            auto temp = *this;
            --base_;
            return temp;
        }
        /** @} */

        /** Advance iterator in forward (or backward if negative) direction by @p n edges.
         *
         * @{ */
        iterator& operator+=(difference_type n) {
            base_ += n;
            return *this;
        }
        iterator operator+(difference_type n) const {
            iterator retval = *this;
            retval += n;
            return retval;
        }
        /** @} */

        /** Advance iterator in backward (or forward if negative) direction by @p n edges.
         *
         * @{ */
        iterator& operator-=(difference_type n) {
            base_ -= n;
            return *this;
        }
        iterator operator-(difference_type n) const {
            iterator retval = *this;
            retval -= n;
            return retval;
        }
        /** @} */

        /** Distance between two iterators. */
        difference_type operator-(const iterator &other) const {
            return other.base_ - base_;
        }

        /** Return an edge relative to the current one.
         *
         *  Returns the edge that's @p n edges after (or before if negative) the current edge. */
        Edge<T>& operator[](difference_type n) const {
            ASSERT_not_null(base_[n]);
            return *base_[n];
        }

        /** Return a reference to the current edge. */
        Edge<T>& operator*() {
            ASSERT_not_null(*base_);
            return **base_;
        }

        /** Return a pointer to the current edge. */
        Edge<T>* operator->() {
            ASSERT_not_null(*base_);
            return &**base_;
        }

        /** Make this iterator point to the same element as the @ other iterator. */
        iterator& operator=(const iterator &other) {
            base_ = other.base_;
            return *this;
        }

        /** Compare two iterators.
         *
         * @{ */
        bool operator==(const iterator &other) const {
            return base_ == other.base_;
        }
        bool operator!=(const iterator &other) const {
            return base_ != other.base_;
        }
        bool operator<(const iterator &other) const {
            return base_ < other.base_;
        }
        bool operator<=(const iterator &other) const {
            return base_ <= other.base_;
        }
        bool operator>(const iterator &other) const {
            return base_ > other.base_;
        }
        bool operator>=(const iterator &other) const {
            return base_ >= other.base_;
        }
        /** @} */
    };

protected:
    List() {}

public:
    /** Allocating constructor.
     *
     *  Constructs a new node that has no children. */
    static std::shared_ptr<List> instance() {
        return std::shared_ptr<List>(new List);
    }

    /** Test whether vector is empty.
     *
     *  Returns true if this node contains no child edges, null or otherwise. */
    bool empty() const {
        return elmts_.empty();
    }

    /** Number of child edges.
     *
     *  Returns the number of children edges, null or otherwise. */
    size_t size() const {
        return elmts_.size();
    }

    /** Reserve space so the child edge vector can grow without being reallocated. */
    void reserve(size_t n) {
        elmts_.reserve(n);
    }

    /** Reserved capacity. */
    size_t capacity() const {
        return elmts_.capacity();
    }

    /** Insert a child pointer at the end of this node.
     *
     *  If the new element is non-null, then it must satisfy all the requirements for inserting a node as a child of another
     *  node, and its parent pointer will be adjusted automatically. */
    Edge<T>& push_back(const std::shared_ptr<T>& elmt) {
        elmts_.push_back(std::make_unique<Edge<T>>(*this, elmt));
        return *elmts_.back();
    }

    /** Erase a child edge from the end of this node.
     *
     *  If the edge being erased points to a child, then that child's parent pointer is reset. */
    BasePtr pop_back() {
        ASSERT_forbid(elmts_.empty());
        BasePtr retval = (*elmts_.back())();
        elmts_.pop_back();
        return retval;
    }

    /** Return a reference to the Ith edge.
     *
     * @{ */
    const Edge<T>& operator[](size_t i) const {
        return *elmts_.at(i);
    }
    Edge<T>& operator[](size_t i) {
        return *elmts_.at(i);
    }
    const Edge<T>& at(size_t i) const {
        return *elmts_.at(i);
    }
    Edge<T>& at(size_t i) {
        return *elmts_.at(i);
    }
    /** @} */

    /** Return an iterator pointing to the first edge. */
    iterator begin() {
        return iterator(elmts_.begin());
    }

    /** Return an iterator pointing to one past the last edge. */
    iterator end() {
        return iterator(elmts_.end());
    }

    /** Return a reference to the first edge. */
    Edge<T>& front() {
        ASSERT_forbid(elmts_.empty());
        return *elmts_.front();
    }

    /** Return a reference to the last edge. */
    Edge<T>& back() {
        ASSERT_forbid(elmts_.empty());
        return *elmts_.back();
    }

protected:
    virtual ChildDescriptor findChild(size_t i) const override {
        if (i < elmts_.size()) {
            return ChildDescriptor{i, boost::lexical_cast<std::string>(i), (*elmts_[i])()};
        } else {
            return ChildDescriptor{elmts_.size(), "", nullptr};
        }
    }
};

} // namespace
} // namespace
#endif
