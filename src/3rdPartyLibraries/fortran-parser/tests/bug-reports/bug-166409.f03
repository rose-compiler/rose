! This code illustrated both bug #166409 and #173230.
select type (p_or_c)
class is (point)
   print *, 'class is point'
type is (integer(c_int))
   print *, 'type is integer'
end select

end
