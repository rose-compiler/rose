   integer multg
   do mul = 1, multg
      if( 1.eq.0 ) then
          write(nfile(1),'(a23,i3,a3,100a1)') &
     &      ' ---- multigrid level (', mul, ' ) ',('-',i=1,40)
          write(nfile(2),'(a23,i3,a3,100a1)') &
     &      ' ---- multigrid level (', mul, ' ) ',('-',i=1,40)
      endif
   enddo
end
