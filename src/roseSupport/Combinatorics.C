#include <sage3basic.h>
#include <Combinatorics.h>

namespace Rose {
namespace Combinatorics {

ROSE_UTIL_API bool
flip_coin()
{
    static LinearCongruentialGenerator rng;
    return 0 == (rng() & 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hasher base class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
Hasher::toString(const Digest &digest) {
    std::string str;
    for (size_t i=0; i<digest.size(); ++i) {
        str += "0123456789abcdef"[(digest[i] >> 4) & 0xf];
        str += "0123456789abcdef"[digest[i] & 0xf];
    }
    return str;
}

std::string
Hasher::toString() {
    return toString(digest());
}

void
Hasher::print(std::ostream &out) {
    out <<toString(digest());
}

ROSE_UTIL_API std::string digest_to_string(const uint8_t *data, size_t size) {
    std::vector<uint8_t> digest(data+0, data+size);
    return Hasher::toString(digest);
}

ROSE_UTIL_API std::string
digest_to_string(const std::vector<uint8_t> &data) {
    return Hasher::toString(data);
}

ROSE_UTIL_API std::string
digest_to_string(const std::string &data) {
    const char *s = data.c_str();
    std::vector<uint8_t> digest(s, s + data.size());
    return Hasher::toString(digest);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHA1 hashing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ROSE_UTIL_API std::vector<uint8_t>
sha1_digest(const std::vector<uint8_t> &data) {
    HasherSha1 hasher;
    if (!data.empty())
        hasher.append(&data[0], data.size());
    return hasher.digest();
}

ROSE_UTIL_API std::vector<uint8_t>
sha1_digest(const std::string &data) {
    HasherSha1 hasher;
    hasher.insert(data);
    return hasher.digest();
}

ROSE_UTIL_API std::vector<uint8_t>
sha1_digest(const uint8_t *data, size_t size) {
    HasherSha1 hasher;
    hasher.append(data, size);
    return hasher.digest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fowler-Noll-Vo hashing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Hasher::Digest&
HasherFnv::digest() {
    if (digest_.empty()) {
        ASSERT_require(8 == sizeof partial_);
        digest_.resize(8, 0);
        for (size_t i=0; i<8; ++i)
            digest_[i] = (partial_ >> ((7-i)*8)) & 0xff;
    }
    return Hasher::digest();
}

void
HasherFnv::append(const uint8_t *message, size_t messageSize) {
    ASSERT_require(message || 0==messageSize);
    if (!digest_.empty())
        throw Exception("cannot append after returning digest");
    if (messageSize > 0)
    for (size_t i=0; i<messageSize; ++i)
        partial_ = (partial_ ^ message[i]) * 0x100000001b3ull;
}

ROSE_UTIL_API uint64_t
fnv1a64_digest(const std::vector<uint8_t> &data) {
    HasherFnv hasher;
    if (!data.empty())
        hasher.append(&data[0], data.size());
    return hasher.partial();
}

ROSE_UTIL_API uint64_t
fnv1a64_digest(const std::string &data) {
    HasherFnv hasher;
    hasher.insert(data);
    return hasher.partial();
}

ROSE_UTIL_API uint64_t
fnv1a64_digest(const uint8_t *data, size_t size) {
    HasherFnv hasher;
    hasher.append(data, size);
    return hasher.partial();
}

} // namespace
} // namespace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream &out, Rose::Combinatorics::Hasher &hasher) {
    hasher.print(out);
    return out;
}

