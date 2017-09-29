!! R863 lock-stmt
!    is LOCK ( lock-variable [ , lock-stat-list ] )
!
logical :: flag
integer :: i, a_lock_variable

   LOCK (a_lock_variable)
13 LOCK (msg, ACQUIRED_LOCK=flag, STAT=i)

end
