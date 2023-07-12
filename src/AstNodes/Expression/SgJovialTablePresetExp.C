#include <sage3basic.h>

void
SgJovialTablePresetExp::post_construction_initialization()
{
  ROSE_ASSERT(p_preset_list);
  p_preset_list->set_parent(this);
}

SgType*
SgJovialTablePresetExp::get_type() const
{
  SgAssignInitializer* parent = isSgAssignInitializer(get_parent());
  ROSE_ASSERT(parent);

  return parent->get_type();
}
