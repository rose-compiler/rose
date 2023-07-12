#include <sage3basic.h>

void
SgConditionalExp::post_construction_initialization()
   {
     if (p_conditional_exp != NULL)
          p_conditional_exp->set_parent(this);
     if (p_true_exp != NULL)
          p_true_exp->set_parent(this);
     if (p_false_exp != NULL)
          p_false_exp->set_parent(this);
   }

SgType*
SgConditionalExp::get_type() const
   {
  // In this function we want to return the type of the true of false option but it only makes sense for them to be the same type.

  // DQ (1/14/2006): p_expression_type has been removed, we have to compute the appropriate type (IR specific code)
  // printf ("SgConditionalExp::get_type(): p_expression_type has been removed, we have to compute the appropriate type \n");

  // Jeremiah (7/20/2006): Despite the comments, SOURCE_CONDITIONAL_EXPRESSION refers to a p_expression_type member,
  // but then ignores it.  I think you may want to store the type explicitly for this operator, as computing it is
  // difficult (requires handling user-defined conversions and such).  The return type can be either the true or false
  // types, and they can be different.

     ROSE_ASSERT(p_true_exp  != NULL);
     ROSE_ASSERT(p_false_exp != NULL);

     SgType* trueType   = p_true_exp->get_type();
     ROSE_ASSERT(trueType != NULL);

     SgType* falseType  = p_false_exp->get_type();
     ROSE_ASSERT(falseType != NULL);

#if PRINT_DEVELOPER_WARNINGS
     string expression_type_name = "NULL";
     if (p_expression_type != NULL)
          expression_type_name = p_expression_type->class_name();

  // ROSE_ASSERT(trueType == falseType);
     if (trueType != falseType)
        {
          printf ("Warning: In SgConditionalExp::get_type(): trueType = %s != falseType = %s  (p_expression_type = %s) \n",
               trueType->class_name().c_str(),falseType->class_name().c_str(),expression_type_name.c_str());
       // get_file_info()->display("Warning: In SgConditionalExp::get_type(): trueType != falseType");
        }
#endif

  // This was the previous choice for how p_expression_type was set,
  // so to be consistant we will select the same branch.
     SgType* returnType = trueType;

  // PC (10/12/2009): If returnType is in fact an SgArrayType it will undergo array-to-pointer conversion
     SgType *retElemType = SageInterface::getElementType(returnType);
     if (retElemType != NULL)
        {
          returnType = SgPointerType::createType(retElemType);
        }

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }

SgExpression*
SgConditionalExp::get_next(int& n) const
   {
     SgExpression *tmp = NULL;
     switch (n)
        {
          case 0: tmp = get_conditional_exp(); n++; break;
          case 1: tmp = get_true_exp(); n++; break;
          case 2: tmp = get_false_exp(); n++; break;
        }

     return tmp;
   }

int
SgConditionalExp::replace_expression(SgExpression *o, SgExpression *n)
   {
  // DQ (12/17/2006): This function should have the semantics that it will represent a
  // structural change to the AST, thus it is free to set the parent of the new expression.

     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     if (get_conditional_exp() == o)
        {
          set_conditional_exp(n);
          n->set_parent(this);
          return 1;
        }
       else
        {
          if (get_true_exp() == o)
             {
               set_true_exp(n);
               n->set_parent(this);
               return 1;
             }
            else
             {
               if (get_false_exp() == o)
                  {
                    set_false_exp(n);
                    n->set_parent(this);
                    return 1;
                  }
                 else
                  {
                    return 0;
                  }
             }
        }
   }
