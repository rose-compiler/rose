! Bug from Jean at ANL

subroutine foo(cyear, cmonth, cday, csecond, date_string)
 character(*) :: cyear, cmonth, cday, csecond, date_string
 write (date_string,1000) cyear, cmonth, cday, csecond
 1000 format (a4,:,'-',a2:,'-',a2,:,'-',a5) ! problem is the comma before the :
end subroutine

program p 
 implicit none
 character(5) :: cyear, cmonth, cday, csecond 
 character(20) :: date_string
 cyear='2010'
 cmonth='11'
 cday='11'
 csecond='11111'
 call foo(cyear, cmonth, cday, csecond, date_string)
 if (date_string .eq. "2010-11-11-11111") then 
   print *,'OK'
 else 
   print *,'FAILED datestring is ',date_string,'should be 2010-11-11-11111'
 end if
end program
