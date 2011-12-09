character*6 function f (x) result ( z )
 character :: c*3 = "abc"
 z = c
 c = z
end
