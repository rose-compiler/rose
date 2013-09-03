      program equivalence4
        real*4 a(4,16), b
        real*4 c
        equivalence(a(1,4),b)
        b = 1.
        c = b + 5.
      end
