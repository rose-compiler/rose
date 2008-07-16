sum = 0.0
read(iun) n
outer: do l = 1, n
   read(iun) iqual, m, array(1:m)
   if(iqual < iqual_min) cycle outer
   inner: do 40 i = 1,m
     call calculate (array(i), result)
     if(result < 0.0) cycle
     sum = sum + result
     if(sum > sum_max) exit outer
40 end do inner
end do outer
end

