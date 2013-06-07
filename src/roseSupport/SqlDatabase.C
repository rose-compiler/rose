#define __STDC_LIMIT_MACROS

#include "rosePublicConfig.h"
#include "SqlDatabase.h"
#include "string_functions.h" // i.e., namespace StringUtility

#ifdef ROSE_HAVE_SQLITE3
#include "sqlite3x.h"
#endif

#ifdef ROSE_HAVE_LIBPQXX
#include <pqxx/connection>
#include <pqxx/transaction>
#endif

#include <cassert>

namespace SqlDatabase {

/*******************************************************************************************************************************
 *                                      Connections
 *******************************************************************************************************************************/


class ConnectionImpl {
public:
    ConnectionImpl(const std::string &open_spec, Driver driver): open_spec(open_spec), driver(driver) {
        assert(driver!=NO_DRIVER);
    }

    ~ConnectionImpl();

    // Returns a driver-level connection number for a new transaction and increments the pending count for that connection.
    size_t conn_for_transaction();

    // Decrements the pending count for a driver-level connection number.  This may or may not close that connection.
    void dec_pending(size_t idx);

    std::string open_spec;              // specification for opening a connection
    Driver driver;                      // low-level driver number

    // Most drivers allow only one outstanding transaction per connection, so we create enough connections to handle all the
    // outstanding transactions.  The pending.size() is always equal to the driver's connections.size().
    std::vector<size_t> pending;
#ifdef ROSE_HAVE_SQLITE3
    std::vector<sqlite3x::sqlite3_connection*> sqlite3_connections;
#endif
#ifdef ROSE_HAVE_LIBPQXX
    std::vector<pqxx::connection*> postgres_connections;
#endif

};

ConnectionImpl::~ConnectionImpl()
{
    switch (driver) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            assert(pending.size()==sqlite3_connections.size());
            for (size_t i=0; i<sqlite3_connections.size(); ++i) {
                assert(0==pending[i]);
                if (sqlite3_connections[i]) {
                    delete sqlite3_connections[i];
                    sqlite3_connections[i] = NULL;
                }
            }
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            assert(pending.size()==postgres_connections.size());
            for (size_t i=0; i<postgres_connections.size(); ++i) {
                assert(0==pending[i]);
                if (postgres_connections[i]) {
                    delete postgres_connections[i];
                    postgres_connections[i] = NULL;
                }
            }
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
}

size_t
ConnectionImpl::conn_for_transaction()
{
    switch (driver) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            assert(pending.size()==sqlite3_connections.size());
            size_t retval = (size_t)(-1);
            for (size_t i=0; i<pending.size(); ++i) {
                if (0==pending[i]) {
                    retval = i;
                    break;
                }
            }
            if (retval==(size_t)(-1)) {
                retval = pending.size();
                pending.resize(pending.size()+10, 0);
                sqlite3_connections.resize(pending.size(), NULL);
            }
            if (sqlite3_connections[retval]==NULL) {
                sqlite3_connections[retval] = new sqlite3x::sqlite3_connection(open_spec.c_str());
                sqlite3_connections[retval]->busy_timeout(15*60*1000); // 15 minutes
            }
            ++pending[retval];
            return retval;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            assert(pending.size()==postgres_connections.size());
            size_t retval = (size_t)(-1);
            for (size_t i=0; i<pending.size(); ++i) {
                if (0==pending[i]) {
                    retval = i;
                    break;
                }
            }
            if (retval==(size_t)(-1)) {
                retval = pending.size();
                pending.resize(pending.size()+10, 0);
                postgres_connections.resize(pending.size(), NULL);
            }
            if (postgres_connections[retval]==NULL)
                postgres_connections[retval] = new pqxx::connection(open_spec);
            ++pending[retval];
            return retval;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }
}

void
ConnectionImpl::dec_pending(size_t idx)
{
    assert(idx<pending.size());
    assert(pending[idx]>0);
    --pending[idx];

    switch (driver) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            // keep the connection around in case we need it later
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            // keep the connection around in case we need it later
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

    // If it looks like a file name ending with ".db" then it's probably an SQLite3 database.
    bool is_filename = true;
    for (size_t i=0; is_filename && i<open_spec.size(); ++i)
        is_filename = isgraph(open_spec[i]);
    if (is_filename && open_spec.size()>=4 && 0==open_spec.substr(open_spec.size()-3).compare(".db"))
        return SQLITE3;

    return NO_DRIVER;
}

Driver
Connection::driver() const
{
    return impl->driver;
}

TransactionPtr
Connection::transaction()
{
    return Transaction::create(shared_from_this());
}

/*******************************************************************************************************************************
 *                                      Transactions
 *******************************************************************************************************************************/


class TransactionImpl {
public:
    TransactionImpl(const ConnectionPtr &conn, size_t drv_conn_idx): conn(conn), drv_conn_idx(drv_conn_idx) {
        init();
    }
    ~TransactionImpl() { finish(); }
    void init();
    void finish();
    void rollback();
    void commit();
    bool is_terminated() const;
    Driver driver() const;

    ConnectionPtr conn;         // Reference to the connection, or null when terminated
    size_t drv_conn_idx;        // index of driver connection number

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

    switch (driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            sqlite3x::sqlite3_connection *drv_conn = conn->impl->sqlite3_connections[drv_conn_idx];
            sqlite3_tranx = new sqlite3x::sqlite3_transaction(*drv_conn, sqlite3x::sqlite3_transaction::LOCK_IMMEDIATE);
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            std::string tranx_name = "Transaction_" + StringUtility::numberToString(drv_conn_idx);
            pqxx::connection *drv_conn = conn->impl->postgres_connections[drv_conn_idx];
            postgres_tranx = new pqxx::transaction<>(*drv_conn, tranx_name);
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
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }

    conn->impl->dec_pending(drv_conn_idx);
    conn.reset();
    drv_conn_idx = -1;
    assert(is_terminated());
}

void
TransactionImpl::commit()
{
    assert(!is_terminated());
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

    conn->impl->dec_pending(drv_conn_idx);
    conn.reset();
    drv_conn_idx = -1;
    assert(is_terminated());
}

bool
TransactionImpl::is_terminated() const
{
    return conn==NULL;
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
    assert(conn->impl->pending[drv_conn_idx] > 0);
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


/*******************************************************************************************************************************
 *                                      Statements
 *******************************************************************************************************************************/

class StatementImpl {
public:
    StatementImpl(const TransactionPtr &tranx, const std::string &sql)
        : tranx(tranx), sql(sql), execution_seq(0), row_num(0) { init(); }
    ~StatementImpl() { finish(); }
    void init();
    void finish();
    Driver driver() const;
    void bind_check(size_t idx);
    void bind(size_t idx, int32_t);
    void bind(size_t idx, uint32_t);
    void bind(size_t idx, int64_t);
    void bind(size_t idx, uint64_t);
    void bind(size_t idx, double);
    void bind(size_t idx, const std::string&);
    std::string escape(const std::string&);
    std::string expand();
    size_t begin();
    TransactionPtr tranx;
    std::string sql;            // with '?' placeholders
    std::vector<std::pair<size_t/*position*/, std::string/*value*/> > placeholders;
    size_t execution_seq;       // number of times this statement was executed
    size_t row_num;             // high water mark from all existing iterators for this execution
#ifdef ROSE_HAVE_SQLITE3
    sqlite3x::sqlite3_command *sqlite3_cmd;
    sqlite3x::sqlite3_reader *sqlite3_cursor;
#endif
#ifdef ROSE_HAVE_LIBPQXX
    pqxx::result postgres_result;
    pqxx::result::const_iterator postgres_iter;
#endif
};

void
StatementImpl::init()
{
#ifdef ROSE_HAVE_SQLITE3
    sqlite3_cmd = NULL;
    sqlite3_cursor = NULL;
#endif

    for (size_t i=0; i<sql.size(); ++i) {
        if ('?'==sql[i])
            placeholders.push_back(std::make_pair(i, std::string()));
    }
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
StatementImpl::bind_check(size_t idx)
{
    if (idx>=placeholders.size()) {
        std::string mesg = "SQL statement has only " + StringUtility::numberToString(placeholders.size()) +
                           " placeholder" + (1==placeholders.size()?"":"s") + " but needs at least " +
                           StringUtility::numberToString(idx+1);
        throw Exception(mesg);
    }
}

void
StatementImpl::bind(size_t idx, int32_t val)
{
    bind_check(idx);
    placeholders[idx].second = StringUtility::numberToString(val);
}

void
StatementImpl::bind(size_t idx, uint32_t val)
{
    bind_check(idx);
    placeholders[idx].second = StringUtility::numberToString(val);
}

void
StatementImpl::bind(size_t idx, int64_t val)
{
    bind_check(idx);
    placeholders[idx].second = StringUtility::numberToString(val);
}

void
StatementImpl::bind(size_t idx, uint64_t val)
{
    bind_check(idx);
    placeholders[idx].second = StringUtility::numberToString(val);
}

void
StatementImpl::bind(size_t idx, double val)
{
    bind_check(idx);
    placeholders[idx].second = StringUtility::numberToString(val);
}

void
StatementImpl::bind(size_t idx, const std::string &val)
{
    bind_check(idx);
    placeholders[idx].second = "'" + escape(val) + "'";
}

std::string
StatementImpl::escape(const std::string &src)
{
    std::string dst;
    size_t sz = src.size();
    for (size_t i=0; i<sz; ++i) {
        if ('\''==src[i]) {
            dst += "''";
        } else {
            dst += src[i];
        }
    }
    return dst;
}

std::string
StatementImpl::expand()
{
    std::string s;
    size_t sz = sql.size();
    size_t nph = 0;
    for (size_t i=0; i<sz; ++i) {
        if ('?'==sql[i]) {
            s += placeholders[nph].second;
        } else {
            s += sql[i];
        }
    }
    return s;
}

size_t
StatementImpl::begin()
{
    assert(tranx!=NULL);
    assert(!tranx->is_terminated());

    for (size_t i=0; i<placeholders.size(); ++i) {
        if (placeholders[i].second.empty())
            throw Exception("placeholder " + StringUtility::numberToString(i) + " is not bound");
    }

    std::string sql = expand();
    execution_seq += 1;
    row_num = 0;

    switch (driver()) {
#ifdef ROSE_HAVE_SQLITE3
        case SQLITE3: {
            delete sqlite3_cursor;
            sqlite3_cursor = NULL;
            delete sqlite3_cmd;
            sqlite3_cmd = NULL;
            size_t drv_conn_idx = tranx->impl->drv_conn_idx;
            sqlite3x::sqlite3_connection *drv_conn = tranx->impl->conn->impl->sqlite3_connections[drv_conn_idx];
            assert(drv_conn!=NULL);
            sqlite3_cmd = new sqlite3x::sqlite3_command(*drv_conn, sql);
            sqlite3_cursor = new sqlite3x::sqlite3_reader;
            *sqlite3_cursor = sqlite3_cmd->executereader();
            if (!sqlite3_cursor->read()) {
                delete sqlite3_cursor;
                sqlite3_cursor = NULL;
                delete sqlite3_cmd;
                sqlite3_cmd = NULL;
            }
            break;
        }
#endif

#ifdef ROSE_HAVE_LIBPQXX
        case POSTGRESQL: {
            postgres_result = tranx->impl->postgres_tranx->exec(sql);
            postgres_iter = postgres_result.begin();
            break;
        }
#endif

        default:
            assert(!"database driver not supported");
            abort();
    }

    return execution_seq;
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

Statement::iterator
Statement::begin()
{
    size_t execution_seq = impl->begin();
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
    assert(i!=end());
    return i.get<int>(0);
}

void Statement::bind(size_t idx, int32_t val) { impl->bind(idx, val); }
void Statement::bind(size_t idx, int64_t val) { impl->bind(idx, val); }
void Statement::bind(size_t idx, uint32_t val) { impl->bind(idx, val); }
void Statement::bind(size_t idx, uint64_t val) { impl->bind(idx, val); }
void Statement::bind(size_t idx, double val) { impl->bind(idx, val); }
void Statement::bind(size_t idx, const std::string &val) { impl->bind(idx, val); }

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

template<> int32_t Statement::iterator::get<int32_t>(size_t idx) { return get_i32(idx); }
template<> int64_t Statement::iterator::get<int64_t>(size_t idx) { return get_i64(idx); }
template<> uint32_t Statement::iterator::get<uint32_t>(size_t idx) { return get_u32(idx); }
template<> uint64_t Statement::iterator::get<uint64_t>(size_t idx) { return get_u64(idx); }
template<> float Statement::iterator::get<float>(size_t idx) { return get_dbl(idx); }
template<> double Statement::iterator::get<double>(size_t idx) { return get_dbl(idx); }
template<> std::string Statement::iterator::get<std::string>(size_t idx) { return get_str(idx); }

} // namespace
