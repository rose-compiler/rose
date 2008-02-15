#ifndef STEEENSGAARD_PTR_ANAL_H
#define STEEENSGAARD_PTR_ANAL_H
#include <PtrAnal.h>
#include <IteratorTmpl.h>
#include <steensgaard.h>


class SteensgaardPtrAnal : public PtrAnal, private ECRmap
{
 private:
  typedef ECRmap Impl;
  virtual bool may_alias(const std::string& x, const std::string& y) 
      { return Impl::mayAlias(x, y); }
  virtual Stmt x_eq_y(const std::string& x, const std::string& y) 
      { Impl:: x_eq_y(x, y); return 0; }
  virtual Stmt x_eq_addr_y(const std::string& x, const std::string& y) 
      { Impl::x_eq_addr_y(x, y); return 0; }
  virtual Stmt x_eq_deref_y(const std::string& x, const std::string& field,
                             const std::string& y) 
      { Impl::x_eq_deref_y(x, y); return 0; }
  virtual Stmt x_eq_field_y(const std::string& x, const std::string& field,
                             const std::string& y) 
      { Impl::x_eq_y(x, y); return 0; }
  virtual Stmt deref_x_eq_y(const std::string& x, 
                   const std::list<std::string>& fields, const std::string& y) 
      { Impl::deref_x_eq_y(x,y);  return 0; }
  virtual Stmt field_x_eq_y(const std::string& x, 
                   const std::list<std::string>& fields, const std::string& y) 
      { Impl::x_eq_y(x,y);  return 0; }
  virtual Stmt x_eq_op_y(OpType op, const std::string& x, const std::list<std::string>& y) 
      { Impl::x_eq_op_y(x,y); return 0; }
  virtual Stmt allocate_x(const std::string& x) 
      { Impl::allocate(x); return 0; }
  virtual Stmt funcdef_x(const std::string& x, 
                          const std::list<std::string>& params,
                          const std::list<std::string>& output) 
      { Impl::function_def_x(x,params,output); return 0; }
  virtual Stmt funccall_x ( const std::string& x, const std::list<std::string>& args,
                            const std::list<std::string>& result)
      { Impl::function_call_p(x, result, args); return 0; }
  virtual Stmt funcexit_x( const std::string& x) {return 0; }

 public:
  void output(std::ostream& out) { Impl::output(out); }
};
#endif
