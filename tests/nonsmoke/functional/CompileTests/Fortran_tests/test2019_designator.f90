   subroutine reset_peak_values
!

     implicit none
!
     type mat_peak_variable_T
!
          character(100),dimension(:),pointer,contiguous :: &
                                                 names => Null() !# peak names
     end type mat_peak_variable_T
!
     real,dimension(:,:,:),pointer :: peaks
     type(mat_peak_variable_T),pointer    :: epeak,ipeak
!
!
           if    (ipeak%names(1)(1:3)=="max") then
             peaks(:,:,1)= 0.
           endif
    end subroutine reset_peak_values


