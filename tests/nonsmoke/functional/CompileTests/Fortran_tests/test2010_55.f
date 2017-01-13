C The bug is that the function type is not unparsed...
       logical function ifcor(n1)
       include 'test2010_55.h'

       integer n1
       ifcor=.false.

       return
       end

       subroutine check_refine()
       logical ifcor
       return
       end
