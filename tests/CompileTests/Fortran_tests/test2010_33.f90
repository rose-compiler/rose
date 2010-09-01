! Bug reported by Rice 8/27/2010.

module  m1
 integer,parameter :: xx= 2
end module

module  m2
 integer,parameter :: yy= 2
end module

program foo
 use m1
 use m2

! This was unparsed as "INTEGER :: xyz"
integer(xx+yy*2+2) xyz
end
