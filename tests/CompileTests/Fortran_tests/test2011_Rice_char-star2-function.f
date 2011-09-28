! This fails in OFP 0.8.3 with the following message:
!     no viable alternative at input 'CKCHUP'
! 
! The alternative syntax 'CHARACTER(LEN=*)' parses correctly.

      CHARACTER*(*) FUNCTION CKCHUP (ISTR, ILEN)
      END
