subroutine example
     real ar(10)
     pointer(ipt, arpte(10))
     real arpte
     ipt = loc(ar)  ! Makes arpte is an alias for ar
     arpte(1) = 1.0 ! Sets ar(1) to 1.0

end subroutine example
