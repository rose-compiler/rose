   integer istat, i, lovelo, lovskp

 ! if( istat == 0 ) read(4,*,iostat=istat) lovelo, lovskp

 ! Working statement...
 ! read(4,*,iostat=istat) lovelo, lovskp
 ! Unparsed as: READ (UNIT=4, IOSTAT=istat, FMT=*) lovelo,lovskp

   if( i == 0 ) read(4,*,iostat=istat) lovelo, lovskp
end

