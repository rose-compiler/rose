   implicit none

 ! Correct in the AST, but unparses as: "character(len=5) :: C,D" (not clear how to make this a compiler error).
   character :: C*5,D*9
   C = D
end
