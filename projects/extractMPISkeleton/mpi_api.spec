;
; API specification for a basic subset of MPI
;
(api-spec MPI

  ( dep-types payload topology tag other )

  ( default_deptype other )

  ( (MPI_Init              2 )
    (MPI_Finalize          0 )
    (MPI_Abort             2 )
    (MPI_Comm_rank         2  (topology 1) )
    (MPI_Comm_size         2  (topology 1) )
    (MPI_Comm_split        4 )
    (MPI_Wait              2 )
    (MPI_Send              6  (payload 0 1 2) (topology 3) (tag 4) )
    (MPI_Recv              7  (payload 0 1 2) (topology 3) (tag 4) )
    (MPI_Irecv             7  (payload 0 1 2) (topology 3) (tag 4) )
    (MPI_Isend             7  (payload 0 1 2) (topology 3) (tag 4) )
    (MPI_Barrier           1 )
    (MPI_Bcast             5  (payload 0 1 2) (topology 3) )
    (MPI_Scatter           8  (payload 0 1 2 3 4 5) (topology 6) )
    (MPI_Scatterv          9  (payload 0 1 2 3 4 5 6) (topology 7) )
    (MPI_Allgather         7  (payload 0 1 2 3 4 5) )
    (MPI_Allgatherv        8  (payload 0 1 2 3 4 5 6) )
    (MPI_Gather            8  (payload 0 1 2 3 4 5) (topology 6) )
    (MPI_Gatherv           9  (payload 0 1 2 3 4 5 6) (topology 7) )
    (MPI_Reduce            7  (payload 0 1 2 3) (topology 5))
    (MPI_Scan              6  (payload 0 1 2 3) )
    (MPI_Allreduce         6  (payload 0 1 2 3) )
    (MPI_Sendrecv          12 (payload 0 1 2 5 6 7) (topology 3 8) (tag 4 9) )
    (MPI_Sendrecv_replace  9  (payload 0 1 2) (topology 3 5) (tag 4 6) )
    (MPI_Alltoall          7  (payload 0 1 2 3 4 5) )
    (MPI_Alltoallv         9  (payload 0 1 2 3 4 5 6 7 8) )
  )
)
