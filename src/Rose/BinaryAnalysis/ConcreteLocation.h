#ifndef ROSE_BinaryAnalysis_ConcreteLocation_H
#define ROSE_BinaryAnalysis_ConcreteLocation_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>
#include <Rose/Location.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#endif

#include <ostream>
#include <Sawyer/Optional.h>
#include <string>

namespace Rose {
namespace BinaryAnalysis {

/** Concrete location of data.
 *
 *  A concrete location is a machine register, a concrete memory address, or a memory address calculated
 *  from a register's current value and a signed byte offset.
 *
 *  Concrete locations are immutable objects.
 *
 *  See also, @ref AbstractLocation. */
class ConcreteLocation: public Location {
public:
    /** Type of location. */
    enum Type {
        NO_LOCATION,                                    /**< Used by default-constructed locations. */
        REGISTER,                                       /**< Parameter is in a register. */
        RELATIVE,                                       /**< Parameter in memory relative to a register. E.g., stack. */
        ABSOLUTE                                        /**< Parameter is at a fixed memory address. */
    };

private:
    RegisterDescriptor reg_;
    Sawyer::Optional<uint64_t> va_;
    RegisterDictionaryPtr regdict_;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
    friend class boost::serialization::access;
    template<class S> void serialize(S&, unsigned);
#endif

public:
    /** Default constructor.
     *
     * Constructs a concrete location that does not refer to any location. The @ref isEmpty method return true for such
     * objects. */
    ConcreteLocation();
    virtual ~ConcreteLocation();

    /** Copy constructor. */
    ConcreteLocation(const ConcreteLocation&);
    ConcreteLocation& operator=(const ConcreteLocation&);

    /** Register location.
     *
     *  Construct a concrete location that refers to a register.  The optional register dictionary can be associated with the
     *  location and is only used when printing the location.
     *
     *  @{ */
    explicit ConcreteLocation(RegisterDescriptor);
    explicit ConcreteLocation(RegisterDescriptor, const RegisterDictionaryPtr&);
    /** @} */

    /** Address location.
     *
     *  Construct a concrete location that refers to an address in memory. */
    explicit ConcreteLocation(Address);

    /** Register and offset location.
     *
     *  Construct a concrete location that refers to an address in memory calculated by adding a register's value and a signed byte
     *  offset. The optional register dictionary can be associated with the location and is only used when printing the location.
     *
     * @{ */
    ConcreteLocation(RegisterDescriptor, int64_t offset);
    ConcreteLocation(RegisterDescriptor, int64_t offset, const RegisterDictionaryPtr&);
    /** @} */

    /** Parse a concrete location from a string. */
    static ConcreteLocation parse(const std::string&);

    // Inherited methods
    virtual bool isValid() const override;
    virtual std::string toString() const override;
    virtual void print(std::ostream&) const override;
    virtual std::string printableName() const override;
    virtual bool isEqual(const Location&) const override;
    virtual bool operator<(const Location&) const override;
    virtual bool operator<=(const Location&) const override;
    virtual bool operator>(const Location&) const override;
    virtual bool operator>=(const Location&) const override;

    /** Print using the specified register dictionary.
     *
     *  Concrete locations have an optional internal register dictionary pointer that's usually used for printing, but this
     *  method provides a way to use some other register dictionary. */
    void print(std::ostream&, const RegisterDictionaryPtr&) const;

    /** Compare two concrete locations.
     *
     *  Returns negative if this location is less than the @p other, positive if this location is greater than the @p other, or
     *  zero if this and the @p other location are equal. */
    int compare(const ConcreteLocation&) const;

    /** Type of location. */
    Type type() const;

    /** Property: Register dictionary.
     *
     *  This is the optional register dictionary associated with this location and used when printing the location.
     *
     * @{ */
    RegisterDictionaryPtr registerDictionary() const;
    void registerDictionary(const RegisterDictionaryPtr&);
    /** @} */

    /** Register part of the location.
     *
     *  Returns the register part of the location. This returns a valid register descriptor if the location is either a register
     *  or an indirect (register plus offset) memory address. Otherwise it returns the empty register descriptor. */
    RegisterDescriptor reg() const;

    /** Address part of the location.
     *
     *  Returns the address of the location if the location is a simple memory address. Otherwise, if the address is formed
     *  from a base register and an offset, or if the location is a register, then zero is returned. */
    Address address() const;

    /** Offset part of the location.
     *
     *  Returns the signed offset when the location is a memory address formed from a register base value and an
     *  offset. Otherwise it returns zero. */
    int64_t offset() const;
};

} // namespace
} // namespace

#endif
#endif
