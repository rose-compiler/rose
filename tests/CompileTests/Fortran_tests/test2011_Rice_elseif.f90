! Rice test
! if-then-else-endif is incorrectly translated
program fused_else_if
 if (.true.) then
   n = 1
 else 
   if (.false.) n = 0 ! incorrectly combined with the else in the translation
 endif
end program
