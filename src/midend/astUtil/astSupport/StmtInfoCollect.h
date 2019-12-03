
#ifndef STMT_INFO_COLLECT_H
#define STMT_INFO_COLLECT_H

#include "AstInterface.h"
#include "SinglyLinkedList.h"
#include "ProcessAstTree.h"
#include "AnalysisInterface.h"
#include "union_find.h"
#include <map>
#include <list>
#include <sstream>

class StmtInfoCollect : public ProcessAstTreeBase
{ 
 protected:
  struct ModRecord{
     AstNodePtr rhs;
     bool readlhs;
     ModRecord() : readlhs(false) {}
     ModRecord( const AstNodePtr& _rhs, bool _readlhs)
      : rhs(_rhs), readlhs(_readlhs) {}
  };
  typedef std::map<AstNodePtr, ModRecord, std::less<AstNodePtr> > ModMap;
  struct ModStackEntry {
      AstNodePtr root;
      ModMap modmap;
      ModStackEntry(const AstNodePtr& r) : root(r) {}
  };
  std::list<ModStackEntry> modstack;
  AstNodePtr curstmt;
 protected:
  virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& mod, 
                              const AstNodePtr& rhs = AstNodePtr()) = 0;
  virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& read) = 0; 
  virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& fc) = 0; 

  void AppendFuncCallArguments( AstInterface& fa, const AstNodePtr& fc) ; 
  void AppendFuncCallWrite( AstInterface& fa, const AstNodePtr& fc) ; 
 
  virtual bool ProcessTree( AstInterface &_fa, const AstNodePtr& s,
                               AstInterface::TraversalVisitType t);
 public:
  void operator()( AstInterface& fa, const AstNodePtr& h) ;
};

class FunctionSideEffectInterface;
class StmtSideEffectCollect 
: public StmtInfoCollect, public SideEffectAnalysisInterface
{
 private:
  bool modunknown, readunknown;
  FunctionSideEffectInterface* funcanal;
  CollectObject< std::pair<AstNodePtr,AstNodePtr> > *modcollect, *readcollect, *killcollect;


  virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& mod,
                              const AstNodePtr& rhs = AstNodePtr()) ;
  virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& read) ;
  virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& fc);
 public:
  StmtSideEffectCollect( FunctionSideEffectInterface* a=0) 
     : modunknown(false), readunknown(false),funcanal(a), modcollect(0), 
       readcollect(0), killcollect(0) {}
  bool get_side_effect(AstInterface& fa, const AstNodePtr& h,
                       CollectObject<std::pair<AstNodePtr,AstNodePtr> >* collectmod,
                       CollectObject<std::pair<AstNodePtr,AstNodePtr> >* collectread = 0,
                       CollectObject<std::pair<AstNodePtr,AstNodePtr> >* collectkill = 0)
    { return operator()( fa, h, collectmod, collectread, collectkill); }
  bool operator()( AstInterface& fa, const AstNodePtr& h, 
                   CollectObject< std::pair<AstNodePtr,AstNodePtr> >* mod, 
                   CollectObject< std::pair<AstNodePtr,AstNodePtr> >* read=0,
                   CollectObject< std::pair<AstNodePtr,AstNodePtr> >* kill = 0) 
    {
      modcollect = mod;
      readcollect = read;
      killcollect = kill;
      modunknown = readunknown = false;
      StmtInfoCollect::operator()(fa, h);
      return !modunknown && !readunknown;
    }
};

class Ast2StringMap {
  typedef std::map<AstNodePtr, std::string, std::less<AstNodePtr> > MapType;
  MapType astmap;
  int cur;
  static Ast2StringMap* handle;
  Ast2StringMap() : cur(0) {}
  ~Ast2StringMap() {}
 public:
  static Ast2StringMap* inst();
  std::string get_string( const AstNodePtr& s); 
  std::string get_string( const AstNodePtr& s) const; 
  std::string lookup_string( const AstNodePtr& s) const; 
};

class InterProcVariableUniqueRepr {
 public: 

  static std:: string  // get name for the ith parameter of function 
  get_unique_name(const std::string& fname, int i) {
    std::stringstream out;
    out << i;
    std::string res = fname + "-" + out.str();
    return res;
  }

  static std:: string 
  get_unique_name(const std::string& fname, const AstNodePtr& scope, 
                               const std::string& varname)
    { return fname + "-" + (scope==0? "" : Ast2StringMap::inst()->get_string(scope))
                   + "-" + varname; }

  static std:: string 
  get_unique_name(AstInterface& fa, const AstNodePtr& exp);

  static std:: string 
  get_unique_name(AstInterface& fa, const AstNodePtr& scope, const std::string& varname);
};

class FunctionAliasInterface;
// flow insensitive alias analysis for named variables only
class StmtVarAliasCollect 
: public StmtInfoCollect, public AliasAnalysisInterface
{
 public:
   class VarAliasMap {  // name -> disjoint-set ID, we only need to check if two variables belong to the same group
      std::map<std::string, UF_elem*, std::less<std::string> > aliasmap;
      static Ast2StringMap scopemap;
     public:
       ~VarAliasMap() {
         for (std::map<std::string, UF_elem*, std::less<std::string> >::
             const_iterator p = aliasmap.begin(); p != aliasmap.end(); ++p) {
               delete (*p).second;
         }
       }
       //! Obtain the Union-find element from a variable within a scope
       UF_elem* get_alias_map( const std::string& varname, const AstNodePtr& scope);
   };
 private:
  FunctionAliasInterface* funcanal;
  VarAliasMap aliasmap;
  bool hasunknown, hasresult;

  UF_elem* get_alias_map( const std::string& varname, const AstNodePtr& scope);
  virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& mod,
                              const AstNodePtr& rhs = AstNodePtr());
  virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& fc);
  virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& read) {}
 public:
  StmtVarAliasCollect( FunctionAliasInterface* a = 0) 
    : funcanal(a), hasunknown(false), hasresult(false) {}
  virtual void analyze(AstInterface& fa, const AstNodePtr& funcdefinition);
  void operator()( AstInterface& fa, const AstNodePtr& funcdefinition);
  bool may_alias(AstInterface& fa, const AstNodePtr& r1, 
                 const AstNodePtr& r2);
};

template <class Select>
class ModifyVariableMap 
   : public CollectObject<std::pair<AstNodePtr,AstNodePtr> >,
     public StmtSideEffectCollect
{
  AstInterface& ai;
  class VarModSet : public std::set<AstNodePtr> {};
  typedef std::map <std::string, VarModSet, std::less<std::string> > VarModInfo;
  VarModInfo varmodInfo;
  Select sel;
  bool operator()(const std::pair<AstNodePtr,AstNodePtr>& cur)
   {
     std::string varname;
     if (ai.IsVarRef(cur.first,0, &varname)) {
         AstNodePtr l = ai.GetParent(cur.first);
         VarModSet& cur = varmodInfo[varname];
         for ( ; l != AST_NULL; l = ai.GetParent(l)) {
           if (sel(ai,l))
              cur.insert(l);
         }
     }
     return true;
   }
  public:
   AstInterface& get_astInterface() { return ai; }
   ModifyVariableMap(AstInterface& _ai, Select _sel,
                     FunctionSideEffectInterface* a=0) 
     : StmtSideEffectCollect(a), ai(_ai), sel(_sel) {}
   void Collect(const AstNodePtr& root)
     {
      StmtSideEffectCollect::get_side_effect(ai, root, this);
     }
   bool Modify( const AstNodePtr& l, const std::string& varname) const
      { 
         typename VarModInfo::const_iterator p = varmodInfo.find(varname);
         if (p != varmodInfo.end()) {
            if (l == AST_NULL)
                 return true;
            const VarModSet& cur = (*p).second;
            return cur.find(l) != cur.end();
         }
         else
           return false;
      }
};
 

#endif
