// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_MappedBuffer_H
#define Sawyer_MappedBuffer_H

#include <Sawyer/AllocatingBuffer.h>
#include <Sawyer/Buffer.h>
#include <Sawyer/Sawyer.h>
#include <Sawyer/StaticBuffer.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/lexical_cast.hpp>

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
#include <boost/serialization/string.hpp>
#endif

#ifdef SAWYER_HAVE_CEREAL
#include <cereal/types/string.hpp>
#endif

namespace Sawyer {
namespace Container {

/** Memory mapped file.
 *
 *  This buffer points to a file that is mapped into memory by the operating system.  The API supports a common divisor for
 *  POSIX and Microsoft Windows and is therefore not all that powerful, but it does allow simple maps to be created that have a
 *  file as backing store.  See http://www.boost.org/doc/libs for more information.
 *
 *  Access modes are the following enumerated constants:
 *
 *  @li <code>boost::iostreams::mapped_file::readonly</code>: shared read-only access
 *  @li <code>boost::iostreams::mapped_file::readwrite</code>: shared read/write access
 *  @li <code>boost::iostreams::mapped_file::priv</code>: private read/write access
 *  
 *  When a file is mapped with private access changes written to the buffer are not reflected in the underlying file. */
template<class A, class T>
class MappedBuffer: public Buffer<A, T> {
    boost::iostreams::mapped_file_params params_;
    boost::iostreams::mapped_file device_;

public:
    typedef A Address;                                  /**< Type of addresses. */
    typedef T Value;                                    /**< Type of values. */
    typedef Buffer<A, T> Super;                         /**< Type of base class. */

#ifdef SAWYER_HAVE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    // Users: You'll need to register the subclass once you know its type, such as
    // BOOST_CLASS_REGISTER(Sawyer::Container::MappedBuffer<size_t,uint8_t>);
    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & boost::serialization::make_nvp("path", params_.path);
        s & boost::serialization::make_nvp("flags", params_.flags);
        s & boost::serialization::make_nvp("mode", params_.mode);
        s & boost::serialization::make_nvp("offset", params_.offset);
        s & boost::serialization::make_nvp("length", params_.length);
        s & boost::serialization::make_nvp("new_file_size", params_.new_file_size);

        boost::uint64_t hint;
        BOOST_STATIC_ASSERT(sizeof hint >= sizeof params_.hint);
        hint = (boost::uint64_t)(params_.hint);
        s & BOOST_SERIALIZATION_NVP(hint);
    }

    // Users: You'll need to register the subclass once you know its type, such as
    // BOOST_CLASS_REGISTER(Sawyer::Container::MappedBuffer<size_t,uint8_t>);
    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & boost::serialization::make_nvp("path", params_.path);
        s & boost::serialization::make_nvp("flags", params_.flags);
        s & boost::serialization::make_nvp("mode", params_.mode);
        s & boost::serialization::make_nvp("offset", params_.offset);
        s & boost::serialization::make_nvp("length", params_.length);
        s & boost::serialization::make_nvp("new_file_size", params_.new_file_size);

        boost::uint64_t hint;
        BOOST_STATIC_ASSERT(sizeof hint >= sizeof params_.hint);
        s & BOOST_SERIALIZATION_NVP(hint);
        params_.hint = (const char*)hint;

        device_.open(params_);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

#ifdef SAWYER_HAVE_CEREAL
private:
    friend class cereal::access;

    template<class Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive &archive) const {
        archive(cereal::base_class<Super>());
        archive(cereal::make_nvp("path", params_.path));
        archive(cereal::make_nvp("flags", params_.flags));
        archive(cereal::make_nvp("mode", params_.mode));
        archive(cereal::make_nvp("offset", params_.offset));
        archive(cereal::make_nvp("length", params_.length));
        archive(cereal::make_nvp("new_file_size", params_.new_file_size));

        boost::uint64_t hint;
        BOOST_STATIC_ASSERT(sizeof hint >= sizeof params_.hint);
        hint = (boost::uint64_t)(params_.hint);
        archive(CEREAL_NVP(hint));
    }

    template<class Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive &archive) {
        archive(cereal::base_class<Super>());
        archive(cereal::make_nvp("path", params_.path));
        archive(cereal::make_nvp("flags", params_.flags));
        archive(cereal::make_nvp("mode", params_.mode));
        archive(cereal::make_nvp("offset", params_.offset));
        archive(cereal::make_nvp("length", params_.length));
        archive(cereal::make_nvp("new_file_size", params_.new_file_size));

        boost::uint64_t hint;
        BOOST_STATIC_ASSERT(sizeof hint >= sizeof params_.hint);
        archive(CEREAL_NVP(hint));
        params_.hint = (const char*)hint;

        device_.open(params_);
    }
#endif

protected:
    MappedBuffer()
        : Super(".MappedBuffer") {}           // needed for de-serialization
    explicit MappedBuffer(const boost::iostreams::mapped_file_params &params)
        : Super(".MappedBuffer"), params_(params), device_(params) {}

public:
    /** Map a file according to boost parameters.
     *
     *  The parameters describe which file (by name) and part thereof should be mapped into memory. */
    static typename Buffer<A, T>::Ptr instance(const boost::iostreams::mapped_file_params &params) {
        try {
            return typename Buffer<A, T>::Ptr(new MappedBuffer(params));
        } catch (const std::ios_base::failure &e) {
            if (boost::contains(e.what(), "Invalid argument") &&
                boost::filesystem::is_regular_file(params.path) &&
                boost::filesystem::is_empty(params.path)) {
                return StaticBuffer<Address, Value>::instance((const Value*)NULL, 0);
            } else {
                throw;
            }
        }
    }

    /** Map a file by name.
     *
     *  The specified file, which must already exist, is mapped into memory and pointed to by this new buffer. */
    static typename Buffer<A, T>::Ptr
    instance(const boost::filesystem::path &path,
             boost::iostreams::mapped_file::mapmode mode=boost::iostreams::mapped_file::readonly,
             boost::intmax_t offset=0,
             boost::iostreams::mapped_file::size_type length=boost::iostreams::mapped_file::max_length) {
        boost::iostreams::mapped_file_params params(path.string());
        params.flags = mode;
        params.length = length;
        params.offset = offset;
        return instance(params);
    }

    // It doesn't make sense to copy a memory-mapped buffer since the point of copying is to result in two independent buffers
    // pointing to non-shared data. If a shared, writable, memory-mapped buffer is backed by a file and we make a new copy also
    // backed by the file, then changing one buffer would change the other.  Therefore, we allocate new memory that will hold a
    // snapshot of the source buffer.
    typename Buffer<A, T>::Ptr copy() const /*override*/ {
        typename Buffer<A, T>::Ptr newBuffer = AllocatingBuffer<A, T>::instance(this->size());
        Address nWritten = newBuffer->write((const Value*)device_.data(), 0, this->size());
        if (nWritten != this->size()) {
            throw std::runtime_error("MappedBuffer::copy() failed after copying " +
                                     boost::lexical_cast<std::string>(nWritten) + " of " +
                                     boost::lexical_cast<std::string>(this->size()) +
                                     (1==this->size()?" value":" values"));
        }
        return newBuffer;
    }
    
    Address available(Address address) const /*override*/ {
        return address >= device_.size() ? Address(0) : (Address(device_.size()) - address) / sizeof(Value);
    }

    void resize(Address n) /*override*/ {
        if (n != this->size())
            throw std::runtime_error("resizing not allowed for MappedBuffer");
    }

    Address read(Value *buf, Address address, Address n) const /*override*/ {
        Address nread = std::min(n, available(address));
        memcpy(buf, device_.const_data() + address, nread * sizeof(Value));
        return nread;
    }

    Address write(const Value *buf, Address address, Address n) /*override*/ {
        Address nwritten = std::min(n, available(address));
        memcpy(device_.data() + address, buf, nwritten * sizeof(Value));
        return nwritten;
    }

    const Value* data() const /*override*/ {
        return (Value*)device_.const_data();
    }
};

} // namespace
} // namespace
#endif
