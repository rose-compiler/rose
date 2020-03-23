#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_SValue_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_SValue_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemanticsTypes.h>
#include <BinarySmtSolver.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic Values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is leftover for compatibility with an older API.  The old API had code like this:
//    User::SValue user_svalue = BaseSemantics::dynamic_pointer_cast<User::SValue>(base_svalue);
// Which can be replaced now with
//    User::SValue user_svalue = base_svalue.dynamicCast<User::SValue>();
template<class To, class From>
Sawyer::SharedPointer<To> dynamic_pointer_cast(const Sawyer::SharedPointer<From> &from) {
    return from.template dynamicCast<To>();
}

/** Base class for semantic values.
 *
 *  A semantic value represents a datum from the specimen being analyzed. The datum could be from memory, it could be something
 *  stored in a register, it could be the result of some computation, etc.  The datum in the specimen has a datum type that
 *  might be only partially known; the datum value could, for instance, be 32-bits but unknown whether it is integer or
 *  floating point.
 *
 *  The various semantic domains will define SValue subclasses that are appropriate for that domain--a concrete domain will
 *  define an SValue that specimen data in a concrete form, an interval domain will define an SValue that represents specimen
 *  data in intervals, etc.
 *
 *  Semantics value objects are allocated on the heap and reference counted.  The BaseSemantics::SValue is an abstract class
 *  that defines the interface.  See the Rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts
 *  fit together.*/
class SValue: public Sawyer::SharedObject, public Sawyer::SharedFromThis<SValue>, public Sawyer::SmallObject {
protected:
    size_t width;                               /** Width of the value in bits. Typically (not always) a power of two. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(width);
    }
#endif
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Normal, protected, C++ constructors
protected:
    SValue(): width(0) {}                               // needed for serialization
    explicit SValue(size_t nbits): width(nbits) {}      // hot
    SValue(const SValue &other): Sawyer::SharedObject(other), width(other.width) {}

public:
    /** Shared-ownership pointer for an @ref SValue object. See @ref heap_object_shared_ownership. */
    typedef SValuePtr Ptr;

public:
    virtual ~SValue() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Allocating static constructor.  None are needed--this class is abstract.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Allocating virtual constructors.  undefined_() needs underscores, so we do so consistently for all
    // these allocating virtual c'tors.  However, we use copy() rather than copy_() because this one is fundamentally
    // different: the object (this) is use for more than just selecting which virtual method to invoke.
    //
    // The naming scheme we use here is a bit different than for most other objects for historical reasons.  Most other classes
    // use "create" and "clone" as the virtual constructor names, but SValue uses names ending in undercore, and "copy". The
    // other difference (at least in this base class) is that we don't define any real constructors or static allocating
    // constructors (usually named "instance")--it's because this is an abstract class.
public:
    /** Create a new undefined semantic value.  The new semantic value will have the same dynamic type as the value
     *  on which this virtual method is called.  This is the most common way that a new value is created. The @ref unspecified_
     *  method is closely related.
     *
     *  @sa unspecified_ */
    virtual SValuePtr undefined_(size_t nbits) const = 0; // hot

    /** Create a new unspecified semantic value. The new semantic value will have the same dynamic type as the value on which
     *  this virtual method is called.  Undefined (@ref undefined_) and unspecified are closely related.  Unspecified values
     *  are the same as undefined values except they're instantiated as the result of some machine instruction where the ISA
     *  documentation indicates that the value is unspecified (e.g., status flags for x86 shift and rotate instructions).
     *
     *  Most semantic domains make no distinction between undefined and unspecified.
     *
     *  @sa undefined_ */
    virtual SValuePtr unspecified_(size_t nbits) const = 0;

    /** Data-flow bottom value.
     *
     *  Returns a new value that represents bottom in a data-flow analysis. If a semantic domain can represent a bottom value
     *  then the @ref isBottom predicate is true when invoked on this method's return value. If a semantic domain cannot
     *  support a bottom value, then it may return some other value. */
    virtual SValuePtr bottom_(size_t nBits) const = 0;

    /** Create a new concrete semantic value. The new value will represent the specified concrete value and have the same
     *  dynamic type as the value on which this virtual method is called. This is the most common way that a new constant is
     *  created.  The @p number is truncated to contain @p nbits bits (higher order bits are cleared). */
    virtual SValuePtr number_(size_t nbits, uint64_t number) const = 0; // hot

    /** Create a new, Boolean value. The new semantic value will have the same dynamic type as the value on
     *  which this virtual method is called. This is how 1-bit flag register values (among others) are created. The base
     *  implementation uses number_() to construct a 1-bit value whose bit is zero (false) or one (true). */
    virtual SValuePtr boolean_(bool value) const { return number_(1, value?1:0); }

    /** Create a new value from an existing value, changing the width if @p new_width is non-zero. Increasing the width
     *  logically adds zero bits to the most significant side of the value; decreasing the width logically removes bits from the
     *  most significant side of the value. */
    virtual SValuePtr copy(size_t new_width=0) const = 0;

    /** Possibly create a new value by merging two existing values.
     *
     *  This method optionally returns a new semantic value as the data-flow merge of @p this and @p other.  If the two inputs
     *  are "equal" in some sense of the dataflow implementation then nothing is returned, otherwise a new value is returned.
     *  Typical usage is like this:
     *
     * @code
     *  if (SValuePtr merged = v1->createOptionalMerge(v2).orDefault()) {
     *      std::cout <<"v1 and v2 were merged to " <<*merged <<"\n";
     *  } else {
     *      std::cout <<"no merge is necessary\n";
     *  }
     *
     *  or
     *
     * @code
     *  SValuePtr merge;
     *  if (v1->createOptionalMerge(v2).assignTo(merged)) {
     *      std::cout <<"v1 and v2 were merged to " <<*merged <<"\n";
     *  } else {
     *      std::cout <<"v1 and v2 are equal in some sense (no merge necessary)\n";
     *  }
     * @endcode
     *
     *  If you always want a copy regardless of whether the merge is necessary, then use the @ref createMerged convenience
     *  function instead. */
    virtual Sawyer::Optional<SValuePtr>
    createOptionalMerge(const SValuePtr &other, const MergerPtr &merger, const SmtSolverPtr &solver) const = 0;

    /** Create a new value by merging two existing values.
     *
     *  This is a convenience wrapper around @ref createOptionalMerge. It always returns a newly constructed semantic value
     *  regardless of whether a merge was necessary.  In order to determine if a merge was necessary one can compare the
     *  return value to @p this using @ref must_equal, although doing so is more expensive than calling @ref
     *  createOptionalMerge. */
    SValuePtr createMerged(const SValuePtr &other, const MergerPtr &merger, const SmtSolverPtr &solver) const /*final*/ {
        return createOptionalMerge(other, merger, solver).orElse(copy());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts. No-ops since this is the base class
public:
    static SValuePtr promote(const SValuePtr &x) {
        ASSERT_not_null(x);
        return x;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The rest of the API...
public:
    /** Determines whether a value is a data-flow bottom.
     *
     *  Returns true if this value represents a bottom value for data-flow analysis.  Any RiscOperation performed on an operand
     *  whose isBottom predicate returns true will itself return a bottom value.  This includes operations like "xor x x" which
     *  would normally return zero. */
    virtual bool isBottom() const = 0;

    /** Determines if the value is a concrete number. Concrete numbers can be created with the number_(), boolean_()
     *  virtual constructors, or by other means. */
    virtual bool is_number() const = 0;

    /** Return the concrete number for this value.  Only values for which is_number() returns true are able to return a
     *  concrete value by this method. */
    virtual uint64_t get_number() const = 0;

    /** Accessor for value width.
     * @{ */
    virtual size_t get_width() const { return width; }
    virtual void set_width(size_t nbits) { width = nbits; }
    /** @} */

    /** Returns true if two values could be equal. The SMT solver is optional for many subclasses. */
    virtual bool may_equal(const SValuePtr &other, const SmtSolverPtr &solver = SmtSolverPtr()) const = 0;

    /** Returns true if two values must be equal.  The SMT solver is optional for many subclasses. */
    virtual bool must_equal(const SValuePtr &other, const SmtSolverPtr &solver = SmtSolverPtr()) const = 0;

    /** Returns true if concrete non-zero. This is not virtual since it can be implemented in terms of @ref is_number and @ref
     *  get_number. */
    bool isTrue() const {
        return is_number() && get_number()!=0;
    }

    /** Returns true if concrete zero.  This is not virtual since it can be implemented in terms of @ref is_number and @ref
     *  get_number. */
    bool isFalse() const {
        return is_number() && get_number()==0;
    }

    /** Print a value to a stream using default format. The value will normally occupy a single line and not contain leading
     *  space or line termination.  See also, with_format().
     *  @{ */
    void print(std::ostream&) const;
    virtual void print(std::ostream&, Formatter&) const = 0;
    /** @} */

    /** SValue with formatter. See with_formatter(). */
    class WithFormatter {
        SValuePtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const SValuePtr &svalue, Formatter &fmt): obj(svalue), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing values with formatting. The usual way to use this is:
     * @code
     *  SValuePtr val = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*val+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(SValuePtr(this), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    WithFormatter operator+(const std::string &linePrefix);
    /** @} */
    
    /** Some subclasses support the ability to add comments to values. We define no-op versions of these methods here
     *  because it makes things easier.  The base class tries to be as small as possible by not storing comments at
     *  all. Comments should not affect any computation (comparisons, hash values, etc), and therefore are allowed to be
     *  modified even for const objects.
     * @{ */
    virtual std::string get_comment() const { return ""; }
    virtual void set_comment(const std::string&) const {} // const is intended; cf. doxygen comment
    /** @} */
};

std::ostream& operator<<(std::ostream&, const SValue&);
std::ostream& operator<<(std::ostream&, const SValue::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
