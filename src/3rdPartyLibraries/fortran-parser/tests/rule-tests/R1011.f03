! Test control-edit-desc
!      control-edit-desc  is  position-edit-desc
!                         or  [r]/
!                         or  :
!                         or  sign-edit-desc
!                         or  kP
!                         or  blank-interp-edit-desc
!                         or  round-edit-desc
!                         or  decimal-edit-desc
!
!      k  is  signed-int-literal-constant
!
!      position-edit-desc  is  Tn
!                          or  TLn
!                          or  TRn
!                          or  nX
! 
!      n  is  int-literal-constant
!
!      sign-edit-desc  is  SS
!                      or  SP
!                      or  S
! 
!      blank-interp-edit-desc  is  BN
!                              or  BZ
!
!      round-edit-desc  is  RU
!                       or  RD
!                       or  RZ
!                       or  RN
!                       or  RC
!                       or  RP
!
!      decimal-edit-desc  is  DC
!                         or  DP
!
! Tested as part of a format-stmt.
001 format(T1)
002 format(TL1)
003 format(TR1)
004 format(1X)
005 format(/)
! This next one doesn't actually seem to be accepted correctly because it 
! separates the 1 and the / though they should be considered together.
006 format(1/)
! This next one seems to cause an extra control-edit-desc to be generated that
! is an empty-string.
007 format(:)
008 format(SS)
009 format(SP)
! This next one isn't correctly accepted.
010 format(S)
011 format(1P)
012 format(BN)
013 format(BZ)
014 format(RU)
015 format(RD)
016 format(RZ)
017 format(RN)
018 format(RC)
019 format(RP)
020 format(DC)
021 format(DP)

end

