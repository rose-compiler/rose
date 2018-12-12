#ifndef Rose_BitFlags_H
#define Rose_BitFlags_H

#include <boost/format.hpp>
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
template<typename E, typename V = unsigned long>
class BitFlags {
public:
    typedef E Enum;
    typedef V Vector;

private:
    Vector vector_;

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

    /** Test whether a bit is clear. */
    bool isClear(Enum e) const {
        return !isSet(e);
    }

    /** Set the specified bit. */
    BitFlags& set(Enum e) {
        vector_ |= Vector(e);
        return *this;
    }

    /** Clear the specified bit. */
    BitFlags& clear(Enum e) {
        vector_ &= ~Vector(e);
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
    
    /** Convert to string.
     *
     *  Converts a bit vector to a string of the form "NAME1|NAME2|...". The @p constants are the individual enum flags, and
     *  the @p stringifier is a function that converts each of those constants to strings. */
    std::string toString(std::vector<long> constants, const char*(*stringifier)(long)) const {
        std::string retval;
        Vector tmp = vector_;
        std::sort(constants.begin(), constants.end(), moreBits);
        while (tmp) {
            bool found = false;
            for (size_t i=0; i<constants.size() && !found; ++i) {
                if (Vector(tmp & constants[i]) == Vector(constants[i])) {
                    const char *name = stringifier(constants[i]);
                    ASSERT_not_null(name);
                    retval += std::string(retval.empty()?"":"|") + name;
                    tmp &= ~constants[i];
                    found = true;
                }
            }
            if (!found) {
                retval += retval.empty()?"":"|" + (boost::format("%lx") % (unsigned long)tmp).str();
                tmp = 0;
                break;
            }
        }

        if (!vector_) {
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
