#ifndef Rose_BitFlags_H
#define Rose_BitFlags_H

#include <rosePublicConfig.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/serialization/access.hpp>
#include <Sawyer/Assert.h>
#include <Sawyer/BitFlags.h>
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
class BitFlags: public Sawyer::BitFlags<E, V> {
public:
    typedef E Enum;
    typedef V Vector;

    /** Default constructor with all bits clear. */
    BitFlags()
        : Sawyer::BitFlags<E, V>(0) {}

    /** Construct bit vector from value or bit. */
    BitFlags(Vector v) /*implicit*/
        : Sawyer::BitFlags<E, V>(v) {}

    BitFlags(Sawyer::BitFlags<E, V> bf) /*implicit*/
        : Sawyer::BitFlags<E, V>(bf) {}
    
    /** Convert to string.
     *
     *  Converts a bit vector to a string of the form "NAME1|NAME2|...". The @p constants are the individual enum flags, and
     *  the @p stringifier is a function that converts each of those constants to strings. */
    std::string toString(std::vector<int64_t> constants, const char*(*stringifier)(int64_t)) const {
        std::string retval;
        if (this->vector() != Vector(0)) {
            Vector leftovers(0);
            std::vector<Enum> members = this->split(constants, leftovers /*out*/);
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
};


} // namespace

#endif
