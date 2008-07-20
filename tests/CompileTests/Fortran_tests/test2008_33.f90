! This is a fragement from POP, but it work fine outside of a module!

!  integer, parameter, public ::          &
   integer, parameter         ::          &
      char_len  = 100                    ,&
      log_kind  = kind(.true.)           ,&
      int_kind  = kind(1)                ,&
      i4        = selected_int_kind(6)   ,&
      i8        = selected_int_kind(13)  ,&
      r4        = selected_real_kind(6)  ,&
      r8        = selected_real_kind(13)

end
