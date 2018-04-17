! This was a failing case for a while (until it was fixed).
 TYPE DD
  INTEGER :: I
 END TYPE DD
 TYPE(DD) :: X(2)=(/(DD(I),I=1,2)/)
 END
