! Test from gfortran bounds_check_5.f90

!   INTEGER, POINTER :: p(:)

   TYPE particle_type
      INTEGER, POINTER :: p(:)
   END TYPE particle_type
   TYPE(particle_type), POINTER  :: t(:)

  integer :: i
  logical :: f
  i = 1
  allocate(t(1))
  allocate(t(1)%p(0))
  f = associated(t(i)%p,t(i)%p)
end
