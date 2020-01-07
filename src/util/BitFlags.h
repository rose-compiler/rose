#ifndef Rose_BitFlags_H
#define Rose_BitFlags_H

#include <rosePublicConfig.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/serialization/access.hpp>
#include <Sawyer/Assert.h>
#include <vector>

namespace Rose {

/** Stores a vector of enum bit flags.
 *
 *  This is a more self-documenting and terse way to use bit flags. For example, consider this original code:
 *
 * @code
 *  class TableCharacter {
 *      enum Corner { NONE = 0, LEFT = 1, RIGHT = 2, TOP = 4, BOTTOM = 8 };
 *
 *      unsigned current; // bit vector of Corner flags for current character
 *      unsigned previous; // bit vector of Corner flags for previous character
 *
 *      TableCharacter(): current(NONE), previous(NONE) {}
 *
 *      void process(unsigned next) { // next is vector of Corner bit flags
 *          if ((current & LEFT) != 0) {
 *              previous |= LEFT;
 *              current &= ~LEFT;
 *          }
 *          if ((next & TOP) == 0) {
 *              current &= ~TOP;
 *              current |= BOTTOM;
 *          }
 *      }
 *  };
 * @endcode
 *
 *  Now rewritten to use this class:
 *
 * @code
 *  class TableCharacter {
 *      enum Corner { NONE = 0, LEFT = 1, RIGHT = 2, TOP = 4, BOTTOM = 8 };
 *
 *      BitFlags<Corner> current;
 *      BitFlags<Corner> previous;
 *
 *      TableCharacters(): current(NONE), previous(NONE) {}
 *
 *      void process(BitFlags<Corner> next) {
 *          if (current.testAndClear(LEFT))
 *              previous.set(LEFT);
 *          if (next.isClear(TOP))
 *              current.clear(TOP).set(BOTTOM);
 *      }
 *  };
 * @endcode */
template<typename E, typename V = int64_t>
class BitFlags {
public:
    typedef E Enum;
    typedef V Vector;

private:
    Vector vector_;

private:
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(vector_);
    }
#endif

public:
    /** Default constructor with all bits clear. */
    BitFlags()
        : vector_(0) {}

    /** Construct bit vector from value or bit. */
    BitFlags(Vector v) /*implicit*/
        : vector_(v) {}

    /** Current value of the bit vector. */
    Vector vector() const {
        return vector_;
    }

    /** Test whether a bit is set. */
    bool isSet(Enum e) const {
        return (vector_ & Vector(e)) != 0;
    }

    /** True if all specified bits are set. */
    bool isAllSet(BitFlags other) const {
        return (vector_ & other.vector_) == other.vector_;
    }

    /** True if any of the specified bits are set. */
    bool isAnySet(BitFlags other) const {
        return (vector_ & other.vector_) != 0;
    }

    /** True if any bit is set. */
    bool isAnySet() const {
        return vector_ != 0;
    }

    /** True if no bits are set. */
    bool isEmpty() const {
        return 0 == vector_;
    }

    /** Test whether a bit is clear. */
    bool isClear(Enum e) const {
        return !isSet(e);
    }

    /** Set the specified bit. */
    BitFlags& set(Enum e) {
        vector_ |= Vector(e);
        return *this;
    }

    /** Set all bits that are set in @p other. */
    BitFlags& set(BitFlags other) {
        vector_ |= other.vector_;
        return *this;
    }

    /** Clear the specified bit. */
    BitFlags& clear(Enum e) {
        vector_ &= ~Vector(e);
        return *this;
    }

    /** Clear all bits that are set in @p other. */
    BitFlags& clear(BitFlags other) {
        vector_ &= ~other.vector_;
        return *this;
    }

    /** Clear all bits. */
    BitFlags& clear() {
        vector_ = Vector(0);
        return *this;
    }

    /** Test whether a bit is set, then clear it. */
    bool testAndClear(Enum e) {
        bool retval = isSet(e);
        clear(e);
        return retval;
    }

    /** Test whether a bit is set, then set it. */
    bool testAndSet(Enum e) {
        bool retval = isSet(e);
        set(e);
        return retval;
    }

    /** Set the vector to an exact value. */
    BitFlags& operator=(Vector v) {
        vector_ = v;
        return *this;
    }

    /** Set the vector to the same as another. */
    BitFlags& operator=(BitFlags other) {
        vector_ = other.vector_;
        return *this;
    }

    /** Create a new vector that's the union of two vectors.
     *
     * @{ */
    BitFlags operator|(BitFlags other) const {
        return vector_ | other.vector_;
    }
    BitFlags operator|(Enum e) const {
        return vector_ | Vector(e);
    }
    /** @} */

    /** Create a new vector that's the intersection of two vectors.
     *
     * @{ */
    BitFlags intersection(BitFlags other) const {
        return vector_ & other.vector_;
    }
    BitFlags intersection(Enum e) const {
        return vector_ & Vector(e);
    }
    BitFlags operator&(BitFlags other) const {
        return intersection(other);
    }
    BitFlags operator&(Enum e) const {
        return intersection(e);
    }
    /** @} */

    /** Compare two vectors.
     *
     * @{ */
    bool operator==(BitFlags other) const {
        return vector_ == other.vector_;
    }
    bool operator!=(BitFlags other) const {
        return vector_ != other.vector_;
    }
    bool operator<(BitFlags other) const {
        return vector_ < other.vector_;
    }
    bool operator<=(BitFlags other) const {
        return vector_ <= other.vector_;
    }
    bool operator>(BitFlags other) const {
        return vector_ > other.vector_;
    }
    bool operator>=(BitFlags other) const {
        return vector_ >= other.vector_;
    }
    /** @} */

    /** Split a vector into the individual enum values.
     *
     *  The enum constants are first sorted so that those with more set bits appear before those with fewer bits. Then each
     *  constant is searched in the bit vector and those bits are removed. This continues until either no bits remain or no
     *  matching constant is found. The @p leftovers is set to those bits that could not be matched by this process. */
    std::vector<Enum> split(std::vector<int64_t> constants, Vector &leftovers /*out*/) const {
        leftovers = Vector(0);
        std::vector<Enum> retval;
        std::sort(constants.begin(), constants.end(), moreBits);
        Vector tmp = vector_;
        while (tmp) {
            bool found = false;
            for (size_t i=0; i<constants.size() && !found; ++i) {
                if (Vector(tmp & constants[i]) == Vector(constants[i])) {
                    retval.push_back(Enum(constants[i]));
                    tmp &= ~constants[i];
                    found = true;
                }
            }
            if (!found) {
                leftovers = tmp;
                tmp = 0;
            }
        }
        return retval;
    }

    /** Convert to string.
     *
     *  Converts a bit vector to a string of the form "NAME1|NAME2|...". The @p constants are the individual enum flags, and
     *  the @p stringifier is a function that converts each of those constants to strings. */
    std::string toString(std::vector<int64_t> constants, const char*(*stringifier)(int64_t)) const {
        std::string retval;
        if (vector_ != Vector(0)) {
            Vector leftovers(0);
            std::vector<Enum> members = split(constants, leftovers /*out*/);
            BOOST_FOREACH (Enum member, members) {
                const char *name = stringifier(member);
                ASSERT_not_null(name);
                retval += std::string(retval.empty()?"":"|") + name;
            }
            if (leftovers != Vector(0))
                retval += retval.empty()?"":"|" + (boost::format("%lx") % (unsigned long)leftovers).str();
        } else {
            if (const char* name = stringifier(0)) {
                retval = name;
            } else {
                retval = "0";
            }
        }
        return retval;
    }

private:
    static size_t nBits(Vector vector) {
        size_t retval = 0;
        for (size_t i = 0; i < 8*sizeof(Vector); ++i) {
            if ((vector & (Vector(1) << i)) != 0)
                ++retval;
        }
        return retval;
    }

    static bool moreBits(Vector a, Vector b) {
        return nBits(a) > nBits(b);
    }
};


} // namespace

#endif
