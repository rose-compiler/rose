#ifndef ROSE_SourceLocation_H
#define ROSE_SourceLocation_H

#include <boost/filesystem.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include <Sawyer/Optional.h>

namespace Rose {

/** Information about a source location.
 *
 *  A @ref SourceLocation object is simply a file name, line number, and optional column number. However, since file names are
 *  shared by many objects certain optimizations can be made to reduce the total amount of storage occupied by all the objects
 *  collectively.  Although the file name storage is optimized, the objects behave as if each object has its own file name data
 *  member.
 *
 *  This class attempts to correct a number of issues encountered with @ref Sg_File_Info and to make the class more suitable for
 *  use in binary analysis. Some of the corrected problems are:
 *
 *  @li The API is completely thread-safe and query operations are lock free.  The objects themselves are immutable since there
 *  are no mutating API functions.
 *
 *  @li Constructors are exception safe. A failure during construction no longer leaves the shared data structures in an
 *  inconsistent state.
 *
 *  @li File names are stored only once per unique name instead of twice per unique name.
 *
 *  @li Constructors are amortized constant time instead of O(log), and furthermore, there is only one lookup operation instead
 *  of two.
 *
 *  @li Querying the file name is O(1) instead of O(log).
 *
 *  @li File names are stored as @c boost::filesystem::path instead of @c std::string, therefore there is no need for constantly
 *  converting between the two types (good user code should not use std::string for file names).
 *
 *  @li Column numbers are officially optional. No more using some user-defined special value to indicate lack of a column
 *  number.
 *  
 *  @li Line and column numbers are type @c size_t since they're conceptually unsigned sizes.  This avoids having to cast in
 *  well-written user code.
 *
 *  @li This class exists in the Rose name space instead of the global name space.
 *
 *  @li The names used in this class are camelCase instead of a weird mixture of camelCase and underscores.  All names follow
 *  the ROSE Binary Analysis naming convention.  In particular, property accessors are nouns without a leading "get".
 *
 *  Line and column numbers are conventionally one-origin values, but nothing in this API or the implementation prevents
 *  you from storing line zero and/or column zero. You're free to use zero-origin number if you desire.
 *
 *  This class is intended to be extended through derivation to provide additional location features such as scope information, but those
 *  things are not included here because they're not always needed or even available. */
class SourceLocation {
    typedef boost::shared_ptr<boost::filesystem::path> NamePtr;
    NamePtr fileName_;                                  // null iff default constructed
    size_t line_;
    Sawyer::Optional<size_t> column_;

    
    // Shared index for file names
    struct NameHasher { size_t operator()(const NamePtr&) const; };
    struct NameEquivalence { bool operator()(const NamePtr&, const NamePtr&) const; };
    typedef boost::unordered_set<NamePtr, NameHasher, NameEquivalence> FileNames;
    static SAWYER_THREAD_TRAITS::Mutex classMutex_;     // protects the following static data members
    static FileNames fileNames_;

private:
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, unsigned version) {
        s & BOOST_SERIALIZATION_NVP(fileName_);
        s & BOOST_SERIALIZATION_NVP(line_);
        s & BOOST_SERIALIZATION_NVP(column_);
        registerFileName();                             // necessary for de-serialization; okay to also call for serialization
    }
#endif

public:
    /** Default constructor.
     *
     *  The default constructor creates a location with an empty name, line zero, and no column. It's intended mainly for use
     *  in containers that require a default constructor. */
    SourceLocation(): line_(0) {}

    /** Construct a new source location.
     *
     *  The behavior of this constructor is as if the file name, line, and column were all copied into data members of the new
     *  object. However, the implementation is optimized to store only one copy of each unique file name across all objects. */
    SourceLocation(const boost::filesystem::path &fileName, size_t line, const Sawyer::Optional<size_t> &column = Sawyer::Nothing());

    /** Construct a new source location by parsing a string.
     *
     *  If the string ends with ":N:M" then N and M are the line and column numbers. Otherwise, if the string ends with ":N"
     *  then it has only a line number and no column number. Otherwise the line number is zero and there is no column number.
     *  Everything before the line and column is the file name. If the file name is surrounded by double quotes then the entire
     *  name is parsed as if it were a C string literal. */
    static SourceLocation parse(const std::string&);

    /** Destructor.
     *
     *  A special destructor is used in order to free file names that are no longer referenced by any @ref SourceLocation object. */
    ~SourceLocation();

    /** Test whether object is empty.
     *
     *  This object is empty if and only if it was default constructed. A non-default constructed object having an empty file name is
     *  not considered to be an empty object. This is useful for distinguishing default constructed objects from those that indicate
     *  a present but unknown location. */
    bool isEmpty() const;
    
    /** File name associated with this location.
     *
     *  The behavior is as if the file name were stored as a data member of this object; that is, the reference is valid as
     *  long as this object is valid.  The actual implementation optimizes name storage and therefore the reference is likely
     *  (but not guaranteed) to be valid longer than the life time of this object.
     *
     *  Thread safety: The referenced name is guaranteed to not change for its entire lifetime. */
    const boost::filesystem::path& fileName() const;

    /** Line number. */
    size_t line() const {
        return line_;                                   // no lock necessary since *this is immutable
    }

    /** Column number. */
    const Sawyer::Optional<size_t>& column() const {
        return column_;                                 // no lock necessary since *this is immutable
    }
    
    /** Equality and inequality.
     *
     *  Two objects are equal if they have the same file name (exact match), same line number, and either the same column
     *  number or both have no column.
     *
     * @{ */
    bool operator==(const SourceLocation &other) const;
    bool operator!=(const SourceLocation &other) const {
        return !(*this == other);
    }
    /** @} */

    /** Ordered comparison.
     *
     *  Compares by file name, line number, and column number, in that order.  Although the sort is stable, file names are not
     *  compared lexicographically. That is, the sort will not be alphabetical -- if you want an alphabetical sort then you'll
     *  need to provide a different comparator. A location with a non-existing column number is considered less than a location
     *  with column number zero.
     *
     * @{ */
    int compare(const SourceLocation &other) const;
    bool operator<(const SourceLocation &other) const {
        return compare(other) < 0;
    }
    bool operator<=(const SourceLocation &other) const {
        return compare(other) <= 0;
    }
    bool operator>(const SourceLocation &other) const {
        return compare(other) > 0;
    }
    bool operator>=(const SourceLocation &other) const {
        return compare(other) >= 0;
    }
    /** @} */

    /** Convert location to string.
     *
     *  Converts this location to a file name, line number, and optional column number. Special characters in the file name are
     *  not escaped nor is the file name enclosed in quotes. The file name is separated from the line number by a colon (no
     *  white space), and if the location has a column, the column number is separated from the line number by a colon (also no
     *  white space).   An empty (default constructed) object returns an empty string. */
    std::string toString() const;

    /** Convert location to escaped string.
     *
     *  Prints the location in a safe manner by printing the file name as a C-style string literal (with double quotes) and with
     *  all non-graphic characters except ASCII SPC escaped using only graphic characters (the usual C syntax). An empty (default
     *  constructed) object returns an empty string. */
    std::string printableName() const;

    /** Output location to a stream.
     *
     *  The format is the same as the @ref toString method. */
    void print(std::ostream&) const;

    // The following trickery is to allow things like "if (x)" to work but without having an implicit
    // conversion to bool which would cause no end of other problems. This is fixed in C++11.
private:
    typedef void(SourceLocation::*unspecified_bool)() const;
    void this_type_does_not_support_comparisons() const {}
public:
    /** Type for Boolean context.
     *
     *  Implicit conversion to a type that can be used in a boolean context such as an <code>if</code> or <code>while</code>
     *  statement. The value in Boolean context is the inverse of what @ref isEmpty would return. */
    operator unspecified_bool() const {
        return isEmpty() ? 0 : &SourceLocation::this_type_does_not_support_comparisons;;
    }

private:
    // When reconstituting an object from a serialization, we need to make sure the file name is an element of the shared index.
    // It doesn't hurt to call this on any object at any time.
    void registerFileName();
};

std::ostream& operator<<(std::ostream&, const SourceLocation&);

} // namespace

#endif
