!! R862 sync-memory-stmt
!    is SYNC MEMORY [ ( [ sync-stat-list ] ) ]
!
character :: msg
integer :: me

   SYNC MEMORY
   sync memory (ERRMSG=msg)
 3 sync Memory ( STAT=me, ERRmsg = msg )

end
