#include <sage3basic.h>

#if OBSOLETE_CODE

SgType* SgAdaTaskRefExp::get_type() const
{
  return SgAdaTaskType::createType(get_decl(), NULL);
}

#endif /* OBSOLETE_CODE */
