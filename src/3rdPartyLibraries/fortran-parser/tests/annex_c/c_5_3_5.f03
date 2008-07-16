sum = 0.0
read(iun) n
do 80, l = 1, n
   read(iun) iqual, m, array(1:m)
   if(iqual < iqual_min) m = 0
   do 80 i = 1,m
      call calculate(array(i), result)
      if(result < 0.) cycle
      sum = sum + result
      if(sum > sum_max) goto 81
80    continue
81    continue

end

