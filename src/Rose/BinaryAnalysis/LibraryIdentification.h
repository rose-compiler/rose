#ifndef ROSE_BinaryAnalysis_LibraryIdentification_H
#define ROSE_BinaryAnalysis_LibraryIdentification_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION

#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Sawyer/CommandLine.h>
#include <Sawyer/Database.h>
#include <Sawyer/Message.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

#include <ctime>
#include <regex>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {

/** Identify libraries by function recognition.
 *
 *  This analysis has two parts:
 *
 *  @li The functions of a specimen (presumably a library) are measured and identifying characteristics are stored in a
 *  database.
 *
 *  @li The functions of a specimen (presumably a statically linked executable) are measured and compared with functions
 *  previously stored in a database.
 *
 *  A database can store information about multiple libraries and their functions. */
class LibraryIdentification {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Settings that affect the analysis as a whole. */
    struct Settings {
        /** Names of functions to select.
         *
         *  Any function whose mangled or demangled name exactly matches a string in this list is not ignored when inserting
         *  or matching, regardless of whether any of the exclusion predicates apply. */
        Sawyer::Container::Set<std::string> includeNames;

        /** Name patterns of functions to select.
         *
         *  If this list is non-empty and a function's mangled or demangled name matches one of these patterns, then the
         *  function is not ignored when inserting or matching, regardless of whether it matches one of the exclusion
         *  criteria. */
        std::vector<std::regex> includeRes;

        /** Name of a file that lists function names to select.
         *
         *  If non-empty, the specified file is read and lines whose first non-space character is "#", or which are empty,
         *  or which contain only white space are discarded. Remaining lines are function names. If a function's mangled or
         *  demangled name matches any of these lines then the function is not ignored when inserting or matching, regardless
         *  of whether it matches one of the exclusion criteria. */
        boost::filesystem::path includeFile;

        /** Names of functions to ignore.
         *
         *  Any function whose mangled or demangled name exactly matches a string in this list will be removed from
         *  consideration. This exclusion predicate does not apply to functions that match an inclusion predicate above. */
        Sawyer::Container::Set<std::string> excludeNames;

        /** Names of functions to ignore.
         *
         *  This is a list of regular expressions. If a function has a mangled or demangled name that matches the regular
         *  expression then the function is removed from consideration. This exclusion predicate does not apply to functions
         *  that match an inclusion predicate. */
        std::vector<std::regex> excludeRes;

        /** Name of a file that lists function names to ignore.
         *
         *  If non-empty, the specified file is read and lines whose first non-space character is '#', or which are empty,
         *  or which contain only white space are discarded. Remaining lines are function names. If a function's mangled or
         *  demangled name matches any of these lines then the function is removed from consideration for inserting or
         *  matching unless the function is already selected by an inclusion criteria above. */
        boost::filesystem::path excludeFile;

        /** Minimum size of significant functions.
         *
         *  Any function that has fewer than this many instructions is ignored when inserting or matching. This exclusion
         *  predicate does not apply to functions that match an inclusion predicate. */
        size_t minFunctionInsns = 0;

        /** Key functions by hash instead of library address.
         *
         *  Functions are stored in a database according to their key. Version zero of the database used the function's hash as its
         *  key, which meant that smaller functions which often hash to the same value could not be distinguished from one another.
         *  After version zero, the database uses a pair of values as the key: the library to which the function belongs, and the
         *  entry address for the function.
         *
         *  If this data member is set, then the function hash will be used as the identification key instead of using the library
         *  and starting address. */
        bool identifyFunctionsByHash = false;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Libraries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Information about a library. */
    class Library: public Sawyer::SharedObject {
    public:
        /** Shared ownership pointer. */
        using Ptr = Sawyer::SharedPointer<Library>;

    private:
        std::string hash_;                              // required unique key for library
        std::string name_;                              // required name of library
        std::string version_;                           // optional version of library
        std::string architecture_;                      // optional architecture string
        time_t ctime_ = 0;                              // Unix time at which this object was created; zero if unknown
        std::string cversion_;                          // ROSE version which created this object; empty if unknown

    protected:
        Library(const std::string &hash, const std::string &name, const std::string &version, const std::string &architecture,
                time_t ctime, const std::string &cversion);

    public:
        /** Allocating constructor.
         *
         *  The arguments are the same as the various properties documented for this class. The creation time and creation
         *  ROSE version are set to the current time and version, thus this factory is useful for creating objects in memory
         *  that are not backed by a database. */
        static Ptr instance(const std::string &hash, const std::string &name, const std::string &version,
                            const std::string &architecture);

        /** Allocating constructor.
         *
         *  The arguments are the same as the various properties documented for this class. The creation time and creation
         *  version are specified, thus this factory is useful for creating objects from a database where those parameters
         *  are stored. */
        static Ptr instance(const std::string &hash, const std::string &name, const std::string &version,
                            const std::string &architecture, time_t creationTime, const std::string &creationVersion);

    public:
        /** Property: Hash.
         *
         *  This is the unique SHA-256 hash of the library as a whole and identifies the library in the database. It is
         *  usually the hash of the library file. */
        const std::string& hash() const;

        /** Property: Library name.
         *
         *  This is the name of the library and is usually the base name of the file containing the library, sans file
         *  name extensions. */
        const std::string& name() const;

        /** Property: Library version string.
         *
         *  An optional version number for the library. */
        const std::string& version() const;

        /** Property: Library architecture.
         *
         *  An optional string describing the instruction set architecture for the library. */
        const std::string& architecture() const;

        /** Property: Time at which this record was created.
         *
         *  This is the time this record was created. The time is stored in the database and an object that's created from
         *  the database will get the original creation time. */
        time_t creationTime() const;

        /** Property: ROSE version that created this record.
         *
         *  This is the version of ROSE that created this object. The version is stored in the database and an object that's
         *  created from the database will get the original creation version. */
        const std::string& creationVersion() const;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Information about a function.
     *
     *  A function has a name, identifying address, and hash (for matching) and belongs to a particular library. */
    class Function: public Sawyer::SharedObject {
    public:
        /** Shared ownership pointer. */
        using Ptr = Sawyer::SharedPointer<Function>;

    private:
        rose_addr_t address_;                           // required starting address that identifies function in library
        std::string name_;                              // optional name of function (possibly mangled)
        std::string demangledName_;                     // optional demangled name of function
        std::string hash_;                              // hash used for matching
        size_t nInsns_;                                 // number of instructions in function
        Library::Ptr library_;                          // library to which function belongs
        time_t ctime_ = 0;                              // time at which this object was created; zero if unknown
        std::string cversion_;                          // ROSE version that created this object; empty if unknown

    protected:
        Function(rose_addr_t address, const std::string &name, const std::string &demangledName, const std::string &hash,
                 size_t nInsns, time_t ctime, const std::string &cversion, const Library::Ptr &library);

    public:
        /** Allocating constructor.
         *
         *  The arguments are the same as the various properties documented for this class. The creation time and creation
         *  version are set to current values, thus this factory is useful for creating function objects that are backed
         *  by a database. */
        static Ptr instance(rose_addr_t address, const std::string &name, const std::string &demangledName,
                            const std::string &hash, size_t nInsns, const Library::Ptr &library);

        /** Allocating constructor.
         *
         *  The arguments are the same as the various properties documented for this class. The creation time and creation
         *  version are specified, thus this factory is useful for creating function objects from a database that stores
         *  those fields. */
        static Ptr instance(rose_addr_t address, const std::string &name, const std::string &demangledName,
                            const std::string &hash, size_t nInsns, time_t creationTime, const std::string &creationVersion,
                            const Library::Ptr &library);

    public:
        /** Property: Function entry virtual address.
         *
         *  The entry address identifies the function within its library. */
        rose_addr_t address() const;

        /** Property: Function name.
         *
         *  Optional name of the function. This is normally the mangled name. */
        const std::string& name() const;

        /** Property: Function demangled name.
         *
         *  Optional demangled name of the function. This property returns the @ref name value if the demangled value is the
         *  empty string. */
        const std::string& demangledName() const;

        /** Property: Hash used for matching. */
        const std::string& hash() const;

        /** Property: Number of instructions in function. */
        size_t nInsns() const;

        /** Property: Library to which function belongs. */
        Library::Ptr library() const;

        /** Property: Time at which this object was created. */
        time_t creationTime() const;

        /** Property: ROSE version that created this object. */
        const std::string& creationVersion() const;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    static Sawyer::Message::Facility mlog;              // diagnostic facility for debugger
private:
    Settings settings_;
    std::string databaseUrl_;                           // name of database to which this object is communicating
    Sawyer::Database::Connection db_;                   // the database, when connected

    // Cached info from the settings_.includeFile and settings_.excludeFile
    boost::filesystem::path cachedIncludeFile_;
    Sawyer::Container::Set<std::string> cachedIncludeNames_;
    boost::filesystem::path cachedExcludeFile_;
    Sawyer::Container::Set<std::string> cachedExcludeNames_;

    using LibraryCache = Sawyer::Container::Map<std::string /*hash*/, Library::Ptr>;
    LibraryCache libraryCache_;                         // cache of library objects read from or written to database

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Configuration settings
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Settings controlling general behavior.
     *
     * @{ */
    const Settings& settings() const;
    Settings& settings();
    void settings(const Settings&);
    /** @} */

    /** Command-line parser switch declarations that adjust settings.
     *
     *  This returns a switch group (part of a command-line parser's language specification) that will affect the data
     *  members of the supplied settings. The reference to the settings is saved in the return value. */
    static Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions for attaching to the database.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Connect to an existing database.
     *
     *  Throws an @ref Exception if the database cannot be opened. */
    void connect(const std::string &databaseUrl);

    /** Create a new database.
     *
     *  This is only possible for certain database drivers, such as SQLite. It creates the database, initializes the database
     *  tables, and connects to the database as if @ref connect was also invoked. */
    void createDatabase(const std::string &databaseUrl);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions that modify a database.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Insert a single function into the database.
     *
     *  Whether the funcition is inserted (or updated) depends on the current inclusion/exclusion settings. If the function is
     *  inserted or updated then the function returns a true value (such as the function that was inserted); otherwise it
     *  returns a false value.
     *
     * @{ */
    Function::Ptr insertFunction(const Library::Ptr&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);
    bool insertFunction(const Function::Ptr&);
    /** @} */

    /** Insert library information into the database.
     *
     *  All functions from the specified partitioner are measured and inserted into the database, subject to the settings for
     *  inclusion and exclusion. Each of the functions is associated with the specified library.
     *
     *  Returns the number of funcitons inserted and/or updated. */
    size_t insertLibrary(const Library::Ptr&, const Partitioner2::PartitionerConstPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions for querying a database.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Return information about a particular library.
     *
     *  Returns a pointer to the library having the specified hash, or a null pointer if no such library exists. */
    Library::Ptr library(const std::string &hash);

    /** Return information about all libraries in the database.
     *
     *  The return value is sorted by library hash. */
    std::vector<Library::Ptr> libraries();

    /** Return information about functions in the database.
     *
     *  The return value is sorted by function hash. Only those functions that are selected by the inclusion/exclusion @ref
     *  settings are included in the return value. If a library is given as an argument, then the functions are further
     *  restricted to belong to the specified library.
     *
     * @{ */
    std::vector<Function::Ptr> functions();
    std::vector<Function::Ptr> functions(const Library::Ptr&);
    /** @} */

    /** Find database functions that match a given function. */
    std::vector<Function::Ptr> search(const Partitioner2::PartitionerConstPtr &partitioner, const Partitioner2::FunctionPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Utilities
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Initialize diagnostic output.
     *
     *  This is called automatically when ROSE is initialized. */
    static void initDiagnostics();

    /** Compute the hash for a function.
     *
     *  This is how functions are identified and matched. */
    std::string hash(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&) const;

    /** Compute the number of instructions in a function. */
    static size_t nInsns(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);

private:
    // Throws an Exception if the database driver version is not adequate.
    void checkVersion();

    // Returns the database version number if there is one.
    Sawyer::Optional<unsigned> version();

    // Create the tables needed by a new database.
    void createTables();

    // Upgrade the database to the current version
    void upgradeDatabase();

    // Create or update the properties of a library in the database.
    void createLibrary(const Library::Ptr&);

    // Create or update the properties of a function in the database. It is not possible to update the address or library
    // to which a function belongs with this call since those properties are what identifies the function -- attempting to
    // change them simply creates a new function record without deleting the old one.
    void createFunction(const Function::Ptr&);

    // Whether the current settings specifically include this function from consideration. Exactly one of f1 or f2 should be
    // non-null.
    bool isIncluded(const Function::Ptr&);
    bool isIncluded(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);

    // Whether the current settings exclude a function from consideration. Exactly one of f1 or f2 should be non-null.
    bool isExcluded(const Function::Ptr&);
    bool isExcluded(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);

    // Whether to consider a function for insertion into a database or as a possible match of a database function to a
    // non-database function.
    bool isConsidered(const Function::Ptr&);
    bool isConsidered(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);

    // Load function names from files if necessary.
    void cacheFiles();
    void cacheNamesFromFile(const boost::filesystem::path &fileName, boost::filesystem::path &cachedFileName /*in,out*/,
                            Sawyer::Container::Set<std::string> &cachedNames /*out*/);

    // Return functions based on query.
    std::vector<Function::Ptr> functions(Sawyer::Database::Statement);
};

} // namespace
} // namespace

#endif
#endif
