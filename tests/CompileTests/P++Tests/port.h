#ifndef __PORT_H_
#define __PORT_H_

#ifdef HAVE_CONFIG_H
/* #include <config.h> */
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>


/*
 *
 *     MPI 
 *
 */

#if 0
#ifdef USE_MPI
#define PARTI_ENABLE_MP_INTERFACE_MPI
/* error "MPI defined in port.h" */
#else
#error "MPI NOT defined in port.h"
#endif
#endif
#include <Parti_config.h>

#ifdef PARTI_ENABLE_MP_INTERFACE_MPI
#include <mpi.h>
/* include "mpi-port.h" */

#if defined(PARTI_ENABLE_MP_INTERFACE_PVM) && defined(PARTI_ENABLE_MP_INTERFACE_MPI)
#error PVM and MPI defined!
#endif

#ifdef MAIN_PARTI_SOURCE_FILE
/* #error "__FILE__ == bsparti.c" */
int      Global_PARTI_PADRE_Interface_Number_Of_Processors;
MPI_Comm Global_PARTI_PADRE_Interface_PADRE_Comm_World;
#else
/* #error "__FILE__ != bsparti.c" */
extern int      Global_PARTI_PADRE_Interface_Number_Of_Processors;
extern MPI_Comm Global_PARTI_PADRE_Interface_PADRE_Comm_World;
#endif

#define GID_TYPE MPI_Request

#define PARTI_numprocs()                   PARTI_MPI_numprocs()
#define PARTI_gsync()                      PARTI_MPI_gsync()

#define PARTI_myproc()                     PARTI_MPI_myproc()

#define PARTI_csend(tag,buf,len,node,pid)  PARTI_MPI_csend(tag,buf,len,node,pid)
#define PARTI_crecv(tag,buf,len,node,pid)  PARTI_MPI_crecv(tag,buf,len,node,pid)
#if 1
#define PARTI_isend(tag,buf,len,node,pid)  PARTI_MPI_isend(tag,buf,len,node,pid)
#define PARTI_irecv(tag,buf,len,node,pid)  PARTI_MPI_irecv(tag,buf,len,node,pid)
#define PARTI_msgdone(gid)                 PARTI_MPI_msgdone(gid)
#define PARTI_msgwait(gid)                 PARTI_MPI_msgwait(gid)
#else
#define PARTI_isend(tag,buf,len,node,pid)  PARTI_MPI_csend(tag,buf,len,node,pid)
#define PARTI_irecv(tag,buf,len,node,pid)  PARTI_MPI_crecv(tag,buf,len,node,pid)
#define PARTI_msgdone(gid)                 1
#define PARTI_msgwait(gid)                 NULL
#endif


#else
/* define GID_TYPE for all other parallel machines... */

#define GID_TYPE long

#endif /* MPI */

/*
 * 	PVM 3.1	
 *
 */

#ifdef PARTI_ENABLE_MP_INTERFACE_PVM
#include <pvm3.h>

extern int   Global_PARTI_PADRE_Interface_Number_Of_Processors;
extern char* Global_PARTI_PADRE_Interface_Name_Of_Main_Processor_Group;
int myproc(int);

#define NUMNODES Global_PARTI_PADRE_Interface_Number_Of_Processors
#define EXECNAME Global_PARTI_PADRE_Interface_Name_Of_Main_Processor_Group

#define PARTI_numprocs()         NUMNODES
#define PARTI_gsync()            pvm_barrier(EXECNAME,NUMNODES)

/* Added by Dan Quinlan by not required since NUMNODES and EXECNAME are defined above  */
/* define PARTI_numprocs()     Global_PARTI_PADRE_Interface_Number_Of_Processors */
/* define PARTI_gsync()        pvm_barrier(EXECNAME,Global_PARTI_PADRE_Interface_Number_Of_Processors) */

#define PARTI_myproc()        	myproc(-1) 
#define PARTI_csend(tag,buf,len,node,pid) \
                                jimsend(tag,buf,len,node,pid)
#define PARTI_crecv(tag,buf,len,node,pid) \
                                jimrecv(tag,buf,len,node,pid)
/*  No isend or irecv yet */
/* Added by Dan Quinlan   */
#define PARTI_isend(tag,buf,len,node,pid) \
                                jimsend(tag,(char*)buf,len,node,pid)
#define PARTI_irecv(tag,buf,len,node,pid) \
                                jimrecv(tag,(char*)buf,len,node,pid)
#define PARTI_msgdone(gid)	1
#define PARTI_msgwait(gid)	NULL
#endif /* PARTI_ENABLE_MP_INTERFACE_PVM */


/*
 * 	INTEL i860	
 *
 */

#ifdef INTEL
#define PARTI_numprocs()        numnodes()
#define PARTI_gsync()           gsync()
#define PARTI_myproc()          mynode()
#define PARTI_csend(tag,buf,len,node,pid) \
                                csend(tag,buf,len,node,pid)
#define PARTI_crecv(tag,buf,len,node,pid) \
                                crecv(tag,buf,len)
#define PARTI_isend(tag,buf,len,node,pid) \
                                isend(tag,buf,len,node,pid)
#define PARTI_irecv(tag,buf,len,node,pid) \
                                irecv(tag,buf,len)
#define PARTI_msgwait(gid)	msgwait(gid)
#define PARTI_msgdone(gid)	msgdone(gid)
#define PARTI_gdsum(x,n,work)   gdsum(x,n,work)
#define PARTI_killall()	killcube(ALL_NODES,NODE_PID)
#define PARTI_dclock()	dclock()
#endif /* intel*/


/*
 * 	CM-5 CMMD
 *
 */

#ifdef CM5
#include <cm/cmmd.h>
#define PARTI_numprocs()        CMMD_partition_size()
#define PARTI_gsync()           CMMD_sync_with_nodes()
#define PARTI_myproc()          CMMD_self_address()
#define PARTI_csend(tag,buf,size,node,pid) \
                                CMMD_send_block(node,tag,buf,size)
#define PARTI_crecv(tag,buf,size,node,pid) \
                                CMMD_receive_block(node,tag,buf,size)
#define PARTI_isend(tag,buf,size,node,pid) \
                                CMMD_send_async(node,tag,buf,size,NULL,NULL)
#define PARTI_irecv(tag,buf,size,node,pid) \
                                CMMD_receive_async(node,tag,buf,size,NULL,NULL)
#define PARTI_msgwait(id)       {CMMD_msg_wait(id);CMMD_free_mcb(id);}
#define PARTI_msgdone(id)       {!CMMD_msg_pending(CMMD_ANY_NODE,id);CMMD_free_mcb(id);}
#define PARTI_gdsum(x,n,work)   CMMD_reduce_v(x,x,CMMD_combiner_dadd,sizeof(double),n)

#define PARTI_killall()		
#endif /* cm5 */
#endif


/*
 *      IBM SP-1
 *
 */


#ifdef SP1

/*#include<mpceui.h>
#include<mpctof.c>*/


/*
 * sp1.h
 *
 * SP1-Parti definitions, macros, and function prototypes...
 * Version 1.0
 *                                      shin 10/4/93 
 */

/* IBM SP-1 header files */
#include <mpproto.h>

/*
 * Global variables
 */

/* variables for SP-1 communication functions */
extern  int     PARTI_source;           /* source node of incoming message */
extern  int     PARTI_type;             /* type of incoming message */
extern  int     PARTI_nbytes;           /* number of bytes received */

/* wild cards */
extern  int     dontcare;               /* don't care */
extern  int     allmsg;                 /* all messages */
extern  int     nulltask;
extern  int     allgrp;                 /* all groups */

/*
 * Definitions
 */

/* tags for force type message */
#define FORCED_GATHER_EXECUTOR_MSG      10000
#define FORCED_SCATTER_EXECUTOR_MSG     20000
#define FORCED_ADD_EXECUTOR_MSG         30000
#define FORCED_SUB_EXECUTOR_MSG         40000
#define FORCED_MULT_EXECUTOR_MSG        50000

/* tags for unforce type message */
#define GATHER_EXECUTOR_MSG             00000
#define SCATTER_EXECUTOR_MSG            10000
#define ADD_EXECUTOR_MSG                20000
#define SUB_EXECUTOR_MSG                30000
#define MULT_EXECUTOR_MSG               40000

/* tags for synchronizing message */
#define GATHER_SYNC                     50000
#define SCATTER_SYNC                    60000
#define ADD_SYNC                        70000
#define SUB_SYNC                        80000
#define MULT_SYNC                       90000

#define MAXNODES                        512
#define MAXCOUNT                        10

#define TABLE_REMAP                     17000
#define MSG_TTABLE                      18097
#define MSG_PROC                        15500
#define MSG_OFFS                        25500

#define PARTI_ANY_NODE                  DONTCARE 

/*
 * IBM SP-1 Communication Routines
 */
#define PARTI_numprocs()        mpc_numnodes()
#define PARTI_myproc()          mpc_mynode()
#define PARTI_gsync()           mpc_sync(ALLGRP)
#define PARTI_bsend(tag,buf,len,node,pid) \
                                mpc_bsend(buf,len,node,tag)
#define PARTI_csend(tag,buf,len,node,pid) \
                                mpc_bsend(buf,len,node,tag)
#define PARTI_isend(tag,buf,len,node,pid) \
                                mpc_isend(tag,buf,len,node)
#define PARTI_crecv(tag,buf,len,node,pid) \
                                mpc_crecv(tag,buf,len,node)
#define PARTI_irecv(tag,buf,len,node,pid) \
                                mpc_irecv(tag,buf,len,node)
#define PARTI_msgwait(id)       mpc_wait(&id,&PARTI_nbytes)
#define PARTI_gisum(x,n,work)   mpc_gisum(x,n,work)
#define PARTI_gihigh(x,n,work)  mpc_gihigh(x,n,work)
#define PARTI_gilow(x,n,work)   mpc_gilow(x,n,work)
#define PARTI_gdsum(x,n,work)   mpc_gdsum(x,n,work)
#define PARTI_msgdone(id)	(mpc_status(id)+1)
#define MPI_msgwait(id)         mpc_wait(&id,&PARTI_nbytes)
#define PARTI_killall()
#endif /* sp */

