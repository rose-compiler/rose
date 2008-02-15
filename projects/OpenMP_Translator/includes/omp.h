/* 
 * $Id: omp.h,v 1.1 2008/01/08 02:55:52 dquinlan Exp $
 * $RWC_Release: Omni-1.6 $
 * $RWC_Copyright:
 *  Omni Compiler Software Version 1.5-1.6
 *  Copyright (C) 2002 PC Cluster Consortium
 *  
 *  This software is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version
 *  2.1 published by the Free Software Foundation.
 *  
 *  Omni Compiler Software Version 1.0-1.4
 *  Copyright (C) 1999, 2000, 2001.
 *   Tsukuba Research Center, Real World Computing Partnership, Japan.
 *  
 *  Please check the Copyright and License information in the files named
 *  COPYRIGHT and LICENSE under the top  directory of the Omni Compiler
 *  Software release kit.
 *  
 *  
 *  $
 */
#ifndef _OMP_H
#define _OMP_H


#ifdef __cplusplus
extern "C" {
#endif

typedef void *omp_lock_t;  /* represented as a pointer */
typedef void *omp_nest_lock_t; /* represented as a pointer */

/*
 * Excution Environment Functions
 */
void omp_set_num_threads(int num);
int omp_get_num_threads(void);
int omp_get_max_threads(void);
int omp_get_thread_num(void);
int omp_get_num_procs(void);
int omp_in_parallel(void);
void omp_set_dynamic(int dynamic_thds);
int omp_get_dynamic(void);
void omp_set_nested(int n_nested);
int omp_get_nested(void);
/*Timer routine*/
double omp_get_wtime(void);
double omp_get_wtick(void);
/*
 * Lock Functions
 */
void omp_init_lock(omp_lock_t *lock);
void omp_init_nest_lock(omp_nest_lock_t *lock);
void omp_destroy_lock(omp_lock_t *lock);
void omp_destroy_nest_lock(omp_nest_lock_t *lock);
void omp_set_lock(omp_lock_t *lock);
void omp_set_nest_lock(omp_nest_lock_t *lock);
void omp_unset_lock(omp_lock_t *lock);
void omp_unset_nest_lock(omp_nest_lock_t *lock);
int omp_test_lock(omp_lock_t *lock);
int omp_test_nest_lock(omp_nest_lock_t *lock);

/*
 * FORTRAN Excution Environment Function Wrappers
 */
void omp_set_num_threads_(int *num);
int omp_get_num_threads_(void);
int omp_get_max_threads_(void);
int omp_get_thread_num_(void);
int omp_get_num_procs_(void);
int omp_in_parallel_(void);
void omp_set_dynamic_(int *dynamic_thds);
int omp_get_dynamic_(void);
void omp_set_nested_(int *n_nested);
int omp_get_nested_(void);

/*
 * FORTRAN Lock Function Wrappers
 */
typedef unsigned int _omf77Lock_t;
void omp_init_lock_(_omf77Lock_t *lock);
void omp_init_nest_lock_(_omf77Lock_t *lock);
void omp_destroy_lock_(_omf77Lock_t *lock);
void omp_destroy_nest_lock_(_omf77Lock_t *lock);
void omp_set_lock_(_omf77Lock_t *lock);
void omp_set_nest_lock_(_omf77Lock_t *lock);
void omp_unset_lock_(_omf77Lock_t *lock);
void omp_unset_nest_lock_(_omf77Lock_t *lock);
int omp_test_lock_(_omf77Lock_t *lock);
int omp_test_nest_lock_(_omf77Lock_t *lock);

/*
 * for cluster
 */
#if defined(__OMNI_SCASH__) || defined(__OMNI_SHMEM__)

/* OMNI_DEST_* parameter work on SCASH, only */
#define  OMNI_DEST_NONE		0
#define	 OMNI_DEST_BLOCK	1
#define	 OMNI_DEST_DIRECT	2

void * ompsm_galloc (int size, int mode, int arg);

#endif /* defined(__OMNI_SCASH__) || defined(__OMNI_SHMEM__) */

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* _OMP_H */

