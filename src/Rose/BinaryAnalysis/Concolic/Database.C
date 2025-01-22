// Second implementation of Concolic::Database that works for both SQLite and PostgreSQL
// The schema is not identical to that of the first implementation.
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/archive/binary_iarchive.hpp>            // included before ROSE headers
#include <boost/archive/binary_oarchive.hpp>            // included before ROSE headers
#include <boost/archive/xml_iarchive.hpp>               // included before ROSE headers
#include <boost/archive/xml_oarchive.hpp>               // included bofore ROSE headers
#endif

#include <Rose/BinaryAnalysis/Concolic/Database.h>

#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/I386Linux/ExitStatusResult.h>
#include <Rose/BinaryAnalysis/Concolic/I386Linux/TracingResult.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingResult.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Concolic/TestSuite.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/StringUtility/SplitJoin.h>
#include <ROSE_UNUSED.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/regex.hpp>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/nvp.hpp>
#endif

#include <Sawyer/FileSystem.h>

#include <array>
#include <ctime>
#include <fstream>

#ifdef ROSE_HAVE_SQLITE3
#include <Sawyer/DatabaseSqlite.h>
#endif

#ifdef ROSE_HAVE_LIBPQXX
#include <Sawyer/DatabasePostgresql.h>
#endif

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

// The only purpose of this class is to access private friend functions in other classes.
class DatabaseAccess {
public:
    static void executionEventRecreateTable(Sawyer::Database::Connection db) {
        ExecutionEvent::recreateTable(db);
    }

    static void toDatabase(const Database::Ptr &db, const ExecutionEvent::Ptr &obj, ExecutionEventId id) {
        obj->toDatabase(db, id);
    }

    static void fromDatabase(const Database::Ptr &db, const ExecutionEvent::Ptr &obj, ExecutionEventId id) {
        obj->fromDatabase(db, id);
    }
};



// Register the derived types that we might be serializing through base pointers. Archive is one of the boost::archive classes.
template<class Archive>
static void
registerTypes(Archive &archive) {
    archive.template register_type<IS::SymbolicSemantics::MemoryListState>();
    archive.template register_type<IS::SymbolicSemantics::MemoryMapState>();
    archive.template register_type<I386Linux::ExitStatusResult>();
    archive.template register_type<I386Linux::TracingResult>();
    archive.template register_type<M68kSystem::TracingResult>();
}

// Return the file name part of an SQLite connection URL
static boost::filesystem::path
sqliteFileName(const std::string &url) {
    boost::regex re("(\\w*)://(.+)");
    boost::smatch matched;
    if (boost::regex_match(url, matched, re)) {
        if (matched.str(1) == "sqlite" || matched.str(1) == "sqlite3") {
            return matched.str(2);
        } else {
            return boost::filesystem::path();
        }
    } else {
        return url;
    }
}

// Initialize the database schema
static void
initSchema(Sawyer::Database::Connection db) {

    // SQLite has a relatively small limit for storing large `bytea` objects. This table holds a larger binary blob as a
    // broken up sequence of parts numbered consecutively starting at zero.
    db.run("drop table if exists blob_parts");
    db.run("create table blob_parts ("
           " id integer not null,"
           " part_number integer not null,"
           " part bytea not null)");

    db.run("drop table if exists test_suites");
    db.run("create table test_suites ("
           " created_ts varchar(32) not null,"
           " id integer primary key,"
           " name text not null unique)");

    db.run("drop table if exists specimens");
    db.run("create table specimens ("
           " id integer primary key,"
           " created_ts varchar(32) not null,"
           " name text not null,"
           " content bytea,"                            // maybe null
           " rba integer,"                              // null, or the id for a blob in the blob_parts table
           " test_suite integer not null,"

           " constraint fk_test_suite foreign key (test_suite) references test_suites (id))");

    db.run("drop table if exists test_cases");
    db.run("create table test_cases ("
           " id integer primary key,"
           " parent integer,"                           // parent test case or null
           " created_ts varchar(32) not null,"
           " name text not null,"
           " executor text not null,"
           " specimen integer not null,"
           " argv bytea not null,"
           " envp bytea not null,"
           " symbolic_state bytea,"                     // optional serialized symbolic state
           " assertions string,"                        // optional serialized SMT solver assertions
           " concrete_rank real,"                       // null if concrete executor not run yet
           " concrete_result bytea,"                    // null if concrete executor not run yet
           " concolic_result integer,"                  // non-null if concolic executor has been run
           " concrete_interesting integer not null default 1," // concrete results are uninteresting if present? (bool)
           " test_suite integer not null,"

           " constraint fk_specimen foreign key (specimen) references specimens (id),"
           " constraint fk_test_suite foreign key (test_suite) references test_suites (id))");

    DatabaseAccess::executionEventRecreateTable(db);
}

static void
initTestSuite(const Database::Ptr &db) {
    if (auto id = db->connection().get<size_t>("select id from test_suites order by created_ts desc limit 1")) {
        auto ts = db->object(TestSuiteId(*id));
        db->testSuite(ts);
    } else {
        auto ts = TestSuite::instance();
        db->testSuite(ts);
    }
}

static size_t
generateId() {
    static boost::random::mt19937 prn(::time(NULL) + ::getpid());
    boost::random::uniform_int_distribution<size_t> distributor(0, 2*1000*1000*1000); // databases have low upper limits
    return distributor(prn);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Low-level test suite database operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
updateObject(const Database::Ptr &db, TestSuiteId id, const TestSuite::Ptr &obj) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    ASSERT_not_null(obj);
    auto iter = db->connection().stmt("select name, created_ts from test_suites where id = ?id").bind("id", *id).begin();
    if (!iter)
        throw Exception("no such test_suite in database where id=" + boost::lexical_cast<std::string>(*id));
    obj->name(*iter->get<std::string>(0));
    obj->timestamp(*iter->get<std::string>(1));
}

static void
updateDb(const Database::Ptr &db, TestSuiteId id, const TestSuite::Ptr &obj) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    ASSERT_not_null(obj);
    Sawyer::Database::Statement stmt;
    if (*db->connection().stmt("select count(*) from test_suites where id = ?id").bind("id", *id).get<size_t>()) {
        stmt = db->connection().stmt("update test_suites set name = ?name where id = ?id");
    } else {
        std::string ts = obj->timestamp().empty() ? Database::timestamp() : obj->timestamp();
        obj->timestamp(ts);
        stmt = db->connection().stmt("insert into test_suites (id, created_ts, name) values (?id, ?ts, ?name)")
               .bind("ts", ts);
    }

    // Test suites must have unique names. Try the ID if no name was specified
    if (obj->name().empty())
        obj->name("suite-" + boost::lexical_cast<std::string>(*id));
    
    stmt
        .bind("id", *id)
        .bind("name", obj->name())
        .run();
}

static void
eraseDb(const Database::Ptr &db, TestSuiteId id) {
    ASSERT_require(id);
    if (db->testSuite()) {
        TestSuiteId current = db->id(db->testSuite(), Update::NO);
        if (*id == *current)
            throw Exception("cannot delete the current test suite");
    }

    db->eraseSpecimens(id);
    db->connection().stmt("delete from test_suites where id = ?id")
        .bind("id", *id)
        .run();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Low-level specimen database operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
updateObject(const Database::Ptr &db, SpecimenId id, const Specimen::Ptr &obj) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    ASSERT_not_null(obj);
    auto iter = db->connection().stmt("select name, created_ts, content from specimens where id = ?id").bind("id", *id).begin();
    if (!iter)
        throw Exception("no such specimen in database where id=" + boost::lexical_cast<std::string>(*id));
    obj->name(iter->get<std::string>(0).orDefault());
    obj->timestamp(iter->get<std::string>(1).orDefault());
    obj->content(iter->get<Specimen::BinaryData>(2).orDefault());
}

static void
updateDb(const Database::Ptr &db, SpecimenId id, const Specimen::Ptr &obj) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    ASSERT_not_null(obj);
    Sawyer::Database::Statement stmt;
    if (*db->connection().stmt("select count(*) from specimens where id = ?id").bind("id", *id).get<size_t>()) {
        stmt = db->connection().stmt("update specimens set name = ?name, content = ?content where id = ?id");
    } else {
        std::string ts = obj->timestamp().empty() ? Database::timestamp() : obj->timestamp();
        obj->timestamp(ts);
        stmt = db->connection().stmt("insert into specimens (id, created_ts, name, content, test_suite)"
                                     " values (?id, ?ts, ?name, ?content, ?test_suite)")
               .bind("ts", ts)
               .bind("test_suite", *db->id(db->testSuite()));
    }
    stmt
        .bind("id", *id)
        .bind("name", obj->name())
        .bind("content", obj->content())
        .run();
}

static void
eraseDb(const Database::Ptr &db, SpecimenId id) {
    ASSERT_require(id);
    db->eraseTestCases(id);
    db->connection().stmt("delete from specimens where id = ?id")
        .bind("id", *id)
        .run();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Low-level test case database operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
updateObject(const Database::Ptr &db, TestCaseId id, const TestCase::Ptr &obj) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    ASSERT_not_null(obj);
    //                                        0     1         2         3     4     5              6                7
    auto iter = db->connection().stmt("select name, executor, specimen, argv, envp, concrete_rank, concolic_result, created_ts,"
                                      // 8                    9       10
                                      " concrete_interesting, parent, assertions"
                                      " from test_cases"
                                      " where id = ?id"
                                      " order by created_ts").bind("id", *id).begin();
    if (!iter)
        throw Exception("no such test case in database where id=" + boost::lexical_cast<std::string>(*id));
    obj->name(*iter->get<std::string>(0));
    obj->executor(*iter->get<std::string>(1));
    obj->specimen(db->object(SpecimenId(*iter->get<size_t>(2)), Update::YES));
    obj->args(StringUtility::split('\0', *iter->get<std::string>(3)));
    obj->concreteRank(iter->get<double>(5));
    obj->concolicResult(iter->get<size_t>(6));
    obj->timestamp(*iter->get<std::string>(7));
    obj->concreteIsInteresting(*iter->get<int>(8) != 0);

    if (auto parent = iter->get<size_t>(9)) {
        obj->parent(TestCaseId(*parent));
    } else {
        obj->parent(TestCaseId());
    }

    if (auto assertions = iter->get<std::string>(10)) {
        std::istringstream ss(*assertions);
        boost::archive::binary_iarchive archive(ss);
        registerTypes(archive);
        std::vector<SymbolicExpression::Ptr> v;
        archive >>v;
        obj->assertions(v);
    } else {
        obj->assertions().clear();
    }

    std::vector<EnvValue> env;
    for (std::string str: StringUtility::split('\0', *iter->get<std::string>(4))) {
        auto parts = StringUtility::split('=', str, 2);
        ASSERT_require(parts.size() == 2);
        env.push_back(EnvValue(parts[0], parts[1]));
    }
    obj->env(env);
}

static void
updateDb(const Database::Ptr &db, TestCaseId id, const TestCase::Ptr &obj) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    ASSERT_not_null(obj);
    Sawyer::Database::Statement stmt;
    if (*db->connection().stmt("select count(*) from test_cases where id = ?id").bind("id", *id).get<size_t>()) {
        stmt = db->connection().stmt("update test_cases set"
                                     " name = ?name,"
                                     " parent = ?parent,"
                                     " executor = ?executor,"
                                     " specimen = ?specimen,"
                                     " argv = ?argv,"
                                     " envp = ?envp,"
                                     " assertions = ?assertions,"
                                     " concrete_rank = ?concrete_rank,"
                                     " concrete_interesting = ?interesting,"
                                     " concolic_result = ?concolic_result"
                                     " where id = ?id");
    } else {
        std::string ts = obj->timestamp().empty() ? Database::timestamp() : obj->timestamp();
        obj->timestamp(ts);
        stmt = db->connection().stmt("insert into test_cases ("
                                     "  id, parent, created_ts, name, executor, specimen, argv, envp,"
                                     "  concrete_rank, concrete_interesting, concolic_result, test_suite,"
                                     "  assertions"
                                     ") values ("
                                     "  ?id, ?parent, ?ts, ?name, ?executor, ?specimen, ?argv, ?envp,"
                                     "  ?concrete_rank, ?interesting, ?concolic_result, ?test_suite,"
                                     "  ?assertions)")
               .bind("ts", ts)
               .bind("test_suite", *db->id(db->testSuite()));
    }

    // Join environment variables to form a single string
    std::string envp;
    for (auto pair: obj->env()) {
        if (!envp.empty())
            envp += '\0';
        envp += pair.first + "=" + pair.second;
    }

    if (obj->parent()) {
        stmt.bind("parent", *obj->parent());
    } else {
        stmt.bind("parent", Sawyer::Nothing());
    }

    if (obj->assertions().empty()) {
        stmt.bind("assertions", Sawyer::Nothing());
    } else {
        std::ostringstream ss;
        {
            boost::archive::binary_oarchive archive(ss);
            registerTypes(archive);
            archive <<obj->assertions();
        }
        stmt.bind("assertions", ss.str());
    }

    stmt
        .bind("id", *id)
        .bind("name", obj->name())
        .bind("executor", obj->executor())
        .bind("specimen", *db->id(obj->specimen(), Update::YES))
        .bind("argv", StringUtility::join('\0', obj->args()))
        .bind("envp", envp)
        .bind("concrete_rank", obj->concreteRank())
        .bind("interesting", obj->concreteIsInteresting() ? 1 : 0)
        .bind("concolic_result", obj->concolicResult())
        .run();
}

static void
eraseDb(const Database::Ptr &db, TestCaseId id) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    db->eraseExecutionEvents(id);
    db->connection().stmt("delete from test_cases where id = ?tcid")
        .bind("tcid", *id)
        .run();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Low-level execution event database operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
updateObject(const Database::Ptr &db, ExecutionEventId id, const ExecutionEvent::Ptr &obj) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    ASSERT_not_null(obj);
    DatabaseAccess::fromDatabase(db, obj, id);
}

static void
updateDb(const Database::Ptr &db, ExecutionEventId id, const ExecutionEvent::Ptr &obj) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    ASSERT_not_null(obj);
    DatabaseAccess::toDatabase(db, obj, id);
}

static void
eraseDb(const Database::Ptr &db, ExecutionEventId id) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    db->connection().stmt("delete from execution_events where id = ?id")
        .bind("id", *id)
        .run();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Database operation helper templates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Helper template function for the Database::object method
template<class IdObjMap>
static typename IdObjMap::Target // object pointer
objectHelper(const Database::Ptr &db, const typename IdObjMap::Source &id, Update update, IdObjMap &objMap,
             const std::string &objectTypeName) {
    if (!id)
        throw Exception("invalid " + objectTypeName + " ID");
    auto obj = objMap.forward().getOrDefault(id);
    if (!obj) {
        obj = IdObjMap::Target::Pointee::instance();
        updateObject(db, id, obj);
        objMap.insert(id, obj);
    } else if (Update::YES == update) {
        updateObject(db, id, obj);
    }
    ASSERT_not_null(obj);
    return obj;
}

// Helper template function for the Database::id method
template<class IdObjMap>
static typename IdObjMap::Source // ID
idHelper(const Database::Ptr &db, const typename IdObjMap::Target &obj, Update &update, IdObjMap &objMap) {
    ASSERT_not_null(obj);
    auto id = objMap.reverse().getOrDefault(obj);
    if (id) {
        if (Update::YES == update)
            updateDb(db, id, obj);
    } else if (Update::YES == update) {
        id = generateId();
        updateDb(db, id, obj);
        objMap.insert(id, obj);
    }
    return id;
}

// Helper template function for Database::erase method
template<class IdObjMap>
static typename IdObjMap::Source // ID
eraseHelper(const Database::Ptr &db, const typename IdObjMap::Source &id, IdObjMap &objMap, const std::string &objectTypeName) {
    if (!id)
        throw Exception("invalid " + objectTypeName + " ID");
    eraseDb(db, id);
    objMap.eraseSource(id);
    return typename IdObjMap::Source();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations of member functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Database::Database() {}

Database::~Database() {}

// class method
Database::Ptr
Database::instance(const std::string &url) {
    Ptr db(new Database);

    if (boost::starts_with(url, "postgresql://") || boost::starts_with(url, "postgres://")) {
#ifdef ROSE_HAVE_LIBPQXX
#if 0 // [Robb Matzke 2020-02-21]: not ready to use yet
        db->connection_ = Sawyer::Database::Postgresql(url);
#else
        ASSERT_not_implemented("[Robb Matzke 2020-02-21]");
#endif
#else
        throw Exception("ROSE was not configured with PostgreSQL");
#endif
    }

    boost::filesystem::path fileName = sqliteFileName(url);
    if (!fileName.empty()) {
#ifdef ROSE_HAVE_SQLITE3
        if (!boost::filesystem::exists(fileName))
            throw Exception("sqlite database " + boost::lexical_cast<std::string>(fileName) + " does not exist");
        db->connection_ = Sawyer::Database::Sqlite(fileName);
#else
        throw Exception("ROSE was not configured with SQLite");
#endif
    }

    initTestSuite(db);
    return db;
}

// class method
Database::Ptr
Database::create(const std::string &url, const Sawyer::Optional<std::string> &testSuiteName) {
    Ptr db;

    if (boost::starts_with(url, "postgresql://") || boost::starts_with(url, "postgres://")) {
#ifdef ROSE_HAVE_LIBPQXX
        throw Exception("cannot create a PostgreSQL database; use external tools to do that, then open the database");
#else
        throw Exception("ROSE was not configured with PostgreSQL");
#endif
    }

    boost::filesystem::path fileName = sqliteFileName(url);
    if (!fileName.empty()) {
#ifdef ROSE_HAVE_SQLITE3
        db = Ptr(new Database);
        boost::system::error_code ec;
        boost::filesystem::remove(fileName, ec);
        db->connection_ = Sawyer::Database::Sqlite(fileName);
        initSchema(db->connection_);
#else
        throw Exception("ROSE was not configured with SQLite");
#endif
    }

    if (testSuiteName) {
        auto testSuite = TestSuite::instance(*testSuiteName);
        db->testSuiteId_ = db->id(testSuite);
    } else {
        initTestSuite(db);
    }
    return db;
}
    

// class method
Database::Ptr
Database::create(const std::string &url) {
    return create(url, Sawyer::Nothing());
}

// class method
Database::Ptr
Database::create(const std::string &url, const std::string &testSuiteName) {
    return create(url, Sawyer::Optional<std::string>(testSuiteName));
}

// class method
std::string
Database::timestamp() {
    time_t now = time(NULL);
    struct tm tm;
    gmtime_r(&now, &tm);
    return (boost::format("%04d-%02d-%02d %02d:%02d:%02d")
            % (tm.tm_year+1900) % (tm.tm_mon+1) % tm.tm_mday
            % tm.tm_hour % tm.tm_min % tm.tm_sec).str();
}

std::vector<TestSuiteId>
Database::testSuites() {
    std::vector<TestSuiteId> retval;
    for (auto row: connection_.stmt("select id from test_suites order by created_ts"))
        retval.push_back(TestSuiteId(*row.get<size_t>(0)));
    return retval;
}

TestSuite::Ptr
Database::testSuite() {
    return testSuiteId_ ? object(testSuiteId_) : TestSuite::Ptr();
}

TestSuiteId
Database::testSuite(const TestSuite::Ptr &ts) {
    if (ts) {
        testSuiteId_ = id(ts);
    } else {
        testSuiteId_.reset();
    }
    return testSuiteId_;
}

std::vector<SpecimenId>
Database::specimens() {
    if (testSuiteId_) {
        return specimens(testSuiteId_);
    } else {
        std::vector<SpecimenId> retval;
        Sawyer::Database::Statement stmt = connection_.stmt("select id from specimens order by created_ts");
        for (auto row: stmt)
            retval.push_back(SpecimenId(*row.get<size_t>(0)));
        return retval;
    }
}

std::vector<SpecimenId>
Database::specimens(TestSuiteId testSuiteId) {
    ASSERT_require(testSuiteId);
    ROSE_UNUSED(testSuiteId);

    Sawyer::Database::Statement stmt;
    stmt = connection_.stmt("select id from specimens where test_suite = ?tsid order by created_ts")
           .bind("tsid", *testSuiteId_);
    std::vector<SpecimenId> retval;
    for (auto row: stmt)
        retval.push_back(SpecimenId(row.get<size_t>(0)));
    return retval;
}

void
Database::eraseSpecimens(TestSuiteId testSuiteId) {
    ASSERT_require(testSuiteId);
    for (SpecimenId sid: specimens(testSuiteId))
        erase(sid);
}

std::vector<TestCaseId>
Database::testCases() {
    std::vector<TestCaseId> retval;
    Sawyer::Database::Statement stmt;
    if (testSuiteId_) {
        stmt = connection_.stmt("select id from test_cases where test_suite = ?tsid order by created_ts")
               .bind("tsid", *testSuiteId_);
    } else {
        stmt = connection_.stmt("select id from test_cases                          order by created_ts");
    }
    for (auto row: stmt)
        retval.push_back(TestCaseId(row.get<size_t>(0)));
    return retval;
}

std::vector<TestCaseId>
Database::testCases(SpecimenId specimenId) {
    ASSERT_require(specimenId);
    Sawyer::Database::Statement stmt;
    stmt = connection_.stmt("select id from test_cases where specimen = ?sid order by created_ts")
           .bind("sid", *specimenId);
    std::vector<TestCaseId> retval;
    for (auto row: stmt)
        retval.push_back(TestCaseId(row.get<size_t>(0)));
    return retval;
}

void
Database::eraseTestCases(SpecimenId specimenId) {
    ASSERT_require(specimenId);
    for (TestCaseId tcid: testCases(specimenId))
        erase(tcid);
}

std::vector<ExecutionEventId>
Database::executionEvents() {
    std::vector<ExecutionEventId> retval;
    Sawyer::Database::Statement stmt;
    if (testSuiteId_) {
        stmt = connection_.stmt("select id from execution_events where test_suite = ?tsid order by created_ts"
                                " order by location_primary, location_when, location_secondary")
               .bind("tsid", *testSuiteId_);
    } else {
        stmt = connection_.stmt("select id from execution_units order by created_ts");
    }
    for (auto row: stmt)
        retval.push_back(ExecutionEventId(row.get<size_t>(0)));
    return retval;
}

std::vector<ExecutionEventId>
Database::executionEvents(TestCaseId tcid) {
    ASSERT_require(tcid);
    Sawyer::Database::Statement stmt;
    stmt = connection_.stmt("select id from execution_events where test_case = ?tcid"
                            " order by location_primary, location_when, location_secondary")
           .bind("tcid", *tcid);
    std::vector<ExecutionEventId> retval;
    for (auto row: stmt)
        retval.push_back(ExecutionEventId(row.get<size_t>(0)));
    return retval;
}

std::vector<ExecutionEventId>
Database::executionEventsSince(TestCaseId tcid, ExecutionEventId startingAtId) {
    ASSERT_require(tcid);
    ASSERT_require(startingAtId);
    ROSE_UNUSED(tcid);

    ExecutionEvent::Ptr startingAt = object(startingAtId, Update::NO);

    int when = -1;
    switch (startingAt->location().when()) {
        case When::PRE:
            when = 0;
            break;
        case When::POST:
            when = 1;
            break;
    }
    ASSERT_forbid(-1 == when);

    Sawyer::Database::Statement stmt;
    stmt = connection_.stmt("select id from execution_events"
                            " where (location_primary > ?location_primary)"
                            " or (location_primary = ?location_primary and location_when > ?location_when)"
                            " or (location_primary = ?location_primary and location_when = ?location_when and"
                            "     location_secondary > ?location_secondary)"
                            " order by location_primary, location_when, location_secondary")
           .bind("location_primary", startingAt->location().primary())
           .bind("location_secondary", startingAt->location().secondary())
           .bind("location_when", when);
    std::vector<ExecutionEventId> retval;
    for (auto row: stmt)
        retval.push_back(ExecutionEventId(row.get<size_t>(0)));
    return retval;
}

size_t
Database::nExecutionEvents(TestCaseId tcid) {
    ASSERT_require(tcid);
    return connection_.stmt("select count(*) from execution_events where test_case = ?tcid")
        .bind("tcid", *tcid)
        .get<size_t>()
        .orElse(0);
}

std::vector<ExecutionEventId>
Database::executionEvents(TestCaseId tcid, uint64_t primaryKey) {
    ASSERT_require(tcid);
    Sawyer::Database::Statement stmt;
    stmt = connection_.stmt("select id from execution_events"
                            " where test_case = ?tcid and location_primary = ?location_primary"
                            " order by location_primary, location_when, location_secondary")
           .bind("tcid", *tcid)
           .bind("location_primary", primaryKey);
    std::vector<ExecutionEventId> retval;
    for (auto row: stmt)
        retval.push_back(ExecutionEventId(row.get<size_t>(0)));
    return retval;
}

std::vector<uint64_t>
Database::executionEventKeyFrames(TestCaseId tcid) {
    ASSERT_require(tcid);
    Sawyer::Database::Statement stmt;
    stmt = connection_.stmt("select distinct location_primary from execution_events"
                            " where test_case = ?tcid"
                            " order by location_primary, location_when, location_secondary")
           .bind("tcid", *tcid);
    std::vector<uint64_t> retval;
    for (auto row: stmt)
        retval.push_back(*row.get<uint64_t>(0));
    return retval;
}

void
Database::eraseExecutionEvents(TestCaseId tcid) {
    ASSERT_require(tcid);
    for (ExecutionEventId eeid: executionEvents(tcid))
        erase(eeid);
}

TestSuite::Ptr
Database::object(TestSuiteId id, Update update) {
    return objectHelper(sharedFromThis(), id, update, testSuites_, "test suite");
}

TestCase::Ptr
Database::object(TestCaseId id, Update update) {
    return objectHelper(sharedFromThis(), id, update, testCases_, "test case");
}

Specimen::Ptr
Database::object(SpecimenId id, Update update) {
    return objectHelper(sharedFromThis(), id, update, specimens_, "specimen");
}

ExecutionEvent::Ptr
Database::object(ExecutionEventId id, Update update) {
    return objectHelper(sharedFromThis(), id, update, executionEvents_, "execution event");
}

TestSuiteId
Database::id(const TestSuite::Ptr &obj, Update update) {
    return idHelper(sharedFromThis(), obj, update, testSuites_);
}

TestCaseId
Database::id(const TestCase::Ptr &obj, Update update) {
    return idHelper(sharedFromThis(), obj, update, testCases_);
}

SpecimenId
Database::id(const Specimen::Ptr &obj, Update update) {
    return idHelper(sharedFromThis(), obj, update, specimens_);
}

ExecutionEventId
Database::id(const ExecutionEvent::Ptr &obj, Update update) {
    return idHelper(sharedFromThis(), obj, update, executionEvents_);
}

TestSuiteId
Database::erase(TestSuiteId id) {
    return eraseHelper(sharedFromThis(), id, testSuites_, "test suite");
}

TestCaseId
Database::erase(TestCaseId id) {
    return eraseHelper(sharedFromThis(), id, testCases_, "test casse");
}

SpecimenId
Database::erase(SpecimenId id) {
    return eraseHelper(sharedFromThis(), id, specimens_, "specimen");
}

ExecutionEventId
Database::erase(ExecutionEventId id) {
    return eraseHelper(sharedFromThis(), id, executionEvents_, "execution event");
}

TestSuite::Ptr
Database::findTestSuite(const std::string &nameOrId) {
    try {
        if (auto id = connection_.stmt("select id from test_suites where name = ?name").bind("name", nameOrId).get<size_t>())
            return object(TestSuiteId(*id));
        TestSuiteId id(nameOrId);
        return object(id);
    } catch (...) {
        return TestSuite::Ptr();
    }
}

std::vector<SpecimenId>
Database::findSpecimensByName(const std::string &name) {
    std::vector<SpecimenId> retval;
    Sawyer::Database::Statement stmt;
    if (testSuiteId_) {
        stmt = connection_.stmt("select id from specimens where name = ?name and test_suite = ?ts").bind("ts", *testSuiteId_);
    } else {
        stmt = connection_.stmt("select id from specimens where name = ?name");
    }
    for (auto row: stmt.bind("name", name))
        retval.push_back(SpecimenId(row.get<size_t>(0)));
    return retval;
}

bool
Database::rbaExists(SpecimenId id) {
    return connection_.stmt("select count(*) from specimens where id = ?id and rba is not null")
        .bind("id", *id).get<size_t>().orElse(0) != 0;
}

void
Database::saveRbaFile(const boost::filesystem::path &fileName, SpecimenId specimenId) {
    // Open the file
    std::ifstream in(fileName.string().c_str(), std::ios_base::binary);
    if (!in)
        throw Exception("cannot read file \"" + StringUtility::cEscape(fileName.string()) + "\"");

    // Read the file in chunks and write it to the database
    const size_t rbaId = connection().stmt("select max(id) from blob_parts").get<size_t>().orElse(0) + 1;
    const size_t maxPartSize = (size_t)500 * 1024 * 1024;
    std::vector<uint8_t> part;
    part.resize(maxPartSize);
    for (size_t partNumber = 0; /*void*/; ++partNumber) {
        in.read((char*)part.data(), maxPartSize);
        const size_t nBytes = in.gcount();
        if (0 == nBytes) {
            break;
        } else if (nBytes < maxPartSize) {
            part.resize(nBytes);
        }

        connection().stmt("insert into blob_parts (id, part_number, part) values (?id, ?part_number, ?part)")
            .bind("id", rbaId)
            .bind("part_number", partNumber)
            .bind("part", part)
            .run();

        if (nBytes < maxPartSize)
            break;
    }

    // Save the RBA ID number
    connection().stmt("update specimens set rba = ?rbaId where id = ?specimenId")
        .bind("rbaId", rbaId)
        .bind("specimenId", *specimenId)
        .run();
}

void
Database::extractRbaFile(const boost::filesystem::path &fileName, SpecimenId specimenId) {
    // Open/create the output file
    std::ofstream out(fileName.string().c_str(), std::ios_base::binary);
    if (!out)
        throw Exception("cannot create or truncate file \"" + StringUtility::cEscape(fileName.string()) + "\"");

    // Get the RBA ID number
    const auto rbaId = connection().stmt("select rba from specimens where id = ?id").bind("id", *specimenId).get<size_t>();
    if (!rbaId)
        throw Exception("no RBA data associated with specimen " + boost::lexical_cast<std::string>(*specimenId));

    // Get the RBA parts from the parts table and write each one to the output.
    auto stmt = connection().stmt("select part from blob_parts where id = ?id order by part_number").bind("id", *rbaId);
    for (auto row: stmt) {
        using Iter = std::ostream_iterator<uint8_t>;
        const auto part = row.get<std::vector<uint8_t>>(0);
        std::copy(part.get().begin(), part.get().end(), Iter(out));
    }
}

void
Database::eraseRba(SpecimenId id) {
    ASSERT_require(id);

    if (const auto rbaId = connection().stmt("select rba from specimens where id = ?id").bind("id", *id).get<size_t>()) {
        connection().stmt("update specimens set rba = null where id = ?id").bind("id", *id).run();
        connection().stmt("delete from blob_parts where id = ?id").bind("id", *rbaId).run();
    }
}

bool
Database::symbolicStateExists(TestCaseId id) {
    ASSERT_require(id);
    return connection().stmt("select count(*) from test_cases where id = ?id and symbolic_state is not null")
        .bind("id", *id).get<size_t>().orElse(0) != 0;
}

void
Database::saveSymbolicState(TestCaseId id, const BS::State::Ptr &state) {
    ASSERT_require(id);
    if (state) {
        Sawyer::FileSystem::TemporaryFile tempFile;
        {
            boost::archive::binary_oarchive archive(tempFile.stream());
            registerTypes(archive);
            archive <<state;
        }
        tempFile.stream().close();

        std::ifstream in(tempFile.name().string().c_str(), std::ios_base::binary);
        ASSERT_require2(in, "cannot read " + tempFile.name().string());
        using Iter = std::istreambuf_iterator<char>;
        std::vector<uint8_t> data;
        data.reserve(boost::filesystem::file_size(tempFile.name()));
        data.assign(Iter(in), Iter());
        connection().stmt("update test_cases set symbolic_state = ?data where id = ?id")
            .bind("id", *id)
            .bind("data", data)
            .run();
    } else {
        eraseSymbolicState(id);
    }
}

BS::State::Ptr
Database::extractSymbolicState(TestCaseId id) {
    ASSERT_require(id);

    // Get the data from the database
    auto data = connection().stmt("select symbolic_state from test_cases where id = ?id")
                .bind("id", *id)
                .get<std::vector<uint8_t>>();
    if (!data)
        throw Exception("no symbolic state associated with test case " + boost::lexical_cast<std::string>(*id));

    // Write the data to a file
    Sawyer::FileSystem::TemporaryFile tempFile;
    using Iter = std::ostream_iterator<uint8_t>;
    std::copy(data->begin(), data->end(), Iter(tempFile.stream()));
    tempFile.stream().close();

    // Unserialize from the file
    std::ifstream in(tempFile.name().string().c_str(), std::ios_base::binary);
    boost::archive::binary_iarchive archive(in);
    registerTypes(archive);
    BS::State::Ptr retval;
    archive >>retval;
    return retval;
}

void
Database::eraseSymbolicState(TestCaseId id) {
    ASSERT_require(id);
    connection().stmt("update test_cases set symbolic_state = null where id = ?id").bind("id", *id).run();
}

bool
Database::concreteResultExists(TestCaseId id) {
    return connection_.stmt("select count(*) from test_cases where id = ?id and concrete_result is not null")
        .bind("id", *id).get<size_t>().orElse(0) != 0;
}

void
Database::saveConcreteResult(const TestCase::Ptr &testCase, const ConcreteResult::Ptr &details) {
    ASSERT_not_null(testCase);
    TestCaseId id = save(testCase);

    if (details) {
        double rank = details->rank();
        if (rose_isnan(rank))
            rank = 0.0;
        bool interesting = details->isInteresting();

        std::stringstream ss;
        {
            boost::archive::xml_oarchive archive(ss);
            registerTypes(archive);
            archive <<BOOST_SERIALIZATION_NVP(details);
        }
        connection().stmt("update test_cases"
                          " set concrete_rank = ?rank, concrete_interesting = ?interesting, concrete_result = ?details"
                          " where id = ?id")
            .bind("rank", rank)
            .bind("interesting", interesting ? 1 : 0)
            .bind("details", ss.str())
            .bind("id", *id)
            .run();

        testCase->concreteRank(rank);
        testCase->concreteIsInteresting(interesting);

    } else {
        connection().stmt("update test_cases"
                          " set concrete_rank = null, concrete_interesting = 0, concrete_result = null"
                          " where id = ?id")
            .bind("id", *id)
            .run();

        testCase->concreteRank(Sawyer::Nothing());
        testCase->concreteIsInteresting(false);
    }

    save(testCase);
}

ConcreteResult::Ptr
Database::readConcreteResult(TestCaseId id) {
    Sawyer::Optional<std::string> bytes = connection().stmt("select concrete_result from test_cases where id = ?id")
                                          .bind("id", *id)
                                          .get<std::string>();
    if (bytes) {
        std::istringstream ss(*bytes);
        boost::archive::xml_iarchive archive(ss);
        registerTypes(archive);
        ConcreteResult::Ptr details;
        archive >> BOOST_SERIALIZATION_NVP(details);
        return details;
    } else {
        return {};
    }
}

void
Database::assocTestCaseWithTestSuite(TestCaseId testCase, TestSuiteId testSuite) {
    connection().stmt("update specimen set test_suite = ?ts where id = ?tc")
        .bind("tc", *testCase)
        .bind("ts", *testSuite)
        .run();
}

std::vector<TestCaseId>
Database::needConcreteTesting(size_t n) {
    n = std::min(n, (size_t)INT_MAX);                   // some databases can't handle numbers this big
    Sawyer::Database::Statement stmt;
    if (testSuiteId_) {
        stmt = connection().stmt("select id from test_cases where concrete_rank is null and test_suite = ?ts"
                                 " order by created_ts limit ?n").bind("ts", *testSuiteId_);
    } else {
        stmt = connection().stmt("select id from test_cases where concrete_rank is null"
                                 " order by created_ts limit ?n");
    }
    std::vector<TestCaseId> retval;
    for (auto row: stmt.bind("n", n))
        retval.push_back(TestCaseId(*row.get<size_t>(0)));
    return retval;
}

std::vector<TestCaseId>
Database::needConcolicTesting(size_t n) {
    n = std::min(n, (size_t)INT_MAX);                   // some databases can't handle numbers this big
    Sawyer::Database::Statement stmt;
    if (testSuiteId_) {
        stmt = connection().stmt("select id from test_cases where concolic_result is null and test_suite = ?ts"
                                 " order by created_ts limit ?n").bind("ts", *testSuiteId_);
    } else {
        stmt = connection().stmt("select id from test_cases where concolic_result is null"
                                 " order by created_ts limit ?n");
    }
        
    std::vector<TestCaseId> retval;
    for (auto row: stmt.bind("n", n))
        retval.push_back(TestCaseId(*row.get<size_t>(0)));
    return retval;
}

bool
Database::hasUntested() {
    return !needConcreteTesting(1).empty() || !needConcolicTesting(1).empty();
}

} // namespace
} // namespace
} // namespace

#endif
