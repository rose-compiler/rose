!! R865 unlock-stmt
!    is UNLOCK ( lock-variable [ , sync-stat-list ] )
!
complex :: comp
character err
integer :: a_lock_variable

999 UNLOCK (comp)
    unlock(a_lock_variable, ERRMSG   =err)

end
