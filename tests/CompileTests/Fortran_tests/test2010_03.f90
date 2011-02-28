module precision_03
! use mpif.h
  include 'rose_mpif_error.h' ! include empty file...
! DQ (4/7/2010): "Parser failed" error for OFP (version 0.7.2) but requires empty header file
  integer, parameter :: doubleprec=selected_real_kind(12),singleprec=selected_real_kind(6),defaultprec=kind(0.0)
end module precision_03
