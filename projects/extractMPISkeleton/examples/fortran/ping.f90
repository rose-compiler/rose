program ping
  include 'mpif.h'

  integer numtasks, rank, dest, source, count, tag, ierr
  integer stat(MPI_STATUS_SIZE)
  character inmsg, outmsg
  outmsg = 'x'
  tag = 1

  call MPI_INIT(ierr)
  call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
  call MPI_COMM_SIZE(MPI_COMM_WORLD, numtasks, ierr)

  if (rank .eq. 0) then
     dest = 1
     source = 1
     call MPI_SEND(outmsg, 1, MPI_CHARACTER, dest, tag, MPI_COMM_WORLD, ierr)
     call MPI_RECV(inmsg, 1, MPI_CHARACTER, source, tag, MPI_COMM_WORLD, stat, ierr)

  else if (rank .eq. 1) then
     dest = 0
     source = 0
     call MPI_RECV(inmsg, 1, MPI_CHARACTER, source, tag, MPI_COMM_WORLD, stat, err)
     call MPI_SEND(outmsg, 1, MPI_CHARACTER, dest, tag, MPI_COMM_WORLD, err)
  endif

  call MPI_GET_COUNT(stat, MPI_CHARACTER, count, ierr)
  print *, 'Task ',rank,': Received', count, 'char(s) from task', stat(MPI_SOURCE), 'with tag',stat(MPI_TAG)

  call MPI_FINALIZE(ierr)

end program ping
