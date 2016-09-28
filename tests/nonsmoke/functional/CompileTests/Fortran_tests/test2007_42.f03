
integer, bind(c) :: somevar
bind(c, name="sharedcommon") :: /sharedcommon/
interface
   subroutine foobar() bind(c, name="foobar")
   end subroutine
end interface

end ! end of main program block

