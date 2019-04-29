#include <sage3basic.h>
#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {

  typedef SqlDatabase::TransactionPtr      SqlTransactionPtr;
  typedef SqlDatabase::StatementPtr        SqlStatementPtr;
  typedef SqlDatabase::Statement::iterator SqlIterator;
  typedef SqlDatabase::ConnectionPtr       SqlConnectionPtr;

  typedef Concolic::Database::TestSuiteId  TestSuiteId;
  typedef Concolic::Database::SpecimenId   SpecimenId;
  typedef Concolic::Database::TestCaseId   TestCaseId;

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
    std::string QY_ALL_TEST_SUITES    = "SELECT id FROM TestSuites ORDER BY id;";

    static const
    std::string QY_TEST_SUITE         = "SELECT name FROM TestSuites WHERE id = ?;";

    static const
    std::string QY_ALL_SPECIMENS      = "SELECT id FROM Specimens ORDER BY id;";

    static const
    std::string QY_SPECIMENS_IN_SUITE = "SELECT DISTINCT specimen_id"
                                        "  FROM TestCases"
                                        " WHERE testsuite_id = ?;";

    static const
    std::string QY_SPECIMEN           = "SELECT name, binary FROM Specimens "
                                        " WHERE id = ?;";

    static const
    std::string QY_ALL_TESTCASES      = "SELECT id FROM TestCases ORDER BY id;";

    static const
    std::string QY_TESTCASES_IN_SUITE = "SELECT id"
                                        "  FROM TestCases"
                                        " WHERE testsuite_id = ?;";

    static const
    std::string QY_TESTCASE           = "SELECT executor"
                                        "  FROM TestCases"
                                        " WHERE id = ?;";

    static const
    std::string QY_TESTCASE_ARGS      = "SELECT c.arg"
                                        "  FROM CmdLineArgs c, TestCaseArguments t"
                                        " WHERE c.id = t.comdlinearg_id"
                                        "   AND t.testcase_id = ?"
                                        " ORDER BY order;";

    static const
    std::string QY_TESTCASE_ENV       = "SELECT e.name, e.value"
                                        "  FROM EnvironmentVariables e, TestCaseVariables t"
                                        " WHERE e.id = t.envvar_id"
                                        "   AND t.testcase_id = ?;";

    static const
    std::string QY_TESTCASE_SPECIMEN  = "SELECT specimen_id"
                                        "  FROM TestCases"
                                        " WHERE id = ?;";

    static const
    std::string QY_NEW_TESTSUITE      = "INSERT INTO TestSuites"
                                        "  (name)"
                                        "  VALUES(?);";

    static const
    std::string QY_UPD_TESTSUITE      = "UPDATE TestSuites"
                                        "   SET name = ?"
                                        " WHERE id = ?;";

    static const
    std::string QY_NEW_SPECIMEN      = "INSERT INTO Specimens"
                                       "  (name, binary)"
                                       "  VALUES(?,?);";

    static const
    std::string QY_UPD_SPECIMEN      = "UPDATE Specimens"
                                        "   SET name = ?, binary = ?"
                                        " WHERE id = ?;";

    static const
    std::string QY_NEW_TESTCASE      = "INSERT INTO TestCases"
                                       "  (testsuite_id, specimen_id, executor)"
                                       "  VALUES(?,?,?);";

    static const
    std::string QY_UPD_TESTCASE      = "UPDATE TestCases"
                                       "   SET testsuite_id = ?, specimen_id = ?, executor = ?"
                                       " WHERE id = ?;";

    static const
    std::string QY_RM_TESTCASE_EVAR   = "DELETE FROM TestCaseVariables"
                                        " WHERE testcase_id = ?";

    static const
    std::string QY_NEW_TESTCASE_EVAR = "INSERT INTO TestCaseVariables"
                                       "  (testcase_id, envvar_id)"
                                       "  VALUES(?,?)";

    static const
    std::string QY_ENVVAR_ID         = "SELECT id"
                                       "  FROM EnvironmentVariables"
                                       " WHERE name = ?"
                                       "   AND value = ?;";

    static const
    std::string QY_NEW_ENVVAR        = "INSERT INTO EnvironmentVariables"                                   "  (name, value)"
                                       "  VALUES(?,?);";

    static const
    std::string QY_RM_TESTCASE_CARG   = "DELETE FROM TestCaseArguments"
                                        " WHERE testcase_id = ?";

    static const
    std::string QY_NEW_TESTCASE_CARG = "INSERT INTO TestCaseArguments"
                                       "  (testcase_id, order, cmdlinearg_id)"
                                       "  VALUES(?,?,?)";

    static const
    std::string QY_CMDLINEARG_ID     = "SELECT id"
                                       "  FROM CmdLineArgs"
                                       " WHERE arg = ?;";

    static const
    std::string QY_NEW_CMDLINEARG    = "INSERT INTO CmdLineArgs"
                                        "  (arg)"
                                        "  VALUES(?);";

    static const
    std::string QY_LAST_ROW_SQLITE3  = "SELECT last_insert_row_id();";

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

    void executeObjQuery(SqlStatementPtr stmt, Concolic::TestSuite& tmp)
    {
      tmp.name(stmt->execute_string());
    }

    void executeObjQuery(SqlStatementPtr stmt, Concolic::Specimen& tmp)
    {
      SqlIterator it = stmt->begin();

      tmp.name(it.get_str(0));
      tmp.content(it.get_blob(1));

      ROSE_ASSERT((++it).at_eof());
    }

    void executeObjQuery(SqlStatementPtr stmt, Concolic::TestCase& tmp)
    {
      tmp.name(stmt->execute_string());
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

        tmp.specimen(db.object(specimenid, Concolic::Update::NO));
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
  ResultVec            result(start, limit);

  dbtx.commit();
  return result;
}

std::vector<TestSuiteId>
Database::testSuites()
{
  return queryIds<TestSuite>(dbconn_, testSuiteId_);
}

TestSuite::Ptr
Database::testSuite()
{
  // \todo \pp what to do if the testsuite is not set?
  // if (!testSuites_) return TestSuite::Ptr(NULL);

  return testSuites_.forward()[testSuiteId_];
}


Database::TestSuiteId
Database::testSuite(const TestSuite::Ptr& pts)
{
  typedef Sawyer::Container::BiMap<TestSuiteId, TestSuite::Ptr> BiMap;

  const BiMap::Reverse&             map = testSuites_.reverse();
  BiMap::Reverse::ConstNodeIterator pos = map.find(pts);

  if (pos == map.nodes().end()) return TestSuiteId();

  return pos.base()->second;
}


// specimens

std::vector<SpecimenId>
Database::specimens()
{
  return queryIds<Specimen>(dbconn_, testSuiteId_);
}

//
// test cases

std::vector<TestCaseId>
Database::testCases()
{
  return queryIds<TestCase>(dbconn_, testSuiteId_);
}


//
// Reconstitute an object from a database ID.

template <class IdTag, class BiMap>
static
typename BiMap::Forward::Value
queryDBObject( Concolic::Database& db,
               SqlDatabase::ConnectionPtr dbconn,
               ObjectId<IdTag> id,
               BiMap& objmap
             )
{
  typedef typename BiMap::Forward::Value Ptr;
  typedef typename Ptr::Pointee          ObjType;

  // \pp \todo how to make this code exception safe?
  Ptr obj = ObjType::instance();

  {
    DBTxGuard       dbtx(dbconn);
    SqlStatementPtr stmt = prepareObjQuery(dbtx.tx(), id);

    executeObjQuery(stmt, *obj);
    dependentObjQuery(db, dbtx.tx(), id, *obj);
    dbtx.commit();
  }

  objmap.insert(id, obj);

  return obj;
}

template <class IdTag, class BidirectionalMap>
static
typename BidirectionalMap::Forward::Value
_object( Concolic::Database& db,
         SqlDatabase::ConnectionPtr dbconn,
         ObjectId<IdTag> id,
         Update::Flag update,
         BidirectionalMap& objmap
       )
{
  typedef typename BidirectionalMap::Forward ForwardMap;
  typedef typename ForwardMap::Value         ResultType;

  // \pp \todo id is node defined
  if (id) return ResultType();
  if (Update::YES == update) return queryDBObject(db, dbconn, id, objmap);
  const ForwardMap&                      map = objmap.forward();

  typename ForwardMap::ConstNodeIterator pos = map.find(id);

  //~if (pos == map.nodes().end()) return queryDBObject(db, dbconn, id, objmap);

  return pos.base()->second;
}

int sqlLastRowId(SqlTransactionPtr tx)
{
  SqlStatementPtr qyid = tx->statement(QY_LAST_ROW_SQLITE3);

  return qyid->execute_int();
}

TestSuite::Ptr
Database::object(TestSuiteId id, Update::Flag update)
{
  return _object(*this, dbconn_, id, update, testSuites_);
}

Specimen::Ptr
Database::object(SpecimenId id, Update::Flag update)
{
  return _object(*this, dbconn_, id, update, specimens_);
}

TestCase::Ptr
Database::object(TestCaseId id, Update::Flag update)
{
  return _object(*this, dbconn_, id, update, testCases_);
}

TestSuiteId
_insertDBObject(Concolic::Database&, SqlTransactionPtr tx, TestSuite::Ptr obj)
{
  SqlStatementPtr stmt = tx->statement(QY_NEW_TESTSUITE);

  stmt->bind(0, obj->name());
  stmt->execute();

  return TestSuiteId(sqlLastRowId(tx));
}

SpecimenId
_insertDBObject(Concolic::Database&, SqlTransactionPtr tx, Specimen::Ptr obj)
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
  std::for_each(obj->args().begin(), obj->args().end(), CmdLineArgInserter(tx, tcid));
  std::for_each(obj->env().begin(),  obj->env().end(),  EnvVarInserter(tx, tcid));
}

void dependentObjUpdate(SqlTransactionPtr tx, int tcid, TestCase::Ptr obj)
{
  deleteTestCaseElems(tx, tcid, QY_RM_TESTCASE_CARG);
  deleteTestCaseElems(tx, tcid, QY_RM_TESTCASE_EVAR);

  dependentObjInsert(tx, tcid, obj);
}

TestCaseId
_insertDBObject(Concolic::Database& db, SqlTransactionPtr tx, TestCase::Ptr obj)
{
  SqlStatementPtr stmt = tx->statement(QY_NEW_TESTCASE);

  // \pp \todo does every testcase belong to the current testsuite?
  TestSuite::Ptr  suite = db.testSuite();
  const int       testsuiteId = db.id(suite, Concolic::Update::NO).get();
  const int       specimenId  = db.id(obj->specimen(), Concolic::Update::NO).get();

  stmt->bind(0, testsuiteId);
  stmt->bind(1, specimenId);
  stmt->bind(2, "linux");
  stmt->execute();

  const int       testcaseId  = sqlLastRowId(tx);

  dependentObjInsert(tx, testcaseId, obj);
  return TestCaseId(testcaseId);
}

template <class ObjPtr>
Concolic::ObjectId< typename ObjPtr::Pointee >
insertDBObject(Concolic::Database& db, SqlConnectionPtr dbconn, ObjPtr obj)
{
  typedef Concolic::ObjectId< typename ObjPtr::Pointee > ResultType;

  DBTxGuard  dbtx(dbconn);
  ResultType res = _insertDBObject(db, dbtx.tx(), obj);

  dbtx.commit();
  return res;
}

void
_updateDBObject(Concolic::Database& db, SqlTransactionPtr tx, TestSuite::Ptr obj)
{
  SqlStatementPtr stmt = tx->statement(QY_UPD_TESTSUITE);
  TestSuiteId     id   = db.id(obj);

  stmt->bind(0, obj->name());
  stmt->bind(1, id.get());
  stmt->execute();
}

void
_updateDBObject(Concolic::Database& db, SqlTransactionPtr tx, Specimen::Ptr obj)
{
  SqlStatementPtr stmt = tx->statement(QY_UPD_SPECIMEN);
  SpecimenId      id   = db.id(obj);

  stmt->bind(0, obj->name());
  stmt->bind(1, obj->content());
  stmt->bind(2, id.get());
  stmt->execute();
}

void
_updateDBObject(Concolic::Database& db, SqlTransactionPtr tx, TestCase::Ptr obj)
{
  throw_ex<std::runtime_error>("NOT IMPLEMENTED YET");
}


template <class ObjPtr>
void
updateDBObject(Concolic::Database& db, SqlConnectionPtr dbconn, ObjPtr obj)
{
  DBTxGuard dbtx(dbconn);

  _updateDBObject(db, dbtx.tx(), obj);
  dbtx.commit();
}


//
// Returns an ID number for an object, optionally writing to the database.

template <class ObjPtr, class BidirectionalMap>
static
typename BidirectionalMap::Reverse::Value
_id( Concolic::Database& db,
     SqlConnectionPtr dbconn,
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

    return insertDBObject(db, dbconn, obj);
  }

  if (Update::NO != update) updateDBObject(db, dbconn, obj);

  return pos.base()->second;
}


TestSuiteId
Database::id(const TestSuite::Ptr& pts, Update::Flag update)
{
  return _id(*this, dbconn_, pts, update, testSuites_);
}

SpecimenId
Database::id(const Specimen::Ptr& psp, Update::Flag update)
{
  return _id(*this, dbconn_, psp, update, specimens_);
}

TestCaseId
Database::id(const TestCase::Ptr& ptc, Update::Flag update)
{
  return _id(*this, dbconn_, ptc, update, testCases_);
}


Database::Ptr
Database::instance(const std::string &url)
{
    SqlDatabase::Driver dbdriver = SqlDatabase::Connection::guess_driver(url);
    SqlConnectionPtr    dbconn = SqlDatabase::Connection::create(url, dbdriver);
    Ptr                 db(new Database);

    db->dbconn_ = dbconn;
    db->testSuites();
    return db;
}

Database::Ptr
Database::create(const std::string &url, const std::string& testSuiteName)
{
    Ptr db = instance(url);

    // \pp \todo
    // db->testSuiteId_ = queryId(testSuiteName);
    return db;
}

bool
Database::rbaExists(Database::SpecimenId) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
Database::saveRbaFile(const boost::filesystem::path&, Database::SpecimenId) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
Database::extractRbaFile(const boost::filesystem::path&, Database::SpecimenId) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
Database::eraseRba(Database::SpecimenId) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

} // namespace
} // namespace
} // namespace
