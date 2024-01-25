#include <sage3basic.h>

#if OBSOLETE_CODE

SgType* SgAdaProtectedRefExp::get_type() const
{
  return SgAdaProtectedType::createType(get_decl(), NULL);
}

#endif /* OBSOLETE_CODE */
