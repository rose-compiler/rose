! This is a fragement from POP, which causes a bug in ROSE!

 module test2008_34_kinds_mod

   implicit none
   private
   save

   integer, parameter, public ::          &
      char_len  = 100                    ,&
      log_kind  = kind(.true.)           ,&
      int_kind  = kind(1)                ,&
      i4        = selected_int_kind(6)   ,&
      i8        = selected_int_kind(13)  ,&
      r4        = selected_real_kind(6)  ,&
      r8        = selected_real_kind(13)

 end module test2008_34_kinds_mod
