/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1994                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/*
 *
 * pcxx_events.h: simple SW monitor routines
 *
 * (c) 1994 Jerry Manic Saftware
 *
 * Version 3.0
 */

# ifndef __PCXX_EVENTS__H__
# define __PCXX_EVENTS__H__

#include "tau_types.h"
#   include "pcxx_ansi.h"

#   if !defined(__ksr__) || defined(UNIPROC) || defined(__PCXX__) || defined(_MASTER_SLAVE_)
#     define __private
#   endif

    /* -- event classes ---------------------- */
    extern __private unsigned long int pcxx_ev_class;

/* Ariadne debugger is currently supported on sgimp and mpi architectures.
   Extend this list as other architectures are added */
#if defined(__MPI__) || defined(__SGIMP__)
#define ARIADNE_SUPPORT
#endif

#ifdef ARIADNE_SUPPORT
extern int aa_poll_count; /* For Ariadne - defined in tulip/mpi/src/KernelCom.c */
#endif /* ARIADNE_SUPPORT */

#   ifdef TRACING_ON

#     ifdef PCXX_EVENT_SRC
#       include "pcxx_machines.h"

        /* -- pcxx tracer events ------------------- */
#       define PCXX_EV_INIT         60000
#       define PCXX_EV_FLUSH_ENTER  60001
#       define PCXX_EV_FLUSH_EXIT   60002
#       define PCXX_EV_CLOSE        60003
#       define PCXX_EV_INITM        60004
#       define PCXX_EV_WALL_CLOCK   60005
#       define PCXX_EV_CONT_EVENT   60006
#	define TAU_MESSAGE_SEND	    60007
#	define TAU_MESSAGE_RECV	    60008

        /* -- the following two events are only the ----- */
        /* -- base numbers, actually both represent ----- */
        /* -- 64 events (60[1234]00 to 60[1234]64)  ----- */
#       define PCXX_WTIMER_CLEAR    60199
#       define PCXX_WTIMER_START    60100
#       define PCXX_WTIMER_STOP     60200
#       define PCXX_UTIMER_CLEAR    60399
#       define PCXX_UTIMER_START    60300
#       define PCXX_UTIMER_STOP     60400

	

#       ifndef PCXX_BUFSIZE
#         define PCXX_BUFSIZE 65536  /* -- 64 K -- */
#       endif

        /* -- event classes ----------------------------- */
#       ifndef PCXX_EC_NULL
#         define PCXX_EC_NULL       0x00000000

#         define PCXX_EC_TRACER     0x00000001
#         define PCXX_EC_TIMER      0x00000002

#         define PCXX_EC_ALL        0xFFFFFFFF
#       endif

#     endif /* PCXX_EVENT_SRC */

#     ifdef __cplusplus
        extern "C" {
#     endif /* __cplusplus */

#     ifdef PCXX_EVENT_SRC

        /* -- event record buffer descriptor ------------------------- */
        typedef struct
        {
          /*           long int           ev;    /* -- event id        -- */
          x_int32            ev;    /* -- event id        -- */
          x_uint16           nid;   /* -- node id         -- */
          x_uint16           tid;   /* -- thread id       -- */
          x_int64            par;   /* -- event parameter -- */
/**** CHANGED long int to long long for 8 bytes. *******/
          x_uint64           ti;    /* -- time [us]?      -- */
/**** CHANGED unsigned long int to unsigned long long for 8 bytes. *******/
        } PCXX_EV;

        /* -- pointer to next free element of event record buffer ---- */
        extern __private PCXX_EV *pcxx_ev_ptr;

        /* -- pointer to last available element of event record buffer */
        extern __private PCXX_EV *pcxx_ev_max;

#     endif /* PCXX_EVENT_SRC */

      /* -- pcxx monitor routines ------------------------------------ */
#     define PCXX_EVENT(c,e,p) \
             if (pcxx_ev_class&(c)) pcxx_Event(e,p)
#     define PCXX_LONG_EVENT(c,e,l,p) \
             if (pcxx_ev_class&(c)) pcxx_LongEvent(e,l,p)
#     ifdef __cplusplus
        void pcxx_EvInit (char *n);
        void pcxx_Event (long int e, x_int64 p);
        void pcxx_LongEvent (long int e, int l, char *p);
        void pcxx_EvClose ();
        void pcxx_EvFlush ();
	/* New tracing interface */
	int TraceEvInit(int tid);
	void TraceUnInitialize(int tid);
 	void TraceReinitialize(int oldid, int newid, int tid);
        void TraceEventOnly(long int ev, x_int64 par, int tid);
        void TraceEvFlush(int tid);
 	void TraceEvent(long int ev, x_int64 par, int tid, x_uint64 ts = 0L, int use_ts = 0);
	void TraceEvClose(int tid);
	void SetFlushEvents(int tid);
        int  GetFlushEvents(int tid);

        }
#     else
        extern void pcxx_EvInit(char *n);
        extern void pcxx_Event(long int e, long long p);
        extern void pcxx_LongEvent(long int e, int l, char *p);
        extern void pcxx_EvClose ();
        extern void pcxx_EvFlush ();
#     endif /* __cplusplus */

#   else

#     define PCXX_EVENT(c, e, p)
#     define PCXX_LONG_EVENT(c, e, l, p)
#     define pcxx_EvInit(n)
#     define pcxx_Event(e, p)
#     define pcxx_LongEvent(e, l, p)
#     define pcxx_EvClose()
#     define pcxx_EvFlush()
#     define pcxx_AriadneTrace(ec, ev, pid, oid, rwtype, mtag, par) 

#   endif /* TRACING_ON */

#   ifdef TRACING_ON   /* for Ariadne */
#     ifdef ARIADNE_SUPPORT
#       ifdef __cplusplus
extern "C" void pcxx_AriadneTrace (long int event_class, long int event, int pid, int oid, int rwtype, int mtag, int parameter); 
#       else 	/* __cplusplus */
 	extern void pcxx_AriadneTrace (long int event_class, long int event, int pid, int oid, int rwtype, int mtag, int parameter); 
#       endif   /* __cplusplus */
#     else  /* Ariadne is not supported */
#     define pcxx_AriadneTrace(ec, ev, pid, oid, rwtype, mtag, par) 
/* define it as blank  - like its done above when tracing is not on */
#     endif /* ARIADNE_SUPPORT */
#   endif   /* TRACING_ON */




# endif /* !__PCXX_EVENTS_H__ */
