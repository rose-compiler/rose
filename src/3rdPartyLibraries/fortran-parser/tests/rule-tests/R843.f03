! Test cycle-stmt
!      cycle-stmt  is  CYCLE [ do-construct-name ]
do i = 1, 10
   x = 2
   cycle
end do 

my_do: do i = 1, 10
   x = 2
   cycle my_do
end do my_do

end

