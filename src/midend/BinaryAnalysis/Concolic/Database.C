#include <sage3basic.h>
#include <rosePublicConfig.h>
#include <BinaryConcolic.h>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/algorithm/string/predicate.hpp>

#ifdef ROSE_HAVE_SQLITE3
// bypass intermediate layers for very large files (RBA)
#define WITH_DIRECT_SQLITE3 1 
#endif

#if WITH_DIRECT_SQLITE3
#include <sqlite3.h>
#endif /* WITH_DIRECT_SQLITE3 */

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
    template <class ExceptionType>
    static inline
    void
    throw_ex(std::string arg1, const std::string& arg2 = "", const std::string& arg3 = "")
    {
      arg1.append(arg2);
      arg1.append(arg3);

      throw ExceptionType(arg1);
    }

    static const
    std::string QY_DB_INITIALIZED     = "SELECT count(*)"
                                        "  FROM TestSuites";

    //
    // make tables

    static const
    std::string QY_MK_TEST_SUITES     = "CREATE TABLE \"TestSuites\" ("
                                        "  \"id\" int PRIMARY KEY,"
                                        "  \"name\" varchar(256) UNIQUE NOT NULL"
                                        ");";

    static const
    std::string QY_MK_SPECIMENS       = "CREATE TABLE \"Specimens\" ("
                                        "  \"id\" int PRIMARY KEY,"
                                        "  \"name\" varchar(256) UNIQUE NOT NULL,"
                                        "  \"binary\" Blob NOT NULL"
                                        ");";

    static const
    std::string QY_MK_TESTCASES       = "CREATE TABLE \"TestCases\" ("
                                        "  \"id\" int PRIMARY KEY,"
                                        "  \"specimen_id\" int NOT NULL,"
                                        "  \"name\" varchar(128),"
                                        "  \"executor\" varchar(128) CHECK(executor = \"linux\"),"
                                        "  \"result\" int,"
                                        "  \"stdout\" text,"
                                        "  \"stderr\" text,"
                                        " CONSTRAINT \"fk_specimen_testcase\" FOREIGN KEY (\"specimen_id\") REFERENCES \"Specimens\" (\"id\"),"
                                        " CONSTRAINT \"uq_specimen_name\" UNIQUE (\"specimen_id\", \"name\")"
                                        ");";

    static const
    std::string QY_MK_CMDLINEARGS     = "CREATE TABLE \"CmdLineArgs\" ("
                                        "  \"id\" int PRIMARY KEY,"
                                        "  \"arg\" varchar(128) UNIQUE NOT NULL"
                                        ");";

    static const
    std::string QY_MK_ENVVARS         = "CREATE TABLE \"EnvironmentVariables\" ("
                                        "  \"id\" int PRIMARY KEY,"
                                        "  \"name\" varchar(128) NOT NULL,"
                                        "  \"value\" varchar(256) NOT NULL,"
                                        " CONSTRAINT \"uq_name_value\" UNIQUE (\"name\", \"value\")"
                                        ");";

    static const
    std::string QY_MK_TESTCASE_ARGS   = "CREATE TABLE \"TestCaseArguments\" ("
                                        "  \"testcase_id\" int NOT NULL,"
                                        "  \"seqnum\" int NOT NULL,"
                                        "  \"cmdlinearg_id\" int NOT NULL,"
                                        " CONSTRAINT \"fk_testcase_argument\" FOREIGN KEY (\"testcase_id\") REFERENCES \"TestCases\" (\"id\"),"
                                        " CONSTRAINT \"fk_cmdlineargs_argument\" FOREIGN KEY (\"cmdlinearg_id\") REFERENCES \"CmdLineArgs\" (\"id\"),"
                                        " CONSTRAINT \"pk_argument\" PRIMARY KEY (\"testcase_id\", \"seqnum\")"
                                        ");";

    static const
    std::string QY_MK_TESTCASE_EVAR   = "CREATE TABLE \"TestCaseVariables\" ("
                                        "  \"testcase_id\" int NOT NULL,"
                                        "  \"envvar_id\" int NOT NULL,"
                                        " CONSTRAINT \"fk_testcase_testcasevar\" FOREIGN KEY (\"testcase_id\") REFERENCES \"TestCases\" (\"id\"),"
                                        " CONSTRAINT \"fk_envvar_testcasevar\" FOREIGN KEY (\"envvar_id\") REFERENCES \"EnvironmentVariables\" (\"id\"),"
                                        " CONSTRAINT \"pk_testcasevar\" PRIMARY KEY (\"testcase_id\", \"envvar_id\")"
                                        ");";

    static const
    std::string QY_MK_TESTSUITE_TESTCASE =
                                       "CREATE TABLE \"TestSuiteTestCases\" ("
                                       "  \"testsuite_id\" int NOT NULL,"
                                       "  \"testcase_id\" int NOT NULL,"
                                       " CONSTRAINT \"fk_testsuite_parent\" FOREIGN KEY (\"testcase_id\") REFERENCES \"TestSuites\" (\"id\"),"
                                       " CONSTRAINT \"fk_testsuite_children\" FOREIGN KEY (\"testcase_id\") REFERENCES \"TestCases\" (\"id\"),"
                                       " CONSTRAINT \"pk_testsuite_members\" PRIMARY KEY (\"testsuite_id\", \"testcase_id\")"
                                       ");";
                                       
    static const
    std::string QY_MK_RBA_FILES       = "CREATE TABLE \"RBAFiles\" ("
                                        "  \"id\" int PRIMARY KEY,"
                                        "  \"specimen_id\" int UNIQUE NOT NULL,"
                                        "  \"data\" Blob NOT NULL,"
                                        " CONSTRAINT \"fk_specimen_rba\" FOREIGN KEY (\"specimen_id\") REFERENCES \"Specimens\" (\"id\")"
                                        ");";

    //
    // db queries

    static const
    std::string QY_ALL_TEST_SUITES    = "SELECT rowid FROM TestSuites ORDER BY rowid;";

    static const
    std::string QY_TEST_SUITE         = "SELECT name FROM TestSuites WHERE rowid = ?;";
    
    static const
    std::string QY_TEST_SUITE_BY_NAME = "SELECT rowid FROM TestSuites WHERE name = ?;";    

    static const
    std::string QY_ALL_SPECIMENS      = "SELECT rowid FROM Specimens ORDER BY rowid;";

    static const
    std::string QY_SPECIMENS_IN_SUITE = "SELECT DISTINCT specimen_id"
                                        "  FROM TestCases"
                                        " WHERE testsuite_id = ?;";

    static const
    std::string QY_SPECIMEN           = "SELECT name, binary FROM Specimens "
                                        " WHERE rowid = ?;";

    static const
    std::string QY_ALL_TESTCASES      = "SELECT rowid FROM TestCases ORDER BY id;";

    static const
    std::string QY_TESTCASES_IN_SUITE = "SELECT tc.rowid"
                                        "  FROM TestCases tc, TestSuiteTestCases tt"
                                        " WHERE tc.rowid = tt.testcase_id"
                                        "   AND tt.testsuite_id = ?;";

    static const
    std::string QY_TESTCASE           = "SELECT executor"
                                        "  FROM TestCases"
                                        " WHERE rowid = ?;";

    static const
    std::string QY_TESTCASE_ARGS      = "SELECT c.arg"
                                        "  FROM CmdLineArgs c, TestCaseArguments t"
                                        " WHERE c.rowid = t.cmdlinearg_id"
                                        "   AND t.testcase_id = ?"
                                        " ORDER BY seqnum;";

    static const
    std::string QY_TESTCASE_ENV       = "SELECT e.name, e.value"
                                        "  FROM EnvironmentVariables e, TestCaseVariables t"
                                        " WHERE e.rowid = t.envvar_id"
                                        "   AND t.testcase_id = ?;";

    static const
    std::string QY_TESTCASE_SPECIMEN  = "SELECT specimen_id"
                                        "  FROM TestCases"
                                        " WHERE rowid = ?;";

    static const
    std::string QY_NEW_TESTSUITE      = "INSERT INTO TestSuites"
                                        "  (name)"
                                        "  VALUES(?);";

    static const
    std::string QY_UPD_TESTSUITE      = "UPDATE TestSuites"
                                        "   SET name = ?"
                                        " WHERE rowid = ?;";

    static const
    std::string QY_NEW_SPECIMEN      = "INSERT INTO Specimens"
                                       "  (name, binary)"
                                       "  VALUES(?,?);";

    static const
    std::string QY_UPD_SPECIMEN      = "UPDATE Specimens"
                                        "   SET name = ?, binary = ?"
                                        " WHERE rowid = ?;";

    static const
    std::string QY_NEW_TESTCASE      = "INSERT INTO TestCases"
                                       "  (specimen_id, name, executor)"
                                       "  VALUES(?,?,?);";

    static const
    std::string QY_UPD_TESTCASE      = "UPDATE TestCases"
                                       "   SET specimen_id = ?, name = ?, executor = ?"
                                       " WHERE rowid = ?;";

    static const
    std::string QY_RM_TESTCASE_EVAR   = "DELETE FROM TestCaseVariables"
                                        " WHERE testcase_id = ?";

    static const
    std::string QY_NEW_TESTCASE_EVAR = "INSERT INTO TestCaseVariables"
                                       "  (testcase_id, envvar_id)"
                                       "  VALUES(?,?)";

    static const
    std::string QY_ENVVAR_ID         = "SELECT rowid"
                                       "  FROM EnvironmentVariables"
                                       " WHERE name = ?"
                                       "   AND value = ?;";

    static const
    std::string QY_NEW_ENVVAR        = "INSERT INTO EnvironmentVariables"
                                       "  (name, value)"
                                       "  VALUES(?,?);";

    static const
    std::string QY_RM_TESTCASE_CARG   = "DELETE FROM TestCaseArguments"
                                        " WHERE testcase_id = ?";

    static const
    std::string QY_NEW_TESTCASE_CARG = "INSERT INTO TestCaseArguments"
                                       "  (testcase_id, seqnum, cmdlinearg_id)"
                                       "  VALUES(?,?,?)";

    static const
    std::string QY_NEW_TESTSUITE_TESTCASE =
                                      "INSERT INTO TestSuiteTestCases"
                                       "  (testsuite_id, testcase_id)"
                                       "  VALUES(?,?)";

    static const
    std::string QY_CMDLINEARG_ID     = "SELECT rowid"
                                       "  FROM CmdLineArgs"
                                       " WHERE arg = ?;";

    static const
    std::string QY_NEW_CMDLINEARG    = "INSERT INTO CmdLineArgs"
                                        "  (arg)"
                                        "  VALUES(?);";
                                        
    std::string QY_COUNT_RBAFILES    = "SELECT count(*) FROM RBAFiles"
                                       "  WHERE specimen_id = ?;";
                                        
    static const
    std::string QY_NEW_RBAFILE       = "INSERT INTO RBAFiles"
                                        "  (specimen_id, data)"
                                        "  VALUES(?,?);";
    
    static const
    std::string QY_RBAFILE           = "SELECT data FROM RBAFiles"
                                       "  WHERE specimen_id = ?;";
                                       
    static const
    std::string QY_RM_RBAFILE        = "DELETE FROM RBAFiles"
                                       "  WHERE specimen_id = ?;";
                                       
    static const
    std::string QY_LAST_ROW_SQLITE3  = "SELECT last_insert_rowid();";

    SqlStatementPtr
    prepareIdQuery(SqlTransactionPtr tx, TestSuiteId id, TestSuiteId)
    {
      return tx->statement(QY_ALL_TEST_SUITES);
    }

    SqlStatementPtr
    _prepareIdQuery( SqlTransactionPtr tx,
                     TestSuiteId id,
                     const std::string& all,
                     const std::string& selected
                   )
    {
      if (!id) return tx->statement(all);

      SqlStatementPtr stmt = tx->statement(selected);

      stmt->bind(0, id.get());
      return stmt;
    }

    SqlStatementPtr
    prepareIdQuery(SqlTransactionPtr tx, TestSuiteId id, SpecimenId)
    {
      return _prepareIdQuery(tx, id, QY_ALL_SPECIMENS, QY_SPECIMENS_IN_SUITE);
    }

    SqlStatementPtr
    prepareIdQuery(SqlTransactionPtr tx, TestSuiteId id, TestCaseId)
    {
      return _prepareIdQuery(tx, id, QY_ALL_TESTCASES, QY_TESTCASES_IN_SUITE);
    }

    static
    const std::string& objQueryString(const TestSuiteId&) { return QY_TEST_SUITE; }

    static
    const std::string& objQueryString(const TestCaseId&)  { return QY_TESTCASE; }

    static
    const std::string& objQueryString(const SpecimenId&)  { return QY_SPECIMEN; }

    template <class IdTag>
    SqlStatementPtr
    prepareObjQuery(SqlDatabase::TransactionPtr tx, Concolic::ObjectId<IdTag> id)
    {
      if (!id) throw_ex<std::logic_error>("ID not set");

      SqlStatementPtr stmt = tx->statement(objQueryString(id));

      stmt->bind(0, id.get());
      return stmt;
    }

    void executeObjQuery(SqlStatementPtr stmt, Concolic::TestSuite& obj)
    {
      obj.name(stmt->execute_string());
    }

    void executeObjQuery(SqlStatementPtr stmt, Concolic::Specimen& obj)
    {
      SqlIterator it = stmt->begin();

      obj.name(it.get_str(0));
      obj.content(it.get_blob(1));

      ROSE_ASSERT((++it).at_eof());
    }

    void executeObjQuery(SqlStatementPtr stmt, Concolic::TestCase& obj)
    {
      obj.name(stmt->execute_string());
    }

    void dependentObjQuery(Concolic::Database&, SqlTransactionPtr, TestSuiteId, Concolic::TestSuite&)
    {
      // no dependent objects for test suites
    }

    void dependentObjQuery(Concolic::Database&, SqlTransactionPtr, SpecimenId, Concolic::Specimen&)
    {
      // no dependent objects for specimen
    }

    std::string
    retrieve_row(SqlIterator& iter, const std::string&) { return iter.get_str(0); }

    std::pair<std::string, std::string>
    retrieve_row(SqlIterator& iter, const std::pair<std::string, std::string>&)
    {
      return std::make_pair(iter.get_str(0), iter.get_str(1));
    }


    template <class Tag>
    Concolic::ObjectId<Tag>
    retrieve_row(SqlIterator& iter, const Concolic::ObjectId<Tag>&)
    {
      return Concolic::ObjectId<Tag>(iter.get_i32(0));
    }

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
    
    void dependentObjQuery( Concolic::Database& db,
                            SqlTransactionPtr tx,
                            TestCaseId id,
                            Concolic::TestCase& tmp
                          )
    {
      // read command line arguments
      {
        typedef TransformingIterator<std::string> ResultIterator;

        std::vector<std::string> args;
        SqlStatementPtr          stmt = tx->statement(QY_TESTCASE_ARGS);

        stmt->bind(0, id.get());

        ResultIterator           start(stmt->begin());
        ResultIterator           limit(stmt->end());

        std::copy(start, limit, std::back_inserter(args));
        tmp.args(args);
      }

      // read environment arguments
      {
        typedef Concolic::EnvValue             EnvValue;
        typedef TransformingIterator<EnvValue> ResultIterator;

        std::vector<EnvValue>    env;
        SqlStatementPtr          stmt = tx->statement(QY_TESTCASE_ENV);

        stmt->bind(0, id.get());

        ResultIterator           start(stmt->begin());
        ResultIterator           limit(stmt->end());

        std::copy(start, limit, std::back_inserter(env));
        tmp.env(env);
      }

      // read specimen
      {
        SqlStatementPtr          stmt = tx->statement(QY_TESTCASE_SPECIMEN);

        stmt->bind(0, id.get());

        SpecimenId               specimenid(stmt->execute_int());
        Concolic::Specimen::Ptr  specimen = db.object_ns(tx, specimenid);

        assert(specimen.getRawPointer());
        tmp.specimen(specimen);
      }
    }
  }

namespace Concolic {

// responsible for commit handling of transactions
struct DBTxGuard
{
    // creates a new database transaction
    explicit
    DBTxGuard(SqlDatabase::ConnectionPtr conn)
    : tx_(conn->transaction())
    {}

    // in case there was no explicit commit the tx is rolled back
    ~DBTxGuard()
    {
      if (tx_) tx_->rollback();
    }

    SqlTransactionPtr tx() { return tx_; }

    void commit() { tx_->commit(); tx_.reset(); }

    SqlDatabase::Transaction& operator->() { return *tx_; }
    SqlDatabase::Transaction& operator*()  { return *tx_; }

  private:
    SqlTransactionPtr tx_;
};


template <class IdTag>
static inline
std::vector<Concolic::ObjectId<IdTag> >
queryIds(SqlDatabase::ConnectionPtr dbconn, TestSuiteId tsid)
{
  typedef Concolic::ObjectId<IdTag>     IdClass;
  typedef std::vector<IdClass>          ResultVec;
  typedef TransformingIterator<IdClass> ResultIterator;

  DBTxGuard            dbtx(dbconn);
  SqlStatementPtr      stmt = prepareIdQuery(dbtx.tx(), tsid, IdClass());
  ResultIterator       start(stmt->begin());
  const ResultIterator limit(stmt->end());
  ResultVec            result;
  
  std::copy(start, limit, std::back_inserter(result));

  dbtx.commit();
  return result;
}

std::vector<TestSuiteId>
Database::testSuites()
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return queryIds<TestSuite>(dbconn_, testSuiteId_);
}

TestSuite::Ptr
Database::testSuite()
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
  
  if (!testSuiteId_) return TestSuite::Ptr();

  return testSuites_.forward()[testSuiteId_];
}


Database::TestSuiteId
Database::testSuite(const TestSuite::Ptr& obj)
{
  Database::TestSuiteId res = id(obj);
  
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  testSuiteId_ = res;
  return testSuiteId_;
}


// specimens

std::vector<SpecimenId>
Database::specimens()
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return queryIds<Specimen>(dbconn_, testSuiteId_);
}

//
// test cases

std::vector<TestCaseId>
Database::testCases()
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return queryIds<TestCase>(dbconn_, testSuiteId_);
}


//
// Reconstitute an object from a database ID.

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

  Ptr             obj = ObjType::instance();
  SqlStatementPtr stmt = prepareObjQuery(tx, id);

  executeObjQuery(stmt, *obj);
  dependentObjQuery(db, tx, id, *obj);

  objmap.insert(id, obj);
  return obj;
}

bool isDbInitialized(SqlDatabase::ConnectionPtr dbconn)
{
  int res = -1;

  try
  {
    DBTxGuard       dbtx(dbconn);
    SqlStatementPtr stmt = dbtx.tx()->statement(QY_DB_INITIALIZED);

    res  = stmt->execute_int();
    dbtx.commit();
  }
  catch (SqlDatabase::Exception& ex)
  {
  }

  return res >= 0;
}


static
void initializeDB(SqlConnectionPtr dbconn)
{
  DBTxGuard         dbtx(dbconn);
  SqlTransactionPtr tx = dbtx.tx();

  tx->execute(QY_MK_TEST_SUITES);
  tx->execute(QY_MK_SPECIMENS);
  tx->execute(QY_MK_TESTCASES);
  tx->execute(QY_MK_CMDLINEARGS);
  tx->execute(QY_MK_ENVVARS);
  tx->execute(QY_MK_TESTCASE_ARGS);
  tx->execute(QY_MK_TESTCASE_EVAR);
  tx->execute(QY_MK_TESTSUITE_TESTCASE);
  tx->execute(QY_MK_RBA_FILES);
  dbtx.commit();

  std::cerr << "initialized DB" << std::endl;
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

  // \pp \todo id is node defined
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
  SqlStatementPtr qyid = tx->statement(QY_LAST_ROW_SQLITE3);

  return qyid->execute_int();
}

TestSuite::Ptr
Database::object(TestSuiteId id, Update::Flag update)
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _object(*this, dbconn_, id, update, testSuites_);
}

Specimen::Ptr
Database::object(SpecimenId id, Update::Flag update)
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _object(*this, dbconn_, id, update, specimens_);
}

TestCase::Ptr
Database::object(TestCaseId id, Update::Flag update)
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _object(*this, dbconn_, id, update, testCases_);
}

Specimen::Ptr
Database::object_ns(SqlTransactionPtr tx, SpecimenId id)
{
  return _object(*this, tx, id, Concolic::Update::NO, specimens_);
}


TestSuiteId
insertDBObject(Concolic::Database&, SqlTransactionPtr tx, TestSuite::Ptr obj)
{
  SqlStatementPtr stmt = tx->statement(QY_NEW_TESTSUITE);

  stmt->bind(0, obj->name());
  stmt->execute();

  return TestSuiteId(sqlLastRowId(tx));
}

SpecimenId
insertDBObject(Concolic::Database&, SqlTransactionPtr tx, Specimen::Ptr obj)
{
  SqlStatementPtr stmt = tx->statement(QY_NEW_SPECIMEN);

  stmt->bind(0, obj->name());
  stmt->bind(1, obj->content());
  stmt->execute();

  return SpecimenId(sqlLastRowId(tx));
}

void deleteTestCaseElems(SqlTransactionPtr tx, int tcid, const std::string& sqlcmd)
{
  SqlStatementPtr stmt = tx->statement(QY_RM_TESTCASE_CARG);

  stmt->bind(0, tcid);
  stmt->execute();
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
    SqlStatementPtr stmt = tx->statement(QY_CMDLINEARG_ID);

    stmt->bind(0, arg);

    SqlIterator     iter = stmt->begin();

    if (iter != stmt->end())
      return iter.get_i32(0);

    SqlStatementPtr ins  = tx->statement(QY_NEW_CMDLINEARG);

    ins->bind(0, arg);
    ins->execute();

    return sqlLastRowId(tx);
  }

  void operator()(const std::string& arg)
  {
    int cmdlineargId = queryCmdLineArgId(arg);

    ++num;

    SqlStatementPtr stmt = tx->statement(QY_NEW_TESTCASE_CARG);

    stmt->bind(0, testcaseId);
    stmt->bind(1, num);
    stmt->bind(2, cmdlineargId);
    stmt->execute();
    std::cout << "dbtest: inserted cmdlarg " << arg << std::endl;
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
    SqlStatementPtr stmt = tx->statement(QY_ENVVAR_ID);

    stmt->bind(0, envvar.first);
    stmt->bind(1, envvar.second);

    SqlIterator     iter = stmt->begin();

    if (iter != stmt->end())
      return iter.get_i32(0);

    SqlStatementPtr ins  = tx->statement(QY_NEW_ENVVAR);

    ins->bind(0, envvar.first);
    ins->bind(1, envvar.second);
    ins->execute();

    return sqlLastRowId(tx);
  }

  void operator()(const EnvValue& envvar)
  {
    int             envvarId = queryEnvVarId(envvar);
    SqlStatementPtr stmt = tx->statement(QY_NEW_TESTCASE_EVAR);

    stmt->bind(0, testcaseId);
    stmt->bind(1, envvarId);
    stmt->execute();
  }
};

void dependentObjInsert(SqlTransactionPtr tx, int tcid, TestCase::Ptr obj)
{
  std::vector<std::string> args = obj->args();
  std::vector<EnvValue>    envv = obj->env();

  if (args.size() == 0) std::cout << "dbtest: 0 args" << std::endl;
  if (envv.size() == 0) std::cout << "dbtest: 0 envv" << std::endl;

  std::for_each(args.begin(), args.end(), CmdLineArgInserter(tx, tcid));
  std::for_each(envv.begin(), envv.end(), EnvVarInserter(tx, tcid));
}

void dependentObjUpdate(SqlTransactionPtr tx, int tcid, TestCase::Ptr obj)
{
  deleteTestCaseElems(tx, tcid, QY_RM_TESTCASE_CARG);
  deleteTestCaseElems(tx, tcid, QY_RM_TESTCASE_EVAR);

  dependentObjInsert(tx, tcid, obj);
}

template <class ObjPtr, class BidirectionalMap>
typename BidirectionalMap::Reverse::Value
_id(Concolic::Database&, SqlConnectionPtr, ObjPtr, Update::Flag, BidirectionalMap&);


TestCaseId
insertDBObject(Concolic::Database& db, SqlTransactionPtr tx, TestCase::Ptr obj)
{
  SqlStatementPtr stmt = tx->statement(QY_NEW_TESTCASE);
  const int       specimenId  = db.id_ns(tx, obj->specimen()).get();

  stmt->bind(0, specimenId);
  stmt->bind(1, obj->name());
  stmt->bind(2, "linux");
  stmt->execute();

  const int       testcaseId  = sqlLastRowId(tx);

  dependentObjInsert(tx, testcaseId, obj);
  return TestCaseId(testcaseId);
}

void
updateDBObject(Concolic::Database& db, SqlTransactionPtr tx, TestSuite::Ptr obj, TestSuiteId id)
{
  SqlStatementPtr stmt = tx->statement(QY_UPD_TESTSUITE);

  stmt->bind(0, obj->name());
  stmt->bind(1, id.get());
  stmt->execute();
}

void
updateDBObject(Concolic::Database& db, SqlTransactionPtr tx, Specimen::Ptr obj, SpecimenId id)
{
  SqlStatementPtr stmt = tx->statement(QY_UPD_SPECIMEN);

  stmt->bind(0, obj->name());
  stmt->bind(1, obj->content());
  stmt->bind(2, id.get());
  stmt->execute();
}

void
updateDBObject(Concolic::Database& db, SqlTransactionPtr tx, TestCase::Ptr obj, TestCaseId)
{
  throw_ex<std::runtime_error>("NOT IMPLEMENTED YET");
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
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _id(*this, dbconn_, pts, update, testSuites_);
}

SpecimenId
Database::id(const Specimen::Ptr& psp, Update::Flag update)
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _id(*this, dbconn_, psp, update, specimens_);
}

TestCaseId
Database::id(const TestCase::Ptr& ptc, Update::Flag update)
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  return _id(*this, dbconn_, ptc, update, testCases_);
}

TestSuiteId 
Database::id_ns(SqlTransactionPtr tx, const TestSuite::Ptr& obj)
{
  return _id(*this, tx, obj, Update::YES, testSuites_);
}

TestCaseId 
Database::id_ns(SqlTransactionPtr tx, const TestCase::Ptr& obj)
{
  return _id(*this, tx, obj, Update::YES, testCases_);
}

SpecimenId 
Database::id_ns(SqlTransactionPtr tx, const Specimen::Ptr& obj)
{
  return _id(*this, tx, obj, Update::YES, specimens_);
}



Database::Ptr
Database::instance(const std::string &url)
{
    SqlDatabase::Driver dbdriver = SqlDatabase::Connection::guess_driver(url);
    SqlConnectionPtr    dbconn   = SqlDatabase::Connection::create(url, dbdriver);
    Ptr                 db(new Database);

    db->dbconn_ = dbconn;

    if (!isDbInitialized(dbconn))
      initializeDB(dbconn);

    db->testSuites();
    return db;
}

Database::Ptr
Database::create(const std::string& url, const std::string& testSuiteName)
{
    Ptr             db = instance("sqlite:" + url);  
    DBTxGuard       dbtx(db->dbconn_);  
    SqlStatementPtr stmt = dbtx.tx()->statement(QY_TEST_SUITE_BY_NAME);

    stmt->bind(0, testSuiteName);
    TestSuiteId     testSuiteid = TestSuiteId(stmt->execute_int());
    dbtx.commit();
    
    db->testSuiteId_ = testSuiteid;
    return db;
}

void
Database::assocTestCaseWithTestSuite(TestCaseId testcase, TestSuiteId testsuite)
{  
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  DBTxGuard       dbtx(dbconn_);
  SqlStatementPtr stmt = dbtx.tx()->statement(QY_NEW_TESTSUITE_TESTCASE);

  stmt->bind(0, testsuite.get());
  stmt->bind(1, testcase.get());
  stmt->execute();

  dbtx.commit();
}


std::vector<uint8_t> 
loadBinaryFile(const boost::filesystem::path& path)
{
  typedef std::istreambuf_iterator<char> stream_iterator;
  
  std::vector<uint8_t> res;
  std::ifstream        stream(path.string().c_str(), std::ios::in | std::ios::binary);

  if (!stream.good())
  {
    throw_ex<std::runtime_error>("Unable to open ", path.string(), ".");
  }

  std::copy(stream_iterator(stream), stream_iterator(), std::back_inserter(res));
  return res;
}


// https://stackoverflow.com/questions/31131907/writing-into-binary-file-with-the-stdostream-iterator
template <class T, class CharT = char, class Traits = std::char_traits<CharT> >
struct ostreambin_iterator : std::iterator<std::output_iterator_tag, void, void, void, void>
{
  typedef std::basic_ostream<CharT, Traits> ostream_type;
  typedef Traits                            traits_type;
  typedef CharT                             char_type;

  ostreambin_iterator(ostream_type& s) : stream(s) { }

  ostreambin_iterator& operator=(const T& value)
  {
    stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return *this;
  }

  ostreambin_iterator& operator*()     { return *this; }
  ostreambin_iterator& operator++()    { return *this; }
  ostreambin_iterator& operator++(int) { return *this; }

  ostream_type& stream;
};

template <class T>
struct FileSink
{
  typedef ostreambin_iterator<T> insert_iterator;

  std::ostream& datastream;

  FileSink(std::ostream& stream)
  : datastream(stream)
  {}

  void reserve(size_t) {}

  insert_iterator
  inserter()
  {
    return insert_iterator(datastream);
  }
};

void
storeBinaryFile(const std::vector<uint8_t>& data, const boost::filesystem::path& binary)
{
  {
    std::ofstream outfile(binary.string().c_str(), std::ofstream::binary);
  
    if (!outfile.good())
    {
      throw_ex<std::runtime_error>("Unable to open ", binary.string(), ".");
    }

    FileSink<char>              sink(outfile);
  
    sink.reserve(data.size());
    std::copy(data.begin(), data.end(), sink.inserter());
  }
}

#if WITH_DIRECT_SQLITE3  

std::pair<std::string, bool>
dbProperties(const std::string& url)
{
  static const std::string locator  = "sqlite3://";
  static const std::string debugopt = "?debug";
  
  if (!boost::starts_with(url, locator)) return std::make_pair(url, false);
  
  size_t limit = url.find_first_of('?', locator.size());
  bool   with_debug = (  (limit != std::string::npos)
                      && debugopt == url.substr(limit, limit + debugopt.size())
                      );
  
  return std::make_pair(url.substr(locator.size(), limit), with_debug);
}


struct GuardedStmt;

struct GuardedDB
{
    explicit  
    GuardedDB(const std::string& url) 
    : db_(NULL), dbg_(false)
    {
      std::pair<std::string, bool> properties = dbProperties(url);
      
      sqlite3_open(properties.first.c_str(), &db_);
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
    explicit  
    GuardedStmt(const std::string& sql) 
    : sql_(sql), stmt_(NULL) 
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

struct SqlLiteStringGuard
{
  explicit
  SqlLiteStringGuard(const char* s)
  : str(s)
  {}
  
  ~SqlLiteStringGuard() { sqlite3_free(const_cast<char*>(str)); }
  
  const char* str;  
};

std::ostream& operator<<(std::ostream& os, const SqlLiteStringGuard& g)
{
  if (g.str) os << g.str << std::endl;
  
  return os;
}

void GuardedDB::debug(GuardedStmt& sql)
{
  if (!dbg_) return;
  
  //~ SqlLiteStringGuard sqlstmt(sqlite3_expanded_sql(sql.stmt()));
  SqlLiteStringGuard sqlstmt(NULL);
  
  std::cerr << sqlstmt << std::endl;  
}


void checkSql(GuardedDB& db, int sql3code, int expected = SQLITE_OK)
{
  if (sql3code != expected)
    throw_ex<std::runtime_error>(sqlite3_errmsg(db));
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
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
  
  DBTxGuard       dbtx(dbconn_);
  SqlStatementPtr stmt = dbtx.tx()->statement(QY_COUNT_RBAFILES);
  
  stmt->bind(0, id.get());  
  int res = stmt->execute_int();
  
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
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
  
  DBTxGuard            dbtx(dbconn_);
  SqlStatementPtr      stmt = dbtx.tx()->statement(QY_NEW_RBAFILE);
  std::vector<uint8_t> content = loadBinaryFile(path);

  stmt->bind(0, id.get());
  stmt->bind(1, content);
  stmt->execute();
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
    throw_ex<std::runtime_error>("unable to open file: ", path.string());
    
  retrieveRBAFile(db, id, FileSink<char>(outfile));  
#else
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
  
  DBTxGuard       dbtx(dbconn_);
  SqlStatementPtr stmt = dbtx.tx()->statement(QY_RBAFILE);    
  
  stmt->bind(0, id.get());
  storeBinaryFile(stmt->execute_blob(), path);
  dbtx.commit();
#endif /* WITH_DIRECT_SQLITE3 */
}

void
Database::eraseRba(Database::SpecimenId id) {
  //~ SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
  
  DBTxGuard       dbtx(dbconn_);
  SqlStatementPtr stmt = dbtx.tx()->statement(QY_RM_RBAFILE);    
  
  stmt->bind(0, id.get());
  stmt->execute();
  dbtx.commit();    
}


} // namespace
} // namespace
} // namespace
