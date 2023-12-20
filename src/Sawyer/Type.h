// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Type_H
#define Sawyer_Type_H

#include <boost/cstdint.hpp>

namespace Sawyer {

/** Data types. */
namespace Type {

/** A signed integer of particular size. */
template<size_t nBits> struct Integer {};
template<> struct Integer<8> { typedef boost::int8_t  type; };
template<> struct Integer<16> { typedef boost::int16_t type; };
template<> struct Integer<32> { typedef boost::int32_t type; };
template<> struct Integer<64> { typedef boost::int64_t type; };

/** An unsigned integer of particular size. */
template<size_t nBits> struct UnsignedInteger {};
template<> struct UnsignedInteger<8> { typedef boost::uint8_t  type; };
template<> struct UnsignedInteger<16> { typedef boost::uint16_t type; };
template<> struct UnsignedInteger<32> { typedef boost::uint32_t type; };
template<> struct UnsignedInteger<64> { typedef boost::uint64_t type; };

} // namespace
} // namespace

#endif
