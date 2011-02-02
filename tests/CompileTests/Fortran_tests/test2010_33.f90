! Bug reported by Rice 8/27/2010.

module  m133
 integer,parameter :: xx= 2
end module

module  m233
 integer,parameter :: yy= 2
end module

program foo
 use m133
 use m233

! This was unparsed as "INTEGER :: xyz"
integer(xx+yy*2+2) xyz
end
