#include <sage3basic.h>

void
SgDesignatedInitializer::post_construction_initialization()
   {
  // This should be a list of designators to handle the case of multi-deminsional array references.
     ROSE_ASSERT(p_designatorList != NULL);
     p_designatorList->set_parent(this);

     if (p_memberInit != NULL) // We want to support NULL p_memberInit when building this node
        {
          p_memberInit->set_parent(this);
        }
   }

SgType*
SgDesignatedInitializer::get_type() const
   {
     ROSE_ASSERT(p_memberInit != NULL);

#if 0
     printf ("In SgDesignatedInitializer::get_type() \n");
#endif

     return p_memberInit->get_type();
   }

int
SgDesignatedInitializer::replace_expression(SgExpression *o, SgExpression *n)
   {
  // DQ (12/17/2006): This function should have the semantics that it will represent a
  // structural change to the AST, thus it is free to set the parent of the new expression.

     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     if (get_memberInit() == o)
        {
          ROSE_ASSERT (isSgInitializer(n));
          set_memberInit(isSgInitializer(n));
          n->set_parent(this);

       // Return 1 to indicate sucess.
          return 1;
        }
       else if (get_designatorList() == o)
        {
          ROSE_ASSERT (isSgExprListExp(n));
          set_designatorList(isSgExprListExp(n));
       // set_designator(n);
          n->set_parent(this);

       // Return 1 to indicate sucess.
          return 1;
        }
       else
        {
       // Return 0 to indicate failure to match original expression ("o").
          return 0;
        }
   }

SgExpression*
SgDesignatedInitializer::get_next(int&) const
   {
  // DQ (7/21/2013): This function should not be called and should be removed at some point.

     printf ("This function should not be called and should be removed at some point \n");
     ROSE_ASSERT(false);

  // tps (12/9/2009) : MSC requires a return value
     return NULL;
   }
