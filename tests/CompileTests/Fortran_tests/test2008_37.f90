module test2008_37_convert_mod
implicit none
contains
function cels_from_fahr(degrees_fahr) result(degrees_cels)
real, intent(in) :: degrees_fahr
real             :: degrees_cels
degrees_cels = (degrees_fahr-32)/1.8
end function cels_from_fahr
end module test2008_37_convert_mod

program xtemperature
use test2008_37_convert_mod, only: cels_from_fahr
real    :: deg
integer :: i
write (*,"(2a10)") "degrees_F","degrees_C"
do i=12,100,20
   deg = real(i)
   write (*,"(2f10.1)") deg,cels_from_fahr(deg)
end do
end program xtemperature
