      program equivalence4
        real*4 a(4), b(4,16)
        equivalence(a,b(1,4))
        a(1) = 5.
      end
