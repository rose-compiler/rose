#include <sage3basic.h>

void
SgAggregateInitializer::post_construction_initialization()
   {
     if (get_initializers())
          get_initializers()->set_parent(this);
   }

// DQ: trying to remove the nested iterator class
void
SgAggregateInitializer::append_initializer(SgExpression* what)
   {
     assert(this != NULL);

  // DQ (11/15/2006): avoid setting newArgs this late in the process.
     ROSE_ASSERT(p_initializers != NULL);
     if(!p_initializers)
        {
       // set_initializers(new SgExprListExp(this->get_file_info()));
          SgExprListExp* newArgs = new SgExprListExp ( this->get_file_info() );
          assert(newArgs != NULL);
          newArgs->set_endOfConstruct( this->get_file_info() );
          set_initializers(newArgs);
        }

  // insert_initializer(p_initializers->end(),what);
     p_initializers->append_expression(what);
   }

SgExpression*
SgAggregateInitializer::get_next(int& n) const
   {
     if(n==0)
        {
          n++;
          return get_initializers();
        }
       else
          return 0;
   }

int
SgAggregateInitializer::replace_expression(SgExpression* o, SgExpression* n)
   {
  // DQ (12/17/2006): This function should have the semantics that it will represent a
  // structural change to the AST, thus it is free to set the parent of the new expression.

     ROSE_ASSERT(o != NULL);
     ROSE_ASSERT(n != NULL);

     if (get_initializers() == o)
        {
          set_initializers(isSgExprListExp(n));
          n->set_parent(this);
          return 1;
        }
       else
        {
          return 0;
        }
   }

SgType*
SgAggregateInitializer::get_type() const
   {
     if (p_expression_type != NULL)
        {
          return p_expression_type;
        }
       else
        {
          return SgTypeDefault::createType();
        }
   }

// DQ (6/11/2015): Moved these eight access functions, they should not be generated by ROSETTA
// so that we could avoid them setting the isModified flag which is a problem in the
// name qualification support for C++ (interfering with the token-based unparsing).
bool
SgAggregateInitializer::get_requiresGlobalNameQualificationOnType () const
   {
     ROSE_ASSERT (this != NULL);
     return p_requiresGlobalNameQualificationOnType;
   }

void
SgAggregateInitializer::set_requiresGlobalNameQualificationOnType ( bool requiresGlobalNameQualificationOnType )
   {
     ROSE_ASSERT (this != NULL);
  // This can't be called by the name qualification API (see test2015_26.C).
  // set_isModified(true);

     p_requiresGlobalNameQualificationOnType = requiresGlobalNameQualificationOnType;
   }

int
SgAggregateInitializer::get_name_qualification_length_for_type () const
   {
     ROSE_ASSERT (this != NULL);
     return p_name_qualification_length_for_type;
   }

void
SgAggregateInitializer::set_name_qualification_length_for_type ( int name_qualification_length_for_type )
   {
     ROSE_ASSERT (this != NULL);
  // This can't be called by the name qualification API (see test2015_26.C).
  // set_isModified(true);

     p_name_qualification_length_for_type = name_qualification_length_for_type;
   }

bool
SgAggregateInitializer::get_type_elaboration_required_for_type () const
   {
     ROSE_ASSERT (this != NULL);
     return p_type_elaboration_required_for_type;
   }

void
SgAggregateInitializer::set_type_elaboration_required_for_type ( bool type_elaboration_required_for_type )
   {
     ROSE_ASSERT (this != NULL);
  // This can't be called by the name qualification API (see test2015_26.C).
  // set_isModified(true);

     p_type_elaboration_required_for_type = type_elaboration_required_for_type;
   }

bool
SgAggregateInitializer::get_global_qualification_required_for_type () const
   {
     ROSE_ASSERT (this != NULL);
     return p_global_qualification_required_for_type;
   }

void
SgAggregateInitializer::set_global_qualification_required_for_type ( bool global_qualification_required_for_type )
   {
     ROSE_ASSERT (this != NULL);
  // This can't be called by the name qualification API (see test2015_26.C).
  // set_isModified(true);

     p_global_qualification_required_for_type = global_qualification_required_for_type;
   }
