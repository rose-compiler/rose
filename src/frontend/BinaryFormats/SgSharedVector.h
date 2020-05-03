#ifndef ROSE_SgSharedVector_H
#define ROSE_SgSharedVector_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#endif

/* An SgSharedVector is like an STL vector except for two things:
 *
 *   1. The underlying storage is managed external to this class
 *   2. Copying (copy ctor, assignment) an SgSharedVector results in two vectors pointing to the same underlying storage.
 *
 * This class is meant to support memory mapped files (see mmap(2)) in such a way so as not to cause the entire file contents
 * to be accessed when a vector is created or copied.  If the mapped memory supplied to the constructor (the "pool") is
 * read-only then modifying the vector will fail with a segmentation fault. */
template <typename _Tp>
class SgSharedVector {
public:
    typedef _Tp         value_type;
    typedef _Tp*        pointer;
    typedef const _Tp*  const_pointer;
    typedef _Tp&        reference;
    typedef const _Tp&  const_reference;
    typedef size_t      size_type;

private:
    pointer p_pool;               // backing store managed externally to this class
    size_type p_capacity;         // number of bytes in pool
    size_type p_size;             // current size of vector (never greater than capacity)

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_NVP(p_size);
        s & boost::serialization::make_nvp("p_pool", boost::serialization::make_array(p_pool, p_size));
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(p_size);
        p_capacity = p_size;
        p_pool = new value_type[p_capacity];
        s & boost::serialization::make_nvp("p_pool", boost::serialization::make_array(p_pool, p_size));
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif
    
public:
    /**** Iterators ****/
    class iterator {
        public:
            iterator operator++() { return (this->p)++; }
            bool operator!=( const iterator & x ) const { return (this->p != x->p); }
            value_type & operator*() const { return *(this->p); }
            iterator(pointer x) : p(x) {}
        private:          
            value_type *p;
    };

    class const_iterator {
        public:
            const_iterator operator++() { return (this->p)++; }
            bool operator!=( const const_iterator & x ) const { return (this->p != x.p); }
            const value_type & operator*() const { return *(this->p); }
            const_iterator(pointer x) : p(x) {}
        private:          
            value_type *p;
    };

    /**** Construct/Copy/Destroy ****/

    // constructs a vector having no data
    explicit SgSharedVector()
        : p_pool(0), p_capacity(0), p_size(0) {}
    // constructs a non-extendible vector of particular size
    explicit SgSharedVector(pointer pool, size_type n)
        : p_pool(pool), p_capacity(n), p_size(n) {}
    // constructs an extendible vector of specified capacity and size
    explicit SgSharedVector(pointer pool, size_type n, size_type nres)
        : p_pool(pool), p_capacity(nres), p_size(0) {
        resize(n);
    }
    // constructs a new vector pointing to same data as initializer vector
    SgSharedVector(const SgSharedVector &x)
        : p_pool(x.pool()), p_capacity(x.capacity()), p_size(x.size()) {}
    // new vector points to offset in initializer vector
    SgSharedVector(const SgSharedVector &x, size_type offset) {

     // DQ (11/3/2011): Avoid warning about pointless comparison of unsigned integer with zero.
     // assert(offset>=0 && offset<=x.size());
        assert(offset <= x.size());

        p_pool = x.pool() + offset;
        p_capacity = x.capacity() - offset;
        p_size = x.size() - offset;
    }
    // new vector points to subset of initializer vector
    SgSharedVector(const SgSharedVector &x, size_type offset, size_type size) {
        assert(offset+size <= x.size());
        p_pool = x.pool() + offset;
        p_capacity = x.capacity() - offset;
        p_size = size;
    }
    // pool memory is managed externally
    ~SgSharedVector() {}

    /**** Assignment ****/
    SgSharedVector& operator=(const SgSharedVector &x) {
        p_pool = x.pool();
        p_capacity = x.capacity();
        p_size = x.size();
        return *this;
    }

    /**** Erasure ****/
    void clear() {
        p_pool = NULL;
        p_size = 0;
        p_capacity = 0;
    }
    void erase(iterator position) {
        abort(); //FIXME
    }
    void erase(iterator start, iterator finish) {
        abort(); //FIXME
    }
    void pop_back() {
        assert(p_size>0);
        --p_size;
    }

    /**** Insertion ****/
    void insert(iterator position, const_reference x) {
        abort(); //FIXME
    }
    void insert(iterator position, size_type n, const_reference x) {
        abort(); //FIXME
    }
    void insert(iterator position, iterator start, iterator finish) {
        abort(); //FIXME
    }
    void push_back(const_reference x) {
        assert(p_size<p_capacity);
        p_pool[p_size++] = x;
    }
        
    /**** Iterators ****/
    iterator begin() { return iterator(p_pool); };
    iterator end() { return iterator(p_pool+p_size); };

    const_iterator  begin() const { return const_iterator(p_pool); };
    const_iterator  end() const { return const_iterator(p_pool+p_size); };

    /**** Element Reference ****/
    reference operator[](size_type n) {
        return at(n);
    }
    const_reference operator[](size_type n) const {
        return at(n);
    }
    reference at(size_type n) {
     // DQ (11/3/2011): Avoid warning about pointless comparison of unsigned integer with zero.
     // assert(n>=0 && n<p_size);
        assert(n < p_size);

        return p_pool[n];
    }
    const_reference at(size_type n) const {
     // DQ (11/3/2011): Avoid warning about pointless comparison of unsigned integer with zero.
        assert(n < p_size);
        return p_pool[n];
    }
    reference back() {
        assert(p_size>0);
        return p_pool[p_size-1];
    }
    const_reference back() const {
        assert(p_size>0);
        return p_pool[p_size-1];
    }
    reference front() {
        assert(p_size>0);
        return p_pool[0];
    }
    const_reference front() const {
        assert(p_size>0);
        return p_pool[0];
    }
    pointer pool() const {
        return p_pool;
    }
    
    /**** Size ****/
    size_type capacity() const {
        return p_capacity;
    }
    bool empty() const {
        return 0==p_size;
    }
    size_type max_size() const {
        return (size_t)-1;
    }
    void reserve(size_type n) {
        assert(n<=p_capacity); //not possible to increase capacity
    }
    void resize(size_type n) {
        if (n<p_size) {
            p_size = n;
        } else {
            assert(n<=p_capacity);
            p_size = n;
        }
    }
    size_type size() const {
        return p_size;
    }

    /**** Not implemented ****/
    // allocator_type get_allocator() const;
    // void swap(SgSharedVector&)
};

template<class T>bool operator==(const SgSharedVector<T> &x, const SgSharedVector<T> &y);
template<class T>bool operator!=(const SgSharedVector<T> &x, const SgSharedVector<T> &y);
template<class T>void swap(SgSharedVector<T> &x, SgSharedVector<T> &y);

#endif
#endif
