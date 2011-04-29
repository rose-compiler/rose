C      default logical type assumed below
C      OpenMP Fortran API v3.0
      include 'omp_lib_kinds.h'
      integer openmp_version
      parameter ( openmp_version = 200805 )

      external omp_set_num_threads
      external omp_get_num_threads
      integer omp_get_num_threads
      external omp_get_max_threads
      integer omp_get_max_threads
      external omp_get_thread_num
      integer omp_get_thread_num
      external omp_get_num_procs
      integer omp_get_num_procs
      external omp_in_parallel
      logical omp_in_parallel
      external omp_set_dynamic
      external omp_get_dynamic
      logical omp_get_dynamic
      external omp_set_nested
      external omp_get_nested
      logical omp_get_nested
      external omp_set_schedule
      external omp_get_schedule
      external omp_get_thread_limit
      integer omp_get_thread_limit
      external omp_set_max_active_levels
      external omp_get_max_active_levels
      integer omp_get_max_active_levels
      external omp_get_level
      integer omp_get_level
      external omp_get_ancestor_thread_num
      integer omp_get_ancestor_thread_num
      external omp_get_team_size
      integer omp_get_team_size
      external omp_get_active_level
      integer omp_get_active_level
      
      external omp_init_lock
      external omp_destroy_lock
      external omp_set_lock
      external omp_unset_lock
      external omp_test_lock
      logical omp_test_lock

      external omp_init_nest_lock
      external omp_destroy_nest_lock
      external omp_set_nest_lock
      external omp_unset_nest_lock
      external omp_test_nest_lock
      integer omp_test_nest_lock

      external omp_get_wtick
      double precision omp_get_wtick
      external omp_get_wtime
      double precision omp_get_wtime
