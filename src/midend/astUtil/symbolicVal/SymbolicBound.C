
#include "SymbolicExpr.h"
#include "SymbolicSelect.h"
#include "SymbolicBound.h"
#include "CommandOptions.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

bool DebugReplaceVal()
{
  static int r = 0;
  if ( r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-debugreplaceval"))
        r = 1;
     else
        r = -1;
  }
  return r == 1;
}

bool DebugValBound()
{
  static int r = 0;
  if ( r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-debugvalbound"))
        r = 1;
     else
        r = -1;
  }
  return r == 1;
}

SymbolicBound VarInfo:: GetVarRestr( const SymbolicVar v)
{
    SymbolicBound r, r1, r2;
    if (UnwrapVarCond( SymbolicCond( REL_LE, b.lb, var), v,r1) == 1)
        r = r1;
    if (UnwrapVarCond( SymbolicCond( REL_LE, var, b.ub), v, r2) == 1)
        r.Intersect( r2);
    return r;
}

class ValGetBound : public SymbolicVisitor
{
 protected:
  SymbolicBound result;
  MapObject<SymbolicVal,SymbolicBound>& GetVarBound;

  bool Default0( const SymbolicVal &v)
     {
        SymbolicBound tmp = GetVarBound(v);
        bool succ = false;
        if (!tmp.lb.IsNIL()) {
          result.lb = tmp.lb;
          succ = true;
        }
        if (!tmp.ub.IsNIL()) {
          result.ub = tmp.ub;
          succ = true;
        }
        return succ;
     }

  void VisitFunction( const SymbolicFunction &v)
     {
       SymbolicFunction::Arguments args;
       SymbolicBound orig = result;
       bool change = false;

       for (SymbolicFunction::const_iterator p = v.args_begin();
            p != v.args_end(); ++p ){
          SymbolicVal cur = *p;
          SymbolicVal n = GetRepl(cur);
          if (cur != n) {
             change = true;
          }
          args.push_back( n);
       }
       if (change)
          result.lb = result.ub = v.cloneFunction(args);
       else
         result = orig;
     }
  void VisitExpr( const SymbolicExpr &exp)
       {
         SymbolicBound orig = result;
         bool change = false, isrepl = true, reverse = false;
         SymOpType op = exp.GetOpType();
         std::vector<SymbolicBound> args;
         for (SymbolicExpr::OpdIterator iter = exp.GetOpdIterator();
               !iter.ReachEnd(); iter.Advance()) {
            SymbolicVal cur = exp.Term2Val(iter.Current());
            SymbolicBound curbound = GetBound(cur);
            if (! curbound.lb.IsSame(cur) || !curbound.ub.IsSame(cur))
               change = true;
            if (!curbound.lb.IsSame( curbound.ub))
               isrepl = false;
            if (op == SYMOP_MULTIPLY && curbound.ub < 0)
                reverse = !reverse;
            args.push_back(curbound);
         }
         if (change) {
            std::vector<SymbolicBound>::const_iterator p = args.begin();
            result = *p;
            for ( ++p ; p != args.end(); ++p) {
               SymbolicBound cur = *p;
               result.ub = ::ApplyBinOP(op, result.ub, cur.ub);
               if (isrepl)
                  result.lb = result.ub;
               else
                  result.lb = ::ApplyBinOP(exp.GetOpType(), result.lb, cur.lb);
            }
            if (!isrepl && reverse)  {
                SymbolicVal tmp = result.lb;
                result.lb = result.ub;
                result.ub = tmp;
            }
         }
         else
            result = orig;
       }
 public:
  ValGetBound( MapObject<SymbolicVal,SymbolicBound>& _f) : GetVarBound(_f) {}
  void Visit( const SymbolicVal &val)
     {
        result.lb = result.ub = val;
        if (!Default0(val))
            val.Visit(this);
      }
  SymbolicBound GetBound( const SymbolicVal &val)
      {
        Visit(val);
        return result;
      }
  SymbolicVal GetRepl( const SymbolicVal &val)
      {
        Visit(val);
        if (result.lb.IsSame(result.ub))
           return result.lb;
        return val;
      }
};

SymbolicVal Max(const SymbolicVal &v1, const SymbolicVal &v2,
                        MapObject<SymbolicVal, SymbolicBound>& f)
{ if (v1.IsNIL())
      return v2;
  if (v2.IsNIL())
      return v1;
  SelectApplicatorWithBound op(f, 1);
  return ::ApplyBinOP(op,v1,v2); }

SymbolicVal Min(const SymbolicVal &v1, const SymbolicVal &v2,
                       MapObject<SymbolicVal, SymbolicBound>& f)
{ if (v1.IsNIL())
      return v2;
  if (v2.IsNIL())
      return v1;
  SelectApplicatorWithBound op(f, -1);
  return ::ApplyBinOP(op,v1,v2);
}

SymbolicVal ReplaceVal( const SymbolicVal &v, const SymbolicVal &var,
                        const SymbolicVal& newval)
{
  SingleValBound op1(var, newval, newval);

  ValGetBound op(op1);
  SymbolicVal r = op.GetRepl(v);
  return r;
}

class VarReplWrap : public MapObject<SymbolicVal, SymbolicBound>
{
  MapObject<SymbolicVal, SymbolicVal>& varmap;
 public:
  VarReplWrap(  MapObject<SymbolicVal, SymbolicVal>& _varmap ) : varmap(_varmap) {}
  SymbolicBound operator () (const SymbolicVal &v)
   {
      SymbolicVal r = varmap(v);
       return SymbolicBound(r,r);
   }
};

SymbolicVal ReplaceVal( const SymbolicVal &v, MapObject<SymbolicVal, SymbolicVal>& varmap)
{
  VarReplWrap op1( varmap);
  ValGetBound op(op1);
  SymbolicVal r = op.GetRepl(v);
  if (DebugReplaceVal()) {
     std::cerr << "replacing " << v.toString() << "->" << r.toString() << std::endl;
  }
  return r;
}

SymbolicVal GetValUB( const SymbolicVal& val, MapObject<SymbolicVal, SymbolicBound>& f)
{ return ValGetBound(f).GetBound(val).ub; }

 SymbolicVal GetValLB( const SymbolicVal& val,  MapObject<SymbolicVal, SymbolicBound>& f)
{ return ValGetBound(f).GetBound(val).lb; }

SymbolicBound
GetValBound(SymbolicVal val, MapObject<SymbolicVal, SymbolicBound>& f)
{
   SymbolicBound b = ValGetBound(f).GetBound(val);
   if (DebugValBound()) {
     std::cerr << " bound of ";
     val.Dump();
     std::cerr << " : ";
     b.lb.Dump();
     std::cerr << " -> ";
     b.ub.Dump();
     std::cerr << std::endl;
   }
   return b;
}

