   subroutine alter_char_len_ml10()
     integer, parameter:: singI = kind(0)
     integer(singI),parameter :: lnv = 32
     integer, parameter:: fullR = kind(0.d0)
     real(fullR) :: sign1,sign2,sign3,sign4,sign5,sign6, &
                    epx1,epx2,epx3,epx4,epx5,epx6,aux
     common/aux14/ &
      sign1(lnv),sign2(lnv),sign3(lnv),sign4(lnv),sign5(lnv),sign6(lnv), &
      epx1(lnv),epx2(lnv),epx3(lnv),epx4(lnv),epx5(lnv),epx6(lnv), &
      aux(lnv,4)
   end subroutine alter_char_len_ml10
