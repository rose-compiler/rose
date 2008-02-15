program Maxwell_3D
   implicit none
   integer, parameter :: NX = 256
   integer, parameter :: NY = 256
   integer, parameter :: NZ = 256
 
   real, parameter :: C2 = 2.997924562d8 * 2.997924562d8
   real, parameter :: PI = 3.14159265358979323846
   real, parameter :: MU = 4.000d-7 * PI

   real, parameter :: dx = 1.0
   real, parameter :: dy = 1.0
   real, parameter :: dz = 1.0

   real, dimension(NX,NY,NZ) :: Bx, By, Bz
   real, dimension(NX,NY,NZ) :: Ex, Ey, Ez
   real, dimension(NX,NY,NZ) :: Jx, Jy, Jz

   real :: dt = 1.0d-3
   integer :: it, nt = 1

   print *, "initializing ..."
   call start_clock()
   call initialize()
   call stop_clock()

   do it = 1, nt
      print *, "running timestep ", it, " ..."
      call start_clock()
      call solve(dt)
      call stop_clock()
   end do

   stop

contains


subroutine solve(dt)
   real, intent(in) :: dt

   !--------------------------------------------------------------------
   !           Magnetic Fields (Bx,By,Bz)
   !--------------------------------------------------------------------

   Bx = Bx + dt * (cshift(Ey,dim=3,shift=+1) - Ey) / dz                &
           - dt * (cshift(Ez,dim=2,shift=+1) - Ez) / dy

   By = By + dt * (cshift(Ez,dim=1,shift=+1) - Ez) / dx                &
           - dt * (cshift(Ex,dim=3,shift=+1) - Ex) / dz

   Bz = Bz + dt * (cshift(Ex,dim=2,shift=+1) - Ex) / dy                &
           - dt * (cshift(Ey,dim=1,shift=+1) - Ey) / dx

   !--------------------------------------------------------------------
   !           Electric Fields (Bx,By,Bz)
   !--------------------------------------------------------------------

   Ex = Ex + (C2 * dt) * (Bz - cshift(Bz,dim=2,shift=-1)) / dy         &
           - (C2 * dt) * (By - cshift(By,dim=3,shift=-1)) / dz         &
           - (C2 * dt) * (MU * Jx)

   Ey = Ey + (C2 * dt) * (Bx - cshift(Bx,dim=3,shift=-1)) / dz         &
           - (C2 * dt) * (Bz - cshift(Bz,dim=1,shift=-1)) / dx         &
           - (C2 * dt) * (MU * Jy)

   Ez = Ez + (C2 * dt) * (By - cshift(By,dim=1,shift=-1)) / dx         &
           - (C2 * dt) * (Bx - cshift(Bx,dim=2,shift=-1)) / dy         &
           - (C2 * dt) * (MU * Jz)

end subroutine solve


!
! TODO try to make initial conditions reasonable
!
subroutine initialize()
   Bx = 1.
   By = 0.0
   Bz = 0.0

   Ex = 0.0
   Ey = 0.0
   Ez = 0.0

   Jx = 3.0e-9
   Jy = 0.0
   Jz = 0.0
end subroutine initialize


end program Maxwell_3D
