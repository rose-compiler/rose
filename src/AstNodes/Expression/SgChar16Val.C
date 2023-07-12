#include <sage3basic.h>

void
SgChar16Val::post_construction_initialization()
   {
  // We can't initialize this to NULL since it might have just been set!
  // p_valueUL = 0L;
   }

// DQ (2/16/2018): ROSE needs to be able to be compiled on non C++11 systems where char_32_t is not defiled.
// char16_t
unsigned short
SgChar16Val::get_value() const
   {
     return p_valueUL;
   }

// void SgChar16Val::set_value(char16_t new_val)
void
SgChar16Val::set_value(unsigned short new_val)
   {
     p_valueUL = new_val;
   }
