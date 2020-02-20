// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Database_H
#define Sawyer_Database_H

#if __cplusplus >= 201103L

#include <boost/iterator/iterator_facade.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <memory.h>
#include <Sawyer/Assert.h>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <string>
#include <vector>

namespace Sawyer {

/** SQL database interface.
 *
 *  This interface is similar to the SQLite3 and PostgreSQL C APIs but using C++ idioms, such as:
 *
 *  @li The parameters in a prepared statement are numbered starting at zero instead of one, just as rows and columns are
 *  numbered starting at zero. The underlying APIs for SQLite3 and PostgreSQL are both inconsistent in this regard.
 *
 *  @li Unexpected errors are handled by throwing exceptions rather than returning a success or error code or by requiring
 *  a call to another error-query function.
 *
 *  @li The resulting rows of a query are accessed by single-pass iterators in the usual C++ way.
 *
 *  @li Result iterators report (by exception) when they're accessed after being invalidated by some operation on the statement
 *  or database.
 *
 *  @li This API uses namespaces and classes for encapsulation rather than using free functions in the global scope.
 *
 *  @li Quantities that are logically non-negative use unsigned types.
 *
 *  @li All objects are reference counted and destroyed automatically.
 *
 *  @li A prepared statement that's not currently executing doesn't count as a reference to the database, allowing prepared
 *  statements to be cached in places (global variables, objects, etc.) that might make them difficult to locate and destroy
 *  in order to remove all references to a database.
 *
 *  Here's a basic example to open or create a database:
 *
 * @code
 *  #include <Sawyer/Database.h>
 *  using namespace Sawyer::Database;
 *
 *  Connection db("the_database_file.db");
 * @endcode
 *
 *  Once a database is opened you can run queries. The simplest API is for queries that don't produce any output. This is a
 *  special case of a more general mechanism which we'll eventually get to:
 *
 * @code
 *  db.run("create table testing (id integer primary key, name text)");
 *  db.run("insert into testing (name) values ('Allan')");
 * @endcode
 *
 *  The next most complicated API is for queries that return a single value. Specifically this is any query that returns at
 *  least one row of results, where the first column of the first row is the value of interest.  Since the return value could
 *  be either a value or null, the return type is a @ref Sawyer::Optional, which allows you to do things like provide a
 *  default:
 *
 * @code
 *  std::string name = db.get<std::string>("select name from testing limit 1").orElse("Burt");
 * @endcode
 *
 *  Then there are queries that return multiple rows each having multiple columns with zero-origin indexing. Again, the "get"
 *  functions return a @ref Sawyer::Optional in order to handle null values.
 *
 * @code
 *  for (auto row: db.stmt("select id, name from testing"))
 *      std::cout <<"id=" <<row.get<int>(0).orElse(0) <<", name=" <<row.get<std::string>(1).orDefault() <<"\n";
 * @endcode
 *
 *  Finally, statements can have named parameters that get filled in later, possibly repeatedly:
 *
 * @code
 *  std::vector<std::pair<std::string, int>> data = ...;
 *  Statement stmt = db.stmt("insert into testing (id, name) values (?id, ?name)");
 *  for (auto record: data) {
 *      stmt.bind("name", record.first);
 *      stmt.bind("id", record.second);
 *      stmt.run();
 *  }
 * @endcode
 *
 *  Chaining of the member functions works too. Here's the same example again:
 *
 * @code
 *  std::vector<std::pair<std::string, int>> data = ...;
 *  Statement stmt = db.stmt("insert into testing (id, name) values (?id, ?name)");
 *  for (auto record: data)
 *      stmt.bind("name", record.first).bind("id", record.second).run();
 * @endcode
 *
 *  Here's a query that uses parameters:
 *
 * @code
 *  // parse the SQL once
 *  Statement stmt = db.stmt("select name from testing where id = ?id");
 *
 *  // later, bind the parameters to actual values and execute
 *  stmt.bind("id", 123);
 *  for (auto row: stmt)
 *      std::cout <<"name=" <<row.get<std::string>(0) <<"\n";
 * @endcode
 *
 *  Of course you can also iterate over results by hand too. And if you decide to stop early then the statement is automatically
 *  canceled:
 *
 * @code
 *  Statement stmt = db.stmt("select name from testing order by name");
 *
 *  for (Statement::Iterator row = stmt.begin(); row != stmt.end(); ++row) {
 *      std::string name = row->get<std::string>(0);
 *      std::cout <<"name = " <<name <<"\n";
 *      if ("foobar" == name)
 *          break;
 *  }
 * @endcode
 *
 *  Here's a small example showing how using idiomatic C++ instead of the lower-level C API can make programs easier to read.
 *  Both versions abort with an error message on any kind of SQLite error, although the second one does so by throwing
 *  exceptions. The original code:
 *
 * @code
 *  ASSERT_not_null(connection);
 *  std::string q = "insert into interesting (url, parent_page, reason, discovered) values (?,?,?,?)";
 *  sqlite3_stmt *stmt = nullptr;
 *  int status;
 *  ASSERT_always_require2(SQLITE_OK == (status = sqlite3_prepare_v2(connection_, q.c_str(), q.size()+1, &stmt, nullptr)),
 *                         sqlite3_errstr(status));
 *  ASSERT_always_require2(SQLITE_OK == (status = sqlite3_bind_text(stmt, 1, url.c_str(), -1, SQLITE_TRANSIENT)),
 *                         sqlite3_errstr(status));
 *  ASSERT_always_require2(SQLITE_OK == (status = sqlite3_bind_int64(stmt, 2, parentPage)),
 *                         sqlite3_errstr(status));
 *  ASSERT_always_require2(SQLITE_OK == (status = sqlite3_bind_text(stmt, 3, reason.c_str(), -1, SQLITE_STATIC)),
 *                         sqlite3_errstr(status));
 *  ASSERT_always_require2(SQLITE_OK == (status = sqlite3_bind_text(stmt, 4, timeStamp().c_str(), -1, SQLITE_TRANSIENT)),
 *                         sqlite3_errstr(status));
 *  ASSERT_always_require2(SQLITE_DONE == (status = sqlite3_step(stmt)), sqlite3_errstr(status));
 *  ASSERT_always_require2(SQLITE_OK == (status = sqlite3_finalize(stmt)), sqlite3_errstr(status)); stmt = nullptr;
 * @endcode
 *
 *  And here's the idiomatic C++ version:
 *
 * @code
 *  connection.stmt("insert into interesting (url, parent_page, reason, discovered)"
 *                  " values (?url, ?parent_page, ?reason, ?discovered)")
 *      .bind("url", url.str())
 *      .bind("parent_page", parentPage)
 *      .bind("reason", reason)
 *      .bind("discovered", timeStamp())
 *      .run();
 * @endcode
 *
 * */
namespace Database {

class Connection;
class Statement;
class Row;
class Iterator;

namespace Detail {
    class ConnectionBase;
    class StatementBase;
}

class Exception: public std::runtime_error {
public:
    Exception(const std::string &what)
        : std::runtime_error(what) {}

    ~Exception() noexcept {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** %Connection to an underlying database.
 *
 *  A @ref Connection is a lightweight object that can be copied. All copies initially point to the same underlying RDBMS
 *  connection, but individuals can have their connection changed with member functions such as @ref open and @ref close. The
 *  connections are destroyed automatically only after no databases or executing statements reference them. */
class Connection {
    friend class ::Sawyer::Database::Statement;
    friend class ::Sawyer::Database::Detail::ConnectionBase;

    std::shared_ptr<Detail::ConnectionBase> pimpl_;

public:
    /** Construct a connection not attached to any driver. */
    Connection() {};

private:
    explicit Connection(const std::shared_ptr<Detail::ConnectionBase> &pimpl);

public:
    /** Destructor.
     *
     *  The destructor calls @ref close before destroying this object. */
    ~Connection() = default;

    /** Tests whether an underlying RDBMS connection is established. */
    bool isOpen() const;

    /** Close a database if open.
     *
     *  Any established connection to a RDBMS is removed. If this was the last active reference to the connection then the
     *  connection is closed.  Statements that are in an "executing" state count as references to the connection, but other
     *  statement states are not counted, and they will become invalid (operating on them will throw an exception). */
    Connection& close();

    /** Creates a statement to be executed.
     *
     *  Takes a string containing a single SQL statement, parses and compiles it, and prepares it to be executed. Any parsing or
     *  compiling errors will throw an exception. The returned statement will be in either an "unbound" or "ready" state
     *  depending on whether it has parameters or not, respectively.  A statement in these states does not count as a reference
     *  to the underlying RDBMS connection and closing the connection will invalidate the statement (it will throw an exception
     *  if used).  This allows prepared statements to be constructed and yet still be possible to close the connection and
     *  release any resources it's using.
     *
     *  Statements may have parameters that need to be bound to actual values before the statement can start
     *  executing. Parameters are represented by "?name" occurring outside any string literals, where "name" is a sequence of
     *  one or more alpha-numeric characters. The same name may appear more than once within a statement, in which case binding
     *  an actual value to the parameter will cause all occurrances of that parameter to bound to the specified value.
     *  Parameters can only appear at certain positions within the SQL where a value rather than a key word is expected. */
    Statement stmt(const std::string &sql);

    /** Shortcut to execute a statement with no result. */
    Connection& run(const std::string &sql);

    /** Shortcut to execute a statement returning a single result.
     *
     *  Runs the query, which must not have any parameters, and returns the first column of the first row. If the query does not
     *  result in any rows or the first column of the first row is null, then nothing is returned. The query is canceled after
     *  returning the first row. */
    template<typename T>
    Optional<T> get(const std::string &sql);

    /** Name of the underlying driver.
     *
     *  Returns "sqlite", "postgresql" or an empty string if not connected. */
    std::string driverName() const;

    // Undocumented: Row number for the last SQL "insert" (do not use).
    //
    // This method is available only if the underlying database driver supports it and it has lots of caveats. In other words,
    // don't use this method. The most portable way to identify the rows that were just inserted is to insert a UUID as part of
    // the data.
    size_t lastInsert() const;

    // Set the pointer to implementation
    void pimpl(const std::shared_ptr<Detail::ConnectionBase> &p) {
        pimpl_ = p;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Statement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** SQL statement.
 *
 *  Statements are lightweight objects that can be copied and assigned. Statements in the EXECUTING state hold a reference
 *  the the underlying RDBMS connection. */
class Statement {
    friend class ::Sawyer::Database::Detail::ConnectionBase;

    std::shared_ptr<Detail::StatementBase> pimpl_;

public:
    /** A statement can be in one of many states. */
    enum State {
        UNBOUND,                                        /**< Statement contains parameters not bound to actual values. */
        READY,                                          /**< All parameters are bound and statement is ready to execute. */
        EXECUTING,                                      /**< Statement has started executing and is pointing to a result row. */
        FINISHED,                                       /**< Statement has finished executing. Parameters remain bound. */
        DEAD                                            /**< Synchronization with lower layer has been lost. */
    };

public:
    /** Construct a statement object not bound to any connection. */
    Statement() {}

private:
    explicit Statement(const std::shared_ptr<Detail::StatementBase> &stmt)
        : pimpl_(stmt) {}

public:
    /** Connection associated with this statement. */
    Connection connection() const;

    /** Bind an actual value to a statement parameter.
     *
     *  The parameter with the specified @p name is bound (or rebound) to the specified actual @p value. An exception is thrown
     *  if the statement doesn't have a parameter with the given name.
     *
     *  If the statement was in the finished state, then binding a parameter causes all other parameters to become unbound. This
     *  helps ensure that all parameters are bound to the correct values each time a prepared statement is executed. If you don't
     *  want this behavior, use @ref rebind instead.  Regardless, the statement is transitioned to either the @ref UNBOUND or
     *  @ref READY state. */
    template<typename T>
    Statement& bind(const std::string &name, const T &value);

    /** Re-bind an actual value to a statement parameter.
     *
     *  This function's behavior is identical to @ref bind except one minor detail: no other parameters are unbound if the
     *  statement was in the finished state. */
    template<typename T>
    Statement& rebind(const std::string &name, const T &value);

    /** Begin execution of a statement.
     *
     *  Begins execution of a statement and returns an iterator to the result.  If the statement returns at least one row
     *  of a result then the returned iterator points to this result and the statement will be in the @ref EXECUTING state
     *  until the iterator is destroyed.  If the statement doesn't produce at least one row then an end iterator is returned
     *  and the statement is in the @ref FINISHED state. Errors are indicated by throwing an exception. */
    Iterator begin();

    /** The end iterator. */
    Iterator end();

    /** Shortcut to run a statement and ignore results.
     *
     *  This is here mainly for constency with @ref Connection::run. It's identical to @ref begin except we ignore the returned
     *  iterator, which causes the statement to be canceled after its first row of output. */
    Statement& run();
    
    /** Shortcut to run a statement returning a single result.
     *
     *  The result that's returned is the first column of the first row. The statement must produce at least one row of result,
     *  although the first column of that row can be null. */
    template<typename T>
    Optional<T> get();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Row
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Refers to a row of the result of an executed statement.
 *
 *  This is the type obtained by dereferencing an iterator. Rows are lightweight objects that can be copied. A row is
 *  automatically invalidated when the iterator is incremented. */
class Row {
    friend class ::Sawyer::Database::Iterator;

    std::shared_ptr<Detail::StatementBase> stmt_;
    size_t sequence_;                                   // for checking validity

private:
    Row()
        : sequence_(0) {}

    explicit Row(const std::shared_ptr<Detail::StatementBase> &stmt);

public:
    /** Get a particular column. */
    template<typename T>
    Optional<T> get(size_t columnIdx) const;

    /** Get the row number.
     *
     *  Rows are numbered starting at zero. */
    size_t rowNumber() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Iterator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Single-pass iterator over the result rows of an executed statement.
 *
 *  An iterator is either the special "end" iterator, or points to a row of a result from a statement currently in the
 *  @ref Statement::EXECUTING "EXECUTING" state. Incrementing the iterator advances to either the next row of
 *  the statement or the end iterator. When the end iterator is reached the associated statement is is transitioned to
 *  the @ref Statement::FINISHED "FINISHED" state. */
class Iterator: public boost::iterator_facade<Iterator, const Row, boost::forward_traversal_tag> {
    friend class ::Sawyer::Database::Detail::StatementBase;

    Row row_;

public:
    /** An end iterator. */
    Iterator() {}

private:
    explicit Iterator(const std::shared_ptr<Detail::StatementBase> &stmt);

public:
    /** Test whether this is an end iterator. */
    bool isEnd() const {
        return !row_.stmt_;
    }

    /** Test whether this is not an end iterator. */
    explicit operator bool() const {
        return !isEnd();
    }
    
private:
    friend class boost::iterator_core_access;
    const Row& dereference() const;
    bool equal(const Iterator&) const;
    void increment();
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                  Only implementation details beyond this point.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace Detail {

// Base class for connection details. The individual drivers (SQLite3, PostgreSQL) will be derived from this class.
//
// Connection detail objects are reference counted. References come from only two places:
//   1. Each top-level Connection object that's in a connected state has a reference to this connection.
//   2. Each low-level Statement object that's in an "executing" state has a reference to this connection.
// Additionally, all low-level statement objects have a weak reference to a connection.
//
class ConnectionBase: public std::enable_shared_from_this<ConnectionBase> {
    friend class ::Sawyer::Database::Connection;

protected:
    ConnectionBase() {}

public:
    virtual ~ConnectionBase() {}

protected:
    // Close any low-level connection.
    virtual void close() = 0;

    // Create a prepared statement from the specified high-level SQL. By "high-level" we mean the binding syntax used by this
    // API such as "?name" (whereas low-level means the syntax passed to the driver such as "?").
    virtual Statement prepareStatement(const std::string &sql) = 0;

    // Row number for the last inserted row if supported by this driver.  It's better to use a table column that holds a value
    // generated from a sequence.
    virtual size_t lastInsert() const = 0;

    Statement makeStatement(const std::shared_ptr<Detail::StatementBase> &detail);

    virtual std::string driverName() const = 0;
};

// Describes the location of "?name" parameters in high-level SQL by associating them with one or more "?" parameters in
// low-level SQL. WARNIN: the low-level parameters are numbered starting at one instead of zero, which is inconsistent with how
// the low-level APIs index other things like query result columns (not to mention being surprising for C and C++ developers).
class Parameter {
    friend class ::Sawyer::Database::Detail::StatementBase;

    std::vector<size_t> indexes;                        // "?" indexes
    bool isBound = true;

    void append(size_t idx) {
        indexes.push_back(idx);
    }
};

template<typename T>
class ColumnReader {
    friend class ::Sawyer::Database::Detail::StatementBase;
    Optional<T> operator()(StatementBase *stmt, size_t idx);
};

//template<>
//class ColumnReader<std::vector<uint8_t>> {
//    friend class ::Sawyer::Database::Detail::StatementBase;
//    Optional<std::vector<uint8_t>> operator()(StatementBase *stmt, size_t idx);
//};

// Reference counted prepared statement details. Objects of this class are referenced from the high-level Statement objects and
// the query iterator rows.  This class is the base class for driver-specific statements.
class StatementBase: public std::enable_shared_from_this<StatementBase> {
    friend class ::Sawyer::Database::Iterator;
    friend class ::Sawyer::Database::Row;
    friend class ::Sawyer::Database::Statement;
    template<class T> friend class ::Sawyer::Database::Detail::ColumnReader;

    using Parameters = Container::Map<std::string, Parameter>;
    
    std::shared_ptr<ConnectionBase> connection_;        // non-null while statement is executing
    std::weak_ptr<ConnectionBase> weakConnection_;      // refers to the originating connection
    Parameters params_;                                 // mapping from param names to question marks
    Statement::State state_ = Statement::DEAD;          // don't set directly; use "state" member function
    size_t sequence_ = 0;                               // sequence number for invalidating row iterators
    size_t rowNumber_ = 0;                              // result row number

public:
    virtual ~StatementBase() {}

protected:
    explicit StatementBase(const std::shared_ptr<ConnectionBase> &connection)
        : weakConnection_(connection) {                 // save only a weak pointer, no shared pointer
        ASSERT_not_null(connection);
    }

    // Parse the high-level SQL (with "?name" parameters) into low-level SQL (with "?" parameters). Returns the low-level SQL
    // and the number of low-level "?" parameters and has the following side effects:
    //   1. Re-initializes this object's parameter list
    //   2. Sets this object's state to READY, UNBOUND, or DEAD.
    std::pair<std::string, size_t> parseParameters(const std::string &highSql) {
        params_.clear();
        std::string lowSql;
        bool inString = false;
        size_t nLowParams = 0;
        state(Statement::READY);                        // possibly reset below
        for (size_t i = 0; i < highSql.size(); ++i) {
            if ('\'' == highSql[i]) {
                inString = !inString;                   // works for "''" escape too
                lowSql += highSql[i];
            } else if ('?' == highSql[i] && !inString) {
                lowSql += '?';
                std::string paramName;
                while (i+1 < highSql.size() && (::isalnum(highSql[i+1]) || '_' == highSql[i+1]))
                    paramName += highSql[++i];
                if (paramName.empty())
                    throw Exception("invalid parameter name at character position " + boost::lexical_cast<std::string>(i));
                Parameter &param = params_.insertMaybeDefault(paramName);
                param.append(nLowParams++);             // 0-origin low-level parameter numbers
                state(Statement::UNBOUND);
            } else {
                lowSql += highSql[i];
            }
        }
        if (inString) {
            state(Statement::DEAD);
            throw Exception("mismatched quotes in SQL statement");
        }
        return std::make_pair(lowSql, nLowParams);
    }

    // Invalidate all iterators and their rows by incrementing this statements sequence number.
    void invalidateIteratorsAndRows() {
        ++sequence_;
    }

    // Sequence number used for checking iterator validity.
    size_t sequence() const {
        return sequence_;
    }
    
    // Cause this statement to lock the database connection by maintaining a shared pointer to the low-level
    // connection. Returns true if the connection could be locked, or false if unable.
    bool lockConnection() {
        return (connection_ = weakConnection_.lock()) != nullptr;
    }

    // Release the connection lock by throwing away the shared pointer to the connection. This statement will still maintain
    // a weak reference to the connection.
    void unlockConnection() {
        connection_.reset();
    }

    // Returns an indication of whether this statement holds a lock on the low-level connection, preventing the connection from
    // being destroyed.
    bool isConnectionLocked() const {
        return connection_ != nullptr;
    }

    // Returns the connection details associated with this statement.  The connection is not locked by querying this property.
    std::shared_ptr<ConnectionBase> connection() const {
        return weakConnection_.lock();
    }

    // Return the current statement state.
    Statement::State state() const {
        return state_;
    }

    // Change the statement state.  A statement in the EXECUTING state will lock the connection to prevent it from being
    // destroyed, but a statement in any other state will unlock the connection causing the last reference to destroy the
    // connection and will invalidate all iterators and rows.
    void state(Statement::State newState) {
        switch (newState) {
            case Statement::DEAD:
            case Statement::FINISHED:
            case Statement::UNBOUND:
            case Statement::READY:
                invalidateIteratorsAndRows();
                unlockConnection();
                break;
            case Statement::EXECUTING:
                ASSERT_require(isConnectionLocked());
                break;
        }
        state_ = newState;
    }

    // Returns true if this statement has parameters that have not been bound to a value.
    bool hasUnboundParameters() const {
        ASSERT_forbid(state() == Statement::DEAD);
        for (const Parameter &param: params_.values()) {
            if (!param.isBound)
                return true;
        }
        return false;
    }

    // Causes all parameters to become unbound and changes the state to either UNBOUND or READY (depending on whether there are
    // any parameters or not, respectively).
    virtual void unbindAllParams() {
        ASSERT_forbid(state() == Statement::DEAD);
        for (Parameter &param: params_.values())
            param.isBound = false;
        state(params_.isEmpty() ? Statement::READY : Statement::UNBOUND);
    }

    // Reset the statement by invalidating all iterators, unbinding all parameters, and changing the state to either UNBOUND or
    // READY depending on whether or not it has any parameters.
    virtual void reset(bool doUnbind) {
        ASSERT_forbid(state() == Statement::DEAD);
        invalidateIteratorsAndRows();
        if (doUnbind) {
            unbindAllParams();
        } else {
            state(hasUnboundParameters() ? Statement::UNBOUND : Statement::READY);
        }
    }

    // Bind a value to a parameter. If isRebind is set and the statement is in the EXECUTING state, then rewind back to the
    // READY state, preserve all previous bindings, and adjust only the specified binding.
    template<typename T>
    void bind(const std::string &name, const T &value, bool isRebind) {
        if (!connection())
            throw Exception("connection is closed");
        switch (state()) {
            case Statement::DEAD:
                throw Exception("statement is dead");
            case Statement::FINISHED:
            case Statement::EXECUTING:
                reset(!isRebind);
                // fall through
            case Statement::READY:
            case Statement::UNBOUND: {
                if (!params_.exists(name))
                    throw Exception("no such parameter \"" + name + "\" in statement");
                Parameter &param = params_[name];
                bool wasUnbound = param.isBound;
                for (size_t idx: param.indexes) {
                    try {
                        bindLow(idx, value);
                    } catch (const Exception &e) {
                        if (param.indexes.size() > 1)
                            state(Statement::DEAD); // might be only partly bound now
                        throw e;
                    }
                }
                param.isBound = true;

                if (wasUnbound && !hasUnboundParameters())
                    state(Statement::READY); 
                break;
            }
        }
    }

    // Bind a value to an optional parameter.
    template<typename T>
    void bind(const std::string &name, const Sawyer::Optional<T> &value, bool isRebind) {
        if (value) {
            bind(name, *value, isRebind);
        } else {
            bind(name, Nothing(), isRebind);
        }
    }

    // Driver-specific part of binding by specifying the 0-origin low-level "?" number and the value.
    virtual void bindLow(size_t idx, int value) = 0;
    virtual void bindLow(size_t idx, int64_t value) = 0;
    virtual void bindLow(size_t idx, size_t value) = 0;
    virtual void bindLow(size_t idx, double value) = 0;
    virtual void bindLow(size_t idx, const std::string &value) = 0;
    virtual void bindLow(size_t idx, const char *cstring) = 0;
    virtual void bindLow(size_t idx, Nothing) = 0;
    virtual void bindLow(size_t idx, const std::vector<uint8_t> &data) = 0;

    Iterator makeIterator() {
        return Iterator(shared_from_this());
    }
    
    // Begin execution of a statement in the READY state. If the statement is in the FINISHED or EXECUTING state it will be
    // restarted.
    Iterator begin() {
        if (!connection())
            throw Exception("connection is closed");
        switch (state()) {
            case Statement::DEAD:
                throw Exception("statement is dead");
            case Statement::UNBOUND: {
                std::string s;
                for (Parameters::Node &param: params_.nodes()) {
                    if (!param.value().isBound)
                        s += (s.empty() ? "" : ", ") + param.key();
                }
                ASSERT_forbid(s.empty());
                throw Exception("unbound parameters: " + s);
            }
            case Statement::FINISHED:
            case Statement::EXECUTING:
                reset(false);
                // fall through
            case Statement::READY: {
                if (!lockConnection())
                    throw Exception("connection has been closed");
                state(Statement::EXECUTING);
                rowNumber_ = 0;
                Iterator iter = beginLow();
                rowNumber_ = 0;                         // in case beginLow changed it
                return iter;
            }
        }
        ASSERT_not_reachable("invalid state");
    }

    // The driver-specific component of "begin". The statement is guaranteed to be in the EXECUTING state when called,
    // but could be in some other state after returning.
    virtual Iterator beginLow() = 0;

    // Advance an executing statement to the next row
    Iterator next() {
        if (!connection())
            throw Exception("connection is closed");
        ASSERT_require(state() == Statement::EXECUTING); // no other way to get here
        invalidateIteratorsAndRows();
        ++rowNumber_;
        return nextLow();
    }

    // Current row number
    size_t rowNumber() const {
        return rowNumber_;
    }
    
    // The driver-specific component of "next". The statement is guaranteed to be in the EXECUTING state when called, but
    // could be in some other state after returning.
    virtual Iterator nextLow() = 0;

    // Get a column value from the current row of result
    template<typename T>
    Optional<T> get(size_t columnIdx) {
        if (!connection())
            throw Exception("connection is closed");
        ASSERT_require(state() == Statement::EXECUTING); // no other way to get here
        if (columnIdx >= nColumns())
            throw Exception("column index " + boost::lexical_cast<std::string>(columnIdx) + " is out of range");
        return ColumnReader<T>()(this, columnIdx);
    }

    // Number of columns returned by a query.
    virtual size_t nColumns() const = 0;

    // Get the value of a particular column of the current row.
    virtual Optional<std::string> getString(size_t idx) = 0;
    virtual Optional<std::vector<std::uint8_t>> getBlob(size_t idx) = 0;
};

template<typename T>
inline Optional<T>
ColumnReader<T>::operator()(StatementBase *stmt, size_t idx) {
    std::string str;
    if (!stmt->getString(idx).assignTo(str))
        return Nothing();
    return boost::lexical_cast<T>(str);
}

template<>
inline Optional<std::vector<uint8_t>>
ColumnReader<std::vector<uint8_t>>::operator()(StatementBase *stmt, size_t idx) {
    return stmt->getBlob(idx);
}

inline Statement
ConnectionBase::makeStatement(const std::shared_ptr<Detail::StatementBase> &detail) {
    return Statement(detail);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations Connection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline Connection::Connection(const std::shared_ptr<Detail::ConnectionBase> &pimpl)
    : pimpl_(pimpl) {}

inline bool
Connection::isOpen() const {
    return pimpl_ != nullptr;
}

inline Connection&
Connection::close() {
    pimpl_ = nullptr;
    return *this;
}

inline std::string
Connection::driverName() const {
    if (pimpl_) {
        return pimpl_->driverName();
    } else {
        return "";
    }
}

inline Statement
Connection::stmt(const std::string &sql) {
    if (pimpl_) {
        return pimpl_->prepareStatement(sql);
    } else {
        throw Exception("no active database connection");
    }
}

inline Connection&
Connection::run(const std::string &sql) {
    stmt(sql).begin();
    return *this;
}

template<typename T>
inline Optional<T>
Connection::get(const std::string &sql) {
    for (auto row: stmt(sql))
        return row.get<T>(0);
    return Nothing();
}

inline size_t
Connection::lastInsert() const {
    if (pimpl_) {
        return pimpl_->lastInsert();
    } else {
        throw Exception("no active database connection");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations for Statement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Connection
Statement::connection() const {
    if (pimpl_) {
        return Connection(pimpl_->connection());
    } else {
        return Connection();
    }
}

template<typename T>
inline Statement&
Statement::bind(const std::string &name, const T &value) {
    if (pimpl_) {
        pimpl_->bind(name, value, false);
    } else {
        throw Exception("no active database connection");
    }
    return *this;
}

template<typename T>
inline Statement&
Statement::rebind(const std::string &name, const T &value) {
    if (pimpl_) {
        pimpl_->bind(name, value, true);
    } else {
        throw Exception("no active database connection");
    }
    return *this;
}

inline Iterator
Statement::begin() {
    if (pimpl_) {
        return pimpl_->begin();
    } else {
        throw Exception("no active database connection");
    }
}

inline Iterator
Statement::end() {
    return Iterator();
}

inline Statement&
Statement::run() {
    begin();
    return *this;
}

template<typename T>
inline Optional<T>
Statement::get() {
    Iterator row = begin();
    if (row.isEnd())
        throw Exception("query did not return a row");
    return row->get<T>(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations for Iterator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline
Iterator::Iterator(const std::shared_ptr<Detail::StatementBase> &stmt)
    : row_(stmt) {}

inline const Row&
Iterator::dereference() const {
    if (isEnd())
        throw Exception("dereferencing the end iterator");
    if (row_.sequence_ != row_.stmt_->sequence())
        throw Exception("iterator has been invalidated");
    return row_;
}

inline bool
Iterator::equal(const Iterator &other) const {
    return row_.stmt_ == other.row_.stmt_ && row_.sequence_ == other.row_.sequence_;
}

inline void
Iterator::increment() {
    if (isEnd())
        throw Exception("incrementing the end iterator");
    *this = row_.stmt_->next();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementations for Row
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline
Row::Row(const std::shared_ptr<Detail::StatementBase> &stmt)
    : stmt_(stmt), sequence_(stmt ? stmt->sequence() : 0) {}

template<typename T>
inline Optional<T>
Row::get(size_t columnIdx) const {
    ASSERT_not_null(stmt_);
    if (sequence_ != stmt_->sequence())
        throw Exception("row has been invalidated");
    return stmt_->get<T>(columnIdx);
}

inline size_t
Row::rowNumber() const {
    ASSERT_not_null(stmt_);
    if (sequence_ != stmt_->sequence())
        throw Exception("row has been invalidated");
    return stmt_->rowNumber();
}

} // namespace
} // namespace

#endif
#endif
