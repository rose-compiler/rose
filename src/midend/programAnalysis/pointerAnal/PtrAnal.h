#ifndef PTR_ANAL_H
#define PTR_ANAL_H
#include <boost/unordered_map.hpp>
#include <ProcessAstTree.h>
#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <CFG.h>

class PtrAnal 
: public ProcessAstTreeBase, public AliasAnalysisInterface
{
 public:
   typedef enum { EQ, NE, LT, GT, LE, GE, OTHER } OpType;
   typedef void* Stmt;
   struct VarRef { Stmt stmt; std::string name;
                   VarRef(Stmt _stmt=0, const std::string& _name="") 
                    : stmt(_stmt), name(_name) {};
                 };
  typedef std::list<Stmt>  StmtRef;

  void operator()( AstInterface& fa,  const AstNodePtr& program);
  bool may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2);
  VarRef translate_exp(const AstNodePtr& exp) const;
  StmtRef translate_stmt(const AstNodePtr& stmt) const;

  virtual bool may_alias(const std::string& x, const std::string& y) = 0;
  virtual Stmt x_eq_y(const std::string& x, const std::string& y) = 0; 
  virtual Stmt x_eq_addr_y(const std::string& x, const std::string& y) = 0; 
  virtual Stmt x_eq_deref_y(const std::string& x, 
                            const std::string& field,
                            const std::string& y) = 0;
  virtual Stmt x_eq_field_y(const std::string& x, 
                            const std::string& field,
                            const std::string& y) = 0;
  virtual Stmt deref_x_eq_y(const std::string& x, 
                            const std::list<std::string>& field,
                            const std::string& y) = 0;
  virtual Stmt field_x_eq_y(const std::string& x, 
                            const std::list<std::string>& field,
                            const std::string& y) = 0;
  virtual Stmt allocate_x(const std::string& x) = 0;
  virtual Stmt x_eq_op_y(OpType op, const std::string& x, const std::list<std::string>& y) =0;
  virtual Stmt funcdef_x(const std::string& x, const std::list<std::string>& params,
                          const std::list<std::string>& ouput) = 0;
  virtual Stmt funccall_x ( const std::string& x, const std::list<std::string>& args,
                            const std::list<std::string>& result)=0; 
  virtual Stmt funcexit_x( const std::string& x) = 0;

  virtual void contrl_flow(Stmt stmt1, Stmt stmt2, CFGConfig::EdgeType t) {}

private:
   class hash {
    public:
      size_t operator()(void * p) const { return (size_t) p; }
   };
  typedef boost::unordered_map<void*, VarRef, PtrAnal::hash> NameMap;
  typedef boost::unordered_map<void*, std::pair<size_t,size_t>, PtrAnal::hash> StmtMap;

  std::list<std::string> fdefined;
  NameMap namemap;
  StmtMap stmtmap;
  std::vector<Stmt> stmts;
  std::list<size_t> stmt_active;

 protected:
  void ProcessAssign( AstInterface& fa, const AstNodePtr& mod, const AstNodePtr& rhs, bool readlhs=0);
  void ProcessExpression( AstInterface& fa, const std::string& modname, const AstNodePtr& rhs);
  void ProcessMod(AstInterface& fa, const std::string& readname, 
                  std::list<std::string>& fields, const AstNodePtr& mod);

  void ControlFlowAnalysis(AstInterface& fa, const AstNodePtr& head, Stmt defn);

  std::string Get_VarName(AstInterface& fa, const AstNodePtr& rhs);

  virtual bool ProcessTree( AstInterface &_fa, const AstNodePtr& s,
                               AstInterface::TraversalVisitType t);
  static std::string get_func_output(const std::string& fname, int i) 
   { return InterProcVariableUniqueRepr::get_unique_name(fname+"-return",i); }
  static std::string get_func_par(const std::string& fname, int i) 
   { return InterProcVariableUniqueRepr::get_unique_name(fname+"-par",i); }
};
#endif
