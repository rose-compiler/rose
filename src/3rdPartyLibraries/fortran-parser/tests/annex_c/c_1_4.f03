! combined the code in C.1.4 into a module to make it a program unit to parse
module c_1_4
  type, abstract :: drawable_object
     real, dimension(3) :: rgb_color=(/1.0, 1.0, 1.0/) ! White
     real, dimension(2) :: position=(/0.0,0.0/) ! Centroid
   contains
     procedure(render_x), pass(object), deferred :: render
  end type drawable_object

  abstract interface
     subroutine render_x(object, window)
       class(drawable_object), intent(in) :: object
       class(x_window), intent(inout) :: window
     end subroutine render_x
  end interface

  type, extends(drawable_object) :: drawable_triangle ! Not ABSTRACT
     real, dimension(2,3) :: vertices ! In relation to centroid
   contains
     procedure, pass(object) :: render=>render_triangle_x
  end type drawable_triangle

contains
  subroutine render_triangle_x(object, window)
    class(drawable_triangle), intent(in) :: object
    class(x_window), intent(inout) :: window
  end subroutine render_triangle_x
end module c_1_4



