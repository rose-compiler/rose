TYPE TB
  INTEGER :: c
END TYPE TB
TYPE TA
  TYPE(TB) :: b
END TYPE TA
TYPE(TA) :: lhs_a, a
TYPE(TB) :: lhs_b
INTEGER :: lhs_c
lhs_a = a
lhs_b = a%b
lhs_c = a%b%c
END PROGRAM
