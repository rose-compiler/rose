! R624.f03 alloc-opt
allocate(array(5), errmsg=a)
! NOTE 6.17
ALLOCATE (X (N), B (-3 : M, 0:9), STAT = IERR_ALLOC)
! NOTE 6.19
ALLOCATE (NEW, SOURCE=OLD)
10 allocate(new, source=old,errmsg=a, stat=ierr)
end
