! Test data-edit-desc
!      data-edit-desc  is  Iw[.m]
!                      or  Bw[.m]
!                      or  Ow[.m]
!                      or  Zw[.m]
!                      or  Fw.d
!                      or  Ew.d[Ee]
!                      or  ENw.d[Ee]
!                      or  ESw.d[Ee]
!                      or  Gw.d[Ee]
!                      or  Lw
!                      or  A[w]
!                      or  Dw.d
!                      or  DT[char-literal-constant][(v-list)]
!
!      w  is  int-literal-constant
!      m  is  int-literal-constant
!      d  is  int-literal-constant
!      e  is  int-literal-constant
!      v  is  signed-int-literal-constant
!
! Tested as part of a format-stmt.
001 format(i1)
002 format(i1.2)
003 format(B1)
003 format(B1.2)
004 format(O1)
005 format(O1.2)
006 format(Z1)
007 format(Z1.2)
008 format(F1.2)
009 format(E1.2)
009 format(E1.2E2)
010 format(EN1.2)
011 format(EN1.2E3)
012 format(ES1.2)
013 format(ES1.2E3)
014 format(G1.2)
015 format(G1.2E3)
016 format(L1)
017 format(A)
018 format(A1)
019 format(D1.2)
020 format(DT'hello')
021 format(DT'hello'(10))

end

