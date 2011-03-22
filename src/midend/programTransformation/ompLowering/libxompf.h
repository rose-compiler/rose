! Liao 1/11/2011
! XOMP function prototype support Fortran source-to-source translation
! This is necessary since they will be treated as returning REAL
! since they start with X (implicit to be REAL, not INTEGER)

! loop scheduling functions
!loop start
      EXTERNAL XOMP_loop_static_start
      EXTERNAL XOMP_loop_dynamic_start
      EXTERNAL XOMP_loop_guided_start
      EXTERNAL XOMP_loop_runtime_start
      EXTERNAL XOMP_loop_ordered_static_start
      EXTERNAL XOMP_loop_ordered_dynamic_start
      EXTERNAL XOMP_loop_ordered_guided_start
      EXTERNAL XOMP_loop_ordered_runtime_start

      INTEGER XOMP_loop_static_start
      INTEGER XOMP_loop_dynamic_start
      INTEGER XOMP_loop_guided_start
      INTEGER XOMP_loop_runtime_start
      INTEGER XOMP_loop_ordered_static_start
      INTEGER XOMP_loop_ordered_dynamic_start
      INTEGER XOMP_loop_ordered_guided_start
      INTEGER XOMP_loop_ordered_runtime_start


! loop next       
      EXTERNAL XOMP_loop_static_next
      EXTERNAL XOMP_loop_dynamic_next
      EXTERNAL XOMP_loop_guided_next
      EXTERNAL XOMP_loop_runtime_next
      EXTERNAL XOMP_loop_ordered_static_next
      EXTERNAL XOMP_loop_ordered_dynamic_next
      EXTERNAL XOMP_loop_ordered_guided_next
      EXTERNAL XOMP_loop_ordered_runtime_next

      INTEGER XOMP_loop_static_next
      INTEGER XOMP_loop_dynamic_next
      INTEGER XOMP_loop_guided_next
      INTEGER XOMP_loop_runtime_next
      INTEGER XOMP_loop_ordered_static_next
      INTEGER XOMP_loop_ordered_dynamic_next
      INTEGER XOMP_loop_ordered_guided_next
      INTEGER XOMP_loop_ordered_runtime_next

! a few others
      EXTERNAL XOMP_single
      INTEGER XOMP_single
      EXTERNAL XOMP_master
      INTEGER XOMP_master
