// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_DatabaseSqlite_H
#define Sawyer_DatabaseSqlite_H

#if __cplusplus >= 201103L

#include <Sawyer/Database.h>

#include <boost/filesystem.hpp>
#include <Sawyer/Optional.h>
#include <sqlite3.h>
#include <string>

namespace Sawyer {
namespace Database {

/** Connection to SQLite database. */
class Sqlite: public Connection {
public:
    /** Construct an SQLite connection not connected to any database. */
    Sqlite() {}

    /** Construct an SQLite connection connected to the specified file.
     *
     *  An existing file is used, or otherwise a new file is created. */
    Sqlite(const boost::filesystem::path &fileName) {
        open(fileName);
    }

    /** Close any previous connection and open a new one. */
    Sqlite& open(const boost::filesystem::path &fileName);
};


// Only implementation details beyond this point -- no public APIs
namespace Detail {

class SqliteStatement;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQLite3 connection details
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SqliteConnection: public ConnectionBase {
    friend class ::Sawyer::Database::Sqlite;
    friend class ::Sawyer::Database::Detail::SqliteStatement;

    sqlite3 *connection = nullptr;

public:
    ~SqliteConnection() {
        close();
    }

private:
    void open(const boost::filesystem::path &filename) {
        close();
        if (strlen(filename.native().c_str()) != filename.native().size())
            throw Exception("invalid database name: internal NUL character");
        int status = sqlite3_open(filename.native().c_str(), &connection);
        if (SQLITE_OK != status)
            throw Exception(sqlite3_errstr(status));
        sqlite3_busy_timeout(connection, 1000 /*ms*/);
    }

    void close() {
        if (connection) {
            // Will return SQLITE_BUSY (a.k.a., "database is locked") if there are outstanding prepared statements. Due to
            // reference counting, this "close" function will only be called when none of those statements are in the EXECUTING
            // state. Note that SQLITE_LOCKED is an entirely different error.
            int status = sqlite3_close(connection);
            connection = nullptr;
            if (SQLITE_OK != status && SQLITE_BUSY != status)
                throw Exception(sqlite3_errstr(status));
        }
    }

    std::string driverName() const override {
        return "sqlite";
    }
    
    Statement prepareStatement(const std::string &sql);

    size_t lastInsert() const {
        ASSERT_not_null(connection);
        return boost::numeric_cast<size_t>(sqlite3_last_insert_rowid(connection));
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQLite3 statement details
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SqliteStatement: public StatementBase {
    friend class ::Sawyer::Database::Detail::SqliteConnection;

    sqlite3_stmt *stmt = nullptr;                       // underlying SQLite3 statement

private:
    SqliteStatement(const std::shared_ptr<ConnectionBase> &db, const std::string &sql)
        : StatementBase(db) {
        std::string lowSql = parseParameters(sql).first;
        const char *rest = nullptr;
        std::shared_ptr<SqliteConnection> sqlite = std::dynamic_pointer_cast<SqliteConnection>(db);
        ASSERT_not_null(sqlite);
        int status = sqlite3_prepare_v2(sqlite->connection, lowSql.c_str(), lowSql.size()+1, &stmt, &rest);
        if (SQLITE_OK != status)
            throw Exception(sqlite3_errstr(status));
        while (rest && ::isspace(*rest))
            ++rest;
        if (rest && *rest) {
            sqlite3_finalize(stmt);                     // clean up if possible; ignore error otherwise
            stmt = nullptr;
            throw Exception("extraneous text after end of SQL statement");
        }
    }

public:
    ~SqliteStatement() {
        if (stmt) {
            sqlite3_finalize(stmt);
            stmt = nullptr;
        }
    }

private:
    void reset(bool doUnbind) override {
        if (!connection())
            throw Exception("connection is closed");
        if (state() == Statement::DEAD)
            throw Exception("statement is dead");

        // Reset the SQL statement with delayed error reporting
        int status = SQLITE_OK;
        if (Statement::EXECUTING == state() || Statement::FINISHED == state())
            status = sqlite3_reset(stmt);               // doesn't actually unbind parameters

        // Do the higher-level part of the reset
        StatementBase::reset(doUnbind);

        // Finally report errors from above
        if (SQLITE_OK != status) {
            state(Statement::DEAD);                     // we no longer know the SQLite3 state
            throw Exception(sqlite3_errstr(status));
        }
    }

    void bindLow(size_t idx, int value) override {
        int status = sqlite3_bind_int(stmt, idx+1, value);
        if (SQLITE_OK != status)
            throw Exception(sqlite3_errstr(status));
    }

    void bindLow(size_t idx, int64_t value) override {
        int status = sqlite3_bind_int64(stmt, idx+1, value);
        if (SQLITE_OK != status)
            throw Exception(sqlite3_errstr(status));
    }

    void bindLow(size_t idx, size_t value) override {
        bindLow(idx, boost::numeric_cast<int64_t>(value));
    }
    
    void bindLow(size_t idx, double value) override {
        int status = sqlite3_bind_double(stmt, idx+1, value);
        if (SQLITE_OK != status)
            throw Exception(sqlite3_errstr(status));
    }

    void bindLow(size_t idx, const std::string &value) override {
        int status = sqlite3_bind_text(stmt, idx+1, value.c_str(), value.size(), SQLITE_TRANSIENT);
        if (SQLITE_OK != status)
            throw Exception(sqlite3_errstr(status));
    }

    void bindLow(size_t idx, const char *value) override {
        value ? bindLow(idx, std::string(value)) : bindLow(idx, Nothing());
    }

    void bindLow(size_t idx, const std::vector<uint8_t> &value) override {
        int status = sqlite3_bind_blob(stmt, idx+1, value.data(), value.size(), SQLITE_TRANSIENT);
        if (SQLITE_OK != status)
            throw Exception(sqlite3_errstr(status));
    }
    
    void bindLow(size_t idx, Nothing) override {
        int status = sqlite3_bind_null(stmt, idx+1);
        if (SQLITE_OK != status)
            throw Exception(sqlite3_errstr(status));
    }

    Iterator beginLow() override {
        return nextLow();
    }

    Iterator nextLow() override {
        int status = sqlite3_step(stmt);
        if (SQLITE_ROW == status) {
            return makeIterator();
        } else if (SQLITE_DONE == status) {
            state(Statement::FINISHED);
            return Iterator();
        } else {
            state(Statement::DEAD);
            throw Exception(sqlite3_errstr(status));
        }
    }

    size_t nColumns() const override {
        return boost::numeric_cast<size_t>(sqlite3_column_count(stmt));
    }

    Sawyer::Optional<std::string> getString(size_t idx) override {
        if (SQLITE_NULL == sqlite3_column_type(stmt, idx))
            return Nothing();
        size_t nBytes = sqlite3_column_bytes(stmt, idx);
        const unsigned char *s = sqlite3_column_text(stmt, idx);
        ASSERT_not_null(s);
        return std::string(s, s+nBytes);
    }

    Sawyer::Optional<std::vector<uint8_t>> getBlob(size_t idx) override {
        if (SQLITE_NULL == sqlite3_column_type(stmt, idx))
            return Nothing();
        size_t nBytes = sqlite3_column_bytes(stmt, idx);
        const uint8_t *data = static_cast<const uint8_t*>(sqlite3_column_blob(stmt, idx));
        ASSERT_not_null(data);
        return std::vector<uint8_t>(data, data+nBytes);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SqliteConnection implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Statement
SqliteConnection::prepareStatement(const std::string &sql) {
    auto detail = std::shared_ptr<SqliteStatement>(new SqliteStatement(shared_from_this(), sql));
    return makeStatement(detail);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Top-level Sqlite connection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Sqlite&
Sqlite::open(const boost::filesystem::path &fileName) {
    auto pimpl = std::shared_ptr<Detail::SqliteConnection>(new Detail::SqliteConnection);
    pimpl->open(fileName);
    this->pimpl(pimpl);
    return *this;
}

} // namespace
} // namespace

#endif
#endif
