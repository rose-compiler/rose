      program labeltest
      do 100 i=1,100
      do 200 j=1,200
      goto 200
200   continue
      goto 100
100   continue
      end
