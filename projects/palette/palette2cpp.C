#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <cassert>
#include <utility>
#include "prologParser.h"
#include "prologAst.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

// We need the paths and functions from here, but not the redefinition of
// operator<< on vectors
#include "rose_paths.h"
class SgProject;
#include "string_functions.h"
#include "commandline_processing.h"

using namespace std;

typedef pair<string, unsigned int> TableMapKey;

string getPrologString(PrologAst::Node* n);
string getPrologAtomString(PrologAst::Node* n);

vector<PrologAst::Node*> splitCommasToList(PrologAst::Node* n) {
  PrologAst::Term* t = NULL;
  vector<PrologAst::Node*> result;
  while (true) {
    t = dynamic_cast<PrologAst::Term*>(n);
    if (!t || t->getFunctor() != "," || t->getArity() != 2) {
      result.push_back(n);
      return result;
    }
    result.push_back(t->getArgs()[0]);
    n = t->getArgs()[1];
  }
}

string getCppType(PrologAst::Node* n) {
  if (dynamic_cast<PrologAst::String*>(n)) {
    return dynamic_cast<PrologAst::String*>(n)->getValue();
  }
  PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(n);
  if (!t || t->getArity() != 0) {
    cerr << "Bad tuple element type " << n << endl;
    exit(1);
  }

  string s = t->getFunctor();
  if (s == "node") {
    return "SgNode*";
  } else if (s == "cfgNode") {
    return "VirtualCFG::CFGNode";
  } else if (s == "string") {
    return "std::string";
  } else if (s == "int") {
    return "int";
  } else if (s == "double") {
    return "double";
  } else {
    cerr << "Error: unknown field type " << s << endl;
    exit(1);
  }
}

vector<PrologAst::Node*> prologListToVector(PrologAst::Node* n) {
  vector<PrologAst::Node*> result;
  while (true) {
    PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(n);
    bool isEmptyList = t && t->getFunctor() == "[]" && t->getArity() == 0;
    bool isPair = t && t->getFunctor() == "." && t->getArity() == 2;
    if (!t || (!isEmptyList && !isPair)) {
      cerr << "Wanted a list, got " << n << endl;
      exit(1);
    }
    if (isEmptyList) {
      return result;
    } else {
      result.push_back(t->getArgs()[0]);
      n = t->getArgs()[1];
    }
  }
}

enum Mode {UNKNOWN_MODE, GROUND, UNBOUND, WILDCARD};
ostream& operator<<(ostream& os, Mode m) {
  switch (m) {
    case UNKNOWN_MODE: os << '?'; break;
    case GROUND: os << '+'; break;
    case UNBOUND: os << '-'; break;
    case WILDCARD: os << '_'; break;
    default: assert (!"Bad mode");
  }
  return os;
}

struct TableReferenceArg {
  PrologAst::Node* arg;
  Mode mode;
  string type;

  TableReferenceArg(PrologAst::Node* arg = 0,
                    Mode mode = UNKNOWN_MODE, string type = "@unknown")
    : arg(arg), mode(mode), type(type) {}
};

ostream& operator<<(ostream& os, const TableReferenceArg& arg) {
  os << arg.mode << "(" << arg.type << "):" << arg.arg;
  return os;
}

enum TableReferenceSortInformation {
  UNSORTED_TABLE_REFERENCE,
  ASCENDING_TABLE_REFERENCE,
  DESCENDING_TABLE_REFERENCE
};

ostream& operator<<(ostream& os, TableReferenceSortInformation si) {
  switch (si) {
    case UNSORTED_TABLE_REFERENCE: os << "unsorted"; break;
    case ASCENDING_TABLE_REFERENCE: os << "ascending"; break;
    case DESCENDING_TABLE_REFERENCE: os << "descending"; break;
    default: os << "???unknown_sort???"; break;
  }
  return os;
}

struct TableReference {
  string tableName;
  vector<TableReferenceArg> args;
  bool negative;
  TableReferenceSortInformation sortInfo;

  TableReference(const string& tableName,
                 const vector<TableReferenceArg>& args,
                 bool negative,
                 TableReferenceSortInformation sortInfo)
    : tableName(tableName), args(args), negative(negative), sortInfo(sortInfo) {
    computeModes();
  }

  void computeModes() {
    for (unsigned int i = 0; i < args.size(); ++i) { // Fill in the known modes
      if (args[i].mode == UNKNOWN_MODE) {
        if (dynamic_cast<PrologAst::Wildcard*>(args[i].arg)) {
          args[i].mode = WILDCARD;
        } else if (dynamic_cast<PrologAst::Integer*>(args[i].arg)) {
          args[i].mode = GROUND;
          args[i].type = "int";
        } else if (dynamic_cast<PrologAst::FloatingPoint*>(args[i].arg)) {
          args[i].mode = GROUND;
          args[i].type = "double";
        } else if (dynamic_cast<PrologAst::String*>(args[i].arg)) {
          args[i].mode = GROUND;
          args[i].type = "std::string"; // This really may be a C++ expression
        } else if (dynamic_cast<PrologAst::Term*>(args[i].arg)) {
          args[i].mode = GROUND;
          args[i].type = "@unknown";
        } else if (dynamic_cast<PrologAst::Variable*>(args[i].arg)) {
          args[i].mode = UNKNOWN_MODE;
          args[i].type = "@unknown";
        } else if (!args[i].arg) {
          args[i].mode = GROUND;
          args[i].type = "@null";
        } else {
          cerr << "Cannot handle constant " << args[i].arg << endl;
          exit(1);
        }
      }
    }
  }

  TableMapKey getKey() const {
    return make_pair(tableName, args.size());
  }

  explicit TableReference(const string& tableName) // Mostly for null outputs
    : tableName(tableName), negative(false),
      sortInfo(UNSORTED_TABLE_REFERENCE) {}

  explicit TableReference(PrologAst::Node* ref, bool checkForValidity = true);

  private: TableReference() {}
};

ostream& operator<<(ostream& os, const TableReference& tr) {
  os << (tr.negative ? "\\+ " : "") << tr.sortInfo << " ";
  os << tr.tableName << "(";
  for (unsigned int i = 0; i < tr.args.size(); ++i) {
    if (i != 0) os << ", ";
    os << tr.args[i];
  }
  os << ")";
  return os;
}

struct PrimitiveRule {
  vector<Mode> modes; // UNBOUND allows any mode, others must be matched
                      // exactly
  vector<string> types;
  vector<TableReference> actions;

  PrimitiveRule(const vector<Mode>& modes,
                const vector<string>& types,
                const vector<TableReference>& actions):
    modes(modes), types(types), actions(actions) {}
};

PrimitiveRule scanPrimitiveRule(unsigned int expectedArity,
                                PrologAst::Node* n) {
  PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(n);
  if (!t || t->getFunctor() != "rule" || t->getArity() != 2) {
    cerr << "Wanted primitive rule definition, got " << n << endl;
    exit(1);
  }

  PrologAst::Node* modeObj = t->getArgs()[0];
  vector<PrologAst::Node*> actionNodes = prologListToVector(t->getArgs()[1]);

  vector<PrologAst::Node*> modeEntryList = prologListToVector(modeObj);
  if (modeEntryList.size() != expectedArity) {
    cerr << "Mode list has incorrect arity: is " << modeEntryList.size() << ", should be " << expectedArity << endl;
    exit(1);
  }
  vector<Mode> modes(expectedArity);
  vector<string> types(expectedArity);
  for (unsigned int i = 0; i < modeEntryList.size(); ++i) {
    PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(modeEntryList[i]);
    if (!t || t->getArity() != 1 || t->getFunctor().size() != 1) {
      cerr << "Expected modeChar('type'), got " << modeEntryList[i] << endl;
      exit(1);
    }
    Mode m = UNKNOWN_MODE;
    switch (t->getFunctor()[0]) {
      case '+': m = GROUND; break;
      case '-': m = UNBOUND; break;
      case '_': m = WILDCARD; break;
      default: {cerr << "Bad mode " << t->getFunctor() << " in mode list" << endl; exit(1);}
    }
    modes[i] = m;
    PrologAst::Term* typeT = dynamic_cast<PrologAst::Term*>(t->getArgs()[0]);
    if (!typeT || typeT->getArity() != 0) {
      cerr << "Expected field type, got " << t->getArgs()[0] << endl;
      exit(1);
    }
    types[i] = typeT->getFunctor();
  }

  vector<TableReference> actions;
  for (unsigned int i = 0; i < actionNodes.size(); ++i) {
    actions.push_back(TableReference(actionNodes[i], false)); // May use tables which are really built-in and so don't exist in the list of tables
  }

  return PrimitiveRule(modes, types, actions);
}

vector<PrimitiveRule> scanPrimitiveRules(unsigned int expectedArity,
                                         PrologAst::Term* t) {
  vector<PrimitiveRule> results;
  vector<PrologAst::Node*> ls = prologListToVector(t);
  for (unsigned int i = 0; i < ls.size(); ++i) {
    results.push_back(scanPrimitiveRule(expectedArity, ls[i]));
  }
  return results;
}

struct TableInfo {
  vector<string> fieldTypes; // Ignored for primitive tables
  set<vector<Mode> > implementationsRequired;
  unsigned int computationIndex;
  bool isPrimitive;
  vector<PrimitiveRule> primitiveRules;

  TableInfo(const vector<string>& fieldTypes):
    fieldTypes(fieldTypes), implementationsRequired(),
    computationIndex((unsigned int)(-1)),
    isPrimitive(false), primitiveRules() {}
};

template <typename T>
ostream& operator<<(ostream& os, const std::vector<T>& v) {
  os << "[";
  for (unsigned int i = 0; i < v.size(); ++i) {
    if (i != 0) os << ", ";
    os << v[i];
  }
  os << "]";
  return os;
}

struct Rule {
  TableReference lhs;
  vector<TableReference> rhs;

  Rule(const TableReference& lhs, const vector<TableReference>& rhs)
    : lhs(lhs), rhs(rhs) {}

  TableMapKey targetTable() const {
    return lhs.getKey();
  }

  vector<TableReference> rhsMinusOneGoal(int index) const {
    vector<TableReference> result = rhs;
    result.erase(result.begin() + index);
    return result;
  }
};

ostream& operator<<(ostream& os, const Rule& r) {
  os << r.lhs << " :- " << r.rhs << ".";
  return os;
}

// Information about the program
vector<string> includePaths;
typedef map<TableMapKey, TableInfo> TableMapType;
TableMapType tables; // (name, arity) -> def
vector<Rule> rules;
vector<vector<TableReference> > queries;
string templateName;

TableReference::TableReference(PrologAst::Node* ref, bool checkForValidity)
                                : negative(false),
                                  sortInfo(UNSORTED_TABLE_REFERENCE) {
top:
  PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(ref);
  if (!t) {
    cerr << "Table reference is not a term: " << ref << endl;
    exit(1);
  }
  if (t->getFunctor() == "\\+" && t->getArity() == 1) {
    negative = true;
    ref = t->getArgs()[0];
    goto top;
  }
  if (t->getFunctor() == "ascending" && t->getArity() == 1) {
    sortInfo = ASCENDING_TABLE_REFERENCE;
    ref = t->getArgs()[0];
    goto top;
  }
  if (t->getFunctor() == "descending" && t->getArity() == 1) {
    sortInfo = DESCENDING_TABLE_REFERENCE;
    ref = t->getArgs()[0];
    goto top;
  }
  tableName = t->getFunctor();
  args.clear();
  for (unsigned int i = 0; i < t->getArity(); ++i) {
    args.push_back(TableReferenceArg(t->getArgs()[i]));
  }
  computeModes();
  if (checkForValidity) {
    TableMapType::const_iterator i = tables.find(make_pair(tableName, args.size()));
    if (i == tables.end() && (tableName != "=" || args.size() != 2)) {
      cerr << "Unknown table " << tableName << "/" << args.size() << endl;
      exit(1);
    }
  }
}

void processIs(PrologAst::Node* lhs, PrologAst::Node* rhs, 
               vector<TableReference>& subgoalRefs) {
  static int isVarCounter = 0;
  PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(rhs);
  if (t) {
    vector<TableReferenceArg> newArgs;
    for (unsigned int i = 0; i < t->getArity(); ++i) {
      PrologAst::Node* thisArg = 0;
      if (dynamic_cast<PrologAst::Term*>(t->getArgs()[i])) {
        ostringstream varStream;
        varStream << "PaletteIsTemp" << ++isVarCounter;
        thisArg = new PrologAst::Variable(varStream.str());
        processIs(thisArg, t->getArgs()[i], subgoalRefs);
      } else {
        thisArg = t->getArgs()[i];
      }
      newArgs.push_back(TableReferenceArg(thisArg));
    }
    newArgs.push_back(TableReferenceArg(lhs));
    subgoalRefs.push_back(TableReference(t->getFunctor(), newArgs, false, UNSORTED_TABLE_REFERENCE));
  } else {
    vector<TableReferenceArg> args;
    args.push_back(TableReferenceArg(lhs));
    args.push_back(TableReferenceArg(rhs));
    subgoalRefs.push_back(TableReference("=", args, false, UNSORTED_TABLE_REFERENCE));
  }
}

void addIncludePath(const string& p) {
  if (std::find(includePaths.begin(), includePaths.end(), p) == includePaths.end()) {
    includePaths.push_back(p);
  }
}

vector<string> parseParameters(const string& argv0, const vector<char*>& args) {
  vector<string> sourceFiles;
  for (unsigned int i = 0; i < args.size(); ++i) {
    string arg = args[i];
    if (arg.size() >= 2 && arg[0] == '-' && arg[1] == 'I') {
      if (arg.size() == 2) {
	if (i + 1 != args.size()) {
	  addIncludePath(args[i + 1]);
	  ++i;
	} else {
	  cerr << "Option -I needs argument" << endl;
	  exit(1);
	}
      } else {
	addIncludePath(arg.substr(2));
      }
    } else if (arg == "--") {
      sourceFiles.insert(sourceFiles.end(), args.begin() + i + 1, args.end());
      break;
    } else {
      sourceFiles.push_back(arg);
    }
  }
  return sourceFiles;
}

string findIncludeFileMaybe(const string& baseName) {
  for (unsigned int i = 0; i < includePaths.size(); ++i) {
    string fileName = includePaths[i] + "/" + baseName;
    ifstream f(fileName.c_str());
    if (f) return fileName;
  }
  return "";
}

string findIncludeFile(const string& baseName) {
  string name = findIncludeFileMaybe(baseName);
  if (!name.empty()) return name;
  cerr << "Include file \"" << baseName << "\" not found in path " << includePaths << endl;
  exit(1);
}

// Handle parsing and getting information from one file, including handling
// operator definitions and include directives
void processOneFile(const std::string& fileName) {
  std::string contents = PrologLexer::readFile(fileName);
  unsigned int idx = 0;
  int line = 1;
  int col = 1;
  while (true) {
    std::vector<PrologLexer::PrologToken> sentenceTokens =
      PrologLexer::lexSentence(contents, idx, fileName, line, col);
    // This vector now contains either a list of tokens ending in period or the
    // single element EOF (assert in lexSentence prevents EOF at end of
    // non-singleton vectors)
    assert (!sentenceTokens.empty());
    if (sentenceTokens.size() == 1 &&
	sentenceTokens.back().kind == PrologLexer::END_OF_FILE) {
      return;
    }
    PrologAst::Node* sentence = PrologParser::parseSentence(sentenceTokens);
    assert (sentence);
    PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(sentence);
    if (t) {
      if (t->getFunctor() == ":-" && t->getArity() == 1) { // Compiler directive
        PrologAst::Term* child = 
          dynamic_cast<PrologAst::Term*>(t->getArgs()[0]);
        if (child && child->getFunctor() == "declare" &&
            child->getArity() == 1) {
          PrologAst::Term* tableDecl =
            dynamic_cast<PrologAst::Term*>(child->getArgs()[0]);
          assert (tableDecl);
          string tableName = tableDecl->getFunctor();
          vector<string> paramTypes(tableDecl->getArgs().size());
          for (unsigned int i = 0; i < tableDecl->getArgs().size(); ++i) {
            paramTypes[i] = getCppType(tableDecl->getArgs()[i]);
          }
          bool inserted = 
            tables.insert(make_pair(make_pair(tableName, paramTypes.size()), TableInfo(paramTypes))).second;
          if (!inserted) {
            cerr << "Duplicate table " << tableName << "/" << paramTypes.size() << endl;
            exit(1);
          }
        } else if (child && child->getFunctor() == "include" &&
                   child->getArity() == 1) {
          PrologAst::String* fileName = 
            dynamic_cast<PrologAst::String*>(child->getArgs()[0]);
          if (!fileName) {
            cerr << "File name in :- include is not a string" << endl;
            exit(1);
          }
	  processOneFile(findIncludeFile(fileName->getValue()));
          continue;
        } else if (child && child->getFunctor() == "template" &&
                   child->getArity() == 1) {
          PrologAst::String* tmpl = 
            dynamic_cast<PrologAst::String*>(child->getArgs()[0]);
          if (!tmpl) {
            cerr << "Template name in :- template is not a string" << endl;
            exit(1);
          }
          templateName = tmpl->getValue();
        } else if (child && child->getFunctor() == "prim" &&
                   child->getArity() == 3) {
          PrologAst::Term* tableNameTerm = dynamic_cast<PrologAst::Term*>(child->getArgs()[0]);
          PrologAst::Integer* tableArityTerm = dynamic_cast<PrologAst::Integer*>(child->getArgs()[1]);
          assert (tableNameTerm && tableNameTerm->getArity() == 0);
          assert (tableArityTerm);
          PrologAst::Term* rules =
            dynamic_cast<PrologAst::Term*>(child->getArgs()[2]);
          assert (rules);
          string tableName = tableNameTerm->getFunctor();
          vector<string> paramTypes(tableArityTerm->getValue(), "");
          pair<TableMapType::iterator, bool> insertPair = 
            tables.insert(make_pair(make_pair(tableName, paramTypes.size()), TableInfo(paramTypes)));
          if (!insertPair.second) {
            cerr << "Duplicate table " << tableName << "/" << paramTypes.size() << endl;
            exit(1);
          }
          TableMapType::iterator it = insertPair.first;
          it->second.isPrimitive = true;
          it->second.primitiveRules =
            scanPrimitiveRules(paramTypes.size(), rules);
	} else if (child && child->getFunctor() == "op" &&
                   child->getArity() == 3) {
          PrologAst::Integer* precNode = dynamic_cast<PrologAst::Integer*>(child->getArgs()[0]);
	  if (!precNode) {
	    cerr << "Precedence in operator definition " << child << " is not an integer" << endl;
	    exit(1);
	  }
	  int prec = precNode->getValue();
	  PrologAst::Term* typeNode = dynamic_cast<PrologAst::Term*>(child->getArgs()[1]);
	  if (!typeNode || typeNode->getArity() != 0) {
	    cerr << "Type in operator definition " << child << " is not an atom" << endl;
	    exit(1);
	  }
	  string type = typeNode->getFunctor();
	  PrologParser::Associativity assoc = PrologParser::NOT_AN_OPERATOR;
	  PrologParser::OperatorTag tag = PrologParser::NON_OPERATOR;
	  if (type == "xfx") {
	    assoc = PrologParser::XFX; tag = PrologParser::INFIX;
	  } else if (type == "xfy") {
	    assoc = PrologParser::XFY; tag = PrologParser::INFIX;
	  } else if (type == "yfx") {
	    assoc = PrologParser::YFX; tag = PrologParser::INFIX;
	  } else if (type == "yfy") {
	    assoc = PrologParser::YFY; tag = PrologParser::INFIX;
	  } else if (type == "fx") {
	    assoc = PrologParser::FX; tag = PrologParser::PREFIX;
	  } else if (type == "fy") {
	    assoc = PrologParser::FY; tag = PrologParser::PREFIX;
	  } else if (type == "xf") {
	    assoc = PrologParser::XF; tag = PrologParser::POSTFIX;
	  } else if (type == "yf") {
	    assoc = PrologParser::YF; tag = PrologParser::POSTFIX;
	  } else {
	    cerr << "Type " << type << " in operator definition is invalid" << endl;
	    exit(1);
	  }
	  vector<string> operators;
	  PrologAst::Term* opTerm = dynamic_cast<PrologAst::Term*>(child->getArgs()[2]);
	  bool isList = false;
	  while (true) {
	    bool continueLoop = false;
	    PrologAst::Term* thisOp = NULL;
	    if (!opTerm) {
	      cerr << "Invalid operator or list of operators " << child->getArgs()[2] << endl;
	      exit(1);
	    } else if (opTerm->getFunctor() == "[]" && opTerm->getArity() == 0) {
	      break;
	    } else if (opTerm->getFunctor() == "." && opTerm->getArity() == 2) {
	      isList = true;
	      continueLoop = true;
	      thisOp = dynamic_cast<PrologAst::Term*>(opTerm->getArgs()[0]);
	      opTerm = dynamic_cast<PrologAst::Term*>(opTerm->getArgs()[1]);
	    } else if (opTerm->getArity() == 0 && !isList) {
	      continueLoop = false;
	      thisOp = dynamic_cast<PrologAst::Term*>(opTerm);
	    }
	    if (!thisOp || thisOp->getArity() != 0) {
	      cerr << "Invalid operator of list of operators " << child->getArgs()[2] << endl;
	      exit(1);
	    }
	    operators.push_back(thisOp->getFunctor());
	    if (!continueLoop) break;
	  }
	  map<string, pair<PrologParser::Associativity, int> >* operatorMapPtr = NULL;
	  switch (tag) {
	    case PrologParser::INFIX: operatorMapPtr = &PrologParser::infixOperators; break;
	    case PrologParser::PREFIX: operatorMapPtr = &PrologParser::prefixOperators; break;
	    case PrologParser::POSTFIX: operatorMapPtr = &PrologParser::postfixOperators; break;
	    default: cerr << "Internal error: invalid operator tag " << tag << endl; abort();
	  }
	  map<string, pair<PrologParser::Associativity, int> >& operatorMap = *operatorMapPtr;
	  for (unsigned int i = 0; i < operators.size(); ++i) {
	    string op = operators[i];
	    map<string, pair<PrologParser::Associativity, int> >::iterator it = operatorMap.find(op);
	    if (prec == 0) { // Delete operator
	      // cerr << "Removing op " << op << " as " << tag << endl;
	      if (it == operatorMap.end()) {
		cerr << "Operator being removed does not exist: " << child << endl;
		exit(1);
	      } else {
		operatorMap.erase(it);
	      }
	    } else { // Add operator
	      // cerr << "Adding op " << op << " as " << tag << endl;
	      if (it != operatorMap.end()) {
		cerr << "Operator being added already exists: " << child << endl;
		exit(1);
	      } else {
		operatorMap.insert(make_pair(op, make_pair(assoc, prec)));
	      }
	    }
	  }
        } else {
	  if (child) {
	    cerr << "Bad :- line: child is " << child->getFunctor() << "/" << child->getArity() << endl;
	  } else {
	    cerr << "Bad :- line: child is not a term: " << t << endl;
	  }
          exit(1);
        }
      } else if (t->getFunctor() == ":-" && t->getArity() == 2) { // Rule
        vector<PrologAst::Node*> subgoals = splitCommasToList(t->getArgs()[1]);
        vector<TableReference> subgoalRefs;
        for (unsigned int i = 0; i < subgoals.size(); ++i) {
          PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(subgoals[i]);
          if (t && t->getFunctor() == "is" && t->getArity() == 2) {
            processIs(t->getArgs()[0], t->getArgs()[1], subgoalRefs);
          } else {
            subgoalRefs.push_back(TableReference(subgoals[i]));
          }
        }
        rules.push_back(Rule(TableReference(t->getArgs()[0]),
                             subgoalRefs));
      } else if (t->getFunctor() == "?-" && t->getArity() == 1) { // Query
        vector<PrologAst::Node*> refs = splitCommasToList(t->getArgs()[0]);
        vector<TableReference> tableRefs;
        for (unsigned int i = 0; i < refs.size(); ++i) {
          PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(refs[i]);
          if (t && t->getFunctor() == "is" && t->getArity() == 2) {
            processIs(t->getArgs()[0], t->getArgs()[1], tableRefs);
          } else {
            tableRefs.push_back(TableReference(refs[i]));
          }
        }
        queries.push_back(tableRefs);
      } else { // Fact
        rules.push_back(Rule(TableReference(t), vector<TableReference>()));
      }
    } else {
      cerr << "The term " << sentence << " does not belong as its own element of a program" << endl;
      exit(1);
    }
  }
}

struct Tarjan {
  int n, sccCounter;
  set<TableMapKey> done;
  vector<TableMapKey> st;
  map<TableMapKey, int> dfsNumber;
  map<TableMapKey, int> lowlink;
  vector<vector<TableMapKey> > components;

  Tarjan(): n(0), sccCounter(0) {}

  void tarjan(const TableMapKey& t) {
    // Tarjan SCC algorithm from Wikipedia
    done.insert(t);
    dfsNumber[t] = n;
    lowlink[t] = n;
    ++n;
    st.push_back(t);
    TableMapType::iterator tInfoIter = tables.find(t);
    if (tInfoIter == tables.end()) {
      cerr << "Unknown table " << t.first << "/" << t.second << endl;
      exit(1);
    }
    for (vector<Rule>::const_iterator j = rules.begin();
         j != rules.end(); ++j) {
      if (j->targetTable() != t) continue;
      for (vector<TableReference>::const_iterator k = j->rhs.begin();
           k != j->rhs.end(); ++k) {
        if (k->tableName != "=" || k->args.size() != 2) {
          if (tables.find(k->getKey()) == tables.end()) {
            cerr << "Unknown table " << k->tableName << "/" << k->args.size() << endl;
            exit(1);
          }
          if (done.find(k->getKey()) == done.end()) {
            tarjan(k->getKey());
            lowlink[t] = (std::min)(lowlink[t], lowlink[k->getKey()]);
          } else if (std::find(st.begin(), st.end(), k->getKey()) !=
                     st.end()) {
            lowlink[t] = (std::min)(lowlink[t], dfsNumber[k->getKey()]);
          }
        }
      }
    }
    if (lowlink[t] == dfsNumber[t]) {
      vector<TableMapKey> sccMembers(1, t);
      tInfoIter->second.computationIndex = sccCounter;
      while (!st.empty() && st.back() != t) {
        sccMembers.push_back(st.back());
        TableMapType::iterator backInfoIter = tables.find(st.back());
        assert (backInfoIter != tables.end());
        backInfoIter->second.computationIndex = sccCounter;
        st.pop_back();
      }
      assert (!st.empty());
      assert (st.back() == t);
      st.pop_back();
      ++sccCounter;
      components.push_back(sccMembers);
      assert (components.size() == (unsigned int)sccCounter);
    }
  }
};

vector<vector<TableMapKey> > orderTableComputations() {
  // Topological sort of strongly connected components of tables
  // The output of Tarjan's algorithm is topologically sorted according to
  // ftp://reports.stanford.edu/pub/cstr/reports/cs/tr/75/508/CS-TR-75-508.pdf
  Tarjan tarj;
  for (TableMapType::const_iterator i = tables.begin();
       i != tables.end(); ++i) {
    TableMapKey t = i->first;
    if (tarj.done.find(t) != tarj.done.end()) continue;
    tarj.tarjan(t);
  }
  return tarj.components;
}

struct TriggerCondition {
  virtual void unparse(ostream& os) const = 0;
  virtual ~TriggerCondition() {};
};

ostream& operator<<(ostream& os, const TriggerCondition* tc) {
  tc->unparse(os);
  return os;
}

struct NullTriggerCondition: public TriggerCondition {
  NullTriggerCondition() {}

  virtual void unparse(ostream& os) const {
    os << "null";
  }
};

struct AndTriggerCondition: public TriggerCondition {
  TriggerCondition* body1;
  TriggerCondition* body2;

  AndTriggerCondition(TriggerCondition* body1, TriggerCondition* body2)
    : body1(body1), body2(body2) {}

  virtual void unparse(ostream& os) const {
    os << "and(" << body1 << ", " << body2 << ")";
  }
};

struct NotTriggerCondition: public TriggerCondition {
  TriggerCondition* body;

  NotTriggerCondition(TriggerCondition* body): body(body) {}

  virtual void unparse(ostream& os) const {
    os << "not(" << body << ")";
  }
};

struct TableRefTriggerCondition: public TriggerCondition {
  TableReference tableRef;

  TableRefTriggerCondition(const TableReference& tableRef): tableRef(tableRef) {
    assert (!tableRef.negative);
  }
  virtual void unparse(ostream& os) const {
    os << "tableRef(" << tableRef << ")";
  }
};

TriggerCondition* tableRefToTriggerCondition(const TableReference& tr) {
  if (tr.negative) {
    TableReference tr2 = tr;
    tr2.negative = false;
    return new NotTriggerCondition(new TableRefTriggerCondition(tr2));
  } else {
    // Sorted tables are allowed here
    return new TableRefTriggerCondition(tr);
  }
}

// First is join elements after the trigger has started, 
// second is the insertion to do
typedef pair<TriggerCondition*, TableReference> Trigger;

// Determine whether a mode vector matches a pattern
bool modeVectorMatches(const vector<Mode>& pat,
                       const vector<string>& patTypes,
                       const vector<TableReferenceArg>& x) {
  if (pat.size() != x.size() || patTypes.size() != x.size()) {
    cerr << "Mode vector size mismatch" << endl;
    exit(1);
  }
  for (unsigned int i = 0; i < pat.size(); ++i) {
    if (pat[i] != UNBOUND) {
      if (pat[i] != x[i].mode) return false;
      if (x[i].mode == GROUND && patTypes[i] != x[i].type) return false;
    }
  }
  return true;
}

int variableInstantiationCounter;

// Plug a variable suffix into an AST node, including replacing $ in strings
pair<PrologAst::Node*, Mode>
plugInVariables(PrologAst::Node* n,
                const string& suffix,
                const TableReference& ref) {
  PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(n);
  if (t) {
    vector<PrologAst::Node*> newArgs;
    for (unsigned int i = 0; i < t->getArity(); ++i) {
      newArgs.push_back(plugInVariables(t->getArgs()[i], suffix, ref).first);
    }
    return make_pair(new PrologAst::Term(t->getFunctor(), newArgs),
                     UNKNOWN_MODE);
  }
  PrologAst::Variable* v = dynamic_cast<PrologAst::Variable*>(n);
  if (v && !v->getName().empty() && v->getName()[0] == '$') {
    istringstream nStream(v->getName().substr(1));
    unsigned int n;
    nStream >> n;
    assert (nStream.eof());
    if (n < ref.args.size()) {
      return make_pair(ref.args[n].arg, ref.args[n].mode);
    } else {
      return make_pair(
               new PrologAst::Variable("var" +
                                       v->getName().substr(1) +
                                       suffix),
               UNKNOWN_MODE);
    }
  }
  PrologAst::String* s = dynamic_cast<PrologAst::String*>(n);
  if (s) {
    string newStr;
    for (unsigned int i = 0; i < s->getValue().size(); ++i) {
      if (s->getValue()[i] == '$') {
        unsigned int n = 0;
        string nString;
        while (true) {
          ++i;
          if (i < s->getValue().size() && isdigit(s->getValue()[i])) {
            n = n * 10 + s->getValue()[i] - '0';
            nString += s->getValue()[i];
          } else {
            --i; // Will be incremented by outer loop
            break;
          }
        }
        if (n < ref.args.size()) {
          PrologAst::Variable* var =
            dynamic_cast<PrologAst::Variable*>(ref.args[n].arg);
          if (var) {
            newStr += var->getName();
          } else if (dynamic_cast<PrologAst::String*>(ref.args[n].arg)) {
            ostringstream os;
            os << ref.args[n].arg;
            newStr += os.str();
          } else { // FIXME
            newStr += getPrologString(ref.args[n].arg);
          }
        } else {
          newStr += "var" + nString + suffix;
        }
      } else {
        newStr += s->getValue()[i];
      }
    }
    return make_pair(new PrologAst::String(newStr), UNKNOWN_MODE);
  }
  return make_pair(n, UNKNOWN_MODE);
}

// Create a new variable suffix
string generateVariableSuffix() {
  ostringstream suffixStream;
  suffixStream << "_rl" << ++variableInstantiationCounter;
  return suffixStream.str();
}

typedef map<string /* var name */, pair<Mode, string /* var type */> >
  variableModeMap;

vector<Mode> getModesFromTableRef(const TableReference& ref) {
  vector<Mode> modes(ref.args.size());
  for (unsigned int i = 0; i < ref.args.size(); ++i) {
    modes[i] = ref.args[i].mode;
  }
  return modes;
}

// Assign modes and fix up built-in table refs for a single subgoal
void assignModesOne(TriggerCondition* tc,
                    TriggerCondition*& newTriggerConditions,
                    variableModeMap& variableModes) {
  if (dynamic_cast<NullTriggerCondition*>(tc)) {
    return;
  } else if (dynamic_cast<AndTriggerCondition*>(tc)) {
    assignModesOne(dynamic_cast<AndTriggerCondition*>(tc)->body1, newTriggerConditions, variableModes);
    assignModesOne(dynamic_cast<AndTriggerCondition*>(tc)->body2, newTriggerConditions, variableModes);
    return;
  } else if (dynamic_cast<NotTriggerCondition*>(tc)) {
    TriggerCondition* ntc = new NullTriggerCondition;
    assignModesOne(dynamic_cast<NotTriggerCondition*>(tc)->body, ntc, variableModes);
    newTriggerConditions = new AndTriggerCondition(newTriggerConditions, new NotTriggerCondition(ntc));
    return;
  }
  assert (dynamic_cast<TableRefTriggerCondition*>(tc));
  TableReference& ref = dynamic_cast<TableRefTriggerCondition*>(tc)->tableRef;
#if 0
  cerr << "assignModesOne " << ref << endl;
  for (variableModeMap::const_iterator i = variableModes.begin();
       i != variableModes.end(); ++i) {
    if (i != variableModes.begin()) cerr << ", ";
    cerr << i->first << " -> " << i->second.first << i->second.second;
  }
  cerr << endl;
#endif
  for (unsigned int i = 0; i < ref.args.size(); ++i) {
    PrologAst::Variable* var =
      dynamic_cast<PrologAst::Variable*>(ref.args[i].arg);
    if (var) {
      variableModeMap::iterator it = variableModes.find(var->getName());
      if (it == variableModes.end()) {
        it = variableModes.insert(make_pair(var->getName(),
                                            make_pair(UNBOUND, "@unknown")))
                          .first;
      }
      assert (it != variableModes.end());
      ref.args[i].mode = it->second.first;
      ref.args[i].type = it->second.second;
    }
  }
#if 0
  cerr << "assignModesOne2 " << ref << endl;
#endif
  TableMapType::iterator tableInfoIter =
    tables.find(ref.getKey());
  if (ref.tableName == "=") { // Special case because = is polymorphic
    assert (ref.args.size() == 2);
    if (ref.args[0].mode == WILDCARD || ref.args[1].mode == WILDCARD) {
    } else if (ref.args[0].mode == GROUND && ref.args[1].mode == GROUND) {
      if (ref.args[0].type != ref.args[1].type) {
        cerr << "Bad types for = operator in " << ref << endl;
        exit(1);
      }
      ostringstream conditionStream;
      conditionStream << "(" << ref.args[0].arg << ") == (" << ref.args[1].arg << ")";
      newTriggerConditions = new AndTriggerCondition(newTriggerConditions, tableRefToTriggerCondition(TableReference("<cond>", vector<TableReferenceArg>(1, TableReferenceArg(new PrologAst::String(conditionStream.str()), GROUND, "string")), ref.negative, UNSORTED_TABLE_REFERENCE)));
    } else if (ref.args[0].mode == GROUND && ref.args[1].mode == UNBOUND) {
      PrologAst::Variable* var = 
        dynamic_cast<PrologAst::Variable*>(ref.args[1].arg);
      assert (var);
      vector<TableReferenceArg> args(2);
      string type = ref.args[0].type;
      variableModes[var->getName()] = make_pair(GROUND, type);
      ref.args[1].type = type;
      args[0] = ref.args[1];
      args[1] = ref.args[0];
      newTriggerConditions = new AndTriggerCondition(newTriggerConditions, tableRefToTriggerCondition(TableReference("<let>", args, false, UNSORTED_TABLE_REFERENCE)));
    } else if (ref.args[0].mode == UNBOUND && ref.args[1].mode == GROUND) {
      PrologAst::Variable* var = 
        dynamic_cast<PrologAst::Variable*>(ref.args[0].arg);
      assert (var);
      vector<TableReferenceArg> args(2);
      string type = ref.args[1].type;
      variableModes[var->getName()] = make_pair(GROUND, type);
      ref.args[0].type = type;
      args[0] = ref.args[0];
      args[1] = ref.args[1];
      newTriggerConditions = new AndTriggerCondition(newTriggerConditions, tableRefToTriggerCondition(TableReference("<let>", args, false, UNSORTED_TABLE_REFERENCE)));
    } else {
      cerr << "Bad modes for = operator: " << ref.args << endl;
      exit(1);
    }
  } else {
    assert (tableInfoIter != tables.end());
    TableInfo& tableInfo = tableInfoIter->second;
    if (tableInfo.isPrimitive) {
      assert (!ref.negative); // FIXME: handle negation
      assert (ref.sortInfo == UNSORTED_TABLE_REFERENCE); // FIXME: handle sorting
      const vector<PrimitiveRule>& primRules = tableInfo.primitiveRules;
      // Scan through the rules, taking the first one which matches the current
      // modes and sign; use this to assign variable types
      bool found = false;
      for (unsigned int j = 0; j < primRules.size(); ++j) {
        const PrimitiveRule& r = primRules[j];
        // cerr << "Trying " << ref << " against " << r.modes << " " << r.types << ": " << boolalpha << modeVectorMatches(r.modes, r.types, ref.args) << endl;
        if (modeVectorMatches(r.modes, r.types, ref.args)) {
          for (unsigned int i = 0; i < r.types.size(); ++i) {
            ref.args[i].type = r.types[i];
            PrologAst::Variable* var = dynamic_cast<PrologAst::Variable*>(ref.args[i].arg);
            if (var) {
              if (variableModes.find(var->getName()) == variableModes.end()) {
                variableModes[var->getName()].first = UNBOUND;
              }
              variableModes[var->getName()].second = r.types[i];
            }
          }
          string suffix = generateVariableSuffix();
          const vector<TableReference>& actions = r.actions;
          for (unsigned int k = 0; k < actions.size(); ++k) {
            const TableReference& act = actions[k];
#if 0
            cerr << "Working on action " << act << " from ref " << ref << endl;
            cerr << "Variable modes: ";
            for (variableModeMap::const_iterator it = variableModes.begin(); it != variableModes.end(); ++it) cerr << " " << it->first << "." << it->second.first;
            cerr << endl;
#endif
            TableReference newAct = act;
            for (unsigned int l = 0; l < act.args.size(); ++l) {
              pair<PrologAst::Node*, Mode> argAndMode =
                plugInVariables(act.args[l].arg, suffix, ref);
              newAct.args[l].arg = argAndMode.first;
              newAct.args[l].mode = act.args[l].mode;
              if (argAndMode.second != UNKNOWN_MODE) {
                newAct.args[l].mode = argAndMode.second;
              }
              newAct.computeModes();
            }
            // cerr << "act = " << act << ", newAct = " << newAct << endl;
            const string& actKind = newAct.tableName;
            if (actKind == "rescan" && newAct.args.size() == 0) {
              newAct = ref;
              // cerr << "Before rescan, newAct = " << newAct << endl;
              assignModesOne(tableRefToTriggerCondition(newAct), newTriggerConditions, variableModes);
            } else if (actKind == "let" && newAct.args.size() == 3) {
              string type;
              PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(newAct.args[0].arg);
              if (!t || t->getArity() != 0) {
                cerr << "First arg to let should be a type, not " << newAct.args[0].arg << endl;
                exit(1);
              }
              type = t->getFunctor();
              assert (newAct.args[0].mode == GROUND);
              assert (newAct.args[2].mode == GROUND);
              PrologAst::Variable* var =
                dynamic_cast<PrologAst::Variable*>(newAct.args[1].arg);
              switch (newAct.args[1].mode) {
                case GROUND: // Equality test
                  newAct.tableName = "<=>";
                  newAct.args.erase(newAct.args.begin());
                  newTriggerConditions = new AndTriggerCondition(newTriggerConditions, tableRefToTriggerCondition(newAct));
                  break;
                case UNBOUND: // Actual let
                  newAct.tableName = "<let>";
                  newAct.args.erase(newAct.args.begin()); // Don't need type as an argument anymore
                  newAct.args[0].type = type;
                  assert (var);
                  variableModes[var->getName()] = make_pair(GROUND, type);
                  newTriggerConditions = new AndTriggerCondition(newTriggerConditions, tableRefToTriggerCondition(newAct));
                  break;
                default: break; // Wildcard, so do nothing
              }
            } else if (actKind == "makeGround" && newAct.args.size() == 2) {
              assert (newAct.args[0].mode == UNBOUND); // Variable
              assert (newAct.args[1].mode == GROUND); // Type
              PrologAst::Variable* var = dynamic_cast<PrologAst::Variable*>(newAct.args[0].arg);
              assert (var);
              string type = getPrologAtomString(newAct.args[1].arg);
              variableModes[var->getName()] = make_pair(GROUND, type);
              newAct.args[0].type = type;
            } else {
              newAct.tableName = "<" + actKind + ">";
	      newTriggerConditions = new AndTriggerCondition(newTriggerConditions, tableRefToTriggerCondition(newAct));
            }
#if 0
            cerr << "After working on action " << newAct << " from ref " << ref << endl;
            cerr << "Variable modes: ";
            for (variableModeMap::const_iterator it = variableModes.begin(); it != variableModes.end(); ++it) cerr << " " << it->first << "." << it->second.first;
            cerr << endl;
#endif
          }
          found = true;
          break;
        }
      }
      if (!found) {
        cerr << "Invalid primitive use " << ref << " -- no built-in table template matches this lookup" << endl;
        exit(1);
      }
    } else {
      vector<Mode> modes = getModesFromTableRef(ref);
      tableInfo.implementationsRequired.insert(modes);
      for (unsigned int i = 0; i < ref.args.size(); ++i) {
        PrologAst::Variable* var = dynamic_cast<PrologAst::Variable*>(ref.args[i].arg);
        if (var) {
          variableModes[var->getName()] =
            make_pair(GROUND, tableInfo.fieldTypes[i]);
          ref.args[i].type = tableInfo.fieldTypes[i];
        }
      }
      newTriggerConditions = new AndTriggerCondition(newTriggerConditions, tableRefToTriggerCondition(ref));
    }
  }
#if 0
  cerr << "After assignModesOne, " << ref << endl;
  for (variableModeMap::const_iterator i = variableModes.begin();
       i != variableModes.end(); ++i) {
    if (i != variableModes.begin()) cerr << ", ";
    cerr << i->first << " -> " << i->second.first << i->second.second;
  }
  cerr << endl;
#endif
}

// Assign modes to table references, and add helper table references for
// built-in tables
void assignModes(const TableMapKey& triggeredByTable, Trigger& tr) {
  TriggerCondition* newTriggerConditions = new NullTriggerCondition;
  variableModeMap variableModes;
  vector<string> fieldTypes; // Set as empty for special "main" table
  TableMapType::const_iterator tableInfoIter =
    tables.find(triggeredByTable);
  if (triggeredByTable != make_pair((string)"main", 0u) &&
      tableInfoIter != tables.end()) {
    assert (tableInfoIter != tables.end());
    const TableInfo& info = tableInfoIter->second;
    fieldTypes = info.fieldTypes;
  }
  // Set up types for input parameters of trigger
  for (unsigned int i = 0; i < fieldTypes.size(); ++i) {
    ostringstream os;
    os << "Palette_Param" << i;
    variableModes[os.str()] = make_pair(GROUND, fieldTypes[i]);
  }
  assignModesOne(tr.first, newTriggerConditions, variableModes);
  if (tr.second.negative) {
    cerr << "Insertion result should not be negative: " << tr.second << endl;
    exit(1);
  }
  if (tr.second.sortInfo != UNSORTED_TABLE_REFERENCE) {
    cerr << "Insertion result should not be sorted: " << tr.second << endl;
    exit(1);
  }
  tables.insert(make_pair(TableMapKey("", 0), TableInfo(vector<string>())));
  TriggerCondition* newTriggerConditions2 = new NullTriggerCondition;
  assignModesOne(tableRefToTriggerCondition(tr.second), newTriggerConditions2, variableModes);
  assert (dynamic_cast<AndTriggerCondition*>(newTriggerConditions2) &&
	  dynamic_cast<NullTriggerCondition*>(dynamic_cast<AndTriggerCondition*>(newTriggerConditions2)->body1) &&
	  dynamic_cast<TableRefTriggerCondition*>(dynamic_cast<AndTriggerCondition*>(newTriggerConditions2)->body2));
  tr.first = newTriggerConditions;
  tr.second = dynamic_cast<TableRefTriggerCondition*>(dynamic_cast<AndTriggerCondition*>(newTriggerConditions2)->body2)->tableRef;
  for (unsigned int i = 0; i < tr.second.args.size(); ++i) {
    if (tr.second.args[i].mode != GROUND) {
      cerr << "Variable used in insertion is not ground: " << tr.second << endl;
      exit(1);
    }
  }
}

string mangle(const TableMapKey& str) {
  // FIXME
  if (str.first == "=" && str.second == 2) {
    return "paletteEqual";
  } else {
    ostringstream os;
    os << str.first << "__slash__" << str.second;
    return os.str();
  }
}

set<TableMapKey> insertFunctionsNeeded;

string insertFunctionName(const TableMapKey& tableName) {
  return "paletteInsert_" + mangle(tableName);
}

void requestInsertFunction(const TableMapKey& tableName) {
  insertFunctionsNeeded.insert(tableName);
}

map<vector<string>, string> tupleTypeNames;
int tupleNameCounter = 0;

string makeTupleType(const vector<string>& fieldTypes) {
  if (fieldTypes.size() == 1) {
    return fieldTypes[0];
  } else {
    map<vector<string>, string>::const_iterator i = 
      tupleTypeNames.find(fieldTypes);
    if (i != tupleTypeNames.end()) {
      return i->second;
    } else {
      ostringstream nameStream;
      nameStream << "PaletteTuple" << tupleNameCounter++;
      tupleTypeNames[fieldTypes] = nameStream.str();
      return nameStream.str();
    }
  }
}

string makeTuple(const vector<string>& fieldTypes,
                 const vector<string>& args) {
  assert (fieldTypes.size() == args.size());
  if (fieldTypes.size() == 1) {
    return args[0];
  } else {
    string ty = makeTupleType(fieldTypes);
    string result = ty + "(";
    for (unsigned int i = 0; i < args.size(); ++i) {
      if (i != 0) result += ", ";
      result += args[i];
    }
    result += ")";
    return result;
  }
}

string makeTupleElementRef(const vector<string>& fieldTypes,
                           string tupleObj, unsigned int idx) {
  if (fieldTypes.size() == 1) {
    return tupleObj;
  } else {
    ostringstream os;
    os << "(" << tupleObj << ").param" << idx;
    return os.str();
  }
}

string makeTupleDefinition(const vector<string>& fieldTypes) {
  ostringstream os;
  string name = makeTupleType(fieldTypes);
  os << "struct " << name << " {\n";
  for (unsigned int j = 0; j < fieldTypes.size(); ++j) {
    os << "  " << fieldTypes[j] << " param" << j << ";\n";
  }
  os << "  " << name << "(";
  for (unsigned int j = 0; j < fieldTypes.size(); ++j) {
    if (j != 0) os << ", ";
    os << fieldTypes[j] << " const& param" << j;
  }
  os << ")";
  if (!fieldTypes.empty()) {
    os << ": ";
    for (unsigned int j = 0; j < fieldTypes.size(); ++j) {
      if (j != 0) os << ", ";
      os << "param" << j << "(param" << j << ")";
    }
  }
  os << " {}" << endl;
  os << "  bool operator==(const " << name << "& o) const {" << endl;
  os << "    return ";
  if (fieldTypes.empty()) {
    os << "true";
  } else {
    for (unsigned int i = 0; i < fieldTypes.size(); ++i) {
      if (i != 0) os << " && ";
      os << "param" << i << " == o.param" << i;
    }
  }
  os << ";" << endl;
  os << "  }" << endl;
  os << "  bool operator!=(const " << name << "& o) const {return !(*this == o);}" << endl;
  os << "  bool operator<(const " << name << "& o) const {" << endl;
  if (fieldTypes.empty()) {
    os << "    return false;" << endl;
  } else {
    for (unsigned int i = 0; i < fieldTypes.size(); ++i) {
      if (i == fieldTypes.size() - 1) {
        os << "    return param" << i << " < o.param" << i << ";" << endl;
      } else {
        os << "    if (param" << i << " < o.param" << i << ") return true;" << endl;
        os << "    if (o.param" << i << " < param" << i << ") return false;" << endl;
      }
    }
  }
  os << "  }" << endl;
  // FIXME hash function?
  os << "};\n";
  return os.str();
}

string implementationTableType(const vector<string>& fieldTypes,
                               const vector<Mode>& modes) {
  vector<string> keyTypes, valueTypes;
  assert (fieldTypes.size() == modes.size());
  for (unsigned int i = 0; i < fieldTypes.size(); ++i) {
    switch (modes[i]) {
      case GROUND: keyTypes.push_back(fieldTypes[i]); break;
      case UNBOUND: valueTypes.push_back(fieldTypes[i]); break;
      default: break;
    }
  }
  if (false && keyTypes.empty() && valueTypes.empty()) { // Avoid needing to deal with a type without a constructor
    return "bool";
  } else if (valueTypes.empty()) { // This order ensures that all-+ modes always use a set
    return "std::set< " + makeTupleType(keyTypes) + " >";
  } else if (keyTypes.empty()) {
    return "std::vector< " + makeTupleType(valueTypes) + " >";
  } else {
    return "std::map< " + makeTupleType(keyTypes) + ", std::vector< " + makeTupleType(valueTypes) + " > >";
  }
}

char modeToChar(Mode m) {
  switch (m) {
    case GROUND: return 'p';
    case UNBOUND: return 'm';
    case WILDCARD: return 'u';
    default: abort();
  }
}

string mangleTableNameAndModes(const string& tableName,
                               const vector<Mode>& modes) {
  string result = (tableName == "=" ? "__eq__" : tableName) + "_";
  for (unsigned int i = 0; i < modes.size(); ++i) {
    result += modeToChar(modes[i]);
  }
  return result;
}

string implementationTableName(const TableMapKey& tableName,
                               const vector<Mode>& modes) {
  return "implementationTable_" + mangleTableNameAndModes(tableName.first, modes);
}

string astNodeToCpp(PrologAst::Node* n) {
  if (PrologAst::Variable* v = dynamic_cast<PrologAst::Variable*>(n)) {
    return v->getName();
  } else if (dynamic_cast<PrologAst::Wildcard*>(n)) {
    assert (!"Should not try to unparse wildcard");
  } else if (PrologAst::Integer* i = dynamic_cast<PrologAst::Integer*>(n)) {
    ostringstream os;
    os << i->getValue();
    return os.str();
  } else if (PrologAst::FloatingPoint* f =
               dynamic_cast<PrologAst::FloatingPoint*>(n)) {
    ostringstream os;
    os << std::showpoint << f->getValue();
    return os.str();
  } else if (PrologAst::String* s = dynamic_cast<PrologAst::String*>(n)) {
    return "std::string(\"" + escapeString(s->getValue()) + "\")";
  } else if (PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(n)) {
    string result = t->getFunctor();
    if (t->getArity() != 0) {
      result += '(';
      for (unsigned int i = 0; i < t->getArity(); ++i) {
        if (i != 0) result += ", ";
        result += astNodeToCpp(t->getArgs()[i]);
      }
      result += ')';
    }
    return result;
  } else {
    cerr << "Bad AST node to unparse: " << n << endl;
    abort();
  }
}

string getPrologString(PrologAst::Node* n) {
  PrologAst::String* s = dynamic_cast<PrologAst::String*>(n);
  PrologAst::Variable* v = dynamic_cast<PrologAst::Variable*>(n);
  PrologAst::Integer* i = dynamic_cast<PrologAst::Integer*>(n);
  PrologAst::FloatingPoint* f = dynamic_cast<PrologAst::FloatingPoint*>(n);
  if (s) {
    return s->getValue();
  } else if (v) {
    return v->getName();
  } else if (i || f) {
    ostringstream os;
    os << n;
    return os.str();
  } else {
    cerr << "Expected a string, found " << n << endl;
    exit(1);
  }
}

string getPrologAtomString(PrologAst::Node* n) {
  PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(n);
  if (t && t->getArity() == 0) {
    return t->getFunctor();
  } else {
    cerr << "Expected a nullary functor, found " << n << endl;
    exit (1);
  }
}

string plugInForIterationDefinition(
    const string& defn,
    const string& nameSuffix,
    const TableReference& ref,
    const string& body) {
  string result;
  // Codes in defn:
  // # expands to nameSuffix
  // $0..$9 is AST node (as C++) for ref.args[0].arg .. ref.args[9].arg
  // $s0..$s9 is raw string for ref.args[0].arg .. ref.args[9].arg
  // $t0..$t9 is type for ref.args[0] .. ref.args[9]
  // $r0..$r9 is raw streamed output for ref.args[0].arg .. ref.args[9].arg
  // @ expands to body
  // Everything else is passed on literally
  for (size_t i = 0; i < defn.size(); ++i) {
    switch (defn[i]) {
      case '#': {
        result += nameSuffix;
        break;
      }
      case '$': {
        assert (i + 1 < defn.size());
        switch (defn[i + 1]) {
          case 's': {
            assert (i + 2 < defn.size());
            assert (defn[i + 2] >= '0' && defn[i + 2] <= '9');
            size_t idx = defn[i + 2] - '0';
            result += getPrologString(ref.args[idx].arg);
            ++i;
            break;
          }
          case 't': {
            assert (i + 2 < defn.size());
            assert (defn[i + 2] >= '0' && defn[i + 2] <= '9');
            size_t idx = defn[i + 2] - '0';
            ostringstream os;
            os << ref.args[idx].type;
            result += os.str();
            ++i;
            break;
          }
          case 'r': {
            assert (i + 2 < defn.size());
            assert (defn[i + 2] >= '0' && defn[i + 2] <= '9');
            size_t idx = defn[i + 2] - '0';
            ostringstream os;
            os << ref.args[idx].arg;
            result += os.str();
            ++i;
            break;
          }
          default: {
            assert (defn[i + 1] >= '0' && defn[i + 1] <= '9');
            size_t idx = defn[i + 1] - '0';
            result += astNodeToCpp(ref.args[idx].arg);
            break;
          }
        }
        ++i;
        break;
      }
      case '@': {
        result += body;
        break;
      }
      default: {
        result += defn[i];
        break;
      }
    }
  }
  return result;
}

string generateStatement(TriggerCondition* tc,
                         const string& body = "") {
  ostringstream resultStream;
  if (dynamic_cast<NullTriggerCondition*>(tc)) {
    resultStream << body;
  } else if (dynamic_cast<AndTriggerCondition*>(tc)) {
    AndTriggerCondition* atc = dynamic_cast<AndTriggerCondition*>(tc);
    return generateStatement(atc->body1, generateStatement(atc->body2, body));
  } else if (dynamic_cast<NotTriggerCondition*>(tc)) {
    // Iterate through solutions to the nested condition, breaking out (and skipping past this statement's body) if one is found; if none is found, do the body
    unsigned int nameSuffix = ++variableInstantiationCounter;
    resultStream << "{" << endl;
    ostringstream nestedBody;
    nestedBody << "{goto paletteFoundElementForNegation" << nameSuffix << ";}\n";
    resultStream << generateStatement(dynamic_cast<NotTriggerCondition*>(tc)->body, nestedBody.str());
    resultStream << body;
    resultStream << "  paletteFoundElementForNegation" << nameSuffix << ": ;" << endl;
    resultStream << "}" << endl;
  } else if (dynamic_cast<TableRefTriggerCondition*>(tc)) {
    const TableReference& ref = dynamic_cast<TableRefTriggerCondition*>(tc)->tableRef;
    // cout << "// Waiting on " << body << endl;
    const TableMapKey& tableName = ref.getKey();
    bool isBuiltin = !tableName.first.empty() && tableName.first[0] == '<';
    if (isBuiltin) {
      assert (tableName.first.size() >= 2);
      assert (tableName.first[tableName.first.size() - 1] == '>');
      unsigned int nameSuffix = ++variableInstantiationCounter;
      resultStream << "// " << ref << endl;
      if (tableName == TableMapKey("<iterMemoryPools>", 2)) {
        vector<PrologAst::Node*> pools = prologListToVector(ref.args[1].arg);
        string var = getPrologString(ref.args[0].arg);
        string filename = findIncludeFile(mangleTableNameAndModes(tableName.first.substr(1, tableName.first.size() - 2), getModesFromTableRef(ref)) + ".itmpl");
        const string defn = PrologLexer::readFile(filename);
        for (unsigned int i = 0; i < pools.size(); ++i) {
          PrologAst::Term* t = dynamic_cast<PrologAst::Term*>(pools[i]);
          assert (t && t->getArity() == 0);
          TableReference ref2 = ref;
          ref2.args[1].arg = t;
          resultStream << plugInForIterationDefinition(defn, StringUtility::numberToString(nameSuffix), ref2, body);
        }
      } else {
        string filename = findIncludeFileMaybe(mangleTableNameAndModes(tableName.first.substr(1, tableName.first.size() - 2), getModesFromTableRef(ref)) + ".itmpl");
        if (!filename.empty()) {
          const string defn = PrologLexer::readFile(filename);
          resultStream << plugInForIterationDefinition(defn, StringUtility::numberToString(nameSuffix), ref, body);
        } else {
          cerr << "Bad built-in table name " << tableName.first << "/" << tableName.second << endl;
          exit(1);
        }
      }
    } else {
      vector<Mode> modes(ref.args.size());
      for (unsigned int i = 0; i < ref.args.size(); ++i) {
        modes[i] = ref.args[i].mode;
      }
      string name = implementationTableName(tableName, modes);
      vector<string> inputTypes, inputValues, outputTypes;
      vector<PrologAst::Node*> outputNames;
      for (unsigned int i = 0; i < ref.args.size(); ++i) {
        Mode m = ref.args[i].mode;
        string type = ref.args[i].type;
        switch (m) {
          case GROUND: inputTypes.push_back(type);
                       inputValues.push_back(astNodeToCpp(ref.args[i].arg));
                       break;
          case UNBOUND: outputTypes.push_back(type);
                        outputNames.push_back(ref.args[i].arg);
                        break;
          default: break;
        }
      }
      // This needs to match the corresponding tests in generateInsertFunction
      // and implementationTableType
      if (false && inputTypes.empty() && outputTypes.empty()) {
        resultStream << "if (" << (ref.negative ? "!" : "") << name << ") {" << endl;
        resultStream << body;
        resultStream << "}" << endl;
      } else if (outputTypes.empty()) {
        resultStream << "if (" << name << ".find(" << makeTuple(inputTypes, inputValues) << ") " << (ref.negative ? "==" : "!=") << " " << name << ".end()) {" << endl;
        resultStream << body;
        resultStream << "}" << endl;
      } else if (inputTypes.empty()) {
        assert (!ref.negative);
        unsigned int nameSuffix = ++variableInstantiationCounter;
        resultStream << "const unsigned int paletteContainerSize" << nameSuffix << " = " << name << ".size();" << endl; // Size may be changed during the iteration, but new elements will be handled separately
        string containerName = name;
        if (ref.sortInfo != UNSORTED_TABLE_REFERENCE) {
          resultStream << "std::vector< " << makeTupleType(outputTypes) << " > paletteContainerTemp" << nameSuffix << "(" << name << ");" << endl;
          containerName = "paletteContainerTemp" + StringUtility::numberToString(nameSuffix);
          switch (ref.sortInfo) {
            case ASCENDING_TABLE_REFERENCE: {
              resultStream << "std::sort(" << containerName << ".begin(), " << containerName << ".end());" << endl;
              break;
            }
            case DESCENDING_TABLE_REFERENCE: {
              resultStream << "std::sort(" << containerName << ".begin(), " << containerName << ".end(), Palette::PaletteGreater());" << endl;
              break;
            }
            case UNSORTED_TABLE_REFERENCE: break;
            default: {
              cerr << "Bad sorting type " << ref.sortInfo << endl;
              abort();
            }
          }
        }
        resultStream << "for (unsigned int paletteIterator" << nameSuffix << " = 0; paletteIterator" << nameSuffix << " != paletteContainerSize" << nameSuffix << "; ++paletteIterator" << nameSuffix << ") {" << endl;
        ostringstream iteratorDerefStream;
        iteratorDerefStream << containerName << "[paletteIterator" << nameSuffix << "]";
        for (unsigned int i = 0; i < outputNames.size(); ++i) {
          if (dynamic_cast<PrologAst::Variable*>(outputNames[i])) { // Skip wildcards
            resultStream << outputTypes[i] << " const& " << astNodeToCpp(outputNames[i]) << " = " << makeTupleElementRef(outputTypes, iteratorDerefStream.str(), i) << ";" << endl;
          }
        }
        resultStream << body;
        resultStream << "}" << endl;
      } else {
        assert (!ref.negative);
        unsigned int nameSuffix = ++variableInstantiationCounter;
        resultStream << "{" << endl;
        resultStream << "std::map< " << makeTupleType(inputTypes) << " , std::vector< " << makeTupleType(outputTypes) << " > >::const_iterator paletteContainerIterator" << nameSuffix << " = " << name << ".find(" << makeTuple(inputTypes, inputValues) << ");" << endl;
        resultStream << "if (paletteContainerIterator" << nameSuffix << " != " << name << ".end()) {" << endl;
        if (ref.sortInfo != UNSORTED_TABLE_REFERENCE) {
          resultStream << "std::vector< " << makeTupleType(outputTypes) << " > paletteContainerRef" << nameSuffix << " = paletteContainerIterator" << nameSuffix << "->second;" << endl; // Non-const copy
        } else {
          resultStream << "const std::vector< " << makeTupleType(outputTypes) << " >& paletteContainerRef" << nameSuffix << " = paletteContainerIterator" << nameSuffix << "->second;" << endl; // Const reference
        }
        switch (ref.sortInfo) {
          case ASCENDING_TABLE_REFERENCE: {
            resultStream << "std::sort(paletteContainerRef" << nameSuffix << ".begin(), paletteContainerRef" << nameSuffix << ".end());" << endl;
            break;
          }
          case DESCENDING_TABLE_REFERENCE: {
            resultStream << "std::sort(paletteContainerRef" << nameSuffix << ".begin(), paletteContainerRef" << nameSuffix << ".end(), Palette::PaletteGreater());" << endl;
            break;
          }
          case UNSORTED_TABLE_REFERENCE: break;
          default: {
            cerr << "Bad sorting type " << ref.sortInfo << endl;
            abort();
          }
        }
        resultStream << "const unsigned int paletteContainerSize" << nameSuffix << " = paletteContainerRef" << nameSuffix << ".size();" << endl; // Size may be changed during the iteration, but new elements will be handled separately
        resultStream << "for (unsigned int paletteIterator" << nameSuffix << " = 0; paletteIterator" << nameSuffix << " != paletteContainerSize" << nameSuffix << "; ++paletteIterator" << nameSuffix << ") {" << endl;
        ostringstream iteratorDerefStream;
        iteratorDerefStream << "paletteContainerRef" << nameSuffix << "[paletteIterator" << nameSuffix << "]";
        for (unsigned int i = 0; i < outputNames.size(); ++i) {
          if (dynamic_cast<PrologAst::Variable*>(outputNames[i])) { // Skip wildcards
            resultStream << outputTypes[i] << " const& " << astNodeToCpp(outputNames[i]) << " = " << makeTupleElementRef(outputTypes, iteratorDerefStream.str(), i) << ";" << endl;
          }
        }
        resultStream << body;
        resultStream << "}" << endl; // Close for
        resultStream << "}" << endl; // Close if
        resultStream << "}" << endl; // Close outer block
      }
    }
  } else {
    cerr << "Bad trigger condition" << endl;
    exit (1);
  }
  return resultStream.str();
}

string generateInsertFunctionCall(const TableReference& ref) {
  assert (ref.tableName != "" &&
          !ref.negative &&
          ref.sortInfo == UNSORTED_TABLE_REFERENCE);
  string result = insertFunctionName(ref.getKey()) + "(";
  for (unsigned int i = 0; i < ref.args.size(); ++i) {
    if (i != 0) result += ", ";
    result += astNodeToCpp(ref.args[i].arg);
  }
  result += ");\n";
  return result;
}

void generateTrigger(const Trigger& tr) {
  cout << generateStatement(tr.first, generateInsertFunctionCall(tr.second));
}

void generateInsertFunction(const TableMapKey& tableName,
                            const vector<Trigger>& triggers) {
  // cout << "// generateInsertFunction " << tableName << " " << triggers.size() << endl;
  if (tableName.first == "") return; // Special table used for queries
  TableMapType::const_iterator tableInfoIter = tables.find(tableName);
  if (tableInfoIter == tables.end()) {
    cerr << "Trying to insert into undefined table " << tableName.first << "/" << tableName.second << endl;
    exit(1);
  }
  const TableInfo& tableInfo = tableInfoIter->second;
  unsigned int paramCount = tableInfo.fieldTypes.size();
  vector<string> paramNames;
  for (unsigned int i = 0; i < paramCount; ++i) {
    ostringstream os;
    os << "Palette_Param" << i;
    paramNames.push_back(os.str());
  }
  cout << "void " << insertFunctionName(tableName) << "(";
  for (unsigned int i = 0; i < paramCount; ++i) {
    if (i != 0) cout << ", ";
    cout << tableInfo.fieldTypes[i] << " " << paramNames[i];
  }
  cout << ") {\n";
  cout << "  if (!" << implementationTableName(tableName, vector<Mode>(paramCount, GROUND)) << ".insert(" << makeTuple(tableInfo.fieldTypes, paramNames) << ").second) return;\n";
  for (set<vector<Mode> >::const_iterator i =
         tableInfo.implementationsRequired.begin();
       i != tableInfo.implementationsRequired.end(); ++i) {
    if (*i == vector<Mode>(paramCount, GROUND)) continue; // Already did this one
    string name = implementationTableName(tableName, *i);
    vector<string> keyTypes, keys, valueTypes, values;
    for (unsigned int j = 0; j < paramCount; ++j) {
      switch ((*i)[j]) {
        case UNBOUND: {
          valueTypes.push_back(tableInfo.fieldTypes[j]);
          values.push_back(paramNames[j]);
        } break;

        case GROUND: {
          keyTypes.push_back(tableInfo.fieldTypes[j]);
          keys.push_back(paramNames[j]);
        } break;

        case WILDCARD: break;
        default: assert (false);
      }
    }
    // This if statement must match the one in implementationTableType
    if (false && keyTypes.empty() && valueTypes.empty()) {
      cout << "  " << name << " = true;\n";
    } else if (valueTypes.empty()) {
      cout << "  " << name << ".insert(" << makeTuple(keyTypes, keys) << ");\n";
    } else if (keyTypes.empty()) {
      cout << "  " << name << ".push_back(" << makeTuple(valueTypes, values) << ");\n";
    } else {
      cout << "  " << name << "[" << makeTuple(keyTypes, keys) << "].push_back(" << makeTuple(valueTypes, values) << ");\n";
    }
  }
  for (unsigned int i = 0; i < triggers.size(); ++i) {
    cout << "// Working on trigger " << triggers[i].first << " -> " << triggers[i].second << endl;
    generateTrigger(triggers[i]);
  }
  cout << "}\n";
}

TriggerCondition* makeTriggerCondFromListOfTableRefs(const vector<TableReference>& tableRefs) {
  TriggerCondition* tc = new NullTriggerCondition;
  for (unsigned int i = tableRefs.size(); i > 0; --i) {
    tc = new AndTriggerCondition(tableRefToTriggerCondition(tableRefs[i - 1]), tc);
  }
  return tc;
}

int main(int argc, char** argv) {

  PrologParser::initParser();

  includePaths.clear();
  addIncludePath(".");
  vector<string> sourceFiles = parseParameters(argv[0], vector<char*>(argv + 1, argv + argc));
  addIncludePath(findRoseSupportPathFromSource("projects/palette/", "share"));
  addIncludePath(findRoseSupportPathFromBuild("projects/palette/", "share"));

  vector<PrologAst::Node*> program;
  for (unsigned int i = 0; i < sourceFiles.size(); ++i) {
    processOneFile(sourceFiles[i]);
  }
#if 0
  for (unsigned int i = 0; i < program.size(); ++i) {
    cout << program[i] << endl;
  }
#endif

#if 0
  for (TableMapType::const_iterator i = tables.begin();
       i != tables.end(); ++i) {
    cerr << i->first << ": " << i->second.fieldTypes << endl;
  }
#endif

  vector<vector<TableMapKey> > computationList = orderTableComputations();

  map<TableMapKey, vector<Trigger> > triggers; // Map from input table (or "main") to rule and index of recursive call within rule

  for (unsigned int scc = 0; scc < computationList.size(); ++scc) {
    const vector<TableMapKey>& tablesToDo = computationList[scc];
    for (vector<Rule>::const_iterator i = rules.begin();
         i != rules.end(); ++i) {
      const Rule& r = *i;
      if (std::find(tablesToDo.begin(), tablesToDo.end(), r.targetTable())
            == tablesToDo.end()) {
        continue; // Not in this SCC
      }
      bool anyRecursionsInThisRule = false;
      for (unsigned int goalIndex = 0;
           goalIndex < r.rhs.size(); ++goalIndex) {
        const TableReference& goal = r.rhs[goalIndex];
        TableMapType::const_iterator found
          = tables.find(goal.getKey());
        if (found == tables.end() &&
            (goal.tableName != "=" || goal.args.size() != 2)) {
          cerr << "Unknown table " << goal.tableName << endl;
          exit(1);
        }
        if ((goal.tableName != "=" || goal.args.size() != 2) &&
            !found->second.isPrimitive &&
            found->second.computationIndex == scc) {
          // Recursive call within this SCC
          assert (found != tables.end());
          if (goal.negative) {
            cerr << "Bad negation of call within same SCC (not stratified)" << endl;
            exit(1);
          }
          if (goal.sortInfo != UNSORTED_TABLE_REFERENCE) {
            cerr << "Bad sorting of call within same SCC (not stratified)" << endl;
            exit(1);
          }
          anyRecursionsInThisRule = true;
          Trigger trig(makeTriggerCondFromListOfTableRefs(r.rhsMinusOneGoal(goalIndex)), r.lhs);
          for (unsigned int i = goal.args.size(); i > 0; --i) {
            vector<TableReferenceArg> args(2);
            ostringstream varNameStream;
            varNameStream << "Palette_Param" << i - 1;
            args[0] = TableReferenceArg(new PrologAst::Variable(varNameStream.str()), GROUND, found->second.fieldTypes[i - 1]);
            args[1] = goal.args[i - 1];
            TableReference tr("=", args, false, UNSORTED_TABLE_REFERENCE);
            tr.args[0].mode = GROUND; // So it won't be assigned later
            trig.first = new AndTriggerCondition(tableRefToTriggerCondition(tr), trig.first);
          }
          triggers[goal.getKey()].push_back(trig);
        }
      }
      if (!anyRecursionsInThisRule) {
        triggers[TableMapKey("main", 0)].push_back(Trigger(makeTriggerCondFromListOfTableRefs(r.rhs), r.lhs));
      }
    }
  }

  // Add the queries to main
  for (unsigned int i = 0; i < queries.size(); ++i) {
    triggers[TableMapKey("main", 0)].push_back(Trigger(makeTriggerCondFromListOfTableRefs(queries[i]), TableReference("")));
  }

  // Assign modes to all of the joins, possibly adding new equality
  // constraints for built-in tables
  for (map<TableMapKey, vector<Trigger> >::iterator i = triggers.begin();
       i != triggers.end(); ++i) {
    for (unsigned int j = 0; j < i->second.size(); ++j) {
      Trigger& trig = i->second[j];
      assignModes(i->first, trig);
      // Request an insert function for each insertion
      requestInsertFunction(trig.second.getKey());
    }
  }

#if 0
  for (map<TableMapKey, vector<Trigger> >::const_iterator i =
         triggers.begin();
       i != triggers.end(); ++i) {
    cerr << "Triggers for " << i->first.first << "/" << i->first.second << ":" << endl;
    for (unsigned int j = 0; j < i->second.size(); ++j) {
      TriggerCondition* tc = i->second[j].first;
      // vector<TableReference> joinElts = i->second[j].first;
      TableReference lhs = i->second[j].second;
      cerr << "  "; tc->unparse(cerr); cerr << " -> " << lhs << endl;
    }
  }
#endif

  // Start generating code, starting with the implementation tables
  
  // The table defs need to be saved because they may refer to tuple types
  // which need to be defined earlier in the program
  string implementationTableDefs;
  for (TableMapType::iterator i = tables.begin();
       i != tables.end(); ++i) {
    if (i->second.isPrimitive) continue;
    i->second.implementationsRequired.insert(vector<Mode>(i->second.fieldTypes.size(), GROUND)); // For duplicate checking
    for (set<vector<Mode> >::const_iterator j =
           i->second.implementationsRequired.begin();
         j != i->second.implementationsRequired.end(); ++j) {
      implementationTableDefs += 
        implementationTableType(i->second.fieldTypes, *j) +
        " " +
        implementationTableName(i->first, *j) + ";\n";
    }
  }

  // Produce the final code on cout
  if (templateName == "") {
    cerr << "Need to specify a code template to use" << endl;
    exit(1);
  }
  ifstream tmpl(findIncludeFile(templateName + ".tmpl").c_str());
  while (tmpl) {
    string line;
    getline(tmpl, line);
    if (line == "@@@@@") {
      // First, do all of the tuple definitions
      for (map<vector<string>, string>::const_iterator i = tupleTypeNames.begin();
           i != tupleTypeNames.end(); ++i) {
        cout << makeTupleDefinition(i->first);
      }

      // Then, the implementation tables
      cout << implementationTableDefs;

      // Then, the insert functions and included triggers
      for (set<TableMapKey>::const_iterator i = insertFunctionsNeeded.begin();
           i != insertFunctionsNeeded.end(); ++i) {
        generateInsertFunction(*i, triggers[*i]);
      }

      // Then, the special "main" trigger which doesn't correspond to a table
      cout << "void paletteMain() {\n";
      const vector<Trigger>& mainTriggers = triggers[TableMapKey("main", 0)];
      for (unsigned int i = 0; i < mainTriggers.size(); ++i) {
        if (mainTriggers[i].second.tableName == "") { // Query
          cout << generateStatement(mainTriggers[i].first);
        } else {
          generateTrigger(mainTriggers[i]);
        }
      }
      cout << "}" << endl;
    } else {
      cout << line << endl;
    }
  }

  return 0;
}
