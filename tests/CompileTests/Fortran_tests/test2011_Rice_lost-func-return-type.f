C     Declaring a local variable with the same name as that of an existing function
C     causes the function to be unparsed without its return type.
C     In the example below, the type of CKCHUP is lost in translation
      CHARACTER(len = *) FUNCTION CKCHUP (ILEN)
      END

      CHARACTER(len = *) FUNCTION UPCASE (ILEN)
      INTEGER ILEN
      CHARACTER CKCHUP*128
      EXTERNAL CKCHUP
      UPCASE = CKCHUP (ILEN)
      END
