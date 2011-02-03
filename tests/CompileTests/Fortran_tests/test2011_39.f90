! Contributed by Jean Utke to the bug tracker (1/31/2011)
! This code complains about tabs and is an OPF issue (for each line).
!      Warning: Nonconforming tab character at (1)
! Since the origial code was not submitted it had to be reconstructed 
! (likely without tabs).  So this code likely will not demonstrate 
! the problem.

program charArrSlice
 character(10), dimension(2) ::  sa
 sa(2)="blabla"
 if (sa(2)(2:4) .eq. "lab") then
   print *,'OK'
 endif
end
 
