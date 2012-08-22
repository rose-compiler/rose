subroutine foo(i,j)
implicit none
common /block/ blocki
save /block/
integer :: blocki=0,i,j
blocki=blocki+i
j=blocki
end subroutine 


program p 
integer j
call foo(2,j)
call foo(2,j)
if (j == 4) then 
print *, 'OK'
else 
print *,j
end if
end program
