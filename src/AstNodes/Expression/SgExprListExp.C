#include <sage3basic.h>

bool
SgExprListExp::empty() const
   {
     if (p_expressions.empty())
          return 1;
       else
          return 0;
   }

SgExpressionPtrList&
SgExprListExp::get_expressions()
   { return p_expressions; }

const SgExpressionPtrList&
SgExprListExp::get_expressions() const
   { return p_expressions; }

void
SgExprListExp::append_expression(SgExpression *what)
   { what->set_parent(this); p_expressions.push_back(what); }

void
SgExprListExp::prepend_expression(SgExpression *what)
   { what->set_parent(this); p_expressions.insert(p_expressions.begin(), what); }

int
SgExprListExp::replace_expression(SgExpression *o, SgExpression *n)
   {
  // DQ (10/5/2007): This is a new version of this function (from Jeremiah) which observes STL vector iterator semantics.

  // DQ (12/17/2006): This function should have the semantics that it will represent a
  // structural change to the AST, thus it is free to set the parent of the new expression.

     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     for (SgExpressionPtrList::iterator i=p_expressions.begin();
          i != p_expressions.end(); ++i)
        {
          if( (*i) == o)
             {
               *i = n;
               return 1;
             }
        }
     return 0;
   }
