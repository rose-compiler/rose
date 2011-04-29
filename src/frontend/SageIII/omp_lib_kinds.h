c    from OpenMP 3.0 D.2 p 304: Interface Declaration
      integer     omp_lock_kind
      parameter ( omp_lock_kind = 8 )
      integer     omp_nest_lock_kind
      parameter ( omp_nest_lock_kind = 8 )

      integer     omp_sched_kind
      parameter ( omp_sched_kind = 4)

      integer   ( omp_sched_kind ) omp_sched_static
      parameter ( omp_sched_static = 1 )
      integer   ( omp_sched_kind ) omp_sched_dynamic
      parameter ( omp_sched_dynamic = 2 )
      integer   ( omp_sched_kind ) omp_sched_guided
      parameter ( omp_sched_guided = 3 )
      integer   ( omp_sched_kind ) omp_sched_auto
      parameter ( omp_sched_auto = 4 )
