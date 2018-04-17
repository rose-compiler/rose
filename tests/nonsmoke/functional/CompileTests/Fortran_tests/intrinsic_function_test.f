      program main

c*********************************************************************72
c
cc MAIN is the main program for F77_INTRINSICS_PRB.
c
c  Discussion:
c
c    F77_INTRINSICS_PRB tests FORTRAN77 intrinsic routines.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      call timestamp ( )

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'F77_INTRINSICS_PRB'
      write ( *, '(a)' ) '  FORTRAN77 version'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Test FORTRAN77 intrinsic routines.'
 
      call test_abs
      call test_acos
      call test_aimag
      call test_aint
      call test_anint
      call test_asin
      call test_atan
      call test_atan2
      call test_btest
      call test_char
      call test_cmplx
      call test_conjg
      call test_cos_c4
      call test_cos_r8
      call test_cosh
      call test_dble
      call test_dim
      call test_dprod
      call test_exp
      call test_iand_i4
      call test_ibclr
      call test_ibits
      call test_ibset
      call test_ichar
      call test_ieor_i4
      call test_index
      call test_int
      call test_ior_i4
      call test_ishft
      call test_ishftc
      call test_len
      call test_len_trim
      call test_lge
      call test_lgt
      call test_lle
      call test_llt
      call test_log
      call test_log10
      call test_max
      call test_min
      call test_mod_i4
      call test_mod_r4
      call test_mvbits
      call test_nint
      call test_not_i4
      call test_real_c4
      call test_sign
      call test_sin_r8
      call test_sinh
      call test_sqrt
      call test_tan
      call test_tanh

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'F77_INTRINSICS_PRB'
      write ( *, '(a)' ) '  Normal end of execution.'

      write ( *, '(a)' ) ' '
      call timestamp ( )

      stop
      end
      subroutine test_abs

c*********************************************************************72
c
cc TEST_ABS tests ABS.
c
c  Discussion:
c
c    The FORTRAN77 function ABS returns the absolute value of a 
c    number.  For complex numbers, this is the magnitude.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      complex c4
      integer i4
      real r4
      double precision r8

      c4 = ( 1.0E+00, -2.0E+00 )
      i4 = -88
      r4 = 45.78E+00
      r8 = 45.78D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_ABS'
      write ( *, '(a)' ) '  ABS is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the absolute value of a numeric quantity'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' )    
     & '      Type                  VALUE                ABS(VALUE)'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i12,12x,i12)' )    
     &     '  integer              ', i4, abs ( i4 )
      write ( *, '(a,f12.4,12x,f12.4)' )    
     & '  Real                 ', r4, abs ( r4 )
      write ( *, '(a,f12.4,12x,f12.4)' )    
     & '  Double precision     ', r8, abs ( r8 )
      write ( *, '(a,2f12.4,f12.4)' )    
     &  '  Complex              ', c4, abs ( c4 )

      return
      end
      subroutine test_acos

c*********************************************************************72
c
cc TEST_ACOS tests ACOS.
c
c  Discussion:
c
c    The FORTRAN77 function ACOS returns the inverse cosine of a number X.
c    assuming -1 <= X <= 1.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      double precision r8_uniform
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y
      double precision z

      seed = 123456789
      x_hi = 1.0D+00
      x_lo = -1.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_ACOS'
      write ( *, '(a)' ) '  ACOS is a FORTRAN77 function which returns'
      write ( *, '(a)' )     
     &  '  the inverse cosine of a value between -1 and 1'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '      X               ACOS(X)     COS(ACOS(X))'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = acos ( x )
        z = cos ( y )
        write ( *, '(2x,g14.6,2x,g14.6,2x,g14.6)' ) x, y, z
      end do

      return
      end
      subroutine test_aimag

c*********************************************************************72
c
cc TEST_AIMAG tests AIMAG.
c
c  Discussion:
c
c    The FORTRAN77 function AIMAG returns the imaginary part of a
c    complex number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      complex c4_uniform_01
      complex c
      integer i
      integer seed
      real r

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_AIMAG'
      write ( *, '(a)' ) 
     &  '  AIMAG is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) '  imaginary part of a complex number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '                  X                      AIMAG(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        c = c4_uniform_01 ( seed )
        r = aimag ( c )
        write ( *, '(2x,f14.6,f14.6,6x,f14.6,f14.6)' ) c, r
      end do

      return
      end
      subroutine test_aint

c*********************************************************************72
c
cc TEST_AINT tests AINT.
c
c  Discussion:
c
c    The FORTRAN77 function AINT returns a real number rounded towards
c    zero.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      double precision r8_uniform
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_AINT'
      write ( *, '(a)' ) 
     &  '  AINT is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) 
     &  '  value of a real number rounded towards zero.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X              AINT(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = aint ( x )
        write ( *, '(2x,g14.6,2x,g14.6)' ) x, y
      end do

      return
      end
      subroutine test_anint

c*********************************************************************72
c
cc TEST_ANINT tests ANINT.
c
c  Discussion:
c
c    The FORTRAN77 function ANINT returns, as a real value, the nearest 
c    integer to a given real value.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      double precision r8_uniform
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_ANINT'
      write ( *, '(a)' ) 
     &  '  ANINT is a FORTRAN77 function which returns,'
      write ( *, '(a)' ) '  as a real value, the nearest integer to a '
      write ( *, '(a)' ) '  given real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X             ANINT(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = anint ( x )
        write ( *, '(2x,g14.6,2x,g14.6)' ) x, y
      end do

      return
      end
      subroutine test_asin

c*********************************************************************72
c
cc TEST_ASIN tests ASIN.
c
c  Discussion:
c
c    The FORTRAN77 function ASIN returns the inverse sine of a number X.
c    assuming -1 <= X <= 1.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      double precision r8_uniform
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y
      double precision z

      seed = 123456789
      x_lo = -1.0D+00
      x_hi =  1.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_ASIN'
      write ( *, '(a)' ) 
     &  '  ASIN is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) '  inverse sine of a value between -1 and 1'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '      X               ASIN(X)     SIN(ASIN(X))'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = asin ( x )
        z = sin ( y )
        write ( *, '(2x,g14.6,2x,g14.6,2x,g14.6)' ) x, y, z
      end do

      return
      end
      subroutine test_atan

c*********************************************************************72
c
cc TEST_ATAN tests ATAN.
c
c  Discussion:
c
c    The FORTRAN77 function ATAN returns the inverse tangent of a number X.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      double precision r8_uniform
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y
      double precision z

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_ATAN'
      write ( *, '(a)' ) 
     &  '  ATAN is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) '  inverse tangent of a value'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '      X               ATAN(X)     TAN(ATAN(X))'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = atan ( x )
        z = tan ( y )
        write ( *, '(2x,g14.6,2x,g14.6,2x,g14.6)' ) x, y, z
      end do

      return
      end
      subroutine test_atan2

c*********************************************************************72
c
cc TEST_ATAN2 tests ATAN2.
c
c  Discussion:
c
c    The FORTRAN77 function ATAN2 returns the inverse tangent of a number X.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision w
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y
      double precision z

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_ATAN2'
      write ( *, '(a)' ) 
     &  '  ATAN2 is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) '  inverse tangent of a value'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '       X               Y           ATAN2(Y,X)  TAN(ATAN2(Y,X))'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = r8_uniform ( x_lo, x_hi, seed )
        z = atan2 ( y, x )
        w = tan ( z )
        write ( *, '(2x,g14.6,2x,g14.6,2x,g14.6,2x,g14.6)' ) x, y, z, w
      end do

      return
      end
      subroutine test_btest

c*********************************************************************72
c
cc TEST_BTEST tests BTEST.
c
c  Discussion:
c
c    The FORTRAN77 function BTEST reports whether a given bit is 0 or 1
c    in an integer word.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i1
      integer i2
      logical l
      integer pos
      integer pos_high
      character*32 s

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_BTEST'
      write ( *, '(a)' ) 
     &  '  BTEST(I,POS) is a FORTRAN77 function which is TRUE'
      write ( *, '(a)' ) '  if bit POS of I is 1.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '  Here, we are only going to check the lowest 32 bits.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I       POS    BTEST(I,POS)'
      write ( *, '(a)' ) ' '

      i1 = 213456
      pos_high = min ( 32, bit_size(i1) )

      do pos = 0, pos_high - 1

        l = btest ( i1, pos )

        if ( l ) then
          s(pos_high-pos:pos_high-pos) = '1'
        else
          s(pos_high-pos:pos_high-pos) = '0'
        end if

        write ( *, '(2x,i8,2x,i8,10x,l1)' ) i1, pos, l

      end do

      write ( *, '(a)' ) ' '
      write ( *, '(a,i12,a)' ) 
     &  '  The binary representation of ', i1, ' is:'
      write ( *, '(a)' ) '  "' // s //'".' 

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I       POS    BTEST(I,POS)'
      write ( *, '(a)' ) ' '

      i1 = -28

      do pos = 0, pos_high - 1

        l = btest ( i1, pos )

        if ( l ) then
          s(pos_high-pos:pos_high-pos) = '1'
        else
          s(pos_high-pos:pos_high-pos) = '0'
        end if

        write ( *, '(2x,i8,2x,i8,10x,l1)' ) i1, pos, l

      end do

      write ( *, '(a)' ) ' '
      write ( *, '(a,i12,a)' ) 
     &  '  The binary representation of ', i1, ' is:'
      write ( *, '(a)' ) '  "' // s //'".' 

      return
      end
      subroutine test_char

c*********************************************************************72
c
cc TEST_CHAR tests CHAR
c
c  Discussion:
c
c    The FORTRAN77 function CHAR returns the character corresponding
c    to the given character index, between 0 and 255.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      character c
      logical ch_is_printable
      integer i

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_CHAR'
      write ( *, '(a)' ) 
     &  '  CHAR is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) '  character of given character index.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I     CHAR(I)'
      write ( *, '(a)' ) ' '

      do i = 0, 255

        c = char ( i )

        if ( ch_is_printable ( c ) ) then
          write ( *, '(2x,i8,8x,a1)' ) i, c
        end if

      end do

      return
      end
      subroutine test_cmplx

c*********************************************************************72
c
cc TEST_CMPLX tests CMPLX.
c
c  Discussion:
c
c    The FORTRAN77 function CMPLX returns a complex number given its
c    real and imaginary parts.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_CMPLX'
      write ( *, '(a)' ) 
     &  '  CMPLX is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) 
     &  '  complex number formed by real and imaginary parts.'
      write ( *, '(a)' ) ' '
      write ( *, '(a,f14.6,f14.6)' ) '  CMPLX(1)        ', cmplx ( 1 )
      write ( *, '(a,f14.6,f14.6)' ) 
     &  '  CMPLX(2,3)      ', cmplx ( 2, 3 )
      write ( *, '(a,f14.6,f14.6)' ) 
     &  '  CMPLX(4.5)      ', cmplx ( 4.5 )
      write ( *, '(a,f14.6,f14.6)' ) 
     &  '  CMPLX(6.7, 8.9 )', cmplx ( 6.7, 8.9 )

      return
      end
      subroutine test_conjg

c*********************************************************************72
c
cc TEST_CONJG tests CONJG.
c
c  Discussion:
c
c    The FORTRAN77 function CONJG returns the conjugate of a complex number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      complex c4_uniform_01
      complex c1
      complex c2
      integer i
      integer seed

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_CONJG'
      write ( *, '(a)' ) '  CONJG is a FORTRAN77 function which'
      write ( *, '(a)' ) '  returns the conjugate of a complex number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a,a)' ) '                  X ',
     &  '                            CONJG(X)'
      write ( *, '(a,a)' ) 
     &  '     --------------------------      ',
     &  '----------------------------'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        c1 = c4_uniform_01 ( seed )
        c2 = conjg ( c1 )
        write ( *, '(2x,f14.6,f14.6,6x,f14.6,f14.6)' ) c1, c2
      end do

      return
      end
      subroutine test_cos_c4

c*********************************************************************72
c
cc TEST_COS_C4 tests COS on complex arguments.
c
c  Discussion:
c
c    The FORTRAN77 function COS returns the cosine of a real or complex number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      complex c4_uniform_01
      integer i
      integer seed
      complex x_c4
      complex y_c4

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_COS_C4'
      write ( *, '(a)' ) '  COS is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the cosine of a real or complex number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Here we use complex arguments.'
      write ( *, '(a)' ) ' '
      write ( *, '(a,a)' ) '              X',
     &  '                          COS(X)'
      write ( *, '(a,a)' ) 
     &  '    --------------------------  ', 
     &  '----------------------------'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x_c4 = c4_uniform_01 ( seed )
        y_c4 = cos ( x_c4 )
        write ( *, '(2x,2g14.6,2x,2g14.6)' ) x_c4, y_c4
      end do

      return
      end
      subroutine test_cos_r8

c*********************************************************************72
c
cc TEST_COS_R8 tests COS on double precision arguments.
c
c  Discussion:
c
c    The FORTRAN77 function COS returns the cosine of a real or complex number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_COS_R8'
      write ( *, '(a)' ) '  COS is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the cosine of a real or complex number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Here we use double precision arguments.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X              COS(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = cos ( x )
        write ( *, '(2x,g14.6,2x,g14.6)' ) x, y
      end do

      return
      end
      subroutine test_cosh

c*********************************************************************72
c
cc TEST_COSH tests COSH.
c
c  Discussion:
c
c    The FORTRAN77 function COSH returns the hyperbolic cosine of a real number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_COSH'
      write ( *, '(a)' ) '  COSH is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the hyperbolic cosine of a real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X              COSH(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = cosh ( x )
        write ( *, '(2x,g14.6,2x,g14.6)' ) x, y
      end do

      return
      end
      subroutine test_dble

c*********************************************************************72
c
cc TEST_DBLE tests DBLE.
c
c  Discussion:
c
c    The FORTRAN77 function DBLE converts a numeric value to double precision.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      complex x_c4
      integer x_i4
      real x_r4
      double precision x_r8

      x_c4 = ( 1.1E+00, 2.2E+00 )
      x_i4 = 5
      x_r4 = 7.7E+00
      x_r8 = 8.8D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_DBLE'
      write ( *, '(a)' ) '  DBLE is a FORTRAN77 function which converts'
      write ( *, '(a)' ) 
     &  '  a complex, integer or real value to double precision'
      write ( *, '(a)' ) '  real'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '  Type                   X             DBLE(X)'
      write ( *, '(a)' ) ' '
      write ( *, '(a,f6.4,2x,f6.4,2x,f6.4)' ) '  complex           ',
     & x_c4, dble ( x_c4 )
      write ( *, '(a,i6,10x,f6.4)'          ) '  integer           ', 
     & x_i4, dble ( x_i4 )
      write ( *, '(a,f6.4,10x,f6.4)'        ) '  real              ', 
     & x_r4, dble ( x_r4 )
      write ( *, '(a,f6.4,10x,f6.4)'        ) '  double precision  ', 
     & x_r8, dble ( x_r8 )

      return
      end
      subroutine test_dim

c*********************************************************************72
c
cc TEST_DIM tests DIM
c
c  Discussion:
c
c    The FORTRAN77 function DIM(X,Y) returns the maximum of (X-Y) and 0.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i4_uniform
      real r
      integer seed
      integer test
      integer test_num
      parameter ( test_num = 5 )
      integer x
      integer y
      integer z

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_DIM'
      write ( *, '(a)' ) '  DIM is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the maximum of X-Y or 0.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Arithmetic type: integer X, Y'

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X       Y  DIM(X,Y)'
      write ( *, '(a)' ) ' '

      do test = 1, test_num

        x = i4_uniform ( 0, 100, seed )
        y = i4_uniform ( 0, 100, seed )

        z = dim ( x, y )

        write ( *, '(2x,i6,2x,i6,2x,i6)' ) x, y, z

      end do

      return
      end
      subroutine test_dprod

c*********************************************************************72
c
cc TEST_DPROD tests DPROD
c
c  Discussion:
c
c    The FORTRAN77 function DPROD(X,Y) returns the product of real
c    numbers X and Y using double precision.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      real r4_uniform_01
      integer seed
      real x_r4
      real y_r4

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_DPROD'
      write ( *, '(a)' ) '  DPROD is a FORTRAN77 function which'
      write ( *, '(a)' ) '  returns the product of real values X and Y,'
      write ( *, '(a)' ) '  using double precision.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  real x, y'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '        X             Y         DPROD(X,Y)'
      write ( *, '(a)' ) ' '
      do i = 1, 5
        x_r4 = 100.0E+00 * r4_uniform_01 ( seed )
        y_r4 = 100.0E+00 * r4_uniform_01 ( seed )
        write ( *, '(2x,f12.6,2x,f12.6,2x,f12.6)' ) 
     &    x_r4, y_r4, dprod ( x_r4, y_r4 )
      end do

      return
      end
      subroutine test_exp

c*********************************************************************72
c
cc TEST_EXP tests EXP.
c
c  Discussion:
c
c    The FORTRAN77 function EXP returns the exponential of a real number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_EXP'
      write ( *, '(a)' ) 
     &  '  EXP is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) '  exponential of a real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X              EXP(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = exp ( x )
        write ( *, '(2x,g14.6,2x,g14.6)' ) x, y
      end do

      return
      end
      subroutine test_iand_i4

c*********************************************************************72
c
cc TEST_IAND_I4 tests IAND on integer arguments.
c
c  Discussion:
c
c    The FORTRAN77 function IAND returns the bitwise AND 
c    of two integers.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      integer i4_uniform
      integer j
      integer k
      integer seed
      integer test

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_IAND_I4'
      write ( *, '(a)' ) 
     &  '  IAND is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) '  bitwise AND of two integers.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Here, I and J are integers.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I         J    IAND(I,J)'
      write ( *, '(a)' ) ' '

      do test = 1, 10
        i = i4_uniform ( 0, 100, seed )
        j = i4_uniform ( 0, 100, seed )
        k = iand ( i, j )
        write ( *, '(2x,i8,2x,i8,2x,i8)' ) i, j, k
      end do

      return
      end
      subroutine test_ibclr

c*********************************************************************72
c
cc TEST_IBCLR tests IBCLR.
c
c  Discussion:
c
c    The FORTRAN77 function IBCLR sets a given bit to zero in an integer word.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i1
      integer i2
      integer pos
c
c  Put 11 consecutive 1's into I1.
c
      i1 = 0
      do pos = 0, 10
        i1 = 2 * i1 + 1
      end do

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_IBCLR'
      write ( *, '(a)' ) 
     &  '  IBCLR is a FORTRAN77 function which sets a given'
      write ( *, '(a)' ) '  bit to zero in an integer word.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I       POS    IBCLR(I,POS)'
      write ( *, '(a)' ) ' '
      do pos = 0, 10
        i2 = ibclr ( i1, pos )
        write ( *, '(2x,i8,2x,i8,2x,i8)' ) i1, pos, i2
      end do

      return
      end
      subroutine test_ibits

c*********************************************************************72
c
cc TEST_IBITS tests IBITS.
c
c  Discussion:
c
c    The FORTRAN77 function IBITS extracts a sequence of bits from
c    an integer word.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer byte
      integer i
      integer i1
      integer i2
      integer i4
      integer len
      integer pos

      i1 = 1396

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_IBITS'
      write ( *, '(a)' ) 
     &  '  IBITS is a FORTRAN77 function which extracts'
      write ( *, '(a)' ) '  LEN bits from word I start at position POS.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I       POS    LEN    IBITS(I,POS,LEN)'
      write ( *, '(a)' ) ' '
      len = 3
      do pos = 0, 10
        i2 = ibits ( i1, pos, len )
        write ( *, '(2x,i8,2x,i8,2x,i8,2x,i8)' ) i1, pos, len, i2
      end do

      write ( *, '(a)' ) ' '

      pos = 2
      do len = 1, 10
        i2 = ibits ( i1, pos, len )
        write ( *, '(2x,i8,2x,i8,2x,i8,2x,i8)' ) i1, pos, len, i2
      end do

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '  Use IBITS to extract the 4 bytes that make up'
      write ( *, '(a)' ) '  an integer word.'
      write ( *, '(a)' ) ' '
c
c  3: 00111110 =  62
c
c  2: 00000100 =   4
c
c  1: 11010010 = 210
c
c  0: 00001111 =  15
c
      i4 = 2**29 + 2**28 + 2**27 + 2**26 + 2**25 
     &   + 2**18 
     &   + 2**15 + 2**14 + 2**12 + 2**9 
     &   + 2**3 + 2**2 + 2**1 + 2**0 

      write ( *, '(a,i12)' ) '  I4 = ', i4
      write ( *, '(a)' ) ' '
      len = 8
      do i = 0, 3
        pos = i * len
        byte = ibits ( i4, pos, len )
        write ( *, '(a,i8,a,i8)' ) '  Byte ', i, ' = ', byte
      end do

      return
      end
      subroutine test_ibset

c*********************************************************************72
c
cc TEST_IBSET tests IBSET.
c
c  Discussion:
c
c    The FORTRAN77 function IBSET sets a given bit to one in an integer word.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i1
      integer i2
      integer pos

      i1 = 0

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_IBSET'
      write ( *, '(a)' ) 
     &  '  IBSET is a FORTRAN77 function which sets a given'
      write ( *, '(a)' ) '  bit to one in an integer word.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I       POS    IBSET(I,POS)'
      write ( *, '(a)' ) ' '
      do pos = 0, 10
        i2 = ibset ( i1, pos )
        write ( *, '(2x,i8,2x,i8,2x,i8)' ) i1, pos, i2
        i1 = i2
      end do

      return
      end
      subroutine test_ichar

c*********************************************************************72
c
cc TEST_ICHAR tests ICHAR
c
c  Discussion:
c
c    The FORTRAN77 function ICHAR returns the character index (between 0 
c    and 255) of a character.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      character c1
      character c2
      integer i
      integer i1
      character*25 string

      string = 'This is a string of text!'

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_ICHAR'
      write ( *, '(a)' ) 
     &  '  ICHAR is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) '  character index of a given character'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  C        ICHAR(C)    CHAR(ICHAR(C))'
      write ( *, '(a)' ) ' '

      do i = 1, len ( string )

        c1 = string (i:i)

        i1 = ichar ( c1 )

        c2 = char ( i1 )

        write ( *, '(2x,a1,8x,i8,8x,a1)' ) c1, i1, c2

      end do

      return
      end
      subroutine test_ieor_i4

c*********************************************************************72
c
cc TEST_IEOR_I4 tests IEOR on integer arguments.
c
c  Discussion:
c
c    The FORTRAN77 function IEOR returns the bitwise exclusive OR 
c    of two integers.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      integer i4_uniform
      integer j
      integer k
      real r
      integer seed
      integer test

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_IEOR_I4'
      write ( *, '(a)' ) '  IEOR is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the bitwise exclusive OR of two integers.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Here, I and J are integers.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I         J    IEOR(I,J)'
      write ( *, '(a)' ) ' '

      do test = 1, 10
        i = i4_uniform ( 0, 100, seed )
        j = i4_uniform ( 0, 100, seed )
        k = ieor ( i, j )
        write ( *, '(2x,i8,2x,i8,2x,i8)' ) i, j, k
      end do

      return
      end
      subroutine test_index

c*********************************************************************72
c
cc TEST_INDEX tests INDEX.
c
c  Discussion:
c
c    The FORTRAN77 function INDEX determines the first occurrence
c    of a substring in a string.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_INDEX'
      write ( *, '(a)' ) '  INDEX(S,SUB) is a FORTRAN77 function which'
      write ( *, '(a)' ) 
     &  '  returns the location of the first occurrence'
      write ( *, '(a)' ) '  of substring SUB in string S.'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) 
     &  '  index ( ''THE CATATONIC CAT'', ''CAT'' )', 
     &  index ( 'THE CATATONIC CAT', 'CAT' )
      write ( *, '(a,i8)' ) 
     &  '  index ( ''THE CATATONIC CAT'', ''cat'' )', 
     &  index ( 'THE CATATONIC CAT', 'cat' )

      return
      end
      subroutine test_int

c*********************************************************************72
c
cc TEST_INT tests INT.
c
c  Discussion:
c
c    The FORTRAN77 function INT converts a numeric value to an integer.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      complex x_c4
      integer x_i4
      real x_r4
      double precision x_r8

      x_c4 = ( 1.1E+00, 2.2E+00 )
      x_i4 = 5
      x_r4 = 7.7E+00
      x_r8 = 8.8D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_INT'
      write ( *, '(a)' ) '  INT is a FORTRAN77 function which converts'
      write ( *, '(a)' ) 
     &  '  a complex, integer or real value to integer.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Type                   X             INT(X)'
      write ( *, '(a)' ) ' '
      write ( *, '(a,f6.4,2x,f6.4,2x,i6)' ) '  complex             ', 
     &  x_c4, int ( x_c4 )
      write ( *, '(a,i6,10x,i6)'          ) '  integer             ', 
     &  x_i4, int ( x_i4 )
      write ( *, '(a,f6.4,10x,i6)'        ) '  real                ', 
     &  x_r4, int ( x_r4 )
      write ( *, '(a,f6.4,10x,i6)'        ) '  double precision    ', 
     &  x_r8, int ( x_r8 )

      return
      end
      subroutine test_ior_i4

c*********************************************************************72
c
cc TEST_IOR_I4 tests IOR on integer arguments.
c
c  Discussion:
c
c    The FORTRAN77 function IOR returns the bitwise inclusive OR 
c    of two integers.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      integer i4_uniform
      integer j
      integer k
      integer seed
      integer test

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_IOR_I4'
      write ( *, '(a)' ) '  IOR is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the bitwise inclusive OR of two integers.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Here, I and J are integers.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I         J     IOR(I,J)'
      write ( *, '(a)' ) ' '

      do test = 1, 10
        i = i4_uniform ( 0, 100, seed )
        j = i4_uniform ( 0, 100, seed )
        k = ior ( i, j )
        write ( *, '(2x,i8,2x,i8,2x,i8)' ) i, j, k
      end do

      return
      end
      subroutine test_ishft

c*********************************************************************72
c
cc TEST_ISHFT tests ISHFT.
c
c  Discussion:
c
c    The FORTRAN77 function ISHFT shifts the bits in an integer word.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i1
      integer i2
      integer shift

      i1 = 89

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_ISHFT'
      write ( *, '(a)' ) '  ISHFT is a FORTRAN77 function which shifts'
      write ( *, '(a)' ) '  the bits in an integer word.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I       SHIFT    ISHFT(I,SHIFT)'
      write ( *, '(a)' ) ' '
      do shift = -5, 5
        i2 = ishft ( i1, shift )
        write ( *, '(2x,i8,2x,i8,2x,i8)' ) i1, shift, i2
      end do

      return
      end
      subroutine test_ishftc

c*********************************************************************72
c
cc TEST_ISHFTC tests ISHFTC.
c
c  Discussion:
c
c    The FORTRAN77 function ISHFTC circular-shifts the bits in an integer word.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i1
      integer i2
      integer shift
      integer size

      i1 = 89
      size = 32

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_ISHFTC'
      write ( *, '(a)' ) '  ISHFTC is a FORTRAN77 function which'
      write ( *, '(a)' ) 
     &  '  circular-shifts the bits in an integer word.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '         I       SHIFT    ISHFTC(I,SHIFT,SIZE)'
      write ( *, '(a)' ) ' '
      do shift = -5, 5
        i2 = ishftc ( i1, shift, size )
        write ( *, '(2x,i8,2x,i8,2x,i12)' ) i1, shift, i2
      end do

      return
      end
      subroutine test_len

c*********************************************************************72
c
cc TEST_LEN tests LEN.
c
c  Discussion:
c
c    The FORTRAN77 function LEN returns the declared length of a
c    character string.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      character*1 s1
      character*2 s2
      character*4 s4
      character*8 s8
      character*16 s16

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_LEN'
      write ( *, '(a)' ) 
     &  '  LEN is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) 
     &  '  declared length of a string variable, or the length of'
      write ( *, '(a)' ) '  a string constant'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '      S                     LEN(S)'
      write ( *, '(a)' ) '   ----------               -----'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  character*1            ', len ( s1 )
      write ( *, '(a,i8)' ) '  character*2            ', len ( s2 )
      write ( *, '(a,i8)' ) '  character*4            ', len ( s4 )
      write ( *, '(a,i8)' ) '  character*8            ', len ( s8 )
      write ( *, '(a,i8)' ) '  character*16           ', len ( s16 )
      write ( *, '(a,i8)' ) 
     &  ' "A STRING"              ', len ( 'A STRING' )

      return
      end
      subroutine test_len_trim

c*********************************************************************72
c
cc TEST_LEN_TRIM tests LEN_TRIM.
c
c  Discussion:
c
c    The FORTRAN77 function LEN_TRIM returns the "used" length of a
c    character string, up to the last non_blank.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      character*10 s

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_LEN_TRIM'
      write ( *, '(a)' ) 
     &  '  LEN_TRIM is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) 
     &  '  "used" length of a string variable up to the last'
      write ( *, '(a)' ) '  nonblank.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '      S          LEN_TRIM(S)'
      write ( *, '(a)' ) '   ----------    ----------'
      write ( *, '(a)' ) ' '
      s = '1234567890'
      write ( *, '(a,i8)' ) '  "' // s // '"', len_trim(s)
      s = '12345     '
      write ( *, '(a,i8)' ) '  "' // s // '"', len_trim(s)
      s = '     67890'
      write ( *, '(a,i8)' ) '  "' // s // '"', len_trim(s)
      s = '    5     '
      write ( *, '(a,i8)' ) '  "' // s // '"', len_trim(s)
      s = '1 3 5 7 9 '
      write ( *, '(a,i8)' ) '  "' // s // '"', len_trim(s)

      return
      end
      subroutine test_lge

c*********************************************************************72
c
cc TEST_LGE tests LGE.
c
c  Discussion:
c
c    The FORTRAN77 function LGE(S1,S2) returns the value of
c    "string S1 is lexically greater than or equal to string S2".
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      character*3 s4
      character*3 s5
      character*4 s6
      character*3 s7
      character*3 s8

      s4 = 'boy' 
      s5 = 'cat'
      s6 = 'cats'
      s7 = 'dog'
      s8 = 'CAT'

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_LGE'
      write ( *, '(a)' ) 
     &  '  LGE is a FORTRAN77 function which returns the value'
      write ( *, '(a)' ) '  of "S1 >= S2" where S1 and S2 are strings.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '    S1    S2   LGE(S1,S2)'
      write ( *, '(a)' ) '   ---   ---   ----------'
      write ( *, '(a)' ) ' '
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s4 // '"  "' // s4 // '"  ', lge ( s4, s4 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s4 // '"  "' // s5 // '"  ', lge ( s4, s5 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s5 // '"  "' // s4 // '"  ', lge ( s5, s4 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s5 // '"  "' // s7 // '"  ', lge ( s5, s7 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s5 // '"  "' // s8 // '"  ', lge ( s5, s8 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s5 // '"  "' // s6 //  '" ', lge ( s5, s6 )

      return
      end
      subroutine test_lgt

c*********************************************************************72
c
cc TEST_LGT tests LGT.
c
c  Discussion:
c
c    The FORTRAN77 function LGT(S1,S2) returns the value of
c    "string S1 is lexically greater than string S2".
c
c  Modified:
c
c    05 July 2006
c
c  Author:
c
c    John Burkardt
c
      implicit none

      character*3 s4
      character*3 s5
      character*4 s6
      character*3 s7
      character*3 s8

      s4 = 'boy' 
      s5 = 'cat'
      s6 = 'cats'
      s7 = 'dog'

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_LGT'
      write ( *, '(a)' ) 
     &  '  LGT is a FORTRAN77 function which returns the value'
      write ( *, '(a)' ) '  of "S1 > S2" where S1 and S2 are strings.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '    S1    S2   LGT(S1,S2)'
      write ( *, '(a)' ) '   ---   ---   ----------'
      write ( *, '(a)' ) ' '
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s4 // '"  "' // s4 // '"  ', lgt ( s4, s4 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s4 // '"  "' // s5 // '"  ', lgt ( s4, s5 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s5 // '"  "' // s4 // '"  ', lgt ( s5, s4 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s5 // '"  "' // s7 // '"  ', lgt ( s5, s7 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s5 // '"  "' // s8 // '"  ', lgt ( s5, s8 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s5 // '"  "' // s6 //  '" ', lgt ( s5, s6 )

      return
      end
      subroutine test_lle

c*********************************************************************72
c
cc TEST_LLE tests LLE.
c
c  Discussion:
c
c    The FORTRAN77 function LLE(S1,S2) returns the value of
c    "string S1 is lexically less than or equal to string S2".
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      character*3 s4
      character*3 s5
      character*4 s6
      character*3 s7
      character*3 s8

      s4 = 'boy' 
      s5 = 'cat'
      s6 = 'cats'
      s7 = 'dog'

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_LLE'
      write ( *, '(a)' ) 
     &  '  LLE is a FORTRAN77 function which returns the value'
      write ( *, '(a)' ) '  of "S1 <= S2" where S1 and S2 are strings.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '    S1    S2   LLE(S1,S2)'
      write ( *, '(a)' ) '   ---   ---   ----------'
      write ( *, '(a)' ) ' '
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s4 // '"  "' // s4 // '"  ', lle ( s4, s4 )
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s4 // '"  "' // s5 // '"  ', lle ( s4, s5 )
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s5 // '"  "' // s4 // '"  ', lle ( s5, s4 )
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s5 // '"  "' // s7 // '"  ', lle ( s5, s7 )
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s5 // '"  "' // s8 // '"  ', lle ( s5, s8 )
      write ( *, '(a,8x,l1)' ) 
     &  '  "' // s5 // '"  "' // s6 //  '" ', lle ( s5, s6 )

      return
      end
      subroutine test_llt

c*********************************************************************72
c
cc TEST_LLT tests LLT.
c
c  Discussion:
c
c    The FORTRAN77 function LLT(S1,S2) returns the value of
c    "string S1 is lexically less than string S2".
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      character*3 s4
      character*3 s5
      character*4 s6
      character*3 s7
      character*3 s8

      s4 = 'boy' 
      s5 = 'cat'
      s6 = 'cats'
      s7 = 'dog'

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_LGT'
      write ( *, '(a)' ) 
     &  '  LLT is a FORTRAN77 function which returns the'
      write ( *, '(a)' ) 
     &  '  value of "S1 < S2" where S1 and S2 are strings.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '    S1    S2   LLT(S1,S2)'
      write ( *, '(a)' ) '   ---   ---   ----------'
      write ( *, '(a)' ) ' '
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s4 // '"  "' // s4 // '"  ', llt ( s4, s4 )
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s4 // '"  "' // s5 // '"  ', llt ( s4, s5 )
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s5 // '"  "' // s4 // '"  ', llt ( s5, s4 )
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s5 // '"  "' // s7 // '"  ', llt ( s5, s7 )
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s5 // '"  "' // s8 // '"  ', llt ( s5, s8 )
      write ( *, '(a,8x,l1)' )  
     &  '  "' // s5 // '"  "' // s6 //  '" ', llt ( s5, s6 )

      return
      end
      subroutine test_log

c*********************************************************************72
c
cc TEST_LOG tests LOG.
c
c  Discussion:
c
c    The FORTRAN77 function LOG returns the natural logarithm of a real number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y
      double precision z

      seed = 123456789
      x_lo =   0.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_LOG'
      write ( *, '(a)' ) '  LOG is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the natural logarithm of a real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X              LOG(X)     EXP(LOG(X))'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = log ( x )
        z = exp ( y )
        write ( *, '(2x,g14.6,2x,g14.6,2x,g14.6)' ) x, y, z
      end do

      return
      end
      subroutine test_log10

c*********************************************************************72
c
cc TEST_LOG10 tests LOG10.
c
c  Discussion:
c
c    The FORTRAN77 function LOG10 returns the base 10 logarithm of a 
c    real number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y
      double precision z

      seed = 123456789
      x_lo =   0.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_LOG10'
      write ( *, '(a)' ) '  LOG10 is a FORTRAN77 function which'
      write ( *, '(a)' ) '  returns the base 10 logarithm '
      write ( *, '(a)' ) '  of a real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '       X              LOG10(X)     10**(LOG(X))'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = log10 ( x )
        z = 10.0D+00**y
        write ( *, '(2x,g14.6,2x,g14.6,2x,g14.6)' ) x, y, z
      end do

      return
      end
      subroutine test_max

c*********************************************************************72
c
cc TEST_MAX tests MAX.
c
c  Discussion:
c
c    The FORTRAN77 function MAX returns the maximum value in a list.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_MAX'
      write ( *, '(a)' ) '  MAX is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the maximum value in a list.'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  max(2,1) =     ', max ( 2, 1 )
      write ( *, '(a,i8)' ) '  max(1,3,2) =   ', max ( 1, 3, 2 )
      write ( *, '(a,i8)' ) '  max(3,2,4,1) = ', max ( 3, 2, 4, 1 )
      write ( *, '(a)' ) ' '
      write ( *, '(a,f4.1)' ) 
     &  '  max(2.1, 1.2) =           ', max ( 2.1, 1.2 )
      write ( *, '(a,f4.1)' ) 
     &  '  max(1.1, 3.2, 2.3) =      ', max ( 1.1, 3.2, 2.3 )
      write ( *, '(a,f4.1)' ) 
     &  '  max(3.1, 2.2, 4.3, 1.4) = ', max ( 3.1, 2.2, 4.3, 1.4 )

      return
      end
      subroutine test_min

c*********************************************************************72
c
cc TEST_MIN tests MIN.
c
c  Discussion:
c
c    The FORTRAN77 function MIN returns the minimum value in a list.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_MIN'
      write ( *, '(a)' ) '  MIN is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the minimum value in a list.'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  min(3, 4) =       ', min ( 3, 4 )
      write ( *, '(a,i8)' ) '  min(4, 2, 3) =    ', min ( 4, 2, 3 )
      write ( *, '(a,i8)' ) '  min(2, 3, 1, 4) = ', min ( 2, 3, 1, 4 )

      write ( *, '(a)' ) ' '
      write ( *, '(a,f4.1)' ) 
     &  '  min(3.1, 4.2) =           ', min ( 3.1, 4.2 )
      write ( *, '(a,f4.1)' ) 
     &  '  min(4.1. 2.2, 3.3) =      ', min ( 4.1, 2.2, 3.3 )
      write ( *, '(a,f4.1)' ) 
     &  '  min(2.1, 3.2, 1.3, 4.4) = ', min ( 2.1, 3.2, 1.3, 4.4 )

      return
      end
      subroutine test_mod_i4

c*********************************************************************72
c
cc TEST_MOD_I4 tests MOD on integers.
c
c  Discussion:
c
c    The FORTRAN77 function MOD(A,B) returns the remainder after division.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      integer i_hi
      integer i_lo
      integer i4_uniform
      integer j
      integer k
      integer seed
      integer test

      seed = 123456789
      i_lo = -10
      i_hi =  20

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_MOD_I4'
      write ( *, '(a)' ) '  MOD is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the remainder after division.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Here, the arguments are integers.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '         I         J       MOD(I,J)'
      write ( *, '(a)' ) ' '
      do test = 1, 10

        i = i4_uniform ( i_lo, i_hi, seed )
        j = i4_uniform ( i_lo, i_hi, seed )

        if ( j == 0 ) then
          write ( *, '(2x,i8,2x,i8,2x,a)' ) i, j, 'Undefined'
        else
          k = mod ( i, j )
          write ( *, '(2x,i8,2x,i8,2x,i8)' ) i, j, k
        end if

      end do

      return
      end
      subroutine test_mod_r4

c*********************************************************************72
c
cc TEST_MOD_R4 tests MOD on reals.
c
c  Discussion:
c
c    The FORTRAN77 function MOD(A,B) returns the remainder after division.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      real r4_uniform
      integer seed
      integer test
      real x
      real x_hi
      real x_lo
      real y
      real z

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  20.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_MOD_R4'
      write ( *, '(a)' ) '  MOD is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the remainder after division.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Here, the arguments are reals.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '          X               Y              MOD(X,Y)'
      write ( *, '(a)' ) ' '
      do test = 1, 10
        x = r4_uniform ( x_lo, x_hi, seed )
        y = r4_uniform ( x_lo, x_hi, seed )
        z = mod ( x, y )
        write ( *, '(2x,f14.6,2x,f14.6,2x,f14.6)' ) x, y, z
      end do

      return
      end
      subroutine test_mvbits

c*********************************************************************72
c
cc TEST_MVBITS tests MVBITS.
c
c  Discussion:
c
c    The FORTRAN77 function MVBITS copies a sequence of bits.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i1
      integer i2

      i1 = 1396
      i2 = 0

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_MVBITS'
      write ( *, '(a)' ) '  MVBITS is a FORTRAN77 function which'
      write ( *, '(a)' ) '  extracts bits from one place and copies'
      write ( *, '(a)' ) '  them elsewhere.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  CALL MVBITS(FROM,FROMPOS,LEN,TO,TOPOS)'
      write ( *, '(a)' ) ' '
      write ( *, '(a,i8)' ) '  We will always use I1 =        ', i1
      write ( *, '(a,i8)' ) '  We will always start with I2 = ', i2

      write ( *, '(a)' ) ' '
      call mvbits ( i1, 0, 5, i2, 0 )
      write ( *, '(a,i12)' ) '  CALL MVBITS(I1,0, 5,I2,0): I2 = ', i2

      i2 = 0
      call mvbits ( i1, 0, 32, i2, 0 )
      write ( *, '(a,i12)' ) '  CALL MVBITS(I1,0,32,I2,0): I2 = ', i2

      i2 = 0
      call mvbits ( i1, 5, 5, i2, 0 )
      write ( *, '(a,i12)' ) '  CALL MVBITS(I1,5, 5,I2,0): I2 = ', i2

      i2 = 0
      call mvbits ( i1, 5, 5, i2, 5 )
      write ( *, '(a,i12)' ) '  CALL MVBITS(I1,5, 5,I2,5): I2 = ', i2

      return
      end
      subroutine test_nint

c*********************************************************************72
c
cc TEST_NINT tests NINT.
c
c  Discussion:
c
c    The FORTRAN77 function NINT returns, as an integer, the nearest 
c    integer to a given real value.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer j
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      integer y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_NINT'
      write ( *, '(a)' ) '  NINT is a FORTRAN77 function which returns,'
      write ( *, '(a)' ) '  as an integer, the nearest integer to a '
      write ( *, '(a)' ) '  given real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X             NINT(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        j = nint ( x )
        write ( *, '(2x,g14.6,2x,i8)' ) x, j
      end do

      return
      end
      subroutine test_not_i4

c*********************************************************************72
c
cc TEST_NOT_I4 tests NOT on integer arguments.
c
c  Discussion:
c
c    The FORTRAN77 function NOT returns the bitwise NOT of an integer.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      integer i
      integer i4_uniform
      integer j
      integer seed
      integer test

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_NOT_I4'
      write ( *, '(a)' ) '  NOT is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the bitwise NOT of an integer.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Here, I is an integer.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '             I         NOT(I)'
      write ( *, '(a)' ) ' '

      do test = 1, 10
        i = i4_uniform ( 0, 100, seed )
        j = not ( i )
        write ( *, '(2x,i12,2x,i12)' ) i, j
      end do

      return
      end
      subroutine test_real_c4

c*********************************************************************72
c
cc TEST_REAL_C4 tests REAL as applied to complex numbers.
c
c  Discussion:
c
c    The FORTRAN77 function REAL can return the real part of a complex number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      complex c4_uniform_01
      complex c
      integer i
      real r
      integer seed

      seed = 123456789

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_REAL_C4'
      write ( *, '(a)' ) '  REAL is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the real part of a complex number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '                  X                         REAL(X)'
      write ( *, '(a)' ) 
     &  '       ------------------------    ----------------'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        c = c4_uniform_01 ( seed )
        r = real ( c )
        write ( *, '(2x,f14.6,f14.6,6x,f14.6)' ) c, r
      end do

      return
      end
      subroutine test_sign

c*********************************************************************72
c
cc TEST_SIGN tests SIGN.
c
c  Discussion:
c
c    The FORTRAN77 function SIGN(X,Y) transfers the sign of Y to the
c    magnitude of X.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y
      double precision z

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_SIGN'
      write ( *, '(a)' ) '  SIGN is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the sign of Y times the magnitude of X.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X               Y           SIGN(X,Y)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = r8_uniform ( x_lo, x_hi, seed )
        z = sign ( x, y )
        write ( *, '(2x,g14.6,2x,g14.6,2x,g14.6)' ) x, y, z
      end do

      return
      end
      subroutine test_sin_r8

c*********************************************************************72
c
cc TEST_SIN_R8 tests SIN on double precision arguments.
c
c  Discussion:
c
c    The FORTRAN77 function SIN returns the sine of a real or complex number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_SIN_R8'
      write ( *, '(a)' ) '  SIN is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the sine of a real or complex number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '  Here we use double precision arguments.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X              SIN(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = sin ( x )
        write ( *, '(2x,g14.6,2x,g14.6)' ) x, y
      end do

      return
      end
      subroutine test_sinh

c*********************************************************************72
c
cc TEST_SINH tests SINH.
c
c  Discussion:
c
c    The FORTRAN77 function SINH returns the hyperbolic sine of a real number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_SINH'
      write ( *, '(a)' ) '  SINH is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the hyperbolic sine of a real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X              SINH(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = sinh ( x )
        write ( *, '(2x,g14.6,2x,g14.6)' ) x, y
      end do

      return
      end
      subroutine test_sqrt

c*********************************************************************72
c
cc TEST_SQRT tests SQRT.
c
c  Discussion:
c
c    The FORTRAN77 function SQRT returns the square root of a real number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y
      double precision z

      seed = 123456789
      x_lo =   0.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_SQRT'
      write ( *, '(a)' ) '  SQRT is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the square root of a real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 
     &  '       X              SQRT(X)        (SQRT(X))**2'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = sqrt ( x )
        z = y * y
        write ( *, '(2x,g14.6,2x,g14.6,2x,g14.6)' ) x, y, z
      end do

      return
      end
      subroutine test_tan

c*********************************************************************72
c
cc TEST_TAN tests TAN.
c
c  Discussion:
c
c    The FORTRAN77 function TAN returns the tangent of a real number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_TAN'
      write ( *, '(a)' ) '  TAN is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the tangent of a real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X              TAN(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = tan ( x )
        write ( *, '(2x,g14.6,2x,g14.6)' ) x, y
      end do

      return
      end
      subroutine test_tanh

c*********************************************************************72
c
cc TEST_TANH tests TANH.
c
c  Discussion:
c
c    The FORTRAN77 function TANH returns the hyperbolic tangent of a 
c    real number.
c
c  Modified:
c
c    02 June 2007
c
c  Author:
c
c    John Burkardt
c
      implicit none

      double precision r8_uniform
      integer i
      integer seed
      double precision x
      double precision x_hi
      double precision x_lo
      double precision y

      seed = 123456789
      x_lo = -10.0D+00
      x_hi =  10.0D+00

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'TEST_TANH'
      write ( *, '(a)' ) '  TANH is a FORTRAN77 function which returns'
      write ( *, '(a)' ) '  the hyperbolic tangent of a real number.'
      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) '       X              TANH(X)'
      write ( *, '(a)' ) ' '
      do i = 1, 10
        x = r8_uniform ( x_lo, x_hi, seed )
        y = tanh ( x )
        write ( *, '(2x,g14.6,2x,g14.6)' ) x, y
      end do

      return
      end
      function c4_uniform_01 ( seed )

c*********************************************************************72
c
cc C4_UNIFORM_01 returns a unit pseudorandom C4.
c
c  Discussion:
c
c    A C4 is a complex single precision value.
c
c    The angle should be uniformly distributed between 0 and 2 * PI,
c    the square root of the radius uniformly distributed between 0 and 1.
c
c    This results in a uniform distribution of values in the unit circle.
c
c  Modified:
c
c    15 March 2005
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Input/output, integer SEED, the "seed" value, which should NOT be 0.
c    On output, SEED has been updated.
c
c    Output, complex C4_UNIFORM_01, a pseudorandom complex value.
c
      implicit none

      complex c4_uniform_01
      integer i4_huge
      integer k
      real pi
      parameter ( pi = 3.1415926E+00 )
      real r
      integer seed
      real theta

      if ( seed .eq. 0 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'C4_UNIFORM_01 - Fatal error!'
        write ( *, '(a)' ) '  Input value of SEED = 0.'
        stop
      end if

      k = seed / 127773

      seed = 16807 * ( seed - k * 127773 ) - k * 2836

      if ( seed .lt. 0 ) then
        seed = seed + i4_huge ( )
      end if

      r = sqrt ( real ( dble ( seed ) * 4.656612875D-10 ) )

      k = seed / 127773

      seed = 16807 * ( seed - k * 127773 ) - k * 2836

      if ( seed .lt. 0 ) then
        seed = seed + i4_huge ( )
      end if

      theta = 2.0E+00 * pi 
     &  * real ( dble ( seed ) * 4.656612875D-10 )

      c4_uniform_01 = r * cmplx ( cos ( theta ), sin ( theta ) )

      return
      end
      function ch_is_printable ( ch )

c*********************************************************************72
c
cc CH_IS_PRINTABLE is TRUE if C is printable.
c
c  Discussion:
c
c    Instead of ICHAR, we now use the IACHAR function, which
c    guarantees the ASCII collating sequence.
c
c  Modified:
c
c    31 October 2001
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Input, character CH, a character to check.
c
c    Output, logical CH_IS_PRINTABLE is TRUE if C is a printable character.
c
      implicit none

      character ch
      logical ch_is_printable
      integer i

      i = ichar ( ch )

      if ( 32 <= i .and. i <= 126 ) then
        ch_is_printable = .true.
      else
        ch_is_printable = .false.
      end if

      return
      end
      function i4_huge ( )

c*********************************************************************72
c
cc I4_HUGE returns a "huge" I4.
c
c  Modified:
c
c    13 November 2006
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Output, integer I4_HUGE, a huge number.
c
      implicit none

      integer i4_huge

      i4_huge = 2147483647

      return
      end
      function i4_uniform ( a, b, seed )

c*********************************************************************72
c
cc I4_UNIFORM returns a scaled pseudorandom I4.
c
c  Discussion:
c
c    An I4 is an integer value.
c
c    The pseudorandom number should be uniformly distributed
c    between A and B.
c
c  Modified:
c
c    12 November 2006
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    Paul Bratley, Bennett Fox, Linus Schrage,
c    A Guide to Simulation,
c    Springer Verlag, pages 201-202, 1983.
c
c    Pierre L'Ecuyer,
c    Random Number Generation,
c    in Handbook of Simulation,
c    edited by Jerry Banks,
c    Wiley Interscience, page 95, 1998.
c
c    Bennett Fox,
c    Algorithm 647:
c    Implementation and Relative Efficiency of Quasirandom
c    Sequence Generators,
c    ACM Transactions on Mathematical Software,
c    Volume 12, Number 4, pages 362-376, 1986.
c
c    Peter Lewis, Allen Goodman, James Miller
c    A Pseudo-Random Number Generator for the System/360,
c    IBM Systems Journal,
c    Volume 8, pages 136-143, 1969.
c
c  Parameters:
c
c    Input, integer A, B, the limits of the interval.
c
c    Input/output, integer SEED, the "seed" value, which should NOT be 0.
c    On output, SEED has been updated.
c
c    Output, integer I4_UNIFORM, a number between A and B.
c
      implicit none

      integer a
      integer b
      integer i4_huge
      integer i4_uniform
      integer k
      real r
      integer seed
      integer value

      if ( seed .eq. 0 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'I4_UNIFORM - Fatal error!'
        write ( *, '(a)' ) '  Input value of SEED = 0.'
        stop
      end if

      k = seed / 127773

      seed = 16807 * ( seed - k * 127773 ) - k * 2836

      if ( seed .lt. 0 ) then
        seed = seed + i4_huge ( )
      end if

      r = real ( seed ) * 4.656612875E-10
c
c  Scale R to lie between A-0.5 and B+0.5.
c
      r = ( 1.0E+00 - r ) * ( real ( min ( a, b ) ) - 0.5E+00 )
     &  +             r   * ( real ( max ( a, b ) ) + 0.5E+00 )
c
c  Use rounding to convert R to an integer between A and B.
c
      value = nint ( r )

      value = max ( value, min ( a, b ) )
      value = min ( value, max ( a, b ) )

      i4_uniform = value

      return
      end
      function r4_uniform ( a, b, seed )

c*********************************************************************72
c
cc R4_UNIFORM returns a scaled pseudorandom R4.
c
c  Discussion:
c
c    The pseudorandom number should be uniformly distributed
c    between A and B.
c
c  Modified:
c
c    29 January 2005
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Input, real A, B, the limits of the interval.
c
c    Input/output, integer SEED, the "seed" value, which should NOT be 0.
c    On output, SEED has been updated.
c
c    Output, real R4_UNIFORM, a number strictly between A and B.
c
      implicit none

      real a
      real b
      integer i4_huge
      integer k
      integer seed
      real r4_uniform

      if ( seed .eq. 0 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'R4_UNIFORM - Fatal error!'
        write ( *, '(a)' ) '  Input value of SEED = 0.'
        stop
      end if

      k = seed / 127773

      seed = 16807 * ( seed - k * 127773 ) - k * 2836

      if ( seed .lt. 0 ) then
        seed = seed + i4_huge ( )
      end if
c
c  Although SEED can be represented exactly as a 32 bit integer,
c  it generally cannot be represented exactly as a 32 bit real number!
c
      r4_uniform = a + ( b - a ) 
     &  * real ( dble ( seed ) * 4.656612875D-10 )

      return
      end
      function r4_uniform_01 ( seed )

c*********************************************************************72
c
cc R4_UNIFORM_01 returns a unit pseudorandom R4.
c
c  Discussion:
c
c    This routine implements the recursion
c
c      seed = 16807 * seed mod ( 2**31 - 1 )
c      r4_uniform_01 = seed / ( 2**31 - 1 )
c
c    The integer arithmetic never requires more than 32 bits,
c    including a sign bit.
c
c    If the initial seed is 12345, then the first three computations are
c
c      Input     Output      R4_UNIFORM_01
c      SEED      SEED
c
c         12345   207482415  0.096616
c     207482415  1790989824  0.833995
c    1790989824  2035175616  0.947702
c
c  Modified:
c
c    11 August 2004
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    Paul Bratley, Bennett Fox, Linus Schrage,
c    A Guide to Simulation,
c    Springer Verlag, pages 201-202, 1983.
c
c    Pierre L'Ecuyer,
c    Random Number Generation,
c    in Handbook of Simulation,
c    edited by Jerry Banks,
c    Wiley Interscience, page 95, 1998.
c
c    Bennett Fox,
c    Algorithm 647:
c    Implementation and Relative Efficiency of Quasirandom
c    Sequence Generators,
c    ACM Transactions on Mathematical Software,
c    Volume 12, Number 4, pages 362-376, 1986.
c
c    Peter Lewis, Allen Goodman, James Miller,
c    A Pseudo-Random Number Generator for the System/360,
c    IBM Systems Journal,
c    Volume 8, pages 136-143, 1969.
c
c  Parameters:
c
c    Input/output, integer SEED, the "seed" value, which should NOT be 0.
c    On output, SEED has been updated.
c
c    Output, real R4_UNIFORM_01, a new pseudorandom variate,
c    strictly between 0 and 1.
c
      implicit none

      integer i4_huge
      integer k
      integer seed
      real r4_uniform_01

      if ( seed .eq. 0 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'R4_UNIFORM_01 - Fatal error!'
        write ( *, '(a)' ) '  Input value of SEED = 0.'
        stop
      end if

      k = seed / 127773

      seed = 16807 * ( seed - k * 127773 ) - k * 2836

      if ( seed .lt. 0 ) then
        seed = seed + i4_huge ( )
      end if
c
c  Although SEED can be represented exactly as a 32 bit integer,
c  it generally cannot be represented exactly as a 32 bit real number!
c
      r4_uniform_01 = real ( dble ( seed ) * 4.656612875D-10 )

      return
      end
      function r8_uniform ( a, b, seed )

c*********************************************************************72
c
cc R8_UNIFORM returns a scaled pseudorandom R8.
c
c  Discussion:
c
c    The pseudorandom number should be uniformly distributed
c    between A and B.
c
c  Modified:
c
c    06 January 2006
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    Input, double precision A, B, the limits of the interval.
c
c    Input/output, integer SEED, the "seed" value, which should NOT be 0.
c    On output, SEED has been updated.
c
c    Output, double precision R8_UNIFORM, a number strictly between A and B.
c
      implicit none

      double precision a
      double precision b
      integer i4_huge
      integer k
      double precision r8_uniform
      integer seed

      if ( seed .eq. 0 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'R8_UNIFORM - Fatal error!'
        write ( *, '(a)' ) '  Input value of SEED = 0.'
        stop
      end if

      k = seed / 127773

      seed = 16807 * ( seed - k * 127773 ) - k * 2836

      if ( seed .lt. 0 ) then
        seed = seed + i4_huge ( )
      end if
c
c  Although SEED can be represented exactly as a 32 bit integer,
c  it generally cannot be represented exactly as a 32 bit real number!
c
      r8_uniform = a + ( b - a ) * dble ( seed ) * 4.656612875D-10

      return
      end
      function r8_uniform_01 ( seed )

c*********************************************************************72
c
cc R8_UNIFORM_01 returns a unit pseudorandom R8.
c
c  Discussion:
c
c    This routine implements the recursion
c
c      seed = 16807 * seed mod ( 2**31 - 1 )
c      r8_uniform_01 = seed / ( 2**31 - 1 )
c
c    The integer arithmetic never requires more than 32 bits,
c    including a sign bit.
c
c    If the initial seed is 12345, then the first three computations are
c
c      Input     Output      R8_UNIFORM_01
c      SEED      SEED
c
c         12345   207482415  0.096616
c     207482415  1790989824  0.833995
c    1790989824  2035175616  0.947702
c
c  Modified:
c
c    11 August 2004
c
c  Author:
c
c    John Burkardt
c
c  Reference:
c
c    Paul Bratley, Bennett Fox, Linus Schrage,
c    A Guide to Simulation,
c    Springer Verlag, pages 201-202, 1983.
c
c    Pierre L'Ecuyer,
c    Random Number Generation,
c    in Handbook of Simulation,
c    edited by Jerry Banks,
c    Wiley Interscience, page 95, 1998.
c
c    Bennett Fox,
c    Algorithm 647:
c    Implementation and Relative Efficiency of Quasirandom
c    Sequence Generators,
c    ACM Transactions on Mathematical Software,
c    Volume 12, Number 4, pages 362-376, 1986.
c
c    Peter Lewis, Allen Goodman, James Miller,
c    A Pseudo-Random Number Generator for the System/360,
c    IBM Systems Journal,
c    Volume 8, pages 136-143, 1969.
c
c  Parameters:
c
c    Input/output, integer SEED, the "seed" value, which should NOT be 0.
c    On output, SEED has been updated.
c
c    Output, double precision R8_UNIFORM_01, a new pseudorandom variate,
c    strictly between 0 and 1.
c
      implicit none

      integer i4_huge
      integer k
      double precision r8_uniform_01
      integer seed

      if ( seed .eq. 0 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'R8_UNIFORM_01 - Fatal error!'
        write ( *, '(a)' ) '  Input value of SEED = 0.'
        stop
      end if

      k = seed / 127773

      seed = 16807 * ( seed - k * 127773 ) - k * 2836

      if ( seed .lt. 0 ) then
        seed = seed + i4_huge ( )
      end if
c
c  Although SEED can be represented exactly as a 32 bit integer,
c  it generally cannot be represented exactly as a 32 bit real number!
c
      r8_uniform_01 = dble ( seed ) * 4.656612875D-10

      return
      end
      subroutine timestamp ( )

c*********************************************************************72
c
cc TIMESTAMP prints out the current YMDHMS date as a timestamp.
c
c  Discussion:
c
c    This FORTRAN77 version is made available for cases where the
c    FORTRAN90 version cannot be used.
c
c  Modified:
c
c    12 January 2007
c
c  Author:
c
c    John Burkardt
c
c  Parameters:
c
c    None
c
      implicit none

      character * ( 8 ) ampm
      integer d
      character * ( 8 ) date
      integer h
      integer m
      integer mm
      character * ( 9 ) month(12)
      integer n
      integer s
      character * ( 10 ) time
      integer y

      save month

      data month /
     &  'January  ', 'February ', 'March    ', 'April    ', 
     &  'May      ', 'June     ', 'July     ', 'August   ', 
     &  'September', 'October  ', 'November ', 'December ' /

      call date_and_time ( date, time )

      read ( date, '(i4,i2,i2)' ) y, m, d
      read ( time, '(i2,i2,i2,1x,i3)' ) h, n, s, mm

      if ( h .lt. 12 ) then
        ampm = 'AM'
      else if ( h .eq. 12 ) then
        if ( n .eq. 0 .and. s .eq. 0 ) then
          ampm = 'Noon'
        else
          ampm = 'PM'
        end if
      else
        h = h - 12
        if ( h .lt. 12 ) then
          ampm = 'PM'
        else if ( h .eq. 12 ) then
          if ( n .eq. 0 .and. s .eq. 0 ) then
            ampm = 'Midnight'
          else
            ampm = 'AM'
          end if
        end if
      end if

      write ( *, 
     &  '(i2,1x,a,1x,i4,2x,i2,a1,i2.2,a1,i2.2,a1,i3.3,1x,a)' ) 
     &  d, month(m), y, h, ':', n, ':', s, '.', mm, ampm

      return
      end
