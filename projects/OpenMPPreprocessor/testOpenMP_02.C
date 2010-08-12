/*

NANOS STYLE CODE GENERATION

c
c lu - LU computation
c
        subroutine lu (C,N)
        implicit none
        integer*4 N
        real C(N, N)

        integer*4 i, j, k

        do k = 1, N-1
C$OMP PARALLEL DO PRIVATE(i,j) SCHEDULE(STATIC)
           do i = k+1, N
              C(i,k) = C(i,k) / C(k,k)
              do j = k+1, N
                 C(i,j) = C(i,j) - C(i,k)*C(k,j)
              enddo
           enddo
        enddo
        end

// Transformed Code

       SUBROUTINE lu(c,n)
       IMPLICIT NONE
       INTEGER nthf_cpus_actual
       EXTERNAL nthf_cpus_actual
       INTEGER n
       REAL c(n,n)
       INTEGER i, j, k
       
       DO k = 1,n - 1
         CALL p_lu_l_02(n,c,k,k + 1,n,nthf_cpus_actual())
       END DO
       END

// Transformed Code (supporting function)

       SUBROUTINE lu_l_02(nth_min,nth_max,n,c,k)
       IMPLICIT NONE
       INTEGER nthf_rel_whoami
       EXTERNAL nthf_rel_whoami
       INTEGER n
       REAL c(n,n)
       INTEGER k
       INTEGER j, i
       INTEGER nth_max, nth_min
       
       DO i = nth_min,nth_max
         c(i,k) = c(i,k) / c(k,k)
         DO j = k + 1,n
           c(i,j) = c(i,j) - c(i,k) * c(k,j)
         END DO
       END DO
       RETURN 
       END       


// Transformed Code (supporting function)

       SUBROUTINE p_lu_l_02(n,c,k,nth_bottom_02,nth_top_02,nth_nprocs_02
     1)
       IMPLICIT NONE
       INTEGER nth_thread_id
       INTEGER nth_tmp_nprocs_02, nth_total_cpus_02, nth_thread_id_02, n
     1th_cpus_list_02, nth_niter_02, nth_step_02
       INTEGER *8 nth_mask_02
       INTEGER nth_rest_02, nth_p_02, nth_chunk_02, nth_down_02, nth_up_
     102, nth_02
       EXTERNAL lu_l_02
       INTEGER n
       REAL c(n,n)
       INTEGER k, nth_bottom_02, nth_top_02, nth_nprocs_02
       INTEGER *8 nth_mask
       INTEGER nthf_in_parallel
       EXTERNAL nthf_in_parallel
       INTEGER nthf_burst_create
       EXTERNAL nthf_burst_create
       INTEGER nthf_create_1s
       EXTERNAL nthf_create_1s
       INTEGER nthf_create
       EXTERNAL nthf_create
       INTEGER nthf_create_1s_vp
       EXTERNAL nthf_create_1s_vp
       INTEGER nthf_create_vp
       EXTERNAL nthf_create_vp
       EXTERNAL nthf_depadd
       EXTERNAL nthf_dispatcher_create
       EXTERNAL nthf_to_rq_end
       EXTERNAL nthf_to_lrq_end
       EXTERNAL nthf_block
       EXTERNAL nthf_burst_wait
       INTEGER nthf_self
       EXTERNAL nthf_self
       INTEGER nthf_cpus_actual
       EXTERNAL nthf_cpus_actual
       INTEGER nthf_myteam
       EXTERNAL nthf_myteam
       INTEGER nthf_cpus_maxuser
       EXTERNAL nthf_cpus_maxuser
       INTEGER nthf_chunk_base_size
       EXTERNAL nthf_chunk_base_size
       INTEGER nthf_whoami
       EXTERNAL nthf_whoami
       INTEGER nthf_rel_whoami
       EXTERNAL nthf_rel_whoami
       INTEGER nthf_cpu
       EXTERNAL nthf_cpu
       EXTERNAL nthf_nullify
       EXTERNAL nthf_endsupply
       EXTERNAL nthf_wdsupply
       EXTERNAL nthf_gwdsupply
       EXTERNAL nthf_wdcreate
       INTEGER *8 nthf_create_desc
       EXTERNAL nthf_create_desc
       EXTERNAL nthf_lwdsupply
       EXTERNAL nthf_depsatisfy
       EXTERNAL nthf_to_lrq
       EXTERNAL nthf_barrier
       EXTERNAL nthf_spin_lock
       EXTERNAL nthf_spin_unlock
       INTEGER nthf_finc
       EXTERNAL nthf_finc
       EXTERNAL nthf_finc_alloc
       EXTERNAL nthf_finc_free
       EXTERNAL nthf_take_turn
       EXTERNAL nthf_give_turn
       EXTERNAL nthf_ordered_on
       EXTERNAL nthf_iter_start
       EXTERNAL nthf_iter_end
       EXTERNAL synchronize
       INTEGER nthf_atm_upd8
       EXTERNAL nthf_atm_upd8
       INTEGER nthf_atm_upd4
       EXTERNAL nthf_atm_upd4
       EXTERNAL nthf_define_groups
       EXTERNAL nthf_compute_uniform_groups
       EXTERNAL nthf_compute_groups
       EXTERNAL nthf_undefine_groups
       EXTERNAL nthf_compute_groups_vec
       INTEGER nthf_master
       EXTERNAL nthf_master
       LOGICAL nthf_is_master
       EXTERNAL nthf_is_master
       EXTERNAL nthf_def_precedence
       EXTERNAL nthf_free_precedence
       EXTERNAL nthf_free_waiting
       INTEGER nthf_translate_ws
       EXTERNAL nthf_translate_ws
       INTEGER nthf_translate_do
       EXTERNAL nthf_translate_do
       INTEGER nthf_translate_sections
       EXTERNAL nthf_translate_sections
       INTEGER nthf_translate_single
       EXTERNAL nthf_translate_single
       EXTERNAL nthf_alloc_precmutex
       EXTERNAL nthf_free_precmutex
       EXTERNAL nthf_init_mutex
       EXTERNAL nthf_define_repl
       EXTERNAL nthf_init_repl
       EXTERNAL nthf_free_repl
       EXTERNAL nthf_bound_repl
       EXTERNAL nthf_undefine_repl
       EXTERNAL nthf_lookup_repl
       INTEGER nthf_cpus_loop
       EXTERNAL nthf_cpus_loop
       EXTERNAL nthf_spawnparallel
       
       nth_tmp_nprocs_02 = nth_nprocs_02
       nth_total_cpus_02 = nthf_cpus_maxuser()
       nth_chunk_02 = (nth_top_02 - nth_bottom_02 + 1) / nth_tmp_nprocs_
     102
       nth_rest_02 = abs(mod(nth_top_02 - nth_bottom_02 + 1,nth_tmp_npro
     1cs_02))
       IF (0 .EQ. nth_chunk_02) THEN
         nth_tmp_nprocs_02 = nth_rest_02
         CALL nthf_depadd(nthf_self(),nth_rest_02 + 1)
       ELSE
         CALL nthf_depadd(nthf_self(),nth_tmp_nprocs_02 + 1)
       END IF
       nth_mask = 672
       nth_down_02 = nth_bottom_02
       DO nth_p_02 = 0,nth_rest_02 - 1
         nth_up_02 = nth_chunk_02 + nth_down_02
         nth_02 = nthf_create_1s_vp(lu_l_02,0,nth_p_02,nthf_self(),nth_m
     1ask,05,nth_down_02,nth_up_02,n,c,k)
         nth_down_02 = nth_down_02 + nth_chunk_02 + 1
       END DO
       DO nth_p_02 = nth_rest_02,nth_tmp_nprocs_02 - 1
         nth_up_02 = nth_down_02 + nth_chunk_02 - 1
         nth_02 = nthf_create_1s_vp(lu_l_02,0,nth_p_02,nthf_self(),nth_m
     1ask,05,nth_down_02,nth_up_02,n,c,k)
         nth_down_02 = nth_chunk_02 + nth_down_02
       END DO
       CALL nthf_block()
       END
       

*/

#define SIZE 10
#define SC(i,j) (j)*SIZE+(i)

int
main()
   {
     int i,j;
     float x[SIZE*SIZE];

  // OpenMP directive omp parallel for
#pragma omp parallel for
     for (i=0; i < SIZE; i++)
        {
          for (j=0; j < SIZE; j++)
             {
               x[SC(i,j)] = 0.0;
             }
        }

     return 0;
   }

