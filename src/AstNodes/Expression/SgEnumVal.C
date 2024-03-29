#include <sage3basic.h>

// DQ (2/5/2020): Addedin custom get_type() function so that we can pass the associated SgEnumDeclaration.
SgType* SgEnumVal::get_type() const
   {
#if 0
     printf ("In SgEnumVal::get_type() \n");
#endif

     SgEnumDeclaration* enumDeclaration = this->get_declaration();
     ROSE_ASSERT(enumDeclaration != NULL);

     return SgEnumType::createType(enumDeclaration);
   }

// DQ (6/11/2015): Moved these six access functions, they should not be generated by ROSETTA
// so that we could avoid them setting the isModified flag which is a problem in the
// name qualification support for C++ (interfering with the token-based unparsing).
int
SgEnumVal::get_name_qualification_length () const
   {
     ROSE_ASSERT (this != NULL);
     return p_name_qualification_length;
   }

void
SgEnumVal::set_name_qualification_length ( int name_qualification_length )
   {
     ROSE_ASSERT (this != NULL);
  // This can't be called by the name qualification API (see test2015_26.C).
  // set_isModified(true);

     p_name_qualification_length = name_qualification_length;
   }

bool
SgEnumVal::get_type_elaboration_required () const
   {
     ROSE_ASSERT (this != NULL);
     return p_type_elaboration_required;
   }

void
SgEnumVal::set_type_elaboration_required ( bool type_elaboration_required )
   {
     ROSE_ASSERT (this != NULL);
  // This can't be called by the name qualification API (see test2015_26.C).
  // set_isModified(true);

     p_type_elaboration_required = type_elaboration_required;
   }

bool
SgEnumVal::get_global_qualification_required () const
   {
     ROSE_ASSERT (this != NULL);
     return p_global_qualification_required;
   }

void
SgEnumVal::set_global_qualification_required ( bool global_qualification_required )
   {
     ROSE_ASSERT (this != NULL);

  // This can't be called by the name qualification API (see test2015_26.C).
  // set_isModified(true);

     p_global_qualification_required = global_qualification_required;
   }
