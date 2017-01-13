// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_NullBuffer_H
#define Sawyer_NullBuffer_H

#include <Sawyer/Buffer.h>
#include <Sawyer/Sawyer.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

namespace Sawyer {
namespace Container {

/** %Buffer that has no data.
 *
 *  This can be useful to reserve areas of of a BufferMap address space without actually storing any data at them.  All
 *  reads return default values and writes using such a buffer will fail (return zero). */
template<class A, class T>
class NullBuffer: public Buffer<A, T> {
public:
    typedef A Address;                                  /**< Type of addresses. */
    typedef T Value;                                    /**< Type of values. */
    typedef Buffer<A, T> Super;                         /**< Type of base class. */

private:
    Address size_;

private:
    friend class boost::serialization::access;

    // Users: You'll need to register the subclass once you know its type, such as
    // BOOST_CLASS_REGISTER(Sawyer::Container::NullBuffer<size_t,uint8_t>);
    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & boost::serialization::base_object<Super>(*this);
        s & size_;
    }

protected:
    NullBuffer(): Super(".NullBuffer"), size_(0) {}
    explicit NullBuffer(Address size): Super(".NullBuffer"), size_(size) {}

public:
    /** Construct a new buffer.
     *
     *  The new buffer will act as if it contains @p size values, although no values will actually be stored. */
    static typename Buffer<A, T>::Ptr instance(Address size) {
        return typename Buffer<A, T>::Ptr(new NullBuffer(size));
    }

    typename Buffer<A, T>::Ptr copy() const /*override*/ {
        return instance(size_);
    }
    
    Address available(Address start) const /*override*/ {
        return start < size_ ? size_ - start : 0;
    }

    void resize(Address newSize) /*override*/ {
        size_ = newSize;
    }

    Address read(Value *buf, Address address, Address n) const /*override*/ {
        Address nread = std::min(available(address), n);
        if (buf) {
            for (Address i=0; i<n; ++i)
                buf[i] = Value();
        }
        return nread;
    }

    Address write(const Value */*buf*/, Address /*address*/, Address /*n*/) /*override*/ {
        return 0;
    }

    const Value* data() const /*override*/ {
        return NULL;
    }
};

} // namespace
} // namespace

#endif
