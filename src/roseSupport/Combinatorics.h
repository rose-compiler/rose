#ifndef ROSE_Combinatorics_H
#define ROSE_Combinatorics_H

#include <rosePublicConfig.h>

#include <Rose/Constants.h>
#include <ROSE_DEPRECATED.h>
#include <ROSE_UNUSED.h>

#include <algorithm>
#include <cassert>
#include <istream>
#include <list>
#include <memory>
#include <ostream>
#include <Rose/Exception.h>
#include <Sawyer/Assert.h>
#include <Sawyer/Synchronization.h>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

#ifdef ROSE_HAVE_LIBGCRYPT
#include <gcrypt.h>
#endif

namespace Rose {

/** Combinatoric functions. */
namespace Combinatorics {

/** Returns the factorial of @p n. */
template<typename T>
static T
factorial(const T n) {
    T retval = 1;
    while (n>1) {
        const T next = retval * n--;
        assert(next > retval); // overflow; signed overflow is UB, so this likely doesn't work right when T is signed
        retval = next;
    }
    return retval;
}

/** Simulate flipping a coin. Randomly returns true or false with equal probability. */
ROSE_DLL_API bool flip_coin();

/** Permute a vector according to the specified permutation number.
 *
 *  The permutation number should be between zero (inclusive) and the factorial of the values size (exclusive).  A permutation
 *  number of zero is a no-op; higher permutation numbers shuffle the values in repeatable ways.  Using swap rather that
 *  erase/insert is much faster than the standard Lehmer codes, but doesn't return permutations in lexicographic order.  This
 *  function can perform approx 9.6 million permutations per second on a vector of 12 64-bit integers on Robb's machine (computing
 *  all 12! permutations in about 50 seconds). */
template<typename T>
static void
permute(std::vector<T> &values/*in,out*/, uint64_t pn, const size_t sz = UNLIMITED) {
    if (UNLIMITED == sz)
        sz = values.size();
    assert(sz <= values.size());
    assert(pn < factorial(sz));
    for (size_t i = 0; i < sz; ++i) {
        uint64_t radix = sz - i;
        uint64_t idx = pn % radix;
        std::swap(values[i + idx], values[i]);
        pn /= radix;
    }
}

/** Shuffle the values of a vector.
 *
 *  This algorithm randomly shuffles the items in the vector by swapping values at indexes zero through @p limit with values at
 *  randomly selected indexes zero through @p nitems. The defaults for @p nitems and @p limit are the size of the input @p
 *  vector. */
template<typename T>
void
shuffle(std::vector<T> &vector, size_t nitems = UNLIMITED, size_t limit = UNLIMITED) {
    nitems = std::min(nitems, vector.size());
    limit = std::min(limit, nitems);

    for (size_t i=0; i<limit; ++i) {
        size_t j = Sawyer::fastRandomIndex(nitems);
        std::swap(vector[i], vector[j]);
    }
}

/** Reorder the values of one vector according to another.
 *
 *  The second vector says how the first vector is rearranged. The argument @p values are the values to be reordered, and the
 *  argument @p remap says how to reorder them.  Both arguments must be the same length, <em>n</em>. The @p remap argument contains
 *  all the integer indices from 0 (inclusive) to <em>n</em> (exclusive).  If the @p values in their original order are named "old"
 *  and the @p values in their new desired order are named "new", then new[i] = old[remap[i]].
 *
 *  This function is useful when sorting parallel arrays. Say we have three parallel arrays named @c a, @c b, and @c c and we want
 *  to sort them all so array @c a is in ascending order according to its natural less-than operator. We could do that as follows:
 *
 * @code
 *  std::vector<ValueA> a = { ... };
 *  std::vector<ValueB> b = { ... };
 *  std::vector<ValueC> c = { ... };
 *
 *  std::vector<size_t> remap(a.size());
 *  std::iota(remap.begin(), remap.end(), 0);
 *  std::sort(remap.begin(), remap.end(), [&a](const size_t i, const size_t j) {
 *      return a[i] < a[j];
 *  });
 *
 *  reorder(a, remap);
 *  reorder(b, remap);
 *  reorder(c, remap);
 * @endcode */
template<class T>
void
reorder(std::vector<T> &values, const std::vector<size_t> &remap) {
    assert(values.size() == remap.size());

    // O(n) implementation using a temporary vector. Alternatively, we could use an O(n^2) algorithm that uses constant space.
    std::vector<T> old = values;
    for (size_t i = 0; i < old.size(); ++i)
        values[i] = old[remap[i]];
}

/**
 *  Converts a 64 bit int to base 62.
 *
 *  The symbols in base 62 are the digits '0' through '9', followed by the upper-case letters 'A' through 'Z', followed by the
 *  lower-case letters 'a' through 'z' (62 symbols in total). Note that this order is not the same as Base64 (RFC 4648).
 *
 *  The return value is useful as the non-leading part of a C++ identifier. It cannot serve as the beginning (or complete)
 *  identifier because the return value might start with characters that are not valid for the start of a C++ identifier (i.e, the
 *  digits '0' through '9').
 *
 *  The return value is not even close to being the most compressed format for an identifier since there are approximately 760 valid
 *  characters that can occur at the start and within a C++ identifier. But it does return strings that are almost the most compact
 *  for the set of valid ASCII characters. */
ROSE_DLL_API std::string toBase62String(uint64_t);

/** Converts a base 62 string to a 64 bit int.
 *
 *  The symbols in base 62 are the digits '0' through '9', followed by the upper-case letters 'A' through 'Z', followed by the
 *  lower-case letters 'a' through 'z' (62 symbols in total).
 *  
 *  Base 62 is used to store hashes in a way that can also almost be a C++ identifier, although base 62 can start with the digits
 *  '0' through '9' which are not valid first characters for a C++ identifier. */
ROSE_DLL_API uint64_t fromBase62String(const std::string& base62);

/** Hash interface.
 *
 *  This class defines the API for hash functions. A hash function takes an arbitrary size message as input and returns a
 *  fixed size digest. The subclasses implement specific hash functions, the digests of which are different sizes. For
 *  instance, a SHA1 digest is always 20 bytes, and a SHA512 digest is always 64 bytes.
 *
 *  The digest can be computed incrementally by inserting the message in parts using various @ref insert functions. Once the
 *  message has been fully inserted, the digest can be obtained with the @ref digest function. The @ref digest can be retrieved
 *  repeatedly, but no additional message parts can be added once a digest is retrieved (doing so will result in an @ref
 *  Hasher::Exception "Exception" being thrown).  However, the hasher can be reset to an initial state by calling @ref clear,
 *  after which a new message can be inserted.  Subclasses that return hashes that are 8 bytes or narrower sometimes have an
 *  additional method that returns the digest as an unsigned integer (but the @ref digest method must always be present).
 *
 *  Hasher objects are copyable. The new hasher starts out in the same state as the original hasher, but then they can diverge
 *  from one another. For instance, if you have a multi-part message and want an intermediate hash, the easiest way to do that
 *  is to insert some message parts into a hasher, then make a copy to obtain its digest, then continue inserting message parts
 *  into the original hasher (the copy cannot accept more parts since we called its @ref digest method). Obtaining intermediate
 *  hashes this way is usually faster than re-hashing.
 *
 *  The API contains a few functions for printing hashes. There's a @ref toString function that can return the digest as a
 *  hexadecimal string and a static version that that converts a given digest to a string. Also, using a hasher in an @c
 *  std::ostream output operator is the same as obtaining the hasher's digest, formatting it, and sending the string to the
 *  stream.  All these functions (except the static @c toString) call @ref digest under the covers, thus finalizing the
 *  hasher.
 *
 *  New hash functions can be created very easily by subclassing Hasher and defining an @ref append method. Often, this is all
 *  that's necessary. Other commonly overridden functions are the constructor, @ref clear, and @ref digest.  The subclasses
 *  generally have names beginning with the string "Hasher" so they can be found easily in alphabetical class listings. For
 *  instance, see the classes and typedefs in the @ref Rose::Combinatorics namespace.
 *
 *  If a hasher uses an external library (like libgcrypt) then it should be designed in such a way that the code that uses the
 *  hash compiles, but throws an exception.  The @ref Hasher::Exception "Exception" class is intended for this purpose, as well
 *  as all other situations where hashing fails. */
class ROSE_DLL_API Hasher {
public:
    /** The digest of the input message.
     *
     *  Since different hash functions have different sized digests, the digest is represented most generically as a vector of
     *  bytes. */
    typedef std::vector<uint8_t> Digest;

    /** Exceptions for hashing. */
    class Exception: public Rose::Exception {
    public:
        /** Constructor. */
        Exception(const std::string &mesg): Rose::Exception(mesg) {}
        ~Exception() throw () {}
    };
    
protected:
    Digest digest_;

public:
    virtual ~Hasher() {}

    /** Reset the hasher to its initial state. */
    virtual void clear();

    /** Return the digest.
     *
     *  Finalizes the hash function and returns the digest for all the input.  Additional input should not be inserted after this
     *  function is called since some hash functions don't support this. */
    virtual const Digest& digest();

    /** Insert data into the digest.
     *
     *  This method inserts data into a digest. Data can only be inserted if the user has not called @ref digest yet.
     *
     * @{ */
    void insert(const std::string&);
    void insert(uint64_t);
    void insert(const uint8_t *bytes, size_t nBytes);
    void insert(const std::vector<uint8_t>&);
    void insert(std::istream&);
    /** @} */
    
    /** Insert data into the digest.
     *
     *  This is the lowest level method of inserting new message content into the digest. This can be called as often as
     *  desired, building a digest incrementally. */
    virtual void append(const uint8_t *message, size_t messageSize) = 0;

    /** Convert a digest to a hexadecimal string. */
    static std::string toString(const Digest&);

    /** String representation of the digest.
     *
     *  This works by first calling @ref digest, which finalizes the hasher and thus later calls to @ref insert will not be
     *  permitted unless the hasher is first reset. */
    std::string toString();

    /** Returns the hash as a 64 bit int.
     *
     * The return value is computed by a sequence of shift and xor operations across all bytes of the digest. If the digest is eight
     * or fewer bytes, then this degenerates to the uint64_t interpretation of the big-endian digest.
     *
     * @{ */
    uint64_t toU64();
    uint64_t toU64(const Digest&);
    /** @} */

    // [Robb Matzke 2025-05-16]: deprecated; casting to another type should have "to" in the name, like "toString" above.
    uint64_t make64Bits() ROSE_DEPRECATED("use toU64");
    uint64_t make64Bits(const Digest&) ROSE_DEPRECATED("use toU64");

    /** Print a hash to a stream.
     *
     *  This is a wrapper that calls the @ref digest function to finalize the hash, converts the digest to a hexadecimal
     *  string, and sends it to the stream. */
    void print(std::ostream&);

    /** Common subclass all the classes that construct Hashers.
     *
     *  This is for the HasherFactory.
     *
     *  Actually, there is only one (templated) class that makes Hashers.  @ref HasherMaker does it all. */
    class IHasherMaker {
    public:
        virtual std::shared_ptr<Hasher> create() const = 0;
        virtual ~IHasherMaker() {}
    };
    
    /** Templated to create any Hasher and register it with @ref HasherFactory.
     *
     *  @ref HasherMaker makes the @ref Hasher named by `typename T`, and automatically registers itself with @ref HasherFactory.
     *  If a user creates a new @ref Hasher, In their implemntation file (.C) they should include a static variable declared and
     *  defined like this
     *
     *  @code
     *  static Hasher::HasherMaker<HasherSha256Builtin> makerSHA256("SHA256");
     *  @endcode
     *
     *  The instantiation of this variable at module load time will call the @ref HasherMaker constructor, which automatically
     *  registers it with the @ref HasherFactory (and constructs the @ref HasherFactory if necessary.) */
    template<typename T>
    class HasherMaker : public IHasherMaker {
    public:
        /** Creates a HasherMaker and registers it with @ref HasherFactory.
         *
         *  Make @ref HasherMakers static variableso so this is run at module initialization time.
         *
         *  @param[in] hashType  The name/key of this hasher in the HasherFactory. */
        HasherMaker(const std::string& hashType) {
            HasherFactory::instance().registerMaker(hashType, this);
        }

        /** Creates a Hasher.
         *
         *  The type of Hasher is determined by the `typename T` and returns it as a `shared_ptr`. */
        virtual std::shared_ptr<Hasher> create() const {
            return std::make_shared<T>();
        }
    };

    /** A singleton that creates and returns Hashers by name.
     *
     * HasherFactory contains a map of names to @ref HasherMaker .  When createHasher is passed a name, it will attempt to create
     * the correct @ref Hasher and pass it back.  Users can add hashers to HasherFactory using @ref HasherMaker.  HasherFactory is
     * created when @ref instance is first called.  Generally this is done at module initialization time.
     **/
    class HasherFactory {
    public:
        /** Returns a reference to the HasherFactory singleton.
         *
         *  Creates a HasherFactory if necessary **/
        static HasherFactory& instance();

        // [Robb Matzke 2025-05-16]: deprecated; violates ROSE public function naming convention
        static HasherFactory& Instance() ROSE_DEPRECATED("use instance");
        
        /** Adds a new @HasherMaker to the HasherFactory.
         *
         *  Usually called by the @ref HasherMaker constructor. **/
        void registerMaker(const std::string& hashType, IHasherMaker* createHasherPtr);
        
        /** Creates a registered Hasher by type from the map 
         *
         *  @param[in] hashType The type of the @ref Hasher to create. e.g. FNV.
         *  @return A @ref Hasher of the correct type in an `std::shared_ptr`.
         *  @throw  @ref Rose::Combinatorics::Exception if `hashType` names an unsupported type. */
        std::shared_ptr<Hasher> createHasher(const std::string& hashType) const;
        
    private:
        HasherFactory() {}
        
        // Non-copyable
        HasherFactory(const HasherFactory&) = delete;
        HasherFactory& operator=(const HasherFactory&) = delete;
        
        // Maps keys to @ref HasherMaker
        std::map<std::string, IHasherMaker* > hashMakers;
    };
};

/** Hasher for any libgcrypt hash algorithm.
 *
 *  The @c hashAlgorithmId template parameter is one of the libgcrypt algorithm constants (type int) with a name starting with
 *  "GCRY_MD_". You can find a list
 *  [here](https://gnupg.org/documentation/manuals/gcrypt/Available-hash-algorithms.html#Available-hash-algorithms). */
template<int hashAlgorithmId>
class HasherGcrypt: public Hasher {
#ifdef ROSE_HAVE_LIBGCRYPT
    gcry_md_hd_t md_;
#endif

public:
    HasherGcrypt() {
      #ifdef ROSE_HAVE_LIBGCRYPT
        if (gcry_md_open(&md_, hashAlgorithmId, 0) != GPG_ERR_NO_ERROR)
            throw Exception("cannot initialize libgcrypt hash " + std::string(gcry_md_algo_name(hashAlgorithmId)));
      #else
        throw Exception("ROSE was not configured with libgcrypt");
      #endif
    }

    HasherGcrypt(const HasherGcrypt &other) {
      #ifdef ROSE_HAVE_LIBGCRYPT
        if (gcry_md_copy(&md_, other.md_) != GPG_ERR_NO_ERROR)
            throw Exception("cannot copy libgcrypt hash " + std::string(gcry_md_algo_name(hashAlgorithmId)));
      #else
        ROSE_UNUSED(other);
        throw Exception("ROSE was not configured with libgcrypt");
      #endif
    }

    ~HasherGcrypt() {
      #ifdef ROSE_HAVE_LIBGCRYPT
        gcry_md_close(md_);
      #endif
    }

    HasherGcrypt& operator=(const HasherGcrypt &other) {
      #ifdef ROSE_HAVE_LIBGCRYPT
        gcry_md_close(md_);
        if (gcry_md_copy(&md_, other.md_) != GPG_ERR_NO_ERROR)
            throw Exception("cannot copy libgcrypt hash " + std::string(gcry_md_algo_name(hashAlgorithmId)));
      #else
        ROSE_UNUSED(other);
        throw Exception("ROSE was not configured with libgcrypt");
      #endif
    }
    
    void clear() {
      #ifdef ROSE_HAVE_LIBGCRYPT
        gcry_md_reset(md_);
      #endif
        Hasher::clear();
    }

    const Digest& digest() {
        if (digest_.empty()) {
          #ifdef ROSE_HAVE_LIBGCRYPT
            gcry_md_final(md_);
            digest_.resize(gcry_md_get_algo_dlen(hashAlgorithmId), 0);
            uint8_t *d = gcry_md_read(md_, hashAlgorithmId);
            ASSERT_not_null(d);
            memcpy(&digest_[0], d, digest_.size());
          #else
            ASSERT_not_reachable("ROSE was not configured with libgcrypt");
          #endif
        }
        return Hasher::digest();
    }

    void append(const uint8_t *message, size_t messageSize) {
        ASSERT_require(message || 0==messageSize);
      #ifdef ROSE_HAVE_LIBGCRYPT
        if (!digest_.empty())
            throw Exception("cannot append after returning digest");
        if (messageSize > 0)
            gcry_md_write(md_, message, messageSize);
      #else
        ASSERT_not_reachable("ROSE was not configured with libgcrypt");
      #endif
    }
};

#ifdef ROSE_HAVE_LIBGCRYPT
typedef HasherGcrypt<GCRY_MD_MD5> HasherMd5;            /**< MD5 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<GCRY_MD_SHA1> HasherSha1;          /**< SHA1 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<GCRY_MD_SHA256> HasherSha256;      /**< SHA-256 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<GCRY_MD_SHA384> HasherSha384;      /**< SHA-384 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<GCRY_MD_SHA512> HasherSha512;      /**< SHA-512 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<GCRY_MD_CRC32> HasherCrc32;        /**< ISO 3309 hasher. Throws exception if libgcrypt is not configured. */
#else // the template argument for the following unimplemented hashers is arbitrary and they can all be the same
typedef HasherGcrypt<0> HasherMd5;                      /**< MD5 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<0> HasherSha1;                     /**< SHA1 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<0> HasherSha256;                   /**< SHA-256 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<0> HasherSha384;                   /**< SHA-384 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<0> HasherSha512;                   /**< SHA-512 hasher. Throws exception if libgcrypt is not configured. */
typedef HasherGcrypt<0> HasherCrc32;                    /**< ISO 3309 hasher. Throws exception if libgcrypt is not configured. */
#endif



/** Fowler-Noll-Vo hashing using the Hasher interface. */
class ROSE_DLL_API HasherFnv: public Hasher {
    uint64_t partial_;
public:
    HasherFnv(): partial_(0xcbf29ce484222325ull) {}
    const Digest& digest() override;
    void append(const uint8_t *message, size_t messageSize) override;
    uint64_t partial() const { return partial_; }
};

/** Built-in SHA-256 hasher.
 *
 *  This algorithm is built into ROSE and doesn't depend on any external libraries. */
class ROSE_DLL_API HasherSha256Builtin: public Hasher {
    static const uint32_t roundConstants_[64];          // statically-generated constants for the algorithm
    uint32_t state_[8];                                 // 256 bits of state information
    size_t processedBytes_;                             // number of message bytes hashed (excludes padding)
    std::vector<uint8_t> leftoverBytes_;                // message bytes inserted but not yet hashed
public:
    HasherSha256Builtin();
    void clear() override;
    const Digest& digest() override;
    void append(const uint8_t *message, size_t messageSize) override;
private:
    uint8_t messageByte(size_t index, const uint8_t *message, size_t messageSize);
    bool getNextChunk(const uint8_t* &message /*in,out*/, size_t &messageSize /*in,out*/, uint32_t words[16] /*out*/);
    void accumulateChunk(const uint32_t chunk[16]);
};

/** Convert two vectors to a vector of pairs.
 *
 *  If the two input vectors are not the same length, then the length of the result is the length of the shorter input vector. */
template<class T, class U>
std::vector<std::pair<T, U> >
zip(const std::vector<T> &first, const std::vector<U> &second) {
    size_t retvalSize = std::min(first.size(), second.size());
    std::vector<std::pair<T, U> > retval;
    retval.reserve(retvalSize);
    for (size_t i = 0; i < retvalSize; ++i)
        retval.push_back(std::pair<T, U>(first[i], second[i]));
    return retval;
}

/** Convert a vector of pairs to a pair of vectors. */
template<class T, class U>
std::pair<std::vector<T>, std::vector<U> >
unzip(const std::vector<std::pair<T, U> > &pairs) {
    std::pair<std::vector<T>, std::vector<U> > retval;
    retval.first.reserve(pairs.size());
    retval.second.reserve(pairs.size());
    for (size_t i = 0; i < pairs.size(); ++i) {
        retval.first.push_back(pairs[i].first);
        retval.second.push_back(pairs[i].second);
    }
    return retval;
}

} // namespace
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Prints a hash to a stream. See @ref Hasher::print. */
ROSE_DLL_API std::ostream& operator<<(std::ostream&, Rose::Combinatorics::Hasher&);

#endif
