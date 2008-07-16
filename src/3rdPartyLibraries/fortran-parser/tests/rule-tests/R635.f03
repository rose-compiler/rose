! Test deallocate-stmt, which is:
!      deallocate-stmt  is  DEALLOCATE ( allocate-object-list 
!                             [, dealloc-opt-list] )
!
! Tested separately is: allocate-object-list (R629).
deallocate(a)
deallocate(a,stat=i)
deallocate(a, stat=i, errmsg=m)

end
