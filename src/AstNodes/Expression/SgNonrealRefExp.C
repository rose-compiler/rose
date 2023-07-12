#include <sage3basic.h>

int
SgNonrealRefExp::get_name_qualification_length () const
   {
     ROSE_ASSERT (this != NULL);
     return p_name_qualification_length;
   }

void
SgNonrealRefExp::set_name_qualification_length ( int name_qualification_length )
   {
     ROSE_ASSERT (this != NULL);
     p_name_qualification_length = name_qualification_length;
   }

bool
SgNonrealRefExp::get_type_elaboration_required () const
   {
     ROSE_ASSERT (this != NULL);
     return p_type_elaboration_required;
   }

void
SgNonrealRefExp::set_type_elaboration_required ( bool type_elaboration_required )
   {
     ROSE_ASSERT (this != NULL);
     p_type_elaboration_required = type_elaboration_required;
   }

bool
SgNonrealRefExp::get_global_qualification_required () const
   {
     ROSE_ASSERT (this != NULL);
     return p_global_qualification_required;
   }

void
SgNonrealRefExp::set_global_qualification_required ( bool global_qualification_required )
   {
     ROSE_ASSERT (this != NULL);

     p_global_qualification_required = global_qualification_required;
   }
