#ifndef ROSE_BinaryAnalysis_RegisterParts_H
#define ROSE_BinaryAnalysis_RegisterParts_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Sawyer/IntervalSet.h>
#include <Sawyer/Map.h>
#include <boost/serialization/access.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Holds a set of registers without regard for register boundaries.
 *
 *  This class is a container to which registers can be inserted and erased in such a way that the boundaries of registers are
 *  irrelevant. For instance, if one constructs an empty container, then inserts the 16-bit x86 AX register, then removes the
 *  8-bit AH register (the high order eight bits of AX) all that's left is AL (the low-order 8 bits).
 *
 *  Sometimes this container contains parts of registers that don't correpond to actual named registers for the machine.  For
 *  instance, if we insert the x86 EFLAGS register and then erase the ZF bit flag we're left with two parts of EFLAGS that
 *  don't actually have names in the x86 documentation.  However, we can query whether another bit flag, say NF, is still
 *  present (which it is in this example).
 *
 *  One way to figure out names for the parts contained here is to get a list of all registers from a register dictionary, then
 *  query whether each is fully present in this container, report its name if present, and then remove its part from this
 *  container. If the container is non-empty at the end then all that can be done is to report which parts are still present. */
class RegisterParts {

private:
    typedef Sawyer::Container::Interval<size_t> BitRange;
    typedef Sawyer::Container::IntervalSet<BitRange> BitSet;

 // DQ (2/13/2017): Testing a simpler case.
 // BitSet XXX;

private:
    class MajorMinor {
        unsigned majr_, minr_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(majr_);
            s & BOOST_SERIALIZATION_NVP(minr_);
        }
#endif

    public:
        MajorMinor(): majr_(0), minr_(0) {}

        MajorMinor(RegisterDescriptor reg) /*implicit*/
            : majr_(reg.majorNumber()), minr_(reg.minorNumber()) {}

        bool operator<(const MajorMinor &other) const {
            return majr_ < other.majr_ || (majr_ == other.majr_ && minr_ < other.minr_);
        }

        unsigned get_major() const { return majr_; }    // "get_" works around Windows #define pollution
        unsigned get_minor() const { return minr_; }    // "get_" works around Windows #define pollution
    };

private:
 // DQ (2/13/2017): Test alternative formulation.
    typedef Sawyer::Container::Map<MajorMinor, BitSet> Map;

 // DQ (2/13/2017): Testing a simpler case.
 // typedef Sawyer::Container::Map<MajorMinor, BitSet > MapXXX;
 // MapXXX Map_YYY;

 // DQ (2/13/2017): Both of these work fine.
 // typedef Sawyer::Container::Map<MajorMinor, Sawyer::Container::IntervalSet< Sawyer::Container::Interval<size_t> > > Map;
 // typedef Sawyer::Container::Map<MajorMinor, Sawyer::Container::IntervalSet< BitRange > > Map;
    Map map_;


#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(map_);
    }
#endif
    
public:
    /** Default construct an object with no register parts. */
    RegisterParts() {}

    /** Constructor to insert a register.
     *
     *  This is the same as default-constructing an instance and inserting the specified register. */
    explicit RegisterParts(RegisterDescriptor reg) {
        insert(reg);
    }

    /** Predicate checking whether this container is empty.
     *
     *  Returns true if this container holds no part of any register. */
    bool isEmpty() const {
        return map_.isEmpty();
    }

    /** Predicate checking if part of a register is present.
     *
     *  Returns true if any part of @p reg is present in this container. The @p reg need not be entirely present. See also @ref
     *  existsAll. */
    bool existsAny(RegisterDescriptor reg) const {
        return map_.exists(reg) && map_[reg].isOverlapping(bitRange(reg));
    }

    /** Predicate checking if all of a register is present.
     *
     *  Returns true if all of @p reg is present in this container. It is not sufficient for just part of @p reg to be
     *  present. See also @ref existsAny. */
    bool existsAll(RegisterDescriptor reg) const {
        return map_.exists(reg) && map_[reg].contains(bitRange(reg));
    }

    /** Insert register into container.
     *
     *  Inserts @p reg into this container. After inserting, @ref existsAll and @ref existsAny will both return true for @p
     *  reg. Nothing happens if @p reg is already fully present in this container. */
    void insert(RegisterDescriptor reg) {
        map_.insertMaybeDefault(reg).insert(bitRange(reg));
    }

    /** Erase register from container.
     *
     *  Removes all of @p reg from this container.  After erasing, @ref existsAll and @ref existsAny will both return false for
     *  @p reg.  Nothing happens if @p reg is already fully absent from this container. */
    void erase(RegisterDescriptor reg);

    /** Erase everything.
     *
     *  All registers are removed from this container, which becomes empty as if it were freshly constructed. */
    void clear() {
        map_.clear();
    }

    /** Erase some register parts.
     *
     *  Erases from this container all register parts stored in the @p other container. */
    RegisterParts& operator-=(const RegisterParts &other);

    /** Compute difference.
     *
     *  Returns a new container that contains those register parts that are in @p this container but not in @p other. */
    RegisterParts operator-(const RegisterParts &other) const;

    /** Add some register parts.
     *
     *  Inserts parts stored in @p other into this container. */
    RegisterParts& operator|=(const RegisterParts &other);

    /** Compute the union.
     *
     *  Returns a new container that is the union of @p this container and @p other. */
    RegisterParts operator|(const RegisterParts &other) const;

    /** Erase some register parts.
     *
     *  Erases from this container all register parts that are not in the @p other container. */
    RegisterParts& operator&=(const RegisterParts &other);

    /** Compute the intersection.
     *
     *  Returns a new container containing only those parts that are in both @p this and @p other. */
    RegisterParts operator&(const RegisterParts &other) const;
    
    /** Extract individual registers.
     *
     *  Uses an optional register dictionary to extract individual register descriptors from this container, returning all of
     *  those that were found.  The dictionary is quered to obtain its register descriptors in decreasing size, and each
     *  register which is fully present in this container is added to the end of the return value and removed from this
     *  container.
     *
     *  If @p extractAll is true (or no register dictionary is specified) then all leftover parts from the prior step are
     *  appended to the return value and this container is cleared. */
    std::vector<RegisterDescriptor> extract(const RegisterDictionary *regDict = NULL, bool extractAll = false);

    /** List registers present.
     *
     *  This is similar to @ref extract except it doesn't modify this container.  The @ref listAll version returns a list of
     *  all possible registers regardless of whether they have entries in the supplied dictionary, while the @ref listNamed
     *  version returns only those that are present in the dictionary.
     *
     *  The register dictionary may be null for @ref listAll, in which case no attempt is made to divide consecutive bits into
     *  smaller parts that would have had names.
     *
     * @{ */
    std::vector<RegisterDescriptor> listAll(const RegisterDictionary*) const;
    std::vector<RegisterDescriptor> listNamed(const RegisterDictionary*) const;
    /** @} */


private:
    static BitRange bitRange(RegisterDescriptor reg) {
        ASSERT_forbid(reg.isEmpty());
        return BitRange::baseSize(reg.offset(), reg.nBits());
    }

};

} // namespace
} // namespace

#endif
#endif
