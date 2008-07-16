! this is the third example in C.3.1
type point
   real :: x, y
end type point

type, extends(point) :: color_point
   ! components x and y, and component name point, inherited from parent
   integer :: color
end type color_point

type(point) :: pv = point(1.0, 2.0)
type(color_point) :: cpv = color_point(pv, 3) ! nested form constructor

print *, cpv%point
print *, cpv%point%x, cpv%point%y
print *, cpv%x, cpv%y

end
