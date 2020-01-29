// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#if __cplusplus >= 201103L

#ifndef DRIVER
#define DRIVER 1
#endif

#if DRIVER == 1
    #include <Sawyer/DatabaseSqlite.h>
#elif DRIVER == 2
    #include <Sawyer/DatabasePostgresql.h>
#else
    #error "invalid database driver number"
#endif

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace Sawyer::Database;

Statement globalStmt;

// Initialize the global statement but never run it.
static void initGlobalStmt(Connection db) {
    globalStmt = db.stmt("create table table_z (name text)");
}

// Basic API
static void test01(Connection db) {
    db.run("drop table if exists table_a");
    db.run("create table table_a (id integer primary key, name text)");
    db.run("insert into table_a (id, name) values (1, 'alice')");
}

// Iteration
static void test02(Connection db) {
    Statement stmt = db.stmt("select name from table_a limit 1");
    Iterator iter = stmt.begin();
    Row row = *iter;
    std::string name = *row.get<std::string>(0);
    ASSERT_always_require2(name == "alice", name);
}

// Binding
static void test03(Connection db) {
    Statement stmt = db.stmt("insert into table_a (id, name) values (?id, ?name)");
    stmt.bind("id", 2);
    stmt.bind("name", "betty");
    stmt.run();
}

// Multi-row, multi-column results
static void test04(Connection db) {
    Statement stmt = db.stmt("select id, name from table_a order by name");
    Iterator iter = stmt.begin();
    ASSERT_always_require(iter != stmt.end());
    int id = *iter->get<int>(0);
    std::string name = *iter->get<std::string>(1);
    ASSERT_always_require2(id == 1, boost::lexical_cast<std::string>(id));
    ASSERT_always_require2(name == "alice", name);

    ++iter;
    ASSERT_always_require(iter != stmt.end());
    id = *iter->get<int>(0);
    name = *iter->get<std::string>(1);
    ASSERT_always_require2(id == 2, boost::lexical_cast<std::string>(id));
    ASSERT_always_require2(name == "betty", name);

    ++iter;
    ASSERT_always_require(iter == stmt.end());
}

// Special characters
static void
test05(Connection db) {
    db.run("drop table if exists table_b");
    db.run("create table table_b (s bytea)");

    std::string s = "\377\177"; // WARNING: not valid UTF-8
    db.stmt("insert into table_b (s) values (?s)").bind("s", s).run();
    Iterator iter = db.stmt("select s from table_b").begin();
    std::string answer = *iter->get<std::string>(0);
    ASSERT_always_require(answer == s);
}

// Saving and reading null values
static void
test06(Connection db) {
    auto stmt = db.stmt("insert into table_b (s) values (?s)");
    stmt.bind("s", Sawyer::Nothing());
    stmt.run();

    const Sawyer::Optional<std::string> nothing;
    stmt.bind("s", nothing);

    stmt = db.stmt("select s from table_b where s is null");
    for (auto row: stmt) {
        auto s = row.get<std::string>(0);
        ASSERT_always_forbid2(s, "expected null");
    }
}

// Queries that return no rows vs. a row containing a null value
static void
test07(Connection db) {
    // Returns a non-null value
    auto val = db.get<std::string>("select s from table_b where s is not null");
    ASSERT_always_require(val);
    ASSERT_always_require(*val != "");

    // Returns a null value
    val = db.get<std::string>("select s from table_b where s is null");
    ASSERT_always_forbid(val);

    // Does not return any row
    val = db.get<std::string>("select s from table_b where s = 'does not exist'");
    ASSERT_always_forbid(val);
}

// Tests that a parameter can appear more than once in a query
static void
test08(Connection db) {
    db.run("drop table if exists table_c");
    db.run("create table table_c (firstname text, lastname text)");
    db.stmt("insert into table_c (firstname, lastname) values (?s, ?s)")
        .bind("s", "Morgan")
        .run();

    for (auto row: db.stmt("select firstname, lastname from table_c")) {
        auto firstname = row.get<std::string>(0);
        auto lastname = row.get<std::string>(1);
        ASSERT_always_require(firstname);
        ASSERT_always_require(lastname);
        ASSERT_always_require(*firstname == *lastname);
    }
}

// Tests that save large data objects
static void
test09(Connection db) {
    db.run("drop table if exists table_d");
    db.run("create table table_d (name text, data bytea)");

    // Save some large data
    std::string name1(5000, 'a');
    std::vector<uint8_t> data1(5000, 0xff);
    db.stmt("insert into table_d (name, data) values (?name, ?data)")
        .bind("name", name1)
        .bind("data", data1)
        .run();

    // Query the large data
    for (auto row: db.stmt("select name, data from table_d")) {
        auto name = row.get<std::string>(0);
        auto data = row.get<std::vector<uint8_t>>(1);
        ASSERT_always_require(name);
        ASSERT_always_require(data);
        ASSERT_always_require(*name == name1);
        ASSERT_always_require(*data == data1);
    }
}

int main() {

#if DRIVER == 1
    boost::filesystem::path dbName = "test" + boost::lexical_cast<std::string>(DRIVER) + ".db";
    boost::system::error_code ec;
    boost::filesystem::remove(dbName, ec);
    Sqlite db(dbName.native());
#elif DRIVER == 2
    // Set up the test with these commands:
    //   sudo su postgres
    //   psql
    //     create database sawyer_testing;
    //     create user sawyer with encrypted password 'the_password';
    //     grant all privileges on database sawyer_testing to sawyer;
    Postgresql::Locator where;
    where.hostname = "localhost";
    where.user = "sawyer";
    where.password = "the_password";
    where.database = "sawyer_testing";
    Postgresql db(where);
#else
    #error "invalid database driver number"
#endif

    initGlobalStmt(db);
    test01(db);
    test02(db);
    test03(db);
    test04(db);
    test05(db);
    test06(db);
    test07(db);
    test08(db);
    test09(db);
}

#else

#include <iostram>
int main() {
    std::cerr <<"not tested; needs C++11 or later\n";
}

#endif
