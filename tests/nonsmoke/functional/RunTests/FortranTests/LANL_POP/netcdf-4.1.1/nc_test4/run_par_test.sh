mpd&
mpiexec -n 4 ./tst_mpi_parallel
mpiexec -n 4 ./tst_parallel
mpiexec -n 16 ./tst_parallel3
echo "num_proc   time(s)  write_rate(B/s)"
mpiexec -n 1 ./tst_parallel4
mpiexec -n 2 ./tst_parallel4
mpiexec -n 4 ./tst_parallel4
mpiexec -n 8 ./tst_parallel4
mpiexec -n 16 ./tst_parallel4
mpiexec -n 32 ./tst_parallel4
mpiexec -n 64 ./tst_parallel4
mpiexec -n 4 ./tst_nc4perf
