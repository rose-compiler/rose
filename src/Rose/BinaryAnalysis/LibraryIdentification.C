#include <featureTests.h>
#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION
#include <Rose/BinaryAnalysis/LibraryIdentification.h>

#include <Rose/BinaryAnalysis/AstHasher.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine/Parser.h>
#include <Rose/CommandLine/Version.h>
#include <rose_getline.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#ifdef ROSE_HAVE_SQLITE3
#include <Sawyer/DatabaseSqlite.h>
#endif

#ifdef ROSE_HAVE_LIBPQXX
#include <Sawyer/DatabasePostgresql.h>
#endif

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Libraries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LibraryIdentification::Library::Library(const std::string &hash, const std::string &name, const std::string &version,
                                        const std::string &architecture, time_t ctime, const std::string &cversion)
    : hash_(hash), name_(name), version_(version), architecture_(architecture), ctime_(ctime), cversion_(cversion) {
    ASSERT_forbid(hash.empty());
}

LibraryIdentification::Library::Ptr
LibraryIdentification::Library::instance(const std::string &hash, const std::string &name, const std::string &version,
                                         const std::string &architecture) {
    return Ptr(new Library(hash, name, version, architecture, time(NULL), CommandLine::versionString));
}

LibraryIdentification::Library::Ptr
LibraryIdentification::Library::instance(const std::string &hash, const std::string &name, const std::string &version,
                                         const std::string &architecture, time_t ctime, const std::string &cversion) {
    return Ptr(new Library(hash, name, version, architecture, ctime, cversion));
}

const std::string&
LibraryIdentification::Library::hash() const {
    return hash_;
}

const std::string&
LibraryIdentification::Library::name() const {
    return name_;
}

const std::string&
LibraryIdentification::Library::version() const {
    return version_;
}

const std::string&
LibraryIdentification::Library::architecture() const {
    return architecture_;
}

time_t
LibraryIdentification::Library::creationTime() const {
    return ctime_;
}

const std::string&
LibraryIdentification::Library::creationVersion() const {
    return cversion_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LibraryIdentification::Function::Function(rose_addr_t address, const std::string &name, const std::string &demangledName,
                                          const std::string &hash, size_t nInsns, time_t ctime, const std::string &cversion,
                                          const Library::Ptr &library)
    : address_(address), name_(name), demangledName_(demangledName), hash_(hash), nInsns_(nInsns), library_(library),
      ctime_(ctime), cversion_(cversion) {
    ASSERT_not_null(library);
}

LibraryIdentification::Function::Ptr
LibraryIdentification::Function::instance(rose_addr_t address, const std::string &name, const std::string &demangledName,
                                          const std::string &hash, size_t nInsns, const Library::Ptr &library) {
    return Ptr(new Function(address, name, demangledName, hash, nInsns, time(NULL), CommandLine::versionString, library));
}

LibraryIdentification::Function::Ptr
LibraryIdentification::Function::instance(rose_addr_t address, const std::string &name, const std::string &demangledName,
                                          const std::string &hash, size_t nInsns, time_t ctime, const std::string &cversion,
                                          const Library::Ptr &library) {
    return Ptr(new Function(address, name, demangledName, hash, nInsns, ctime, cversion, library));
}

rose_addr_t
LibraryIdentification::Function::address() const {
    return address_;
}

const std::string&
LibraryIdentification::Function::name() const {
    return name_;
}

const std::string&
LibraryIdentification::Function::demangledName() const {
    return demangledName_.empty() ? name_ : demangledName_;
}

const std::string&
LibraryIdentification::Function::hash() const {
    return hash_;
}

size_t
LibraryIdentification::Function::nInsns() const {
    return nInsns_;
}

LibraryIdentification::Library::Ptr
LibraryIdentification::Function::library() const {
    return library_;
}

time_t
LibraryIdentification::Function::creationTime() const {
    return ctime_;
}

const std::string&
LibraryIdentification::Function::creationVersion() const {
    return cversion_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Analysis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Message::Facility LibraryIdentification::mlog;

// class method
void
LibraryIdentification::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::LibraryIdentification");
        mlog.comment("fast library identification and recognition (FLIR)");
    }
}

const LibraryIdentification::Settings&
LibraryIdentification::settings() const {
    return settings_;
}

LibraryIdentification::Settings&
LibraryIdentification::settings() {
    return settings_;
}

void
LibraryIdentification::settings(const Settings &s) {
    settings_ = s;
}

Sawyer::CommandLine::SwitchGroup
LibraryIdentification::commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Library identification settings");
    sg.name("flir");
    sg.switchOrder(INSERTION_ORDER);
    sg.doc("These switches affect the fast library identification and recognition analysis. This analysis uses hashes of "
           "the instructions in the order that instructions appear in the virtual address space, but it avoids hashing "
           "certain parts of the instruction that are dependent on the instruction address.\n\n"

           "Some of these switches are used to select which functions of a library are inserted into a database, or which "
           "functions of a database can be considered when matching a specimen function to them. The way this works is that "
           "a function is considered if it matches an \"include\" switch, or else if it doesn't match an \"exclude\" switch. "
           "That is, the \"include\" switches are considered first, then the \"exclude\" switches if needed. For example, to "
           "prevent all ELF shared library functions from being considered, use just \"@s{exclude-re}='.*@@plt'\", or to "
           "do the inverse and select only the functions whose names end with \"@@plt\", say \"@s{include-re}='.*@@plt' "
           "@s{exclude-re}='.*'\" (meaning include the @@plt names, and exclude everything else).");

    sg.insert(Switch("include-name")
              .argument("name", anyParser(settings.includeNames))
              .doc("If a function's mangled or demangled name matches the name specified by this switch, then the function "
                   "is considered for insertion or matching regardless of whether any of the exclusion switches would match. "
                   "This switch can be specified multiple times in order to whitelist multiple names."));

    sg.insert(Switch("include-re")
              .argument("RE", anyParser(settings.includeRes))
              .whichValue(SAVE_ALL)
              .doc("If a function's mangled or demangled name matches the regular expression specified by this switch, then "
                   "the function is considered for insertion or matching regardless of whether any of the exclusion switches "
                   "would match. This switch can be specified multiple times in order to whitelist mutliple regular "
                   "expressions."));

    sg.insert(Switch("include-file")
              .argument("filename", anyParser(settings.includeFile))
              .doc("If a function's mangled or demangled name matches any of the names in the specified file, then the "
                   "function is considered for insertion or matching regardless of whether any of the exclusion switches "
                   "would match. Each line of the file is a function name, but empty lines, lines containing only white "
                   "space, and lines whose first non-space character is \"#\" are ignored."));

    sg.insert(Switch("exclude-name")
              .argument("name", anyParser(settings.excludeNames))
              .doc("If a function's mangled or demangled name matches the name specified by this switch, then the function "
                   "is excluded from consideration for insertion or matching unless it is already selected by any of the "
                   "whitelist switches. This switch can be specified multiple times in order to blacklist multiple names."));

    sg.insert(Switch("exclude-re")
              .argument("RE", anyParser(settings.excludeRes))
              .whichValue(SAVE_ALL)
              .doc("If a function's mangled or demangled name matches the regular expression specified by this switch, then "
                   "the function is excluded from consideration for insertion or matching unless it is already selected by "
                   "any of the whitelist switches. This switch can be specified multiple times in order to blacklist multiple "
                   "regular expressions."));

    sg.insert(Switch("exclude-file")
              .argument("filename", anyParser(settings.excludeFile))
              .doc("If a function's mangled or demangled name matches any of the names in the specified file, then the "
                   "function is excluded from consideration for insertion or matching unless it is already selected by "
                   "any of the whitelist switches. Each line of the file is a function name, but empty lines, lines containing "
                   "only white space, and lines whose first non-space character is \"#\" are ignored."));

    sg.insert(Switch("exclude-size")
              .argument("ninsns", nonNegativeIntegerParser(settings.minFunctionInsns))
              .doc("Functions that have fewer than @v{ninsns} instructions are excluded from consideration for insertion or "
                   "matching unless they are already selected by any of the whitelist switches. The default is " +
                   StringUtility::plural(settings.minFunctionInsns, "instructions") + "."));

    Rose::CommandLine::insertBooleanSwitch(sg, "identify-functions-by-hash", settings.identifyFunctionsByHash,
                                           "Functions are stored in a database according to their key. Version zero of the "
                                           "database used the function's hash as its key, which meant that small functions that "
                                           "sometimes hash to the same value could not be distinguished from one another. "
                                           "Databases after version zero (used by this tool) use a pair of values as the "
                                           "identification key: the library to which the function belongs, and the entry address "
                                           "of the function. When inserting a new function into the database, if the new "
                                           "function's key matches the key of another function that's already in the database, "
                                           "then the new function overwrites the existing function.\n\n"

                                           "This switch causes version 1 and later of the database to use the version zero "
                                           "design of identifying functions by their hashes.");
    return sg;
}

void
LibraryIdentification::connect(const std::string &url) {
    libraryCache_.clear();
    db_ = Sawyer::Database::Connection::fromUri(url);
    checkVersion();
    upgradeDatabase();

    // Check that tables exist by reading from them. An exception is thrown if they don't exist or something is wrong.
    for (auto row: db_.stmt("select name, version, architecture, ctime, cversion from libraries where hash = ''"))
        break;
    for (auto row: db_.stmt("select address, name, hash, ninsns, ctime, cversion from functions where library_hash = ''"))
        break;
}

void
LibraryIdentification::createDatabase(const std::string &url) {
    db_ = Sawyer::Database::Connection::fromUri(url);
    checkVersion();
    createTables();
}

void
LibraryIdentification::checkVersion() {
    if (db_.isOpen() && db_.driverName() == "sqlite") {
#if defined(SQLITE_VERSION_NUMBER) && defined(SQLITE_VERSION) && SQLITE_VERSION_NUMBER < 3024000
        throw Exception("SQLite version 3.24.0 or later is required (you have " SQLITE_VERSION ")");
#endif
    }
}

void
LibraryIdentification::createTables() {
    libraryCache_.clear();

    db_.run("drop table if exists settings");
    db_.run("create table settings ("
            "name text,"
            "value text)");
    db_.run("insert into settings (name, value) values ('version', '2')");

    db_.run("drop table if exists libraries");
    db_.run("create table libraries ("
            "hash text primary key,"
            "name text,"
            "version text,"
            "architecture text,"
            "ctime bigint,"
            "cversion text)");

    db_.run("drop table if exists functions");
    db_.run("create table functions ("
            "address integer,"
            "name text,"
            "demangled_name text,"
            "hash text,"
            "ninsns integer,"
            "ctime bigint,"
            "cversion text,"
            "library_hash text not null)");

    db_.run("drop index if exists function_index");
    db_.run("create unique index function_index on functions (address, library_hash)");
}

Sawyer::Optional<unsigned>
LibraryIdentification::version() {
    // If there's a "settings" table that has a "version" property, return its value
    try {
        return *db_.get<unsigned>("select value from settings where name = 'version'");
    } catch (...) {}

    // Dan's original implementation didn't have any version information, so just try to read the tables. We'll
    // call this version zero.
    try {
        for (auto row: db_.stmt("select library_name, library_version, architecture, time from libraries where libraryID = ''"))
            break;
        for (auto row: db_.stmt("select function_name, libraryID from functions where functionId = ''"))
            break;
        return 0;
    } catch (...) {}

    // No idea. Perhaps this isn't a library recognition database.
    return Sawyer::Nothing();
}

void
LibraryIdentification::upgradeDatabase() {
    libraryCache_.clear();
    if (auto v = version()) {
        switch (*v) {
            case 0:                                     // Dan's original version
                SAWYER_MESG(mlog[INFO]) <<"upgrading database from version 0 to version 1\n";

                db_.run("alter table libraries rename column libraryID to hash");
                db_.run("alter table libraries rename column library_name to name");
                db_.run("alter table libraries rename column library_version to version");
                db_.run("alter table libraries rename column time to ctime");
                db_.run("alter table libraries add cversion text");

                db_.run("alter table functions rename column functionID to hash");
                db_.run("alter table functions rename column function_name to name");
                db_.run("alter table functions rename column libraryID to library_hash");
                db_.run("alter table functions add column ctime bigint");
                db_.run("alter table functions add column address integer");
                db_.run("alter table functions add column ninsns integer");
                db_.run("alter table functions add column cversion text");
                db_.run("alter table functions add column demangled_name text");

                db_.run("drop table if exists settings");
                db_.run("create table settings (name text, value text)");
                db_.run("insert into settings (name, value) values ('version', '1')");

                SAWYER_MESG(mlog[WARN]) <<"upgrading database from version 0: functions have no address or size\n"
                                        <<"upgraded database may not be entirely useful due to missing information\n";
                // fall through

            case 1:
                // This may fail for a database that was upgraded from version zero because all the function starting addresses
                // are zero (not available in the version 0 database).
                SAWYER_MESG(mlog[INFO]) <<"upgrading database from version 1 to version 2\n";
                db_.run("drop index if exists function_index");
                db_.run("create unique index function_index on functions (address, library_hash)");
                db_.run("update settings set value = 2 where name = 'version'");
                // fall through

            case 2:
                // current version
                break;

            default:
                throw Exception("unsupported database version " + boost::lexical_cast<std::string>(*v));
        }
    } else {
        throw Exception("database has no version information and thus cannot be upgraded");
    }
}

LibraryIdentification::Library::Ptr
LibraryIdentification::library(const std::string &hash) {
    // Return a cached object
    if (Library::Ptr library = libraryCache_.getOrDefault(hash))
        return library;

    // Or find the object in the database and cache it
    auto stmt = db_.stmt("select name, version, architecture, ctime, cversion from libraries where hash = ?hash")
                .bind("hash", hash);
    for (auto row: stmt) {
        const std::string name = row.get<std::string>(0).orDefault();
        const std::string version = row.get<std::string>(1).orDefault();
        const std::string architecture = row.get<std::string>(2).orDefault();
        const time_t ctime = row.get<time_t>(3).orElse(0);
        const std::string cversion = row.get<std::string>(4).orDefault();
        auto retval = Library::instance(hash, name, version, architecture, ctime, cversion);
        libraryCache_.insert(hash, retval);
        return retval;
    }

    return {};
}

std::vector<LibraryIdentification::Library::Ptr>
LibraryIdentification::libraries() {
    std::vector<Library::Ptr> retval;
    auto stmt = db_.stmt("select hash, name, version, architecture, ctime, cversion from libraries order by hash");
    for (auto row: stmt) {
        Library::Ptr lib;
        const std::string hash = row.get<std::string>(0).orDefault();
        if (!libraryCache_.getOptional(hash).assignTo(lib)) {
            const std::string name = row.get<std::string>(1).orDefault();
            const std::string version = row.get<std::string>(2).orDefault();
            const std::string architecture = row.get<std::string>(3).orDefault();
            const time_t ctime = row.get<time_t>(4).orElse(0);
            const std::string cversion = row.get<std::string>(5).orDefault();
            lib = Library::instance(hash, name, version, architecture, ctime, cversion);
            libraryCache_.insert(hash, lib);
        }
        ASSERT_not_null(lib);
        retval.push_back(lib);
    }
    return retval;
}

std::vector<LibraryIdentification::Function::Ptr>
LibraryIdentification::functions() {
    auto stmt = db_.stmt("select address, name, demangled_name, hash, ninsns, ctime, cversion, library_hash"
                         " from functions"
                         " order by hash");
    return functions(stmt);
}

std::vector<LibraryIdentification::Function::Ptr>
LibraryIdentification::functions(const Library::Ptr &lib) {
    ASSERT_not_null(lib);
    auto stmt = db_.stmt("select address, name, demangled_name, hash, ninsns, ctime, cversion, library_hash"
                         " from functions"
                         " where library_hash = ?lib"
                         " order by hash")
                .bind("lib", lib->hash());
    return functions(stmt);
}

std::vector<LibraryIdentification::Function::Ptr>
LibraryIdentification::functions(Sawyer::Database::Statement stmt) {
    std::vector<Function::Ptr> retval;
    for (auto row: stmt) {
        const rose_addr_t va = row.get<rose_addr_t>(0).orElse(0);
        const std::string name = row.get<std::string>(1).orDefault();
        const std::string demangledName = row.get<std::string>(2).orDefault();
        const std::string hash = row.get<std::string>(3).orDefault();
        const size_t nInsns = row.get<size_t>(4).orElse(0);
        const time_t ctime = row.get<time_t>(5).orElse(0);
        const std::string cversion = row.get<std::string>(6).orDefault();
        const std::string libHash = row.get<std::string>(7).orDefault();
        auto library = this->library(libHash);
        ASSERT_not_null(library);
        auto function = Function::instance(va, name, demangledName, hash, nInsns, ctime, cversion, library);

        if (isConsidered(function))
            retval.push_back(function);
    }
    return retval;
}

void
LibraryIdentification::createLibrary(const Library::Ptr &library) {
    ASSERT_not_null(library);
    db_.stmt("insert into libraries (hash, name, version, architecture, ctime, cversion)"
             " values (?hash, ?name, ?version, ?architecture, ?ctime, ?cversion)"
             " on conflict (hash) do"
             " update set name = ?name, version = ?version, architecture = ?architecture, ctime = ?ctime, cversion = ?cversion")
        .bind("hash", library->hash())
        .bind("name", library->name())
        .bind("version", library->version())
        .bind("architecture", library->architecture())
        .bind("ctime", library->creationTime())
        .bind("cversion", library->creationVersion())
        .run();

    libraryCache_.insert(library->hash(), library);
}

void
LibraryIdentification::createFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);
    db_.stmt("insert into functions (address,  name,  demangled_name,  hash,  ninsns,  ctime,  cversion,  library_hash)"
             " values (?address, ?name, ?demangled_name, ?hash, ?ninsns, ?ctime, ?cversion, ?library)"
             " on conflict (address, library_hash) do"
             " update set name = ?name, demangled_name = ?demangled_name, hash = ?hash, ninsns = ?ninsns, "
             "   ctime = ?ctime, cversion = ?cversion")
        .bind("address", function->address())
        .bind("name", function->name())
        .bind("demangled_name", function->demangledName())
        .bind("hash", function->hash())
        .bind("ninsns", function->nInsns())
        .bind("ctime", function->creationTime())
        .bind("cversion", function->creationVersion())
        .bind("library", function->library()->hash())
        .run();
}

std::string
LibraryIdentification::hash(const Partitioner2::Partitioner::ConstPtr &partitioner,
                            const Partitioner2::Function::Ptr &function) const {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);

    auto hasher = std::make_shared<Combinatorics::HasherSha256Builtin>();
    AstHasher astHash(hasher);
    for (rose_addr_t va: function->basicBlockAddresses()) {
        if (Partitioner2::BasicBlock::Ptr bb = partitioner->basicBlockExists(va))
            astHash.hash(bb);
    }
    return hasher->toString();
}

size_t
LibraryIdentification::nInsns(const Partitioner2::Partitioner::ConstPtr &partitioner, const Partitioner2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);
    size_t nInsns = 0;
    for (rose_addr_t bbVa: function->basicBlockAddresses()) {
        if (Partitioner2::BasicBlock::Ptr bb = partitioner->basicBlockExists(bbVa))
            nInsns += bb->nInstructions();
    }
    return nInsns;
}

LibraryIdentification::Function::Ptr
LibraryIdentification::insertFunction(const Library::Ptr &library, const Partitioner2::Partitioner::ConstPtr &partitioner,
                                      const Partitioner2::Function::Ptr &function) {
    ASSERT_not_null(library);
    ASSERT_not_null(function);

    if (isConsidered(partitioner, function)) {
        auto f = Function::instance(function->address(), function->name(), function->demangledName(), hash(partitioner, function),
                                    nInsns(partitioner, function), library);
        createLibrary(library);
        createFunction(f);
        return f;
    } else {
        return Function::Ptr();
    }
}

bool
LibraryIdentification::insertFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);

    if (isConsidered(function)) {
        createLibrary(function->library());
        createFunction(function);
        return true;
    } else {
        return false;
    }
}

bool
LibraryIdentification::isIncluded(const Function::Ptr &function) {
    ASSERT_not_null(function);

    // Include based on function name
    if (settings_.includeNames.exists(function->name()) || cachedIncludeNames_.exists(function->name()))
        return true;


    // Include based on name patterns
    for (const std::regex &re: settings_.includeRes) {
        if (std::regex_match(function->name(), re))
            return true;
    }
    return false;
}

bool
LibraryIdentification::isIncluded(const Partitioner2::Partitioner::ConstPtr&, const Partitioner2::Function::Ptr &function) {
    ASSERT_not_null(function);

    // Include based on function name
    if (settings_.includeNames.exists(function->name()) || cachedIncludeNames_.exists(function->name()))
        return true;
    if (settings_.includeNames.exists(function->demangledName()) || cachedIncludeNames_.exists(function->demangledName()))
        return true;

    // Include based on name patterns
    for (const std::regex &re: settings_.includeRes) {
        if (std::regex_match(function->name(), re) || std::regex_match(function->demangledName(), re))
            return true;
    }
    return false;
}

bool
LibraryIdentification::isExcluded(const Function::Ptr &function) {
    ASSERT_not_null(function);

    // Exclude based on number of instructions
    if (settings_.minFunctionInsns > 0) {
        if (function->nInsns() < settings_.minFunctionInsns)
            return true;
    }

    // Exclude based on name
    if (settings_.excludeNames.exists(function->name()) || cachedExcludeNames_.exists(function->name()))
        return true;

    // Exclude based on name patterns
    for (const std::regex &re: settings_.excludeRes) {
        if (std::regex_match(function->name(), re))
            return true;
    }

    return false;
}

bool
LibraryIdentification::isExcluded(const Partitioner2::Partitioner::ConstPtr &partitioner,
                                  const Partitioner2::Function::Ptr &function) {
    ASSERT_not_null(function);

    // Exclude based on number of instructions
    if (settings_.minFunctionInsns > 0) {
        const size_t n = nInsns(partitioner, function);
        if (n < settings_.minFunctionInsns)
            return true;
    }

    // Exclude based on name
    if (settings_.excludeNames.exists(function->name()) || cachedExcludeNames_.exists(function->name()))
        return true;
    if (settings_.excludeNames.exists(function->demangledName()) || cachedExcludeNames_.exists(function->demangledName()))
        return true;

    // Exclude based on name patterns
    for (const std::regex &re: settings_.excludeRes) {
        if (std::regex_match(function->name(), re) || std::regex_match(function->demangledName(), re))
            return true;
    }
    return false;
}

bool
LibraryIdentification::isConsidered(const Function::Ptr &function) {
    ASSERT_not_null(function);
    cacheFiles();
    return isIncluded(function) || !isExcluded(function);
}

bool
LibraryIdentification::isConsidered(const Partitioner2::Partitioner::ConstPtr &partitioner,
                                    const Partitioner2::Function::Ptr &function) {
    ASSERT_not_null(function);
    cacheFiles();
    return isIncluded(partitioner, function) || !isExcluded(partitioner, function);
}

size_t
LibraryIdentification::insertLibrary(const Library::Ptr &library, const Partitioner2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(library);
    size_t nInserted = 0;
    Sawyer::ProgressBar<size_t> progress(partitioner->nFunctions(), mlog[MARCH], "functions");
    for (const Partitioner2::Function::Ptr &function: partitioner->functions()) {
        ++progress;
        if (insertFunction(library, partitioner, function))
            ++nInserted;
    }
    return nInserted;
}

std::vector<LibraryIdentification::Function::Ptr>
LibraryIdentification::search(const Partitioner2::Partitioner::ConstPtr &partitioner, const Partitioner2::Function::Ptr &function) {
    ASSERT_not_null(function);
    std::vector<Function::Ptr> retval;

    const std::string h = hash(partitioner, function);
    auto stmt = db_.stmt("select address, name, demangled_name, ninsns, ctime, cversion, library_hash"
                         " from functions where hash = ?hash")
                .bind("hash", h);
    for (auto row: stmt) {
        const rose_addr_t address = row.get<rose_addr_t>(0).orElse(0);
        const std::string name = row.get<std::string>(1).orDefault();
        const std::string demangledName = row.get<std::string>(2).orDefault();
        const size_t nInsns = row.get<size_t>(3).orElse(0);
        const time_t ctime = row.get<time_t>(4).orElse(0);
        const std::string cversion = row.get<std::string>(5).orDefault();
        const std::string libhash = row.get<std::string>(6).orDefault();

        auto library = this->library(libhash);
        ASSERT_not_null(library);
        auto found = Function::instance(address, name, demangledName, h, nInsns, ctime, cversion, library);

        if (isConsidered(found))
            retval.push_back(found);
    }
    return retval;
}

void
LibraryIdentification::cacheNamesFromFile(const boost::filesystem::path &fileName,
                                          boost::filesystem::path &cachedFileName /*in,out*/,
                                          Sawyer::Container::Set<std::string> &cachedNames /*out*/) {
    if (fileName != cachedFileName) {
        cachedNames.clear();
        if (!fileName.empty()) {
            std::ifstream in(fileName.c_str());
            if (!in)
                throw Exception("cannot open " + boost::lexical_cast<std::string>(fileName));
            while (in) {
                std::string line = boost::trim_copy(rose_getline(in));
                if (boost::starts_with(line, "#"))
                    continue;
                if (!line.empty())
                    cachedNames.insert(line);
            }
        }
        cachedFileName = fileName;
    }
}

void
LibraryIdentification::cacheFiles() {
    cacheNamesFromFile(settings_.includeFile, cachedIncludeFile_, cachedIncludeNames_);
    cacheNamesFromFile(settings_.excludeFile, cachedExcludeFile_, cachedExcludeNames_);
}

} // namespace
} // namespace

#endif
