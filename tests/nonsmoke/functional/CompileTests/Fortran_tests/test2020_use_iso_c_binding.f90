program main
   use iso_c_binding

   integer(c_int8_t)    :: i8t
   integer(c_int16_t)   :: i16t
   integer(c_int32_t)   :: i32t
   integer(c_int64_t)   :: i64t

   integer(c_int)       :: ii
   integer(c_short)     :: is
   integer(c_long)      :: il
   integer(c_long_long) :: ill

   integer(c_size_t)    :: isizet
   integer(c_intmax_t)  :: imax
   integer(c_intptr_t)  :: iptrt
   integer(c_ptrdiff_t) :: iptrdifft

   real(c_float)        :: rf
   real(c_double)       :: rd
   real(c_long_double)  :: rld

end program
