
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

class StmtInfoCollect : public ProcessAstTreeBase<AstInterface::AstNodePtr>
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
  using ProcessAstTreeBase<AstInterface::AstNodePtr>::Skip;
  virtual void AppendVariableDecl(AstInterface& fa, const AstNodePtr& variable, const AstNodePtr& var_init) = 0; 
  virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& mod, 
                              const AstNodePtr& rhs = AstNodePtr()) = 0;
  virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& read, 
                              const AstNodePtr& lhs = 0) = 0; 
  virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& fc) = 0; 
  virtual void AppendMemoryAllocate( AstInterface& /* fa */, const AstNodePtr& /* s */) {}
  virtual void AppendMemoryFree( AstInterface& /* fa */, const AstNodePtr& /* s */) {}

  // Analyzes call arguments to determine what are read. Returns callee if requested.
  void AppendFuncCallArguments( AstInterface& fa, const AstNodePtr& fc, AstNodePtr* callee) ; 
  void AppendFuncCallWrite( AstInterface& fa, const AstNodePtr& fc) ; 
 
  virtual bool ProcessTree( AstInterface &_fa, const AstInterface::AstNodePtr& s,
                               AstInterface::TraversalVisitType t) override;
 public:
  void operator()( AstInterface& fa, const AstNodePtr& h) ;
};

class FunctionSideEffectInterface;
class StmtSideEffectCollect : public StmtInfoCollect, public SideEffectAnalysisInterface
{
  using SideEffectAnalysisInterface::modcollect;
  using SideEffectAnalysisInterface::killcollect;
  using SideEffectAnalysisInterface::readcollect;
  using SideEffectAnalysisInterface::callcollect;
  using SideEffectAnalysisInterface::allocate_collect;
  using SideEffectAnalysisInterface::free_collect;
  using SideEffectAnalysisInterface::varcollect;
 public:
  StmtSideEffectCollect(AstInterface& fa, FunctionSideEffectInterface* a=0) 
     : fa_(fa), modunknown(false), readunknown(false),funcanal(a) {} 

  typedef typename SideEffectAnalysisInterface::CollectObject  CollectObject;

  virtual bool get_side_effect(AstInterface& /*fa*/, const AstNodePtr& h) override {
      return operator() (h);
  }

  bool operator() (const AstNodePtr& h) {
      modunknown = readunknown = false;
      StmtInfoCollect::operator()(fa_, h);
      return !modunknown && !readunknown;
    }

  protected:
    using StmtInfoCollect::AppendFuncCallArguments;
    using StmtInfoCollect::AppendFuncCallWrite;
    virtual void AppendVariableDecl(AstInterface& /* fa */, const AstNodePtr& variable, const AstNodePtr& var_init) override;
    virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& mod,
                              const AstNodePtr& rhs = AstNodePtr()) override;
    virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& read, 
                              const AstNodePtr& lhs = 0) override;
    virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& fc) override;
    virtual void AppendMemoryAllocate( AstInterface& /* fa */, const AstNodePtr& s) override;
    virtual void AppendMemoryFree( AstInterface& /* fa */, const AstNodePtr& s) override;
    AstInterface& fa_;
  private:
    using StmtInfoCollect::curstmt;
    bool modunknown, readunknown;
    FunctionSideEffectInterface* funcanal;
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
  virtual void AppendVariableDecl(AstInterface& fa, const AstNodePtr& variable, const AstNodePtr& var_init) override; 
  virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& mod,
                              const AstNodePtr& rhs = AstNodePtr()) override;
  virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& fc) override;
  virtual void AppendReadLoc(AstInterface&, const AstNodePtr&, 
                              const AstNodePtr& lhs = 0)  override {}
 public:
  StmtVarAliasCollect( FunctionAliasInterface* a = 0) 
    : funcanal(a), hasunknown(false), hasresult(false) {}
  virtual void analyze(AstInterface& fa, const AstNodePtr& funcdefinition);
  void operator()( AstInterface& fa, const AstNodePtr& funcdefinition);
  bool may_alias(AstInterface& fa, const AstNodePtr& r1, 
                 const AstNodePtr& r2);
};

/* Support tracking modification of variables, by building a map from each variable name
 * to all the AST nodes where the variable is modified */
template <class Select>
class ModifyVariableMap : public StmtSideEffectCollect
{
  class VarModSet : public std::set<AstNodePtr> {};
  typedef std::map <std::string, VarModSet, std::less<std::string> > VarModInfo;
  VarModInfo varmodInfo;
  Select sel;
  std::function<bool(AstNodePtr,AstNodePtr)> collect_modify;

  public:
   AstInterface& get_astInterface() { return fa_; }
   ModifyVariableMap(AstInterface& _fa, Select _sel,
                     FunctionSideEffectInterface* a=0) 
     : StmtSideEffectCollect(_fa,a), sel(_sel) {
      collect_modify = [this](AstNodePtr mod_first, AstNodePtr /*mod_second*/) {
         std::string varname;
         if (fa_.IsVarRef(mod_first,0, &varname)) {
             AstNodePtr l = fa_.GetParent(mod_first);
             VarModSet& cur = varmodInfo[varname];
             for ( ; l != 0; l = fa_.GetParent(l)) {
               if (sel(fa_,l))
                  cur.insert(l);
             }
         }
         return true;
      };
      StmtSideEffectCollect::set_modify_collect(collect_modify);
   }
   void Collect(const AstNodePtr& root)
     {
      StmtSideEffectCollect::get_side_effect(fa_, root);
     }
   bool Modify( const AstNodePtr& l, const std::string& varname) const
      { 
         typename VarModInfo::const_iterator p = varmodInfo.find(varname);
         if (p != varmodInfo.end()) {
            if (l == 0)
                 return true;
            const VarModSet& cur = (*p).second;
            return cur.find(l) != cur.end();
         }
         else
           return false;
      }
};

#endif 
