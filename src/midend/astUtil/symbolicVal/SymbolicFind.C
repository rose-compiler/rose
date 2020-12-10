#include "SymbolicVal.h"
#include "SymbolicExpr.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

class ValFindBase : public SymbolicVisitor
{
  SymbolicVal target, cur;
  bool result;

  virtual void Default() { result = (target == cur); }


  void VisitFunction( const SymbolicFunction &v)
       {
          if (target.GetValType() == VAL_FUNCTION && cur == target)
              result = true;
          else {
            for (SymbolicFunction::const_iterator p = v.args_begin();
                 p != v.args_end(); ++p) {
              SymbolicVal tmp = *p;
              cur = tmp;
              cur.Visit(this);
              if ( result)
                 break;
            }
          }
       }
  void VisitExpr( const SymbolicExpr &v)
       {
         if (target.GetValType() == VAL_EXPR && cur == target)
              result = true;
         else {
           for (SymbolicExpr::OpdIterator iter = v.GetOpdIterator();
                !iter.ReachEnd(); iter.Advance()) {
              SymbolicVal tmp = v.Term2Val(iter.Current());
              cur = tmp;
              cur.Visit(this);
              if (result)
                 break;
           }
         }
       }
 public:
  ValFindBase() : result(false) {}

  bool operator ()( const SymbolicVal &v, const SymbolicVal& _target)
    {
      target = _target;
      cur = v;
      result = false;
      v.Visit(this);
      return result;
    }
};

bool FindVal( const SymbolicVal &v, const SymbolicVal &var)
{
   ValFindBase op;
   return op(v, var);
}

