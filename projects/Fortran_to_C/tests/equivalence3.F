      program equivalence3
        real*4 a(4,16), b(4,4)
        real*4 c
        equivalence(a(1,4),b)
        b(2,1) = 1.
        c = b(2,1) + 5.
      end
