      subroutine divnod( ndata, node, myid, ndnod1, ndnod2, ndnod )
!-----------------------------------------------------------------------
! ( input )
!     ndata ...... total No. of data points
!     node  ...... No. of nodes
!     myid  ...... my ID
!
! ( output )
!     ndnod1 ...... starting pointer
!     ndnod2 ...... ending   pointer
!     ndnod  ...... No. of data points
!-----------------------------------------------------------------------

!      ndnod  = ndata/node
!      ndnod1 = ndnod*myid
!      msrest = mod(ndata,node) - 1
!      if( myid.le.msrest ) then
!          ndnod1 = ndnod1 + myid
!          ndnod  = ndnod + 1
!        else
!          ndnod1 = ndnod1 + msrest + 1
!      endif
!      ndnod1 = ndnod1 + 1
!      ndnod2 = ndnod1 + ndnod - 1

      ndnod2  = (myid+1)*ndata/node + 1
      if( mod((myid+1)*ndata,node).eq.0 ) ndnod2 = ndnod2 - 1
      if( myid.eq.0 ) then
          ndnod1 = 1
        else
          ndnod1  = myid*ndata/node + 2
          if( mod(myid*ndata,node).eq.0 ) ndnod1 = ndnod1 - 1
      endif
      ndnod = ndnod2 - ndnod1 + 1

      return
      end

