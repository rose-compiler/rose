character (len=*) function id (c)
             character (len=*), intent(in) :: c
             id= c
 end function id
   
 program main
		 character (len=3), external :: id
		 character (len=3) :: start
		 character (len=4) :: first
		 character (len=5) :: second
		 character (len=12) :: third, fourth, fifth
		 integer :: myindex
		 start="abc"
		 first=id(start)//"x"
		 second=first//trim("y  ")
		 third=repeat(second,2)//"  "
		 fourth=adjustr(third)
		 fifth=adjustl(fourth)
                 
		 if(fifth.NE.third) then
		   print *, "failed"
		 else
		   myindex=index(fourth, "abc")
		   if(myindex.NE.3) then
		     print *, "failed"
		   else
             print *, "OK"
           end if			 

         end if
       end	
