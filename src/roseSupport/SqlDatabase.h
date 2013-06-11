#ifndef ROSE_SqlDatabase_H
#define ROSE_SqlDatabase_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <cassert>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

/** Support for a variety of relational database drivers.
 *
 *  ROSE originally supported only sqlite3x, a C++ API for SQLite3 databases.  While SQLite3 is easy to configure (a database
 *  is just a local file), it is not the best relational database system to use for large databases with complex queries and
 *  parallel access. The sqlite and the sqlite3x interface also have some peculiarities that make it difficult to use. For
 *  instance,
 *
 *  <ul>
 *    <li>An SQLite3 database can be accessed in parallel only if one first implements and registers a function to handle
 *        lock contention.</li>
 *    <li>All parallel writes to an SQLite3 database must use "immediate" transaction locks if any one of the writers uses
 *        such a lock.  Since we don't always know what kinds of locks might be used, we must always uses immediate locking.</li>
 *    <li>The indices for statement binding are one-origin, while the indices for cursor reading are zero origin.  This is
 *        especially confusing when a single statement has both binding and reading.</li>
 *    <li>sqlite3x_reader (an iterator over results) has an odd interface.  After the iterator is created, it must be advanced
 *        one row before reading.</li>
 *    <li>The SQL variant supported by SQLite3 is somewhat limited.  For instance, it has no full outer join or column
 *        renaming.  This means that many complex queries need to be implemented in C++ rather than SQL.</li>
 *    <li>The sqlite3x API is minimally documented; the only documentation is that which the ROSE team has added.</li>
 *  </ul>
 *
 *  ROSE's SqlDatabase is an abstraction layer around sqlite3x and other relational database systems.  The goal is to be able
 *  to write C++ that manipulates a database and not care too much about what driver provides that database. There are a couple
 *  of existing APIs that provide this abstraction, but they're not C++ oriented.
 *
 *  The SqlDatabase API is used as follows:
 *
 *  First, a connection is established to the database via a SqlDatabase::Connection object.  This causes the database to be
 *  opened. SQLite3 will create the database if necesssary; PostgreSQL will access an existing database.  The connection
 *  specification varies depending on the underlying driver (a filename for SQLite3; a URL or connection parameters for
 *  PostgreSQL).
 *
 *  The connection is used to create one or more SQL statements.  A statement may have value placeholders indicated with "?",
 *  which are numbered starting with zero according to their relative positions in the SQL string.  The placeholders are bound
 *  to actual values with the Statement::bind() function.  It is permissible to rebind new values to an existing statement in
 *  order to execute a statement repeatedly with different values.
 *
 *  Once all placeholders of a statement are bound to values, the statement can be executed.  A statement is executed by
 *  calling its begin() method, which returns an iterator.  The iterator points to consective rows of the result.  Various
 *  convenience methods are available for query statements that return a single row with a single value, or statements that do
 *  not return any rows.  In general, a new statement should not be executed until all rows of the previous statement are
 *  consumed (this is a limitation of the underlying drivers).
 *
 *  For instance, here's how one would construct an std::map of all the file IDs and file names for files containing more
 *  than N lines, where N is supplied as an argument to the function:
 * 
 * @code
 *    std::map<int, std::string>
 *    get_file_names(SqlDatabase::Connection &db, int nlines) {
 *      std::map<int, std::string> result;
 *      SqlDatabase::Statement stmt(db, "select id, name from files where nlines > ?");
 *      stmt.bind(0, nlines);
 *      for (SqlDatabase::Statement::iterator row=stmt.begin(); row!=stmt.end(); ++row)
 *        result[row.get<int>(0)] = row.get<std::string>(1);
 *      return result;
 *    }
 * @endcode
 */
namespace SqlDatabase {

/** Low-level driver to use for the database.  Multiple database drivers are supported depending on how ROSE was configured. */
enum Driver {
    NO_DRIVER,                  /**< Used only by the default Connection constructor. */
    SQLITE3,                    /**< SQLite3 using the C++ sqlite3x API. */
    POSTGRESQL                  /**< PostgreSQL. */
};


/*******************************************************************************************************************************
 *                                      Forwards and types
 *******************************************************************************************************************************/

class Connection;
class ConnectionImpl;
class Transaction;
class TransactionImpl;
class Statement;
class StatementImpl;

/** Smart pointer to a database connection.  Database connections are always referenced through their smart pointers and are
 *  automatically deleted when all references disappear. See Connection::create(). */
typedef boost::shared_ptr<Connection> ConnectionPtr;

/** Smart pointer to a transaction.  Transactions are always referenced through their smart pointers and are
 *  automatically deleted when all references disappear. See Connection::transaction(). */
typedef boost::shared_ptr<Transaction> TransactionPtr;

/** Smart pointer to a statement.  Statements are always referenced through their smart pointers and are
 *  automatically deleted when all references disappear. See Transaction::statement(). */
typedef boost::shared_ptr<Statement> StatementPtr;


/*******************************************************************************************************************************
 *                                      Exceptions
 *******************************************************************************************************************************/

/** Exceptions thrown by database operations. */
class Exception: public std::runtime_error {
public:
    explicit Exception(const char *mesg): std::runtime_error(mesg) {}
    explicit Exception(const std::string &mesg): std::runtime_error(mesg) {}
    explicit Exception(const std::runtime_error &e, const ConnectionPtr &conn, const TransactionPtr &tx,
                       const StatementPtr &stmt)
        : std::runtime_error(e), connection(conn), transaction(tx), statement(stmt) {}
    explicit Exception(const std::string &mesg, const ConnectionPtr &conn, const TransactionPtr &tx,
                       const StatementPtr &stmt)
        : std::runtime_error(mesg), connection(conn), transaction(tx), statement(stmt) {}

    virtual ~Exception() throw() {}
    virtual const char *what() const throw() /*override*/;
    void print(std::ostream&) const;
    ConnectionPtr connection;
    TransactionPtr transaction;
    StatementPtr statement;
private:
    mutable std::string what_str;       // backing store for what()
};


/*******************************************************************************************************************************
 *                                      Connections
 *******************************************************************************************************************************/

/** Connection to a database.  Multiple connections may exist to a single database, and a connection may have multiple
 *  transactions in which statements are executed.  The connection is automatically closed once all references to it have
 *  disappeared (both user references and references from transactions).
 *
 *  Connections should be closed around fork().  Most low-level drivers don't gracefully handle this situation. (FIXME: we
 *  might be able to work around this to some extent in the SqlDatabase implementation. [Robb P. Matzke 2013-05-31] */
class Connection: public boost::enable_shared_from_this<Connection> {
    friend class TransactionImpl;
    friend class Transaction;
    friend class StatementImpl;
public:
    /** Create a new database connection.  All connection objects are bound to a database throughout their lifetime, although
     * depending on the driver, the actual low-level connection may open and close. The @p open_spec string describes how to
     * connect to the database and its format varies depending on the underlying database driver.  If no @p driver is specified
     * then a driver is chosen (if possible) based on the @p open_spec. The underlying low-level connections are automatically
     * closed when all transactions terminate. */
    static ConnectionPtr create(const std::string &open_spec, Driver driver=NO_DRIVER) {
        if (NO_DRIVER==driver)
            driver = guess_driver(open_spec);
        return ConnectionPtr(new Connection(open_spec, driver));
    }

    /** Begins a new transaction for this connection.  A connection may have multiple transactions at one time; if the
     *  underlying database driver doesn't support multiple transactions, then multiple low-level connections are instantiated.
     *  All statements are executed within a transaction. */
    TransactionPtr transaction();

    /** Guess a driver based on a connection specification. This will typically work only for drivers that use URLs that
     *  contain the driver name.  Returns NO_DRIVER if a driver cannot be guessed. */
    static Driver guess_driver(const std::string &open_spec);

    /** Return the low-level driver being used. */
    Driver driver() const;

    /** Debugging property.  If non-null, then debugging information is sent to that file. Transactions inherit the debugging
     * property of their connection, and statements inherit the debugging property of their transaction.
     * @{ */
    void set_debug(FILE *f);
    FILE *get_debug() const;
    /** @} */

    /** Print some basic info about this connection. */
    void print(std::ostream&) const;

    // Only called by boost::shared_ptr
    ~Connection() { finish(); }

protected:
    // Protected because you should be using create() to get a smart pointer.  Database driver-level connections are typically
    // not copyable object anyway.
    Connection(const std::string &open_spec, Driver driver): impl(NULL) { init(open_spec, driver); }

private:
    void init(const std::string &open_spec, Driver driver);
    void finish();

private:
    ConnectionImpl *impl;
};

/*******************************************************************************************************************************
 *                                      Transactions
 *******************************************************************************************************************************/

/** Unit of work within a database.  A transaction comprises a unit of work performed against a database and treated in a
 *  coherent and reliable way independent of other transactions.  A database connection may have multiple transactions and all
 *  statements are executed within a transaction. A transaction is obtained (and started) via Connection::transaction(). A
 *  transaction can be canceled with its rollback() method or by deletion; it can be completed by calling its commit() method.
 *  A transaction that has been canceled or completed can no longer be used. */
class Transaction: public boost::enable_shared_from_this<Transaction> {
    friend class Statement;
    friend class StatementImpl;
public:
    /** Create a new transaction.  Transactions can be created either by this class method or by calling
     *  Connection::transaction().  The transaction will exist until there are no references (user or statements).
     *  Transactions must be explicitly completed via commit(), or its statements will have no effect on the database. */
    static TransactionPtr create(const ConnectionPtr &conn);

    /** Cancel a transaction.  The statements that executed within this transaction are rolled back so that they do not have
     *  any effect on the database.  Once this transaction is rolled back it enters a terminated state and cannot be used; any
     *  method call (other than is_terminated()) will result in an exception. */
    void rollback();

    /** Complete a transaction.  The statements that executed within this transaction are completed so that their effects are
     *  visible in the database.  Once this transaction is committed it enters a terminated state and cannot be used; any
     *  method call (other than is_terminated()) will result in an exception. */
    void commit();

    /** Returns termination status.  A transaction is terminated if it has been canceled or completed via rollback() or
     *  commit(), respectively. */
    bool is_terminated() const;

    /** Create a new statement. */
    StatementPtr statement(const std::string &sql);

    /** Execute one or more statements. The provided SQL source code is parsed into individual statements and executed one
     *  statement at a time until all are processed or statement fails causing an exception to be thrown.  All results are
     *  discarded. The transaction is not automatically committed. */
    void execute(const std::string &sql);

    /** Returns the low-level driver name for this transaction. */
    Driver driver() const;

    /** Debugging property.  If non-null, then debugging information is sent to that file. Transactions inherit the debugging
     * property of their connection (which is overridden by this method), and statements inherit the debugging property of
     * their transaction.
     *  @{ */
    void set_debug(FILE *f);
    FILE *get_debug() const;
    /** @} */

    /** Print some basic info about this transaction. */
    void print(std::ostream&) const;

    // Only called by boost::shared_ptr
    ~Transaction() { finish(); }

protected:
    explicit Transaction(const ConnectionPtr &conn, size_t drv_conn): impl(NULL) { init(conn, drv_conn); }

private:
    void init(const ConnectionPtr &conn, size_t drv_conn);
    void finish();

private:
    TransactionImpl *impl;
};


/*******************************************************************************************************************************
 *                                      Statements
 *******************************************************************************************************************************/

/** A database operation.  A Statement represents a single SQL statement to be executed within a transaction.  Statements may
 *  contain value placeholders indicated by '?' characters which are numbered consecutively starting at zero.  Actual values
 *  must be bound to all the placeholders before the statement can be executed.  A single statement may be executed multiple
 *  times and may have different actual values bound to its place holders each time.
 *
 *  When a statement is executed within a transaction, it may return zero or more result rows each containing one or more
 *  columns of values.  The results are returned via STL-like iterators.  Re-executing a statement will cause its previous
 *  iterators to become invalid and throw exceptions if they are used. */
class Statement: public boost::enable_shared_from_this<Statement> {
public:

    /** Create a new statement.  Statements can be created either by this class method or by calling
     *  Transaction::statement(). */
    static StatementPtr create(const TransactionPtr &tranx, const std::string &sql) {
        return StatementPtr(new Statement(tranx, sql));
    }

    /** Iterates over the result rows. Statement iterators are very restricted compared to normal C++ STL iterators because
     *  statement iterators access result rows that are not actually stored locally and the underlying drivers often don't
     *  provide an easy way to save an entire row. */
    class iterator {
    public:
        iterator(): execution_seq(0), row_num(0) { init(); } // an end iterator
        iterator(const StatementPtr &stmt, size_t execution_seq)
            : stmt(stmt), execution_seq(execution_seq), row_num(0) { init(); }
        template<typename T> T get(size_t idx);
        int32_t get_i32(size_t idx);
        int64_t get_i64(size_t idx);
        uint32_t get_u32(size_t idx);
        uint64_t get_u64(size_t idx);
        double get_dbl(size_t idx);
        std::string get_str(size_t idx);
        iterator& operator++();
        bool at_eof() const;
        bool operator==(const iterator &other) const;
        bool operator!=(const iterator &other) const { return !(*this==other); }
    private:
        void init();
        void check() const;                     // check that this iterator is not stale
    private:
        StatementPtr stmt;                      // statement whose results we are iterating over
        size_t execution_seq;                   // statement execution counter
        size_t row_num;                         // row number
    };
    
    /** Bind value to a '?' placeholder in the SQL statement.  Placeholders are counted from zero based on their position
     *  in the text of the SQL command.  Only after all placeholders are bound to values can this statement be executed.
     *  The bind() method returns the statement pointer so that bind calls can be chained together.
     * @{ */
    StatementPtr bind(size_t idx, int32_t val);
    StatementPtr bind(size_t idx, int64_t val);
    StatementPtr bind(size_t idx, uint32_t val);
    StatementPtr bind(size_t idx, uint64_t val);
    StatementPtr bind(size_t idx, double val);
    StatementPtr bind(size_t idx, const std::string &val);
    /** @} */

    /** Execute this statement.  Returns the iterator pointing to the first row of the result. */
    iterator begin();

    /** Returns an end iterator.  End iterators are just default-constructed iterators--an iterator from any statement can be
     *  compared to an end iterator from any statement. */
    iterator end() { return iterator(); }

    /** Execute a statement and ignore its result rows if it has any. */
    void execute();

    /** Execute a statement that returns a single integer. */
    int execute_int();

    /** Returns the low-level driver name for this statement. */
    Driver driver() const;

    /** Debugging property.  If non-null, then debugging information is sent to that file. Transactions inherit the debugging
     * property of their connection, and statements inherit the debugging property of their transaction (which is overridden by
     * this method).
     *  @{ */
    void set_debug(FILE *f);
    FILE *get_debug() const;
    /** @} */

    /** Print some basic info about this statement. */
    void print(std::ostream&) const;

public:
    // Called only by boost::shared_ptr
    ~Statement() { finish(); }

protected:
    Statement(const TransactionPtr &tranx, const std::string &sql): impl(NULL) { init(tranx, sql); }

private:
    void init(const TransactionPtr &tranx, const std::string &sql);
    void finish();

private:
    StatementImpl *impl;
};

template<> int32_t Statement::iterator::get<int32_t>(size_t idx);
template<> int64_t Statement::iterator::get<int64_t>(size_t idx);
template<> uint32_t Statement::iterator::get<uint32_t>(size_t idx);
template<> uint64_t Statement::iterator::get<uint64_t>(size_t idx);
template<> float Statement::iterator::get<float>(size_t idx);
template<> double Statement::iterator::get<double>(size_t idx);
template<> std::string Statement::iterator::get<std::string>(size_t idx);
    
/*******************************************************************************************************************************
 *                                      Miscellaneous functions
 *******************************************************************************************************************************/

/** Split SQL source code into individual statements.  This is not a full parser--it only looks for top-level semicolons. */
std::vector<std::string> split_sql(const std::string &sql);

std::ostream& operator<<(std::ostream&, const Exception&);
std::ostream& operator<<(std::ostream&, const Connection&);
std::ostream& operator<<(std::ostream&, const Transaction&);
std::ostream& operator<<(std::ostream&, const Statement&);

} // namespace
#endif
