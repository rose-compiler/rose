// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_DatabasePostgresql_H
#define Sawyer_DatabasePostgresql_H

#if __cplusplus >= 201103L

#include <Sawyer/Database.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <cctype>
#include <pqxx/pqxx>

namespace Sawyer {
namespace Database {

/** Connection to PostgreSQL database. */
class Postgresql: public Connection {
public:
    /** Connection details. */
    struct Locator {
        std::string hostname;                           /**< Server host name. */
        std::string port;                               /**< Server port name or number. */
        std::string user;                               /**< Remote user name. */
        std::string password;                           /**< Remote user password. */
        std::string database;                           /**< Remote database name. */
    };

    /** Construct a PostgreSQL connection not connected to any database. */
    Postgresql() {}

    /** Construct a PostgreSQL connection to the specified host, username, and database. */
    explicit Postgresql(const Locator &c) {
        open(c);
    }

    Postgresql& open(const Locator &c);
};

// Only implementation details beyond this point -- no public APIs
namespace Detail {

class PostgresqlStatement;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PostgreSQL connection details
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PostgresqlConnection: public ConnectionBase {
    friend class ::Sawyer::Database::Postgresql;
    friend class ::Sawyer::Database::Detail::PostgresqlStatement;

    // pqxx::connection has deleted operator= and not defined swap, so in order to be able to close and re-open a connection
    // we need to throw away the old connection and create a new one. Thus the use of pointers here.
    std::unique_ptr<pqxx::connection> connection;
    std::unique_ptr<pqxx::work> transaction;

public:
    ~PostgresqlConnection() {
        close();
    }

private:
    // See RFC 3986
    std::string uriEscape(const std::string &s) {
        std::string retval;
        for (char ch: s) {
            if (::isalnum(ch) || ::strchr("-_.~", ch)) {
                retval += ch;
            } else {
                retval += (boost::format("%02X") % (unsigned)ch).str();
            }
        }
        return retval;
    }
    
    void open(const Postgresql::Locator &where) {
        close();

        // Create the URI. See https://www.postgresql.org/docs/10/libpq-connect.html section 33.1.1.2
        std::string uri = "postgresql://";
        if (!where.user.empty() || !where.password.empty()) {
            uri += uriEscape(where.user);
            if (!where.password.empty())
                uri += ":" + uriEscape(where.password);
            uri += "@";
        }
        if (!where.hostname.empty())
            uri += uriEscape(where.hostname);
        if (!where.port.empty())
            uri += ":" + uriEscape(where.port);
        if (!where.database.empty())
            uri += "/" + uriEscape(where.database);

        connection = std::unique_ptr<pqxx::connection>(new pqxx::connection(uri));
        transaction = std::unique_ptr<pqxx::work>(new pqxx::work(*connection));
    }

    void close() {
        if (connection && connection->is_open() && transaction)
            transaction->commit();
        transaction.reset();
        connection.reset();
    }

    std::string driverName() const override {
        return "postgresql";
    }
    
    Statement prepareStatement(const std::string &sql);

    size_t lastInsert() const {
        throw Exception("last inserted row ID not supported; suggestion: use UUIDs instead");
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PostgreSQL statement details
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PostgresqlStatement: public StatementBase {
    friend class ::Sawyer::Database::Detail::PostgresqlConnection;

    std::string sql_;                                   // SQL with "?" parameters
    std::vector<std::string> pvalues_;                  // value for each "?" parameter
    pqxx::result result_;                               // result of most recent query

private:
    PostgresqlStatement(const std::shared_ptr<ConnectionBase> &db, const std::string &sql)
        : StatementBase(db) {
        auto low = parseParameters(sql);
        sql_ = low.first;
        pvalues_.resize(low.second, "null");
    }

    void unbindAllParams() override {
        pvalues_.clear();
    }

    void bindLow(size_t idx, int value) override {
        pvalues_[idx] = boost::lexical_cast<std::string>(value);
    }

    void bindLow(size_t idx, int64_t value) override {
        pvalues_[idx] = boost::lexical_cast<std::string>(value);
    }

    void bindLow(size_t idx, size_t value) override {
        pvalues_[idx] = boost::lexical_cast<std::string>(value);
    }

    void bindLow(size_t idx, double value) override {
        pvalues_[idx] = boost::lexical_cast<std::string>(value);
    }

    void bindLow(size_t idx, const std::string &value) override {
        auto tx = std::dynamic_pointer_cast<PostgresqlConnection>(connection())->transaction.get();
        pvalues_[idx] = "'" + tx->esc_raw(reinterpret_cast<const unsigned char*>(value.c_str()), value.size()) + "'";
    }

    void bindLow(size_t idx, const char *value) override {
        auto tx = std::dynamic_pointer_cast<PostgresqlConnection>(connection())->transaction.get();
        pvalues_[idx] = "'" + tx->esc(value) + "'";
    }

    void bindLow(size_t idx, const std::vector<uint8_t> &value) override {
        auto tx = std::dynamic_pointer_cast<PostgresqlConnection>(connection())->transaction.get();
        auto data = static_cast<const unsigned char*>(value.data());
        pvalues_[idx] = "'" + tx->esc_raw(data, value.size()) + "'";
    }

    void bindLow(size_t idx, Nothing) override {
        pvalues_[idx] = "null";
    }

    Iterator beginLow() override {
        // Expand the low-level "?" parameters into a new SQL string.  The values are already escaped and quoted if necessary.
        std::string sql;
        size_t paramIdx = 0;
        bool inQuote = false;
        for (size_t i=0; i<sql_.size(); ++i) {
            if ('\'' == sql_[i]) {
                inQuote = !inQuote;
                sql += sql_[i];
            } else if ('?' == sql_[i] && !inQuote) {
                ASSERT_require(paramIdx < pvalues_.size());
                sql += pvalues_[paramIdx++];
            } else {
                sql += sql_[i];
            }
        }

        auto tx = std::dynamic_pointer_cast<PostgresqlConnection>(connection())->transaction.get();
        result_ = tx->exec(sql);
        if (result_.empty())
            return Iterator();
        return makeIterator();
    }

    Iterator nextLow() override {
        if (rowNumber() >= result_.size()) {
            state(Statement::FINISHED);
            return Iterator();
        } else {
            return makeIterator();
        }
    }

    size_t nColumns() const override {
        return result_.columns();
    }

    Sawyer::Optional<std::string> getString(size_t idx) override {
        ASSERT_require(rowNumber() < result_.size());
        ASSERT_require(idx < result_.columns());
        if (result_[rowNumber()][boost::numeric_cast<int>(idx)].is_null()) {
            return Nothing();
        } else {
            return unescapeRaw(result_[rowNumber()][boost::numeric_cast<int>(idx)].as<std::string>());
        }
    }

    static unsigned hex2int(char ch) {
        if (::isdigit(ch))
            return ch - '0';
        if (ch >= 'a' && ch <= 'f')
            return ch - 'a' + 10;
        ASSERT_require(ch >= 'A' && ch <= 'F');
        return ch - 'A' + 10;
    }
    
    // Because pqxx::transaction_base::unesc_raw is not available yet
    static std::string unescapeRaw(const std::string &s) {
        if (boost::starts_with(s, "\\x") && s.size() % 2 == 0) {
            std::string retval;
            for (size_t i=2; i<s.size(); i+=2)
                retval += static_cast<char>(hex2int(s[i])*16 + hex2int(s[i+1]));
            return retval;
        } else {
            return s;
        }
    }
    
    Sawyer::Optional<std::vector<uint8_t>> getBlob(size_t idx) override {
        ASSERT_require(rowNumber() < result_.size());
        ASSERT_require(idx < result_.columns());
        if (result_[rowNumber()][boost::numeric_cast<int>(idx)].is_null()) {
            return Nothing();
        } else {
            std::string bytes = unescapeRaw(result_[rowNumber()][boost::numeric_cast<int>(idx)].as<std::string>());
            auto data = reinterpret_cast<const uint8_t*>(bytes.c_str());
            return std::vector<uint8_t>(data, data + bytes.size());
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PostgresqlConnection implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Statement
PostgresqlConnection::prepareStatement(const std::string &sql) {
    auto detail = std::shared_ptr<PostgresqlStatement>(new PostgresqlStatement(shared_from_this(), sql));
    return makeStatement(detail);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Top-level Postgresql connection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Postgresql&
Postgresql::open(const Postgresql::Locator &where) {
    auto pimpl = std::shared_ptr<Detail::PostgresqlConnection>(new Detail::PostgresqlConnection);
    pimpl->open(where);
    this->pimpl(pimpl);
    return *this;
}

} // namespace
} // namespace

#endif
#endif
