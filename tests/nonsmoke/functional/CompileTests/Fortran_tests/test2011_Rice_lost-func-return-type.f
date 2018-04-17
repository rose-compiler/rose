C     Declaring a local variable with the same name as that of an existing function
C     causes the function to be unparsed without its return type.
C     In the example below, the type of foo is lost in translation
      CHARACTER(len = *) FUNCTION foo (i)
      END

      CHARACTER(len = *) FUNCTION bar (i)
      INTEGER i
      CHARACTER foo*128
      EXTERNAL foo
      bar = foo (i)
      END
