#ifndef ROSE_BinaryAnalysis_DistanceMatrix_H
#define ROSE_BinaryAnalysis_DistanceMatrix_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#ifdef ROSE_HAVE_DLIB
#include <dlib/matrix.h>
#endif

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {

/** Matrix values.
 *
 *  This is a matrix that uses the Dlib-like API, with the main difference being that sizes are of type @c size_t instead of a
 *  signed type. */
template<class T>
class Matrix {
public:
    typedef T Value;
    
#ifdef ROSE_HAVE_DLIB
    //-------- Dlib implementation --------
private:
    dlib::matrix<T> data_;
public:
    Matrix(): data_(0, 0) {}
    explicit Matrix(size_t n): data_(n, n) {}
    Matrix(size_t nr, size_t nc): data_(nr, nc) {}
    size_t nr() const { return data_.nr(); }
    size_t nc() const { return data_.nc(); }
    T& operator()(size_t i, size_t j) { return data_(i, j); }
    const T& operator()(size_t i, size_t j) const { return data_(i, j); }
    const dlib::matrix<T>& dlib() const { return data_; }
#else
    //-------- ROSE implementation --------
private:
    size_t nr_, nc_;
    T* data_;
public:
    // Construction and destruction
    Matrix(): nr_(0), nc_(0), data_(NULL) {}
    explicit Matrix(size_t n): nr_(n), nc_(n), data_(new T[n*n]) {}
    Matrix(size_t nr, size_t nc): nr_(nr), nc_(nc), data_(new T[nr*nc]) {}
    ~Matrix() { delete[] data_; }

    // Copying
    Matrix(const Matrix<T> &other): nr_(other.nr_), nc_(other.nc_), data_(new T[other.size()]) {
        memcpy(data_, other.data_, size()*sizeof(T));
    }
    Matrix& operator=(const Matrix<T> &other) {
        if (size() != other.size()) {
            delete[] data_;
            data_ = new T[other.size()];
        }
        nr_ = other.nr_;
        nc_ = other.nc_;
        memcpy(data_, other.data_, size()*sizeof(T));
        return *this;
    }

    // Size
    size_t nr() const { return nr_; }
    size_t nc() const { return nc_; }

    // Element access
    T& operator()(size_t i, size_t j) {
        ASSERT_require(i < nr_);
        ASSERT_require(j < nc_);
        return data_[i*nc_+j];
    }
    const T& operator()(size_t i, size_t j) const {
        ASSERT_require(i < nr_);
        ASSERT_require(j < nc_);
        return data_[i*nc_+j];
    }

    // dlib::matrix<T> &dlib() -- cannot do this here since there's no dlib
#endif

    //--------  Common implementation --------
public:
    size_t size() const { return nr()*nc(); }

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        size_t nr = this->nr(), nc = this->nc();
        s & BOOST_SERIALIZATION_NVP(nr);
        s & BOOST_SERIALIZATION_NVP(nc);
        if (nr > 0 && nc > 0) {
            const T *values = &(*this)(0, 0);
            s & boost::serialization::make_nvp("values", boost::serialization::make_array(values, nr*nc));
        }
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        size_t nr, nc;
        s & BOOST_SERIALIZATION_NVP(nr);
        s & BOOST_SERIALIZATION_NVP(nc);
        Matrix<T> tmp(nr, nc);
        if (nr > 0 && nc > 0) {
            T *values = &tmp(0, 0);
            s & boost::serialization::make_nvp("values", boost::serialization::make_array(values, nr*nc));
        }
        *this = tmp;
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif
};

} // namespace
} // namespace

#endif
#endif
