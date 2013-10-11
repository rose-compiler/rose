      program equivalence4
        real*4 a(4,16), b(4,4)
        equivalence(a(1,4),b(1,1))
        b(1,1) = 5.
        a(1,10) = 10.
      end
