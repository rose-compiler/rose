
#ifndef STMT_INFO_COLLECT_H
#define STMT_INFO_COLLECT_H

#include "AstInterface.h"
#include "SinglyLinkedList.h"
#include "ProcessAstTree.h"
#include "AnalysisInterface.h"
#include "union_find.h"
#include <map>
#include <sstream>
//! An abstract class to collect read/mod/call information
class StmtInfoCollect : public ProcessAstTreeBase
{ 
 protected:
  //Store the rhs operand (read) of operations, indicate if lhs is also read 
  struct ModRecord{
     AstNodePtr rhs;
     bool readlhs; //Some operations like +=, -= also read lhs operand
     // default constructor: 
     ModRecord() : readlhs(false) {}
     // Constructor
     ModRecord( const AstNodePtr& _rhs, bool _readlhs)
      : rhs(_rhs), readlhs(_readlhs) {}
  };
  // A map between a lhs operand (write) and its corresponding rhs operand (read)
  typedef std::map<AstNodePtr, ModRecord, std::less<AstNodePtr> > ModMap;

  struct ModStackEntry {
      AstNodePtr root; //The statement/expression in question
      ModMap modmap;   // Its lhs and rhs operand
      ModStackEntry(const AstNodePtr& r) : root(r) {}
  };
  //Use: modstack.back().modmap[lhs] =  ModRecord( rhs,readlhs); 
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
//! Collect statement side effects with the help of side effect analysis Interface
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
  //! Collect mod, read, and kill information for a node h using an interface fa  
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
 public:
  Ast2StringMap() : cur(0) {}
  std::string get_string( const AstNodePtr& s); 
  std::string get_string( const AstNodePtr& s) const; 
  std::string lookup_string( const AstNodePtr& s) const;
};

class InterProcVariableUniqueRepr {
  static Ast2StringMap astmap;
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
    { return fname + "-" + (scope==0? "" : astmap.get_string(scope))
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
   class VarAliasMap {
      std::map<std::string, UF_elem*, std::less<std::string> > aliasmap;
      Ast2StringMap scopemap;
     public:
       ~VarAliasMap() {
         for (std::map<std::string, UF_elem*, std::less<std::string> >::
             const_iterator p = aliasmap.begin(); p != aliasmap.end(); ++p) {
               delete (*p).second;
         }
       }
       //! Find the alias group of a variable named 'varname' within 'scope'
      // Create a new alias group if it does not exist for this variable
       UF_elem* get_alias_map( const std::string& varname, const AstNodePtr& scope);
   };
 private:
  FunctionAliasInterface* funcanal;
  VarAliasMap aliasmap;
  bool hasunknown, hasresult;

  UF_elem* get_alias_map( const std::string& varname, const AstNodePtr& scope);
  // Inherited from StmtInfoCollect
  virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& mod,
                              const AstNodePtr& rhs = AstNodePtr());
  virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& fc);
  virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& read) {}
 public:
  StmtVarAliasCollect( FunctionAliasInterface* a = 0) 
    : funcanal(a), hasunknown(false), hasresult(false) {}
  void operator()( AstInterface& fa, const AstNodePtr& funcdefinition);
  // Inherited from AliasAnalysisInterface: if r1 and r2 may be alias to each other
  bool may_alias(AstInterface& fa, const AstNodePtr& r1, 
                 const AstNodePtr& r2);
};

//! An interface to collect modified variables for a set of AST nodes chosen by 'Select', such as loop nodes 
// and to query if a variable is modifed within the node (e.g: loop)
template <class Select>
class ModifyVariableMap 
   : public CollectObject<std::pair<AstNodePtr,AstNodePtr> >,
     public StmtSideEffectCollect
{
  AstInterface& ai;
  // A set of AST node pointers
  class VarModSet : public std::set<AstNodePtr> {};
  // A map between a named variable and the AST nodes(selected by 'Select') modifying it.
  typedef std::map <std::string, VarModSet, std::less<std::string> > VarModInfo;
  VarModInfo varmodInfo;
  Select sel;
  // Collect the loops containing nodes modifying a variable
  bool operator()(const std::pair<AstNodePtr,AstNodePtr>& cur)
   {
     std::string varname;
     if (ai.IsVarRef(cur.first,0, &varname)) {
         AstNodePtr l = ai.GetParent(cur.first);
         VarModSet& modset = varmodInfo[varname];
         // Collect all the parent loops containing the variable reference node into the var mod set
         for ( ; l != AST_NULL; l = ai.GetParent(l)) {
           if (sel(ai,l)) 
              modset.insert(l);
         }
     }
     return true;
   }
  public:
   ModifyVariableMap(AstInterface& _ai, Select _sel,
                     FunctionSideEffectInterface* a=0) 
     : StmtSideEffectCollect(a), ai(_ai), sel(_sel) {}
   void Collect(const AstNodePtr& root)
     {
      StmtSideEffectCollect::get_side_effect(ai, root, this);
     }
   // Check if a node ( e.g: loop node) modifies a variable named 'varname'  
   bool Modify( const AstNodePtr& l, const std::string& varname) const
      { 
         // Find mode set for the variable 
         typename VarModInfo::const_iterator p = varmodInfo.find(varname);
         if (p != varmodInfo.end()) {
            // Return true if the node is NULL. It means this variable is actually modified somewhere.
            if (l == AST_NULL)
                 return true;
            const VarModSet& cur = (*p).second;
            // Find node in the mod set
            return cur.find(l) != cur.end();
         }
         else
           return false;
      }
};
 

#endif
