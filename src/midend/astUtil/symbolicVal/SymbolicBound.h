#ifndef SYMBOLIC_BOUND_H
#define SYMBOLIC_BOUND_H

#include "SymbolicVal.h"
#include <map>
//! A single variable's bound
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


// DQ (11/27/2009): When using MSVC, this should be using "typename" instead of "class"
// Note that this might be the better fix and should work for Linux as well (g++).
// Another likely related problem is the "interface" is a keyword in MSVC.
// So I have changed to name to avoid requiring that we turn off MS language extensions.
template <class Stmt, typename InterfaceArg>
class SymbolicBoundAnalysis 
  : public MapObject<SymbolicVal, SymbolicBound>, private SymbolicVisitor
{
 protected:
  InterfaceArg interfaceClass;
  SymbolicBound result;
  Stmt node, ances;
  void VisitVar( const SymbolicVar& var)
   { result = GetBound(var); }
 public:
  SymbolicBoundAnalysis(InterfaceArg _interface, Stmt n, Stmt a = 0)
      : interfaceClass(_interface), node(n), ances(a) {}
  SymbolicBound GetBound(const SymbolicVar& var, Stmt* stop = 0)
       {
         SymbolicBound tmp;
         Stmt n = node; 
         for ( ; n != ances; n = interfaceClass.GetParent(n)) {
             VarInfo info = interfaceClass.GetVarInfo(n);
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

template <class Stmt, class InterfaceArg>
class SymbolicConstBoundAnalysis : public SymbolicBoundAnalysis<Stmt,InterfaceArg> 
{
 protected:
  SymbolicBoundAnalysis<Stmt,InterfaceArg>::result;
  SymbolicBoundAnalysis<Stmt,InterfaceArg>::ances;
  SymbolicBoundAnalysis<Stmt,InterfaceArg>::interfaceClass;
  SymbolicBoundAnalysis<Stmt,InterfaceArg>::node;
 private:
  void VisitVar( const SymbolicVar& var)
   { result = GetConstBound(var); }
 public:
  SymbolicConstBoundAnalysis(InterfaceArg _interface, Stmt n, Stmt a )
      : SymbolicBoundAnalysis<Stmt,InterfaceArg>(_interface,n,a) {}
  SymbolicBound GetConstBound(const SymbolicVar& var)
       {
         Stmt n;
         SymbolicBound tmp = SymbolicBoundAnalysis<Stmt,InterfaceArg>::GetBound(var, &n);
         if (n != ances) {
            SymbolicBoundAnalysis<Stmt,InterfaceArg> next(interfaceClass,interfaceClass.GetParent(n),ances); 
            if (tmp.lb.IsNIL())
                tmp.lb = var;
            if (tmp.ub.IsNIL())
                tmp.ub = var;
            tmp.lb = GetValBound(tmp.lb,next).lb;
            tmp.ub = GetValBound(tmp.ub,next).ub;
         }
         else if (tmp.lb.IsNIL() && tmp.ub.IsNIL()) {
            for (n = node ; n != ances; n = interfaceClass.GetParent(n)) {
               VarInfo info = interfaceClass.GetVarInfo(n);
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
