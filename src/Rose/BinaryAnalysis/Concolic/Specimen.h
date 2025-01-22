#ifndef ROSE_BinaryAnalysis_Concolic_Specimen_H
#define ROSE_BinaryAnalysis_Concolic_Specimen_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <boost/filesystem.hpp>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#endif

#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Binary specimen.
 *
 *  The specimen represents the thing that is to be tested, but not how to test it.  In other words, a specimen might be an
 *  executable, but not any specifics about how to run the executable such as which arguments to give it. */
class Specimen: public Sawyer::SharedObject, public Sawyer::SharedFromThis<Specimen> {
public:
    /** Referenc-counting pointer to a @ref Specimen. */
    typedef Sawyer::SharedPointer<Specimen> Ptr;

    /** Binary data, such as for the specimen content. */
    typedef std::vector<uint8_t> BinaryData;

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    std::string name_;                                  // name of specimen (e.g., for debugging)
    std::string timestamp_;                             // time of creation
    BinaryData content_;                                // content of the binary executable file
    mutable bool read_only_;                            // safe guards from writing content after it has been shared;
    bool empty_;                                        // indicates if this object is empty

private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(name_);
        s & BOOST_SERIALIZATION_NVP(content_);
        s & BOOST_SERIALIZATION_NVP(empty_);
    }

protected:
    Specimen();

public:
    ~Specimen();

    /** Allocating constructor.
     * @{ */
    static Ptr instance(const boost::filesystem::path &executableName);
    static Ptr instance();
    /** @} */

    /** Open an executable file.
     *
     *  The contents of the executable file are copied into this object when the file is opened.  After the file is opened
     *  successfully, the @ref isEmpty predicate returns false.  Throws an @ref Exception if the file cannot be opened.
     *
     *  Thread safety: This method is thread-safe. */
    void open(const boost::filesystem::path &executableName);

    /** Close the executable file.
     *
     *  Removes all content from this object, or does nothing if @ref isEmpty returns true. After this call, @ref isEmpty
     *  returns true.
     *
     *  Thread safety: This method is thread-safe. */
    void close();

    /** Test whether this object is empty.
     *
     *  Returns true if no binary executable is associated with this specimen, false otherwise. See also, @ref open and @ref close.
     *
     *  Thread safety: This method is thread-safe. */
    bool isEmpty() const;

    /** Property: Specimen name.
     *
     *  This should be a printable name, such as a file name. It's used mostly for informational purposes such as
     *  debugging. There is no requirement that names be unique or non-empty or contain only printable characters.
     *
     *  Thread safety: This methods is thread-safe.
     *
     * @{ */
    std::string name() const;                           // value return is intentional for thread safety
    void name(const std::string&);
    /** @} */

    /** Returns printable name of specimen for diagnostic output.
     *
     *  Returns a string suitable for printing to a terminal, containing the word "specimen", the database ID if appropriate,
     *  and the specimen name using C-style double-quoted string literal syntax if not empty.  The database ID is shown if a
     *  non-null database is specified and this specimen exists in that database. */
    std::string printableName(const DatabasePtr &db = DatabasePtr());

    /** Print as a YAML node. */
    void toYaml(std::ostream&, const DatabasePtr&, std::string prefix);

    /** Property: Database creation timestamp string.
     *
     *  Time stamp string describing when this object was created in the database, initialized the first time the object is
     *  written to the database. If a value is assigned prior to writing to the database, then the assigned value is used
     *  instead. The value is typically specified in ISO-8601 format (except a space is used to separate the date and time for
     *  better readability, as in RFC 3339). This allows dates to be sorted chronologically as strings.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    std::string timestamp() const;
    void timestamp(const std::string&);
    /** @} */

    /** Property: Specimen content.
     *
     *  This property contains the bytes that compose a specimen. For instance, for an ELF executable, the content is the bytes
     *  that compose the executable file.  This property is read-only, initialized when the @ref Specimen is created.
     *
     *  Thread safety: This method is not thread safe since it returns a reference. */
    const std::vector<uint8_t>& content() const;

    // FIXME[Robb Matzke 2019-08-12]: content is read-only, created by constructor. Therefore this member shouldn't be defined,
    // or at least should be private.
    /** The setter helps to conveniently populate a Specimen's properties
     *  from a database query. */
    void content(std::vector<uint8_t> binary_data);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
