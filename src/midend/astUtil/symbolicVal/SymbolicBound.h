#ifndef SYMBOLIC_BOUND_H
#define SYMBOLIC_BOUND_H

#include "SymbolicVal.h"
#include <map>

class SingleValBound : public MapObject<SymbolicVal, SymbolicBound>
{
  SymbolicVal val;
  SymbolicBound bound;
 public:
  SingleValBound( const SymbolicVal& v, const SymbolicVal& lb, const SymbolicVal& ub)
      : val(v), bound(lb, ub) {}
  SymbolicBound operator() ( const SymbolicVal& v)
   {
      return (v == val)? bound : SymbolicBound(); 
   }
};
class MapVarBound 
 : public MapObject<SymbolicVal, SymbolicBound>,
  public SymbolicVisitor
{
  SymbolicBound result;
  typedef std::pair<std::string,AstNodePtr> Key;
  typedef std::map< Key, SymbolicBound, std::less<Key> > MapBound;
  MapBound bmap;
  
  void VisitVar( const SymbolicVar& var)
  {
    MapBound::const_iterator p = bmap.find(Key(var.GetVarName(), var.GetVarScope()));
    if (p != bmap.end())
       result = (*p).second;
  }
 public:
  void add( const SymbolicVar& var, const SymbolicBound& bound)
  { bmap[Key(var.GetVarName(), var.GetVarScope())] = bound; }
  SymbolicBound operator()(const SymbolicVal& v)
  { 
    result = SymbolicBound();
    v.Visit(this);
    return result;
  }
};

class VarInfo : public MapObject<SymbolicVal, SymbolicBound>
{
  SymbolicVar var;
  SymbolicBound b;
 public:
  VarInfo() {}
  VarInfo( const SymbolicVar& _var, const SymbolicVal& _lb, 
           const SymbolicVal& _ub) : var(_var),b(_lb,_ub) {}
  VarInfo( const SymbolicVar& _var, const SymbolicBound& _b)
           : var(_var),b(_b) {}
  std::string toString() const {
      return  var.toString() + " : " + b.toString();
  }
  SymbolicBound operator() ( const SymbolicVal& v)
   { return (v == var)? b : SymbolicBound(); }

  const SymbolicVar& GetVar() const { return var; }
  SymbolicVar& GetVar() { return var; }
  const SymbolicBound& GetBound() const { return b; }
  SymbolicBound& GetBound() { return b; }
  bool IsTop() const { return var.GetVarName() == ""; }

  SymbolicBound GetVarRestr( const SymbolicVar v);

};

template <class Stmt, class Interface>
class SymbolicBoundAnalysis 
  : public MapObject<SymbolicVal, SymbolicBound>, private SymbolicVisitor
{
 protected:
  Interface interface;
  SymbolicBound result;
  Stmt node, ances;
  void VisitVar( const SymbolicVar& var)
   { result = GetBound(var); }
 public:
  SymbolicBoundAnalysis(Interface _interface, Stmt n, Stmt a = 0)
      : interface(_interface), node(n), ances(a) {}
  SymbolicBound GetBound(const SymbolicVar& var, Stmt* stop = 0)
       {
         SymbolicBound tmp;
         Stmt n = node; 
         for ( ; n != ances; n = interface.GetParent(n)) {
             VarInfo info = interface.GetVarInfo(n);
             if (info.IsTop())
                continue;
             if (info.GetVar() == var) {
                if (tmp.lb.IsNIL())
                     tmp.lb = info.GetBound().lb;
                if (tmp.ub.IsNIL())
                     tmp.ub = info.GetBound().ub;
             }
             if (!tmp.lb.IsNIL() && !tmp.ub.IsNIL())
                  break;
         }
         if (stop != 0)
            *stop = n;
         return tmp;
       }
  SymbolicBound operator()( const SymbolicVal &v)
   { result = SymbolicBound();
     v.Visit(this);
     return result;
   }
};

template <class Stmt, class Interface>
class SymbolicConstBoundAnalysis : public SymbolicBoundAnalysis<Stmt,Interface> 
{
 protected:
  SymbolicBoundAnalysis<Stmt,Interface>::result;
  SymbolicBoundAnalysis<Stmt,Interface>::ances;
  SymbolicBoundAnalysis<Stmt,Interface>::interface;
  SymbolicBoundAnalysis<Stmt,Interface>::node;
 private:
  void VisitVar( const SymbolicVar& var)
   { result = GetConstBound(var); }
 public:
  SymbolicConstBoundAnalysis(Interface _interface, Stmt n, Stmt a )
      : SymbolicBoundAnalysis<Stmt,Interface>(_interface,n,a) {}
  SymbolicBound GetConstBound(const SymbolicVar& var)
       {
         Stmt n;
         SymbolicBound tmp = SymbolicBoundAnalysis<Stmt,Interface>::GetBound(var, &n);
         if (n != ances) {
            SymbolicBoundAnalysis<Stmt,Interface> next(interface,interface.GetParent(n),ances); 
            if (tmp.lb.IsNIL())
                tmp.lb = var;
            if (tmp.ub.IsNIL())
                tmp.ub = var;
            tmp.lb = GetValBound(tmp.lb,next).lb;
            tmp.ub = GetValBound(tmp.ub,next).ub;
         }
         else if (tmp.lb.IsNIL() && tmp.ub.IsNIL()) {
            for (n = node ; n != ances; n = interface.GetParent(n)) {
               VarInfo info = interface.GetVarInfo(n);
               if (info.IsTop())
                  continue;
               SymbolicCond cond(REL_LE, info.GetBound().lb, info.GetBound().ub);
               SymbolicVal r = UnwrapVarCond(cond,var,tmp);
               if (r != 1) {
                  tmp = SymbolicBound();
                  continue;
               }
               else {
                  if (tmp.lb.GetValType() != VAL_CONST)
                      tmp.lb = SymbolicVal();
                  if (tmp.ub.GetValType() != VAL_CONST)
                      tmp.ub = SymbolicVal();
                  break;
               }
             }
         }
         return tmp;
       }
};
#endif
