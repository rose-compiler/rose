#include <sage3basic.h>

void
SgBinaryOp::post_construction_initialization()
   {
     if (get_lhs_operand())
          get_lhs_operand()->set_parent(this);
     if (get_rhs_operand())
          get_rhs_operand()->set_parent(this);
   }

SgExpression*
SgBinaryOp::get_lhs_operand() const
   { return get_lhs_operand_i(); }

void
SgBinaryOp::set_lhs_operand(SgExpression * exp)
   {
     set_lhs_operand_i(exp);
     if (exp)
        exp->set_parent(this);
   }

int
SgBinaryOp::length() const
   {
     return 2;
   }

// I don't think this is used (so exclude it until we clearly need it)!
bool
SgBinaryOp::empty() const
   {
     return false;  // return 0;
   }

SgExpression*
SgBinaryOp::get_rhs_operand() const
   { return get_rhs_operand_i(); }

void
SgBinaryOp::set_rhs_operand(SgExpression * exp)
   {
     set_rhs_operand_i(exp);
     if (exp)
          exp->set_parent(this);
   }

SgType*
SgBinaryOp::get_type() const
   {
  // We have to select one, so for now chose the lhs, it might be that
  // we have to explicitly store the return type of binary operators.

     ROSE_ASSERT(get_lhs_operand() != NULL);
     ROSE_ASSERT(get_rhs_operand() != NULL);

     SgType* lhsType = get_lhs_operand()->get_type();
     ROSE_ASSERT(lhsType != NULL);

     SgType* rhsType = get_rhs_operand()->get_type();
     ROSE_ASSERT(rhsType != NULL);

  // DQ (9/27/2006): Ideally, this should not be set, though sometimes it is.
  // ROSE_ASSERT(p_expression_type != NULL);
     std::string expression_type_name = "NULL";
     if (p_expression_type != NULL)
        {
          expression_type_name = p_expression_type->class_name();
          printf ("Found an case where SgBinaryOp::p_expression_type != NULL on %s expression_type_name = %s \n",
               this->class_name().c_str(),expression_type_name.c_str());
        }

     if (variantT() == V_SgAddOp || variantT() == V_SgSubtractOp)
        {
       // PC (10/9/2009): Pointer arithmetic is a special case
          SgType *lhsBase = SageInterface::getElementType(lhsType),
                 *rhsBase = SageInterface::getElementType(rhsType);

       /* Note that we return an SgPointerType regardless of whether the original
        * type was an SgPointerType or SgArrayType.  This is because the latter
        * undergoes implicit array-to-pointer conversion. */
          if (lhsBase != NULL && rhsBase == NULL)
             {
               return SgPointerType::createType(lhsBase);
             }
          else if (lhsBase == NULL && rhsBase != NULL)
             {
               return SgPointerType::createType(rhsBase);
             }
          else if (lhsBase != NULL && rhsBase != NULL && variantT() == V_SgSubtractOp)
             {
            // PL (1/27/2025): We return a long as the ptrdiff_t type because ROSE configures
            // EDG to emit a long when encountering a ptrdiff_t.
               return SgTypeLong::createType();
             }
        }

     return lhsType;
   }

SgExpression*
SgBinaryOp::get_next(int& n) const
   {
     if(n==0)
        {
          n++;
          return get_lhs_operand();
        }
       else
        {
          if (n==1)
             {
               n++;
               return get_rhs_operand();
             }
        }

     return 0;
   }

int
SgBinaryOp::replace_expression(SgExpression *o, SgExpression *n)
   {
  // DQ (12/17/2006): This function should have the semantics that it will represent a
  // structural change to the AST, thus it is free to set the parent of the new expression.

     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     if (get_lhs_operand() == o)
        {
          set_lhs_operand(n);
          return 1;
        }
       else
          if (get_rhs_operand() == o)
             {
               set_rhs_operand(n);
               return 1;
             }
            else
             {
               printf ("Warning: inside of SgBinaryOp::replace_expression original SgExpression unidentified \n");
               return 0;
             }
   }
