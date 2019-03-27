SUBROUTINE sub1(fff,e_s,var1,var2,var3)
IMPLICIT NONE
INTEGER, PARAMETER :: vecsize = 1024576
REAL(kind=8), DIMENSION(vecsize,3,10), TARGET :: glo1
REAL(kind=8), DIMENSION(vecsize) :: glo3
REAL(kind=8) :: px1(vecsize), px2(vecsize), px3(vecsize), px4(vecsize), px5(vecsize), px6(vecsize), px7(vecsize), px8(vecsize), py1(vecsize), py2(vecsize), py3(vecsize), py4(vecsize), py5(vecsize), py6(vecsize), py7(vecsize), py8(vecsize), pz1(vecsize), pz2(vecsize), pz3(vecsize), pz4(vecsize), pz5(vecsize), pz6(vecsize), pz7(vecsize), pz8(vecsize)
REAL(kind=8), DIMENSION(vecsize,3,10) :: glo2
REAL(kind=8), DIMENSION(vecsize) :: glo4
REAL(kind=8), DIMENSION(vecsize,3,10) :: glo6
REAL(kind=8), DIMENSION(vecsize) :: glo5
REAL(kind=8), DIMENSION(vecsize) :: glo7
INTEGER :: vecn
REAL(kind=8) :: gs1, gs2
REAL(kind=8) :: e_s, var1, var2, var3
REAL(kind=8), DIMENSION(12,vecsize) :: fff
INTEGER :: i, j, k
REAL(kind=8) :: scal1, scal2, scal3, scal4
!$rose scalarization
REAL(kind=8) :: fac, sv11, sv21, sv31, sv41, sv51, sv61, sv71, sv81, sv12, sv22, sv32, sv42, sv52, sv62, sv72, sv82, sv13, sv23, sv33, sv43, sv53, sv63, sv73, sv83, sv14, sv24, sv34, sv44, sv54, sv64, sv74, sv84, qqx1, qqx2, qqx3, qqx4, qqy1, qqy2, qqy3, qqy4, qqz1, qqz2, qqz3, qqz4, hx1, hx2, hx3, hx4, hy1, hy2, hy3, hy4, hz1, hz2, hz3, hz4, hx1v, hx2v, hx3v, hx4v, hy1v, hy2v, hy3v, hy4v, hz1v, hz2v, hz3v, hz4v, qqwk
IF (var1 <= 0.0d0 .AND. var2 <= 0.0d0) THEN
DO k = 1, 8
DO j = 1, 3
DO i = 1, vecn
glo6(i,j,k) = 0.0d0
END DO
END DO
END DO
RETURN
END IF
scal4 = 0.0d0
DO i = 1, vecn
hx1 = glo1(i,1,3) - glo1(i,1,4) - glo1(i,1,7) + glo1(i,1,8)
hx2 = glo1(i,1,2) - glo1(i,1,3) - glo1(i,1,5) + glo1(i,1,8)
hx3 = glo1(i,1,1) - glo1(i,1,4) - glo1(i,1,6) + glo1(i,1,7)
hx4 = glo1(i,1,1) - glo1(i,1,2) - glo1(i,1,5) + glo1(i,1,6)
hy1 = glo1(i,2,3) - glo1(i,2,4) - glo1(i,2,7) + glo1(i,2,8)
hy2 = glo1(i,2,2) - glo1(i,2,3) - glo1(i,2,5) + glo1(i,2,8)
hy3 = glo1(i,2,1) - glo1(i,2,4) - glo1(i,2,6) + glo1(i,2,7)
hy4 = glo1(i,2,1) - glo1(i,2,2) - glo1(i,2,5) + glo1(i,2,6)
hz1 = glo1(i,3,3) - glo1(i,3,4) - glo1(i,3,7) + glo1(i,3,8)
hz2 = glo1(i,3,2) - glo1(i,3,3) - glo1(i,3,5) + glo1(i,3,8)
hz3 = glo1(i,3,1) - glo1(i,3,4) - glo1(i,3,6) + glo1(i,3,7)
hz4 = glo1(i,3,1) - glo1(i,3,2) - glo1(i,3,5) + glo1(i,3,6)
scal3 = 1.0d0 / glo4(i)
qqx1 = (hx3 - hx2) * scal3
qqx2 = (hx3 + hx2) * scal3
qqx3 = (hx4 - hx1) * scal3
qqx4 = (hx4 + hx1) * scal3
qqy1 = (hy3 - hy2) * scal3
qqy2 = (hy3 + hy2) * scal3
qqy3 = (hy4 - hy1) * scal3
qqy4 = (hy4 + hy1) * scal3
qqz1 = (hz3 - hz2) * scal3
qqz2 = (hz3 + hz2) * scal3
qqz3 = (hz4 - hz1) * scal3
qqz4 = (hz4 + hz1) * scal3
sv11 = 1.0d0 - qqx1 * px1(i) - qqy1 * py1(i) - qqz1 * pz1(i)
sv21 = - 1.0d0 - qqx1 * px2(i) - qqy1 * py2(i) - qqz1 * pz2(i)
sv31 = 1.0d0 - qqx1 * px3(i) - qqy1 * py3(i) - qqz1 * pz3(i)
sv41 = - 1.0d0 - qqx1 * px4(i) - qqy1 * py4(i) - qqz1 * pz4(i)
sv51 = 1.0d0 - qqx1 * px5(i) - qqy1 * py5(i) - qqz1 * pz5(i)
sv61 = - 1.0d0 - qqx1 * px6(i) - qqy1 * py6(i) - qqz1 * pz6(i)
sv71 = 1.0d0 - qqx1 * px7(i) - qqy1 * py7(i) - qqz1 * pz7(i)
sv81 = - 1.0d0 - qqx1 * px8(i) - qqy1 * py8(i) - qqz1 * pz8(i)
sv12 = 1.0d0 - qqx2 * px1(i) - qqy2 * py1(i) - qqz2 * pz1(i)
sv22 = 1.0d0 - qqx2 * px2(i) - qqy2 * py2(i) - qqz2 * pz2(i)
sv32 = - 1.0d0 - qqx2 * px3(i) - qqy2 * py3(i) - qqz2 * pz3(i)
sv42 = - 1.0d0 - qqx2 * px4(i) - qqy2 * py4(i) - qqz2 * pz4(i)
sv52 = - 1.0d0 - qqx2 * px5(i) - qqy2 * py5(i) - qqz2 * pz5(i)
sv62 = - 1.0d0 - qqx2 * px6(i) - qqy2 * py6(i) - qqz2 * pz6(i)
sv72 = 1.0d0 - qqx2 * px7(i) - qqy2 * py7(i) - qqz2 * pz7(i)
sv82 = 1.0d0 - qqx2 * px8(i) - qqy2 * py8(i) - qqz2 * pz8(i)
sv13 = 1.0d0 - qqx3 * px1(i) - qqy3 * py1(i) - qqz3 * pz1(i)
sv23 = - 1.0d0 - qqx3 * px2(i) - qqy3 * py2(i) - qqz3 * pz2(i)
sv33 = - 1.0d0 - qqx3 * px3(i) - qqy3 * py3(i) - qqz3 * pz3(i)
sv43 = 1.0d0 - qqx3 * px4(i) - qqy3 * py4(i) - qqz3 * pz4(i)
sv53 = - 1.0d0 - qqx3 * px5(i) - qqy3 * py5(i) - qqz3 * pz5(i)
sv63 = 1.0d0 - qqx3 * px6(i) - qqy3 * py6(i) - qqz3 * pz6(i)
sv73 = 1.0d0 - qqx3 * px7(i) - qqy3 * py7(i) - qqz3 * pz7(i)
sv83 = - 1.0d0 - qqx3 * px8(i) - qqy3 * py8(i) - qqz3 * pz8(i)
sv14 = 1.0d0 - qqx4 * px1(i) - qqy4 * py1(i) - qqz4 * pz1(i)
sv24 = - 1.0d0 - qqx4 * px2(i) - qqy4 * py2(i) - qqz4 * pz2(i)
sv34 = 1.0d0 - qqx4 * px3(i) - qqy4 * py3(i) - qqz4 * pz3(i)
sv44 = - 1.0d0 - qqx4 * px4(i) - qqy4 * py4(i) - qqz4 * pz4(i)
sv54 = - 1.0d0 - qqx4 * px5(i) - qqy4 * py5(i) - qqz4 * pz5(i)
sv64 = 1.0d0 - qqx4 * px6(i) - qqy4 * py6(i) - qqz4 * pz6(i)
sv74 = - 1.0d0 - qqx4 * px7(i) - qqy4 * py7(i) - qqz4 * pz7(i)
sv84 = 1.0d0 - qqx4 * px8(i) - qqy4 * py8(i) - qqz4 * pz8(i)
hx1v = glo2(i,1,1) * sv11 + glo2(i,1,2) * sv21 + glo2(i,1,3) * sv31 + glo2(i,1,4) * sv41 + glo2(i,1,5) * sv51 + glo2(i,1,6) * sv61 + glo2(i,1,7) * sv71 + glo2(i,1,8) * sv81
hx2v = glo2(i,1,1) * sv12 + glo2(i,1,2) * sv22 + glo2(i,1,3) * sv32 + glo2(i,1,4) * sv42 + glo2(i,1,5) * sv52 + glo2(i,1,6) * sv62 + glo2(i,1,7) * sv72 + glo2(i,1,8) * sv82
hx3v = glo2(i,1,1) * sv13 + glo2(i,1,2) * sv23 + glo2(i,1,3) * sv33 + glo2(i,1,4) * sv43 + glo2(i,1,5) * sv53 + glo2(i,1,6) * sv63 + glo2(i,1,7) * sv73 + glo2(i,1,8) * sv83
hx4v = glo2(i,1,1) * sv14 + glo2(i,1,2) * sv24 + glo2(i,1,3) * sv34 + glo2(i,1,4) * sv44 + glo2(i,1,5) * sv54 + glo2(i,1,6) * sv64 + glo2(i,1,7) * sv74 + glo2(i,1,8) * sv84
hy1v = glo2(i,2,1) * sv11 + glo2(i,2,2) * sv21 + glo2(i,2,3) * sv31 + glo2(i,2,4) * sv41 + glo2(i,2,5) * sv51 + glo2(i,2,6) * sv61 + glo2(i,2,7) * sv71 + glo2(i,2,8) * sv81
hy2v = glo2(i,2,1) * sv12 + glo2(i,2,2) * sv22 + glo2(i,2,3) * sv32 + glo2(i,2,4) * sv42 + glo2(i,2,5) * sv52 + glo2(i,2,6) * sv62 + glo2(i,2,7) * sv72 + glo2(i,2,8) * sv82
hy3v = glo2(i,2,1) * sv13 + glo2(i,2,2) * sv23 + glo2(i,2,3) * sv33 + glo2(i,2,4) * sv43 + glo2(i,2,5) * sv53 + glo2(i,2,6) * sv63 + glo2(i,2,7) * sv73 + glo2(i,2,8) * sv83
hy4v = glo2(i,2,1) * sv14 + glo2(i,2,2) * sv24 + glo2(i,2,3) * sv34 + glo2(i,2,4) * sv44 + glo2(i,2,5) * sv54 + glo2(i,2,6) * sv64 + glo2(i,2,7) * sv74 + glo2(i,2,8) * sv84
hz1v = glo2(i,3,1) * sv11 + glo2(i,3,2) * sv21 + glo2(i,3,3) * sv31 + glo2(i,3,4) * sv41 + glo2(i,3,5) * sv51 + glo2(i,3,6) * sv61 + glo2(i,3,7) * sv71 + glo2(i,3,8) * sv81
hz2v = glo2(i,3,1) * sv12 + glo2(i,3,2) * sv22 + glo2(i,3,3) * sv32 + glo2(i,3,4) * sv42 + glo2(i,3,5) * sv52 + glo2(i,3,6) * sv62 + glo2(i,3,7) * sv72 + glo2(i,3,8) * sv82
hz3v = glo2(i,3,1) * sv13 + glo2(i,3,2) * sv23 + glo2(i,3,3) * sv33 + glo2(i,3,4) * sv43 + glo2(i,3,5) * sv53 + glo2(i,3,6) * sv63 + glo2(i,3,7) * sv73 + glo2(i,3,8) * sv83
hz4v = glo2(i,3,1) * sv14 + glo2(i,3,2) * sv24 + glo2(i,3,3) * sv34 + glo2(i,3,4) * sv44 + glo2(i,3,5) * sv54 + glo2(i,3,6) * sv64 + glo2(i,3,7) * sv74 + glo2(i,3,8) * sv84
END DO
IF (var1 > 0.0d0) THEN
scal1 = .25d0 * var1 * gs2 * var3
DO i = 1, vecn
scal2 = px1(i) ** 2 + px2(i) ** 2 + px3(i) ** 2 + px4(i) ** 2 + py1(i) ** 2 + py2(i) ** 2 + py3(i) ** 2 + py4(i) ** 2 + pz1(i) ** 2 + pz2(i) ** 2 + pz3(i) ** 2 + pz4(i) ** 2 + px5(i) ** 2 + px6(i) ** 2 + px7(i) ** 2 + px8(i) ** 2 + py5(i) ** 2 + py6(i) ** 2 + py7(i) ** 2 + py8(i) ** 2 + pz5(i) ** 2 + pz6(i) ** 2 + pz7(i) ** 2 + pz8(i) ** 2
fac = scal1 * scal2 / glo4(i)
fff(1,i) = fff(1,i) + fac * hx1v
fff(2,i) = fff(2,i) + fac * hx2v
fff(3,i) = fff(3,i) + fac * hx3v
fff(4,i) = fff(4,i) + fac * hx4v
fff(5,i) = fff(5,i) + fac * hy1v
fff(6,i) = fff(6,i) + fac * hy2v
fff(7,i) = fff(7,i) + fac * hy3v
fff(8,i) = fff(8,i) + fac * hy4v
fff(9,i) = fff(9,i) + fac * hz1v
fff(10,i) = fff(10,i) + fac * hz2v
fff(11,i) = fff(11,i) + fac * hz3v
fff(12,i) = fff(12,i) + fac * hz4v
END DO
IF (var2 <= 0.0d0) THEN
DO i = 1, vecn
hx1 = fff(1,i)
hx2 = fff(2,i)
hx3 = fff(3,i)
hx4 = fff(4,i)
hy1 = fff(5,i)
hy2 = fff(6,i)
hy3 = fff(7,i)
hy4 = fff(8,i)
hz1 = fff(9,i)
hz2 = fff(10,i)
hz3 = fff(11,i)
hz4 = fff(12,i)
END DO
ELSE
scal1 = -0.25d0 * var2 * gs1
DO i = 1, vecn
fac = glo7(i) * scal1 * glo3(i) * glo3(i) / glo5(i)
hx1 = fac * hx1v + fff(1,i)
hx2 = fac * hx2v + fff(2,i)
hx3 = fac * hx3v + fff(3,i)
hx4 = fac * hx4v + fff(4,i)
hy1 = fac * hy1v + fff(5,i)
hy2 = fac * hy2v + fff(6,i)
hy3 = fac * hy3v + fff(7,i)
hy4 = fac * hy4v + fff(8,i)
hz1 = fac * hz1v + fff(9,i)
hz2 = fac * hz2v + fff(10,i)
hz3 = fac * hz3v + fff(11,i)
hz4 = fac * hz4v + fff(12,i)
END DO
END IF
ELSE IF (var2 > 0.0d0) THEN
scal1 = -0.25d0 * var2 * gs1
DO i = 1, vecn
fac = glo7(i) * scal1 * glo3(i) * glo3(i) / glo5(i)
hx1 = fac * hx1v
hx2 = fac * hx2v
hx3 = fac * hx3v
hx4 = fac * hx4v
hy1 = fac * hy1v
hy2 = fac * hy2v
hy3 = fac * hy3v
hy4 = fac * hy4v
hz1 = fac * hz1v
hz2 = fac * hz2v
hz3 = fac * hz3v
hz4 = fac * hz4v
END DO
END IF
DO i = 1, vecn
glo6(i,1,1) = -hx1 * sv11 - hx2 * sv12 - hx3 * sv13 - hx4 * sv14
glo6(i,1,2) = -hx1 * sv21 - hx2 * sv22 - hx3 * sv23 - hx4 * sv24
glo6(i,1,3) = -hx1 * sv31 - hx2 * sv32 - hx3 * sv33 - hx4 * sv34
glo6(i,1,4) = -hx1 * sv41 - hx2 * sv42 - hx3 * sv43 - hx4 * sv44
glo6(i,1,5) = -hx1 * sv51 - hx2 * sv52 - hx3 * sv53 - hx4 * sv54
glo6(i,1,6) = -hx1 * sv61 - hx2 * sv62 - hx3 * sv63 - hx4 * sv64
glo6(i,1,7) = -hx1 * sv71 - hx2 * sv72 - hx3 * sv73 - hx4 * sv74
glo6(i,1,8) = -hx1 * sv81 - hx2 * sv82 - hx3 * sv83 - hx4 * sv84
glo6(i,2,1) = -hy1 * sv11 - hy2 * sv12 - hy3 * sv13 - hy4 * sv14
glo6(i,2,2) = -hy1 * sv21 - hy2 * sv22 - hy3 * sv23 - hy4 * sv24
glo6(i,2,3) = -hy1 * sv31 - hy2 * sv32 - hy3 * sv33 - hy4 * sv34
glo6(i,2,4) = -hy1 * sv41 - hy2 * sv42 - hy3 * sv43 - hy4 * sv44
glo6(i,2,5) = -hy1 * sv51 - hy2 * sv52 - hy3 * sv53 - hy4 * sv54
glo6(i,2,6) = -hy1 * sv61 - hy2 * sv62 - hy3 * sv63 - hy4 * sv64
glo6(i,2,7) = -hy1 * sv71 - hy2 * sv72 - hy3 * sv73 - hy4 * sv74
glo6(i,2,8) = -hy1 * sv81 - hy2 * sv82 - hy3 * sv83 - hy4 * sv84
glo6(i,3,1) = -hz1 * sv11 - hz2 * sv12 - hz3 * sv13 - hz4 * sv14
glo6(i,3,2) = -hz1 * sv21 - hz2 * sv22 - hz3 * sv23 - hz4 * sv24
glo6(i,3,3) = -hz1 * sv31 - hz2 * sv32 - hz3 * sv33 - hz4 * sv34
glo6(i,3,4) = -hz1 * sv41 - hz2 * sv42 - hz3 * sv43 - hz4 * sv44
glo6(i,3,5) = -hz1 * sv51 - hz2 * sv52 - hz3 * sv53 - hz4 * sv54
glo6(i,3,6) = -hz1 * sv61 - hz2 * sv62 - hz3 * sv63 - hz4 * sv64
glo6(i,3,7) = -hz1 * sv71 - hz2 * sv72 - hz3 * sv73 - hz4 * sv74
glo6(i,3,8) = -hz1 * sv81 - hz2 * sv82 - hz3 * sv83 - hz4 * sv84
qqwk = hx1 * hx1v + hx2 * hx2v + hx3 * hx3v + hx4 * hx4v + hy1 * hy1v + hy2 * hy2v + hy3 * hy3v + hy4 * hy4v + hz1 * hz1v + hz2 * hz2v + hz3 * hz3v + hz4 * hz4v
scal4 = scal4 + qqwk
END DO
e_s = e_s - gs2 * scal4
END SUBROUTINE sub1

