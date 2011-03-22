#include <rose.h>
#include "sideEffect.h"
#include "SqliteDatabaseGraph.h"
#include "string_functions.h"

#include <iostream>
#include <cstring>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include "sqlite3x.h"

using namespace std;
using namespace sqlite3x;
using namespace StringUtility;

#include <boost/config.hpp>
#include <assert.h>
#include <iostream>

#include <vector>
#include <algorithm>
#include <utility>

#include "boost/graph/visitors.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/breadth_first_search.hpp"
#include "boost/graph/depth_first_search.hpp"

#include "boost/graph/strong_components.hpp"
#define SCOPE_NA    -1
#define SCOPE_PARAM  0

#define SCOPE_LOCAL  1
#define SCOPE_GLOBAL 2

#define MAXSTRINGSZ 512

#define DEBUG_OUTPUT 1
#undef DEBUG_OUTPUT

#define LMODFUNC   "__lmod"
#define LMODFORMAL "formal"

bool debugOut = false; // output information about traversal to stderr

#define SIMPLEFUNCTBL "simpleFuncTable"
#define VARNODETBL    "varNode"
#define LOCALVARTBL   "localVars"
#define FORMALTBL     "formals"
#define SIDEEFFECTTBL "sideEffect"
#define CALLEDGETBL   "callEdge"

class simpleFuncRow : public dbRow
{
  public:
    simpleFuncRow() : dbRow(SIMPLEFUNCTBL) {}

    simpleFuncRow(int project, string& name) : dbRow(SIMPLEFUNCTBL)
    {
      columnNames.push_back("id");
      columnNames.push_back("projectId");
      columnNames.push_back("functionName");

      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("text");

      pid = project;
      fname = name;
    }
    simpleFuncRow(sqlite3_reader& r) : dbRow(SIMPLEFUNCTBL) { load(r); }

    void load(sqlite3_reader& r)
    {
      rowid = r.getint(0);
      pid = r.getint(1);
      fname = r.getstring(2);
    }

    void insert(sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + string(SIMPLEFUNCTBL) + " WHERE projectId=? AND functionName=?;");
      selectcmd.bind(1,pid);
      selectcmd.bind(2,fname);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3_command insertcmd(*db,
          "INSERT INTO " + string(SIMPLEFUNCTBL) + " (projectId,functionName) VALUES (?,?);");
      insertcmd.bind(1,pid);
      insertcmd.bind(2,fname);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    string get_functionName() const { return fname; }

    int pid;
    string fname;
};

class varNodeRow : public dbRow
{
  public:
    varNodeRow() : dbRow(VARNODETBL) {}

    varNodeRow(int project, string function, string var ) : dbRow(VARNODETBL)
    {
      columnNames.push_back("id");
      columnNames.push_back("projectId");
      columnNames.push_back("functionName");
      columnNames.push_back("varName");

      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("text");
      columnTypes.push_back("text");

      pid = project;
      fname = function;
      varname = var;
    }
    varNodeRow(sqlite3_reader& r) : dbRow(VARNODETBL) { load(r); }

    void load(sqlite3_reader& r)
    {
      rowid = r.getint(0);
      pid = r.getint(1);
      fname = r.getstring(2);
      varname = r.getstring(3);
    }

    void insert(sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + string(VARNODETBL) + " WHERE projectId=? AND functionName=? AND varName=?;");
      selectcmd.bind(1,pid);
      selectcmd.bind(2,fname);
      selectcmd.bind(3,varname);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3_command insertcmd(*db,
          "INSERT INTO " + string(VARNODETBL) + " (projectId,functionName,varName) VALUES (?,?,?);");
      insertcmd.bind(1,pid);
      insertcmd.bind(2,fname);
      insertcmd.bind(3,varname);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    string get_functionName() const { return fname; }
    string get_varName() const { return varname; }

    int pid;
    string fname;
    string varname;
};

class localVarRow : public dbRow
{
  public:
    localVarRow(int project, string function, string name) : dbRow(LOCALVARTBL)
    {
      columnNames.push_back("id");
      columnNames.push_back("projectId");
      columnNames.push_back("functionName");
      columnNames.push_back("varName");

      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("text");
      columnTypes.push_back("text");

      pid = project;
      fname = function;
      varname = name;
    }
    localVarRow(sqlite3_reader& r) : dbRow(LOCALVARTBL) { load(r); }

    void load(sqlite3_reader& r)
    {
      rowid = r.getint(0);
      pid = r.getint(1);
      fname = r.getstring(2);
      varname = r.getstring(3);
    }

    void insert(sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + string(LOCALVARTBL) + " WHERE projectId=? AND functionName=? AND varName=?;");
      selectcmd.bind(1,pid);
      selectcmd.bind(2,fname);
      selectcmd.bind(3,varname);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3_command insertcmd(*db,
          "INSERT INTO " + string(LOCALVARTBL) + " (projectId,functionName,varName) VALUES (?,?,?);");
      insertcmd.bind(1,pid);
      insertcmd.bind(2,fname);
      insertcmd.bind(3,varname);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    string get_functionName() const { return fname; }
    string get_varName() const { return varname; }

    int pid;
    string fname;
    string varname;
};

class formalRow : public dbRow
{
  public:
    formalRow(int project, string function, string formalname, int ord) : dbRow(FORMALTBL)
    {
      columnNames.push_back("id");
      columnNames.push_back("projectId");
      columnNames.push_back("functionName");
      columnNames.push_back("formal");
      columnNames.push_back("ordinal");

      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("text");
      columnTypes.push_back("text");
      columnTypes.push_back("integer");

      pid = project;
      fname = function;
      formal = formalname;
      ordinal = ord;
    }
    formalRow(sqlite3_reader& r) : dbRow(FORMALTBL) { load(r); }

    void load(sqlite3_reader& r)
    {
      rowid = r.getint(0);
      pid = r.getint(1);
      fname = r.getstring(2);
      formal = r.getstring(3);
      ordinal = r.getint(4);
    }

    void insert(sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + string(FORMALTBL) + " WHERE projectId=? AND functionName=? AND formal=? AND ordinal=?;");
      selectcmd.bind(1,pid);
      selectcmd.bind(2,fname);
      selectcmd.bind(3,formal);
      selectcmd.bind(4,ordinal);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3_command insertcmd(*db,
          "INSERT INTO " + string(FORMALTBL) + " (projectId,functionName,formal,ordinal) VALUES (?,?,?,?);");
      insertcmd.bind(1,pid);
      insertcmd.bind(2,fname);
      insertcmd.bind(3,formal);
      insertcmd.bind(4,ordinal);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    string get_functionName() const { return fname; }
    string get_formal() const { return formal; }
    int get_ordinal() const { return ordinal; }

    int pid;
    string fname;
    string formal;
    int ordinal;
};

class sideEffectRow : public dbRow
{
  public:
    sideEffectRow(int project, string function, int numargs) : dbRow(SIDEEFFECTTBL)
    {
      columnNames.push_back("id");
      columnNames.push_back("projectId");
      columnNames.push_back("functionName");
      columnNames.push_back("arg");

      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("text");
      columnTypes.push_back("integer");

      pid = project;
      fname = function;
      args = numargs;
    }
    sideEffectRow(sqlite3_reader& r) : dbRow(SIDEEFFECTTBL) { load(r); }

    void load(sqlite3_reader& r)
    {
      rowid = r.getint(0);
      pid = r.getint(1);
      fname = r.getstring(2);
      args = r.getint(3);
    }

    void insert(sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + string(SIDEEFFECTTBL) + " WHERE projectId=? AND functionName=? AND arg=?;");
      selectcmd.bind(1,pid);
      selectcmd.bind(2,fname);
      selectcmd.bind(3,args);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3_command insertcmd(*db,
          "INSERT INTO " + string(SIDEEFFECTTBL) + " (projectId,functionName,arg) VALUES (?,?,?);");
      insertcmd.bind(1,pid);
      insertcmd.bind(2,fname);
      insertcmd.bind(3,args);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    int get_arg() const { return args; }

    int pid;
    string fname;
    int args;
};

class callEdgeRow : public dbRow
{
  public:
    callEdgeRow() : dbRow(CALLEDGETBL) { empty = true; }

    callEdgeRow(int project, string callsite, string actualname, int callscope, int ord) : dbRow(CALLEDGETBL)
    {
      columnNames.push_back("id");
      columnNames.push_back("projectId");
      columnNames.push_back("site");
      columnNames.push_back("actual");
      columnNames.push_back("scope");
      columnNames.push_back("ordinal");

      columnTypes.push_back("integer");
      columnTypes.push_back("integer");
      columnTypes.push_back("text");
      columnTypes.push_back("text");
      columnTypes.push_back("integer");
      columnTypes.push_back("integer");

      pid = project;
      site = callsite;
      actual = actualname;
      scope = callscope;
      ordinal = ord;
      empty = false;
    }
    callEdgeRow(sqlite3_reader& r) : dbRow(CALLEDGETBL) { load(r); }

    void load(sqlite3_reader& r)
    {
      rowid = r.getint(0);
      pid = r.getint(1);
      site = r.getstring(2);
      actual = r.getstring(3);
      scope = r.getint(4);
      ordinal = r.getint(5);
      empty = empty;
    }

    void insert(sqlite3_connection* db)
    {
      sqlite3x::sqlite3_command selectcmd(*db,
          "SELECT * FROM " + string(CALLEDGETBL) + " WHERE projectId=? AND site=? AND actual=? AND scope=? AND ordinal=?;");
      selectcmd.bind(1,pid);
      selectcmd.bind(2,site);
      selectcmd.bind(3,actual);
      selectcmd.bind(4,scope);
      selectcmd.bind(5,ordinal);

      sqlite3x::sqlite3_reader r = selectcmd.executereader();
      if( r.read() ) {
        rowid = r.getint(0);
        return;
      }

      sqlite3_command insertcmd(*db,
          "INSERT INTO " + string(CALLEDGETBL) + " (projectId,site,actual,scope,ordinal) VALUES (?,?,?,?,?);");
      insertcmd.bind(1,pid);
      insertcmd.bind(2,site);
      insertcmd.bind(3,actual);
      insertcmd.bind(4,scope);
      insertcmd.bind(5,ordinal);
      insertcmd.executenonquery();

      rowid = db->insertid();
    }

    string get_site() const { return site; }
    string get_actual() const { return actual; }
    int get_scope() const { return scope; }
    int get_ordinal() const { return ordinal; }
    bool isEmpty() const { return empty; }

    int pid;
    string site;
    string actual;
    int scope;
    int ordinal;
    bool empty;
};


typedef DatabaseGraph<simpleFuncRow, callEdgeRow,
                     boost::vecS, boost::vecS, boost::bidirectionalS> CallGraph;
typedef CallGraph::dbgType Graph;
typedef boost::graph_traits < CallGraph >::vertex_descriptor callVertex;
typedef boost::property_map<CallGraph, boost::vertex_index_t>::const_type callVertexIndexMap;

typedef DatabaseGraph<varNodeRow, callEdgeRow,
                     boost::vecS, boost::vecS, boost::bidirectionalS> CallMultiGraph;


// milki (06/16/2010) Redefine for std::string to produce
// deterministic behavior in map_type
struct eqstdstr
{
  bool operator()(string s1, string s2) const
  {
 // Return true if s1 == s2, else return false.
    return s1.compare(s2) == 0;
  }
};
#if 0
// DQ (12/1/2010): Changed the name to support using local version since the 
// version is src/ROSETTA/Grammar/Support.code has been extended to support 
// case sensitive/insensitive semantics.  I hope that is not required here!
// milki (06/16/2010) Name clash in src/ROSETTA/Grammar/Support.code:325
struct local_eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};
#endif

struct eqint
{
  bool operator()(int n1, int n2) const
  {
    return n1 == n2;
  }
};

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const 
  {
    return strcmp(s1, s2) < 0;
  }
};

struct pairltstr
{
  bool operator()(pair<const char *, const char *> s1,
		  pair<const char *, const char *> s2)
  {
    return ( ( strcmp(s1.first, s2.first) < 0 ) &&
             ( strcmp(s1.second, s2.second) < 0 ) );
  }
};

// boost::unordered_multimap<Key, Data, HashFcn, EqualKey, Alloc>
//typedef boost::unordered_multimap<const char*, const char *, boost::hash<const char*>, eqstr> map_type;
typedef boost::unordered_multimap<string, const char*, boost::hash<string>, eqstdstr> map_type;

// boost::unordered_map<Key, Data, HashFcn, EqualKey, Alloc>

#if 0
// milki (07/08/2010) Convert map keys to std::string
//typedef boost::unordered_map<const char *, int, boost::hash<const char *>, eqstr> str_int_map;
//typedef boost::unordered_map<const char *, const char *, boost::hash<const char *>, eqstr> str_str_map;
//typedef boost::unordered_map<const char *, str_int_map *, boost::hash<const char *>, eqstr> str_map_map;
typedef boost::unordered_map<string, int, boost::hash<string>, local_eqstr> str_int_map;
typedef boost::unordered_map<string, const char *, boost::hash<string>, local_eqstr> str_str_map;
typedef boost::unordered_map<string, str_int_map *, boost::hash<string>, local_eqstr> str_map_map;

typedef boost::unordered_map<int, const char *, boost::hash<int>, eqint> int_str_map;
//typedef boost::unordered_map<const char *, int_str_map *, boost::hash<const char *>, eqstr> int_map_map;
typedef boost::unordered_map<string, int_str_map *, boost::hash<string>, local_eqstr> int_map_map;
#else
// DQ (12/1/2010): Using eqstdstr in place of eqstr or local_eqstr.
typedef boost::unordered_map<string, int, boost::hash<string>, eqstdstr> str_int_map;
typedef boost::unordered_map<string, const char *, boost::hash<string>, eqstdstr> str_str_map;
typedef boost::unordered_map<string, str_int_map *, boost::hash<string>, eqstdstr> str_map_map;
typedef boost::unordered_map<int, const char *, boost::hash<int>, eqint> int_str_map;
typedef boost::unordered_map<string, int_str_map *, boost::hash<string>, eqstdstr> int_map_map;
#endif

void prettyprint( map_type M ) {
  for( map_type::const_iterator it = M.cbegin() ; it != M.cend() ; it++ )
  {
    cout << "<" << (*it).first << "," << (*it).second << ">" << endl;
  }
}

// Database Table Initialization
bool createDatabaseTables(sqlite3_connection& con, bool drop) {

  try {
    if( drop) {
      con.executenonquery("DROP TABLE IF EXISTS localVars");
      con.executenonquery("DROP TABLE IF EXISTS simpleFuncTable");
      con.executenonquery("DROP TABLE IF EXISTS varNode");
      con.executenonquery("DROP TABLE IF EXISTS formals");
      con.executenonquery("DROP TABLE IF EXISTS sideEffect");
      con.executenonquery("DROP TABLE IF EXISTS callEdge");
    }
    con.executenonquery("CREATE TABLE IF NOT EXISTS localVars(id INTEGER PRIMARY KEY, projectId INTEGER, functionName TEXT, varName TEXT)");
    con.executenonquery("CREATE TABLE IF NOT EXISTS simpleFuncTable(id INTEGER PRIMARY KEY, projectId INTEGER, functionName TEXT)");
    con.executenonquery("CREATE TABLE IF NOT EXISTS varNode(id INTEGER PRIMARY KEY, projectId INTEGER, functionName TEXT, varName TEXT)");
    con.executenonquery("CREATE TABLE IF NOT EXISTS formals(id INTEGER PRIMARY KEY, projectId INTEGER, functionName TEXT, formal TEXT, ordinal INTEGER)");
    con.executenonquery("CREATE TABLE IF NOT EXISTS sideEffect(id INTEGER PRIMARY KEY, projectId INTEGER, functionName TEXT, arg INTEGER)");
    con.executenonquery("CREATE TABLE IF NOT EXISTS callEdge(id INTEGER PRIMARY KEY, projectId INTEGER, site TEXT, actual TEXT, scope INTEGER, ordinal INTEGER);");

    // DatabaseGraph tables
    define_schema(con);
  } catch( exception &ex ) {
    cerr << "Exception Occured: " << ex.what() << endl;
    return false;
  }
  return true;
}

class SideEffect : public SideEffectAnalysis {

 public:

  SideEffect();

  void setCallRoot(callVertex v) { mCallRoot = v; mCallRootSet = true; }

  bool getCallRootSet() const { return mCallRootSet; }

  void setCallRootName(string root) { mCallRootName = root; }

  string getCallRootName() const { return mCallRootName; }

  int calcSideEffect(SgProject& project);
#if 1
  int calcSideEffect(SgFile& file);

  int calcSideEffect(SgNode& node);
#endif

  list<const char* > getCalledFunctions();

  list<const char* > getGMOD(const char* func);

  list<const char* > getDMOD(const char* stmt);

  string getNodeIdentifier(SgNode *node);

  // private:

  void populateLocalVarsFromDB(sqlite3_connection *db);

  void populateFormalsFromDB(sqlite3_connection *db);

  CallMultiGraph * createMultiGraph(CallGraph *callgraph, long projectId, 
				    sqlite3_connection *db);

  int doSideEffect(list<SgNode*> *nodeList, list<string> &sourceFileNames,
                   string &sanitizedOutputFileName,
		   vector<string> &nodeListFileNames);

  //  string getQualifiedFunctionName(SgFunctionCallExp *astNode);

  void solveGMOD(CallGraph *g);

  int onStack(int num);
  
  void searchGMOD(CallGraph *g, string pName, callVertex p, 
		  callVertexIndexMap index_map);

  void solveRMOD(CallMultiGraph *multigraph, long projectId, 
                 sqlite3_connection *db);

  int nextdfn;
  deque<callVertex> vertexStack;
  
  // CH (4/9/2010): Use boost::unordered_set instead
  //hash_set<int, boost::hash<int>, eqint> shadowStack;
  boost::unordered_set<int, boost::hash<int>, eqint> shadowStack;
  
  int *lowlink;
  int *dfn;

  int insertFormal(const char *func, const char *formal, int num);

  int insertFormalByPos(const char *func, const char *formal, int num);

  int lookupFormal(const char *func, const char *formal);

  const char *lookupFormalByPos(const char *func, int num);

  long insertFuncToId(const char *func, long id);

  int insertFuncToFile(const char *func, const char *file);

  long lookupIdByFunc(const char *func);

  const char *lookupFileByFunc(const char *func);

  pair<map_type::const_iterator, map_type::const_iterator> lookupLocal(const char *func);

  pair<map_type::const_iterator, map_type::const_iterator> lookupIMOD(const char *func);

  pair<map_type::const_iterator, map_type::const_iterator> lookupIMODRep(const char *func);

  pair<map_type::const_iterator, map_type::const_iterator> lookupIMODPlus(const char *func);

  pair<map_type::const_iterator, map_type::const_iterator> lookupRMOD(const char *func);

  pair<map_type::const_iterator, map_type::const_iterator> lookupGMOD(const char *func);

  pair<map_type::const_iterator, map_type::const_iterator> lookupLMOD(const char *stmt);

  pair<map_type::const_iterator, map_type::const_iterator> lookupDMOD(const char *stmt);

  int insertLocal(const char *func, const char *var);

  int insertIMOD(const char *func, const char *var);

  int insertIMODRep(const char *func, const char *var);

  int insertIMODPlus(const char *func, const char *var);

  int insertRMOD(const char *func, const char *var);

  int insertGMOD(const char *func, const char *var);

  int insertLMOD(const char *stmt, const char *var);

  int insertDMOD(const char *stmt, const char *var);

  long nextId;
  long nextAggCallGraphId;
  bool separateCompilation;

  // NB:  locals seems to be both variables declared within a function and
  //      those passed as parameters
  map_type    locals;
  map_type    imod;
  map_type    imodrep;
  map_type    imodplus;
  map_type    rmod;
  map_type    gmod;
  map_type    lmod;
  map_type    dmod;
  
  str_map_map formals;
  int_map_map formalsByPos;
  
  str_int_map funcToId;
  
  str_str_map funcToFile;

  callVertex mCallRoot;
  string     mCallRootName;
  bool       mCallRootSet;

  set<const char *, ltstr> mCalledFuncs;

};

// implement the stubs for the base class

SideEffectAnalysis *
SideEffectAnalysis::create() 
{
  return new SideEffect();
}

SideEffectAnalysis::SideEffectAnalysis() { }

SideEffectAnalysis::~SideEffectAnalysis() { }

list<const char* > 
SideEffect::getCalledFunctions()
{
  list<const char* > calledFuncs;

  typedef set<const char *, ltstr>::iterator funcIter;
  funcIter setIt;
  
  for (setIt = this->mCalledFuncs.begin();
       setIt != this->mCalledFuncs.end();
       setIt++) {
    calledFuncs.push_back(*setIt);
  }
  calledFuncs.push_back(getCallRootName().c_str());

  return calledFuncs;
}

list<const char* > 
SideEffect::getGMOD(const char* func)
{
  list<const char* > sideEffects;

  pair<map_type::const_iterator, map_type::const_iterator> p;

  p = lookupGMOD(func);

  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    sideEffects.push_back((*i).second);

  return sideEffects;
}

list<const char* > 
SideEffect::getDMOD(const char* stmt)
{
  list<const char* > sideEffects;

  pair<map_type::const_iterator, map_type::const_iterator> p;

  p = lookupDMOD(stmt);

  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    sideEffects.push_back((*i).second);

  return sideEffects;
}


string
SideEffect::getNodeIdentifier(SgNode *node)
{
  assert(node);
  assert(node->get_file_info());

  Sg_File_Info* nodeinfo = node->get_file_info();

  // milki (07/07/2010) Revert back to name/line/col/variant format

  string fileName = node->get_file_info()->get_filenameString();
  fileName = stripPathFromFileName(fileName);

  char tmp[strlen(node->get_file_info()->get_filename()) + 512];
  //sprintf(tmp, "%s-%ld", fileName.c_str(), (long)node);
  sprintf(tmp, "%s-%d-%d-%d", fileName.c_str(),
      nodeinfo->get_line(), nodeinfo->get_col(),
      node->variantT());

  string id = tmp;

  return id;
}

// return a function declaration AST node given a function call AST node.
// handles function calls, method calls, and method calls through pointers.
SgFunctionDeclaration *
getFunctionDeclaration(SgFunctionCallExp *astNode)
{
  SgFunctionDeclaration *funcDec = NULL;
  SgFunctionRefExp *funcRef = isSgFunctionRefExp(((SgFunctionCallExp *)astNode)->get_function());
  
  // see if its a member function call
  SgDotExp *funcDotExp = isSgDotExp(((SgFunctionCallExp *)astNode)->get_function());
  
  // see if it is a member function invocation from an object pointer
  SgArrowExp *funcArrowExp = isSgArrowExp(((SgFunctionCallExp *)astNode)->get_function());
  if(funcArrowExp) {
    SgMemberFunctionRefExp *membFunc = isSgMemberFunctionRefExp(funcArrowExp->get_rhs_operand());
    funcDec = membFunc->get_symbol_i()->get_declaration();
    
  } else if(funcDotExp) {
    SgMemberFunctionRefExp *membFunc = isSgMemberFunctionRefExp(funcDotExp->get_rhs_operand());
    funcDec = membFunc->get_symbol_i()->get_declaration();
    
  } else if(funcRef) {
    // found a standard function reference 
    funcDec = funcRef->get_symbol_i()->get_declaration();
  }

  return funcDec;
}

// return a fully-qualified function name given a function declaration AST node
string
getQualifiedFunctionName(SgFunctionDeclaration *astNode)
{
  string funcName;

  SgName qualifiedName;

  SgMemberFunctionDeclaration *methodDec = 
    isSgMemberFunctionDeclaration(astNode);

  if (methodDec) {
    qualifiedName = methodDec->get_qualified_name();
  } else {
    qualifiedName = astNode->get_name();
  }
  funcName.assign(qualifiedName.str());

  return funcName;
}

string
//SideEffect::
getQualifiedFunctionName(SgFunctionCallExp *astNode)
{

  SgFunctionDeclaration *funcDec = getFunctionDeclaration(astNode);

  string funcName = getQualifiedFunctionName(funcDec);
#if 0  
  SgFunctionDeclaration *funcDec = NULL;
  SgFunctionRefExp *funcRef = isSgFunctionRefExp(((SgFunctionCallExp *)astNode)->get_function());
  
  // see if its a member function call
  SgDotExp *funcDotExp = isSgDotExp(((SgFunctionCallExp *)astNode)->get_function());
  
  // see if it is a member function invocation from an object pointer
  SgArrowExp *funcArrowExp = isSgArrowExp(((SgFunctionCallExp *)astNode)->get_function());
  if(funcArrowExp) {
    if(funcArrowExp->get_traversalSuccessorContainer().size()>=2) {
      SgMemberFunctionRefExp *membFunc = isSgMemberFunctionRefExp(funcArrowExp->get_traversalSuccessorContainer()[1]);
      funcDec = membFunc->get_symbol_i()->get_declaration();
      
      SgMemberFunctionDeclaration *methodDec = 
	isSgMemberFunctionDeclaration(funcDec);
      assert(methodDec != NULL);
      
      SgName qualifiedName = methodDec->get_qualified_name();
      funcName.assign(qualifiedName.str());
#if 0
      const char *className = qualifiedName.getString().c_str();
      funcName = new char[strlen(className) + 1];
      strcpy(funcName, className);
#endif      
    }
    
  } else if(funcDotExp) {
    if(funcDotExp->get_traversalSuccessorContainer().size()>=2) {
      SgMemberFunctionRefExp *membFunc = isSgMemberFunctionRefExp(funcDotExp->get_traversalSuccessorContainer()[1]);
      funcDec = membFunc->get_symbol_i()->get_declaration();
      
      SgMemberFunctionDeclaration *methodDec = 
	isSgMemberFunctionDeclaration(funcDec);
      assert(methodDec != NULL);
      
      SgName qualifiedName = methodDec->get_qualified_name();
      funcName.assign(qualifiedName.str());
#if 0
      const char *className = qualifiedName.getString().c_str();
      funcName = new char[strlen(className) + 1];
      strcpy(funcName, className);
#endif      
    }
    
  } else if(funcRef) {
    // found a standard function reference 
    funcDec = funcRef->get_symbol_i()->get_declaration();
    
    funcName.assign(funcRef->get_symbol_i()->get_name().str());
#if 0
    funcName = new char[strlen(funcRef->get_symbol_i()->get_name().str()) + 1];
    strcpy(funcName, funcRef->get_symbol_i()->get_name().str());
#endif
  }
#endif
  
  return funcName;
}

// -------------------------------------------------------------
// Utility functions on map, etc. data structures
// -------------------------------------------------------------

template <class Map, class Key>
//pair<typename Map::const_iterator, typename Map::const_iterator> lookup(const Map& M, Key func)
pair<typename Map::const_iterator, typename Map::const_iterator> lookup(Map& M, Key func)
{
  pair<typename Map::const_iterator, typename Map::const_iterator> p;

  p = M.equal_range(func);
#ifdef DEBUG_OUTPUT
  if ( p.first != p.second ) {
  cout << "For " << func << " found <" << (*p.first).first << "," << (*p.first).second << ">" << endl;
  }
#endif


  return p;
}

// returns 0 if already present
int insert(map_type &M, const char* func, const char* var)
{
  // milki (07/06/2010) Use a string func key
  string myfunc;
  char* myvar;

  pair<map_type::const_iterator, map_type::const_iterator> p = lookup(M, func);
  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    if (!strcmp((*i).second, var))
      return 0;

  myfunc = string(func);

  myvar = new char[(strlen(var) + 1)];
  strcpy(myvar, var);

  map_type::const_iterator it = M.insert(map_type::value_type(myfunc, myvar));
#ifdef DEBUG_OUTPUT
  cout << "Inserted <" << (*it).first << "," << (*it).second << ">" << endl;
#endif

  return 1;
}

SideEffect::SideEffect() :
  nextId(0), nextAggCallGraphId(0), separateCompilation(true),
  mCallRootSet(false)
{
 
}

int 
SideEffect::insertFormal(const char *func, const char *formal, int num)
{
  // milki (07/06/2010) Use a string func key
  string myfunc;
  char* myformal;
  str_int_map *m;

  pair<str_map_map::const_iterator, str_map_map::const_iterator> p = 
    lookup(formals, func);
  
  if (p.first == p.second) {
    // insert a map for this function
    m = new str_int_map();

    myfunc = string(func);

    formals.insert(str_map_map::value_type(myfunc, m));
  } else {

    str_map_map::const_iterator i = p.first; 
    m = (*i).second;
    ++i;
    assert(i == p.second);
    
  }

  pair<str_int_map::const_iterator, str_int_map::const_iterator> p2 = 
    lookup(*m, formal);

  if (p2.first != p2.second) {
    
    str_int_map::const_iterator i2 = p2.first;
    assert(num == (*i2).second);
    ++i2;
    assert(i2 == p2.second);
    return 0;

  }

  myformal = new char[(strlen(formal) + 1)];
  strcpy(myformal, formal);

#ifdef DEBUG_OUTPUT
  cout << "INSERTING formal " << myformal << " at " << num << " for "
    << func << endl;
#endif
  m->insert(str_int_map::value_type(myformal, num));

  return 1;
}

int 
SideEffect::insertFormalByPos(const char *func, const char *formal, int num)
{
  // milki (07/06/2010) Use a string func key
  string myfunc;
  char* myformal;
  int_str_map *m;

  pair<int_map_map::const_iterator, int_map_map::const_iterator> p = 
    lookup(formalsByPos, func);
  
  if (p.first == p.second) {
    // insert a map for this function
    m = new int_str_map();

    myfunc = string(func);

    formalsByPos.insert(int_map_map::value_type(myfunc, m));
  } else {

    int_map_map::const_iterator i = p.first; 
    m = (*i).second;
    ++i;
    assert(i == p.second);
    
  }

  pair<int_str_map::const_iterator, int_str_map::const_iterator> p2 = 
    lookup(*m, num);

  if (p2.first != p2.second) {
    
    int_str_map::const_iterator i2 = p2.first;
    assert(!strcmp(formal, (*i2).second));
    ++i2;
    assert(i2 == p2.second);
    return 0;

  }

  myformal = new char[(strlen(formal) + 1)];
  strcpy(myformal, formal);

#ifdef DEBUG_OUTPUT
  cout << "INSERTING formalbypos " << myformal << " at " << num << " for "
    << func << endl;
#endif

  m->insert(int_str_map::value_type(num, myformal));

  return 1;
}

int 
SideEffect::lookupFormal(const char *func, const char *formal)
{
  str_int_map *m;
  int num;

  pair<str_map_map::const_iterator, str_map_map::const_iterator> p = 
    lookup(formals, func);
  
  if (p.first == p.second) {
    return -1;
  } 

  str_map_map::const_iterator i = p.first; 
  m = (*i).second;
  ++i;
  assert(i == p.second);

  pair<str_int_map::const_iterator, str_int_map::const_iterator> p2 = 
    lookup(*m, formal);

  if (p2.first != p2.second) {
    
    str_int_map::const_iterator i2 = p2.first;
    num = (*i2).second;
    ++i2;
    assert(i2 == p2.second);

#ifdef DEBUG_OUTPUT
    cout << "For formal " << func << " found pos " << num << endl;
#endif
    return num;

  }

  return -1;
}

const char *
SideEffect::lookupFormalByPos(const char *func, int num)
{
  int_str_map *m;
  const char *formal;

  pair<int_map_map::const_iterator, int_map_map::const_iterator> p = 
    lookup(formalsByPos, func);
  
  if (p.first == p.second) {
    return NULL;
  } 

  int_map_map::const_iterator i = p.first; 
  m = (*i).second;
  ++i;
  assert(i == p.second);

  pair<int_str_map::const_iterator, int_str_map::const_iterator> p2 = 
    lookup(*m, num);

  if (p2.first != p2.second) {
    
    int_str_map::const_iterator i2 = p2.first;
    formal = (*i2).second;
    ++i2;
    assert(i2 == p2.second);

#ifdef DEBUG_OUTPUT
    cout << "For formalbypos " << num << " found " << formal << endl;
#endif
    return formal;

  }

  return NULL;
}

long 
SideEffect::insertFuncToId(const char *func, long id)
{
  long ret;

  pair<str_int_map::const_iterator, str_int_map::const_iterator> p = 
    lookup(funcToId, func);

  if (p.first != p.second) {
    
    str_int_map::const_iterator i = p.first;
    ret = (*i).second;
    // assert(id == (*i).second);
    ++i;
    assert(i == p.second);
    return ret;

  }

  char *myfunc = new char[(strlen(func) + 1)];
  strcpy(myfunc, func);

  funcToId.insert(str_int_map::value_type(myfunc, id));

  return id;

}

int 
SideEffect::insertFuncToFile(const char *func, const char *file)
{

  int same;

  // milki (07/06/2010) Use a string file key
  pair<str_str_map::const_iterator, str_str_map::const_iterator> p = 
    lookup(funcToFile, string(file));

  if (p.first != p.second) {
    
    str_str_map::const_iterator i = p.first;
    same = !strcmp(file, (*i).second);
    ++i;
    assert(i == p.second);
    return same;

  }

  // milki (07/06/2010) Use a string func key
  string myfunc = func;

  char *myfile = new char[(strlen(file) + 1)];
  strcpy(myfile, file);

  funcToFile.insert(str_str_map::value_type(myfunc, myfile));

  return 1;

}

long 
SideEffect::lookupIdByFunc(const char *func)
{

  long ret;

  pair<str_int_map::const_iterator, str_int_map::const_iterator> p = 
    lookup(funcToId, func);

  if (p.first != p.second) {
    
    str_int_map::const_iterator i = p.first;
    ret = (*i).second;
    // assert(id == (*i).second);
    ++i;
    assert(i == p.second);
    return ret;

  }

  return -1;

}

const char *
SideEffect::lookupFileByFunc(const char *func)
{

  pair<str_str_map::const_iterator, str_str_map::const_iterator> p = 
    lookup(funcToFile, func);

  if (p.first == p.second) 
    return NULL;

  str_str_map::const_iterator i = p.first;
  
  const char *file = (*i).second;
  ++i;
  assert(i == p.second);

  return file;

}

pair<map_type::const_iterator, map_type::const_iterator> 
SideEffect::lookupLocal(const char *func)
{
  return lookup(locals, func);
}

pair<map_type::const_iterator, map_type::const_iterator> 
SideEffect::lookupIMOD(const char *func)
{
  return lookup(imod, func);
}

pair<map_type::const_iterator, map_type::const_iterator> 
SideEffect::lookupIMODRep(const char *func)
{
  return lookup(imodrep, func);
}

pair<map_type::const_iterator, map_type::const_iterator> 
SideEffect::lookupIMODPlus(const char *func)
{
  return lookup(imodplus, func);
}

pair<map_type::const_iterator, map_type::const_iterator> 
SideEffect::lookupRMOD(const char *func)
{
  return lookup(rmod, func);
}

pair<map_type::const_iterator, map_type::const_iterator> 
SideEffect::lookupGMOD(const char *func)
{
  return lookup(gmod, func);
}

pair<map_type::const_iterator, map_type::const_iterator> 
SideEffect::lookupLMOD(const char *stmt)
{
  return lookup(lmod, stmt);
}

pair<map_type::const_iterator, map_type::const_iterator> 
SideEffect::lookupDMOD(const char *stmt)
{
  return lookup(dmod, stmt);
}

int 
SideEffect::insertLocal(const char *func, const char *var)
{
  return insert(locals, func, var);
}

int 
SideEffect::insertIMOD(const char *func, const char *var)
{
  return insert(imod, func, var);
}

int 
SideEffect::insertIMODRep(const char *func, const char *var)
{
  return insert(imodrep, func, var);
}

int 
SideEffect::insertIMODPlus(const char *func, const char *var)
{
  return insert(imodplus, func, var);
}

int 
SideEffect::insertRMOD(const char *func, const char *var)
{
  return insert(rmod, func, var);
}

int 
SideEffect::insertGMOD(const char *func, const char *var)
{
  return insert(gmod, func, var);
}

int 
SideEffect::insertLMOD(const char *stmt, const char *var)
{
  return insert(lmod, stmt, var);
}

int 
SideEffect::insertDMOD(const char *stmt, const char *var)
{
  return insert(dmod, stmt, var);
}


//-----------------------------------------------------------------------------
// define traversal classes 

//#include "config.h"
//#include <rose.h>

#define UNSET -1

// Build an inherited attribute storing the current function defintion scope
class MyInheritedAttribute
{
 public:

  // Constructors

  MyInheritedAttribute() :
    mFunctionDefinitionId(UNSET), 
    mParent(NULL), 
    mParentName(NULL),
    mAncestorOrSelfIsLHS(false),
    mAssignOp(NULL) { }

  MyInheritedAttribute ( SgNode* astNode ) :
    mFunctionDefinitionId(UNSET), 
    mParent(NULL), 
    mParentName(NULL),
    mAncestorOrSelfIsLHS(false),
    mAssignOp(NULL) { }

  MyInheritedAttribute(const MyInheritedAttribute &X) :
    mFunctionDefinitionId(X.mFunctionDefinitionId),
    mParent(X.mParent),
    mParentName(X.mParentName),
    mAncestorOrSelfIsLHS(X.mAncestorOrSelfIsLHS),
    mAssignOp(X.mAssignOp) { }

  // Accessor functions
  
  void setFunctionDefinitionId(int set) { mFunctionDefinitionId = set; }
  int getFunctionDefinitionId() const   { return mFunctionDefinitionId; }
  
  void setParentFunction(SgFunctionDeclaration *p) { mParent = p; }
  SgFunctionDeclaration *getParentFunction() { return mParent; }

  void setParentName(const char *name) { mParentName = name; }
  const char *getParentName() const { return mParentName; }
  
  void setLHS(bool set) { mAncestorOrSelfIsLHS = set; }
  bool getLHS() const   { return mAncestorOrSelfIsLHS; }
  
  void setAssignOp(SgNode* astNode) { mAssignOp = astNode; }
  SgNode *getAssignOp() const       { return mAssignOp; }
  
 protected:
  
  // id of the current function definition scope
  int mFunctionDefinitionId;
  
  // parent function
  SgFunctionDeclaration *mParent;

  const char *mParentName;
  
  // mark that a parent was a lhs
  bool mAncestorOrSelfIsLHS;
  
  // hold on to expression that represents destructive assign
  SgNode *mAssignOp;

};

class MyTraversal
: public SgTopDownProcessing<MyInheritedAttribute>
{
 public:
  MyTraversal() 
    : SgTopDownProcessing<MyInheritedAttribute>(),
      mGDB(NULL),
      mProjectId(-1),
      mCallgraph(NULL),
      mSimpleCallgraph(NULL),
      mDefinedFuncs(NULL),
      mCalledFuncs(NULL),
      mFoundLHS(false),
      mSideEffectPtr(NULL)
  { };
  
  MyTraversal (int setPID,
               sqlite3_connection *setGdb,
	       CallGraph *setGraph,
	       CallGraph *setSimpleGraph,
	       set<const char *, ltstr> *setDefinedFuncs,
	       set<const char *, ltstr> *setCalledFuncs,
	       SideEffect *setSideEffectPtr) 
    : SgTopDownProcessing<MyInheritedAttribute>(),
      mGDB(setGdb), 
      mProjectId(setPID), 
      mCallgraph(setGraph), 
      mSimpleCallgraph(setSimpleGraph),
      mDefinedFuncs(setDefinedFuncs), 
      mCalledFuncs(setCalledFuncs),
      mFoundLHS(false),
      mSideEffectPtr(setSideEffectPtr)
  { 
    
    // sanity check arguments
    assert(setGdb != NULL);
    assert(setGraph != NULL);
    assert(setSideEffectPtr != NULL);
    if (!mSideEffectPtr->separateCompilation) assert(setSimpleGraph != NULL);
    assert(setDefinedFuncs != NULL);
    assert(setCalledFuncs != NULL);
    
    // create a dummy function called __lmod, which modifies its only argument.
    // call __lmod's sole argument 'formal' in the maps below.
    // we convert any assignments to function invocations on __lmod, letting us
    // treat (modifying) assignments the same way we treat function calls with
    // side effects.  

    // get a handle to the database table of function names
    string LMODFunctionName = LMODFUNC;

    // create the table row entry and insert into the table
    simpleFuncRow LMODRow(mProjectId, LMODFunctionName);
    LMODRow.insert(mGDB);
    mLMODId = LMODRow.get_id();
    
    // separateCompilation indicates that we have separately parsed some
    // source files and that their state will be held in the database.
    // therefore, after creating the callgraph (which has an edge for every
    // argument passed between a caller and callee) we will go back and prune
    // it to create the simplecallgraph (with a single edge per function/method
    // invocation).  if we are parsing all of the source code now, we
    // can create the simplecallgraph now.

    // put the vertex for LMOD in the call graphs
    if (!mSideEffectPtr->separateCompilation) 
      mSimpleCallgraph->insertVertex(LMODRow, LMODRow.get_functionName());
    callVertex v;
    v = mCallgraph->insertVertex(LMODRow, LMODRow.get_functionName());
    
    // maintain a database table (localVars) of variable names (strings) that
    // are local to a given function (string).  formal parameters are
    // considered local to a function.
    // entry < functionName, varName > signifies that 'varName' is 
    // local to 'functionName'

    // get a handle to the localVars database table
    // create an entry for LMOD in localVars, indicating that LMOD has one
    // local variable-- its formal argument 'formal'
    int LMODDummyParamNum = 0;
    string LMODDummyFormal = LMODFORMAL;
    localVarRow localVar(
			      mProjectId, 
			      LMODRow.get_functionName().c_str(), 
			      LMODDummyFormal.c_str());
    localVar.insert(mGDB);
    
    // maintain a data structure in memory that maps function names
    // to local variables.  this mirrors the database table localVar.
    // only create this now if we don't need to make a second pass over the 
    // database to see any functions that have previously been parsed.
    // otherwise, fully create the table in the database, then iterate
    // over it to populate this memory-resident map.
    if (!mSideEffectPtr->separateCompilation) {
      mSideEffectPtr->insertLocal(LMODRow.get_functionName().c_str(), 
				  LMODDummyFormal.c_str());
      
    }
    
    
    // maintain a table (formals) that maps formal parameter (strings) in
    // functions (strings) to corresponding parameter numbers (ints)
    // entry < func, formal, pos > signifies that 'formal' is the 'pos'th
    // argument of 'func'.

    // create LMOD entry in formals, indicating that LMOD has a
    // formal argument 'formal' which is argument number 0
    formalRow formalParam(mProjectId, 
                          LMODRow.get_functionName().c_str(), 
                          LMODDummyFormal.c_str(),
                          LMODDummyParamNum);
    formalParam.insert(mGDB);
    // maintain a datastructure in memory that maps function names
    // to formal parameter strings and their ordinal position in the
    // argument list.  this mirrors the database table formals.
    if (!mSideEffectPtr->separateCompilation) {
      mSideEffectPtr->insertFormal(LMODRow.get_functionName().c_str(), 
                                   LMODDummyFormal.c_str(), 
				   LMODDummyParamNum);
      
      mSideEffectPtr->insertFormalByPos(LMODRow.get_functionName().c_str(), 
					LMODDummyFormal.c_str(), 
					LMODDummyParamNum);
    }
    
    mUniquifier = 0;
    
  }
  
  // Functions required by the tree traversal mechanism
  MyInheritedAttribute evaluateInheritedAttribute(SgNode* astNode,
						  MyInheritedAttribute inheritedAttribute) 
  {
    mUniquifier++;
    
    switch(astNode->variantT())
      {
      case V_SgFunctionDefinition: 
	{

	  // found a function definition
	  SgFunctionDeclaration *funcDec = 
	    isSgFunctionDeclaration(astNode->get_parent());
	  
#ifdef DEBUG_OUTPUT
	  if (debugOut) cerr << " found V_SgFunctionDefinition of " 
                             << funcDec->get_name().str() << endl; 
#endif
	  
	  string funcName;

	  if (funcDec) {
	    
	    SgMemberFunctionDeclaration *methodDec = 
	      isSgMemberFunctionDeclaration(astNode->get_parent());
	    
	    if (methodDec) {
	      // this is a method definition, extract the class name
	      // and append it
	      SgName qualifiedName = methodDec->get_qualified_name();
        funcName = qualifiedName.getString();
	    } else {
        funcName = funcDec->get_name();
	    }

	    // record this function definition if we haven't seen it before
	    if (mDefinedFuncs->find(funcName.c_str()) == mDefinedFuncs->end())
	      mDefinedFuncs->insert(funcName.c_str());
	    
	    // create a database entry for this function in the table 
	    // of functions.
	    simpleFuncRow funcRow(mProjectId, 
                                  funcName);
      funcRow.insert(mGDB);
      int funcId = funcRow.get_id();

	    // store the information about this function.  if this function makes
	    // an invocation we will want to gain access to it as the caller. 
	    inheritedAttribute.setFunctionDefinitionId(funcId);
	    inheritedAttribute.setParentFunction(funcDec);
	    inheritedAttribute.setParentName(funcName.c_str());

	    // add this function as a vertex in the call graphs
	    if (!mSideEffectPtr->separateCompilation) 
	      mSimpleCallgraph->insertVertex(funcRow, funcRow.get_functionName());
	    callVertex v;
	    v = mCallgraph->insertVertex(funcRow, funcRow.get_functionName());

	    // if this is main, remember it as the root of the call tree
	    if (!strcmp(funcRow.get_functionName().c_str(), "main")) {
	      assert(mSideEffectPtr->getCallRootSet() == false);
	      mSideEffectPtr->setCallRoot(v);
	      mSideEffectPtr->setCallRootName("main");
	    }
	    
	    // iterate over the parameter list for this function.  insert
	    // any potentially modifiable variables (i.e., SgPointerTypes or
	    // SgReferenceTypes) into the localVars and formals database tables
	    // and into their respective memory-resident maps, local and formal.
	    SgFunctionParameterList *params = funcDec->get_parameterList();
	    assert(params != NULL);
	    SgInitializedNamePtrList args = params->get_args();
	    SgInitializedNamePtrList::iterator nameIt;
	    
	    // access the types of the arguments so we can determine if
	    // they are pointers or references
	    SgFunctionType *funcType = funcDec->get_type();
	    SgTypePtrList &typeList = funcType->get_arguments();
	    SgTypePtrList::iterator typeIt;
	    
	    // ensure that the number of argument names and the number of
	    // argument types are the same.
	    assert(args.size() == typeList.size());
	    
	    // keep a count of the current formal argument
	    int paramNum;

	    // iterate over the formals
	    for(nameIt = args.begin(), typeIt = typeList.begin(), paramNum = 0; 
		nameIt != args.end();
		++nameIt, ++typeIt, ++paramNum) {

#ifdef DEBUG_OUTPUT
	      cout << "FORMAL: func " << funcName << " formal " << (*nameIt)->get_name().str() << " param #" << paramNum << endl;
#endif
	      
	      // put < func, var > entry in database to signify
	      // that var is local to func (since it is a formal
	      // parameter)

	      // create the table entry
	      localVarRow localVarFuncEntry(
                                            mProjectId, 
                                            funcName,
                                            (*nameIt)->get_name().str());
	      localVarFuncEntry.insert(mGDB);
	      
	      // insert a corresponding entry into the memory-resident map
	      if (!mSideEffectPtr->separateCompilation) 
		mSideEffectPtr->insertLocal(funcName.c_str(),
					    (*nameIt)->get_name().str());

	      // only create an entry in the formals table/map if this argument
	      // may be modified
	      if ( ( (*typeIt)->variantT() == V_SgPointerType ) ||
                   ( (*typeIt)->variantT() == V_SgReferenceType ) ) {

		// put < funcName, formalName, paramNum > entry in database to signify
		// that formalName is the paramNum'th argument of funcName.

		// create the table entry
		formalRow formal(
				      mProjectId, 
				      funcName,
				      (*nameIt)->get_name().str(), 
				      paramNum);
    formal.insert(mGDB);
		
		// insert a corresponding entry into the memory-resident map
		if (!mSideEffectPtr->separateCompilation) {
		  
		  mSideEffectPtr->insertFormal(funcName.c_str(),
                                               (*nameIt)->get_name().str(), 
					       paramNum);

		  mSideEffectPtr->insertFormalByPos(funcName.c_str(),
						    (*nameIt)->get_name().str(), 
						    paramNum);
		  
		}

	      } // end if pointer or reference type

	    } // end for loop over formals
	    
	  } // end if funcDec

	} break; // end case V_SgFunctionDefinition

      case V_SgFunctionCallExp: 
	{

	  // found a function call
#ifdef DEBUG_OUTPUT
	  if(debugOut) cerr << " found V_SgFunctionCallExp ... "; 
#endif

	  //	  mUniquifier++;
	  
	  if(inheritedAttribute.getFunctionDefinitionId() <= 0) break; // not in a recognized definition

#if 0
	  SgName funcName;
#else
	  string funcName;
#endif
	  SgFunctionDeclaration *funcDec = NULL;
	  SgFunctionRefExp *funcRef = isSgFunctionRefExp(((SgFunctionCallExp *)astNode)->get_function());

	  // see if its a member function call
	  SgDotExp *funcDotExp = isSgDotExp(((SgFunctionCallExp *)astNode)->get_function());

	  // see if it is a member function invocation from an object pointer
	  SgArrowExp *funcArrowExp = isSgArrowExp(((SgFunctionCallExp *)astNode)->get_function());
	  if(funcArrowExp) {
            SgMemberFunctionRefExp *membFunc = isSgMemberFunctionRefExp(funcArrowExp->get_rhs_operand());
            funcDec = membFunc->get_symbol_i()->get_declaration();
            
            SgMemberFunctionDeclaration *methodDec = 
              isSgMemberFunctionDeclaration(funcDec);
            assert(methodDec != NULL);

#if 1
            SgName qualifiedName = methodDec->get_qualified_name();
            string className = qualifiedName.getString();
            funcName = className;
#else
            funcName = methodDec->get_qualified_name();
#endif
	  } else if(funcDotExp) {
            SgMemberFunctionRefExp *membFunc = isSgMemberFunctionRefExp(funcDotExp->get_rhs_operand());
            funcDec = membFunc->get_symbol_i()->get_declaration();
            
            SgMemberFunctionDeclaration *methodDec = 
              isSgMemberFunctionDeclaration(funcDec);
            assert(methodDec != NULL);

#if 1
            SgName qualifiedName = methodDec->get_qualified_name();
            string className = qualifiedName.getString();
            funcName = className;
#else
            funcName = methodDec->get_qualified_name();
#endif
	    
	  } else if(funcRef) {
	    // found a standard function reference 
	    funcDec = funcRef->get_symbol_i()->get_declaration();
	    
#ifdef DEBUG_OUTPUT
	    if(debugOut) cerr << " std " ; if(funcDec) cerr << funcDec->get_name().str() << endl; 
#endif

	    funcName = funcDec->get_name().str();
	  }
	  
	  if(funcDec) {

	    // record this function as a callee if we haven't already done so
	    if (mCalledFuncs->find(funcName.c_str()) == mCalledFuncs->end())
	      mCalledFuncs->insert(funcName.c_str());

	    // lookup function name in function table.  insert if it doesn't exist

	    // create table entry
	    simpleFuncRow calleeRow(mProjectId, 
                                    funcName);
      calleeRow.insert(mGDB);

#if 0
// milki (07/01/2010) Is this debug statement valid?
#ifdef DEBUG_OUTPUT
	    cout << "NO funcId: " << data.get_id() << "for functionName " << data.get_functionName() << " id " << data.get_projectId() << endl;
#endif
#endif

	    // add callee as a vertex in the call graphs
	    if (!mSideEffectPtr->separateCompilation) 
	      mSimpleCallgraph->insertVertex(calleeRow, calleeRow.get_functionName());
	    mCallgraph->insertVertex(calleeRow, calleeRow.get_functionName());
	    
	    if (inheritedAttribute.getFunctionDefinitionId() == UNSET) {
	      // the function definition id is not set.  i.e., we have
	      // either arrived here from a global context or we are
	      // analyzing a code fragment (i.e., were called from
	      // calcSideEffect(SgNode*).  just fake up a global context,
	      // which is technically incorrect for the SgNode* case.
	      fakeUpCallingContext(inheritedAttribute);
	    }

	    // retrieve information about calling function by looking up funcId in
	    // function table
      sqlite3_command simplefuncsearch(*mGDB,
          "SELECT * FROM " + string(SIMPLEFUNCTBL) + " WHERE id=?;");
      simplefuncsearch.bind(1,inheritedAttribute.getFunctionDefinitionId());

      sqlite3_reader simplefuncr = simplefuncsearch.executereader();
      simpleFuncRow caller;
      if( simplefuncr.read() )
        caller.load(simplefuncr);
      else
        assert(false);
	    
	    // create an edge between the caller and callee in the callgraph 
	    // for each modifiable argument.  only add a single edge (even if the callee
	    // is void or has no modifiable arguments) in the simplecallgraph.

	    // get a handle to the edge table
	    int argNum = 0;
	    int numEdges = 0;

	    // get the expression list of actual arguments
	    SgExpressionPtrList expr_list = ((SgFunctionCallExp *)astNode)->get_args()->get_expressions();

	    // get the types of the arguments
	    SgFunctionType *funcType = funcDec->get_type();
	    SgTypePtrList &typeList = funcType->get_arguments();
	    SgTypePtrList::iterator typeIt;
	    
	    // ensure the number of arguments and the number of types of arguments agree
	    assert(expr_list.size() == typeList.size());
	    
	    // iterate over all of the actual arguments
        SgExpressionPtrList::iterator e;
	    for (e = expr_list.begin(), typeIt = typeList.begin(); 
                 e != expr_list.end(); ++e, ++typeIt) {

	      // if this is not a pointer or a reference, increment
	      // the current arg number and move on
	      if ( ( (*typeIt)->variantT() != V_SgPointerType ) &&
                   ( (*typeIt)->variantT() != V_SgReferenceType ) ) {
                argNum++;
		continue;
	      }
	      
	      // this argument is a pointer or a reference

	      // determine the number of variable references in an actual argument.
	      // we currenly only handle simple actual arguments.  bomb if there
	      // are multiple variables in an actual argument.

      // DQ (8/13/2004): Working with Brian, this was changed to use the new Query by IR nodes interface
      // list<SgNode*> varRefList = NodeQuery::querySubTree(*e, NodeQuery::VariableReferences);
          std::vector<SgNode*> varRefList = NodeQuery::querySubTree (*e,V_SgVarRefExp);

	      if (varRefList.size() > 1) {
		cerr << "We don' know how to handle complicated expressions!" << endl;
		cerr << "variant: " << ((SgNode *)(*e))->variantT() << endl;
		exit(-1);
	      }

	      if (varRefList.size() == 1) {

		// record the scope of this variable
		int scope = SCOPE_NA;

		SgNode *node = varRefList.front();
		SgVarRefExp *refExp = isSgVarRefExp(node);

		// extract the variable reference expression
		assert(refExp != NULL);

		if (refExp) {

		  // compare the declaration of the actual argument to the
		  // declarations of the caller's formal arguments.  if any
		  // match, this variable is both a formal argument of the caller
		  // and an actual argument from the caller to the callee.
		  // it has SCOPE_PARAM scope.
		  SgVariableSymbol *actualSym = refExp->get_symbol();
		  SgInitializedName *decl = actualSym->get_declaration();
		  SgDeclarationStatement *declptr1 = (*decl).get_declaration(); 

		  SgFunctionDeclaration *p = inheritedAttribute.getParentFunction();
		  if (p != NULL) {
		    // this could be null if calcSideEffect(SgNode*) was
		    // called with a code fragment-- i.e., SgNode* points
		    // within a function

		    SgFunctionParameterList *params = p->get_parameterList();
		    assert(params != NULL);
		    SgInitializedNamePtrList args = params->get_args();
		    SgInitializedNamePtrList::iterator nameIt;
		    
		    for(nameIt = args.begin(); nameIt != args.end(); ++nameIt) {

		      SgDeclarationStatement *declptr2 = (*nameIt)->get_declaration(); 

		      if (declptr2 == declptr1) {
			scope = SCOPE_PARAM;
			break;
		      }

		    }

		  }
		  
#ifdef DEBUG_OUTPUT
		  cout << "symbol name: " << actualSym->get_name().str() << endl;
#endif		   
		  
		  // if the variable reference was not passed as a formal parameter,
		  // it is either a global variable or was declared within the
		  // caller.  determine which is the case by looking at the scope.
		  if (scope != SCOPE_PARAM) {

		    if (isSgGlobal(declptr1->get_scope())) {
		      scope = SCOPE_GLOBAL;
		    } else {
		      scope = SCOPE_LOCAL;
		    }

		  }
		  
		  // not that we have determined the scope, insert an edge into the
		  // call graphs.

		  // record the number of edges we have inserted
		  numEdges++;

		  string callerName = caller.get_functionName();
#if 0
		  char tmp[callerName.length() + 
                           strlen(astNode->get_file_info()->get_filename()) +
			   MAXSTRINGSZ];

		  // file_info is broken for expressions so we have to use
		  // the uniquifier hack
		  sprintf(tmp, "%s-%s-%d-%d", 
			  callerName.c_str(), 
			  astNode->get_file_info()->get_filename(), 
			  astNode->get_file_info()->get_line(), 
			  astNode->get_file_info()->get_col());
#endif
		  // really gross:  until we get the line/col info working
		  // i'll use the address of the node as the uniquifier.
		  // using a simple incrementing counter as originally
		  // intended isn't ideal because it requires the
		  // client to count in the same way.  this may seem
		  // trivial, but consider the case of invoking the
		  // side effect analysis on an SgNode*.  the client
		  // probably starts at an SgProject or an SgFile and its
		  // counter is zero then; the side effect analysis sets its
		  // counter to zero when it begins to parse the SgNode.
#if 0
		  mUniquifier = (long)astNode;
		  sprintf(tmp, "%s-%s-%ld", 
			  callerName.c_str(), 
			  astNode->get_file_info()->get_filename(), 
			  mUniquifier);
		  // create the edge row entry with the label of the edge,
		  // the name of the actual argument, its scope, and
		  // its position in the parameter list
		  string edgeLabel = tmp;
#else
		  string edgeLabel = mSideEffectPtr->getNodeIdentifier(astNode);
#endif
		  string actual = actualSym->get_name().str();
		  if (scope == SCOPE_GLOBAL)
		    actual.insert(0, "::");
		  callEdgeRow edge(
                                       mProjectId, 
				       edgeLabel, 
				       actual.c_str(),
				       scope, 
				       argNum);
      edge.insert(mGDB);

#ifdef DEBUG_OUTPUT
      // milki (07/07/2010) data no longer a valid reference. Assuming
      // calleeRow
		  //cout << "INSERTING edge with scope " << edge.get_scope() << " site " << edge.get_site() << " actual " << edge.get_actual() << " between " << caller.get_functionName() << " and " << data.get_functionName() << endl;
		  cout << "INSERTING edge with scope " << edge.get_scope() << " site " << edge.get_site() << " actual " << edge.get_actual() << " between " << caller.get_functionName() << " and " << calleeRow.get_functionName() << endl;
#endif

		  // insert this edge in the callgraph between the caller and callee
		  mCallgraph->insertEdge(caller, calleeRow, edge);

		  // insert this edge in the simplecallgraph only if it is the
		  // first edge between the caller and callee.  we only want
		  // one edge per caller-callee pair.
		  if (!mSideEffectPtr->separateCompilation) 
		    if (argNum == 0)
		      mSimpleCallgraph->insertEdge(caller, calleeRow, edge);

		} // end if(refExp)

	      } // end if(varRefList.size() == 1)
	      
	      // move on to the next argument
	      argNum++;

	    } // end iteration over expressions in argument list
	    
	    // if the callee is void or has no modifiable arguments, we
	    // would still like to create entries in the callgraphs.
	    if (numEdges == 0) {

	      string voidCallerName = caller.get_functionName();
#if 0
	      char tmp[voidCallerName.length() + 
                       strlen(astNode->get_file_info()->get_filename()) +
		       MAXSTRINGSZ];
              // file_info is broken for expressions
	      sprintf(tmp, "%s-%s-%d-%d", s.c_str(), 
		      astNode->get_file_info()->get_filename(), 
		      astNode->get_file_info()->get_line(), 
		      astNode->get_file_info()->get_col());
#endif
	      // really gross:  until we get the line/col info working
	      // i'll use the address of the node as the uniquifier.
	      // using a simple incrementing counter as originally
	      // intended isn't ideal because it requires the
	      // client to count in the same way.  this may seem
	      // trivial, but consider the case of invoking the
	      // side effect analysis on an SgNode*.  the client
	      // probably starts at an SgProject or an SgFile and its
	      // counter is zero then; the side effect analysis sets its
	      // counter to zero when it begins to parse the SgNode.
#if 0
	      mUniquifier = (long)astNode;
	      sprintf(tmp, "%s-%s-%ld", 
		      voidCallerName.c_str(), 
		      astNode->get_file_info()->get_filename(), 
		      mUniquifier);

	      string edgeLabel = tmp; 
#else
	      string edgeLabel = mSideEffectPtr->getNodeIdentifier(astNode);
#endif
	      // create the dummy edge with actual arg "void" and argument number -1
	      callEdgeRow dummyEdge(mProjectId, edgeLabel, "void", SCOPE_NA, -1);
        dummyEdge.insert(mGDB);

	      // insert the dummy edge in the callgraphs
	      mCallgraph->insertEdge(caller, calleeRow, dummyEdge);
	      if (!mSideEffectPtr->separateCompilation) 
		mSimpleCallgraph->insertEdge(caller, calleeRow, dummyEdge);
	    }
	  }
#ifdef DEBUG_OUTPUT
	  if(debugOut) cerr << " , done." << endl; 
#endif
	} break;
	
      case V_SgVariableDeclaration:
	{
	  
	  // if this variable declaration occurs within a function ...
	  if (inheritedAttribute.getParentFunction() != NULL) {
	    
	    SgVariableDeclaration *varDec = isSgVariableDeclaration(astNode);
	    
	    assert(varDec != NULL);
	    
	    // get the scope of the variable declaration
	    SgStatement *scope = varDec->get_scope();
	    
	    // we've already said we are within a function
	    assert(!isSgGlobal(scope));
	    
	    typedef SgInitializedNamePtrList::iterator name_iter;
	    SgInitializedNamePtrList nameList = varDec->get_variables();
	    
	    // iterate over all variables in this declaration, each
	    // constitutes a local mod for this function
	    for (name_iter n = nameList.begin(); n != nameList.end(); ++n) {
	      
	      assert(*n != NULL);
	      SgInitializedName* initName = *n;
	      SgName sageName = initName->get_name();
	      string varName = sageName.str();
	      
#ifdef DEBUG_OUTPUT
	      if (debugOut) cerr << "Variable Dec: " << varName << endl;
#endif
	      
	      // put < funcName, varName > entry in localVars table to
	      // signify that varName is local to funcName.  funcName
	      // is the function name of this expression's parent that
	      // we have stashed off in the inherited attribute.

	      // create the row entry
	      localVarRow localDec(
					mProjectId, 
					inheritedAttribute.getParentName(), 
					varName.c_str());
        localDec.insert(mGDB);
	      
	      // insert a corresponding entry into the memory-resident map
	      if (!mSideEffectPtr->separateCompilation) {
		//		mSideEffectPtr->insertLocal(inheritedAttribute.getParentFunction()->get_name().str(), varName.c_str());
		mSideEffectPtr->insertLocal(inheritedAttribute.getParentName(), varName.c_str());
	      }
	      
	    } // end for
	    
	  } // end if (inheritedAttribute ... )
	  
	} break;
	
      default:
	{
	  SgExpression *expr = isSgExpression(astNode);
	  
	  if (!expr)
	    break;
	  
	  //	  mUniquifier++;

	  // determine if this expression is a destructive operation
	  
#if 0
	  if (expr->get_is_lvalue()) 
#endif
	  if ( isSgAssignOp(expr) || 
               isSgPlusAssignOp(expr) ||
	       isSgMinusAssignOp(expr) ||
	       isSgAndAssignOp(expr) ||
	       isSgIorAssignOp(expr) ||
	       isSgMultAssignOp(expr) ||
	       isSgDivAssignOp(expr) ||
	       isSgModAssignOp(expr) ||
	       isSgXorAssignOp(expr) ||
	       isSgLshiftAssignOp(expr) ||
	       isSgRshiftAssignOp(expr) )
          { 

	    // this expression is the root of a destructive operation
	    inheritedAttribute.setLHS(true);
	    inheritedAttribute.setAssignOp(expr);
	    mFoundLHS = false;

#ifdef DEBUG_OUTPUT
	    cout << "lval: " << getVariantName(astNode->variantT()) << " line: " << expr->get_file_info()->get_line() << " cur_line: " << expr->get_file_info()->get_line() << endl;
#endif
	    SgBinaryOp *bin = isSgBinaryOp(astNode);

	    if ( ! isSgVarRefExp(expr) &&
                 ! isSgPointerDerefExp(expr) &&
		 ( !bin || (
                            ! isSgArrowExp(bin->get_lhs_operand()) &&
			    ! isSgPntrArrRefExp(bin->get_lhs_operand()) &&
			    ! isSgPointerDerefExp(bin->get_lhs_operand()) &&
			    ! isSgVarRefExp(bin->get_lhs_operand()) &&
			    ! isSgDotExp(bin->get_lhs_operand()) ) ) ) {
#ifdef DEBUG_OUTPUT
	      cout << "bad bad: " << getVariantName(astNode->variantT()) << endl;
#endif
	      assert(1 == 0);
	    }
	    
	  }
	  

	  // if this isn't a variable reference, we are done
	  
	  SgVarRefExp *refExp = isSgVarRefExp(astNode);
	  if (!refExp)
	    break;

	  // scope of this variable reference
	  int scope = SCOPE_NA;

	  // is this variable a LHS?
	  bool destructiveAssign = false;

	  // current thinking:  we are doing a depth first search.
	  // the leftmost var in the subtree of an assignment will
	  // be the lhs variable, at least for the assignment types
	  // i'm currently thinking of.  this variable will be the 
	  // first one encountered in our traversal.  so if we see
	  // a variable and LHS is set, we have a winner, reset LHS to false.
	  // we verify the lhs is one of the expected types above.  
	  // if this ever breaks it probably means i've failed to 
	  // anticipate some kind of assignment.

	  destructiveAssign = inheritedAttribute.getLHS();
	  inheritedAttribute.setLHS(0);

	  if (destructiveAssign && !mFoundLHS) {
	    mFoundLHS = true;

	    // determine the scope of this variable reference

	    // compare the declaration of the variable to the
	    // declarations of the caller's formal arguments.  if any
	    // match, this variable is a formal argument of the caller;
	    // it has SCOPE_PARAM scope.
	    SgVariableSymbol *LHSSym = refExp->get_symbol();
	    SgInitializedName *decl = LHSSym->get_declaration();
	    SgDeclarationStatement *declptr1 = (*decl).get_declaration(); 
	    SgFunctionDeclaration *p = inheritedAttribute.getParentFunction();
	    
	    if (p != NULL) {
	      // this could be null if calcSideEffect(SgNode*) was
	      // called with a code fragment-- i.e., SgNode* points
	      // within a function

	      SgFunctionParameterList *params = p->get_parameterList();
	      assert(params != NULL);
	      SgInitializedNamePtrList args = params->get_args();
	      SgInitializedNamePtrList::iterator nameIt;
	      
	      for(nameIt = args.begin(); nameIt != args.end();
		  ++nameIt) {

		SgDeclarationStatement *declptr2 = (*nameIt)->get_declaration(); 

		if (declptr2 == declptr1) {
		  scope = SCOPE_PARAM;
		  break;
		}

	      }
	    } // end if(p != NULL)
	    
	    // if the variable reference was not passed as a formal parameter,
	    // it is either a global variable or was declared within the
	    // caller.  determine which is the case by looking at the scope.
	    if (scope != SCOPE_PARAM) {

	      if (isSgGlobal(declptr1->get_scope())) {
		scope = SCOPE_GLOBAL;
	      } else {
		scope = SCOPE_LOCAL;
	      }

	    }
	    
	    if (inheritedAttribute.getFunctionDefinitionId() == UNSET) {
	      // the function definition id is not set.  i.e., we have
	      // either arrived here from a global context or we are
	      // analyzing a code fragment (i.e., were called from
	      // calcSideEffect(SgNode*).  just fake up a global context,
	      // which is technically incorrect for the SgNode* case.
	      fakeUpCallingContext(inheritedAttribute);
	    }

	    simpleFuncRow caller;
      sqlite3_command callerSearch(*mGDB,
          "SELECT * FROM " + string(SIMPLEFUNCTBL) + " WHERE id=?;");
      callerSearch.bind(1,inheritedAttribute.getFunctionDefinitionId());

      sqlite3_reader callerreader = callerSearch.executereader();

      if( callerreader.read() )
        caller.load(callerreader);
      else
        assert(false);

	    // transform this assignment into an invocation on the pseudo-function
	    // __lmod.  look up that 'callee' in the database.
	    simpleFuncRow dummyCallee; 

      sqlite3_command dummyCalleesearch(*mGDB,
          "SELECT * FROM " + string(SIMPLEFUNCTBL) + " WHERE id=?;");
      dummyCalleesearch.bind(1,mLMODId);

      sqlite3_reader dummyreader = dummyCalleesearch.executereader();

      if( dummyreader.read() )
        dummyCallee.load(dummyreader);
      else
        assert(false);
	    
	    string dummyCallerName = caller.get_functionName();
	    SgNode *assign = inheritedAttribute.getAssignOp();
#if 0
	    char tmp[dummyCallerName.length() + 
                     strlen(assign->get_file_info()->get_filename()) +
		     MAXSTRINGSZ];
            // file_info is broken for expressions
	    sprintf(tmp, "%s-%s-%d-%d", 
		    dummyCallerName.c_str(), 
		    assign->get_file_info()->get_filename(), 
		    assign->get_file_info()->get_line(), 
		    assign->get_file_info()->get_col());
#endif
	    // really gross:  until we get the line/col info working
	    // i'll use the address of the node as the uniquifier.
	    // using a simple incrementing counter as originally
	    // intended isn't ideal because it requires the
	    // client to count in the same way.  this may seem
	    // trivial, but consider the case of invoking the
	    // side effect analysis on an SgNode*.  the client
	    // probably starts at an SgProject or an SgFile and its
	    // counter is zero then; the side effect analysis sets its
	    // counter to zero when it begins to parse the SgNode.
#if 0
	    mUniquifier = (long)assign;
	    sprintf(tmp, "%s-%s-%ld", 
		    dummyCallerName.c_str(), 
		    assign->get_file_info()->get_filename(), 
		    mUniquifier);

	    string edgeLabel  = tmp; 
#else
	    string edgeLabel = mSideEffectPtr->getNodeIdentifier(assign);
#endif

	    // create the edge row entry with the label of the edge,
	    // the name of the LHS variable, its scope, and
	    // its position in the parameter list.  (since this is an
	    // invocation on the dummy function __lmod, the parameter is zero.)

	    string actual = LHSSym->get_name().str();
	    if (scope == SCOPE_GLOBAL)
	      actual.insert(0, "::");
	    callEdgeRow edge(
                                 mProjectId, 
				 edgeLabel, 
				 actual.c_str(),
				 scope, 
				 0);
      edge.insert(mGDB);

#ifdef DEBUG_OUTPUT
      // milki (07/07/2010) data no longer referenced. Assuming
      // dummyCallee
	    //cout << "INSERTING dummy edge with scope " << edge.get_scope() << " site " << edge.get_site() << " actual " << edge.get_actual() << " between " << caller.get_functionName() << " and " << data.get_functionName() << " tmp is " << tmp << endl;
	    cout << "INSERTING dummy edge with scope " << edge.get_scope() << " site " << edge.get_site() << " actual " << edge.get_actual() << " between " << caller.get_functionName() << " and " << dummyCallee.get_functionName() << endl;
#endif

	    mCallgraph->insertEdge(caller, dummyCallee, edge);
	    if (!mSideEffectPtr->separateCompilation) 
	      mSimpleCallgraph->insertEdge(caller, dummyCallee, edge);

	  } // end if(destructiveAssign && !mFoundLHS)

	} break; // end case default

      } // end switch
 
    MyInheritedAttribute returnAttribute(inheritedAttribute);
    return returnAttribute;
  } // end method

  int getLMODId() { return mLMODId; }
  
 protected:

  // keep a connection to the database for the callgraph
  sqlite3_connection *mGDB;
  
  // the id of the current project
  int mProjectId;
  
  // the id of the pseudo-function __lmod.  all statements that
  // modify a var v are converted to an invocation of __lmod(v)
  int mLMODId;
  
  // used to discriminate between statements on the same line
  // (according to getCurrentLine)
  long mUniquifier;
  
  // the current callgaph
  CallGraph *mCallgraph;
  
  // the current callgaph but with only one edge between vertices
  CallGraph *mSimpleCallgraph;
  
  // these sets contain the functions defined and called during
  // this traversal
  set<const char *, ltstr> *mDefinedFuncs;
  set<const char *, ltstr> *mCalledFuncs;

 private:
  void fakeUpCallingContext(MyInheritedAttribute& inheritedAttribute)
  {
    
    string funcName = "globalContext";
    // create a database entry for this function in the table 
    // of functions.
    simpleFuncRow funcRow(
                                   mProjectId, 
				   funcName);
    funcRow.insert(mGDB);

    int funcId = funcRow.get_id();
    
    // store the information about this function.  if this function makes
    // an invocation we will want to gain access to it as the caller. 
    // actually, we can only take this so far.  this isn't really a 
    // function, so we can give it a name and put it in the function
    // table but let's not push it too far.  there is no function
    // declaration node.  the code that examines the inheritedattribute
    // is smart enough to check for a null parent function pointer
    // and do something sane.
    inheritedAttribute.setFunctionDefinitionId(funcId);
    //      inheritedAttribute.setParentFunction(funcDec);
    inheritedAttribute.setParentFunction(NULL);
    inheritedAttribute.setParentName(funcName.c_str());
    
    // add this function as a vertex in the call graphs
    if (!mSideEffectPtr->separateCompilation) 
      mSimpleCallgraph->insertVertex(funcRow, funcRow.get_functionName());
    callVertex v;
    v = mCallgraph->insertVertex(funcRow, funcRow.get_functionName());
    
    // assumption in this function is that calcsideeffect has been
    // called with a SgNode* fragment-- i.e., within a function.
    // ensure we have seen main

    assert(mSideEffectPtr->getCallRootSet() == false);

    // now set this dummy function to be the root of our call graph,
    // which by now has become a bit of a perversion
    mSideEffectPtr->setCallRoot(v);
    mSideEffectPtr->setCallRootName(funcName);

#if 0
    // iterate over the parameter list for this function ...
    // ...
#endif
    // there is no parameter list this is not a real function
    
  }

  // have we already found the base variable that represents the lhs
  // in an assignment?
  bool mFoundLHS;

  SideEffect *mSideEffectPtr;

};

// read table of < func, var > indicating that variable var is
// local to function fuc and insert into data structure.
void 
SideEffect::populateLocalVarsFromDB(sqlite3_connection *db)
{
  vector<localVarRow> rows;
  
  sqlite3_command localvarget(*db,
      "SELECT * FROM " + string(LOCALVARTBL));

  sqlite3_reader localreader = localvarget.executereader();

  while( localreader.read() ) {
    localVarRow row(localreader);
    rows.push_back(row);
  }

  for (unsigned int i = 0; i < rows.size(); ++i) {
    insertLocal(rows[i].get_functionName().c_str(),
		rows[i].get_varName().c_str());
  }
}

// read table of < func, formal, pos > entries indicating
// that formal is pos'th formal parameter of function func.
// insert into data structure.
void 
SideEffect::populateFormalsFromDB(sqlite3_connection *db)
{
  vector<formalRow> rows;

  sqlite3_command formalvarget(*db,
      "SELECT * FROM " + string(FORMALTBL));

  sqlite3_reader formalreader = formalvarget.executereader();

  while( formalreader.read() ) {
    formalRow row(formalreader);
    rows.push_back(row);
  }

  for (unsigned int i = 0; i < rows.size(); ++i) {
    insertFormal(rows[i].get_functionName().c_str(),
                 rows[i].get_formal().c_str(),
		 rows[i].get_ordinal());

    insertFormalByPos(rows[i].get_functionName().c_str(),
		      rows[i].get_formal().c_str(),
		      rows[i].get_ordinal());
  }
}

CallMultiGraph *
SideEffect::createMultiGraph(CallGraph *callgraph, long projectId, 
                             sqlite3_connection *db)
{
  // a binding multigraph is described on pp 60.  we select out
  // only those vertices that are an endpoint of an edge in the
  // binding multigraph.  an edge in the binding multigraph is an
  // edge in the callMULTIgraph that binds a formal parameter of
  // one function to a formal parameter of another.

  // iterate over the edges in the callmultigraph.  if the variable
  // involved is a formal parameter, add both vertices to the binding
  // multigraph as well as the edge.

  CallMultiGraph *multigraph = new CallMultiGraph( projectId, GTYPE_SIMPLECALLGRAPH, db );

  typedef boost::graph_traits<CallGraph>::edge_iterator edge_iter;
  pair<edge_iter, edge_iter> ep;

  for (ep = edges(*callgraph); ep.first != ep.second; ++ep.first) {
    int scope = get( boost::edge_dbg_data, *callgraph, *ep.first ).get_scope();
    if ( scope == SCOPE_PARAM ) {
#ifdef DEBUG_OUTPUT
      cout << "****** edge: " << get( boost::vertex_dbg_data, *callgraph, boost::source(*(ep.first), *callgraph) ).get_functionName() << " to " << get( boost::vertex_dbg_data, *callgraph, boost::target(*(ep.first), *callgraph) ).get_functionName() << endl;
#endif

      string srcFunc(get( boost::vertex_dbg_data, *callgraph, boost::source(*(ep.first), *callgraph) ).get_functionName());
      string actual(get( boost::edge_dbg_data, *callgraph, *ep.first ).get_actual());

      varNodeRow src(projectId, srcFunc, actual );
      src.insert(db);

      typedef boost::graph_traits < CallMultiGraph >::vertex_descriptor Vertex;
      Vertex v;
		 
      string name1(srcFunc + ":" + actual);
      v = multigraph->insertVertex( src, name1 );
#ifdef DEBUG_OUTPUT
      cout << "Inserted vertex: " << get( boost::vertex_dbg_data, *multigraph, v).get_functionName() << endl;
#endif

      string tarFunc(get( boost::vertex_dbg_data, *callgraph, boost::target(*(ep.first), *callgraph) ).get_functionName());
      const char *param;
      int ordinal;

      ordinal = get( boost::edge_dbg_data, *callgraph, *ep.first ).get_ordinal();
      //      tarFunc =get( boost::vertex_dbg_data, *callgraph, boost::target(*(ep.first), *callgraph) ).get_functionName();

      param = lookupFormalByPos(tarFunc.c_str(), ordinal);
      assert(param != NULL);

      string param2(param);
      //      varNodeRowdata tar( UNKNOWNID, projectId, tarFunc, param2 );
      varNodeRow tar( projectId, tarFunc, param2 );
      tar.insert(db);

      string name2(tarFunc + ":" + param);
      v = multigraph->insertVertex( tar, name2 );
#ifdef DEBUG_OUTPUT
      cout << "Inserted vertex: " << get( boost::vertex_dbg_data, *multigraph, v).get_functionName() << endl;
#endif

      callEdgeRow edge = get( boost::edge_dbg_data, *callgraph, *ep.first );
      edge.set_id( ++nextId );

      typedef boost::graph_traits < CallMultiGraph >::edge_descriptor Edge;
      typedef std::pair<bool, Edge> dbgEdgeReturn; 
      multigraph->insertEdge( src, tar, edge );
      typedef boost::property_map<CallMultiGraph, boost::vertex_index_t>::type
	VertexIndexMap;
      typedef boost::property_map<CallMultiGraph, boost::edge_index_t>::type
	EdgeIndexMap;
      EdgeIndexMap index_map = get(boost::edge_index, *multigraph);

    }
  }

#ifdef DEBUG_OUTPUT
  for (ep = edges(*multigraph); ep.first != ep.second; ++ep.first) {
      cout << "****** mgraph edge: " << get( boost::vertex_dbg_data, *multigraph, boost::source(*(ep.first), *multigraph) ).get_functionName() << " to " << get( boost::vertex_dbg_data, *multigraph, boost::target(*(ep.first), *multigraph) ).get_functionName() << endl;
  }
#endif 

  return multigraph;
}

class solve_rmod : public boost::base_visitor<solve_rmod> {

 public:
  typedef boost::on_finish_vertex event_filter;
  
  solve_rmod(SideEffect *setSideEffect) : 
    mReachedFixedPoint(false),
    mSideEffectPtr(setSideEffect)
  { }

  template <class Vertex, class G>
  void operator()(Vertex m, G& g) {
    string mFunc = get( boost::vertex_dbg_data, g, m).get_functionName();
    string mVarName = get( boost::vertex_dbg_data, g, m).get_varName();

    typedef typename boost::graph_traits<G>::out_edge_iterator out_edge_iter;
    pair<out_edge_iter, out_edge_iter> ep;

    // union the RMODs of any target neighbors
    for (ep = out_edges(m, g); ep.first != ep.second; ++ep.first) {
      Vertex n = boost::target(*(ep.first), g);
      string nFunc = get( boost::vertex_dbg_data, g, n).get_functionName();
      string nVarName = get( boost::vertex_dbg_data, g, n).get_varName();

      pair<map_type::const_iterator, map_type::const_iterator> p = 
	mSideEffectPtr->lookupRMOD(nFunc.c_str());

      for (map_type::const_iterator i = p.first; i != p.second; ++i) {
#ifdef DEBUG_OUTPUT
	cout << "COMPARING " << (*i).second << " with " << nVarName.c_str() << endl;
#endif
	if (!strcmp((*i).second, nVarName.c_str())) {

#ifdef DEBUG_OUTPUT
	  cout << "----- Inserting RMOD(" << mFunc.c_str() << ") = RMOD(" << nFunc.c_str() << ") == " << mVarName.c_str() << endl;
#endif
#if 0
	  if (mSideEffectPtr->insertRMOD( mFunc.c_str(), (*i).second ))
	    mReachedFixedPoint = false;
#endif
	  if (mSideEffectPtr->insertRMOD( mFunc.c_str(), mVarName.c_str() ))
	    mReachedFixedPoint = false;
	}
      }
      
    }

    // union the IMOD of this node
    pair<map_type::const_iterator, map_type::const_iterator> p = 
      mSideEffectPtr->lookupIMODRep(mFunc.c_str());
   
    for (map_type::const_iterator i = p.first; i != p.second; ++i) {
#ifdef DEBUG_OUTPUT
      cout << "IMOD COMPARE: " << (*i).second << " and " << mVarName.c_str() << endl;
#endif
      if (!strcmp((*i).second, mVarName.c_str())) 
	if (mSideEffectPtr->insertRMOD( mFunc.c_str(), (*i).second ))
	  mReachedFixedPoint = false;
    }

#ifdef DEBUG_OUTPUT
    cout << "visit func: " << mFunc << endl;
#endif
  }

  bool mReachedFixedPoint;

 private:
  SideEffect *mSideEffectPtr;

};

void 
SideEffect::solveRMOD(CallMultiGraph *multigraph, long projectId, 
		      sqlite3_connection *db)
{
  // from figure 1 pp 60

  assert(multigraph != NULL);

  // (1) find strongly connected components of the multigraph
  typedef boost::property_map<CallMultiGraph, boost::vertex_index_t>::const_type
    VertexIndexMap;
  VertexIndexMap index_map = get(boost::vertex_index, *multigraph);
  typedef boost::graph_traits < CallMultiGraph >::vertex_descriptor vertex;
  typedef boost::graph_traits < CallMultiGraph >::edge_descriptor edge;
  typedef boost::graph_traits < CallMultiGraph >::vertex_iterator vertex_iterator;
  typedef boost::property_traits < VertexIndexMap >::value_type size_type;
  typedef boost::graph_traits <
    CallMultiGraph >::adjacency_iterator adjacency_iterator;
  
  typedef size_type cg_vertex;
  std::vector < cg_vertex > component_number_vec(num_vertices(*multigraph));
  boost::iterator_property_map < cg_vertex *, VertexIndexMap, cg_vertex, cg_vertex& >
    component_number(&component_number_vec[0], index_map);
  
  //  const char *name;
  string name;

  int num_scc = strong_components(*multigraph, component_number,
				  vertex_index_map(index_map));
  
  std::vector < std::vector < vertex > >components;
  
  build_component_lists(*multigraph, num_scc, component_number, components);
  
#ifdef DEBUG_OUTPUT
  cout << "There are " << components.size() << " SCCs in this multigraph "
    << endl;
  for (cg_vertex s = 0; s < components.size(); ++s) {
    
    cout << "component: " << s << endl;
    for (size_type i = 0; i < components[s].size(); ++i) {
      vertex u = components[s][i];
      cout << "func: " << get( boost::vertex_dbg_data, *multigraph, u).get_functionName() << endl;
    }
  }
#endif
  
  // (2) replace each SCC with a representative node n, setting
  // IMOD(n) to the logical or of all the IMOD sets of the nodes in the
  // SCC.  set RMOD(n) to false.
  //  adjacency_list<vecS, vecS, bidirectionalS> derivedG;   
  //  Graph derivedG;
  // dummy fields
  CallMultiGraph *derivedG = new CallMultiGraph( projectId, GTYPE_SIMPLECALLGRAPH, db );
  std::vector < vertex > reps(num_vertices(*multigraph));

  // first add the vertices to the derived graph
  for (cg_vertex s = 0; s < components.size(); ++s) {
    
    assert(components[s].size() > 0);
    vertex rep = components[s][0];
    //    reps[component_number[rep]] = rep;
    
    //    simpleFuncTableRowdata v( get( boost::vertex_dbg_data, *multigraph, rep ) );
    varNodeRow v = get( boost::vertex_dbg_data, *multigraph, rep );

#ifdef DEBUG_OUTPUT
    cout << "Adding " << rep << " " << v.get_functionName() << endl;
#endif

    name = v.get_functionName() + ":" + v.get_varName();

    //    reps[component_number[rep]] = derivedG->insertVertex( v, v.get_functionName() );
    reps[component_number[rep]] = derivedG->insertVertex( v, name );

    for (size_type i = 0; i < components[s].size(); ++i) {

      varNodeRow v2 = get( boost::vertex_dbg_data, *multigraph, components[s][i] );

      pair<map_type::const_iterator, map_type::const_iterator> p = 
	lookupIMOD(v2.get_functionName().c_str());

      for (map_type::const_iterator i = p.first; i != p.second; ++i)
	//	if (!strcmp(v2.get_varName().c_str(), (*i).second))
	  insertIMODRep( v.get_functionName().c_str(), (*i).second );

    }

  }


#ifdef DEBUG_OUTPUT
  for (cg_vertex s = 0; s < components.size(); ++s) {
    
    assert(components[s].size() > 0);
    vertex rep = components[s][0];
    //    reps[component_number[rep]] = rep;
    
    //    simpleFuncTableRowdata v( get( boost::vertex_dbg_data, *multigraph, rep ) );
    varNodeRow v =  get( boost::vertex_dbg_data, *multigraph, rep );

    pair<map_type::const_iterator, map_type::const_iterator> p = 
      lookupIMOD(v.get_functionName().c_str());

    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << v.get_functionName().c_str() << " var: " << (*i).second << endl;

  }
#endif

  typedef boost::graph_traits<CallMultiGraph>::out_edge_iterator out_edge_iter;
  pair<out_edge_iter, out_edge_iter> ep;

  // now collapse the edges
  for (cg_vertex s = 0; s < components.size(); ++s) {
    
    for (size_type i = 0; i < components[s].size(); ++i) {

      vertex origRep = components[s][i];
      vertex derivedRep = reps[component_number[components[s][i]]];

#ifdef DEBUG_OUTPUT
      cout << "Retrieving " << derivedRep << endl;
#endif

      varNodeRow src = get( boost::vertex_dbg_data, *derivedG, derivedRep );

      for (ep = out_edges(origRep, *multigraph); ep.first != ep.second; ++ep.first) {
	vertex rep2 = reps[component_number[boost::target(*(ep.first), *multigraph)]];
	//	simpleFuncTableRowdata tar( get( boost::vertex_dbg_data, *derivedG, rep2 ) );
	varNodeRow tar = get( boost::vertex_dbg_data, *derivedG, rep2 );

	callEdgeRow edge = get( boost::edge_dbg_data, *multigraph, *ep.first );
	derivedG->insertEdge( src, tar, edge );

      }
    }
  }

  // (3) traverse the derived graph from leaves to roots, applying eqn (6)
#if 1
  solve_rmod solver(this);
  do {
    solver.mReachedFixedPoint = true;
    depth_first_search(*derivedG, visitor(make_dfs_visitor(solver)));
  } while (!solver.mReachedFixedPoint);
#else
  depth_first_search(*derivedG, visitor(make_dfs_visitor(solve_rmod())));
#endif

  // (4) for each SCC, set the RMOD set for each node in the SCC to the
  // value of the RMOD set for its representative node
  for (cg_vertex s = 0; s < components.size(); ++s) {
   
    vertex rep = components[s][0];
    string repName = get( boost::vertex_dbg_data, *multigraph, rep ).get_functionName();
 
    pair<map_type::const_iterator, map_type::const_iterator> p = 
      lookupRMOD(repName.c_str());

    for (size_type i = 1; i < components[s].size(); ++i) {
      
      vertex v = components[s][i];
      string nodeName = get( boost::vertex_dbg_data, *multigraph, v ).get_functionName();

      for (map_type::const_iterator i = p.first; i != p.second; ++i)
	insertRMOD( nodeName.c_str(), (*i).second );

    }
  }

};

// this implements eqn 5
// NB:  we have done away with the IMOD sets, except for the pseudo function
//      __lmod because we have converted all assignments to invocations on that
//      function.  where we would have unioned IMOD sets here we should instead
//      detect if there is an edge from a caller to __lmod, in which case
//      we need to add the corresponding actual to callers imodplus set.
class solve_imodplus : public boost::base_visitor<solve_imodplus> {
 public:
  typedef boost::on_finish_vertex event_filter;
  
  solve_imodplus(SideEffect *setSideEffect) :
    mReachedFixedPoint(false),
    mSideEffectPtr(setSideEffect)
  { }

  template <class Vertex, class G>
  void operator()(Vertex p, G& g) {
    string pFunc = get( boost::vertex_dbg_data, g, p).get_functionName();
#ifdef DEBUG_OUTPUT
    cout << "Operating on solve_imodplus for " << pFunc.c_str() << endl;
#endif
    typedef typename boost::graph_traits<G>::out_edge_iterator out_edge_iter;
    pair<out_edge_iter, out_edge_iter> ep;

    // NB:  this could be much more efficient
    for (ep = out_edges(p, g); ep.first != ep.second; ++ep.first) {
      callVertex q = boost::target(*(ep.first), g);
      string qFunc = get( boost::vertex_dbg_data, g, q).get_functionName();
      int paramNum = get( boost::edge_dbg_data, g, (*ep.first) ).get_ordinal();

#ifdef DEBUG_OUTPUT
    cout << "Examining " << qFunc.c_str() << " with "
      << paramNum << " parameters" << endl;
#endif

      if (paramNum == -1)
	continue;

      // this is a deviation from cooper and kennedy because we use
      // invocations on __lmod to replace IMOD sets.
      // detect if we are invoking __lmod.  if so, add the corresponding
      // actual to the caller's IMOD+ set.
      if (!strcmp(qFunc.c_str(), LMODFUNC)) {
	
	string actual = get( boost::edge_dbg_data, g, (*ep.first) ).get_actual();
#ifdef DEBUG_OUTPUT
  cout << "Inserting to IMODPlus " << actual.c_str() << " for "
    << pFunc.c_str() << endl;
#endif
	if (mSideEffectPtr->insertIMODPlus(pFunc.c_str(), actual.c_str())) {
	  mReachedFixedPoint = false;
  }
    


      }

      pair<map_type::const_iterator, map_type::const_iterator> itpair = 
	mSideEffectPtr->lookupRMOD(qFunc.c_str());

#ifdef DEBUG_OUTPUT
      cout << endl << "RMOD for " << qFunc.c_str() << endl;
#endif

      // union b_e(RMOD(q)) [ NB:  restricted to params ]
      for (map_type::const_iterator i = itpair.first; i != itpair.second; ++i) {
	if ( mSideEffectPtr->lookupFormal(qFunc.c_str(), (*i).second) == paramNum ) {
	  string actual = get( boost::edge_dbg_data, g, (*ep.first) ).get_actual();

#ifdef DEBUG_OUTPUT
	  cout << "INSERTING IMODPLUS FROM RMOD: " << actual.c_str() << " in " << pFunc.c_str() << " from " << qFunc.c_str() << " param # " << paramNum << " formal" << (*i).second << endl;
#endif

	  if (mSideEffectPtr->insertIMODPlus(pFunc.c_str(), actual.c_str()))
	    mReachedFixedPoint = false;
	  break;
	}
#ifdef DEBUG_OUTPUT
	cout << "   " << (*i).second << " index: " << mSideEffectPtr->lookupFormal(qFunc.c_str(), (*i).second) << endl;;
#endif
      }
    }

    // union the IMOD of this node
    pair<map_type::const_iterator, map_type::const_iterator> itpair = 
      mSideEffectPtr->lookupIMOD(pFunc.c_str());
   
    for (map_type::const_iterator i = itpair.first; i != itpair.second; ++i) {
#ifdef DEBUG_OUTPUT
      cout << "INSERTING IMODPLUS FROM IMOD: " << (*i).second << " in " << pFunc.c_str() << endl;
#endif
      if (mSideEffectPtr->insertIMODPlus( pFunc.c_str(), (*i).second ))
	mReachedFixedPoint = false;
    }

  }

  bool mReachedFixedPoint;

 private:
  SideEffect *mSideEffectPtr;
};

#if 0
// implements Figure 2
static int nextdfn;
deque<callVertex> vertexStack;

// hash_set<Key, HashFcn, EqualKey, Alloc>
hash_set<int, boost::hash<int>, eqint> shadowStack;

int *lowlink;
int *dfn;
#endif

int 
SideEffect::onStack(int num)
{
  // CH (4/9/2010): Use boost::unordered_set instead
  //hash_set<int, boost::hash<int>, eqint>::const_iterator it = 
    boost::unordered_set<int, boost::hash<int>, eqint>::const_iterator it = 
    shadowStack.find(num);

  return ( it != shadowStack.end() );
}

void 
SideEffect::searchGMOD(CallGraph *g, string pName, callVertex p, 
                       callVertexIndexMap index_map)
{
  int pIndex = get( index_map, p );
  //  dfn[pName.c_str()] = nextdfn;
  //  lowlink[pName.c_str()] = nextdfn;
  dfn[ pIndex ] = nextdfn;
  lowlink[ pIndex ] = nextdfn;
  nextdfn++;

  // GMOD[p] := IMOD+[p]
  pair<map_type::const_iterator, map_type::const_iterator> pr = 
    lookupIMODPlus(pName.c_str());
  
  for (map_type::const_iterator i = pr.first; i != pr.second; ++i) {
#ifdef DEBUG_OUTPUT
    cout << "INSERTING " << (*i).second << " in " << pName << " from IMODPlus" << endl;
#endif
    insertGMOD( pName.c_str(), (*i).second );
  }

#ifdef DEBUG_OUTPUT
  cout << "   PUSHING " << pName << endl;
#endif
  // push p on stack
  //  vertexStack.push_front(pIndex);
  vertexStack.push_front(p);
  shadowStack.insert(pIndex);

  // foreach q adjacent to p
  // NB:  i think this means (p,q) is in the graph
  typedef boost::graph_traits<CallGraph>::out_edge_iterator out_edge_iter;

  typedef boost::graph_traits<CallGraph>::adjacency_iterator adj_it;
  pair<adj_it, adj_it> adj_pair;
  for (adj_pair = adjacent_vertices(p, *g); adj_pair.first != adj_pair.second;
       ++adj_pair.first) {
    // NB:  consider indexing hashes, etc with numbers via index_map.
    //      tricky part:  keeping indexing consistent across graphs
    callVertex q = *(adj_pair.first);
    int qIndex = get( index_map, q );
    string qName(get( boost::vertex_dbg_data, *g, q ).get_functionName());
#ifdef DEBUG_OUTPUT
    cout << "q: " << qName << " adjacent to p: " << pName;
#endif
    if ( dfn[ qIndex ] == 0 ) {
      // handle tree edge
#ifdef DEBUG_OUTPUT
      cout << " tree edge " << endl;
#endif
      string qName(get( boost::vertex_dbg_data, *g, q ).get_functionName());
      searchGMOD(g, qName, q, index_map);
      lowlink[ pIndex ] = min(lowlink[ pIndex ], lowlink[ qIndex ]);
    }
    if ( ( dfn[ qIndex ] < dfn[ pIndex ] ) && ( onStack(qIndex) ) ) {
      // cross or back edge, same scc
#ifdef DEBUG_OUTPUT
      cout << " cross or back edge " << endl;
#endif
      lowlink[ pIndex ] = min(dfn[ qIndex ], lowlink[ pIndex ]);
    } else {
#ifdef DEBUG_OUTPUT
      cout << " equation 4 " << endl;
#endif
      // apply equation 4
      // GMOD[p] := GMOD[p] U (GMOD[q] intersect NOT LOCAL[q])

      pair<map_type::const_iterator, map_type::const_iterator> gmodq_maps = 
	lookupGMOD(qName.c_str());

      pair<map_type::const_iterator, map_type::const_iterator> local_maps =
	lookupLocal(qName.c_str());

      deque<const char *> gmodq_q;
      for (map_type::const_iterator i = gmodq_maps.first; 
           i != gmodq_maps.second; ++i) {
        gmodq_q.push_front((*i).second);
      }

      sort(gmodq_q.begin(), gmodq_q.end(), ltstr());

      deque<const char *> localq_q;
      for (map_type::const_iterator i = local_maps.first; 
           i != local_maps.second; ++i) {
        localq_q.push_front((*i).second);
      }

      sort(localq_q.begin(), localq_q.end(), ltstr());

      deque<const char *>::iterator qIt;
#ifdef DEBUG_OUTPUT
      cout << "GMOD for " << qName << " ";
      for (qIt = gmodq_q.begin(); qIt != gmodq_q.end(); qIt++) 
	cout << (*qIt) << " ";
      cout << endl;

      cout << "LOCALS for " << qName << " ";
      for (qIt = localq_q.begin(); qIt != localq_q.end(); qIt++) 
	cout << (*qIt) << " ";
      cout << endl;
#endif

      deque<const char *> intersection;

      set_difference(gmodq_q.begin(), gmodq_q.end(),
                     localq_q.begin(), localq_q.end(),
		     inserter(intersection, intersection.begin()),
		     ltstr());

      for (qIt = intersection.begin();
           qIt != intersection.end();
	   qIt++) {
#ifdef DEBUG_OUTPUT
	cout << "INSERTING " << (*qIt) << " in " << pName << " via intersection of " << qName << endl;
#endif
	insertGMOD(pName.c_str(), (*qIt));
      }
    }
  } // end foreach q adjacent to p

  // test for the root of a strong component
  if (lowlink[ pIndex ] == dfn[ pIndex ]) {

    // adjust GMOD sets for each member of the scc
    int uIndex;
    do {
      // pop u from the stack 
      callVertex u = vertexStack.front();
      vertexStack.pop_front();
      uIndex = get( index_map, u );
      shadowStack.erase(uIndex);

      // GMOD[u] := GMOD[u] U (GMOD[p] intersect NOT LOCAL[p]);
      pair<map_type::const_iterator, map_type::const_iterator> gmodp_maps = 
	lookupGMOD(pName.c_str());

      pair<map_type::const_iterator, map_type::const_iterator> localp_maps =
	lookupLocal(pName.c_str());

      //      set<const char *, ltstr> intersection;
      set<pair<const char *, const char *>, pairltstr> intersection2;
      //      set<pair<const char *, const char *>, pairltstr>::iterator endIter;
      deque<const char *>::iterator qIt;

      deque<const char *> gmodp_q;
      for (map_type::const_iterator i = gmodp_maps.first; 
           i != gmodp_maps.second; ++i) {
        gmodp_q.push_front((*i).second);
      }

      sort(gmodp_q.begin(), gmodp_q.end(), ltstr());

      deque<const char *> localp_q;
      for (map_type::const_iterator i = localp_maps.first; 
           i != localp_maps.second; ++i) {
        localp_q.push_front((*i).second);
      }

      sort(localp_q.begin(), localp_q.end(), ltstr());

#ifdef DEBUG_OUTPUT
      cout << "GMOD for " << pName << " ";
      for (qIt = gmodp_q.begin(); qIt != gmodp_q.end(); qIt++) 
	cout << (*qIt) << " ";
      cout << endl;

      cout << "LOCALS for " << pName << " ";
      for (qIt = localp_q.begin(); qIt != localp_q.end(); qIt++) 
	cout << (*qIt) << " ";
      cout << endl;
#endif

      deque<const char *> intersection;

      set_difference(gmodp_q.begin(), gmodp_q.end(),
                     localp_q.begin(), localp_q.end(),
		     inserter(intersection, intersection.begin()),
		     ltstr());

      string uName(get( boost::vertex_dbg_data, *g, u ).get_functionName());
#ifdef DEBUG_OUTPUT
      cout << "    Popping " << uName << endl;
#endif
      for (qIt = intersection.begin();
           qIt != intersection.end();
	   qIt++) {
#ifdef DEBUG_OUTPUT
	cout << "INSERTING " << (*qIt) << " in uname " << uName << " via intersection of " << pName << endl;
#endif
	insertGMOD(uName.c_str(), (*qIt));
      }

    } while ( uIndex != pIndex );
    
  }

}

void 
SideEffect::solveGMOD(CallGraph *g)
{
  int i;

  // handle pseudo-function __lmod
  insertGMOD(LMODFUNC, LMODFORMAL);

  // assume that IMOD+ and LOCAL have been initialized
  nextdfn = 1;

  i = num_vertices(*g);

  lowlink = new int[i];
  dfn     = new int[i];

  for(; i > 0; i--)
    dfn[i-1] = 0;

  callVertexIndexMap index_map = get(boost::vertex_index, *g);

  // search root (main)
  string mainFunc = getCallRootName();
  searchGMOD(g, mainFunc, mCallRoot, index_map);

  delete [] lowlink;
  delete [] dfn;
}


class solve_dmod : public boost::base_visitor<solve_dmod> {
 public:
  typedef boost::on_examine_edge event_filter;

  solve_dmod(SideEffect *setSideEffect) :
    mReachedFixedPoint(false),
    mSideEffectPtr(setSideEffect)
  { }

  template <class Edge, class G>
  void operator()(Edge e, G& g) {
    string site = get( boost::edge_dbg_data, g, e ).get_site();

    // union b_e(GMOD(q))
    // NB:  this could be much more efficient
    callVertex q = boost::target( e, g );
    string qFunc = get( boost::vertex_dbg_data, g, q ).get_functionName();

    pair<map_type::const_iterator, map_type::const_iterator> itpair = 
      mSideEffectPtr->lookupGMOD(qFunc.c_str());

    int paramNum = get( boost::edge_dbg_data, g, e ).get_ordinal();
    //    int scope = get( boost::edge_dbg_data, g, e ).get_scope();
    string actual = get( boost::edge_dbg_data, g, e ).get_actual();

    int inserted = 0;    

    // this function gets called for each argument passed between call
    // sites.  but we only want to insert the global vars from gmod
    // into dmod once.  so detect the first time we visit this call site
    // and do it then.
    pair<map_type::const_iterator, map_type::const_iterator> dmodpair = 
      mSideEffectPtr->lookupDMOD(site.c_str());
    int do_global = 0;
    
    if (dmodpair.first == dmodpair.second)
      do_global = 1;

    if ( strcmp( actual.c_str(), "void") ) {
      for (map_type::const_iterator i = itpair.first; i != itpair.second; ++i) {
#ifdef DEBUG_OUTPUT
	cout << "checking dmodpair " << (*i).second << " in " << qFunc.c_str() << " as paramNum " << paramNum << endl;
#endif	

	if ( mSideEffectPtr->lookupFormal( qFunc.c_str(), (*i).second ) == paramNum ) {
#ifdef DEBUG_OUTPUT
	  cout << "INSERTING DMOD: " << actual.c_str() << " in " << site.c_str() << " from " << qFunc.c_str() << " param # " << paramNum << " formal " << (*i).second << endl;
#endif
	  if (mSideEffectPtr->insertDMOD( site.c_str(), actual.c_str()) ) {
	    inserted = 1;
	    mReachedFixedPoint = false;
	  }
	  break;
	} 
#ifdef DEBUG_OUTPUT
  else
  {
    cout << "dmodpair does not match formal param pos " <<
	mSideEffectPtr->lookupFormal( qFunc.c_str(), (*i).second ) << endl;
  }
#endif
      }
    }

    if (!do_global)
      return;
    
    pair<map_type::const_iterator, map_type::const_iterator> p = 
      mSideEffectPtr->lookupLocal( qFunc.c_str() );

    for (map_type::const_iterator i = itpair.first; i != itpair.second; ++i) {

      int local = 0;
    
      for (map_type::const_iterator i2 = p.first; i2 != p.second; ++i2)
	if ( !strcmp( (*i).second, (*i2).second ) ) {
	  local = 1;
	  break;
	}
      
      if (!local) {
	if ( mSideEffectPtr->insertDMOD( site.c_str(), (*i).second ) )
	  mReachedFixedPoint = false;
      }
    }
    
  }
  
  bool mReachedFixedPoint;
 private:
  SideEffect *mSideEffectPtr;
};

class print_dmod : public boost::base_visitor<print_dmod> {
 public:
  typedef boost::on_examine_edge event_filter;

  print_dmod(SideEffect *setSideEffect) :
    mSideEffectPtr(setSideEffect)
  { }

  template <class Edge, class G>
  void operator()(Edge e, G& g) {
    string site = get( boost::edge_dbg_data, g, e ).get_site();

    pair<map_type::const_iterator, map_type::const_iterator> itpair = 
      mSideEffectPtr->lookupDMOD(site.c_str());

    callVertex q = boost::target( e, g );
    string qFunc = get( boost::vertex_dbg_data, g, q ).get_functionName();
    string actual = get( boost::edge_dbg_data, g, e ).get_actual();

    cout << "DMOD(" << site << ") [tar: " << qFunc << " actual: " << actual << "] = ";
    for (map_type::const_iterator i = itpair.first; i != itpair.second; ++i) 
      cout << (*i).second << " ";
    cout << endl;

  }

 private:
  SideEffect *mSideEffectPtr;
};

CallGraph *
createSimpleCallGraph(CallGraph *callgraph, long projectId, 
		      sqlite3_connection *db)
{

  CallGraph *simpleCallgraph = new CallGraph( projectId, 
					      GTYPE_SIMPLECALLGRAPH, db );

  assert( simpleCallgraph != NULL );

  typedef boost::graph_traits<CallGraph>::vertex_iterator vertex_iter;
  pair<vertex_iter, vertex_iter> vp;

  for (vp = vertices(*callgraph); vp.first != vp.second; ++vp.first) {
      simpleFuncRow v = get( boost::vertex_dbg_data, *callgraph, *vp.first );
      simpleCallgraph->insertVertex( v, v.get_functionName() );
  }

  typedef boost::graph_traits<CallGraph>::edge_iterator edge_iter;
  pair<edge_iter, edge_iter> ep;

  for (ep = edges(*callgraph); ep.first != ep.second; ++ep.first) {
    int paramNum = get( boost::edge_dbg_data, *callgraph, *ep.first ).get_ordinal();

    // we only want to take one edge between a caller and callee.
    // for a call edge with one or more parameters take the first
    // parameter (0th param).  for an edge that represents an invocation
    // of a void function, there is a dummy parameter with a paramNum of -1.
    if ( ( paramNum == 0 ) || ( paramNum == -1 ) ) {
      simpleFuncRow src = get( boost::vertex_dbg_data, *callgraph, boost::source(*(ep.first), *callgraph) );
      simpleFuncRow tar = get( boost::vertex_dbg_data, *callgraph, boost::target(*(ep.first), *callgraph) );

      callEdgeRow edge = get( boost::edge_dbg_data, *callgraph, *ep.first );
      simpleCallgraph->insertEdge( src, tar, edge );      
    }
  }

  return simpleCallgraph;
}

// milki (06/30/2010) Strip extension AND path
// Long paths interfered with table creation
string
stripFileExtension(string fileName)
{
  string stripped = stripPathFromFileName(fileName);
  stripped = stripFileSuffixFromFileName(stripped);
  return stripped;
}

// milki (06/30/2010) Strips extension but leaves full path
#if 0
// remove the extension from a single file
string
stripFileExtension(string fileName)
{
  int len = fileName.length();
  do {

    len--;

    if (len <= 0) {
      cerr << "Expected an extension in file name " << fileName << endl;
      exit(-1);
    }

  } while(fileName[len] != '.');

  string stripped(fileName, 0, len);
  return stripped;
}
#endif

// remove the extension from each file name 
set<string>
stripFileExtensions(list<string> fileNames)
{
  set<string> strippedFileNames;

  for(list<string>::iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
    strippedFileNames.insert(stripFileExtension(*it));
  }

  return strippedFileNames;
}

#if 0
// DQ (9/2/2008): This is not used and redundant with functionality on the SgProject IR node.
list<string>
getFileNames(SgFilePtrListPtr fileList)
{
  list<string> fileNames;

  for(SgFilePtrList::iterator it = fileList->begin();
      it != fileList->end();
      ++it) {
    fileNames.push_back((*it)->getFileName());
  }

  return fileNames;
}
#endif

string
sanitizeFileName(string fileName)
{
  string sanitized(fileName);

  for(unsigned int i = 0; i < sanitized.size(); ++i) {
    if ( (sanitized[i] == '.') || (sanitized[i] == '-') )
      sanitized[i] = '_';
  }

  return sanitized;
}

//-----------------------------------------------------------------------------
// build a simple callgraph - restrictions: e.g. function pointers, virtual 
// functions, recursive calls etc.
#if 0
int 
SideEffect::calcSideEffect(SgProject* project) 
{
  int i;
  
  // we will parse the source files, populating a database with
  // information gleaned from each source file.  

  // the object or library files should already have database entries
  // with the corresponding names.  these databases will be accessed
  // to populate the corresponding data structures.

  // get the list of files associated with this project
  SgFilePtrListPtr fileList = project->get_fileList();
  list<string> sourceFileNames = project->get_sourceFileNameList();

  // strip the extensions off of the source file names
  set<string> strippedSourceFileNames = stripFileExtensions(sourceFileNames);

  // sort the list of file names so we can do a set difference
  //  sort(strippedSourceFileNames.begin(), strippedSourceFileNames.end());

  long lmodId = -1;

  CallGraph *simpleCallgraph;
  CallGraph **callgraphs = new CallGraph *[fileList->size()];
  CallGraph *callgraph;
  sqlite3_connection **dbs = new sqlite3_connection *[fileList->size()];

  // if there is only one file to be processed, we can short circuit a lot of
  // this generality
  separateCompilation = ( fileList->size() != 1 );

  // create the toplevel database.  
  // we will create a database for each source file.  this database
  // will contain the call graph of that source file.
  // each of these call graphs will be merged into one aggregate call
  // graph that is stored in the toplevel database.  we keep separate
  // call graphs/databases to allow for separate compilation.
  // call graphs, tables, etc. are aggregated into one toplevel database
  // to facilitate whole program analysis-- i.e., just sweep over a 
  // single table/graph.
  sqlite3_connection toplevelDb;
  
  // set the name of the toplevel database to be the name of the 
  // output file

  // sql doesn't like hyphens and dots, change them to underscores
  string sanitizedOutputFileName = sanitizeFileName(project->get_outputFileName());

  toplevelDb.setDatabaseParameters(NULL, NULL, NULL, (char *)sanitizedOutputFileName.c_str());

  int initOk =  toplevelDb.initialize();
  assert(initOk == 0);

  CREATE_TABLE(toplevelDb, projects);
  CREATE_TABLE(toplevelDb, graphdata);
  CREATE_TABLE(toplevelDb, graphnode);
  CREATE_TABLE(toplevelDb, graphedge);

  string toplevelProjectName = "testProject"; 
  projectsRowdata toplevelProw( UNKNOWNID ,toplevelProjectName, UNKNOWNID );
  projects.retrieveCreateByColumn( &toplevelProw, "name", 
                                   toplevelProjectName );
  long toplevelProjectId = toplevelProw.get_id();

  // local table holds entries of the form < func, var > where var
  // is defined locally with function func.
  TableAccess< localVarsRowdata > localVars(&toplevelDb);
  localVars.initialize();
  
  // formals table holds entries of the form < func, formal, ordinal >
  // where formal is the ordinal'th argument to function func.
  TableAccess< formalsRowdata > formals(&toplevelDb);
  formals.initialize();

  TableAccess< sideEffectTableRowdata > sideEffect(&toplevelDb); 
  sideEffect.initialize(); 

  // create the aggregate simple call graph
  if (!separateCompilation) {
    simpleCallgraph = new CallGraph( toplevelProjectId, GTYPE_SIMPLECALLGRAPH, &toplevelDb );
  } else {
    simpleCallgraph = NULL;
  }

  // list of functions defined and called as parsed by a traversal.
  set<const char *, ltstr> definedFuncs;
  //  set<const char *, ltstr> calledFuncs;

  long projectId = toplevelProjectId;

  vector<string> strippedFileNames(fileList->size());

  // iterate over all of the files in the project
  i = 0;
  for(SgFilePtrList::iterator it = fileList->begin();
      it != fileList->end();
      ++it, ++i) {

    string fileName = stripFileExtension((*it)->getFileName());
    cout << "Setting gdb for " << fileName.c_str() << endl;

    strippedFileNames[i] = fileName;

    // create a new database for this file
    dbs[i] = new sqlite3_connection();

    sqlite3_connection *db = dbs[i];
    
    // set the name of this database to correspond to the file
    db->setDatabaseParameters(NULL, NULL, NULL, (char *)fileName.c_str());
    
    // create a call graph for this file
    callgraphs[i] = new CallGraph(projectId, GTYPE_SIMPLECALLGRAPH, db);

    if (strippedSourceFileNames.find(fileName) == strippedSourceFileNames.end()) {
      // this must be an object or library file.  we should already have
      // created a database for it.  connect to that database and read
      // the call graph from it.
      // NB:  don't drop the database
      int drop = 0;
      int initOk =  db->initialize(drop);
      assert(initOk == 0);

      callgraphs[i]->loadFromDatabase();

    } else {

      // this is a source file.  we need to create a database for it, and
      // populate that database and a callgraph by parsing the file.
      // NB:  _do_ drop the database if it already exists
      int drop = 1;
      int initOk =  db->initialize(drop);
      assert(initOk == 0);

      // setup the call graph tables
      CREATE_TABLE(*db, projects);
      CREATE_TABLE(*db, graphdata);
      CREATE_TABLE(*db, graphnode);
      CREATE_TABLE(*db, graphedge);
      
      // create/initialize a table with entries < func > where func is
      // a function defined or called within the file
      TableAccess< simpleFuncTableRowdata > simpleFunc(db);
      simpleFunc.initialize();
      
      // create/initialize a table to hold the data for the call graph edges
      TableAccess< callEdgeRowdata > callEdge(db);
      callEdge.initialize();
      
      // create/initialize a table with entries < func, var > where
      // var is defined locally within function func and func is 
      // defined within this file.
      TableAccess< localVarsRowdata > localVars(db);
      localVars.initialize();
      
      // create/initialize a table with entries < func, formal, ordinal >
      // where formal is the ordinal'th argument to function func,
      // a function defined within this file.
      TableAccess< formalsRowdata > formals(db);
      formals.initialize();
      
#if 0
      AstDOTGeneration dotgen;
      dotgen.generateInputFiles(project, AstDOTGeneration::PREORDER);
#endif

      // traverse the AST derived from the source tree to populate
      // the call graph and the above tables
#ifdef DEBUG_OUTPUT
      cout << "Traversing AST to populate call graph and tables" << endl;
#endif
      MyTraversal treeTraversal(projectId, db, callgraphs[i], simpleCallgraph,
				&definedFuncs, &mCalledFuncs, this);
      MyInheritedAttribute inheritedAttribute(project);
      treeTraversal.traverse(project, inheritedAttribute);
      
      if (!separateCompilation)
	lmodId = treeTraversal.getLMODId();
      
      // save graph to database
      callgraphs[i]->writeToDatabase();
    }
  }  

  if (!separateCompilation) {
    
    // special case where there is only one file.  we can short-circuit
    // the creation of an aggregate graph.

    callgraph = callgraphs[0];

  } else {

    // create a new call graph that will be the aggregate of each
    callgraph = new CallGraph(toplevelProjectId, GTYPE_SIMPLECALLGRAPH, &toplevelDb);

    assert( callgraph != NULL );
    
    // loop over each individual callgraph to insert the vertices
    for (i = 0; i < strippedFileNames.size(); ++i) {

      typedef boost::graph_traits<CallGraph>::vertex_iterator vertex_iter;
      pair<vertex_iter, vertex_iter> vp;

      // loop over the vertices in this callgraph.  
      for (vp = vertices(*callgraphs[i]); vp.first != vp.second; ++vp.first) {

	simpleFuncTableRowdata v( get( boost::vertex_dbg_data, *callgraphs[i], *vp.first ) );

	// keep track of which file defines a given function.
	// this vertex was defined in this file if it is a source to any
	// edge in this callgraph.  (else, we would not be able
	// to see the implementation to know what it called.)
	// currently i do not handle static functions.
	// signal if we ever have a case where a function is defined
	// multiple times; we'll have to handle it.
	typedef boost::graph_traits<CallGraph>::adjacency_iterator adj_it;
	pair<adj_it, adj_it> adj_pair;
	adj_pair = adjacent_vertices(*(vp.first), *callgraphs[i]); 
	
	if ( adj_pair.first != adj_pair.second ) {

	  if ( !insertFuncToFile(v.get_functionName().c_str(), strippedFileNames[i].c_str()) ) {
	    cerr << "Function " << v.get_functionName() << " defined multiple times." << endl;
	    cerr << "Possibly defined as static or overloaded" << endl;
	    exit(-1);
	  }

	}

	// try to insert this function in our hash table.  the insertion
	// returns the id of either an existing entry or the
	// newly inserted entry.  we may have already inserted this
	// function from a previous callgraph.
	long id = insertFuncToId(v.get_functionName().c_str(), nextAggCallGraphId);
	v.set_id(id);

	if (id == nextAggCallGraphId) {
	  
	  // first time we've seen this function
	  nextAggCallGraphId++;
	  callgraph->insertVertex(v, v.get_functionName());

	}

      }

    }

    // loop over each individual callgraph to insert the edges
    for (i = 0; i < strippedFileNames.size(); ++i) {

      typedef boost::graph_traits<CallGraph>::edge_iterator edge_iter;
      pair<edge_iter, edge_iter> ep;
      
      // loop over the edges in the callgraph
      for (ep = edges(*callgraphs[i]); ep.first != ep.second; ++ep.first) {
	
	simpleFuncTableRowdata src(get(vertex_dbg_data, *callgraphs[i], 
                                       boost::source(*(ep.first), *callgraphs[i])));


	// keep track of which file defines a given function.
	// this vertex was defined in this file if it is a source to any
	// edge in this callgraph.  (else, we would not be able
	// to see the implementation to know what it called.)
	// currently i do not handle static functions.
	// signal if we ever have a case where a function is defined
	// multiple times; we'll have to handle it.
	if (!insertFuncToFile(src.get_functionName().c_str(), strippedFileNames[i].c_str())) {
	  cerr << "Function " << src.get_functionName() << " defined multiple times." << endl;
	  cerr << "Possibly defined as static or we aren't handling overloaded functions" << endl;
	  exit(-1);
	}

	// try to insert this function in our hash table.  the insertion
	// returns the id of either an existing entry or the
	// newly inserted entry.  we may have already inserted this
	// function from a previous callgraph.
	long srcId = insertFuncToId(src.get_functionName().c_str(), nextAggCallGraphId);
	src.set_id(srcId);

	if (srcId == nextAggCallGraphId) {
	  
	  // first time we've seen this function
	  nextAggCallGraphId++;
	  callgraph->insertVertex(src, src.get_functionName());

	}

	simpleFuncTableRowdata tar(get(vertex_dbg_data, *callgraphs[i], 
                                       boost::target(*(ep.first), *callgraphs[i])));

	// insert target vertex
	long tarId = insertFuncToId(tar.get_functionName().c_str(), nextAggCallGraphId);
	tar.set_id(tarId);

	if (tarId == nextAggCallGraphId) {
	  
	  // first time we've seen this function
	  nextAggCallGraphId++;
	  callgraph->insertVertex(tar, tar.get_functionName());

	}

	callEdgeRowdata edge(get(edge_dbg_data, *callgraphs[i], *ep.first));
	edge.set_id(++nextAggCallGraphId);

	callgraph->insertEdge(src, tar, edge);

      }
    }
  }

  vector<const char *> cFuncs;
  vector<const char *> dFuncs;
  
  copy(mCalledFuncs.begin(), mCalledFuncs.end(), 
       inserter(cFuncs, cFuncs.begin()));
  copy(definedFuncs.begin(), definedFuncs.end(), 
       inserter(dFuncs, dFuncs.begin()));
  sort(cFuncs.begin(), cFuncs.end(), ltstr());
  sort(dFuncs.begin(), dFuncs.end(), ltstr());
  
  typedef set<const char *, ltstr>::iterator funcIter;
  funcIter setIt;
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "Defined Funcs: " << endl;
  for (setIt = definedFuncs.begin();
       setIt != definedFuncs.end();
       setIt++) {
    cout << "Defined functions: " << *setIt << endl;
  }
  
  cout << endl << endl << "Called Funcs: " << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    cout << "Called functions: " << *setIt << endl;
  }
#endif
  
  vector<const char *> undefinedFuncs;
  set_difference(cFuncs.begin(), cFuncs.end(),
                 dFuncs.begin(), dFuncs.end(),
		 inserter(undefinedFuncs, undefinedFuncs.begin()),
		 ltstr());
  
  int undefined = 0;
  long uniq = 0;

  // get handle to __lmod pseudo-function
  string s = LMODFUNC;

  if (separateCompilation) 
    lmodId = lookupIdByFunc( s.c_str() );

  assert( lmodId >= 0 );

  simpleFuncTableRowdata __lmod( lmodId, -1, s );

  //  TableAccess< formalsRowdata > formals( &toplevelDb );
  //  formals.initialize();

  vector<const char *>::iterator vIt;
  
  // iterate through all undefined functions that were
  // called in some traversal but never defined
  // try to find them in the side effect table
  for (vIt = undefinedFuncs.begin();
       vIt != undefinedFuncs.end();
       vIt++) {

    string cmd = "functionName = ''";
    char tmp[cmd.length() + 256];
    sprintf(tmp, "functionName = '%s'", *vIt);
    cmd = tmp; 
    
    vector<sideEffectTableRowdata> rows;
    
    rows = sideEffect.select(cmd);
    if (rows.size() <= 0) {

      // could not find function in side-effect table
      cout << "Undefined name: " << *vIt << endl;
      undefined = 1;

    } else {

      typedef vector<sideEffectTableRowdata>::iterator rowIter;

      // function was found.  the table entries
      // consist of variables that are modified by
      // this function.  foreach such variable insert
      // a dummy edge in the callgraph from the
      // function to __lmod.  subsequent code will
      // recognize this as a destructive assign.
      for (rowIter rIt = rows.begin(); rIt != rows.end(); rIt++) {

	int paramNum = rIt->get_arg();
	if (paramNum == -1)
	  continue;

	string s = (*vIt);

	long id = lookupIdByFunc( s.c_str() );
	assert( id > 0 );

	simpleFuncTableRowdata src( id, -1, s );

	char tmp[s.length() + 256];
#if 0
	// file_info is broken for expressions
	sprintf(tmp, "%s-%s-%d-%d", s.c_str(), 
		assign->get_file_info()->get_filename(), 
		assign->get_file_info()->get_line(), 
		assign->get_file_info()->get_col());
#endif
	//	const char *file = lookupFileByFunc( s.c_str() );
	sprintf(tmp, "%s-sideeffect-%ld", s.c_str(), uniq++);
	string edgeStr = tmp; 


	// put < func, formal, pos > entry in database to signify
	// that formal is the pos'th argument of func.
	// since we don't have access to the implementation of this
	// function (only its sideeffects), we don't know the names
	// of the formal parameters.  no matter; just name them
	// according to their position.
	// NB:  a param of -1 in the side-effect table significes that
	//      there are no side effects for this function.  such
	//      a dummy entry is necessary so that we know that the
	//      user is aware of this function and has verified there
	//      are no side effects, as opposed to their being 
	//      ignorant of a function which may have side effects.

	// build map from func -> formal -> param num
	char *formalStr = new char[256];
	sprintf(formalStr, "param%d", paramNum);
	string formal = formalStr;

	// put < func, formal > entry in database to signify
	// that formal is local to func
	// (since it is a formal parameter)
	//	TableAccess< localVarsRowdata > localVars( &toplevelDb );
	localVarsRowdata localVar( UNKNOWNID , toplevelProjectId, 
                                   s.c_str(), 
				   formal.c_str());

	string columns[3];
	string names[3];
	
	columns[0] = "functionName";
	names[0]   = localVar.get_functionName();
	columns[1] = "varName";
	names[1]   = localVar.get_varName();
	localVars.retrieveCreateByColumns( &localVar, columns, names, 2,
                                           localVar.get_projectId() );

	//	if (!separateCompilation) { }
	  insertLocal(s.c_str(), 
		      formal.c_str());

	// put < func, formal, pos > entry in database to signify
	// that formal is the pos'th argument of func.
	  //	TableAccess< formalsRowdata > formals( &toplevelDb );
	formalsRowdata formalParam( UNKNOWNID , toplevelProjectId, 
				    s.c_str(), 
				    formal.c_str(),
				    paramNum);
	
	columns[0] = "functionName";
	names[0]   = formalParam.get_functionName();
	columns[1] = "formal";
	names[1]   = formalParam.get_formal();
	columns[2] = "ordinal";
	char tmpStr[MAXSTRINGSZ];
	sprintf(tmpStr, "%d", formalParam.get_ordinal());
	names[2]   = strdup(tmpStr);

	formals.retrieveCreateByColumns( &formalParam, columns, names, 3,
                                         formalParam.get_projectId() );

#ifdef DEBUG_OUTPUT
	cout << "INSERTING FORMAL " << formal.c_str() << " in " << s.c_str() << " as paramNum " << paramNum << endl;
#endif

	//	if (!separateCompilation) { }
	  insertFormal(s.c_str(), 
                       formal.c_str(), 
		       paramNum);

	  insertFormalByPos(s.c_str(), 
			    formal.c_str(), 
			    paramNum);

#if 0
	insertFormal(s.c_str(), formalStr, paramNum);
	insertFormalByPos(s.c_str(), formalStr, paramNum);
#endif
	// NB:  a bit of an assumption here:  that all side-effects in the able
	//      are through parameters (i.e. scope == SCOPE_PARAM)
	//	callEdgeRowdata edge( ++nextAggCallGraphId, -1, s, formalStr, SCOPE_PARAM, 0 );
	callEdgeRowdata edge( UNKNOWNID, toplevelProjectId, edgeStr, formalStr, SCOPE_PARAM, 0 );
	    
	callgraph->insertEdge( src, __lmod, edge );
	if (!separateCompilation) 
	  simpleCallgraph->insertEdge( src, __lmod, edge );

      }

    }
    
  }
  
  if (undefined) {
    for (i = 0; i < strippedFileNames.size(); ++i)
      dbs[i]->shutdown();
    toplevelDb.shutdown();
    exit(-1);
  }
  
  if (separateCompilation) {
    //    assert(1 == 0);
    // dummy fields for projectId and &db
    simpleCallgraph = createSimpleCallGraph(callgraph, toplevelProjectId, &toplevelDb);
  }

  if (separateCompilation) {
    //    assert(1 == 0);
    for (i = 0; i < strippedFileNames.size(); ++i) {
      
      sqlite3_connection *db = dbs[i];
      
      populateLocalVarsFromDB(db);
      populateFormalsFromDB(db);
      
    }  
  }

  //  callgraph->writeToDOTFile( "call.dot" );
  //  simpleCallgraph->writeToDOTFile( "scall.dot" );

  // create the multigraph by traversing the callgraph
  // use dummy values for projectId and &db
    CallMultiGraph *multigraph = createMultiGraph(callgraph, toplevelProjectId, &toplevelDb);
  //  multigraph->writeToDOTFile( "mcall.dot" );
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "LOCAL:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    pair<map_type::const_iterator, map_type::const_iterator> p = lookupLocal(*setIt);
    cout << *setIt << ": ";
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " ";
    cout << endl;
  }
  
  cout << endl << endl << "IMOD:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    pair<map_type::const_iterator, map_type::const_iterator> p = lookupIMOD(*setIt);
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " in " << (*i).first << endl;
  }
#endif
  
  // we only have one imod entry.  all other would-be imods are
  // converted into calls to this pseudo-function lmod.  in this
  // manner we can track all assignments in the callgraph by
  // converting them to an invocation of a function which 
  // modifies its parameter.
  insertIMOD(LMODFUNC, LMODFORMAL);
  
  // use dummy values for projectId and &db
  solveRMOD( multigraph, toplevelProjectId, &toplevelDb );
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "RMOD:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    pair<map_type::const_iterator, map_type::const_iterator> p = lookupRMOD(*setIt);
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " in " << (*i).first << endl;
  }
#endif

#if 0
  int imodItr = 0;
  
  solve_imodplus solver(this);
  do {
    solver.mReachedFixedPoint = true;
    cout << "IMOD iter " << imodItr++ << endl;
    depth_first_search(*callgraph, visitor(make_dfs_visitor(solver)));
  } while (!solver.mReachedFixedPoint);
#else
  solve_imodplus IMODSolver(this);
  depth_first_search(*callgraph, visitor(make_dfs_visitor(IMODSolver)));
#endif
  
  pair<map_type::const_iterator, map_type::const_iterator> p;
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "IMODPlus:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    
    p = lookupIMODPlus(*setIt);
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " in " << (*i).first << endl;
  }

  p = lookupIMODPlus(getCallRootName().c_str());
  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    cout << (*i).second << " in " << (*i).first << endl;
  
  cout << endl << endl << "Locals:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    
    p = lookupLocal(*setIt);
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " in " << (*i).first << endl;
  }
  
  p = lookupLocal(getCallRootName().c_str());
  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    cout << (*i).second << " in " << (*i).first << endl;
#endif

  solveGMOD(simpleCallgraph);
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "GMOD:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    
    p = lookupGMOD(*setIt);
    cout << *setIt << ": ";
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " ";
    cout << endl;
  }
  
  p = lookupGMOD(getCallRootName().c_str());
  cout << "main: ";
  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    cout << (*i).second << " ";
  cout << endl;
#endif  
  solve_dmod DMODSolver(this);

  depth_first_search(*callgraph, visitor(make_dfs_visitor(DMODSolver)));
  
#ifdef DEBUG_OUTPUT
  print_dmod DMODPrinter(this);
  depth_first_search(*simpleCallgraph, visitor(make_dfs_visitor(DMODPrinter)));
#endif  

  for (i = 0; i < strippedFileNames.size(); ++i) {
    dbs[i]->shutdown();
    delete callgraphs[i];
  }

  toplevelDb.shutdown();
  delete dbs;
  delete callgraphs;
  if (separateCompilation)
    delete callgraph;
#if 1
  delete simpleCallgraph;
#endif
  return( 0 );
}
#endif


int 
SideEffect::calcSideEffect(SgProject& project) 
{
  // get the list of files associated with this project
  SgFilePtrList fileList = project.get_fileList();

  list<SgNode*> *nodeList = new list<SgNode*>;

  vector<string> sourceFileNamesV = project.get_sourceFileNameList();
  list<string> sourceFileNames(sourceFileNamesV.begin(),sourceFileNamesV.end());


  // sql doesn't like hyphens and dots, change them to underscores
  string sanitizedOutputFileName = sanitizeFileName(project.get_outputFileName());

  vector<string> nodeListFileNames(fileList.size());

  int i = 0;
  for(SgFilePtrList::iterator it = fileList.begin();
      it != fileList.end();
      ++i, ++it) {
    nodeListFileNames[i] = (*it)->getFileName();
    nodeList->push_back((SgNode*)*it);
  }

  int ret = doSideEffect(nodeList, sourceFileNames, sanitizedOutputFileName,
                         nodeListFileNames);

  delete nodeList;

  return ret;

}

#if 1
int
SideEffect::calcSideEffect(SgFile& file)
{
  list<SgNode*> *nodeList = new list<SgNode*>;
  nodeList->push_back(&file);

  string fileName(file.getFileName());

  list<string> sourceFileNames;
  sourceFileNames.push_back(fileName);

  // sql doesn't like hyphens and dots, change them to underscores
  string sanitizedOutputFileName = sanitizeFileName(fileName);

  vector<string> nodeListFileNames(1);
  nodeListFileNames[0] = fileName;

  int ret = doSideEffect(nodeList, sourceFileNames, sanitizedOutputFileName,
		      nodeListFileNames);

  delete nodeList;

  return ret;
}

int
SideEffect::calcSideEffect(SgNode& node)
{
  list<SgNode*> *nodeList = new list<SgNode*>;
  nodeList->push_back(&node);

  string fileName(node.get_startOfConstruct()->get_filename());

  list<string> sourceFileNames;
  sourceFileNames.push_back(fileName);

  // sql doesn't like hyphens and dots, change them to underscores
  string sanitizedOutputFileName = sanitizeFileName(fileName);

  vector<string> nodeListFileNames(1);
  nodeListFileNames[0] = fileName;

  int ret = doSideEffect(nodeList, sourceFileNames, sanitizedOutputFileName,
		      nodeListFileNames);

  delete nodeList;

  return ret;
}
#endif

int
SideEffect::doSideEffect(list<SgNode*> *nodeList, list<string> &sourceFileNames,
                         string &sanitizedOutputFileName,
			 vector<string> &nodeListFileNames)

{
// DQ (12/1/2010): Fixed warning "warning: comparison between signed and unsigned integer expressions"
// int i;
  size_t i;

  // we will parse the source files, populating a database with
  // information gleaned from each source file.  

  // the object or library files should already have database entries
  // with the corresponding names.  these databases will be accessed
  // to populate the corresponding data structures.

  // strip the extensions off of the source file names
  set<string> strippedSourceFileNames = stripFileExtensions(sourceFileNames);
  sanitizedOutputFileName = stripFileExtension(sanitizedOutputFileName);

  // sort the list of file names so we can do a set difference
  //  sort(strippedSourceFileNames.begin(), strippedSourceFileNames.end());

  int lmodId = -1;

  CallGraph *simpleCallgraph;
  CallGraph **callgraphs = new CallGraph *[nodeList->size()];
#ifdef DEBUG_OUTPUT
  cout << "Callgraph created with " << nodeList->size() << " nodes" << endl;
#endif
  CallGraph *callgraph;
  sqlite3_connection **dbs = new sqlite3_connection *[nodeList->size()];

  // if there is only one file to be processed, we can short circuit a lot of
  // this generality
  separateCompilation = ( nodeList->size() != 1 );

  // create the toplevel database.  
  // we will create a database for each source file.  this database
  // will contain the call graph of that source file.
  // each of these call graphs will be merged into one aggregate call
  // graph that is stored in the toplevel database.  we keep separate
  // call graphs/databases to allow for separate compilation.
  // call graphs, tables, etc. are aggregated into one toplevel database
  // to facilitate whole program analysis-- i.e., just sweep over a 
  // single table/graph.
  sqlite3_connection toplevelDb;
  
  // set the name of the toplevel database to be the name of the 
  // output file

  // sql doesn't like hyphens and dots, change them to underscores
  //  string sanitizedOutputFileName = sanitizeFileName(project->get_outputFileName());

  string toplevelDbfile = sanitizedOutputFileName + ".db";

  toplevelDb.open( toplevelDbfile.c_str() );
  toplevelDb.setbusytimeout(1800 * 1000); // 30 minutes

  // localVars, formals, sideeffect
  createDatabaseTables(toplevelDb, false);

  string toplevelProjectName = "testProject";
  projectsRow toplevelProw(toplevelProjectName);
  toplevelProw.insert(&toplevelDb);

  long toplevelProjectId = toplevelProw.get_id();

  // create the aggregate simple call graph

  if (!separateCompilation) {
    simpleCallgraph = new CallGraph( toplevelProjectId, GTYPE_SIMPLECALLGRAPH, &toplevelDb );
  } else {
    simpleCallgraph = NULL;
  }

  // list of functions defined and called as parsed by a traversal.
  set<const char *, ltstr> definedFuncs;
  //  set<const char *, ltstr> calledFuncs;

  long projectId = toplevelProjectId;

  vector<string> strippedFileNames(nodeList->size());

  // iterate over all of the files in the project
  i = 0;
  for(list<SgNode*>::iterator it = nodeList->begin();
      it != nodeList->end();
      ++it, ++i) {

    //    string fileName = stripFileExtension((*it)->getFileName());

    string fileName = stripFileExtension(nodeListFileNames[i]);

    strippedFileNames[i] = fileName;

    // create a new database for this file
    dbs[i] = new sqlite3_connection;

    sqlite3_connection *db = dbs[i];

    // set the name of this database to correspond to the file
    string dbname = fileName + ".db";
    db->open( dbname.c_str() );
    db->setbusytimeout(1800 * 1000); // 30 minutes

    // create a call graph for this file
    // TODO: CALLGRAPH REPLACEMENT
    callgraphs[i] = new CallGraph(projectId, GTYPE_SIMPLECALLGRAPH, db);

    if (strippedSourceFileNames.find(fileName) == strippedSourceFileNames.end()) {
      // this must be an object or library file.  we should already have
      // created a database for it.  connect to that database and read
      // the call graph from it.
      // NB:  don't drop the database
      callgraphs[i]->loadFromDatabase();

    } else {

      // this is a source file.  we need to create a database for it, and
      // populate that database and a callgraph by parsing the file.
      // NB:  _do_ drop the database if it already exists
      // localVars, formals, sideeffect
      createDatabaseTables(*db, true);
      // setup the call graph tables
      //
#if 0
      AstDOTGeneration dotgen;
      dotgen.generateInputFiles(project, AstDOTGeneration::PREORDER);
#endif

      // traverse the AST derived from the source tree to populate
      // the call graph and the above tables
      MyTraversal treeTraversal(projectId, db, callgraphs[i], simpleCallgraph,
				&definedFuncs, &mCalledFuncs, this);
      MyInheritedAttribute inheritedAttribute(*it);
      treeTraversal.traverse(*it, inheritedAttribute);
      
      if (!separateCompilation)
	lmodId = treeTraversal.getLMODId();
      
      // save graph to database
      callgraphs[i]->writeToDatabase();
    }
  }  

  if (!separateCompilation) {
    
    // special case where there is only one file.  we can short-circuit
    // the creation of an aggregate graph.

    callgraph = callgraphs[0];

  } else {

    // create a new call graph that will be the aggregate of each
    callgraph = new CallGraph(toplevelProjectId, GTYPE_SIMPLECALLGRAPH, &toplevelDb);

    assert( callgraph != NULL );
    
    // loop over each individual callgraph to insert the vertices
    for (i = 0; i < strippedFileNames.size(); ++i) {

      typedef boost::graph_traits<CallGraph>::vertex_iterator vertex_iter;
      pair<vertex_iter, vertex_iter> vp;

      // loop over the vertices in this callgraph.  
      for (vp = vertices(*callgraphs[i]); vp.first != vp.second; ++vp.first) {

	simpleFuncRow v = get( boost::vertex_dbg_data, *callgraphs[i], *vp.first );

	// keep track of which file defines a given function.
	// this vertex was defined in this file if it is a source to any
	// edge in this callgraph.  (else, we would not be able
	// to see the implementation to know what it called.)
	// currently i do not handle static functions.
	// signal if we ever have a case where a function is defined
	// multiple times; we'll have to handle it.
	typedef boost::graph_traits<CallGraph>::adjacency_iterator adj_it;
	pair<adj_it, adj_it> adj_pair;
	adj_pair = adjacent_vertices(*(vp.first), *callgraphs[i]); 
	
	if ( adj_pair.first != adj_pair.second ) {

	  if ( !insertFuncToFile(v.get_functionName().c_str(), strippedFileNames[i].c_str()) ) {
	    cerr << "Function " << v.get_functionName() << " defined multiple times." << endl;
	    cerr << "Possibly defined as static or overloaded" << endl;
	    exit(-1);
	  }

	}

	// try to insert this function in our hash table.  the insertion
	// returns the id of either an existing entry or the
	// newly inserted entry.  we may have already inserted this
	// function from a previous callgraph.
	long id = insertFuncToId(v.get_functionName().c_str(), nextAggCallGraphId);
	v.set_id(id);

	if (id == nextAggCallGraphId) {
	  
	  // first time we've seen this function
	  nextAggCallGraphId++;
	  callgraph->insertVertex(v, v.get_functionName());

	}

      }

    }

    // loop over each individual callgraph to insert the edges
    for (i = 0; i < strippedFileNames.size(); ++i) {

      typedef boost::graph_traits<CallGraph>::edge_iterator edge_iter;
      pair<edge_iter, edge_iter> ep;
      
      // loop over the edges in the callgraph
      for (ep = edges(*callgraphs[i]); ep.first != ep.second; ++ep.first) {
	
	simpleFuncRow src = get(boost::vertex_dbg_data, *callgraphs[i], 
                                boost::source(*(ep.first), *callgraphs[i]));


	// keep track of which file defines a given function.
	// this vertex was defined in this file if it is a source to any
	// edge in this callgraph.  (else, we would not be able
	// to see the implementation to know what it called.)
	// currently i do not handle static functions.
	// signal if we ever have a case where a function is defined
	// multiple times; we'll have to handle it.
	if (!insertFuncToFile(src.get_functionName().c_str(), strippedFileNames[i].c_str())) {
	  cerr << "Function " << src.get_functionName() << " defined multiple times." << endl;
	  cerr << "Possibly defined as static or we aren't handling overloaded functions" << endl;
	  exit(-1);
	}

	// try to insert this function in our hash table.  the insertion
	// returns the id of either an existing entry or the
	// newly inserted entry.  we may have already inserted this
	// function from a previous callgraph.
	long srcId = insertFuncToId(src.get_functionName().c_str(), nextAggCallGraphId);
	src.set_id(srcId);

	if (srcId == nextAggCallGraphId) {
	  
	  // first time we've seen this function
	  nextAggCallGraphId++;
	  callgraph->insertVertex(src, src.get_functionName());

	}

	simpleFuncRow tar = get(boost::vertex_dbg_data, *callgraphs[i], 
                                boost::target(*(ep.first), *callgraphs[i]));

	// insert target vertex
	long tarId = insertFuncToId(tar.get_functionName().c_str(), nextAggCallGraphId);
	tar.set_id(tarId);

	if (tarId == nextAggCallGraphId) {
	  
	  // first time we've seen this function
	  nextAggCallGraphId++;
	  callgraph->insertVertex(tar, tar.get_functionName());

	}

	callEdgeRow edge = get(boost::edge_dbg_data, *callgraphs[i], *ep.first);
	edge.set_id(++nextAggCallGraphId);

	callgraph->insertEdge(src, tar, edge);

      }
    }
  }

  vector<const char *> cFuncs;
  vector<const char *> dFuncs;
  
  copy(mCalledFuncs.begin(), mCalledFuncs.end(), 
       inserter(cFuncs, cFuncs.begin()));
  copy(definedFuncs.begin(), definedFuncs.end(), 
       inserter(dFuncs, dFuncs.begin()));
  sort(cFuncs.begin(), cFuncs.end(), ltstr());
  sort(dFuncs.begin(), dFuncs.end(), ltstr());
  
  typedef set<const char *, ltstr>::iterator funcIter;
  funcIter setIt;
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "Defined Funcs: " << endl;
  for (setIt = definedFuncs.begin();
       setIt != definedFuncs.end();
       setIt++) {
    cout << "Defined functions: " << *setIt << endl;
  }
  
  cout << endl << endl << "Called Funcs: " << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    cout << "Called functions: " << *setIt << endl;
  }
#endif
  
  vector<const char *> undefinedFuncs;
  set_difference(cFuncs.begin(), cFuncs.end(),
                 dFuncs.begin(), dFuncs.end(),
		 inserter(undefinedFuncs, undefinedFuncs.begin()),
		 ltstr());
  
  int undefined = 0;
  long uniq = 0;

  // get handle to __lmod pseudo-function
  string s = LMODFUNC;

  if (separateCompilation) 
    lmodId = lookupIdByFunc( s.c_str() );

  assert( lmodId >= 0 );

  //simpleFuncRow __lmod( lmodId, -1, s );
  // need to retrieve or create lmod with id lmodId
  simpleFuncRow __lmod( -1, s );

  //  TableAccess< formalsRowdata > formals( &toplevelDb );
  //  formals.initialize();

  vector<const char *>::iterator vIt;
  
  // iterate through all undefined functions that were
  // called in some traversal but never defined
  // try to find them in the side effect table
  for (vIt = undefinedFuncs.begin();
       vIt != undefinedFuncs.end();
       vIt++) {

    string cmd = "functionName = ''";
    char tmp[cmd.length() + 256];
    sprintf(tmp, "functionName = '%s'", *vIt);
    cmd = tmp; 
    
    vector<sideEffectRow> rows;

    sqlite3_command sideallcmd(toplevelDb,
        "SELECT * FROM " + string(SIDEEFFECTTBL));

    sqlite3_reader sidereader = sideallcmd.executereader();

    while( sidereader.read() ) {
      sideEffectRow row(sidereader);
      rows.push_back(row);
    }

    if (rows.size() <= 0) {

      // could not find function in side-effect table
      cout << "Undefined name: " << *vIt << endl;
      undefined = 1;

    } else {

      typedef vector<sideEffectRow>::iterator rowIter;

      // function was found.  the table entries
      // consist of variables that are modified by
      // this function.  foreach such variable insert
      // a dummy edge in the callgraph from the
      // function to __lmod.  subsequent code will
      // recognize this as a destructive assign.
      for (rowIter rIt = rows.begin(); rIt != rows.end(); rIt++) {

	int paramNum = rIt->get_arg();
	if (paramNum == -1)
	  continue;

	string s = (*vIt);

	long id = lookupIdByFunc( s.c_str() );
	assert( id > 0 );

	//simpleFuncRow src( id, -1, s );
	simpleFuncRow src( -1, s );

	char tmp[s.length() + 256];
#if 0
	// file_info is broken for expressions
	sprintf(tmp, "%s-%s-%d-%d", s.c_str(), 
		assign->get_file_info()->get_filename(), 
		assign->get_file_info()->get_line(), 
		assign->get_file_info()->get_col());
#endif
	//	const char *file = lookupFileByFunc( s.c_str() );
	sprintf(tmp, "%s-sideeffect-%ld", s.c_str(), uniq++);
	string edgeStr = tmp; 


	// put < func, formal, pos > entry in database to signify
	// that formal is the pos'th argument of func.
	// since we don't have access to the implementation of this
	// function (only its sideeffects), we don't know the names
	// of the formal parameters.  no matter; just name them
	// according to their position.
	// NB:  a param of -1 in the side-effect table significes that
	//      there are no side effects for this function.  such
	//      a dummy entry is necessary so that we know that the
	//      user is aware of this function and has verified there
	//      are no side effects, as opposed to their being 
	//      ignorant of a function which may have side effects.

	// build map from func -> formal -> param num
	char *formalStr = new char[256];
	sprintf(formalStr, "param%d", paramNum);
	string formal = formalStr;

	// put < func, formal > entry in database to signify
	// that formal is local to func
	// (since it is a formal parameter)
	//	TableAccess< localVarsRowdata > localVars( &toplevelDb );
	localVarRow localVar(toplevelProjectId, 
                             s.c_str(), 
                             formal.c_str());
  localVar.insert(&toplevelDb);

	//	if (!separateCompilation) { }
	  insertLocal(s.c_str(), 
		      formal.c_str());

	// put < func, formal, pos > entry in database to signify
	// that formal is the pos'th argument of func.
	  //	TableAccess< formalsRowdata > formals( &toplevelDb );
	formalRow formalParam(toplevelProjectId, 
				    s.c_str(), 
				    formal.c_str(),
				    paramNum);
  formalParam.insert(&toplevelDb);
#ifdef DEBUG_OUTPUT
	cout << "INSERTING FORMAL " << formal.c_str() << " in " << s.c_str() << " as paramNum " << paramNum << endl;
#endif

	//	if (!separateCompilation) { }
	  insertFormal(s.c_str(), 
                       formal.c_str(), 
		       paramNum);

	  insertFormalByPos(s.c_str(), 
			    formal.c_str(), 
			    paramNum);

#if 0
	insertFormal(s.c_str(), formalStr, paramNum);
	insertFormalByPos(s.c_str(), formalStr, paramNum);
#endif
	// NB:  a bit of an assumption here:  that all side-effects in the able
	//      are through parameters (i.e. scope == SCOPE_PARAM)
	//	callEdgeRowdata edge( ++nextAggCallGraphId, -1, s, formalStr, SCOPE_PARAM, 0 );
	callEdgeRow edge(toplevelProjectId, edgeStr, formalStr, SCOPE_PARAM, 0 );
	    
	callgraph->insertEdge( src, __lmod, edge );
	if (!separateCompilation) 
	  simpleCallgraph->insertEdge( src, __lmod, edge );

      }
    }
  }
  
  if (undefined) {
    for (i = 0; i < strippedFileNames.size(); ++i)
      dbs[i]->close();
    toplevelDb.close();
    exit(-1);
  }
  
  if (separateCompilation) {
    //    assert(1 == 0);
    // dummy fields for projectId and &db
    simpleCallgraph = createSimpleCallGraph(callgraph, toplevelProjectId, &toplevelDb);
  }

  if (separateCompilation) {
    //    assert(1 == 0);
    for (i = 0; i < strippedFileNames.size(); ++i) {
      
      sqlite3_connection *db = dbs[i];
      
      populateLocalVarsFromDB(db);
      populateFormalsFromDB(db);
      
    }  
  }

  callgraph->writeToDOTFile( "call.dot" );
  simpleCallgraph->writeToDOTFile( "scall.dot" );

  // create the multigraph by traversing the callgraph
  // use dummy values for projectId and &db
  CallMultiGraph *multigraph = createMultiGraph(callgraph, toplevelProjectId, &toplevelDb);
  multigraph->writeToDOTFile( "mcall.dot" );
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "LOCAL:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    pair<map_type::const_iterator, map_type::const_iterator> p = lookupLocal(*setIt);
    cout << *setIt << ": ";
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " ";
    cout << endl;
  }
  
  cout << endl << endl << "IMOD:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    pair<map_type::const_iterator, map_type::const_iterator> p = lookupIMOD(*setIt);
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " in " << (*i).first << endl;
  }
#endif
  
  // we only have one imod entry.  all other would-be imods are
  // converted into calls to this pseudo-function lmod.  in this
  // manner we can track all assignments in the callgraph by
  // converting them to an invocation of a function which 
  // modifies its parameter.
  insertIMOD(LMODFUNC, LMODFORMAL);
  
  // use dummy values for projectId and &db
  solveRMOD( multigraph, toplevelProjectId, &toplevelDb );
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "RMOD:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    pair<map_type::const_iterator, map_type::const_iterator> p = lookupRMOD(*setIt);
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " in " << (*i).first << endl;
  }
#endif

#if 0
  int imodItr = 0;
  
  solve_imodplus solver(this);
  do {
    solver.mReachedFixedPoint = true;
    cout << "IMOD iter " << imodItr++ << endl;
    depth_first_search(*callgraph, visitor(make_dfs_visitor(solver)));
  } while (!solver.mReachedFixedPoint);
#else
  solve_imodplus IMODSolver(this);
  depth_first_search(*callgraph, visitor(make_dfs_visitor(IMODSolver)));
#endif
  
  pair<map_type::const_iterator, map_type::const_iterator> p;
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "IMODPlus:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    
    p = lookupIMODPlus(*setIt);
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " in " << (*i).first << endl;
  }

  p = lookupIMODPlus(getCallRootName().c_str());
  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    cout << (*i).second << " in " << (*i).first << endl;
  
  cout << endl << endl << "Locals:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    
    p = lookupLocal(*setIt);
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " in " << (*i).first << endl;
  }
  
  p = lookupLocal(getCallRootName().c_str());
  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    cout << (*i).second << " in " << (*i).first << endl;
#endif

  solveGMOD(simpleCallgraph);
  
#ifdef DEBUG_OUTPUT
  cout << endl << endl << "GMOD:" << endl;
  for (setIt = mCalledFuncs.begin();
       setIt != mCalledFuncs.end();
       setIt++) {
    
    p = lookupGMOD(*setIt);
    cout << *setIt << ": ";
    for (map_type::const_iterator i = p.first; i != p.second; ++i)
      cout << (*i).second << " ";
    cout << endl;
  }
  
  p = lookupGMOD(getCallRootName().c_str());
  cout << "main: ";
  for (map_type::const_iterator i = p.first; i != p.second; ++i)
    cout << (*i).second << " ";
  cout << endl;
#endif  
  solve_dmod DMODSolver(this);

  depth_first_search(*callgraph, visitor(make_dfs_visitor(DMODSolver)));
  
#ifdef DEBUG_OUTPUT
  print_dmod DMODPrinter(this);
  depth_first_search(*simpleCallgraph, visitor(make_dfs_visitor(DMODPrinter)));

  cout << endl << endl << "|---Pretty printing MODs---|" << endl;
  cout << "LOCALS:" << endl;
  prettyprint(locals);
  cout << "IMODPLUS:" << endl;
  prettyprint(imodplus);
  cout << "RMOD:" << endl;
  prettyprint(rmod);
  cout << "GMOD:" << endl;
  prettyprint(gmod);
  cout << "LMOD:" << endl;
  prettyprint(lmod);
  cout << "DMOD:" << endl;
  prettyprint(dmod);
#endif  

  for (i = 0; i < strippedFileNames.size(); ++i) {
    dbs[i]->close();
    delete callgraphs[i];
  }

  toplevelDb.close();
  delete dbs;
  delete callgraphs;
  if (separateCompilation)
    delete callgraph;
#if 1
  delete simpleCallgraph;
#endif
  return( 0 );
}
