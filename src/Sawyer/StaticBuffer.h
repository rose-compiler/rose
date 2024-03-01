// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_StaticBuffer_H
#define Sawyer_StaticBuffer_H

#include <Sawyer/AllocatingBuffer.h>
#include <Sawyer/Assert.h>
#include <Sawyer/Buffer.h>
#include <Sawyer/Sawyer.h>

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
#include <boost/serialization/array.hpp>
#endif

#ifdef SAWYER_HAVE_CEREAL
#include <cereal/types/array.hpp>
#endif

namespace Sawyer {
namespace Container {

/** Points to static data.
 *
 *  This buffer object points to storage which is not owned by this object and which is therefore not deleted when this object
 *  is deleted. */
template<class A, class T>
class StaticBuffer: public Buffer<A, T> {
public:
    typedef A Address;                                  /**< Type of addresses. */
    typedef T Value;                                    /**< Type of values. */
    typedef Buffer<A, T> Super;                         /**< Type of base class. */

private:
    Value *values_;
    Address size_;
    bool rdonly_;

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(size_);
        s & BOOST_SERIALIZATION_NVP(rdonly_);
        s & boost::serialization::make_nvp("values", boost::serialization::make_array(values_, size_));
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(size_);
        s & BOOST_SERIALIZATION_NVP(rdonly_);
        values_ = new Value[size_];
        s & boost::serialization::make_nvp("values", boost::serialization::make_array(values_, size_));
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive &archive) const {
        archive(cereal::base_class<Super>());
        archive(CEREAL_NVP(size_));
        archive(CEREAL_NVP(rdonly_));
        archive(cereal::binary_data(values_, size_));
    }

    template<class Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive &archive) {
        archive(cereal::base_class<Super>());
        archive(CEREAL_NVP(size_));
        archive(CEREAL_NVP(rdonly_));
        values_ = new Value[size_];
        archive(cereal::binary_data(values_, size_));
    }
#endif

private:
    // For serialization only
    StaticBuffer()
        : values_(NULL), size_(0), rdonly_(false) {}

protected:
    StaticBuffer(Value *values, Address size)
        : Super(".StaticBuffer"), values_(values), size_(size), rdonly_(false) {
        ASSERT_require(size==0 || values!=NULL);
    }
    StaticBuffer(const Value *values, Address size)
        : Super(".StaticBuffer"), values_(const_cast<Value*>(values)), size_(size), rdonly_(true) {
        ASSERT_require(size==0 || values!=NULL);
    }

public:
    /** Construct from caller-supplied data.
     *
     *  The caller supplies a pointer to data and the size of that data.  The new buffer object does not take ownership of the
     *  data or copy it, thus the caller-supplied data must continue to exist for as long as this buffer exists.
     *
     * @{ */
    static typename Buffer<A, T>::Ptr instance(Value *values, Address size) {
        return typename Buffer<A, T>::Ptr(new StaticBuffer(values, size));
    }
    static typename Buffer<A, T>::Ptr instance(const Value *values, Address size) {
        return typename Buffer<A, T>::Ptr(new StaticBuffer(values, size));
    }
    /** @} */

    /** True if the buffer is read-only.
     *
     *  A buffer created from a const value is read only. */
    bool isReadOnly() const {
        return rdonly_;
    }

    // It doesn't make sense to exactly copy a static buffer because the point is to create a new buffer that points to data
    // that is independent of the source buffer.  Therefore we create an allocating buffer instead.
    typename Buffer<A, T>::Ptr copy() const /*override*/ {
        typename Buffer<A, T>::Ptr newBuffer = AllocatingBuffer<A, T>::instance(size_);
        Address nWritten = newBuffer->write(values_, 0, size_);
        if (nWritten != size_) {
            throw std::runtime_error("StaticBuffer::copy() failed after copying " +
                                     boost::lexical_cast<std::string>(nWritten) + " of " +
                                     boost::lexical_cast<std::string>(size_) +
                                     (1==size_?" value":" values"));
        }
        return newBuffer;
    }
    
    Address available(Address start) const /*override*/ {
        return start < size_ ? size_-start : 0;
    }

    void resize(Address newSize) /*override*/ {
        if (newSize != size_)
            throw std::runtime_error("unable to resize StaticBuffer");
    }

    Address read(Value *buf, Address address, Address n) const /*override*/ {
        n = std::min(n, available(address));
        if (buf)
            memcpy(buf, values_+address, n*sizeof(values_[0]));
        return n;
    }

    Address write(const Value *buf, Address address, Address n) /*override*/ {
        if (rdonly_)
            return 0;
        n = std::min(n, available(address));
        if (buf)
            memcpy(values_+address, buf, n*sizeof(values_[0]));
        return n;
    }

    const Value* data() const /*override*/ {
        return values_;
    }
};

} // namespace
} // namespace

#endif
