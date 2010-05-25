% Test code from Craig (rose_bug_kind_param.f90) from email 04/15/2010 12:00 PM

module rose_define_kind

integer, parameter :: REAL8 = SELECTED_REAL_KIND(12)

real(REAL8), parameter :: ZERO      = 0.0_REAL8

end module

