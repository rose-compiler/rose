#define INCLUDE_NAME_TRANSLATOR
/* This include translates names from base form into system dependent form.
   The most common translation is to append an underscore (_) to all
   routine names.  This is required on some systems, since on some sytems
   the fortran compiler appends an _ to all external names. */

/* $Id: pgslib-names.h,v 1.1 2004/07/07 10:27:17 dquinlan Exp $ */

/* This file has two parts.  The first is the translator macro.  The second
   is the definition of all the routines names.*/

/**********************************************************************/
/* Translator Macro                                                   */
/**********************************************************************/

/* The basic macro is TR_ROUTINE_NAME.  
   The default is for this to be an identity macro.
   If APPEND_UNDERSCORE is defined, then TR_ROUTINE_NAME 
   appends an UNDERSCORE. */

#define TR_ROUTINE_NAME(Base_Name) Base_Name

#ifdef  APPEND_UNDERSCORE
#undef  TR_ROUTINE_NAME
#define TR_ROUTINE_NAME(Base_Name) Base_Name ## _
#endif

/**********************************************************************/
/* List of all C routines in PGSLib, by source directory              */
/**********************************************************************/

/* Routines in gath-scatt directory */
#define pgslib_setup_n_duplicate_c	TR_ROUTINE_NAME(pgslib_setup_n_duplicate_c)
#define pgslib_setup_duplicate_buffer_c	TR_ROUTINE_NAME(pgslib_setup_duplicate_buffer_c)
#define pgslib_prep_supplement_c	TR_ROUTINE_NAME(pgslib_prep_supplement_c )
#define pgslib_gs_init_trace_c		TR_ROUTINE_NAME(pgslib_gs_init_trace_c    )
#define pgslib_gs_release_trace_c	TR_ROUTINE_NAME(pgslib_gs_release_trace_c    )
#define pgslib_gather_buf_int_c		TR_ROUTINE_NAME(pgslib_gather_buf_int_c       )
#define pgslib_gather_buf_real_c	TR_ROUTINE_NAME(pgslib_gather_buf_real_c      )
#define pgslib_gather_buf_double_c	TR_ROUTINE_NAME(pgslib_gather_buf_double_c    )
#define pgslib_gather_buf_log_c		TR_ROUTINE_NAME(pgslib_gather_buf_log_c       )
#define pgslib_scatter_buf_int_c	TR_ROUTINE_NAME(pgslib_scatter_buf_int_c      )
#define pgslib_scatter_buf_real_c	TR_ROUTINE_NAME(pgslib_scatter_buf_real_c     )
#define pgslib_scatter_buf_double_c	TR_ROUTINE_NAME(pgslib_scatter_buf_double_c   )
#define pgslib_scatter_buf_log_c	TR_ROUTINE_NAME(pgslib_scatter_buf_log_c      )

/* Routines in indexing directory */
#define pgslib_init_access_table_c	TR_ROUTINE_NAME(pgslib_init_access_table_c)
#define pgslib_add_item_to_table_c	TR_ROUTINE_NAME(pgslib_add_item_to_table_c)
#define pgslib_count_items_in_table_c	TR_ROUTINE_NAME(pgslib_count_items_in_table_c)
#define pgslib_items_from_table_c	TR_ROUTINE_NAME(pgslib_items_from_table_c )
#define pgslib_item_index_from_table_c	TR_ROUTINE_NAME(pgslib_item_index_from_table_c)

/* Routines in io directory */
#define	pgslib_bcast_int_scalar_c	TR_ROUTINE_NAME(pgslib_bcast_int_scalar_c   )
#define	pgslib_bcast_real_scalar_c	TR_ROUTINE_NAME(pgslib_bcast_real_scalar_c  )
#define	pgslib_bcast_double_scalar_c	TR_ROUTINE_NAME(pgslib_bcast_double_scalar_c)
#define	pgslib_bcast_log_scalar_c	TR_ROUTINE_NAME(pgslib_bcast_log_scalar_c   )
#define	pgslib_bcast_int_vector_c	TR_ROUTINE_NAME(pgslib_bcast_int_vector_c   )
#define	pgslib_bcast_real_vector_c	TR_ROUTINE_NAME(pgslib_bcast_real_vector_c  )
#define	pgslib_bcast_double_vector_c	TR_ROUTINE_NAME(pgslib_bcast_double_vector_c)
#define	pgslib_bcast_log_vector_c	TR_ROUTINE_NAME(pgslib_bcast_log_vector_c   )
#define	pgslib_bcast_char_vector_c	TR_ROUTINE_NAME(pgslib_bcast_char_vector_c  )

#define	pgslib_dist_int_scalar_c	TR_ROUTINE_NAME(pgslib_dist_int_scalar_c   )
#define	pgslib_dist_real_scalar_c	TR_ROUTINE_NAME(pgslib_dist_real_scalar_c  )
#define	pgslib_dist_double_scalar_c	TR_ROUTINE_NAME(pgslib_dist_double_scalar_c)
#define	pgslib_dist_log_scalar_c	TR_ROUTINE_NAME(pgslib_dist_log_scalar_c   )
#define	pgslib_dist_int_vector_c	TR_ROUTINE_NAME(pgslib_dist_int_vector_c   )
#define	pgslib_dist_real_vector_c	TR_ROUTINE_NAME(pgslib_dist_real_vector_c  )
#define	pgslib_dist_double_vector_c	TR_ROUTINE_NAME(pgslib_dist_double_vector_c)
#define	pgslib_dist_log_vector_c	TR_ROUTINE_NAME(pgslib_dist_log_vector_c   )

#define	pgslib_collate_int_scalar_c	TR_ROUTINE_NAME(pgslib_collate_int_scalar_c   )
#define	pgslib_collate_real_scalar_c	TR_ROUTINE_NAME(pgslib_collate_real_scalar_c  )
#define	pgslib_collate_double_scalar_c	TR_ROUTINE_NAME(pgslib_collate_double_scalar_c)
#define	pgslib_collate_log_scalar_c	TR_ROUTINE_NAME(pgslib_collate_log_scalar_c   )
#define	pgslib_collate_int_vector_c	TR_ROUTINE_NAME(pgslib_collate_int_vector_c   )
#define	pgslib_collate_real_vector_c	TR_ROUTINE_NAME(pgslib_collate_real_vector_c  )
#define	pgslib_collate_double_vector_c	TR_ROUTINE_NAME(pgslib_collate_double_vector_c)
#define	pgslib_collate_log_vector_c	TR_ROUTINE_NAME(pgslib_collate_log_vector_c   )
#define pgslib_collate_char_vector_c    TR_ROUTINE_NAME(pgslib_collate_char_vector_c  )

/* Routines in reductions directory */
#define pgslib_global_min_int_c		TR_ROUTINE_NAME(pgslib_global_min_int_c)
#define pgslib_global_min_real_c	TR_ROUTINE_NAME(pgslib_global_min_real_c)
#define pgslib_global_min_double_c	TR_ROUTINE_NAME(pgslib_global_min_double_c)
#define pgslib_global_max_int_c		TR_ROUTINE_NAME(pgslib_global_max_int_c)
#define pgslib_global_max_real_c	TR_ROUTINE_NAME(pgslib_global_max_real_c)
#define pgslib_global_max_double_c	TR_ROUTINE_NAME(pgslib_global_max_double_c)
#define pgslib_global_sum_int_c		TR_ROUTINE_NAME(pgslib_global_sum_int_c)
#define pgslib_global_sum_real_c	TR_ROUTINE_NAME(pgslib_global_sum_real_c)
#define pgslib_global_sum_double_c	TR_ROUTINE_NAME(pgslib_global_sum_double_c)
#define pgslib_global_all_log_c		TR_ROUTINE_NAME(pgslib_global_all_log_c)
#define pgslib_global_any_log_c		TR_ROUTINE_NAME(pgslib_global_any_log_c)
#define pgslib_global_minloc_int_c	TR_ROUTINE_NAME(pgslib_global_minloc_int_c)
#define pgslib_global_minloc_real_c	TR_ROUTINE_NAME(pgslib_global_minloc_real_c)
#define pgslib_global_minloc_double_c	TR_ROUTINE_NAME(pgslib_global_minloc_double_c)
#define pgslib_global_maxloc_int_c	TR_ROUTINE_NAME(pgslib_global_maxloc_int_c)
#define pgslib_global_maxloc_real_c	TR_ROUTINE_NAME(pgslib_global_maxloc_real_c)
#define pgslib_global_maxloc_double_c	TR_ROUTINE_NAME(pgslib_global_maxloc_double_c)

/* Routines in the scans directory */
#define off_node_sum_prefix_int_c	TR_ROUTINE_NAME(off_node_sum_prefix_int_c)
#define off_node_sum_prefix_single_c	TR_ROUTINE_NAME(off_node_sum_prefix_single_c)
#define off_node_sum_prefix_double_c	TR_ROUTINE_NAME(off_node_sum_prefix_double_c)
#define off_node_sum_suffix_int_c	TR_ROUTINE_NAME(off_node_sum_suffix_int_c)
#define off_node_sum_suffix_single_c	TR_ROUTINE_NAME(off_node_sum_suffix_single_c)
#define off_node_sum_suffix_double_c	TR_ROUTINE_NAME(off_node_sum_suffix_double_c)

/* Routine in send-rcv directory */
#define	pgslib_attempt_receive_int_c	TR_ROUTINE_NAME(pgslib_attempt_receive_int_c)
#define	pgslib_attempt_receive_real_c	TR_ROUTINE_NAME(pgslib_attempt_receive_real_c)
#define	pgslib_attempt_receive_double_c	TR_ROUTINE_NAME(pgslib_attempt_receive_double_c)
#define	pgslib_attempt_receive_log_c	TR_ROUTINE_NAME(pgslib_attempt_receive_log_c)
#define	pgslib_cnstd_send_rcv_int_c	TR_ROUTINE_NAME(pgslib_cnstd_send_rcv_int_c)
#define	pgslib_cnstd_send_rcv_real_c	TR_ROUTINE_NAME(pgslib_cnstd_send_rcv_real_c)
#define	pgslib_cnstd_send_rcv_double_c	TR_ROUTINE_NAME(pgslib_cnstd_send_rcv_double_c)
#define	pgslib_cnstd_send_rcv_log_c	TR_ROUTINE_NAME(pgslib_cnstd_send_rcv_log_c)
#define	pgslib_random_send_rcv_int_c	TR_ROUTINE_NAME(pgslib_random_send_rcv_int_c)
#define	pgslib_random_send_rcv_real_c	TR_ROUTINE_NAME(pgslib_random_send_rcv_real_c)
#define	pgslib_random_send_rcv_double_c	TR_ROUTINE_NAME(pgslib_random_send_rcv_double_c)
#define	pgslib_random_send_rcv_log_c	TR_ROUTINE_NAME(pgslib_random_send_rcv_log_c)

/* Routines in utility directory */
#define pgslib_initialize_c		TR_ROUTINE_NAME(pgslib_initialize_c)
#define pgslib_mpi_init			TR_ROUTINE_NAME(pgslib_mpi_init)
#define pgslib_finalize_c		TR_ROUTINE_NAME(pgslib_finalize_c)
#define	pgslib_error_c			TR_ROUTINE_NAME(pgslib_error_c)
#define	pgslib_fatal_error_c		TR_ROUTINE_NAME(pgslib_fatal_error_c)
#define	pgslib_abort_c			TR_ROUTINE_NAME(pgslib_abort_c)
#define	pgslib_output_c			TR_ROUTINE_NAME(pgslib_output_c)
#define	pgslib_flush_output_c		TR_ROUTINE_NAME(pgslib_flush_output_c)
#define pgslib_close_output_c		TR_ROUTINE_NAME(pgslib_close_output_c)
#define	pgslib_check_malloc_c		TR_ROUTINE_NAME(pgslib_check_malloc_c)
#define pgslib_barrier_c		TR_ROUTINE_NAME(pgslib_barrier_c)
