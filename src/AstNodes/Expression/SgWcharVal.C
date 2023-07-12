#include <sage3basic.h>

void
SgWcharVal::post_construction_initialization()
   {
  // We can't initialize this to NULL since it might have just been set!
  // p_valueUL = 0L;
   }

wchar_t
SgWcharVal::get_value() const
   {
     return p_valueUL;
   }

void
SgWcharVal::set_value(wchar_t new_val)
   {
     p_valueUL = new_val;
   }
