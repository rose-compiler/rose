!! R860 sync-images-stmt
!    is SYNC IMAGES ( image-set [ , sync-stat-list ] )
!
character :: msg
integer :: me

   SYNC IMAGES (1)
   sync images (*)
 1 sync IMAGes (me+1, stat=me, ERRmsg = msg)
end
