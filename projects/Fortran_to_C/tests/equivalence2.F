      program equivalence2
        real*4 a(4,16), b(4,4)
        real*4 c
        equivalence(a,b)
        b(2,1) = 1.
        c = b(2,1) + 5.
      end
