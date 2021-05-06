// Second implementation of Concolic::Database that works for both SQLite and PostgreSQL
// The schema is not identical to that of the first implementation.
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>

#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/LinuxTraceExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Concolic/TestSuite.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/regex.hpp>
#include <boost/serialization/nvp.hpp>

#include <ctime>
#include <fstream>

#ifdef ROSE_HAVE_SQLITE3
#include <Sawyer/DatabaseSqlite.h>
#endif

#ifdef ROSE_HAVE_LIBPQXX
#include <Sawyer/DatabasePostgresql.h>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

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
           " rba bytea,"                                // maybe null
           " test_suite integer not null,"
           " constraint fk_test_suite foreign key (test_suite) references test_suites (id))");

    db.run("drop table if exists test_cases");
    db.run("create table test_cases ("
           " id integer primary key,"
           " created_ts varchar(32) not null,"
           " name text not null,"
           " executor text not null,"
           " specimen integer not null,"
           " argv bytea not null,"
           " envp bytea not null,"
           " concrete_rank real,"                       // null if concrete executor not run yet
           " concrete_result bytea,"                    // null if concrete executor not run yet
           " concolic_result integer,"                  // non-null if concolic executor has been run
           " concrete_interesting integer not null default 1," // concrete results are uninteresting if present? (bool)
           " test_suite integer not null,"
           " constraint fk_specimen foreign key (specimen) references specimens (id),"
           " constraint fk_test_suite foreign key (test_suite) references test_suites (id))");

    db.run("drop table if exists execution_events");
    db.run("create table execution_events ("
           " created_ts varchar(32) not null,"
           " id integer primary key,"
           " test_suite integer not null,"

           // Identification
           " test_case integer not null,"               // test case to which this event belongs
           " location integer not null,"                // event location field
           " sequence_number integer not null,"         // events sequence field
           " instruction_pointer integer not null,"     // value of the instruction pointer register at this event
           " comment varchar(32) not null,"             // arbitrary comment for debugging

           // Actions. The interpretation of these fields depends on the action type.
           " action_type varchar(32) not null,"         // action to be performed
           " start_va integer,"                         // starting address for actions that need one
           " scalar integer,"                           // scalar value for those actions that need one
           " bytes bytea,"                              // vector value for those actions that need one

           "constraint fk_test_suite foreign key (test_suite) references test_suites (id),"
           "constraint fk_test_case foreign key (test_case) references test_cases (id))");
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

static std::string
timestamp() {
    time_t now = time(NULL);
    struct tm tm;
    gmtime_r(&now, &tm);
    return (boost::format("%04d-%02d-%02d %02d:%02d:%02d")
            % (tm.tm_year+1900) % (tm.tm_mon+1) % tm.tm_mday
            % tm.tm_hour % tm.tm_min % tm.tm_sec).str();
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
        std::string ts = obj->timestamp().empty() ? timestamp() : obj->timestamp();
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
        std::string ts = obj->timestamp().empty() ? timestamp() : obj->timestamp();
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
                                      // 8
                                      " concrete_interesting"
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
        stmt = db->connection().stmt("update test_cases set name = ?name, executor = ?executor,"
                                     " specimen = ?specimen, argv = ?argv, envp = ?envp, concrete_rank = ?concrete_rank,"
                                     " concrete_interesting = ?interesting,"
                                     " concolic_result = ?concolic_result where id = ?id");
    } else {
        std::string ts = obj->timestamp().empty() ? timestamp() : obj->timestamp();
        obj->timestamp(ts);
        stmt = db->connection().stmt("insert into test_cases (id, created_ts, name, executor, specimen, argv, envp,"
                                     " concrete_rank, concrete_interesting, concolic_result, test_suite)"
                                     " values (?id, ?ts, ?name, ?executor, ?specimen, ?argv, ?envp,"
                                     " ?concrete_rank, ?interesting, ?concolic_result, ?test_suite)")
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

    //                                        0           1          2         3                4
    auto iter = db->connection().stmt("select created_ts, test_case, location, sequence_number, instruction_pointer,"
                                      // 5           6         7       8      9
                                      " action_type, start_va, scalar, bytes, comment"
                                      " from execution_events"
                                      " where id = ?id"
                                      " order by created_ts")
                .bind("id", *id)
                .begin();
    if (!iter)
        throw Exception("no such executon event in database where id=" + boost::lexical_cast<std::string>(*id));

    TestCaseId tcid(*iter->get<size_t>(1));
    TestCase::Ptr testcase = db->object(tcid);
    ASSERT_not_null(testcase);

    obj->timestamp(*iter->get<std::string>(0));
    obj->testCase(testcase);
    obj->location(ExecutionEvent::Location(*iter->get<uint64_t>(2), *iter->get<size_t>(3)));
    obj->instructionPointer(*iter->get<rose_addr_t>(4));
    obj->comment(*iter->get<std::string>(9));

    std::string action = *iter->get<std::string>(5);
    auto startVa = iter->get<rose_addr_t>(6);
    auto scalar = iter->get<size_t>(7);
    auto bytes = iter->get<std::vector<uint8_t>>(8);

    if ("none" == action) {
        obj->actionType(ExecutionEvent::Action::NONE);
    } else if ("map_memory" == action) {
        obj->actionType(ExecutionEvent::Action::MAP_MEMORY);
        ASSERT_require(startVa);
        ASSERT_require(scalar);
        obj->memoryLocation(AddressInterval::baseSize(*startVa, *scalar));
        ASSERT_require(bytes);
        obj->bytes(*bytes);
    } else if ("unmap_memory" == action) {
        obj->actionType(ExecutionEvent::Action::UNMAP_MEMORY);
        ASSERT_require(startVa);
        ASSERT_require(scalar);
        obj->memoryLocation(AddressInterval::baseSize(*startVa, *scalar));
    } else if ("write_memory" == action) {
        obj->actionType(ExecutionEvent::Action::WRITE_MEMORY);
        ASSERT_require(startVa);
        ASSERT_require(scalar);
        obj->memoryLocation(AddressInterval::baseSize(*startVa, *scalar));
        ASSERT_require(bytes);
        ASSERT_require(bytes->size() == *scalar);
        obj->bytes(*bytes);
    } else if ("hash_memory" == action) {
        obj->actionType(ExecutionEvent::Action::HASH_MEMORY);
        ASSERT_require(startVa);
        ASSERT_require(scalar);
        obj->memoryLocation(AddressInterval::baseSize(*startVa, *scalar));
        ASSERT_require(bytes);
        obj->bytes(*bytes);
    } else if ("write_register" == action) {
        obj->actionType(ExecutionEvent::Action::WRITE_REGISTER);
        ASSERT_require(scalar);
        obj->scalar(*scalar);
        ASSERT_require(bytes);
        obj->bytes(*bytes);
    } else if ("restore_registers" == action) {
        obj->actionType(ExecutionEvent::Action::RESTORE_REGISTERS);
        ASSERT_require(bytes);
        obj->bytes(*bytes);
    } else if ("os_syscall" == action) {
        obj->actionType(ExecutionEvent::Action::OS_SYSCALL);
        ASSERT_require(scalar);
        obj->scalar(*scalar);
        ASSERT_require(bytes);
        obj->bytes(*bytes);
    } else {
        throw Exception("unrecognized execution action \"" + action + "\" where id=" + boost::lexical_cast<std::string>(*id));
    }
}

static void
updateDb(const Database::Ptr &db, ExecutionEventId id, const ExecutionEvent::Ptr &obj) {
    ASSERT_not_null(db);
    ASSERT_require(id);
    ASSERT_not_null(obj);

    Sawyer::Database::Statement stmt;
    if (*db->connection().stmt("select count(*) from execution_events where id = ?id").bind("id", *id).get<size_t>()) {
        stmt = db->connection().stmt("update execution_events set "
                                     "test_case = ?test_case, location = ?location, sequence_number = ?sequence_number,"
                                     " instruction_pointer = ?instruction_pointer, action_type = ?action_type, start_va = ?start_va,"
                                     " scalar = ?scalar, bytes = ?bytes, comment = ?comment"
                                     " where id = ?id");
    } else {
        std::string ts = obj->timestamp().empty() ? timestamp() : obj->timestamp();
        obj->timestamp(ts);
        stmt = db->connection().stmt("insert into execution_events ("
                                     " id, created_ts, test_suite,"
                                     " test_case, location, sequence_number, instruction_pointer, action_type, start_va,"
                                     " scalar, bytes, comment"
                                     ") values ("
                                     " ?id, ?created_ts, ?test_suite,"
                                     " ?test_case, ?location, ?sequence_number, ?instruction_pointer, ?action_type, ?start_va,"
                                     " ?scalar, ?bytes, ?comment"
                                     ")")
               .bind("created_ts", ts)
               .bind("test_suite", *db->id(db->testSuite()));
    }

    stmt
        .bind("id", *id)
        .bind("test_case", *db->id(obj->testCase(), Update::YES))
        .bind("location", obj->location().primary)
        .bind("sequence_number", obj->location().secondary)
        .bind("instruction_pointer", obj->instructionPointer())
        .bind("comment", obj->comment());

    switch (obj->actionType()) {
        case ExecutionEvent::Action::NONE:
            stmt.bind("action_type", "none");
            stmt.bind("start_va", Sawyer::Nothing());
            stmt.bind("scalar", Sawyer::Nothing());
            stmt.bind("bytes", Sawyer::Nothing());
            break;
        case ExecutionEvent::Action::MAP_MEMORY:
            stmt.bind("action_type", "map_memory");
            stmt.bind("start_va", obj->memoryLocation().least());
            stmt.bind("scalar", obj->memoryLocation().size());
            stmt.bind("bytes", obj->bytes());
            break;
        case ExecutionEvent::Action::UNMAP_MEMORY:
            stmt.bind("action_type", "unmap_memory");
            stmt.bind("start_va", obj->memoryLocation().least());
            stmt.bind("scalar", obj->memoryLocation().size());
            stmt.bind("bytes", Sawyer::Nothing());
            break;
        case ExecutionEvent::Action::WRITE_MEMORY:
            stmt.bind("action_type", "write_memory");
            stmt.bind("start_va", obj->memoryLocation().least());
            stmt.bind("scalar", obj->memoryLocation().size());
            ASSERT_require(obj->bytes().size() == obj->memoryLocation().size());
            stmt.bind("bytes", obj->bytes());
            break;
        case ExecutionEvent::Action::HASH_MEMORY:
            stmt.bind("action_type", "hash_memory");
            stmt.bind("start_va", obj->memoryLocation().least());
            stmt.bind("scalar", obj->memoryLocation().size());
            stmt.bind("bytes", obj->bytes());
            break;
        case ExecutionEvent::Action::WRITE_REGISTER:
            stmt.bind("action_type", "write_register");
            stmt.bind("start_va", Sawyer::Nothing());
            stmt.bind("scalar", obj->scalar());
            stmt.bind("bytes", obj->bytes());
            break;
        case ExecutionEvent::Action::RESTORE_REGISTERS:
            stmt.bind("action_type", "restore_registers");
            stmt.bind("start_va", Sawyer::Nothing());
            stmt.bind("scalar", Sawyer::Nothing());
            stmt.bind("bytes", obj->bytes());
            break;
        case ExecutionEvent::Action::OS_SYSCALL:
            stmt.bind("action_type", "os_syscall");
            stmt.bind("start_va", Sawyer::Nothing());
            stmt.bind("scalar", obj->scalar());
            stmt.bind("bytes", obj->bytes());
    }

    stmt.run();
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
objectHelper(const Database::Ptr &db, const typename IdObjMap::Source &id, Update::Flag update, IdObjMap &objMap,
             const std::string &objectTypeName) {
    if (!id)
        throw Exception("invalid " + objectTypeName + " ID");
    auto obj = objMap.forward().getOrDefault(id);
    if (!obj) {
        obj = IdObjMap::Target::Pointee::instance();
        updateObject(db, id, obj);
        objMap.insert(id, obj);
    } else {
        updateObject(db, id, obj);
    }
    ASSERT_not_null(obj);
    return obj;
}

// Helper template function for the Database::id method
template<class IdObjMap>
static typename IdObjMap::Source // ID
idHelper(const Database::Ptr &db, const typename IdObjMap::Target &obj, Update::Flag &update, IdObjMap &objMap) {
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
        stmt = connection_.stmt("select id from execution_events where test_suite = ?tsid order by created_ts")
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
    stmt = connection_.stmt("select id from execution_events where test_case = ?tcid order by location, sequence_number")
           .bind("tcid", *tcid);
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
                            " where test_case = ?tcid and location = ?locprim"
                            " order by sequence_number")
           .bind("tcid", *tcid)
           .bind("locprim", primaryKey);
    std::vector<ExecutionEventId> retval;
    for (auto row: stmt)
        retval.push_back(ExecutionEventId(row.get<size_t>(0)));
    return retval;
}

std::vector<uint64_t>
Database::executionEventKeyFrames(TestCaseId tcid) {
    ASSERT_require(tcid);
    Sawyer::Database::Statement stmt;
    stmt = connection_.stmt("select distinct location from execution_events"
                            " where test_case = ?tcid"
                            " order by location")
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
Database::object(TestSuiteId id, Update::Flag update) {
    return objectHelper(sharedFromThis(), id, update, testSuites_, "test suite");
}

TestCase::Ptr
Database::object(TestCaseId id, Update::Flag update) {
    return objectHelper(sharedFromThis(), id, update, testCases_, "test case");
}

Specimen::Ptr
Database::object(SpecimenId id, Update::Flag update) {
    return objectHelper(sharedFromThis(), id, update, specimens_, "specimen");
}

ExecutionEvent::Ptr
Database::object(ExecutionEventId id, Update::Flag update) {
    return objectHelper(sharedFromThis(), id, update, executionEvents_, "execution event");
}

TestSuiteId
Database::id(const TestSuite::Ptr &obj, Update::Flag update) {
    return idHelper(sharedFromThis(), obj, update, testSuites_);
}

TestCaseId
Database::id(const TestCase::Ptr &obj, Update::Flag update) {
    return idHelper(sharedFromThis(), obj, update, testCases_);
}

SpecimenId
Database::id(const Specimen::Ptr &obj, Update::Flag update) {
    return idHelper(sharedFromThis(), obj, update, specimens_);
}

ExecutionEventId
Database::id(const ExecutionEvent::Ptr &obj, Update::Flag update) {
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
    if (auto id = connection_.stmt("select id from test_suites where name = ?name").bind("name", nameOrId).get<size_t>())
        return object(TestSuiteId(*id));
    try {
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
Database::saveRbaFile(const boost::filesystem::path &fileName, SpecimenId id) {
    std::ifstream in(fileName.string().c_str(), std::ios_base::binary);
    if (!in)
        throw Exception("cannot read file \"" + StringUtility::cEscape(fileName.string()) + "\"");
    using Iter = std::istreambuf_iterator<char>;
    std::vector<uint8_t> rba;
    rba.reserve(boost::filesystem::file_size(fileName));
    rba.assign(Iter(in), Iter());
    connection().stmt("update specimens set rba = ?rba where id = ?id")
        .bind("id", *id)
        .bind("rba", rba)
        .run();
}

void
Database::extractRbaFile(const boost::filesystem::path &fileName, SpecimenId id) {
    std::ofstream out(fileName.string().c_str(), std::ios_base::binary);
    if (!out)
        throw Exception("cannot create or truncate file \"" + StringUtility::cEscape(fileName.string()) + "\"");
    auto rba = connection().stmt("select rba from specimens where id = ?id").bind("id", *id).get<std::vector<uint8_t>>();
    if (!rba)
        throw Exception("no RBA data associated with specimen " + boost::lexical_cast<std::string>(*id));
    using Iter = std::ostream_iterator<uint8_t>;
    std::copy(rba.get().begin(), rba.get().end(), Iter(out));
}

void
Database::eraseRba(SpecimenId id) {
    connection().stmt("update specimens set rba = null where id = ?id").bind("id", *id).run();
}

bool
Database::concreteResultExists(TestCaseId id) {
    return connection_.stmt("select count(*) from test_cases where id = ?id and concrete_result is not null")
        .bind("id", *id).get<size_t>().orElse(0) != 0;
}

void
Database::saveConcreteResult(const TestCase::Ptr &testCase, const ConcreteExecutorResult *details) {
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
            archive.register_type<LinuxTraceExecutor::Result>();
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

std::unique_ptr<ConcreteExecutorResult>
Database::readConcreteResult(TestCaseId id) {
    ConcreteExecutorResult *details = nullptr;
    Sawyer::Optional<std::string> bytes = connection().stmt("select concrete_result from test_cases where id = ?id")
                                          .bind("id", *id)
                                          .get<std::string>();
    if (bytes) {
        std::istringstream ss(*bytes);
        boost::archive::xml_iarchive archive(ss);
        archive.register_type<LinuxTraceExecutor::Result>();
        archive >> BOOST_SERIALIZATION_NVP(details);
    }
    return std::unique_ptr<ConcreteExecutorResult>(details);
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
    return !needConcreteTesting(1).empty();
}

} // namespace
} // namespace
} // namespace

#endif
