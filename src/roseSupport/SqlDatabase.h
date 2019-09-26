#ifndef ROSE_SqlDatabase_H
#define ROSE_SqlDatabase_H

#include "FormatRestorer.h"
#include "RoseException.h"
#include "StringUtility.h"
#include "rose_override.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <cassert>
#include <iomanip>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <sstream>
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
 *  For instance, here's how one would run a query that prints file IDs, inode number, file name, and owner name for files
 *  containing N lines, where N is supplied as an argument to the function:
 *
 * @code
 *    void show_files(SqlDatabase::TransactionPtr &tx, int nlines, std::ostream &out) {
 *        FormatRestorer fr(out); // so we don't need to restore the original stream flags
 *        SqlDatabase::StatementPtr stmt = tx->statement("select id, inode, name, owner from files where nlines = ?");
 *        stmt.bind(0, nlines);
 *        int nfiles = 0;
 *        out <<      std::setw(7)  <<std::right <<"ID"
 *            <<" " <<std::setw(10) <<std::right <<"Inode"
 *            <<" " <<std::setw(64) <<std::left  <<"File Name"
 *            <<" " <<std::setw(1)  <<std::left  <<"Owner Name" <<"\n"
 *        for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row)
 *            out <<      std::setw(7)  <<std::right <<row.get<int>(0)                 // the ID number
 *                <<" " <<std::setw(10) <<std::right <<row.get<ino_t>(1)               // the inode number
 *                <<" " <<std::setw(64) <<std::left  <<row.get<std::string>(2)         // file name
 *                <<" " <<std::setw(1)  <<std::left  <<row.get<std::string>(3) <<"\n"; // owner name
 *            ++nfiles;
 *        }
 *        out <<"number of matching files: " <<nfiles <<"\n";
 *    }
 * @endcode
 *
 * There are a number of problems with that code:
 *  <ul>
 *    <li>The output operators are so verbose that it's hard to see what's actually being printed.</li>
 *    <li>We kludged the column widths without really knowing how wide the data is.  For instance, the file name
 *        column is likely to be much to wide if most names don't include path components, and much to narrow if
 *        they do.  The too-narrow case causes the owner name column to not line up properly.  Fixing this would
 *        require a previous traversal of an identical query in order to count the width, but even that has the
 *        potential for being wrong when other processes are modifying the "files" table.</li>
 *    <li>We didn't include a separator between the column headings and the table data.  Adding one would make the
 *        code even more verbose and unreadable.  There's also no separator at the end of the table, although that's
 *        not nearly as critical.</li>
 *    <li>If the table is exceptionally long it might be nice to re-print the headers every so often.</li>
 *    <li>In order to count the number of matching files we need to increment a counter in the body of the loop.</li>
 *    <li>Since we're monkeying with stream formatters, we need to restore their original values so as not to leave
 *        surprises for the caller, including when something we call might throw an exception.  Fortunately ROSE's
 *        FormatRestorer class makes this easy to do.</li>
 *  </ul>
 *
 *  We can solve all these problems and improve readability by using the SqlDatabase::Table template class, like this:
 *
 * @code
 *   typedef SqlDatabase::Table<int, ino_t, std::string, std::string> FileTable;
 *   void show_files(SqlDatabase::TransactionPtr &tx, int nlines, std::ostream &out) {
 *      FileTable table(tx->statement("select id, inode, name, owner from files where nlines=?")->bind(0, nlines));
 *      table.headers("ID", "Inode", "File Name", "Owner Name");
 *      table.reprint_headers(100); //print the headers again every 100 lines
 *      table.print(out);
 *      out <<"number of matching files: " <<table.size() <<"\n";
 *   }
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

/** Shared-ownership pointer to a database connection.  Database connections are always referenced through their smart pointers
 *  and are automatically deleted when all references disappear. See @ref Connection::create and @ref
 *  heap_object_shared_ownership. */
typedef boost::shared_ptr<Connection> ConnectionPtr;

/** Shared-ownership pointer to a transaction.  Transactions are always referenced through their smart pointers and are
 *  automatically deleted when all references disappear. See @ref Connection::transaction and @ref
 *  heap_object_shared_ownership. */
typedef boost::shared_ptr<Transaction> TransactionPtr;

/** Shared-ownership pointer to a statement.  Statements are always referenced through their smart pointers and are
 *  automatically deleted when all references disappear. See @ref Transaction::statement and @ref
 *  heap_object_shared_ownership.  */
typedef boost::shared_ptr<Statement> StatementPtr;

// Data type used in templates to indicate lack of a column
class NoColumn {};

/*******************************************************************************************************************************
 *                                      Exceptions
 *******************************************************************************************************************************/

/** Exceptions thrown by database operations. */
class Exception: public Rose::Exception {
public:
    explicit Exception(const char *mesg): Rose::Exception(mesg) {}
    explicit Exception(const std::string &mesg): Rose::Exception(mesg) {}
    explicit Exception(const std::string &mesg, const ConnectionPtr &conn, const TransactionPtr &tx,
                       const StatementPtr &stmt)
        : Rose::Exception(mesg), connection(conn), transaction(tx), statement(stmt) {}

    virtual ~Exception() throw() {}
    virtual const char *what() const throw() ROSE_OVERRIDE;
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
    typedef std::pair<std::string /*name*/, std::string /*value*/> Parameter;

    /** Broken-out info about a URL. */
    struct ParsedUrl {
        Driver driver;                                  // database low-level driver
        std::string dbName;                             // main part of URL -- database name or maybe a file name
        std::vector<Parameter> params;                  // stuff after the last "?"
        std::string error;                              // error message if there was a problem parsing

        ParsedUrl()
            : driver(NO_DRIVER) {}
    };
    
    /** Create a new database connection.  All connection objects are bound to a database throughout their lifetime, although
     * depending on the driver, the actual low-level connection may open and close. The @p open_spec string describes how to
     * connect to the database and its format varies depending on the underlying database driver.  If no @p driver is specified
     * then a driver is chosen (if possible) based on the @p open_spec. The underlying low-level connections are automatically
     * closed when all transactions terminate. */
    static ConnectionPtr create(const std::string &open_spec, Driver driver=NO_DRIVER) {
        if (NO_DRIVER==driver) {
            if (NO_DRIVER==(driver = guess_driver(open_spec)))
                throw Exception("no suitable driver for \""+open_spec+"\"");
        }
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

    /** Returns the open specification used when the connection was established. */
    std::string openspec() const;

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

    /** Converts a uniform resource locator to a driver specific string. */
    static std::string connectionSpecification(const std::string &uri, Driver driver = NO_DRIVER);

    /** Parse a URL to return its parts. */
    static ParsedUrl parseUrl(std::string url);

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
    friend class ConnectionImpl;
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

    /** Bulk load data into table.  The specified input stream contains comma-separated values which are inserted in bulk
     *  into the specified table.  The number of fields in each row of the input stream must match the number of columns
     *  in the table. Some drivers require that a bulk load is the only operation performed in a transaction. */
    void bulk_load(const std::string &tablename, std::istream&);

    /** Returns the low-level driver name for this transaction. */
    Driver driver() const;

    /** Returns the connection for this transaction. */
    ConnectionPtr connection() const;

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
        std::vector<uint8_t> get_blob(size_t idx);
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
    StatementPtr bind(size_t idx, const std::vector<uint8_t> &val);
    StatementPtr bind_null(size_t idx);
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

    /** Execute a statement that returns a single double. */
    double execute_double();

    /** Execute a statement that returns a single std::string. */
    std::string execute_string();
    
    /** Execute a statement that returns a single blob. */
    std::vector<uint8_t> execute_blob();

    /** Returns the low-level driver name for this statement. */
    Driver driver() const;

    /** Returns the transaction for this statement. */
    TransactionPtr transaction() const;

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

template<> NoColumn Statement::iterator::get<NoColumn>(size_t idx);
template<> int64_t Statement::iterator::get<int64_t>(size_t idx);
template<> uint64_t Statement::iterator::get<uint64_t>(size_t idx);
template<> int32_t Statement::iterator::get<int32_t>(size_t idx);
template<> uint32_t Statement::iterator::get<uint32_t>(size_t idx);
template<> float Statement::iterator::get<float>(size_t idx);
template<> double Statement::iterator::get<double>(size_t idx);
template<> std::string Statement::iterator::get<std::string>(size_t idx);

/*******************************************************************************************************************************
 *                                      Miscellaneous functions
 *******************************************************************************************************************************/

/** Documentation about database connection URIs. */
std::string uriDocumentation();

/** Split SQL source code into individual statements.  This is not a full parser--it only looks for top-level semicolons. */
std::vector<std::string> split_sql(const std::string &sql);

/** Produce an SQL string literal from a C++ string. If do_quote is false then don't add the surrounding quote characters. */
std::string escape(const std::string&, Driver, bool do_quote=true);

/** Produce an SQL hexadecimal sequence from an uint8_t vector. */
std::string hexSequence(const std::vector<uint8_t> &v, Driver driver);

/** Returns true if @p name is a valid table name. */
bool is_valid_table_name(const std::string &name);

/** Converts a container of values to an SQL "in" clause. */
template<class Container>
std::string in(const Container &values)
{
    std::ostringstream retval;
    retval <<"in (";
    unsigned nvals = 0;
    for (typename Container::const_iterator vi=values.begin(); vi!=values.end(); ++vi, ++nvals)
        retval <<(nvals?", ":"") <<*vi;
    retval <<")";
    return retval.str();
}

/** Converts a container of numbers to an SQL "in" clause using StringUtility to format them. */
template<class Container, class Stringifier>
std::string in_numbers(const Container &values, Stringifier &stringifier)
{
    return "in (" + Rose::StringUtility::join(", ", Rose::StringUtility::toStrings(values, stringifier)) + ")";
}

/** Converts a container of strings to an SQL "in" clause of strings. The @p driver is necessary in order to properly
 *  quote and escape the supplied values. */
template<class Container>
std::string in_strings(const Container &values, Driver driver)
{
    std::vector<std::string> strings;
    for (typename Container::const_iterator vi=values.begin(); vi!=values.end(); ++vi)
        strings.push_back(escape(*vi, driver));
    return in(strings);
}

std::ostream& operator<<(std::ostream&, const NoColumn&);
std::ostream& operator<<(std::ostream&, const Exception&);
std::ostream& operator<<(std::ostream&, const Connection&);
std::ostream& operator<<(std::ostream&, const Transaction&);
std::ostream& operator<<(std::ostream&, const Statement&);

/*******************************************************************************************************************************
 *                                      Tables
 *******************************************************************************************************************************/

// The things here that don't have doxygen comments are not intended to be used directly by users.

template<typename> struct ColumnTraits {
    enum { valid_column = 1 };
};
template<> struct ColumnTraits<NoColumn> {
    enum { valid_column = 0 };
};

/** Renders a column value as a string.  This is a base class for user-defined renderers. */
template<typename T>
class Renderer {
public:
    virtual ~Renderer() {}

    /** Renders a column value as a string.  The @p width is zero when this functor is called while Table::print() is
     *  trying to determine the maximum width of a value. */
    virtual std::string operator()(const T &value, size_t width) const {
        std::ostringstream ss;
        ss <<value;
        return ss.str();
    }
};

// Specialized for string-valued columns in order to left-justify the string by padding it on the right with an
// appropriate number of SPC characters.
template<>
class Renderer<std::string> {
public:
    virtual ~Renderer() {}
    virtual std::string operator()(const std::string &value, size_t width) const {
        return value + std::string(width, ' ');
    }
};

/** Renders a rose_addr_t as a hexadecimal string. */
struct AddrRenderer: Renderer<uint64_t> {
    size_t nbits; // number of bits in addresses; higher addresses will result in more digits
    explicit AddrRenderer(size_t nbits=32): nbits(nbits) {}
    virtual std::string operator()(const uint64_t &value, size_t width) const ROSE_OVERRIDE {
        return Rose::StringUtility::addrToString(value, nbits);
    }
};
extern AddrRenderer addr8Renderer;              /**< Renders 8-bit addresses in hexadecimal. */
extern AddrRenderer addr16Renderer;             /**< Renders 16-bit addresses in hexadecimal. */
extern AddrRenderer addr32Renderer;             /**< Renders 32-bit addresses in hexadecimal. */
extern AddrRenderer addr64Renderer;             /**< Renders 64-bit addresses in hexadecimal. */

/** Renders a time_t as a string. */
struct TimeRenderer: Renderer<time_t> {
    bool local_tz;              /**< Use local time zone rather than GMT. */
    std::string format;         /**< Format, as for strftime(). */
    TimeRenderer(): local_tz(true), format("%F %T %z") {}
    explicit TimeRenderer(const std::string &format, bool local_tz=true): local_tz(local_tz), format(format) {}
    virtual std::string operator()(const time_t &value, size_t width) const ROSE_OVERRIDE;
};
extern TimeRenderer timeRenderer;               /**< Renders time_t as YYYY-MM-DD HH:MM:SS in the local timezone. */
extern TimeRenderer dateRenderer;               /**< Renders only the date portion of a time as YYYY-MM-DD in local timezone. */
extern TimeRenderer humanTimeRenderer;          /**< Renders a time using the current locale, which is more human-friendly. */

/** Called before and after printing each row of a table. */
template<typename Table>
class PrePostRow {
public:
    virtual ~PrePostRow() {}
    virtual void operator()(std::ostream &out, const Table *table, size_t rownum, const std::vector<size_t> &widths) {}
};

// Prints a header
template<typename T>
class Header {
public:
    void operator()(std::ostream &out, const std::string &prefix, const std::string &hdr, size_t width) {
        FormatRestorer fr(out);
        out <<prefix <<std::setw(width) <<hdr;
    }
};
template<>
class Header<std::string> {
public:
    void operator()(std::ostream &out, const std::string &prefix, const std::string &hdr, size_t width) {
        FormatRestorer fr(out);
        out <<prefix <<std::setw(width) <<std::left <<hdr;
    }
};
template<>
class Header<NoColumn> {
public:
    void operator()(std::ostream &out, const std::string &prefix, const std::string &hdr, size_t width) {}
};

// Prints a row separator for a single column
template<typename T>
class RowSeparator {
public:
    void operator()(std::ostream &out, const std::string &prefix, size_t width) {
        out <<prefix <<std::string(width, '-');
    }
};
template<>
class RowSeparator<NoColumn> {
public:
    void operator()(std::ostream &out, const std::string &prefix, size_t width) {}
};

/** In-memory representation of a database table.
 *
 *  These can be used to hold results of a database table, to build a table in memory, or (mostly) for printing a table
 *  in an easy-to-read format.  For instance, here's a very simple, no-frills way to print the results from a query:
 *
 * @code
 *  SqlDatabase::TransactionPtr tx = ...;
 *  SqlDatabase::StatementPtr stmt = tx->statement("select id, ninsns, name, addr from table where ninsns = ?");
 *  SqlDatabase::Table<int, size_t, std::string, rose_addr_t>(stmt->bind(0, ninsns)).print(std::cout);
 * @endcode
 *
 *  Tables can also be used to operate on the results of a query.  For instance, to calculate the average number of
 *  instructions and bytes per function, one could use a counting-loop-over-array paradigm, which is more comfortable to some
 *  programmers than iterators:
 *
 * @code
 *  typedef SqlDatabase::Table<size_t, size_t> InsnBytesTable;
 *  InsnBytesTable table(tx->statement("select ninsns, bytes from functions"));
 *  size_t ninsns_sum=0, nbytes_sum=0;
 *  for (size_t i=0; i<table.size(); ++i) {
 *      ninsns_sum += table[i].v0; // value from column #0
 *      nbytes_sum += table[i].v1; // value from column #1
 *  }
 *  std::cout <<"average number of instructions: " <<(double)ninsns_sum/table.size() <<"\n"
 *            <<"average size in bytes:          " <<(double)nbytes_sum/table.size() <<"\n";
 * @endcode
 *
 *  Tables can also accumulate the results from more than one query as long as those queries return the same number of
 *  columns and column types:
 *
 * @code
 *  typedef SqlDatabase::Table<int, size_t, std::string> MyTable;
 *  MyTable.insert(tx->query("select id, ninsns, name from functions where size < ?")->bind(0, 100));
 *  MyTable.insert(tx->query("select hashval, size, desc from failures"));
 * @endcode
 *
 *  If you want headers at the top of a printed table, give the columns names like this:
 *
 * @code
 *  typedef SqlDatabase::Table<int, size_t, std::string, rose_addr_t> ResultsTable;
 *  ResultsTable table(stmt->bind(0, ninsns));
 *  table.headers("Id", "NInsns", "Name", "Address");
 * @endcode
 *
 *  It is also possible to specify a rendering for certain columns.  For instance, the query above returns an address of type
 *  rose_addr_t for column #3 (column numbering is always zero-origin).  If we want the address to be formatted as a
 *  zero-padded, eight character hexadecimal number with a "0x" prefix, then we could do that with code like this (building
 *  from the previous example):
 *
 * @code
 *  struct AddrRenderer: SqlDatabase::Renderer<rose_addr_t> {
 *      std::string operator()(const rose_addr_t &value, size_t width) const override {
 *          return StringUtility::addrToString(value);
 *      }
 *  } addressRenderer;
 *
 *  table.renderers().r3 = &addrRenderer;
 * @endcode
 *
 * If we want the headers to be printed again every 50th row, we can do that with a PrePostRow functor. These functors have
 * row info available to them so they can do fancier things too, like printing separators between major portions of a table.
 *
 * @code
 *  struct PeriodicSeparator: SqlDatabase::PrePostRow<ResultsTable> {
 *      void operator()(std::ostream &out, const ResultsTable *table, size_t rownum, const std::vector<size_t> &widths) {
 *          if (0!=rownum && 0==rownum % 50)
 *              table->print_rowsep(out, widths);
 *      }
 *  } periodicSeparator;
 *  table.prepost(&periodicSeparator, NULL);
 * @endcode
 *
 */
template<typename T00,          typename T01=NoColumn, typename T02=NoColumn, typename T03=NoColumn,
         typename T04=NoColumn, typename T05=NoColumn, typename T06=NoColumn, typename T07=NoColumn,
         typename T08=NoColumn, typename T09=NoColumn, typename T10=NoColumn, typename T11=NoColumn,
         typename T12=NoColumn, typename T13=NoColumn, typename T14=NoColumn, typename T15=NoColumn>
class Table {
public:
    enum {
        MAXCOLS=16,             /**< Maximum number of columns allowed in a table. */
    };

    /** One row of a table. */
    struct Tuple { // We could use boost::tuple, but it has a 10-member limit.
        explicit Tuple(const T00 &v0,        const T01 &v1 =T01(), const T02 &v2 =T02(), const T03 &v3 =T03(),
                       const T04 &v4 =T04(), const T05 &v5 =T05(), const T06 &v6 =T06(), const T07 &v7 =T07(),
                       const T08 &v8 =T08(), const T09 &v9 =T09(), const T10 &v10=T10(), const T11 &v11=T11(),
                       const T12 &v12=T12(), const T13 &v13=T13(), const T14 &v14=T14(), const T15 &v15=T15())
            : v0(v0), v1(v1), v2(v2),   v3(v3),   v4(v4),   v5(v5),   v6(v6),   v7(v7),
              v8(v8), v9(v9), v10(v10), v11(v11), v12(v12), v13(v13), v14(v14), v15(v15) {}
        T00 v0; T01 v1; T02 v2;  T03 v3;  T04 v4;  T05 v5;  T06 v6;  T07 v7;
        T08 v8; T09 v9; T10 v10; T11 v11; T12 v12; T13 v13; T14 v14; T15 v15;
    };

    /** Functors for rendering column values. */
    struct Renderers { // We could use boost::tuple, but it has a 10-member limit.
        explicit Renderers(const Renderer<T00> *r0 =NULL, const Renderer<T01> *r1 =NULL,
                           const Renderer<T02> *r2 =NULL, const Renderer<T03> *r3 =NULL,
                           const Renderer<T04> *r4 =NULL, const Renderer<T05> *r5 =NULL,
                           const Renderer<T06> *r6 =NULL, const Renderer<T07> *r7 =NULL,
                           const Renderer<T08> *r8 =NULL, const Renderer<T09> *r9 =NULL,
                           const Renderer<T10> *r10=NULL, const Renderer<T11> *r11=NULL,
                           const Renderer<T12> *r12=NULL, const Renderer<T13> *r13=NULL,
                           const Renderer<T14> *r14=NULL, const Renderer<T15> *r15=NULL)
            : r0(r0), r1(r1), r2(r2),   r3(r3),   r4(r4),   r5(r5),   r6(r6),   r7(r7),
              r8(r8), r9(r9), r10(r10), r11(r11), r12(r12), r13(r13), r14(r14), r15(r15) {}
        const Renderer<T00> *r0;  const Renderer<T01> *r1;  const Renderer<T02> *r2;  const Renderer<T03> *r3;
        const Renderer<T04> *r4;  const Renderer<T05> *r5;  const Renderer<T06> *r6;  const Renderer<T07> *r7;
        const Renderer<T08> *r8;  const Renderer<T09> *r9;  const Renderer<T10> *r10; const Renderer<T11> *r11;
        const Renderer<T12> *r12; const Renderer<T13> *r13; const Renderer<T14> *r14; const Renderer<T15> *r15;
    };

    /** All rows of a table. */
    typedef std::vector<Tuple> Rows;

    /** Creates a new, empty table. */
    Table(): headers_(MAXCOLS), colsep_(" | "), pre_row_(NULL), post_row_(NULL), reprint_headers_(0) {}

    /** Creates a new table and initializes it by running the specified database query. */
    explicit Table(const StatementPtr &stmt)
        : headers_(MAXCOLS), colsep_(" | "), pre_row_(NULL), post_row_(NULL), reprint_headers_(0) {
        insert(stmt);
    }

    /** Headers to use when printing the table.
     * @{ */
    void header(int colnum, const std::string &hdr) {
        assert(colnum<headers_.size());
        headers_[colnum] = hdr;
    }
    const std::string& header(int colnum) const {
        assert(colnum<headers_.size());
        return headers_[colnum];
    }
    void headers(const std::string &h0,     const std::string &h1="",  const std::string &h2="",  const std::string &h3="",
                 const std::string &h4="",  const std::string &h5="",  const std::string &h6="",  const std::string &h7="",
                 const std::string &h8="",  const std::string &h9="",  const std::string &h10="", const std::string &h11="",
                 const std::string &h12="", const std::string &h13="", const std::string &h14="", const std::string &h15="") {
        headers_[ 0] = h0;  headers_[ 1] = h1;  headers_[ 2] = h2;  headers_[ 3] = h3;
        headers_[ 4] = h4;  headers_[ 5] = h5;  headers_[ 6] = h6;  headers_[ 7] = h7;
        headers_[ 8] = h8;  headers_[ 9] = h9;  headers_[10] = h10; headers_[11] = h11;
        headers_[12] = h12; headers_[13] = h13; headers_[14] = h14; headers_[15] = h15;
    }
    /** @} */

    /** How often should headers be reprinted?
     * @{ */
    void reprint_headers(size_t nrows) { reprint_headers_ = nrows; }
    size_t reprint_headers() const { return reprint_headers_; }
    /** @} */

    /** Accessor for value renderers.  Use these to change how values are displayed.  For instance, if a column has values
     *  that are addresses and you want to display them with StringUtility::addrToString(), then use code like this:
     *
     * @code
     * struct AddrRender: SqlDatabase::Renderer<rose_addr_t> {
     *     std::string operator()(const rose_addr_t &value, size_t width) const override {
     *         return StringUtility::addrToString(value);
     *     }
     * } addrRender;
     *
     * SqlDatabase::Table<int, std::string, rose_addr_t> table;
     * table.renderers().r2 = &addrRenderer;
     * @endcode
     *
     * The render pointer should not be deleted before the table is done being used.
     * @{ */
    Renderers& renderers() { return render_; }
    const Renderers& renderers() const { return render_; }
    /** @} */

    /** Accessor for functors called before and after each row is printed.
     * @{ */
    void prepost(PrePostRow<Table> *pre, PrePostRow<Table> *post) {
        pre_row_ = pre;
        post_row_ = post;
    }
    std::pair<PrePostRow<Table>*, PrePostRow<Table>*> prepost() const {
        return std::make_pair(pre_row_, post_row_);
    }
    /** @} */

    /** Return the number of rows in the table. */
    size_t size() const { return rows_.size(); }

    /** Returns true if table has no rows. */
    bool empty() const { return rows_.empty(); }

    /** Clear the table by removing all rows. */
    void clear() { rows_.clear(); }

    /** Return a reference to a particular row.
     * @{ */
    Tuple& operator[](size_t i) { assert(i<rows_.size()); return rows_[i]; }
    const Tuple& operator[](size_t i) const { assert(i<rows_.size()); return rows_[i]; }
    /** @} */


    /** Add a new row to the end of the table.
     * @{ */
    void insert(const Tuple &tuple) { rows_.push_back(tuple); }
    void insert(const T00 &v00,       const T01 &v01=T01(), const T02 &v02=T02(), const T03 &v03=T03(),
                const T04 &v04=T04(), const T05 &v05=T05(), const T06 &v06=T06(), const T07 &v07=T07(),
                const T08 &v08=T08(), const T09 &v09=T09(), const T10 &v10=T10(), const T11 &v11=T11(),
                const T12 &v12=T12(), const T13 &v13=T13(), const T14 &v14=T14(), const T15 &v15=T15()) {
        rows_.push_back(Tuple(v00, v01, v02, v03, v04, v05, v06, v07, v08, v09, v10, v11, v12, v13, v14, v15));
    }
    /** @} */

    /** Insert more rows into a table by running the specified query. The table is not cleared first. */
    void insert(const StatementPtr &stmt) {
        for (Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
            rows_.push_back(Tuple(row.get<T00>( 0), row.get<T01>( 1), row.get<T02>( 2), row.get<T03>( 3),
                                  row.get<T04>( 4), row.get<T05>( 5), row.get<T06>( 6), row.get<T07>( 7),
                                  row.get<T08>( 8), row.get<T09>( 9), row.get<T10>(10), row.get<T11>(11),
                                  row.get<T12>(12), row.get<T13>(13), row.get<T14>(14), row.get<T15>(15)));
        }
    }

    /** String to print at the beginning of every line of output.
     * @{ */
    void line_prefix(const std::string &s) { prefix_ = s; }
    const std::string& line_prefix() const { return prefix_; }
    /** @} */

    /** Render a single value */
    template<typename T>
    std::string render(const Renderer<T> *r, const T &value, size_t width=0) const {
        Renderer<T> r_;
        if (NULL==r)
            r = &r_;
        std::string s = (*r)(value, width);
        if (width!=0)
            s = s.substr(0, width);
        return s;
    }

    /** Compute column widths.  Column widths are computed by internally printing the rows of the table and measuring
     *  the maximum width for each column. The renderers are called with zero for the column widths. */
    std::vector<size_t> colsizes() const {
        std::vector<size_t> widths(MAXCOLS, 0);
        for (typename Rows::const_iterator ri=rows_.begin(); ri!=rows_.end(); ++ri) {
            widths[ 0] = std::max(widths[ 0], render(render_.r0,  ri->v0 ).size());
            widths[ 1] = std::max(widths[ 1], render(render_.r1,  ri->v1 ).size());
            widths[ 2] = std::max(widths[ 2], render(render_.r2,  ri->v2 ).size());
            widths[ 3] = std::max(widths[ 3], render(render_.r3,  ri->v3 ).size());
            widths[ 4] = std::max(widths[ 4], render(render_.r4,  ri->v4 ).size());
            widths[ 5] = std::max(widths[ 5], render(render_.r5,  ri->v5 ).size());
            widths[ 6] = std::max(widths[ 6], render(render_.r6,  ri->v6 ).size());
            widths[ 7] = std::max(widths[ 7], render(render_.r7,  ri->v7 ).size());
            widths[ 8] = std::max(widths[ 8], render(render_.r8,  ri->v8 ).size());
            widths[ 9] = std::max(widths[ 9], render(render_.r9,  ri->v9 ).size());
            widths[10] = std::max(widths[10], render(render_.r10, ri->v10).size());
            widths[11] = std::max(widths[11], render(render_.r11, ri->v11).size());
            widths[12] = std::max(widths[12], render(render_.r12, ri->v12).size());
            widths[13] = std::max(widths[13], render(render_.r13, ri->v13).size());
            widths[14] = std::max(widths[14], render(render_.r14, ri->v14).size());
            widths[15] = std::max(widths[15], render(render_.r15, ri->v15).size());
        }
        return widths;
    }

    /** Print the headers for a table. */
    void print_headers(std::ostream &out, const std::vector<size_t> &widths) const {
        Header<T00>()(out, prefix_, headers_[ 0], widths[ 0]);
        Header<T01>()(out, colsep_, headers_[ 1], widths[ 1]);
        Header<T02>()(out, colsep_, headers_[ 2], widths[ 2]);
        Header<T03>()(out, colsep_, headers_[ 3], widths[ 3]);
        Header<T04>()(out, colsep_, headers_[ 4], widths[ 4]);
        Header<T05>()(out, colsep_, headers_[ 5], widths[ 5]);
        Header<T06>()(out, colsep_, headers_[ 6], widths[ 6]);
        Header<T07>()(out, colsep_, headers_[ 7], widths[ 7]);
        Header<T08>()(out, colsep_, headers_[ 8], widths[ 8]);
        Header<T09>()(out, colsep_, headers_[ 9], widths[ 9]);
        Header<T10>()(out, colsep_, headers_[10], widths[10]);
        Header<T11>()(out, colsep_, headers_[11], widths[11]);
        Header<T12>()(out, colsep_, headers_[12], widths[12]);
        Header<T13>()(out, colsep_, headers_[13], widths[13]);
        Header<T14>()(out, colsep_, headers_[14], widths[14]);
        Header<T15>()(out, colsep_, headers_[15], widths[15]);
        out <<"\n";
    }

    /** Print the row separator that goes between the headers and the data. */
    void print_rowsep(std::ostream &out, const std::vector<size_t> &widths) const {
        std::string colsep = colsep_;
        for (size_t i=0; i<colsep.size(); ++i) {
            if (isspace(colsep[i])) {
                colsep[i] = '-';
            } else if ('|'==colsep[i]) {
                colsep[i] = '+';
            }
        }
        RowSeparator<T00>()(out, prefix_, widths[ 0]);
        RowSeparator<T01>()(out, colsep,  widths[ 1]);
        RowSeparator<T02>()(out, colsep,  widths[ 2]);
        RowSeparator<T03>()(out, colsep,  widths[ 3]);
        RowSeparator<T04>()(out, colsep,  widths[ 4]);
        RowSeparator<T05>()(out, colsep,  widths[ 5]);
        RowSeparator<T06>()(out, colsep,  widths[ 6]);
        RowSeparator<T07>()(out, colsep,  widths[ 7]);
        RowSeparator<T08>()(out, colsep,  widths[ 8]);
        RowSeparator<T09>()(out, colsep,  widths[ 9]);
        RowSeparator<T10>()(out, colsep,  widths[10]);
        RowSeparator<T11>()(out, colsep,  widths[11]);
        RowSeparator<T12>()(out, colsep,  widths[12]);
        RowSeparator<T13>()(out, colsep,  widths[13]);
        RowSeparator<T14>()(out, colsep,  widths[14]);
        RowSeparator<T15>()(out, colsep,  widths[15]);
        out <<"\n";
    }

    /** Print one row of a table. */
    void
    print_row(std::ostream &out, const Tuple &t, const std::vector<size_t> &widths) const {
        FormatRestorer fr(out);
        out <<                                   prefix_       <<std::setw(widths[ 0]) <<render(render_.r0,  t.v0,  widths[ 0])
            <<(ColumnTraits<T01>::valid_column ? colsep_ : "") <<std::setw(widths[ 1]) <<render(render_.r1,  t.v1,  widths[ 1])
            <<(ColumnTraits<T02>::valid_column ? colsep_ : "") <<std::setw(widths[ 2]) <<render(render_.r2,  t.v2,  widths[ 2])
            <<(ColumnTraits<T03>::valid_column ? colsep_ : "") <<std::setw(widths[ 3]) <<render(render_.r3,  t.v3,  widths[ 3])
            <<(ColumnTraits<T04>::valid_column ? colsep_ : "") <<std::setw(widths[ 4]) <<render(render_.r4,  t.v4,  widths[ 4])
            <<(ColumnTraits<T05>::valid_column ? colsep_ : "") <<std::setw(widths[ 5]) <<render(render_.r5,  t.v5,  widths[ 5])
            <<(ColumnTraits<T06>::valid_column ? colsep_ : "") <<std::setw(widths[ 6]) <<render(render_.r6,  t.v6,  widths[ 6])
            <<(ColumnTraits<T07>::valid_column ? colsep_ : "") <<std::setw(widths[ 7]) <<render(render_.r7,  t.v7,  widths[ 7])
            <<(ColumnTraits<T08>::valid_column ? colsep_ : "") <<std::setw(widths[ 8]) <<render(render_.r8,  t.v8,  widths[ 8])
            <<(ColumnTraits<T09>::valid_column ? colsep_ : "") <<std::setw(widths[ 9]) <<render(render_.r9,  t.v9,  widths[ 9])
            <<(ColumnTraits<T10>::valid_column ? colsep_ : "") <<std::setw(widths[10]) <<render(render_.r10, t.v10, widths[10])
            <<(ColumnTraits<T11>::valid_column ? colsep_ : "") <<std::setw(widths[11]) <<render(render_.r11, t.v11, widths[11])
            <<(ColumnTraits<T12>::valid_column ? colsep_ : "") <<std::setw(widths[12]) <<render(render_.r12, t.v12, widths[12])
            <<(ColumnTraits<T13>::valid_column ? colsep_ : "") <<std::setw(widths[13]) <<render(render_.r13, t.v13, widths[13])
            <<(ColumnTraits<T14>::valid_column ? colsep_ : "") <<std::setw(widths[14]) <<render(render_.r14, t.v14, widths[14])
            <<(ColumnTraits<T15>::valid_column ? colsep_ : "") <<std::setw(widths[15]) <<render(render_.r15, t.v15, widths[15])
            <<"\n";
    }

    /** Print all rows of the table to the specified stream. */
    void print(std::ostream &out) const {
        std::vector<size_t> widths = colsizes();

        // Headers
        bool has_headers = false;
        for (size_t i=0; i<MAXCOLS; ++i) {
            widths[i] = std::max(widths[i], headers_[i].size());
            if (!headers_[i].empty())
                has_headers = true;
        }
        if (has_headers) {
            print_headers(out, widths);
            print_rowsep(out, widths);
        }

        // Body
        if (rows_.empty()) {
            out <<prefix_ <<"(0 rows)\n\n";
        } else {
            for (size_t i=0; i<rows_.size(); ++i) {
                if (i>0 && has_headers && reprint_headers_>0 && 0==i%reprint_headers_) {
                    print_rowsep(out, widths);
                    print_headers(out, widths);
                    print_rowsep(out, widths);
                }
                if (pre_row_)
                    (*pre_row_)(out, this, i, widths);
                print_row(out, rows_[i], widths);
                if (post_row_)
                    (*post_row_)(out, this, i, widths);
            }
        }

        // Epilogue
        if (has_headers && !rows_.empty())
            print_rowsep(out, widths);
    }

private:
    std::vector<std::string> headers_;
    Renderers render_;
    Rows rows_;
    std::string colsep_;
    PrePostRow<Table> *pre_row_, *post_row_;
    size_t reprint_headers_;
    std::string prefix_;                        // string printed before every line of output
};

} // namespace
#endif
