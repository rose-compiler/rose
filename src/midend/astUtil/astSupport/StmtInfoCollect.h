
#ifndef DEP_INFO_COLLECT_H
#define DEP_INFO_COLLECT_H

#include <ProcessAstTree.h>
#include <AnalysisInterface.h>
#include <union_find.h>
#include <map>

class AstInterface;
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
  typedef std::map<AstNodePtr, ModRecord > ModMap;
  struct ModStackEntry {
      AstNodePtr root;
      ModMap modmap;
      ModStackEntry(const AstNodePtr& r) : root(r) {}
  };
  std::list<ModStackEntry> modstack;
  AstNodePtr curstmt;

  virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& mod, 
                              const AstNodePtr& rhs = 0) = 0;
  virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& read) = 0; 
  virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& fc) = 0; 

  void AppendFuncCallArguments( AstInterface& fa, const AstNodePtr& fc) ; 
  void AppendFuncCallWrite( AstInterface& fa, const AstNodePtr& fc) ; 
 
//virtual Boolean ProcessTree( AstInterface &_fa, const AstNodePtr& s,
  virtual int ProcessTree( AstInterface &_fa, const AstNodePtr& s,
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
                              const AstNodePtr& rhs = 0) ;
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

class ScopeStringMap {
  std::map<AstNodePtr, std::string> scopemap;
  int cur;
 public:
  ScopeStringMap() : cur(0) {}
  std::string get_scope_string( const AstNodePtr& s); 
  std::string get_scope_string( const AstNodePtr& s) const; 
};

class FunctionAliasInterface;
// flow insensitive alias analysis for named variables only
class StmtVarAliasCollect 
: public StmtInfoCollect, public AliasAnalysisInterface
{
 public:
   class VarAliasMap {
      std::map<std::string, UF_elem> aliasmap;
      ScopeStringMap scopemap;
     public:
        UF_elem& get_alias_map( const std::string& varname, const AstNodePtr& scope);
   };
 private:
  FunctionAliasInterface* funcanal;
  VarAliasMap aliasmap;
  bool hasunknown, hasresult;

  UF_elem& get_alias_map( const std::string& varname, const AstNodePtr& scope);
  virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& mod,
                              const AstNodePtr& rhs = 0);
  virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& fc);
  virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& read) {}
 public:
  StmtVarAliasCollect( FunctionAliasInterface* a = 0) 
    : funcanal(a), hasunknown(false), hasresult(false) {}
  void operator()( AstInterface& fa, const AstNodePtr& funcdefinition);
  bool may_alias(AstInterface& fa, const AstNodePtr& r1, 
		 const AstNodePtr& r2);
};

template <class Select>
class ModifyVariableMap : public CollectObject<std::pair<AstNodePtr,AstNodePtr> >
{
  AstInterface& ai;
  std::map <std::string, std::set<AstNodePtr> > varmodInfo;
  Select sel;
//Boolean operator()(const std::pair<AstNodePtr,AstNodePtr>& cur)
  int operator()(const std::pair<AstNodePtr,AstNodePtr>& cur)
   {
     std::string varname;
     if (AstInterface::IsVarRef(cur.first,0, &varname)) {
         AstNodePtr l = ai.GetParent(cur.first);
         std::set<AstNodePtr>& cur = varmodInfo[varname];
         for ( ; l != 0; l = ai.GetParent(l)) {
           if (sel(ai,l)) 
              cur.insert(l);
         }
     }
     return true;
   }
  public:
   ModifyVariableMap(AstInterface& _ai, Select _sel) : ai(_ai), sel(_sel) {}
   void Collect(const AstNodePtr& root, FunctionSideEffectInterface* a = 0) 
     {
      StmtSideEffectCollect(a).get_side_effect(ai, root, this);
     }
   bool Modify( const AstNodePtr& l, const std::string& varname) const
      { 
         std::map <std::string, std::set<AstNodePtr> >::const_iterator p = varmodInfo.find(varname);
         if (p != varmodInfo.end()) {
            if (l == 0)
                 return true;
            const std::set<AstNodePtr>& cur = (*p).second;
            return cur.find(l) != cur.end();
         }
         else
           return false;
      }
};
 

#endif
