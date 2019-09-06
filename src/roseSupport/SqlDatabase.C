#define __STDC_LIMIT_MACROS

#include "rosePublicConfig.h"
#include "SqlDatabase.h"
#include "StringUtility.h"

#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#ifndef _MSC_VER
#include <sys/time.h>
#else
#include "timing.h"
#endif

#ifdef ROSE_HAVE_SQLITE3
#include "sqlite3x.h"
#endif

#ifdef ROSE_HAVE_LIBPQXX
#include <pqxx/pqxx>
#include <pqxx/tablewriter>
#endif

#include <cassert>

using namespace Rose;

namespace SqlDatabase {

/*******************************************************************************************************************************
 *                                      Exceptions
 *******************************************************************************************************************************/

void
Exception::print(std::ostream &o) const
{
    o <<what() <<"\n";
}

const char *
Exception::what() const throw()
{
    std::ostringstream ss;
    ss <<std::runtime_error::what();
    if (connection!=NULL)
        ss <<"\nconnection: " <<*connection;
    if (transaction!=NULL)
        ss <<"\ntransaction: " <<*transaction;
    if (statement!=NULL)
        ss <<"\n" <<*statement;
    what_str = ss.str();
    return what_str.c_str();
}

/*******************************************************************************************************************************
 *                                      Connections
 *******************************************************************************************************************************/


class ConnectionImpl {
public:
    ConnectionImpl(const std::string &open_spec, Driver driver): open_spec(open_spec), driver(driver), debug(NULL) {
        assert(driver!=NO_DRIVER);
    }

    // Returns a driver-level connection number for a new transaction and increments the pending count for that connection.
    size_t conn_for_transaction();

    // Decrements the reference count for a driver-level connection number.  This may or may not close that connection.
    void dec_driver_connection(size_t idx);

    std::string open_spec;              // specification for opening a connection
    Driver driver;                      // low-level driver number
    FILE *debug;                        // optional debugging stream

    // Most drivers allow only one outstanding transaction per connection, so we create enough connections to handle all the
    // outstanding transactions.
    struct DriverConnection {
        size_t nrefs;                   // number of transactions using this connection
#ifdef ROSE_HAVE_SQLITE3
        sqlite3x::sqlite3_connection* sqlite3_connection;
#endif
#ifdef ROSE_HAVE_LIBPQXX
        pqxx::connection* postgres_connection;
#endif
        DriverConnection(): nrefs(0)
#ifdef ROSE_HAVE_SQLITE3
                            , sqlite3_connection(NULL)
#endif
#ifdef ROSE_HAVE_LIBPQXX
                            , postgres_connection(NULL)
#endif
            {}
        ~DriverConnection() {
            assert(0==nrefs);
#ifdef ROSE_HAVE_SQLITE3
            delete sqlite3_connection;
            sqlite3_connection = NULL;
#endif
#ifdef ROSE_HAVE_LIBPQXX
            delete postgres_connection;
            postgres_connection = NULL;
#endif
        }
    };

    typedef std::vector<DriverConnection> DriverConnections;
    DriverConnections driver_connections;

};

#ifdef ROSE_HAVE_SQLITE3
static std::string
sqlite3_url_documentation() {
    return ("@named{SQLite3}{The uniform resource locator for SQLite3 databases has the format "
            "\"sqlite3://@v{filename}[?@v{param1}[=@v{value1}][&@v{additional_parameters}...]]\" "
            "where @v{filename} is the name of a file in the local filesystem (use a third slash if the name "
            "is an absolute name from the root of the filesystem). The file name can be followed by zero or "
            "parameters separated from the file name by a question mark and from each other by an ampersand. "
            "Each parameter has an optional setting. At this time, the only parameter that is understood is "
            "\"debug\", which takes no value.}");
}
#endif

// Parse an sqlite3 URL of the form:
//    sqlite3://FILENAME[?PARAM1[=VALUE1]&...]
// The only parameter that's currently understood is "debug", which turns on the debug property for the connection.
static std::string
sqlite3_parse_url(const std::string &src, bool *has_debug/*in,out*/) {
    Connection::ParsedUrl parsed = Connection::parseUrl(src);
    if (parsed.driver != SqlDatabase::SQLITE3)
        throw Exception("malformed SQLite3 connection URL: missing \"sqlite3://\"");
    if (!parsed.error.empty())
        throw Exception(parsed.error);

    std::string fileName = parsed.dbName;

    BOOST_FOREACH (const Connection::Parameter &param, parsed.params) {
        if ("debug" == param.first) {
            if (has_debug)
                *has_debug = true;
        } else {
            throw Exception("invalid SQLite3 parameter \"" + StringUtility::cEscape(param.first) + "\"");
        }
    }

    return fileName;
}

 // Added ifdef to remove unused function warning [Rasmussen, 2019.01.28]
#if defined(ROSE_HAVE_SQLITE3) || defined(ROSE_HAVE_LIBPQXX)
static std::string
postgres_url_documentation() {
    return ("@named{PostgreSQL}{The uniform resource locator for PostgreSQL databases has the format "
            "\"postgresql://[@v{user}[:@v{password}]@][@v{hostname}[:@v{port}]][/@v{database}]"
            "[?@v{param1}[=@v{value1}][&@v{additional_parameters}...]]\" where @v{user} is the database user "
            "name; @v{password} is the user's password; @v{hostname} is the host name or IP address of the database "
            "server, defaulting to the localhost; @v{port} is the TCP port number at which the server listens; "
            "and @v{database} is the name of the database. The rest of the URI consists of optional parameters separated "
            "from the prior part of the URI by a question mark and separated from each other by ampersands.  The only "
            "parameter that is understood at this time is \"debug\", which takes no value and causes each SQL statement "
            "to be emitted to standard error as it's executed.}");
}
#endif

// Documentation for lipqxx says pqxx::connection's argument is whatever libpq connect takes, but apparently URLs don't work.
// This function converts a postgresql connection URL into an old-style libpq connection string.  A url is of the form:
//    postgresql://[USER[:PASSWORD]@][NETLOC][:PORT][/DBNAME][?PARAM1[=VALUE1]&...]
// The has_debug argument is set if a "debug" parameter is found.
static std::string
postgres_parse_url(const std::string &url, bool *has_debug/*in,out*/) {
    Connection::ParsedUrl parsed = Connection::parseUrl(url);

    if (parsed.driver != SqlDatabase::POSTGRESQL)
        throw Exception("malformed PostgreSQL connection URL: missing \"postgresql://\"");
    if (!parsed.error.empty())
        throw Exception(parsed.error);
    std::string src = parsed.dbName;

    // The username and password, everything up to the next '@' character if there is one.
    std::string user, password;
    {
        size_t atsign = src.find('@');
        if (atsign != std::string::npos) {
            std::vector<std::string> parts = StringUtility::split(":", src.substr(0, atsign), 2);
            user = parts[0];
            if (parts.size() == 2)
                password = parts[1];
            src = src.substr(atsign+1);
        }
    }

    // Host name and port, everything up to the next '/' character if there is one
    std::string netloc, port;
    {
        size_t slash = src.find('/');
        if (slash != std::string::npos) {
            std::vector<std::string> parts = StringUtility::split(":", src.substr(0, slash), 2);
            netloc = parts[0];
            if (parts.size() == 2)
                port = parts[1];
            src = src.substr(slash+1);
        }
    }

    // Optional database name is everything left over.
    std::string dbname = src;
    
    // Parameters
    std::vector<std::string> url_params;
    BOOST_FOREACH (const Connection::Parameter &param, parsed.params) {
        if ("debug" == param.first) {
            if (has_debug != NULL)
                *has_debug = true;
        } else {
            url_params.push_back(param.first + "=" + param.second);
        }
    }

    // Collect all parameters in the correct order
    std::vector<std::string> params;
    if (!netloc.empty()) {
        if (netloc.find_first_not_of("0123456789.")==std::string::npos) {
            params.push_back("hostaddr="+netloc); // must be an IP address
        } else {
            params.push_back("host=" + netloc); // must be a host name
        }
    }
    if (!port.empty())
        params.push_back("port=" + port);
    if (!dbname.empty())
        params.push_back("dbname=" + dbname);
    if (!user.empty())
        params.push_back("user=" + user);
    if (!password.empty())
        params.push_back("password=" + password);
    params.insert(params.end(), url_params.begin(), url_params.end());

    // Build the connection string
    std::string retval = StringUtility::listToString(params);
    return retval;
}

size_t
ConnectionImpl::conn_for_transaction()
{
    // Find first driver connection that has a zero reference count, or allocate a fresh one
    size_t retval = (size_t)(-1);
    for (size_t i=0; i<driver_connections.size(); ++i) {
        if (0==driver_connections[i].nrefs) {
            retval = i;
            break;
        }
    }
    if (retval==(size_t)(-1)) {
        retval = driver_connections.size();
        driver_connections.resize(retval+10);
    }

#if defined(ROSE_HAVE_SQLITE3) || defined(ROSE_HAVE_LIBPQXX)
    DriverConnection &dconn = driver_connections[retval];
#endif

    // Fill in the necessary info for the driver connection and establish the connection
    switch (driver) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            if (dconn.sqlite3_connection==NULL) {
                bool has_debug_param = false;
                std::string specs = 0==open_spec.substr(0, 10).compare("sqlite3://") ?
                                    sqlite3_parse_url(open_spec, &has_debug_param) :
                                    open_spec;
                if (has_debug_param && debug==NULL)
                    debug = stderr;
                if (debug && 0==retval)
                    fprintf(debug, "SqlDatabase::Connection: SQLite3 open spec: %s\n", specs.c_str());
                dconn.sqlite3_connection = new sqlite3x::sqlite3_connection(specs.c_str());
            }
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            if (dconn.postgres_connection==NULL) {
                bool has_debug_param = false;
                std::string specs = 0==open_spec.substr(0, 13).compare("postgresql://") ?
                                    postgres_parse_url(open_spec, &has_debug_param) :
                                    open_spec;
                if (has_debug_param && debug==NULL)
                    debug = stderr;
                if (debug && 0==retval)
                    fprintf(debug, "SqlDatabase::Connection: PostgreSQL open spec: %s\n", specs.c_str());
                dconn.postgres_connection = new pqxx::connection(specs);
                dconn.postgres_connection->set_client_encoding("UTF8");
            }
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }

    ++driver_connections[retval].nrefs;
    return retval;
}

void
ConnectionImpl::dec_driver_connection(size_t idx)
{
    assert(idx<driver_connections.size());
    DriverConnection &dconn = driver_connections[idx];
    assert(dconn.nrefs>0);
    if (--dconn.nrefs > 0)
        return;

    switch (driver) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            // keep the connection around in case we need it later
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            // Once a transaction is committed or rolled back the connection is no good for anything else; we can't
            // create more transactions on that connection.

            // Also, libpqxx3 has a bug in the pqxx::transaction<> destructor: if the transaction commit() method is called,
            // then the d'tor executes a conditional jump or move that depends on unitialized values (according to valgrind).
            // This occurs in:
            //     pqxx::basic_connection<pqxx::connect_direct>::~basic_connection() (basic_connection.hxx:73)
            //     pqxx::connection_base::~connection_base() (connection_base.hxx:164)
            //     std::map<std::string, pqxx::prepare::internal::prepared_def, ...>::~map() (map.h:103)
            // And eventually leads to a segmentation fault.  This appears to only happen when the debugging versions of
            // std::map are used--so we work around the bug buy not calling the destructor in that case.
#ifdef _GLIBCXX_DEBUG
            static bool reached;
            if (!reached) {
                std::cerr <<"SqlDatabase::ConnectionImpl::dec_driver_connection(): avoiding pqxx::transaction() d'tor"
                          <<" due to bugs when compiled with _GLIBCXX_DEBUG defined.\n";
                reached = true;
            }
#else
            delete dconn.postgres_connection;
#endif
            dconn.postgres_connection = NULL;
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Connection::init(const std::string &open_spec, Driver driver)
{
    if (NO_DRIVER==driver)
        driver = guess_driver(open_spec);
    impl = new ConnectionImpl(open_spec, driver);
}

void
Connection::finish()
{
    delete impl;
}

// class method
Connection::ParsedUrl
Connection::parseUrl(std::string url) {
    ParsedUrl retval;

    // Optional driver (stuff before first "://")
    size_t next = url.find("://");
    if (next != std::string::npos) {
        std::string driverName = url.substr(0, next);
        if ("sqlite3" == driverName) {
            retval.driver = SQLITE3;
        } else if ("postgresql" == driverName) {
            retval.driver = POSTGRESQL;
        } else {
            retval.error = "invalid driver name \"" + StringUtility::cEscape(driverName) + "\"";
            return retval;
        }
        url = url.substr(next + 3);                     // everything after "://"
    } else {
        retval.driver = guess_driver(url);
    }

    // Datatabase is everything up to the last "?", but see below.
    next = url.rfind("?");
    if (next == std::string::npos) {
        retval.dbName = url;
        return retval;                                  // no parameters
    } else {
        retval.dbName = url.substr(0, next);
        url = url.substr(next+1);                       // everything after the "?"
    }

    // Parameters, each "name[=value]" and separated from one another by "&"
    std::vector<std::string> params = StringUtility::split("&", url);
    BOOST_FOREACH (const std::string &s, params) {
        std::vector<std::string> nameValue = StringUtility::split("=", s, 2);
        if (nameValue.size() == 1)
            nameValue.push_back("");
        retval.params.push_back(std::make_pair(nameValue[0], nameValue[1]));
    }

    return retval;
}

Driver
Connection::guess_driver(const std::string &open_spec)
{
    // URL format for postgres is:
    //     postgresql://[user[:password]@[netloc][:port][/dbname][?param1=value&...]
    // where the parameters are: host, hostaddr, port, dbname, user, password, connect_timeout, client_encoding,
    // options, application_name, fallback_application_name, keepalives, keepalives_idle, tty, sslmode, requiressl,
    // sslcompression sslcert, sslkey, sslrootcert, sslcrl, requirepeer, krbsrvname, gsslib, service
    if (0==open_spec.substr(0, 13).compare("postgresql://"))
        return POSTGRESQL;

    if (0==open_spec.substr(0, 10).compare("sqlite3://"))
        return SQLITE3;

    // If it looks like a file name ending with ".db" then it's probably an SQLite3 database.
    bool is_filename = true;
    for (size_t i=0; is_filename && i<open_spec.size(); ++i)
        is_filename = isgraph(open_spec[i]);
    if (is_filename && open_spec.size()>=4 && boost::ends_with(open_spec, ".db"))
        return SQLITE3;

    return NO_DRIVER;
}

Driver
Connection::driver() const
{
    return impl->driver;
}

std::string
Connection::openspec() const
{
    return impl->open_spec;
}

// class method
std::string
Connection::connectionSpecification(const std::string &url, Driver driver) {
    if (NO_DRIVER == driver)
        driver = guess_driver(url);
    switch (driver) {
        case SQLITE3:
            return sqlite3_parse_url(url, NULL);
        case POSTGRESQL:
            return postgres_parse_url(url, NULL);
        default:
            throw Exception("no suitable driver for \"" + url + "\"");
    }
}

TransactionPtr
Connection::transaction()
{
    return Transaction::create(shared_from_this());
}

void
Connection::set_debug(FILE *debug)
{
    assert(impl!=NULL);
    impl->debug = debug;
}

FILE *
Connection::get_debug() const
{
    assert(impl!=NULL);
    return impl->debug;
}

void
Connection::print(std::ostream &o) const
{
    assert(impl!=NULL);
    o <<impl->open_spec;
}

/*******************************************************************************************************************************
 *                                      Transactions
 *******************************************************************************************************************************/


class TransactionImpl {
public:
    TransactionImpl(const ConnectionPtr &conn, size_t drv_conn_idx): conn(conn), drv_conn_idx(drv_conn_idx) {
        assert(conn!=NULL);
        debug = conn->get_debug();
        init();
    }
    ~TransactionImpl() { finish(); }
    void init();
    void bulk_load(const std::string &tablename, std::istream&);
    void finish();
    void rollback();
    void commit();
    bool is_terminated() const;
    Driver driver() const;
    void print(std::ostream&) const;

    ConnectionPtr conn;         // Reference to the connection, or null when terminated
    size_t drv_conn_idx;        // index of driver connection number
    FILE *debug;                // optional debug stream

#ifdef ROSE_HAVE_SQLITE3
    sqlite3x::sqlite3_transaction *sqlite3_tranx;
#endif
#ifdef ROSE_HAVE_LIBPQXX
    pqxx::transaction<> *postgres_tranx;
#endif
};

void
TransactionImpl::init()
{
#ifdef ROSE_HAVE_SQLITE3
    sqlite3_tranx = NULL;
#endif
#ifdef ROSE_HAVE_LIBPQXX
    postgres_tranx = NULL;
#endif
    assert(drv_conn_idx < conn->impl->driver_connections.size());
#if defined(ROSE_HAVE_SQLITE3) || defined(ROSE_HAVE_LIBPQXX)
    ConnectionImpl::DriverConnection &dconn = conn->impl->driver_connections[drv_conn_idx];
    assert(dconn.nrefs>0);
#endif

    switch (driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            assert(dconn.sqlite3_connection != NULL);
            sqlite3_tranx = new sqlite3x::sqlite3_transaction(*dconn.sqlite3_connection);
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            assert(dconn.postgres_connection != NULL);
            std::string tranx_name = "Transaction_" + StringUtility::numberToString(drv_conn_idx);
            postgres_tranx = new pqxx::transaction<>(*dconn.postgres_connection, tranx_name);
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
}

void
TransactionImpl::bulk_load(const std::string &tablename, std::istream &file)
{
    switch (driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            sqlite3x::sqlite3_command *cmd = NULL;
            ConnectionImpl::DriverConnection &dconn = conn->impl->driver_connections[drv_conn_idx];
            try {
                char buf[4096];
                while (file.getline(buf, sizeof buf).good()) {
                    std::vector<std::string> tuple;
                    StringUtility::splitStringIntoStrings(buf, ',', tuple);
                    if (!cmd) {
                        std::string sql = "insert into "+tablename+" values(";
                        for (size_t i=0; i<tuple.size(); ++i)
                            sql += i?", ?":"?";
                        sql += ")";
                        cmd = new sqlite3x::sqlite3_command(*dconn.sqlite3_connection, sql);
                    }
                    for (size_t i=0; i<tuple.size(); ++i)
                        cmd->bind(i+1, tuple[i]); // sqlite3x bind() uses 1-origin indices
                    cmd->executenonquery();
                }
            } catch (...) {
                delete cmd;
                throw;
            }
            delete cmd;
            break;
        }
#endif
#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
#if 1 // [Robb Matzke 2019-04-19]: marked as deprecated, but alternative doesn't work yet
            pqxx::tablewriter twriter(*postgres_tranx, tablename);
            char buf[4096];
            while (file.getline(buf, sizeof buf).good()) {
                std::vector<std::string> tuple;
                StringUtility::splitStringIntoStrings(buf, ',', tuple);
                twriter.insert(tuple);
            }
            twriter.complete();
#else // this should for libpqxx-6.4.3 and probably some earlier versions also, but doesn't
            pqxx::stream_to stream(*postgres_tranx, tablename);
            char buf[4096];
            while (file.getline(buf, sizeof buf).good()) {
                std::vector<std::string> elmts;
                StringUtility::splitStringIntoStrings(buf, ',', elmts);
                stream <<elmts; // compile-time errors in libpqxx header files
            }
            stream.complete();
#endif
            break;
        }
#endif
        default:
            assert(!"database driver not supported");
            abort();
    }
}

void
TransactionImpl::finish()
{
    if (!is_terminated())
        rollback();

#ifdef ROSE_HAVE_SQLITE3
    delete sqlite3_tranx;
#endif
#ifdef ROSE_HAVE_LIBPQXX
    delete postgres_tranx;
#endif
}

Driver
TransactionImpl::driver() const
{
    assert(conn!=NULL);
    return conn->driver();
}

void
TransactionImpl::rollback()
{
    assert(!is_terminated());
    if (debug) {
        std::ostringstream ss;
        ss <<"connection: " <<*conn <<"\n";
        ss <<"transaction: "; print(ss);
        fprintf(debug, "SqlDatabase: rolling back transaction\n%s\n", StringUtility::prefixLines(ss.str(), "    ").c_str());
    }

    switch (driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            assert(sqlite3_tranx != NULL);
            sqlite3_tranx->rollback();
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            assert(postgres_tranx != NULL);
            delete postgres_tranx;
            postgres_tranx = NULL;
            // Libpqxx gets a segmentation fault if we delete a connection whose transaction was rolled back. Therefore, we
            // set the transaction pointer to NULL here so it doesn't get deleted by the dec_driver_connection() call
            // below. [Robb P. Matzke 2013-06-18]
            assert(1==conn->impl->driver_connections[drv_conn_idx].nrefs);
            conn->impl->driver_connections[drv_conn_idx].postgres_connection = NULL; // intentional leak
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }

    conn->impl->dec_driver_connection(drv_conn_idx);
    conn.reset();
    drv_conn_idx = -1;
    assert(is_terminated());
}

void
TransactionImpl::commit()
{
    assert(!is_terminated());
    if (debug) {
        std::ostringstream ss;
        ss <<"connection: " <<*conn <<"\n";
        ss <<"transaction: "; print(ss);
        fprintf(debug, "SqlDatabase: committing transaction\n%s\n", StringUtility::prefixLines(ss.str(), "    ").c_str());
    }

    switch (driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            assert(sqlite3_tranx != NULL);
            sqlite3_tranx->commit();
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            assert(postgres_tranx != NULL);
            postgres_tranx->commit();
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }

    conn->impl->dec_driver_connection(drv_conn_idx);
    conn.reset();
    drv_conn_idx = -1;
    assert(is_terminated());
}

bool
TransactionImpl::is_terminated() const
{
    return conn==NULL;
}

void
TransactionImpl::print(std::ostream &o) const
{
    o <<"connection number " <<drv_conn_idx;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TransactionPtr
Transaction::create(const ConnectionPtr &conn)
{
    assert(conn!=NULL);
    size_t drv_conn_idx = conn->impl->conn_for_transaction();
    return TransactionPtr(new Transaction(conn, drv_conn_idx));
}

void
Transaction::init(const ConnectionPtr &conn, size_t drv_conn_idx)
{
    assert(conn!=NULL);
    assert(drv_conn_idx!=(size_t)(-1));
    assert(conn->impl->driver_connections[drv_conn_idx].nrefs > 0);
    impl = new TransactionImpl(conn, drv_conn_idx);
}

void
Transaction::finish()
{
    delete impl;
}

Driver
Transaction::driver() const
{
    return impl->driver();
}

ConnectionPtr
Transaction::connection() const
{
    return impl->conn;
}

void
Transaction::rollback()
{
    impl->rollback();
}

void
Transaction::commit()
{
    impl->commit();
}

bool
Transaction::is_terminated() const
{
    return impl->is_terminated();
}

StatementPtr
Transaction::statement(const std::string &sql)
{
    assert(!is_terminated());
    return Statement::create(shared_from_this(), sql);
}

void
Transaction::execute(const std::string &all)
{
    assert(!is_terminated());
    std::vector<std::string> sql = split_sql(all);
    for (size_t i=0; i<sql.size(); ++i)
        statement(sql[i])->execute();
}

void
Transaction::bulk_load(const std::string &tablename, std::istream &file)
{
    impl->bulk_load(tablename, file);
}

void
Transaction::set_debug(FILE *debug)
{
    assert(impl!=NULL);
    impl->debug = debug;
}

FILE *
Transaction::get_debug() const
{
    assert(impl!=NULL);
    return impl->debug;
}

void
Transaction::print(std::ostream &o) const
{
    assert(impl!=NULL);
    impl->print(o);
}


/*******************************************************************************************************************************
 *                                      Statements
 *******************************************************************************************************************************/

class StatementImpl {
public:
    StatementImpl(const TransactionPtr &tranx, const std::string &sql)
        : tranx(tranx), sql(sql), execution_seq(0), row_num(0) {
        assert(tranx!=NULL);
        debug = tranx->get_debug();
        init();
    }
    ~StatementImpl() { finish(); }
    void init();
    void finish();
    Driver driver() const;
    void bind_check(const StatementPtr &stmt, size_t idx);
    StatementPtr bind(const StatementPtr &stmt, size_t idx, int32_t);
    StatementPtr bind(const StatementPtr &stmt, size_t idx, uint32_t);
    StatementPtr bind(const StatementPtr &stmt, size_t idx, int64_t);
    StatementPtr bind(const StatementPtr &stmt, size_t idx, uint64_t);
    StatementPtr bind(const StatementPtr &stmt, size_t idx, double);
    StatementPtr bind(const StatementPtr &stmt, size_t idx, const std::string&);
    StatementPtr bind(const StatementPtr &stmt, size_t idx, const std::vector<uint8_t>&);
    StatementPtr bind_null(const StatementPtr &stmt, size_t idx);
    std::vector<size_t> findSubstitutionQuestionMarks(const std::string&);
    std::string expand();
    size_t begin(const StatementPtr &stmt);
    void print(std::ostream&) const;
    TransactionPtr tranx;
    std::string sql;            // with '?' placeholders
    std::string sql_expanded;   // with '?' placeholders expanded to bound values
    std::vector<std::pair<size_t/*position*/, std::string/*value*/> > placeholders;
    size_t execution_seq;       // number of times this statement was executed
    size_t row_num;             // high water mark from all existing iterators for this execution
    FILE *debug;                // optional debugging stream
#ifdef ROSE_HAVE_SQLITE3
    sqlite3x::sqlite3_command *sqlite3_cmd;
    sqlite3x::sqlite3_reader *sqlite3_cursor;
#endif
#ifdef ROSE_HAVE_LIBPQXX
    pqxx::result postgres_result;
    pqxx::result::const_iterator postgres_iter;
#endif
};

// Finds '?' in an SQL statement that correspond to the points where positional arguments are bound. These are
// question marks that are outside things like string literals.
std::vector<size_t>
StatementImpl::findSubstitutionQuestionMarks(const std::string &sql) {
    std::vector<size_t> retval;
    bool inStringLiteral = false;
    for (size_t i=0; i<sql.size(); ++i) {
        if ('\'' == sql[i]) {                           // quotes are escaped by doubling them
            inStringLiteral = !inStringLiteral;
        } else if ('?' == sql[i]) {
            if (!inStringLiteral)
                retval.push_back(i);
        }
    }
    return retval;
}

void
StatementImpl::init()
{
#ifdef ROSE_HAVE_SQLITE3
    sqlite3_cmd = NULL;
    sqlite3_cursor = NULL;
#endif

    std::vector<size_t> qmarks = findSubstitutionQuestionMarks(sql);
    BOOST_FOREACH (size_t i, qmarks)
        placeholders.push_back(std::make_pair(i, std::string()));
}

void
StatementImpl::finish()
{
#ifdef ROSE_HAVE_SQLITE3
    delete sqlite3_cursor;
    delete sqlite3_cmd;
#endif
}

Driver
StatementImpl::driver() const
{
    assert(tranx!=NULL);
    return tranx->driver();
}

void
StatementImpl::bind_check(const StatementPtr &stmt, size_t idx)
{
    if (idx>=placeholders.size()) {
        std::string mesg = "SQL statement has only " + StringUtility::numberToString(placeholders.size()) +
                           " placeholder" + (1==placeholders.size()?"":"s") + " but needs at least " +
                           StringUtility::numberToString(idx+1);
        throw Exception(mesg, tranx->impl->conn, tranx, stmt);
    }
}

StatementPtr
StatementImpl::bind(const StatementPtr &stmt, size_t idx, int32_t val)
{
    bind_check(stmt, idx);
    placeholders[idx].second = StringUtility::numberToString(val);
    return stmt;
}

StatementPtr
StatementImpl::bind(const StatementPtr &stmt, size_t idx, uint32_t val)
{
    bind_check(stmt, idx);
    placeholders[idx].second = StringUtility::numberToString(val);
    return stmt;
}

StatementPtr
StatementImpl::bind(const StatementPtr &stmt, size_t idx, int64_t val)
{
    bind_check(stmt, idx);
    placeholders[idx].second = StringUtility::numberToString(val);
    return stmt;
}

StatementPtr
StatementImpl::bind(const StatementPtr &stmt, size_t idx, uint64_t val)
{
    bind_check(stmt, idx);
    placeholders[idx].second = StringUtility::numberToString(val);
    return stmt;
}

StatementPtr
StatementImpl::bind(const StatementPtr &stmt, size_t idx, double val)
{
    bind_check(stmt, idx);
    placeholders[idx].second = StringUtility::numberToString(val);
    return stmt;
}

StatementPtr
StatementImpl::bind(const StatementPtr &stmt, size_t idx, const std::string &val)
{
    bind_check(stmt, idx);
    placeholders[idx].second = escape(val, tranx->driver());
    return stmt;
}

StatementPtr
StatementImpl::bind(const StatementPtr &stmt, size_t idx, const std::vector<uint8_t> &val)
{
    bind_check(stmt, idx);
    placeholders[idx].second = hexSequence(val, tranx->driver());
    return stmt;
}

StatementPtr
StatementImpl::bind_null(const StatementPtr &stmt, size_t idx)
{
    bind_check(stmt, idx);
    placeholders[idx].second = "NULL";
    return stmt;
}



// Expand some SQL by replacing substitution '?' with the value of the corresponding bound argument.
std::string
StatementImpl::expand()
{
    std::string s = sql;
    for (size_t i=placeholders.size(); i>0; --i) {
        ASSERT_require(placeholders[i-1].first < s.size());
        s.replace(placeholders[i-1].first, 1, placeholders[i-1].second);
    }
    boost::trim(s);
    return s;
}

size_t
StatementImpl::begin(const StatementPtr &stmt)
{
    assert(tranx!=NULL);
    assert(!tranx->is_terminated());

    for (size_t i=0; i<placeholders.size(); ++i) {
        if (placeholders[i].second.empty())
            throw Exception("placeholder " + StringUtility::numberToString(i) + " is not bound",
                            tranx->impl->conn, tranx, stmt);
    }

    sql_expanded = expand();
    execution_seq += 1;
    row_num = 0;
    struct timeval start_time;
    if (debug) {
        std::ostringstream ss;
        gettimeofday(&start_time, NULL);
        ss <<"connection: " <<*tranx->impl->conn <<"\ntransaction: " <<*tranx <<"\n";
        print(ss);
        fprintf(debug, "SqlDatabase: executing\n%s", StringUtility::prefixLines(ss.str(), "    ").c_str());
    }

    switch (driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            try {
                delete sqlite3_cursor;
                sqlite3_cursor = NULL;
                delete sqlite3_cmd;
                sqlite3_cmd = NULL;
                size_t drv_conn_idx = tranx->impl->drv_conn_idx;
                ConnectionImpl::DriverConnection &dconn = tranx->impl->conn->impl->driver_connections[drv_conn_idx];
                assert(dconn.sqlite3_connection!=NULL);
                sqlite3_cmd = new sqlite3x::sqlite3_command(*dconn.sqlite3_connection, sql_expanded);
                sqlite3_cursor = new sqlite3x::sqlite3_reader;
                *sqlite3_cursor = sqlite3_cmd->executereader();
                if (!sqlite3_cursor->read()) {
                    delete sqlite3_cursor;
                    sqlite3_cursor = NULL;
                    delete sqlite3_cmd;
                    sqlite3_cmd = NULL;
                }
            } catch (const std::runtime_error &e) {
                throw Exception(e.what(), tranx->impl->conn, tranx, stmt);
            }
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            try {
                postgres_result = tranx->impl->postgres_tranx->exec(sql_expanded);
                postgres_iter = postgres_result.begin();
            } catch (const std::runtime_error &e) { // postgres exception
                throw Exception(e.what(), tranx->impl->conn, tranx, stmt);
            }
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }

    if (debug) {
        struct timeval end_time;
        gettimeofday(&end_time, NULL);
        double elapsed = (double)(end_time.tv_sec-start_time.tv_sec) + ((double)end_time.tv_usec-start_time.tv_usec)*1e-9;
        fprintf(debug, "    elapsed time: %g seconds\n\n", elapsed);
    }

    return execution_seq;
}

void
StatementImpl::print(std::ostream &o) const
{
    if (0!=sql.compare(sql_expanded))
        o <<"original SQL:\n" <<StringUtility::prefixLines(sql, "    |") <<"\n";
    if (!sql_expanded.empty())
        o <<"executed SQL:\n" <<StringUtility::prefixLines(sql_expanded, "    |") <<"\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Statement::init(const TransactionPtr &tranx, const std::string &sql)
{
    assert(tranx!=NULL);
    assert(!tranx->is_terminated());
    impl = new StatementImpl(tranx, sql);
}

void
Statement::finish()
{
    delete impl;
}

Driver
Statement::driver() const
{
    return impl->driver();
}

TransactionPtr
Statement::transaction() const
{
    return impl->tranx;
}

Statement::iterator
Statement::begin()
{
    size_t execution_seq = impl->begin(shared_from_this());
    return iterator(shared_from_this(), execution_seq);
}

void
Statement::execute()
{
    begin();
}

int
Statement::execute_int()
{
    iterator i = begin();
    if (i==end())
        throw Exception("statement did not return any rows\n" + StringUtility::prefixLines(impl->sql, "  sql: ") + "\n",
                        impl->tranx->impl->conn, impl->tranx, shared_from_this());
    return i.get<int>(0);
}

double
Statement::execute_double()
{
    iterator i = begin();
    if (i==end())
        throw Exception("statement did not return any rows\n" + StringUtility::prefixLines(impl->sql, "  sql: ") + "\n",
                        impl->tranx->impl->conn, impl->tranx, shared_from_this());
    return i.get<double>(0);
}

std::string
Statement::execute_string()
{
    iterator i = begin();
    if (i==end())
        throw Exception("statement did not return any rows\n" + StringUtility::prefixLines(impl->sql, "  sql: ") + "\n",
                        impl->tranx->impl->conn, impl->tranx, shared_from_this());
    return i.get<std::string>(0);
}

std::vector<uint8_t> 
Statement::execute_blob()
{
    iterator i = begin();
    if (i==end())
        throw Exception("statement did not return any rows\n" + StringUtility::prefixLines(impl->sql, "  sql: ") + "\n",
                        impl->tranx->impl->conn, impl->tranx, shared_from_this());
    return i.get_blob(0);
}

void
Statement::set_debug(FILE *debug)
{
    assert(impl!=NULL);
    impl->debug = debug;
}

FILE *
Statement::get_debug() const
{
    assert(impl!=NULL);
    return impl->debug;
}

void
Statement::print(std::ostream &o) const
{
    assert(impl!=NULL);
    impl->print(o);
}

StatementPtr Statement::bind(size_t idx, int32_t val) { return impl->bind(shared_from_this(), idx, val); }
StatementPtr Statement::bind(size_t idx, int64_t val) { return impl->bind(shared_from_this(), idx, val); }
StatementPtr Statement::bind(size_t idx, uint32_t val) { return impl->bind(shared_from_this(), idx, val); }
StatementPtr Statement::bind(size_t idx, uint64_t val) { return impl->bind(shared_from_this(), idx, val); }
StatementPtr Statement::bind(size_t idx, double val) { return impl->bind(shared_from_this(), idx, val); }
StatementPtr Statement::bind(size_t idx, const std::string &val) { return impl->bind(shared_from_this(), idx, val); }
StatementPtr Statement::bind(size_t idx, const std::vector<uint8_t> &val) { return impl->bind(shared_from_this(), idx, val); }
StatementPtr Statement::bind_null(size_t idx) { return impl->bind_null(shared_from_this(), idx); }

/*******************************************************************************************************************************
 *                                      Statement iterators
 *******************************************************************************************************************************/

void
Statement::iterator::init()
{
    if (stmt!=NULL) {
        switch (stmt->driver()) {
#ifdef ROSE_HAVE_SQLITE3
            case SQLITE3: {
                if (stmt->impl->sqlite3_cmd==NULL)
                    stmt.reset();
                break;
            }
#endif

#ifdef ROSE_HAVE_LIBPQXX
            case POSTGRESQL: {
                if (stmt->impl->postgres_iter==stmt->impl->postgres_result.end())
                    stmt.reset();
                break;
            }
#endif

            default:
                assert(!"database driver not supported");
                abort();
        }
    }
}

bool
Statement::iterator::at_eof() const
{
    return stmt==NULL;
}

void
Statement::iterator::check() const
{
    if (stmt==NULL)
        throw Exception("iterator is at EOF");
    if (execution_seq!=stmt->impl->execution_seq)
        throw Exception("stale iterator from previous execution of statement");
    if (row_num!=stmt->impl->row_num)
        throw Exception("stale iterator from previous result row");
}

Statement::iterator &
Statement::iterator::operator++()
{
    check();
    switch (stmt->driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            if (!stmt->impl->sqlite3_cursor->read())
                stmt.reset();
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            if (++stmt->impl->postgres_iter == stmt->impl->postgres_result.end())
                stmt.reset();
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }

    if (stmt!=NULL)
        stmt->impl->row_num = ++row_num;
    return *this;
}

bool
Statement::iterator::operator==(const iterator &other) const
{
    if (stmt==NULL || other.stmt==NULL)
        return stmt==other.stmt;
    return execution_seq==other.execution_seq && row_num==other.row_num;
}

int32_t
Statement::iterator::get_i32(size_t idx)
{
    int32_t retval = 0;
    check();
    switch (stmt->driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            int64_t v = stmt->impl->sqlite3_cursor->getint64(idx);
            if (v<INT32_MIN || v>INT32_MAX)
                throw Exception("32-bit integer value is out of range");
            retval = v;
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            if (!stmt->impl->postgres_iter[(unsigned)idx].to(retval))
                throw Exception("32-bit integer value is null");
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
    return retval;
}

uint32_t
Statement::iterator::get_u32(size_t idx)
{
    uint32_t retval = 0;
    check();
    switch (stmt->driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            int64_t v = stmt->impl->sqlite3_cursor->getint64(idx);
            if (v<0 || v>UINT32_MAX)
                throw Exception("32-bit unsigned integer is out of range");
            retval = v;
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            if (!stmt->impl->postgres_iter[(unsigned)idx].to(retval))
                throw Exception("32-bit unsigned integer value is null");
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
    return retval;
}

int64_t
Statement::iterator::get_i64(size_t idx)
{
    int64_t retval = 0;
    check();
    switch (stmt->driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            retval = stmt->impl->sqlite3_cursor->getint64(idx);
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            if (!stmt->impl->postgres_iter[(unsigned)idx].to(retval))
                throw Exception("64-bit integer value is null");
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
    return retval;
}

uint64_t
Statement::iterator::get_u64(size_t idx)
{
    uint64_t retval = 0;
    check();
    switch (stmt->driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            int64_t v = stmt->impl->sqlite3_cursor->getint64(idx);
            if (v<0)
                throw Exception("64-bit unsigned integer is out of range");
            retval = v;
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            if (!stmt->impl->postgres_iter[(unsigned)idx].to(retval))
                throw Exception("64-bit unsigned integer value is null");
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
    return retval;
}

double
Statement::iterator::get_dbl(size_t idx)
{
    double retval = 0;
    check();
    switch (stmt->driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            retval = stmt->impl->sqlite3_cursor->getdouble(idx);
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            if (!stmt->impl->postgres_iter[(unsigned)idx].to(retval))
                throw Exception("floating point value is null");
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
    return retval;
}

std::string
Statement::iterator::get_str(size_t idx)
{
    std::string retval;
    check();
    switch (stmt->driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            retval = stmt->impl->sqlite3_cursor->getstring(idx);
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            if (!stmt->impl->postgres_iter[(unsigned)idx].to(retval))
                throw Exception("string value is null");
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
    return retval;
}

std::vector<uint8_t>
Statement::iterator::get_blob(size_t idx)
{
    std::vector<uint8_t> retval;
    check();
    switch (stmt->driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            std::string data = stmt->impl->sqlite3_cursor->getblob(idx);
            
            retval.reserve(data.size());
            std::copy(data.begin(), data.end(), std::back_inserter(retval));
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            assert(!"BLOBS (or BYTEA) not yet supported in Postgres interface");
            abort();
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
    return retval;
}


template<> NoColumn Statement::iterator::get<NoColumn>(size_t idx) { return NoColumn(); }
template<> int32_t Statement::iterator::get<int32_t>(size_t idx) { return get_i32(idx); }
template<> int64_t Statement::iterator::get<int64_t>(size_t idx) { return get_i64(idx); }
template<> uint32_t Statement::iterator::get<uint32_t>(size_t idx) { return get_u32(idx); }
template<> uint64_t Statement::iterator::get<uint64_t>(size_t idx) { return get_u64(idx); }
template<> float Statement::iterator::get<float>(size_t idx) { return get_dbl(idx); }
template<> double Statement::iterator::get<double>(size_t idx) { return get_dbl(idx); }
template<> std::string Statement::iterator::get<std::string>(size_t idx) { return get_str(idx); }

/*******************************************************************************************************************************
 *                                      Miscellaneous functions
 *******************************************************************************************************************************/

std::vector<std::string>
split_sql(const std::string &all_)
{
    std::string all = all_; // boost::algorithm_find_iterator needs a mutable string, not sure why; compiler errors otherwise
    std::vector<std::string> retval;
    boost::regex stmt_re("( ( '([^']|'')*'   )"     // string literal
                         "| ( --[^\n]*       )"     // comment
                         "| ( [^;]           )"     // other
                         ")+ (;|$)", boost::regex::perl|boost::regex::mod_x);
    typedef boost::algorithm::find_iterator<std::string::iterator> Sfi; // string find iterator
    for (Sfi i=make_find_iterator(all, boost::algorithm::regex_finder(stmt_re)); i!=Sfi(); ++i) {
        std::string stmt = boost::copy_range<std::string>(*i);
        size_t start=0, stop=stmt.size();
        while (start<stmt.size() && isspace(stmt[start])) ++start;
        if (start<stmt.size()) {
            while (stop>0 && isspace(stmt[stop-1])) --stop;
            assert(stop>=start);
            retval.push_back(stmt.substr(start, stop-start));
        }
    }
    return retval;
}

std::string
escape(const std::string &s, Driver driver, bool quote)
{
    bool has_backslash = false;
    std::string retval;
    size_t sz = s.size();
    for (size_t i=0; i<sz; ++i) {
        if ('\''==s[i]) {
            retval += "''";
        } else if ('\\'==s[i]) {
            has_backslash = true;
            retval += "\\\\";
        } else if (!isprint(s[i])) {
            has_backslash = true;
            retval += "\\";
            char buf[16];
            sprintf(buf, "%03o", (unsigned)s[i]);
            retval += buf;
        } else {
            retval += s[i];
        }
    }

    if (quote)
        retval = std::string(POSTGRESQL==driver && has_backslash ? "E" : "") + "'" + retval + "'";
    return retval;
}

struct hex_appender : std::iterator<std::output_iterator_tag, void, void, void, void>
{
    explicit
    hex_appender(std::string& res)
    : sink(&res)
    {}
    
    static
    char hexDigit(uint8_t digit)
    {
      ROSE_ASSERT(digit < 16);
      
      if (digit < 10) return '0' + digit;
      
      return 'A' + (digit - 10);
    }

    hex_appender& operator=(const uint8_t& value)
    {
      char hexval[] = { hexDigit(value/16), hexDigit(value%16), 0 };
      
      //~ std::cout << ": " << hexval << "." << std::endl;
      sink->append(hexval);
      return *this;
    }

    hex_appender& operator*()     { return *this; }
    hex_appender& operator++()    { return *this; }
    hex_appender& operator++(int) { return *this; }

  private:
    std::string* sink;
};


std::string
hexSequence(const std::vector<uint8_t> &v, Driver driver)
{
    static const std::string HEXPRE  = "X'";
    static const std::string HEXPOST = "'";

    size_t      len = HEXPRE.size() + 2*v.size() + HEXPOST.size();
    std::string retval;

    retval.reserve(len);
    retval.append(HEXPRE);
    std::copy(v.begin(), v.end(), hex_appender(retval));
    retval.append(HEXPOST);
    return retval;
}

bool
is_valid_table_name(const std::string &name)
{
    static const char *valid_chars = "abcdefghijklmnopqrstuvwxyz"
                                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "0123456789"
                                     "_";
    return !name.empty() && std::string::npos==name.find_first_not_of(valid_chars);
}

std::ostream& operator<<(std::ostream &o, const NoColumn&) { return o; }
std::ostream& operator<<(std::ostream &o, const Exception &x) { x.print(o); return o; }
std::ostream& operator<<(std::ostream &o, const Connection &x) { x.print(o); return o; }
std::ostream& operator<<(std::ostream &o, const Transaction &x) { x.print(o); return o; }
std::ostream& operator<<(std::ostream &o, const Statement &x) { x.print(o); return o; }

std::string
uriDocumentation() {
    std::string s;
#ifdef ROSE_HAVE_SQLITE3
    s += sqlite3_url_documentation();
#endif
#ifdef ROSE_HAVE_LIBPQXX
    s += postgres_url_documentation();
#endif
    if (s.empty())
        s = "No database drivers are configured.";
    return s;
}

/*******************************************************************************************************************************
 *                                      Tables
 *******************************************************************************************************************************/

std::string
TimeRenderer::operator()(const time_t &value, size_t width) const
{
    char buf[256];
#ifndef _MSC_VER
    struct tm tm;
    if (local_tz) {
        if (NULL==localtime_r(&value, &tm))
            return "(invalid time value)";
    } else {
        if (NULL==gmtime_r(&value, &tm))
            return "(invalid time value)";
    }
    if (0==strftime(buf, sizeof buf, format.c_str(), &tm) && !format.empty())
        return "(invalid format)";
#endif
    return buf;
}

AddrRenderer addr8Renderer(8), addr16Renderer(16), addr32Renderer(32), addr64Renderer(64);
TimeRenderer timeRenderer, dateRenderer("%F"), humanTimeRenderer("%c");

} // namespace
