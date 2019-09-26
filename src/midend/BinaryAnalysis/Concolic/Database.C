#include <sage3basic.h>
#include <rosePublicConfig.h>
#include <BinaryConcolic.h>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>

#ifdef ROSE_HAVE_SQLITE3
// bypass intermediate layers for very large files (RBA)
  #ifndef WITH_DIRECT_SQLITE3
  #define WITH_DIRECT_SQLITE3 1
  #endif
#endif

#if WITH_DIRECT_SQLITE3
#include <sqlite3.h>
#endif /* WITH_DIRECT_SQLITE3 */

#include "io-utility.h"

namespace bt = boost::tuples;


namespace Rose {
namespace BinaryAnalysis {

  typedef SqlDatabase::TransactionPtr      SqlTransactionPtr;
  typedef SqlDatabase::StatementPtr        SqlStatementPtr;
  typedef SqlDatabase::Statement::iterator SqlIterator;
  typedef SqlDatabase::ConnectionPtr       SqlConnectionPtr;

  typedef Concolic::Database::TestSuiteId  TestSuiteId;
  typedef Concolic::Database::SpecimenId   SpecimenId;
  typedef Concolic::Database::TestCaseId   TestCaseId;

  namespace Concolic
  {
    template <class IdTag, class BidirectionalMap>
    static
    typename BidirectionalMap::Forward::Value
    _object(Database&, SqlDatabase::ConnectionPtr, ObjectId<IdTag>, Update::Flag, BidirectionalMap&);

    template <class IdTag, class BidirectionalMap>
    static
    typename BidirectionalMap::Forward::Value
    _object(Database&, SqlTransactionPtr, ObjectId<IdTag>, Update::Flag, BidirectionalMap&);
  }


  namespace
  {
    // Split a database URL into a file name and debug Boolean. This assumes that the URL contains a file name, and that the
    // debug flag is the first option in the URL.
  std::pair<boost::filesystem::path, bool /*isDebugging*/>
    dbProperties(const std::string& url) {
        SqlDatabase::Connection::ParsedUrl parsed = SqlDatabase::Connection::parseUrl(url);
        if (SqlDatabase::SQLITE3 != parsed.driver)
            return std::make_pair(boost::filesystem::path(), false); // we're only handling SQLite3 for now

        bool withDebug = false;
        BOOST_FOREACH (const SqlDatabase::Connection::Parameter &param, parsed.params) {
            if ("debug" == param.first) {
                withDebug = true;
                break;
            }
        }

        return std::make_pair(boost::filesystem::path(parsed.dbName), withDebug);
    }

    static const size_t
    DBSQLITE_FIRST_ROWID = 1;

    // Unfortunately, the current sqlite3x interface is not able to query
    // NULL values. Thus, a negative pseudo value is used to indicate that
    // no concrete test has been performed.
    // \todo upgrade to newer sqlite3x interface that supports NULL values
    //       e.g., https://github.com/ptrv/sqlite3x

    static const double
    NO_CONCRETE_RANK     = -9999;

    static const std::string
    NO_CONCRETE_RANK_STR = boost::lexical_cast<std::string>(NO_CONCRETE_RANK);

    static const std::string
    NO_CONCRETE_RANK_MAX = boost::lexical_cast<std::string>(NO_CONCRETE_RANK+1);

    // type function to add types to the end of a boost::tuple
    //   generic case
    template <class L, class V>
    struct bt_append
    {
      typedef typename L::head_type                       head_type;
      typedef typename L::tail_type                       tail_type;

      typedef typename bt_append<tail_type, V>::type      new_tail_type;
      typedef typename bt::cons<head_type, new_tail_type> type;
    };

    //   base case
    template <class V>
    struct bt_append<bt::null_type, V>
    {
      typedef typename bt::tuple<V>::inherited type;
    };

    template <class L>
    struct bt_remove
    {};

    template <class H, class T>
    struct bt_remove<bt::cons<H, T> >
    {
      typedef bt::cons<H, typename bt_remove<T>::tuple_type> tuple_type;
      typedef typename bt_remove<T>::elem_type               elem_type;
    };

    template <class H>
    struct bt_remove<bt::cons<H, bt::null_type> >
    {
      typedef bt::null_type tuple_type;
      typedef H             elem_type;
    };

    //
    // Type-safe SQL queries
    //
    // Queries check for type agreements in their placeholder and bindings

    // null value
    typedef void* SqlNullType;

    // SQL placeholder types
    template <class V>
    struct SqlVar {};

    typedef SqlVar<int>                   SqlInt;
    typedef SqlVar<std::string>           SqlString;
    typedef SqlVar<std::vector<uint8_t> > SqlBlob;
    typedef SqlVar<double>                SqlReal;
    typedef SqlVar<bool>                  SqlBool;


    struct SqlQueryBase
    {
      explicit
      SqlQueryBase(const std::string& s)
      : sql(s)
      {}

      std::string sql;
    };

    std::ostream& operator<<(std::ostream& os, const SqlQueryBase& sql)
    {
      return os << sql.sql;
    }


    // a query holding a string containing |L| SQL placeholders
    //   note, instead of directly using tuple<X,Y,Z>
    //         its representation is used cons<X, cons<Y, cons<Z, null_type> > >.
    //         the base can be obtained using tuple<X,Y,Z>::inherited .
    template <class L = bt::null_type >
    struct SqlQuery : SqlQueryBase
    {
      typedef typename bt_remove<L>::tuple_type ParentTuple;
      typedef typename bt_remove<L>::elem_type  VarType;

      SqlQuery(const SqlQuery< ParentTuple >& p, const SqlVar< VarType >&)
      : SqlQueryBase(p.sql + '?')
      {}

      SqlQuery(const SqlQuery<L>& p, const std::string& cont )
      : SqlQueryBase(p.sql + cont)
      {}

      SqlQuery(const SqlQuery<L>& p, const char* cont)
      : SqlQueryBase(p.sql + std::string(cont))
      {}
    };

    // a query w/o SQL placeholders
    template <>
    struct SqlQuery<bt::null_type> : SqlQueryBase
    {
      SqlQuery(const std::string& query)
      : SqlQueryBase(query)
      {}

      SqlQuery(const char* query)
      : SqlQueryBase(query)
      {}
    };


    // a prepared query with |L| unbound SQL placeholders
    template <class L>
    struct SqlQueryPrepared
    {
      typedef SqlQueryPrepared< typename L::tail_type > SqlQueryParentType;

      explicit
      SqlQueryPrepared(const SqlStatementPtr& stmt)
      : sql(stmt)
      {}

      SqlQueryParentType
      tail() const { return SqlQueryParentType(sql); }

      SqlStatementPtr sql;
    };

    // a prepared query w/o any unbound SQL placeholders
    template <>
    struct SqlQueryPrepared<bt::null_type>
    {
      explicit
      SqlQueryPrepared(const SqlStatementPtr& stmt)
      : sql(stmt)
      {}

      SqlStatementPtr sql;
    };


    // concatenates a query of type L with a placeholder V
    // the resulting query has type L + V
    template <class L, class V>
    SqlQuery<typename bt_append<L, V>::type>
    operator+(const SqlQuery<L>& lhs, const SqlVar<V>& rhs)
    {
      typedef SqlQuery<typename bt_append<L, V>::type > ResultType;

      return ResultType(lhs, rhs);
    }

    // concatenates a query of with a string
    //   does not change the queries type
    template <class L>
    SqlQuery<L>
    operator+(const SqlQuery<L>& lhs, const char* rhs)
    {
      return SqlQuery<L>(lhs, rhs);
    }

    // concatenates a query of with a string
    //   does not change the queries type
    template <class L>
    SqlQuery<L>
    operator+(const SqlQuery<L>& lhs, const std::string& rhs)
    {
      return SqlQuery<L>(lhs, rhs);
    }

    // concatenates a string with a SQL variable of type V.
    // the result type is an SqlQuery object with a single placeholder.
    template <class V>
    SqlQuery<typename bt::tuple<V>::inherited>
    operator+(const std::string& lhs, const SqlVar<V>& rhs)
    {
      typedef SqlQuery<typename bt::tuple<V>::inherited> ResultType;

      return ResultType(SqlQuery<>(lhs), rhs);
    }

    // concatenates a string with a SQL variable of type V.
    // the result type is an SqlQuery object with a single placeholder.
    template <class V>
    SqlQuery<typename bt::tuple<V>::inherited>
    operator+(const char* lhs, const SqlVar<V>& rhs)
    {
      typedef SqlQuery<typename bt::tuple<V>::inherited> ResultType;

      return ResultType(SqlQuery<>(lhs), rhs);
    }

    // binds NULL to an SQL placeholder at position @ref pos
    template <class L>
    static inline
    void sqlBindVal(SqlQueryPrepared<L>& stmt, int pos, const SqlNullType&)
    {
      stmt.sql->bind_null(pos);
    }

    // binds a value to an SQL placeholder at position @ref pos
    template <class L>
    static inline
    void sqlBindVal(SqlQueryPrepared<L>& stmt, int pos, const typename L::head_type& val)
    {
      stmt.sql->bind(pos, val);
    }

    // binds optional values
    template <class L>
    static inline
    void sqlBindVal(SqlQueryPrepared<L>& stmt, int pos, const Sawyer::Optional<typename L::head_type>& val)
    {
      if (val)
      {
        sqlBindVal(stmt, pos, val.get());
        return;
      }

      sqlBindVal(stmt, pos, SqlNullType());
    }

    // base case --> all palceholders are bound
    void sqlBind(SqlQueryPrepared<bt::null_type>, const bt::null_type&, int = 0) {}

    // recursively binds all placeholders to values (or NULLs)
    template<class L, class H, class T>
    void sqlBind(SqlQueryPrepared<L> stmt, const bt::cons<H,T>& args, int pos = 0)
    {
      sqlBindVal(stmt, pos, args.get_head());

      sqlBind(stmt.tail(), args.get_tail(), pos+1);
    }

    // convenience function to bind all placeholders and return a prepared statement
    template <class BoostTypeList, class BoostTypeTuple>
    SqlStatementPtr
    sqlPrepareTuple(SqlQueryPrepared<BoostTypeList> stmt, const BoostTypeTuple& args)
    {
      sqlBind(stmt, args);

      return stmt.sql;
    }

    // function to create and return a statement and bind all its placeholders to values
    //   in @ref args
    template <class BoostTypeList, class BoostTypeTuple>
    SqlStatementPtr
    sqlPrepareTuple(SqlTransactionPtr tx, const SqlQuery<BoostTypeList>& stmt, const BoostTypeTuple& args)
    {
      return sqlPrepareTuple(SqlQueryPrepared<BoostTypeList>(tx->statement(stmt.sql)), args);
    }

    // prepares a statement binds N variables to its paceholders.
    // @{
    SqlStatementPtr
    sqlPrepare(SqlTransactionPtr tx, const SqlQuery<>& stmt)
    {
      return sqlPrepareTuple(SqlQueryPrepared<bt::null_type>(tx->statement(stmt.sql)), bt::make_tuple());
    }

    template <class BoostTypeList, class V>
    SqlStatementPtr
    sqlPrepare(SqlTransactionPtr tx, const SqlQuery<BoostTypeList>& stmt, const V& arg1)
    {
      return sqlPrepareTuple(SqlQueryPrepared<BoostTypeList>(tx->statement(stmt.sql)), bt::make_tuple(arg1));
    }

    template <class BoostTypeList, class V1, class V2>
    SqlStatementPtr
    sqlPrepare(SqlTransactionPtr tx, const SqlQuery<BoostTypeList>& stmt, const V1& arg1, const V2& arg2)
    {
      return sqlPrepareTuple(SqlQueryPrepared<BoostTypeList>(tx->statement(stmt.sql)), bt::make_tuple(arg1, arg2));
    }

    template <class BoostTypeList, class V1, class V2, class V3>
    SqlStatementPtr
    sqlPrepare(SqlTransactionPtr tx, const SqlQuery<BoostTypeList>& stmt, const V1& arg1, const V2& arg2, const V3& arg3)
    {
      return sqlPrepareTuple(SqlQueryPrepared<BoostTypeList>(tx->statement(stmt.sql)), bt::make_tuple(arg1, arg2, arg3));
    }

    template <class BoostTypeList, class V1, class V2, class V3, class V4>
    SqlStatementPtr
    sqlPrepare( SqlTransactionPtr tx,
                const SqlQuery<BoostTypeList>& stmt,
                const V1& arg1, const V2& arg2, const V3& arg3, const V4& arg4
              )
    {
      return sqlPrepareTuple( SqlQueryPrepared<BoostTypeList>(tx->statement(stmt.sql)),
                              bt::make_tuple(arg1, arg2, arg3, arg4)
                            );
    }

    template <class BoostTypeList, class V1, class V2, class V3, class V4, class V5>
    SqlStatementPtr
    sqlPrepare( SqlTransactionPtr tx,
                const SqlQuery<BoostTypeList>& stmt,
                const V1& arg1, const V2& arg2, const V3& arg3, const V4& arg4, const V5& arg5
              )
    {
      return sqlPrepareTuple( SqlQueryPrepared<BoostTypeList>(tx->statement(stmt.sql)),
                              bt::make_tuple(arg1, arg2, arg3, arg4, arg5)
                            );
    }

    template <class BoostTypeList, class V1, class V2, class V3, class V4, class V5, class V6>
    SqlStatementPtr
    sqlPrepare( SqlTransactionPtr tx,
                const SqlQuery<BoostTypeList>& stmt,
                const V1& arg1, const V2& arg2, const V3& arg3, const V4& arg4, const V5& arg5, const V6& arg6
              )
    {
      return sqlPrepareTuple( SqlQueryPrepared<BoostTypeList>(tx->statement(stmt.sql)),
                              bt::make_tuple(arg1, arg2, arg3, arg4, arg5, arg6)
                            );
    }

    // @}


    //
    // SQL Queries
    //

    // table makers

    static const
    SqlQuery<>
    QY_MK_TESTSUITES     = "CREATE TABLE \"TestSuites\" ("
                           "  \"id\" int PRIMARY KEY,"
                           "  \"name\" varchar(256) UNIQUE NOT NULL"
                           ");";

    static const
    SqlQuery<>
    QY_MK_SPECIMENS       = "CREATE TABLE \"Specimens\" ("
                            "  \"id\" int PRIMARY KEY,"
                            "  \"name\" varchar(256) NOT NULL,"
                            "  \"binary\" Blob NOT NULL"
                            ");";

    static const
    SqlQuery<>
    QY_MK_TESTCASES       = "CREATE TABLE \"TestCases\" ("
                            "  \"id\" int PRIMARY KEY,"
                            "  \"specimen_id\" int NOT NULL,"
                            "  \"testsuite_id\" int,"
                            "  \"name\" varchar(128),"
                            "  \"executor\" varchar(128) CHECK(executor = \"linux\"),"
                            "  \"concrete_rank\" float,"
                            "  \"concolic_result\" int,"
                            //~ " CONSTRAINT \"uq_specimen_name\" UNIQUE (\"specimen_id\", \"name\"),"
                            " CONSTRAINT \"fk_specimen_testcase\" FOREIGN KEY (\"specimen_id\") REFERENCES \"Specimens\" (\"id\")"
                            ");";

    static const
    SqlQuery<>
    QY_MK_CMDLINEARGS     = "CREATE TABLE \"CmdLineArgs\" ("
                            "  \"id\" int PRIMARY KEY,"
                            "  \"arg\" varchar(128) UNIQUE NOT NULL"
                            ");";

    static const
    SqlQuery<>
    QY_MK_ENVVARS         = "CREATE TABLE \"EnvironmentVariables\" ("
                            "  \"id\" int PRIMARY KEY,"
                            "  \"name\" varchar(128) NOT NULL,"
                            "  \"value\" varchar(256) NOT NULL,"
                            " CONSTRAINT \"uq_name_value\" UNIQUE (\"name\", \"value\")"
                            ");";

    static const
    SqlQuery<>
    QY_MK_CONCRETE_RES   = "CREATE TABLE \"ConcreteResults\" ("
                           "  \"testcase_id\" int UNIQUE NOT NULL,"
                           "  \"result\" Text,"
                           " CONSTRAINT \"fk_testcase_results\" FOREIGN KEY (\"testcase_id\") REFERENCES \"TestCases\" (\"id\")"
                           ");";

    static const
    SqlQuery<>
    QY_MK_TESTCASE_ARGS   = "CREATE TABLE \"TestCaseArguments\" ("
                            "  \"testcase_id\" int NOT NULL,"
                            "  \"seqnum\" int NOT NULL,"
                            "  \"cmdlinearg_id\" int NOT NULL,"
                            " CONSTRAINT \"fk_testcase_argument\" FOREIGN KEY (\"testcase_id\") REFERENCES \"TestCases\" (\"id\"),"
                            " CONSTRAINT \"fk_cmdlineargs_argument\" FOREIGN KEY (\"cmdlinearg_id\") REFERENCES \"CmdLineArgs\" (\"id\"),"
                            " CONSTRAINT \"pk_argument\" PRIMARY KEY (\"testcase_id\", \"seqnum\")"
                            ");";

    static const
    SqlQuery<>
    QY_MK_TESTCASE_EVAR   = "CREATE TABLE \"TestCaseVariables\" ("
                            "  \"testcase_id\" int NOT NULL,"
                            "  \"envvar_id\" int NOT NULL,"
                            " CONSTRAINT \"fk_testcase_testcasevar\" FOREIGN KEY (\"testcase_id\") REFERENCES \"TestCases\" (\"id\"),"
                            " CONSTRAINT \"fk_envvar_testcasevar\" FOREIGN KEY (\"envvar_id\") REFERENCES \"EnvironmentVariables\" (\"id\"),"
                            " CONSTRAINT \"pk_testcasevar\" PRIMARY KEY (\"testcase_id\", \"envvar_id\")"
                            ");";

    static const
    SqlQuery<>
    QY_MK_RBA_FILES       = "CREATE TABLE \"RBAFiles\" ("
                            "  \"id\" int PRIMARY KEY,"
                            "  \"specimen_id\" int UNIQUE NOT NULL,"
                            "  \"data\" Blob NOT NULL,"
                            " CONSTRAINT \"fk_specimen_rba\" FOREIGN KEY (\"specimen_id\") REFERENCES \"Specimens\" (\"id\")"
                            ");";

    //
    // db queries

    static const
    SqlQuery<>
    QY_DB_INITIALIZED     = "SELECT count(*) FROM TestSuites;";

    static const
    SqlQuery<>
    QY_ALL_TESTSUITES     = "SELECT rowid FROM TestSuites ORDER BY rowid;";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_TESTSUITE          = "SELECT name FROM TestSuites WHERE rowid = " + SqlInt() + ";";


    static const
    SqlQuery<bt::tuple<std::string>::inherited >
    QY_TESTSUITE_BY_NAME  = "SELECT rowid FROM TestSuites WHERE name = " + SqlString() + ";";

    static const
    SqlQuery<>
    QY_ALL_SPECIMENS      = "SELECT rowid FROM Specimens ORDER BY rowid;";

    static const
    SqlQuery<bt::tuple<int>::inherited >
    QY_SPECIMENS_IN_SUITE = "SELECT DISTINCT specimen_id"
                            "  FROM TestCases"
                            " WHERE testsuite_id = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_SPECIMEN           = "SELECT name, binary FROM Specimens "
                            " WHERE rowid = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<std::string>::inherited>
    QY_SPECIMEN_BY_NAME   = "SELECT rowid FROM Specimens "
                            " WHERE name = " + SqlString() + ";";

    static const
    SqlQuery<bt::tuple<int, std::string>::inherited>
    QY_SPECIMEN_IN_SUITE_BY_NAME
                          = "SELECT sp.rowid"
                            "  FROM Specimens sp, TestCases tc"
                            " WHERE sp.rowid = tc.specimen_id"
                            "   AND tc.testsuite_id = " + SqlInt() +
                            "   AND sp.name = " + SqlString() + ";";

    static const
    SqlQuery<>
    QY_ALL_TESTCASES      = "SELECT rowid FROM TestCases ORDER BY rowid;";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_TESTCASES_IN_SUITE = "SELECT rowid"
                             "  FROM TestCases"
                             " WHERE testsuite_id = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int, int>::inherited>
    QY_NEED_CONCOLIC      = "SELECT rowid"
                            "  FROM TestCases"
                            " WHERE concolic_result = 0"
                            "   AND testsuite_id = " + SqlInt() +
                            " LIMIT " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_ALL_NEED_CONCOLIC  = "SELECT rowid"
                            "  FROM TestCases"
                            " WHERE concolic_result = 0"
                            " LIMIT " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int, int>::inherited>
    QY_NEED_CONCRETE      = "SELECT rowid"
                            "  FROM TestCases"
                            " WHERE testsuite_id = " + SqlInt() +
                            "   AND concrete_rank < " + NO_CONCRETE_RANK_MAX +
                            " ORDER BY concrete_rank ASC"
                            " LIMIT " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_ALL_NEED_CONCRETE  = "SELECT rowid"
                            "  FROM TestCases"
                            " WHERE concrete_rank < " + NO_CONCRETE_RANK_MAX +
                            " ORDER BY concrete_rank ASC"
                            " LIMIT " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_TESTCASE           = "SELECT name, executor, concrete_rank, concolic_result"
                            "  FROM TestCases"
                            " WHERE rowid = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_TESTCASE_ARGS      = "SELECT c.arg"
                            "  FROM CmdLineArgs c, TestCaseArguments t"
                            " WHERE c.rowid = t.cmdlinearg_id"
                            "   AND t.testcase_id = " + SqlInt() +
                            " ORDER BY seqnum;";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_TESTCASE_ENV       = "SELECT e.name, e.value"
                            "  FROM EnvironmentVariables e, TestCaseVariables t"
                            " WHERE e.rowid = t.envvar_id"
                            "   AND t.testcase_id = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_TESTCASE_SPECIMEN  = "SELECT specimen_id"
                            "  FROM TestCases"
                            " WHERE rowid = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<std::string>::inherited>
    QY_NEW_TESTSUITE      = "INSERT INTO TestSuites"
                            "  (name)"
                            "  VALUES(" + SqlString() + ");";

    static const
    SqlQuery<bt::tuple<std::string, int>::inherited>
    QY_UPD_TESTSUITE      = "UPDATE TestSuites"
                            "   SET name = "  + SqlString() +
                            " WHERE rowid = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<std::string, std::vector<uint8_t> >::inherited>
    QY_NEW_SPECIMEN      = "INSERT INTO Specimens"
                           "  (name, binary)"
                           "  VALUES(" + SqlString() + "," + SqlBlob() + ");";

    static const
    SqlQuery<bt::tuple<std::string, std::vector<uint8_t>, int>::inherited>
    QY_UPD_SPECIMEN      = "UPDATE Specimens"
                           "   SET name = " + SqlString() + ", binary = " + SqlBlob() +
                           " WHERE rowid = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int, std::string, std::string>::inherited>
    QY_NEW_TESTCASE      = "INSERT INTO TestCases"
                           "  (specimen_id, name, executor, concrete_rank, concolic_result)"
                           "  VALUES(" + SqlInt() + "," + SqlString() + "," + SqlString()
                                       + ", " + NO_CONCRETE_RANK_STR + ", 0"
                                    ");";

    static const
    SqlQuery<bt::tuple<int, std::string, std::string, double, bool, int>::inherited>
    QY_UPD_TESTCASE      = "UPDATE TestCases"
                           "   SET specimen_id = " + SqlInt() + ", name = " + SqlString() + ","
                           "       executor = " + SqlString() + ", concrete_rank = " + SqlReal() + ","
                           "       concolic_result = " + SqlBool() +
                           " WHERE rowid = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_RM_TESTCASE_EVAR   = "DELETE FROM TestCaseVariables"
                            " WHERE testcase_id = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int, int>::inherited>
    QY_NEW_TESTCASE_EVAR = "INSERT INTO TestCaseVariables"
                           "  (testcase_id, envvar_id)"
                           "  VALUES(" + SqlInt() + "," + SqlInt() + ");";

    static const
    SqlQuery<bt::tuple<std::string, std::string>::inherited>
    QY_ENVVAR_ID         = "SELECT rowid"
                           "  FROM EnvironmentVariables"
                           " WHERE name = " + SqlString() + " AND value = " + SqlString() + ";";

    static const
    SqlQuery<bt::tuple<std::string, std::string>::inherited>
    QY_NEW_ENVVAR        = "INSERT INTO EnvironmentVariables"
                           "  (name, value)"
                           "  VALUES(" + SqlString() + "," + SqlString() + ");";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_RM_TESTCASE_CARG   = "DELETE FROM TestCaseArguments"
                            " WHERE testcase_id = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int, int, int>::inherited>
    QY_NEW_TESTCASE_CARG = "INSERT INTO TestCaseArguments"
                           "  (testcase_id, seqnum, cmdlinearg_id)"
                           "  VALUES(" + SqlInt() + "," + SqlInt() + "," + SqlInt() + ");";

    static const
    SqlQuery<bt::tuple<int, int>::inherited>
    QY_NEW_TESTSUITE_TESTCASE = "UPDATE TestCases"
                                "   SET testsuite_id = " + SqlInt() +
                                " WHERE rowid = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<std::string>::inherited>
    QY_CMDLINEARG_ID     = "SELECT rowid"
                           "  FROM CmdLineArgs"
                           " WHERE arg = " + SqlString() + ";";

    static const
    SqlQuery<bt::tuple<std::string>::inherited>
    QY_NEW_CMDLINEARG    = "INSERT INTO CmdLineArgs"
                           "  (arg)"
                           "  VALUES(" + SqlString() + ");";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_NUM_RBAFILES      = "SELECT count(*) FROM RBAFiles"
                           "  WHERE specimen_id = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int, std::vector<uint8_t> >::inherited>
    QY_NEW_RBAFILE       = "INSERT INTO RBAFiles"
                           "  (specimen_id, data)"
                           "  VALUES(" + SqlInt() + "," + SqlBlob() + ");";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_RBAFILE           = "SELECT data FROM RBAFiles"
                           "  WHERE specimen_id = " + SqlInt() + ";";

    static const
    SqlQuery<bt::tuple<int>::inherited>
    QY_RM_RBAFILE        = "DELETE FROM RBAFiles"
                           "  WHERE specimen_id = " + SqlInt() + ";";


    static const
    SqlQuery<bt::tuple<int, std::string>::inherited>
    QY_NEW_CONCRETE_RES  = "INSERT INTO ConcreteResults"
                           "  (testcase_id, result)"
                           "  VALUES(" + SqlInt() + "," + SqlString() + ");";

    static const
    SqlQuery<>
    QY_LAST_ROW_SQLITE3  = "SELECT last_insert_rowid();";


    // returns the query associated with an object type
    // @{

    static
    const SqlQuery<bt::tuple<int>::inherited >&
    objQueryString(const TestSuiteId&) { return QY_TESTSUITE; }

    static
    const SqlQuery<bt::tuple<int>::inherited >&
    objQueryString(const TestCaseId&)  { return QY_TESTCASE; }

    static
    const SqlQuery<bt::tuple<int>::inherited >&
    objQueryString(const SpecimenId&)  { return QY_SPECIMEN; }

    // @}

    // executes a bound query and sets the obj's data
    // @{
    void populateObjFromQuery(SqlIterator& it, Concolic::TestSuite& obj)
    {
      obj.name(it.get_str(0));
    }

    void populateObjFromQuery(SqlIterator& it, Concolic::Specimen& obj)
    {
      obj.name(it.get_str(0));
      obj.content(it.get_blob(1));
    }

    void populateObjFromQuery(SqlIterator& it, Concolic::TestCase& obj)
    {
      typedef Sawyer::Optional<double> Double_opt;

      obj.name(it.get_str(0));
      /* std::string exec = */ it.get_str(1); // executor

      // \todo sqlite3x does currently not support NULL tests
      const double concreteTest = it.get_dbl(2);
      Double_opt   concreteTest_opt = ( concreteTest > (NO_CONCRETE_RANK+1)
                                          ? Double_opt(concreteTest)
                                          : Double_opt()
                                                  );
      obj.concreteRank(concreteTest_opt);
      obj.concolicTest(it.get_i32(3));
    }
    // @}

    // family of functions that query dependent objects.
    //   (e.g., lists of associated objects)

    void dependentObjQuery(Concolic::Database&, SqlTransactionPtr&, TestSuiteId, Concolic::TestSuite&)
    {
      // no dependent objects for test suites
    }

    void dependentObjQuery(Concolic::Database&, SqlTransactionPtr&, SpecimenId, Concolic::Specimen&)
    {
      // no dependent objects for specimen
    }

    // converts a row to a string object
    std::string
    retrieve_row(SqlIterator& iter, const std::string&) { return iter.get_str(0); }

    // converts a row to a name value pair (pair of strings)
    std::pair<std::string, std::string>
    retrieve_row(SqlIterator& iter, const std::pair<std::string, std::string>&)
    {
      return std::make_pair(iter.get_str(0), iter.get_str(1));
    }

    // converts a row to an id
    template <class Tag>
    Concolic::ObjectId<Tag>
    retrieve_row(SqlIterator& iter, const Concolic::ObjectId<Tag>&)
    {
      return Concolic::ObjectId<Tag>(iter.get_i32(0));
    }

    // iterator over result set that converts a single row into a single object
    //   using the retrieve_row function family.
    template <class T>
    struct TransformingIterator : std::iterator<std::input_iterator_tag, T, void, void, void>
    {
        explicit
        TransformingIterator(SqlIterator it)
        : iter(it)
        {}

        TransformingIterator& operator++() { ++iter; return *this; }

        TransformingIterator operator++(int)
        {
          TransformingIterator tmp(iter);

          ++iter;
          return tmp;
        }

        bool operator==(const TransformingIterator& other) const
        {
          return this->iter == other.iter;
        }

        bool operator!=(const TransformingIterator& other) const
        {
          return this->iter != other.iter;
        }

        T operator*()
        {
          return retrieve_row(iter, T());
        }

      private:
        SqlIterator iter;
    };

    // queries a sequence of dependent objects of type T
    //   by executing query @ref sql.
    template <class T>
    std::vector<T>
    depObjQuery(SqlTransactionPtr& tx, const SqlQuery< bt::tuple<int>::inherited >& sql, TestCaseId id)
    {
      typedef TransformingIterator<T> ResultIterator;

      SqlStatementPtr stmt = sqlPrepare(tx, sql, id.get());
      ResultIterator  start(stmt->begin());
      ResultIterator  limit(stmt->end());
      std::vector<T>  res;

      std::copy(start, limit, std::back_inserter(res));
      return res;
    }

    // queries dependencies of a TestCase object
    void dependentObjQuery( Concolic::Database& db,
                            SqlTransactionPtr& tx,
                            TestCaseId id,
                            Concolic::TestCase& tmp
                          )
    {
      typedef Concolic::EnvValue             EnvValue;

      // read command line arguments and environment settings
      tmp.args(depObjQuery<std::string>(tx, QY_TESTCASE_ARGS, id));
      tmp.env (depObjQuery<EnvValue>   (tx, QY_TESTCASE_ENV,  id));

      // read specimen
      {
        SqlStatementPtr         stmt = sqlPrepare(tx, QY_TESTCASE_SPECIMEN, id.get());
        SpecimenId              specimenid(stmt->execute_int());
        Concolic::Specimen::Ptr specimen = db.object_ns(tx, specimenid);

        assert(specimen.getRawPointer());
        tmp.specimen(specimen);
      }
    }
  }

namespace Concolic {

//! Responsible for transaction handling
//! It is the user's responsibility to explicitly commit any DB updates,
//!   otherwise the TX is rolled back!.
struct DBTxGuard
{
    // creates a new database transaction
    explicit
    DBTxGuard(SqlDatabase::ConnectionPtr conn)
    : tx_(conn->transaction())
    {}

    // in case there was no explicit commit the tx is rolled back
    // ( alternatively, the destructor could auto commit/rollback
    //   depending on exception handling context. )
    ~DBTxGuard()
    {
      if (tx_)
      {
        tx_->rollback();

        if (!std::uncaught_exception())
        {
          //~ Sawyer::Message::mlog[Sawyer::Message::INFO]
          std::cerr << "ConcolicDB: TX uncommitted -> rollback" << std::endl;
        }
      }
    }

    SqlTransactionPtr tx() { return tx_; }

    void commit() { tx_->commit(); tx_.reset(); }

    SqlDatabase::Transaction& operator->() { return *tx_; }
    SqlDatabase::Transaction& operator*()  { return *tx_; }

  private:
    SqlTransactionPtr tx_;
};


// queries a vector of object IDs using @args to constrain the query.
template <class IdTag, class BoostTypeList, class BoostTypeTuple>
std::vector<Concolic::ObjectId<IdTag> >
queryIds( SqlDatabase::ConnectionPtr dbconn,
          const SqlQuery<BoostTypeList>& sql,
          const BoostTypeTuple& args
        )
{
  typedef Concolic::ObjectId<IdTag>     IdClass;
  typedef std::vector<IdClass>          ResultVec;
  typedef TransformingIterator<IdClass> ResultIterator;

  DBTxGuard            dbtx(dbconn);
  SqlStatementPtr      stmt = sqlPrepareTuple(dbtx.tx(), sql, args);
  ResultIterator       start(stmt->begin());
  const ResultIterator limit(stmt->end());
  ResultVec            result;

  std::copy(start, limit, std::back_inserter(result));

  dbtx.commit();
  return result;
}

// workaround for lack of function default template arguments in C++03.
template <class IdTag, class BoostTypeList>
std::vector<Concolic::ObjectId<IdTag> >
queryIds( SqlDatabase::ConnectionPtr dbconn, const SqlQuery<BoostTypeList>& sql)
{
  return queryIds<IdTag>(dbconn, sql, bt::null_type());
}


// queries all objects associated with test-suite @ref id (@ref restricted)
// if @ref id is not set, all objects are queried (@ref full)
template <class IdTag, class BoostTypeList2>
std::vector<Concolic::ObjectId<IdTag> >
queryIds( SqlDatabase::ConnectionPtr dbconn,
          TestSuiteId id,
          const SqlQuery<>& full,
          const SqlQuery<BoostTypeList2>& restricted
        )
{
  if (!id) return queryIds<IdTag>(dbconn, full);

  return queryIds<IdTag>(dbconn, restricted, bt::make_tuple(id.get()));
}

// in addition to the preceding function, this adds a limit to the
//   maximum number of rows returned
template <class IdTag, class BoostTypeList1, class BoostTypeList2>
std::vector<Concolic::ObjectId<IdTag> >
queryIds( SqlDatabase::ConnectionPtr dbconn,
          TestSuiteId id,
          const SqlQuery<BoostTypeList1>& full,
          const SqlQuery<BoostTypeList2>& restricted,
          int limit
        )
{
  if (!id)
  {
    return queryIds<IdTag>(dbconn, full, bt::make_tuple(limit));
  }

  return queryIds<IdTag>(dbconn, restricted, bt::make_tuple(id.get(), limit));
}


std::vector<TestSuiteId>
Database::testSuites()
{
  return queryIds<TestSuite>(dbconn_, QY_ALL_TESTSUITES);
}

TestSuite::Ptr
Database::testSuite()
{
  // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  if (!testSuiteId_) return TestSuite::Ptr();

  return testSuites_.forward()[testSuiteId_];
}


Database::TestSuiteId
Database::testSuite(const TestSuite::Ptr& obj)
{
  TestSuiteId res;

  if (obj) res = id(obj);

  // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
  testSuiteId_ = res;
  return testSuiteId_;
}


template <class IdClass>
IdClass getId(SqlIterator pos, SqlIterator end)
{
  if (pos == end) return IdClass();

  return IdClass(pos.get_i32(0));
}


template <class IdClass>
IdClass
queryIdByName( SqlDatabase::ConnectionPtr& dbconn,
               const SqlQuery<bt::tuple<std::string>::inherited>& sqlstmt,
               const std::string& name
             )
{
  DBTxGuard       dbtx(dbconn);
  SqlStatementPtr stmt = sqlPrepare(dbtx.tx(), sqlstmt, name);
  IdClass         id = getId<IdClass>(stmt->begin(), stmt->end());

  dbtx.commit();
  return id;
}

TestSuite::Ptr
Database::findTestSuite(const std::string &nameOrId) {
    TestSuiteId id = queryIdByName<TestSuiteId>(dbconn_, QY_TESTSUITE_BY_NAME, nameOrId);
    if (!id) {
        try {
            id = TestSuiteId(nameOrId);
        } catch (...) {
        }
    }
    return id ? object(id) : TestSuite::Ptr();
}

// specimens

std::vector<SpecimenId>
Database::specimens()
{
  return queryIds<Specimen>( dbconn_,
                             testSuiteId_,
                             QY_ALL_SPECIMENS,     /* full */
                             QY_SPECIMENS_IN_SUITE /* restricted */
                           );
}


std::vector<SpecimenId>
Database::findSpecimensByName(const std::string& name)
{
  if (!testSuiteId_)
    return queryIds<Specimen>(dbconn_, QY_SPECIMEN_BY_NAME, bt::make_tuple(name));

  return queryIds<Specimen>(dbconn_, QY_SPECIMEN_IN_SUITE_BY_NAME, bt::make_tuple(testSuiteId_.get(), name));
}


//~ std::vector<Rose::BinaryAnalysis::Concolic::ObjectId<Rose::BinaryAnalysis::Concolic::ObjectId<Rose::BinaryAnalysis::Concolic::Specimen> >, std::allocator<Rose::BinaryAnalysis::Concolic::ObjectId<Rose::BinaryAnalysis::Concolic::ObjectId<Rose::BinaryAnalysis::Concolic::Specimen> > > >
//~ ' to '
//~ std::vector<Rose::BinaryAnalysis::Concolic::ObjectId<Rose::BinaryAnalysis::Concolic::Specimen> >'

//
// test cases

std::vector<TestCaseId>
Database::testCases()
{
  return queryIds<TestCase>( dbconn_,
                             testSuiteId_,
                             QY_ALL_TESTCASES,     /* full */
                             QY_TESTCASES_IN_SUITE /* restricted */
                           );
}

//
// start up functionality to query state and initialize DB

bool isDbInitialized(SqlDatabase::ConnectionPtr dbconn)
{
  int res = -1;

  try
  {
    DBTxGuard       dbtx(dbconn);

    res  = sqlPrepare(dbtx.tx(), QY_DB_INITIALIZED)
             ->execute_int();

    dbtx.commit();
  }
  catch (SqlDatabase::Exception& ex)
  {
  }

  return res >= 0;
}


static
void initializeDB(SqlTransactionPtr tx)
{
  sqlPrepare(tx, QY_MK_TESTSUITES)->execute();
  sqlPrepare(tx, QY_MK_SPECIMENS)->execute();
  sqlPrepare(tx, QY_MK_TESTCASES)->execute();
  sqlPrepare(tx, QY_MK_CMDLINEARGS)->execute();
  sqlPrepare(tx, QY_MK_ENVVARS)->execute();
  sqlPrepare(tx, QY_MK_TESTCASE_ARGS)->execute();
  sqlPrepare(tx, QY_MK_TESTCASE_EVAR)->execute();
  //~ sqlPrepare(tx, QY_MK_TESTSUITE_TESTCASE)->execute();
  sqlPrepare(tx, QY_MK_RBA_FILES)->execute();
  sqlPrepare(tx, QY_MK_CONCRETE_RES)->execute();
}


//
// Object Queries

// Reconstitutes an object from a database ID.
template <class IdTag, class BiMap>
static
typename BiMap::Forward::Value
queryDBObject( Concolic::Database& db,
               SqlTransactionPtr tx,
               ObjectId<IdTag> id,
               BiMap& objmap
             )
{
  typedef typename BiMap::Forward::Value Ptr;
  typedef typename Ptr::Pointee          ObjType;

  Ptr             obj;

  if (!id) return obj;

  SqlStatementPtr stmt = sqlPrepare(tx, objQueryString(id), id.get());
  SqlIterator     it   = stmt->begin();

  if (it.at_eof()) return obj;

  obj  = ObjType::instance();

  populateObjFromQuery(it, *obj);
  dependentObjQuery(db, tx, id, *obj);
  objmap.insert(id, obj);

  return obj;
}


template <class IdTag, class BidirectionalMap>
static
typename BidirectionalMap::Forward::Value
_object( Database& db,
         SqlTransactionPtr tx,
         ObjectId<IdTag> id,
         Update::Flag update,
         BidirectionalMap& objmap
       )
{
  typedef typename BidirectionalMap::Forward ForwardMap;
  typedef typename ForwardMap::Value         ResultType;

  if (!id)
    return ResultType();

  if (Update::YES == update)
    return queryDBObject(db, tx, id, objmap);

  const ForwardMap&                      map = objmap.forward();
  typename ForwardMap::ConstNodeIterator pos = map.find(id);

  if (pos == map.nodes().end())
    return queryDBObject(db, tx, id, objmap);

  ResultType res = pos.base()->second;
  return res;
}


template <class IdTag, class BidirectionalMap>
static
typename BidirectionalMap::Forward::Value
_object( Database& db,
         SqlDatabase::ConnectionPtr dbconn,
         ObjectId<IdTag> id,
         Update::Flag update,
         BidirectionalMap& objmap
       )
{
  typedef typename BidirectionalMap::Forward::Value ResultType;

  DBTxGuard  dbtx(dbconn);
  ResultType res = _object(db, dbtx.tx(), id, update, objmap);

  dbtx.commit();
  return res;
}

int sqlLastRowId(SqlTransactionPtr tx)
{
  return sqlPrepare(tx, QY_LAST_ROW_SQLITE3)
           ->execute_int();
}

TestSuite::Ptr
Database::object(TestSuiteId id, Update::Flag update)
{
  // lock to protect this object's BiMap
  // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _object(*this, dbconn_, id, update, testSuites_);
}

Specimen::Ptr
Database::object(SpecimenId id, Update::Flag update)
{
  // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _object(*this, dbconn_, id, update, specimens_);
}

TestCase::Ptr
Database::object(TestCaseId id, Update::Flag update)
{
  // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _object(*this, dbconn_, id, update, testCases_);
}

Specimen::Ptr
Database::object_ns(SqlTransactionPtr tx, SpecimenId id)
{
  return _object(*this, tx, id, Concolic::Update::NO, specimens_);
}


//
// methods to insert new objects into the DB

TestSuiteId
insertDBObject(Concolic::Database&, SqlTransactionPtr tx, TestSuite::Ptr obj)
{
  sqlPrepare(tx, QY_NEW_TESTSUITE, obj->name())
    ->execute();

  return TestSuiteId(sqlLastRowId(tx));
}

SpecimenId
insertDBObject(Concolic::Database&, SqlTransactionPtr tx, Specimen::Ptr obj)
{
  sqlPrepare(tx, QY_NEW_SPECIMEN, obj->name(), obj->content())
    ->execute();

  return SpecimenId(sqlLastRowId(tx));
}


//
// methods to insert/update new objects into the DB

typedef SqlQuery<bt::tuple<int>::inherited> SqlQueryInt;

void deleteTestCaseElems( SqlTransactionPtr tx, const SqlQueryInt& query, int tcid)
{
  sqlPrepare(tx, query, tcid)
    ->execute();
}

struct CmdLineArgInserter
{
  SqlTransactionPtr tx;
  int               testcaseId;
  int               num;

  CmdLineArgInserter(SqlTransactionPtr transx, int tcid)
  : tx(transx), testcaseId(tcid), num(0)
  {}

  int queryCmdLineArgId(const std::string& arg)
  {
    SqlStatementPtr stmt = sqlPrepare(tx, QY_CMDLINEARG_ID, arg);
    SqlIterator     iter = stmt->begin();

    if (iter != stmt->end())
      return iter.get_i32(0);

    sqlPrepare(tx, QY_NEW_CMDLINEARG, arg)
      ->execute();

    return sqlLastRowId(tx);
  }

  void operator()(const std::string& arg)
  {
    int cmdlineargId = queryCmdLineArgId(arg);

    ++num;

    sqlPrepare(tx, QY_NEW_TESTCASE_CARG, testcaseId, num, cmdlineargId)
      ->execute();
  }
};

struct EnvVarInserter
{
  SqlTransactionPtr tx;
  int               testcaseId;

  EnvVarInserter(SqlTransactionPtr transx, int tcid)
  : tx(transx), testcaseId(tcid)
  {}

  int queryEnvVarId(const EnvValue& envvar)
  {
    SqlStatementPtr stmt = sqlPrepare(tx, QY_ENVVAR_ID, envvar.first, envvar.second);
    SqlIterator     iter = stmt->begin();

    if (iter != stmt->end())
      return iter.get_i32(0);

    sqlPrepare(tx, QY_NEW_ENVVAR, envvar.first, envvar.second)
      ->execute();

    return sqlLastRowId(tx);
  }

  void operator()(const EnvValue& envvar)
  {
    int             envvarId = queryEnvVarId(envvar);

    sqlPrepare(tx, QY_NEW_TESTCASE_EVAR, testcaseId, envvarId)
      ->execute();
  }
};

void dependentObjInsert(SqlTransactionPtr tx, int tcid, TestCase::Ptr obj)
{
  std::vector<std::string> args = obj->args();
  std::vector<EnvValue>    envv = obj->env();

  std::for_each(args.begin(), args.end(), CmdLineArgInserter(tx, tcid));
  std::for_each(envv.begin(), envv.end(), EnvVarInserter(tx, tcid));
}

void dependentObjUpdate(SqlTransactionPtr tx, int tcid, TestCase::Ptr obj)
{
  deleteTestCaseElems(tx, QY_RM_TESTCASE_CARG, tcid);
  deleteTestCaseElems(tx, QY_RM_TESTCASE_EVAR, tcid);

  dependentObjInsert(tx, tcid, obj);
}

template <class ObjPtr, class BidirectionalMap>
typename BidirectionalMap::Reverse::Value
_id(Concolic::Database&, SqlConnectionPtr, ObjPtr, Update::Flag, BidirectionalMap&);


TestCaseId
insertDBObject(Concolic::Database& db, SqlTransactionPtr tx, TestCase::Ptr obj)
{
  static const std::string exec = "linux";

  const int       specimenId  = db.id_ns(tx, obj->specimen()).get();

  sqlPrepare(tx, QY_NEW_TESTCASE, specimenId, obj->name(), exec)
    ->execute();

  const int       testcaseId  = sqlLastRowId(tx);

  dependentObjInsert(tx, testcaseId, obj);
  return TestCaseId(testcaseId);
}

void
updateDBObject(Concolic::Database& db, SqlTransactionPtr& tx, TestSuite::Ptr obj, TestSuiteId id)
{
  sqlPrepare(tx, QY_UPD_TESTSUITE, obj->name(), id.get())
    ->execute();
}

void
updateDBObject(Concolic::Database& db, SqlTransactionPtr& tx, Specimen::Ptr obj, SpecimenId id)
{
  sqlPrepare(tx, QY_UPD_SPECIMEN, obj->name(), obj->content(), id.get())
    ->execute();
}

void
updateDBObject(Concolic::Database& db, SqlTransactionPtr tx, TestCase::Ptr obj, TestCaseId id)
{
  // Update::NO: updating the specimen object for every update to a
  //   testcase is excessive (i.e., copying the entire binary).
  const int                specId       = db.id_ns(tx, obj->specimen(), Update::NO).get();
  Sawyer::Optional<double> concreteRank = obj->concreteRank();

  // \todo use NO_CONCRETE_RANK, since queries cannot test for null
  const double             rank         = concreteRank ? concreteRank.get() : NO_CONCRETE_RANK;
  const bool               hasConc      = obj->hasConcolicTest();

  sqlPrepare(tx, QY_UPD_TESTCASE, specId, obj->name(), "linux", rank, hasConc, id.get())
     ->execute();

  dependentObjUpdate(tx, id.get(), obj);
}


//
// Returns an ID number for an object, optionally writing to the database.

template <class ObjPtr, class BidirectionalMap>
typename BidirectionalMap::Reverse::Value
_id( Concolic::Database& db,
     SqlTransactionPtr tx,
     ObjPtr obj,
     Update::Flag update,
     BidirectionalMap& objmap
   )
{
  typedef typename BidirectionalMap::Reverse ReverseMap;
  typedef typename ReverseMap::Value         ObjIdType;

  const ReverseMap&                       map = objmap.reverse();
  typename ReverseMap::ConstValueIterator pos = map.find(obj);

  if (pos == map.nodes().end())
  {
    if (Update::NO == update) return ObjIdType();

    ObjIdType objid = insertDBObject(db, tx, obj);

    objmap.insert(objid, obj);
    return objid;
  }

  if (Update::YES == update) updateDBObject(db, tx, obj, pos.base()->second);

  return pos.base()->second;
}


template <class ObjPtr, class BidirectionalMap>
typename BidirectionalMap::Reverse::Value
_id( Concolic::Database& db,
     SqlConnectionPtr dbconn,
     ObjPtr obj,
     Update::Flag update,
     BidirectionalMap& objmap
   )
{
  typedef typename BidirectionalMap::Reverse::Value ResultType;

  DBTxGuard  dbtx(dbconn);
  ResultType res = _id(db, dbtx.tx(), obj, update, objmap);

  dbtx.commit();
  return res;
}


TestSuiteId
Database::id(const TestSuite::Ptr& pts, Update::Flag update)
{
  //~ // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _id(*this, dbconn_, pts, update, testSuites_);
}

SpecimenId
Database::id(const Specimen::Ptr& psp, Update::Flag update)
{
  //~ // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _id(*this, dbconn_, psp, update, specimens_);
}

TestCaseId
Database::id(const TestCase::Ptr& ptc, Update::Flag update)
{
  //~ // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _id(*this, dbconn_, ptc, update, testCases_);
}

TestSuiteId
Database::id_ns(SqlTransactionPtr tx, const TestSuite::Ptr& obj, Update::Flag update)
{
  return _id(*this, tx, obj, update, testSuites_);
}

TestCaseId
Database::id_ns(SqlTransactionPtr tx, const TestCase::Ptr& obj, Update::Flag update)
{
  return _id(*this, tx, obj, update, testCases_);
}

SpecimenId
Database::id_ns(SqlTransactionPtr tx, const Specimen::Ptr& obj, Update::Flag update)
{
  return _id(*this, tx, obj, update, specimens_);
}


void failNonExistingDB(bool fileExists, const std::string& url, const boost::filesystem::path &file) {
  if (!fileExists)
      throw Exception("database file for URL \"" + StringUtility::cEscape(url) + "\" does not exist (cannot open)");
}

void overwriteExistingDB(bool fileExists, const std::string &url, const boost::filesystem::path &file) {
  if (fileExists)
    boost::filesystem::remove(file);
}

template <class ExistingDBPolicy>
SqlConnectionPtr
connectToDB(const std::string& url, ExistingDBPolicy policy) {
    std::pair<boost::filesystem::path, bool> dbinfo = dbProperties(url);
    policy(boost::filesystem::exists(dbinfo.first), url, dbinfo.first);
    return SqlDatabase::Connection::create(url);
}

Database::Ptr
Database::instance(const std::string &url)
{
  SqlConnectionPtr dbconn = connectToDB(url, failNonExistingDB);

  if (!isDbInitialized(dbconn))
      throw Exception("database \"" + StringUtility::cEscape(url) + "\" is not a concolic database");

  Ptr              db(new Database);

  db->dbconn_ = dbconn;
    db->testSuites();
    return db;
}

Database::Ptr
Database::create(const std::string& url)
{
  SqlConnectionPtr dbconn = connectToDB(url, overwriteExistingDB);
  DBTxGuard        dbtx(dbconn);

  // create tables
  initializeDB(dbtx.tx());
  dbtx.commit();

  Ptr              db(new Database);

  db->dbconn_      = dbconn;
  return db;
}

Database::Ptr
Database::create(const std::string& url, const std::string& testSuiteName)
{
  Ptr         db = create(url);
  DBTxGuard   dbtx(db->dbconn_);

  // insert new testsuite
  sqlPrepare(dbtx.tx(), QY_NEW_TESTSUITE, testSuiteName)
    ->execute();

  // query id
  TestSuiteId tsid(sqlPrepare(dbtx.tx(), QY_TESTSUITE_BY_NAME, testSuiteName)->execute_int());
  dbtx.commit();

  ROSE_ASSERT(tsid.get() == DBSQLITE_FIRST_ROWID); // first inserted suite
  db->testSuiteId_ = tsid;
  return db;
}


void
Database::assocTestCaseWithTestSuite(TestCaseId testcase, TestSuiteId testsuite)
{
  //~ // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  DBTxGuard       dbtx(dbconn_);

  sqlPrepare(dbtx.tx(), QY_NEW_TESTSUITE_TESTCASE, testsuite.get(), testcase.get())
    ->execute();

  dbtx.commit();
}

#if WITH_DIRECT_SQLITE3


struct GuardedStmt;

struct GuardedDB
{
    explicit
    GuardedDB(const std::string& url)
    : db_(NULL), dbg_(false)
    {
      std::pair<boost::filesystem::path, bool> properties = dbProperties(url);

      sqlite3_open(properties.first.native().c_str(), &db_);
      dbg_ = properties.second;
    }

    ~GuardedDB()
    {
      if (db_)
        sqlite3_close(db_);
    }

    operator sqlite3* () { return db_; }

    void debug(GuardedStmt&);

  private:
    sqlite3* db_;
    bool     dbg_;
};

struct GuardedStmt
{
    template <class T>
    explicit
    GuardedStmt(const SqlQuery<T>& query)
    : sql_(query.sql), stmt_(NULL)
    {}

    ~GuardedStmt()
    {
      if (stmt_)
        sqlite3_finalize(stmt_);
    }

    sqlite3_stmt*& stmt()        { return stmt_; }
    const char*    c_str() const { return sql_.c_str(); }

  private:
    std::string     sql_;
    sqlite3_stmt*   stmt_;
};

//~ struct SqlLiteStringGuard
//~ {
  //~ explicit
  //~ SqlLiteStringGuard(const char* s)
  //~ : str(s)
  //~ {}

  //~ ~SqlLiteStringGuard()
  //~ {
    //~ if (str) sqlite3_free(const_cast<char*>(str));
  //~ }

  //~ operator const char*() { return str; }

  //~ const char* const str;
//~ };


void GuardedDB::debug(GuardedStmt& sql)
{
  if (!dbg_) return;

  //~ Available since the pi release ( >= 3.14 )
  //~ SqlLiteStringGuard sqlstmt(sqlite3_expanded_sql(sql.stmt()));
  //~ SqlLiteStringGuard sqlstmt(NULL);

  //~ Sawyer::Message::mlog[Sawyer::Message::INFO]
  std::cerr
     << sql.c_str() << std::endl;
}


void checkSql(GuardedDB& db, int sql3code, int expected = SQLITE_OK)
{
  if (sql3code != expected)
    throw Exception(sqlite3_errmsg(db));
}

template <class Iterator>
void storeRBAFile(GuardedDB& db, SpecimenId id, Iterator start, Iterator limit)
{
  typedef typename std::iterator_traits<Iterator>::value_type value_type;

  GuardedStmt  sql(QY_NEW_RBAFILE);
  const size_t sz = sizeof(value_type)*std::distance(start, limit);

  checkSql( db, sqlite3_prepare_v2(db, sql.c_str(), -1, &sql.stmt(), NULL) );
  checkSql( db, sqlite3_bind_int(sql.stmt(), 1, id.get()) );
  checkSql( db, sqlite3_bind_blob(sql.stmt(), 2, &*start, sz, SQLITE_STATIC) );

  db.debug(sql);

  checkSql( db, sqlite3_step(sql.stmt()), SQLITE_DONE );
}

template <class Sink>
void retrieveRBAFile(GuardedDB& db, SpecimenId id, Sink sink)
{
  GuardedStmt  sql(QY_RBAFILE);

  checkSql( db, sqlite3_prepare_v2(db, sql.c_str(), -1, &sql.stmt(), NULL) );
  checkSql( db, sqlite3_bind_int(sql.stmt(), 1, id.get()) );
  checkSql( db, sqlite3_step(sql.stmt()), SQLITE_ROW );

  size_t      bytes = sqlite3_column_bytes(sql.stmt(), 0);
  const char* data  = reinterpret_cast<const char*>(sqlite3_column_blob(sql.stmt(), 0));

  sink.reserve(bytes);
  std::copy(data, data+bytes, sink.inserter());

  checkSql( db, sqlite3_step(sql.stmt()), SQLITE_DONE );
}

#endif /* WITH_DIRECT_SQLITE3 */


bool
Database::rbaExists(Database::SpecimenId id)
{
  //~ // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  DBTxGuard dbtx(dbconn_);
  int       res = sqlPrepare(dbtx.tx(), QY_NUM_RBAFILES, id.get())
                    ->execute_int();

  dbtx.commit();
  ROSE_ASSERT(res == 0 || res == 1);
  return res == 1;
}

void
Database::saveRbaFile(const boost::filesystem::path& path, Database::SpecimenId id)
{
#if WITH_DIRECT_SQLITE3
  namespace bstio = boost::iostreams;

  GuardedDB                 db(dbconn_->openspec());
  bstio::mapped_file_source binary(path);

  storeRBAFile(db, id, binary.begin(), binary.end());
#else
  //~ // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  DBTxGuard            dbtx(dbconn_);
  std::vector<uint8_t> content = loadBinaryFile(path);

  sqlPrepare(dbtx.tx(), QY_NEW_RBAFILE, id.get(), content)
    ->execute();

  dbtx.commit();
#endif /* WITH_DIRECT_SQLITE3 */
}

void
Database::extractRbaFile(const boost::filesystem::path& path, Database::SpecimenId id) {

#if WITH_DIRECT_SQLITE3
  GuardedDB     db(dbconn_->openspec());
  // \todo consider using mapped_file_sink
  std::ofstream outfile(path.string().c_str(), std::ofstream::binary);

  if (!outfile.good())
      throw Exception("unable to open file \"" + StringUtility::cEscape(path.string()) + "\"");

  retrieveRBAFile(db, id, FileSink<char>(outfile));
#else
  // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  DBTxGuard       dbtx(dbconn_);
  SqlStatementPtr stmt = sqlPrepare(dbtx.tx(), QY_RBAFILE, id.get());

  storeBinaryFile(stmt->execute_blob(), path);
  dbtx.commit();
#endif /* WITH_DIRECT_SQLITE3 */
}

void
Database::eraseRba(Database::SpecimenId id) {
  // SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  DBTxGuard       dbtx(dbconn_);

  sqlPrepare(dbtx.tx(), QY_RM_RBAFILE, id.get())
    ->execute();

  dbtx.commit();
}


std::vector<Database::TestCaseId>
Database::needConcreteTesting(size_t num)
{
  return queryIds<TestCase>(dbconn_, testSuiteId_, QY_ALL_NEED_CONCRETE, QY_NEED_CONCRETE, num);
}

std::vector<Database::TestCaseId>
Database::needConcolicTesting(size_t num)
{
  return queryIds<TestCase>(dbconn_, testSuiteId_, QY_ALL_NEED_CONCOLIC, QY_NEED_CONCOLIC, num);
}

bool
Database::hasUntested() const
{
  return queryIds<TestCase>(dbconn_, testSuiteId_, QY_ALL_NEED_CONCRETE, QY_NEED_CONCRETE, 1).size();
}

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
template <class T>
static
std::string xml(const T& o)
{
  std::stringstream            stream;
  boost::archive::xml_oarchive oa(stream);

  oa << BOOST_SERIALIZATION_NVP(o);
  return stream.str();
}

template <class T>
static
std::string text(const T& o)
{
  std::stringstream             stream;
  boost::archive::text_oarchive oa(stream);

  oa << BOOST_SERIALIZATION_NVP(o);
  return stream.str();
}
#endif /* ROSE_HAVE_BOOST_SERIALIZATION_LIB */

void
Database::insertConcreteResults(const TestCase::Ptr &testCase, const ConcreteExecutor::Result& details)
{
  std::string  detailtxt = "<error>requires BOOST serialization</error>";

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
  // was: detailtxt = xml(details);
  //      BOOST serialization does not seem to pick up the polymorphic
  //      type...
  //      BOOST_CLASS_EXPORT_KEYs are defined in BinaryConcolic.h
  //      BOOST_CLASS_EXPORT_IMPLEMENTs are defined in LinuxExecutor.C
  const LinuxExecutor::Result* linuxres = dynamic_cast<const LinuxExecutor::Result*>(&details);

  // \todo enable BOOST serialization polymorphism
  detailtxt = linuxres ? xml(*linuxres) : xml(details);
  //~ detailtxt = text(details);

  //~ std::cerr << "XML:" << detailtxt << std::endl;
#else
  //~ Sawyer::Message::mlog[Sawyer::Message::INFO]
  std::cerr
    << "ConcolicDB: Boost::Serialization not available; result not serialized."
    << std::endl;
#endif /* ROSE_HAVE_BOOST_SERIALIZATION_LIB */

  {
    DBTxGuard  dbtx(dbconn_);
    TestCaseId tcid = id_ns(dbtx.tx(), testCase);

    updateDBObject(*this, dbtx.tx(), testCase, tcid);

    // \todo can we overwrite existing results?
    //       if yes, the entries in the QY_NEW_CONCRETE_RES may need
    //       to be updated or deleted.
    sqlPrepare(dbtx.tx(), QY_NEW_CONCRETE_RES, tcid.get(), detailtxt)
      ->execute();

    dbtx.commit();
  }
}

void writeDBSchema(std::ostream& os)
{
  os << QY_MK_TESTSUITES         << "\n\n"
     << QY_MK_SPECIMENS          << "\n\n"
     << QY_MK_TESTCASES          << "\n\n"
     << QY_MK_CMDLINEARGS        << "\n\n"
     << QY_MK_ENVVARS            << "\n\n"
     << QY_MK_TESTCASE_ARGS      << "\n\n"
     << QY_MK_TESTCASE_EVAR      << "\n\n"
     //~ << QY_MK_TESTSUITE_TESTCASE << "\n\n"
     << QY_MK_RBA_FILES          << "\n\n"
     << QY_MK_CONCRETE_RES       ;
}

void writeSqlStmts(std::ostream& os)
{
  os << QY_DB_INITIALIZED             << "\n\n"
     << QY_ALL_TESTSUITES             << "\n\n"
     << QY_TESTSUITE                  << "\n\n"
     << QY_TESTSUITE_BY_NAME          << "\n\n"
     << QY_ALL_SPECIMENS              << "\n\n"
     << QY_SPECIMENS_IN_SUITE         << "\n\n"
     << QY_SPECIMEN                   << "\n\n"
     << QY_SPECIMEN_BY_NAME           << "\n\n"
     << QY_SPECIMEN_IN_SUITE_BY_NAME  << "\n\n"
     << QY_ALL_TESTCASES              << "\n\n"
     << QY_TESTCASES_IN_SUITE         << "\n\n"
     << QY_NEED_CONCOLIC              << "\n\n"
     << QY_ALL_NEED_CONCOLIC          << "\n\n"
     << QY_NEED_CONCRETE              << "\n\n"
     << QY_ALL_NEED_CONCRETE          << "\n\n"
     << QY_TESTCASE                   << "\n\n"
     << QY_TESTCASE_ARGS              << "\n\n"
     << QY_TESTCASE_ENV               << "\n\n"
     << QY_TESTCASE_SPECIMEN          << "\n\n"
     << QY_NEW_TESTSUITE              << "\n\n"
     << QY_UPD_TESTSUITE              << "\n\n"
     << QY_NEW_SPECIMEN               << "\n\n"
     << QY_UPD_SPECIMEN               << "\n\n"
     << QY_NEW_TESTCASE               << "\n\n"
     << QY_UPD_TESTCASE               << "\n\n"
     << QY_RM_TESTCASE_EVAR           << "\n\n"
     << QY_NEW_TESTCASE_EVAR          << "\n\n"
     << QY_ENVVAR_ID                  << "\n\n"
     << QY_NEW_ENVVAR                 << "\n\n"
     << QY_RM_TESTCASE_CARG           << "\n\n"
     << QY_NEW_TESTCASE_CARG          << "\n\n"
     << QY_NEW_TESTSUITE_TESTCASE     << "\n\n"
     << QY_CMDLINEARG_ID              << "\n\n"
     << QY_NEW_CMDLINEARG             << "\n\n"
     << QY_NUM_RBAFILES               << "\n\n"
     << QY_NEW_RBAFILE                << "\n\n"
     << QY_RBAFILE                    << "\n\n"
     << QY_RM_RBAFILE                 << "\n\n"
     << QY_NEW_CONCRETE_RES           << "\n\n"
     << QY_LAST_ROW_SQLITE3           ;
}


} // namespace
} // namespace
} // namespace
