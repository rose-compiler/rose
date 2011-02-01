module sky
  private
  public  :: sphere
  real,        dimension(5) :: bottom
  character*6, dimension(5) :: layer
  data bottom /    0.0,     5.0,    30.0,    50.0,   300.0 /
  data layer  /' Tropo','Strato','  Meso','  Iono','   Exo'/
contains
  character*6 function sphere ( miles ) result ( zone )
    real, intent(IN) :: miles
    do i = 1, size(bottom), 1
       if ( miles .ge. bottom(i) ) zone = layer(i)
    end do
  end function sphere
end module sky

program atmosphere
  use sky
  real, dimension(3) :: altitude = (/ 1.2, 45.6, 789.0 /)
  if(sphere(altitude(1)).EQ." Tropo" .AND. &
       sphere(altitude(2)).EQ."  Meso" .AND. &
       sphere(altitude(3)).EQ."   Exo") then
     print *, "OK"
  else
     print *, "failed"
  end if
end program atmosphere
