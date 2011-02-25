program main
integer n
integer x
n=3
select case(n)
case(0)
x=1
case(1:5)
x=2
case default
x=3
end select
if( x.EQ.2) then
  print *, "OK"
else
 print *, "failed",X
end if   
end program main
