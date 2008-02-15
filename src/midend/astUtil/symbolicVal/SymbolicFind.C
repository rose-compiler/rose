#include <general.h>

#include <SymbolicVal.h>
#include <SymbolicExpr.h>

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

class ValFindBase : public SymbolicVisitor
{
  SymbolicVal target, cur;
  Boolean result;

  virtual void Default() { result = (target == cur); }


  void VisitFunction( const SymbolicFunction &v) 
       { 
          if (target.GetValType() == VAL_FUNCTION && cur == target)
              result = true;
          else if (operator()(v.GetOp(), target) )
              result = true;
          else {
            for (unsigned i = 0; i < v.NumOfArgs(); ++i) {
              SymbolicVal tmp = v.GetArg(i);
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
    
  Boolean operator ()( const SymbolicVal &v, const SymbolicVal& _target)
    { 
      target = _target;
      cur = v;
      result = false; 
      v.Visit(this); 
      return result; 
    }
};

Boolean FindVal( const SymbolicVal &v, const SymbolicVal &var)
{ 
   ValFindBase op;
   return op(v, var);
}

