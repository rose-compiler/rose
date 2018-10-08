!! R859 sync-stat
!    is STAT = stat-variable
!    or ERRMSG = errmsg-variable
!
integer :: i
character :: msg

13   sync all (stat=i, ERRMSG=msg)

end

