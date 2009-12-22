
/*
                              COPYRIGHT

Copyright 1993-2007, Lawrence Livermore National Security, LLC. All
rights reserved.  This work was produced at the Lawrence Livermore
National Laboratory (LLNL) under contract no. DE-AC52-07NA27344
(Contract 44) between the U.S. Department of Energy (DOE) and Lawrence
Livermore National Security, LLC (LLNS) for the operation of
LLNL. Copyright is reserved to Lawrence Livermore National Security,
LLC for purposes of controlled dissemination, commercialization
through formal licensing, or other disposition under terms of Contract
44; DOE policies, regulations and orders; and U.S. statutes. The
rights of the Federal Government are reserved under Contract 44.

                              DISCLAIMER

This work was prepared as an account of work sponsored by an agency of
the United States Government. Neither the United States Government nor
Lawrence Livermore National Security, LLC nor any of their employees,
makes any warranty, express or implied, or assumes any liability or
responsibility for the accuracy, completeness, or usefulness of any
information, apparatus, product, or process disclosed, or represents
that its use would not infringe privately-owned rights. Reference
herein to any specific commercial products, process, or service by
trade name, trademark, manufacturer or otherwise does not necessarily
constitute or imply its endorsement, recommendation, or favoring by
the United States Government or Lawrence Livermore National Security,
LLC. The views and opinions of authors expressed herein do not
necessarily state or reflect those of the United States Government or
Lawrence Livermore National Security, LLC, and shall not be used for
advertising or product endorsement purposes.

                        LICENSING REQUIREMENTS

Any use, reproduction, modification, or distribution of this software
or documentation for commercial purposes requires a license from
Lawrence Livermore National Security, LLC. Contact: Lawrence Livermore
National Laboratory, Industrial Partnerships Office, P.O. Box 808,
L-795, Livermore, CA 94551.

The Government is granted for itself and others acting on its behalf a
paid-up, nonexclusive, irrevocable worldwide license in this data to
reproduce, prepare derivative works, and perform publicly and display
publicly.

*/

/***********************************************************/
/* Dynamic CPU Trace -- gather loop and sequential segment */
/*                      information about program behavior */
/***********************************************************/

/* This code is written to work with a C or C++ compiler. */
/* As a result, the code is uglier than it could be. */
/* Also, it is not currently thread safe. */

#include <stdio.h>
#include <stdlib.h>

#include "ETrt.h"
#ifdef ET_PAPI
#define C99
#include "papi.h"
#else
#include "cycle.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 0
/* Less memory/cache overhead when this mode enabled */
#define ET_POST_PROCESS_SEQ_TO_LOOP
#endif

/****************/
/*    Macros    */
/****************/

#define ET_MAX_LOOPS              8190
#define ET_MAX_LOOP_SEQ_OVERFLOW  (3*ET_MAX_LOOPS)
#define ET_MAX_LOOP_DEPTH         1024
#define ET_MAX_SEQ_LOOP_MAP       (ET_MAX_LOOPS+ET_MAX_LOOP_SEQ_OVERFLOW)

#define ET_MAX_SEGMENTS           8190
#define ET_MAX_SEG_SEQ_OVERFLOW   (ET_MAX_SEGMENTS+(ET_MAX_SEGMENTS/2))

/*************************/
/*    Data Structures    */
/*************************/

struct ET_LocInfo
{
   const char *fileName ;
   const char *funcName ;
   int line ;
} ;

/***************/
/*    Loops    */
/***************/

#ifdef ET_PAPI
static int ET_EventSet = PAPI_NULL ;
#endif

/* for each loop, keep a pointer to __FILE__ and __LINE__ info */
/* This information should be user selectable, with respect to */
/* input source or output source */
static struct ET_LocInfo ET_loopLoc[ET_MAX_LOOPS] ;

#ifndef ET_SIMPLE_LOOP_STATS
struct ET_CallEdge
{
   ET_Idx_t seqId ;
   ET_Idx_t parentSeqId ;
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
   int level ;
#endif
} ;

/* Keep a mapping from loop number to loop sequence ID(s) */
/* positive entry == single sequence ID */
/* negative entry == index into linked list structure */
static struct ET_CallEdge ET_loopSeqId[ET_MAX_LOOPS] ;

/* Linked List Overflow pool for loops having multiple */
/* entry points (aka multiple sequence numbers) */
static ET_Idx_t ET_freeHead = 1 ; /* Waste an entry to enhance performance */
static struct ET_SeqNode
{
   struct ET_CallEdge edge ;
   ET_Idx_t next  ;
} ET_SeqIdOverflow[ET_MAX_LOOP_SEQ_OVERFLOW] ;

#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
/* this is purely for convenience */
static ET_Idx_t ET_seqToLoop[ET_MAX_SEQ_LOOP_MAP] ;
#endif

#endif /* ET_SIMPLE_LOOP_STATS */

/* Holds a sequence number cursor for each loop level */
/* Need to be careful here, because the code may */
/* contain recursion. */
static int ET_loopLevel ;
static ET_Idx_t ET_loopStack[ET_MAX_LOOP_DEPTH] ;
#ifdef ET_PAPI
static long_long ET_loopTime[ET_MAX_LOOP_DEPTH] ;
#else
static ticks ET_loopTime[ET_MAX_LOOP_DEPTH] ;
#endif

/****************/
/*  Loop Stats  */
/****************/

/* Accumulate information for loop Sequence IDs */
#ifdef ET_PAPI
static long_long ET_loopStat[ET_MAX_LOOP_SEQ_OVERFLOW] ;
#else
static double ET_loopStat[ET_MAX_LOOP_SEQ_OVERFLOW] ;
#endif
#ifndef ET_NO_COUNT_ITERS
static int    ET_loopIter[ET_MAX_LOOP_SEQ_OVERFLOW] ;
#endif

/***************/
/*   Segments  */
/***************/

/* make sure we have a data structure that can specialize */
/* this information based on loop path.  For example, the */
/* accumulated segment count should be call-tree path based. */

static struct ET_LocInfo ET_startSegLoc[ET_MAX_SEGMENTS] ;
static struct ET_LocInfo ET_stopSegLoc [ET_MAX_SEGMENTS] ;

struct ET_SegId
{
   ET_Idx_t seqId ;
   ET_Idx_t parentLoopSeqId ;
} ;

static struct ET_SegId ET_startSegId[ET_MAX_SEGMENTS] ;
static struct ET_SegId ET_stopSegId [ET_MAX_SEGMENTS] ;

/* Linked List Overflow pool for loops having multiple */
/* entry points (aka multiple sequence numbers) */
static ET_Idx_t ET_segFreeHead = 1 ; /* Waste entry to enhance performance */
static struct ET_SegNode
{
   struct ET_SegId edge ;
   ET_Idx_t next  ;
} ET_segIdOverflow[2*ET_MAX_SEGMENTS] ;

struct ET_SegMatch
{
   ET_Idx_t segPairId ;
   ET_Idx_t stopId ;
} ;

/* connect a start seq Id with a stop seq Id and create a unique segment Id */
static struct ET_SegMatch ET_segPair[ET_MAX_SEGMENTS] ;

static ET_Idx_t ET_pairFreeHead = 1 ; /* Waste entry to enhance performance */
static struct ET_SegPairNode
{
   struct ET_SegMatch edge ;
   ET_Idx_t next ;
} ET_segMatchOverflow[2*ET_MAX_SEGMENTS] ;

/* trace currently active segment */
static ET_Idx_t ET_currStartSegSeqId ;
#ifdef ET_PAPI
static long_long ET_startSegCounter ;
#else
static ticks ET_startSegCounter ;
#endif

#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
/* this is purely for convenience */
static ET_Idx_t ET_startSeqToId[ET_MAX_SEQ_LOOP_MAP] ;
static ET_Idx_t ET_stopSeqToId[ET_MAX_SEQ_LOOP_MAP] ;
#endif

struct PairTrack
{
   ET_Idx_t start ;
   ET_Idx_t stop ;
} ;

static struct PairTrack ET_pairToSeq[ET_MAX_SEGMENTS] ;

/*******************/
/*  Segment Stats  */
/*******************/

/* Accumulate information for Segment Pair Ids */
#if ET_PAPI
static long_long ET_segStat[ET_MAX_SEGMENTS] ;
#else
static double ET_segStat[ET_MAX_SEGMENTS] ;
#endif

/*************************/
/*    Static Functions   */
/*************************/

/**********************/
/*   Segment related  */
/**********************/

static ET_Idx_t ET_GetNextStartSegId()
{
   static ET_Idx_t ET_startSegId ;
   return ++ET_startSegId ;
}

static ET_Idx_t ET_GetNextStopSegId()
{
   static ET_Idx_t ET_stopSegId ;
   return ++ET_stopSegId ;
}

static ET_Idx_t ET_GetNextPairId()
{
   static ET_Idx_t ET_pairId ;
   return ++ET_pairId ;
}

#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
static ET_Idx_t RegisterSeg(struct ET_SegId *baseSegId, ET_Idx_t segId,
                            ET_Idx_t loopContext, ET_Idx_t (*seqFunc)(),
                            ET_Idx_t *ET_seqToId)
#else
static ET_Idx_t RegisterSeg(struct ET_SegId *baseSegId, ET_Idx_t segId,
                            ET_Idx_t loopContext, ET_Idx_t (*seqFunc)() )
#endif
{
   ET_Idx_t segSeqId ;

   /******************************/
   /*    Generate Sequence ID    */
   /******************************/
   if (baseSegId[segId].seqId > 0) /* common case first */
   {
      /* A unique call path reaches this loop */
      if (baseSegId[segId].parentLoopSeqId == loopContext)
      {
         segSeqId = baseSegId[segId].seqId ;
      }
      else
      {
         /* Create New Overflow Chain */
         ET_Idx_t cursor = ET_segFreeHead ;
         ++ET_segFreeHead ; /* advance to next empty entry */

         ET_segIdOverflow[cursor].edge.seqId       = baseSegId[segId].seqId ;
         ET_segIdOverflow[cursor].edge.parentLoopSeqId = baseSegId[segId].parentLoopSeqId ;
         ET_segIdOverflow[cursor].next             = ET_segFreeHead ;

         baseSegId[segId].seqId       = -cursor ;
         baseSegId[segId].parentLoopSeqId = -32768 ; /* debugging sentinel */

         cursor = ET_segFreeHead ;
         ++ET_segFreeHead ;

         segSeqId = seqFunc() ;
         ET_segIdOverflow[cursor].edge.seqId       = segSeqId ;
         ET_segIdOverflow[cursor].edge.parentLoopSeqId = loopContext ;
         ET_segIdOverflow[cursor].next             = 0 ; /* end of chain */
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
         /* This adds a performance hit */
         ET_seqToId[segSeqId] = segId ;
#endif
      }
   }
   else if (baseSegId[segId].seqId == 0)
   {
      /* This loop has never been encountered */
      segSeqId = seqFunc() ;
      baseSegId[segId].seqId       = segSeqId ;
      baseSegId[segId].parentLoopSeqId = loopContext ;
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
      /* This adds a performance hit */
      ET_seqToId[segSeqId] = segId ;
#endif
   }
   else /* (baseSegId[segId].seqId < 0) */
   {
      /* This loop is reached from multiple call paths */
      /* search for this callpath in overflow chain */
      ET_Idx_t cursor ;
      ET_Idx_t lastCursor ;
      for (cursor = -baseSegId[segId].seqId ; cursor != 0;
           cursor = ET_segIdOverflow[cursor].next)
      {
         lastCursor = cursor ;
         if (ET_segIdOverflow[cursor].edge.parentLoopSeqId == loopContext)
         {
            segSeqId = ET_segIdOverflow[cursor].edge.seqId ;
            break ;
         }
      }
      if (cursor == 0)
      {
         /* Add new entry at end of overflow chain. */
         /* (should new entries be added to head because */
         /*  they are more likely to be deeply nested?) */
         cursor = ET_segFreeHead ;
         ++ET_segFreeHead ;

         ET_segIdOverflow[lastCursor].next = cursor ;

         segSeqId = seqFunc() ;
         ET_segIdOverflow[cursor].edge.seqId       = segSeqId ;
         ET_segIdOverflow[cursor].edge.parentLoopSeqId = loopContext ;
         ET_segIdOverflow[cursor].next             = 0 ; /* end of chain */
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
         /* This adds a performance hit */
         ET_seqToId[segSeqId] = segId ;
#endif
      }
   }

   return segSeqId ;
}

static ET_Idx_t RegisterPair(ET_Idx_t startSegSeqId, ET_Idx_t stopSegSeqId)
{
   ET_Idx_t pairId ;

   /******************************/
   /*    Generate Sequence ID    */
   /******************************/
   if (ET_segPair[startSegSeqId].segPairId > 0) /* common case first */
   {
      /* A unique call path reaches this loop */
      if (ET_segPair[startSegSeqId].stopId == stopSegSeqId)
      {
         pairId = ET_segPair[startSegSeqId].segPairId ;
      }
      else
      {
         /* Create New Overflow Chain */
         ET_Idx_t cursor = ET_pairFreeHead ;
         ++ET_pairFreeHead ; /* advance to next empty entry */

         ET_segMatchOverflow[cursor].edge.segPairId       = ET_segPair[startSegSeqId].segPairId ;
         ET_segMatchOverflow[cursor].edge.stopId = ET_segPair[startSegSeqId].stopId ;
         ET_segMatchOverflow[cursor].next             = ET_pairFreeHead ;

         ET_segPair[startSegSeqId].segPairId       = -cursor ;
         ET_segPair[startSegSeqId].stopId = -32768 ; /* debugging sentinel */

         cursor = ET_pairFreeHead ;
         ++ET_pairFreeHead ;

         pairId = ET_GetNextPairId() ;
         ET_segMatchOverflow[cursor].edge.segPairId       = pairId ;
         ET_segMatchOverflow[cursor].edge.stopId = stopSegSeqId ;
         ET_segMatchOverflow[cursor].next             = 0 ; /* end of chain */
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
         /* This adds a performance hit */
         ET_pairToSeq[pairId].start = startSegSeqId ;
         ET_pairToSeq[pairId].stop  = stopSegSeqId ;
#endif
      }
   }
   else if (ET_segPair[startSegSeqId].segPairId == 0)
   {
      /* This loop has never been encountered */
      pairId = ET_GetNextPairId() ;
      ET_segPair[startSegSeqId].segPairId       = pairId ;
      ET_segPair[startSegSeqId].stopId = stopSegSeqId ;
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
      /* This adds a performance hit */
      ET_pairToSeq[pairId].start = startSegSeqId ;
      ET_pairToSeq[pairId].stop  = stopSegSeqId ;
#endif
   }
   else /* (ET_segPair[startSegSeqId].segPairId < 0) */
   {
      /* This loop is reached from multiple call paths */
      /* search for this callpath in overflow chain */
      ET_Idx_t cursor ;
      ET_Idx_t lastCursor ;
      for (cursor = -ET_segPair[startSegSeqId].segPairId ; cursor != 0;
           cursor = ET_segMatchOverflow[cursor].next)
      {
         lastCursor = cursor ;
         if (ET_segMatchOverflow[cursor].edge.stopId == stopSegSeqId)
         {
            pairId = ET_segMatchOverflow[cursor].edge.segPairId ;
            break ;
         }
      }
      if (cursor == 0)
      {
         /* Add new entry at end of overflow chain. */
         /* (should new entries be added to head because */
         /*  they are more likely to be deeply nested?) */
         cursor = ET_pairFreeHead ;
         ++ET_pairFreeHead ;

         ET_segMatchOverflow[lastCursor].next = cursor ;

         pairId = ET_GetNextPairId() ;
         ET_segMatchOverflow[cursor].edge.segPairId       = pairId ;
         ET_segMatchOverflow[cursor].edge.stopId = stopSegSeqId ;
         ET_segMatchOverflow[cursor].next             = 0 ; /* end of chain */
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
         /* This adds a performance hit */
         ET_pairToSeq[pairId].start = startSegSeqId ;
         ET_pairToSeq[pairId].stop  = stopSegSeqId ;
#endif
      }
   }

   return pairId ;
}

/*******************/
/*   Loop related  */
/*******************/

static ET_Idx_t ET_GetNextLoopId()
{
   static ET_Idx_t ET_loopId ;
   return ++ET_loopId ;
}

#ifndef ET_SIMPLE_LOOP_STATS

static ET_Idx_t ET_GetNextSeqId()
{
   static ET_Idx_t ET_seqId ;
#ifdef ET_DEBUG
   printf("New Sequence %d\n", (int)(ET_seqId+1)) ;
#endif
   return ++ET_seqId ;
}

static struct ET_CallEdge *ET_findSeqNode(ET_Idx_t loopId, ET_Idx_t seqId)
{
   struct ET_CallEdge *retVal = 0 ;
   if (ET_loopSeqId[loopId].seqId == seqId)
   {
      retVal = &ET_loopSeqId[loopId] ;
   }
   else
   {
      ET_Idx_t cursor = -ET_loopSeqId[loopId].seqId ;
      while(cursor != 0)
      {
         if (ET_SeqIdOverflow[cursor].edge.seqId == seqId)
         {
            retVal = &ET_SeqIdOverflow[cursor].edge ;
            break ;
         }

         cursor = ET_SeqIdOverflow[cursor].next ;
      }
   }
   return retVal ;
}

#endif /* ET_SIMPLE_LOOP_STATS */

/***************************/
/*    External Functions   */
/***************************/

/**********************/
/*   Segment related  */
/**********************/

ET_Idx_t ET_RegisterStartSeg(const char *fileName,
                             const char *funcName, int line)
{
  ET_Idx_t segId = ET_GetNextStartSegId() ;
#ifdef ET_DEBUG
   if (segId >= ET_MAX_SEGMENTS)
   {
      printf("Static code contains more than ET_MAX_SEGMENTS(%d) start segments\n",
             (int)ET_MAX_SEGMENTS) ;
      exit(-1) ;
   }
#endif
   ET_startSegLoc[segId].fileName = fileName ;
   ET_startSegLoc[segId].funcName = funcName ;
   ET_startSegLoc[segId].line     = line ;
#ifdef ET_DEBUG
   printf("Loop %d: %s line=%d\n", segId, fileName, line) ;
#endif

   return segId ;
}

ET_Idx_t ET_RegisterStopSeg(const char *fileName,
                            const char *funcName, int line)
{
   ET_Idx_t segId = ET_GetNextStopSegId() ;
#ifdef ET_DEBUG
   if (segId >= ET_MAX_SEGMENTS)
   {
      printf("Static code contains more than ET_MAX_SEGMENTS(%d) stop segments\n",
             (int)ET_MAX_SEGMENTS) ;
      exit(-1) ;
   }
#endif
   ET_stopSegLoc[segId].fileName = fileName ;
   ET_stopSegLoc[segId].funcName = funcName ;
   ET_stopSegLoc[segId].line     = line ;
#ifdef ET_DEBUG
   printf("Loop %d: %s line=%d\n", segId, fileName, line) ;
#endif

   return segId ;
}

void ET_StartSeg(ET_Idx_t segId)
{
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
   ET_currStartSegSeqId = RegisterSeg(ET_startSegId, segId,
                                      ET_loopStack[ET_loopLevel],
                                      ET_GetNextStartSegId,
                                      ET_startSeqToId ) ;
#else
   ET_currStartSegSeqId = RegisterSeg(ET_startSegId, segId,
                                      ET_loopStack[ET_loopLevel],
                                      ET_GetNextStartSegId) ;
#endif
#ifdef ET_PAPI
   PAPI_read(ET_EventSet, &ET_startSegCounter) ;
#else
   ET_startSegCounter = getticks() ;
#endif

   return ;
}

void ET_StopSeg(ET_Idx_t segId)
{
#ifdef ET_PAPI
   long_long stopTime ;
   PAPI_read(ET_EventSet, &stopTime) ;
#else
   ticks stopTime = getticks() ;
#endif
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
   ET_Idx_t stopSegSeqId = RegisterSeg(ET_stopSegId, segId,
                                       ET_loopStack[ET_loopLevel],
                                       ET_GetNextStopSegId,
                                       ET_stopSeqToId ) ;
#else
   ET_Idx_t stopSegSeqId = RegisterSeg(ET_stopSegId, segId,
                                       ET_loopStack[ET_loopLevel],
                                       ET_GetNextStopSegId) ;
#endif
   ET_Idx_t pairId = RegisterPair(ET_currStartSegSeqId, stopSegSeqId) ;

#ifdef ET_PAPI
   ET_segStat[pairId] += stopTime - ET_startSegCounter ;
#else
   ET_segStat[pairId] += elapsed(stopTime, ET_startSegCounter) ;
#endif

   return ;
}

/*******************/
/*   Loop related  */
/*******************/

ET_Idx_t ET_RegisterLoop(const char *fileName, const char *funcName, int line)
{
   ET_Idx_t loopId = ET_GetNextLoopId() ;
#ifdef ET_DEBUG
   if (loopId >= ET_MAX_LOOPS)
   {
      printf("Static code contains more than ET_MAX_LOOPS(%d)\n",
             (int)ET_MAX_LOOPS) ;
      exit(-1) ;
   }
#endif
   ET_loopLoc[loopId].fileName = fileName ;
   ET_loopLoc[loopId].funcName = funcName ;
   ET_loopLoc[loopId].line     = line ;
#ifdef ET_DEBUG
   printf("Loop %d: %s line=%d\n", loopId, fileName, line) ;
#endif

   return loopId ;
}

void ET_PushLoopSeqId(ET_Idx_t loopId)
{
#ifndef ET_SIMPLE_LOOP_STATS
   ET_Idx_t seqId ;            /* return value */
   ET_Idx_t parentSeqId ;

   /* given a loop ID and a parent Seqence number, we can derive */
   /* a local sequence number */

   parentSeqId = (ET_loopLevel != 0) ?
                 ET_loopStack[ET_loopLevel] : --ET_loopStack[0] ;

   /******************************/
   /*    Generate Sequence ID    */
   /******************************/
   if (ET_loopSeqId[loopId].seqId > 0) /* common case first */
   {
      /* A unique call path reaches this loop */
      if (ET_loopSeqId[loopId].parentSeqId == parentSeqId)
      {
         seqId = ET_loopSeqId[loopId].seqId ;
      }
      else
      {
         /* Create New Overflow Chain */
         ET_Idx_t cursor = ET_freeHead ;
         ++ET_freeHead ; /* advance to next empty entry */

         ET_SeqIdOverflow[cursor].edge.seqId       = ET_loopSeqId[loopId].seqId ;
         ET_SeqIdOverflow[cursor].edge.parentSeqId = ET_loopSeqId[loopId].parentSeqId ;
         ET_SeqIdOverflow[cursor].next             = ET_freeHead ;
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
         ET_SeqIdOverflow[cursor].edge.level       = ET_loopSeqId[loopId].level ;
#endif
         ET_loopSeqId[loopId].seqId       = -cursor ;
         ET_loopSeqId[loopId].parentSeqId = -32768 ; /* debugging sentinel */

         cursor = ET_freeHead ;
         ++ET_freeHead ;

         seqId = ET_GetNextSeqId() ;
         ET_SeqIdOverflow[cursor].edge.seqId       = seqId ;
         ET_SeqIdOverflow[cursor].edge.parentSeqId = parentSeqId ;
         ET_SeqIdOverflow[cursor].next             = 0 ; /* end of chain */
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
         ET_SeqIdOverflow[cursor].edge.level       = ET_loopLevel ;
         /* This adds a performance hit */
         ET_seqToLoop[seqId] = loopId ;
#endif
      }
   }
   else if (ET_loopSeqId[loopId].seqId == 0)
   {
      /* This loop has never been encountered */
      seqId = ET_GetNextSeqId() ;
      ET_loopSeqId[loopId].seqId       = seqId ;
      ET_loopSeqId[loopId].parentSeqId = parentSeqId ;
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
      ET_loopSeqId[loopId].level       = ET_loopLevel ;
      /* This adds a performance hit */
      ET_seqToLoop[seqId] = loopId ;
#endif
   }
   else /* (ET_loopSeqId[loopId].seqId < 0) */
   {
      /* This loop is reached from multiple call paths */
      /* search for this callpath in overflow chain */
      ET_Idx_t cursor ;
      ET_Idx_t lastCursor ;
      for (cursor = -ET_loopSeqId[loopId].seqId ; cursor != 0;
           cursor = ET_SeqIdOverflow[cursor].next)
      {
         lastCursor = cursor ;
         if (ET_SeqIdOverflow[cursor].edge.parentSeqId == parentSeqId)
         {
            seqId = ET_SeqIdOverflow[cursor].edge.seqId ;
            break ;
         }
      }
      if (cursor == 0)
      {
         /* Add new entry at end of overflow chain. */
         /* (should new entries be added to head because */
         /*  they are more likely to be deeply nested?) */
         cursor = ET_freeHead ;
         ++ET_freeHead ;

         ET_SeqIdOverflow[lastCursor].next = cursor ;

         seqId = ET_GetNextSeqId() ;
         ET_SeqIdOverflow[cursor].edge.seqId       = seqId ;
         ET_SeqIdOverflow[cursor].edge.parentSeqId = parentSeqId ;
         ET_SeqIdOverflow[cursor].next             = 0 ; /* end of chain */
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
         ET_SeqIdOverflow[cursor].edge.level       = ET_loopLevel ;
         /* This adds a performance hit */
         ET_seqToLoop[seqId] = loopId ;
#endif
      }
   }

   /***************************/
   /*  Push Sequence Context  */
   /***************************/
   ++ET_loopLevel ;
   ET_loopStack[ET_loopLevel] = seqId ;
#else /* ET_SIMPLE_LOOP_STATS */
   ++ET_loopLevel ;
   ET_loopStack[ET_loopLevel] = loopId ;
#endif /* ET_SIMPLE_LOOP_STATS */

#ifdef ET_PAPI
   PAPI_read(ET_EventSet, &ET_loopTime[ET_loopLevel]) ;
#else
   ET_loopTime[ET_loopLevel] = getticks() ;
#endif

#ifdef ET_DEBUG
   printf("Entering Loop %d, Seq %d\n", (int)loopId, (int)seqId) ;
#endif

   return ;
}

#ifndef ET_NO_COUNT_ITERS
void ET_PopLoopSeqId(int iterations)
#else
void ET_PopLoopSeqId()
#endif
{
#ifdef ET_PAPI
   long_long stopTime ;
   PAPI_read(ET_EventSet, &stopTime) ;
#else
   ticks stopTime = getticks() ;
#endif

   ET_Idx_t idx ;
#if 0
   This code needs TLC since ET_PopLoopSeqId(int loopId) is no longer the API.

   /* loopId is not really needed... */
   /* it just allows for a check that loops are properly nested */
   int err = 0 ;
   int seqId = 0 ;
   if (ET_loopSeqId[loopId].seqId > 0)
   {
      if (ET_loopSeqId[loopId].seqId != ET_loopStack[ET_loopLevel])
      {
         err = 1 ;
      }
      else
      {
         seqId = ET_loopSeqId[loopId].seqId ;
      }
   }
   else
   {
      ET_Idx_t cursor ;
      for (cursor = -ET_loopSeqId[loopId].seqId; cursor != 0;
           cursor = ET_SeqIdOverflow[cursor].next)
      {
         if(ET_SeqIdOverflow[cursor].edge.seqId == ET_loopStack[ET_loopLevel])
            break ;
      }
      if (cursor == 0)
      {
         err = 1 ;
      }
      else
      {
         seqId = ET_SeqIdOverflow[cursor].edge.seqId ;
      }
   }
   if (err)
   {
      printf("loops not properly nested: %s (%d)\n",
             ET_loopLoc[loopId].fileName, ET_loopLoc[loopId].line) ;
   }
   printf("Exiting Loop %d, Seq %d\n", (int)loopId, (int)seqId) ;
#endif
   idx = ET_loopStack[ET_loopLevel] ;
#ifdef ET_PAPI
  ET_loopStat[idx] += stopTime - ET_loopTime[ET_loopLevel] ;
#else
   ET_loopStat[idx] +=
      elapsed(stopTime,ET_loopTime[ET_loopLevel]) ;
#endif
#ifndef ET_NO_COUNT_ITERS
   ET_loopIter[idx] += iterations ;
#endif
   --ET_loopLevel ;
}

/* used to escape several levels of scope at once */
void ET_PopLoopSeqIds(int levels)
{
#ifdef ET_RECORD_STATS
   int i ;
#ifdef ET_PAPI
   long_long stop ;
   PAPI_read(ET_EventSet, &stop) ;
#else
   ticks stop = getticks() ;
#endif

   for (i=0; i<levels; ++i)
   {
      /* Currently, don't record times or iterations for aborted loops. */

#ifdef ET_PAPI
      ET_loopStat[ET_loopStack[ET_loopLevel]] +=
         stop - ET_loopTime[ET_loopLevel] ;
#else
      ET_loopStat[ET_loopStack[ET_loopLevel]] +=
         elapsed(stop,ET_loopTime[ET_loopLevel]) ;
#endif
      --ET_loopLevel ;
   }
#else
   ET_loopLevel -= levels ;
#endif
}

/*************************************************/
/*                Post-processing                */
/*************************************************/

#define NIL 0

#ifndef ET_SIMPLE_LOOP_STATS

#ifdef ET_POST_PROCESS_SEQ_TO_LOOP
#define ET_TypeNone     0
#define ET_TypeLoop     1
#define ET_TypeStartSeg 2
#define ET_TypeStopSeg  3

struct ET_TreeNode
{
   ET_Idx_t seqId ;  /* loop or (start)segment sequence Id */
   ET_Idx_t id ;     /* loop or (start)segment Id */
   struct ET_TreeNode **child ;
   int numChildren ;
   int type ;
} ;

struct ET_HashChain {
   struct ET_TreeNode  *entry ;
   struct ET_HashChain *next ;
} ;

struct ET_HashTable {
   struct ET_HashChain **chainHead ;
   int                   size ;
} ;

static struct ET_HashTable *ET_HashCreate(int tableSize)
{
   struct ET_HashTable *retVal =
      (struct ET_HashTable *) malloc(sizeof(struct ET_HashTable)) ;

   retVal->size = tableSize ;
   retVal->chainHead = 
      (struct ET_HashChain **) calloc(retVal->size, sizeof(struct ET_HashChain *)) ;

   return retVal ;
}

static unsigned ET_HashFunction(int seqId, int tableSize)
{
   return (seqId % tableSize) ;
}

static void ET_HashInsert(struct ET_HashTable *hashTable, struct ET_TreeNode *item)
{
   unsigned hashVal = ET_HashFunction(item->seqId, hashTable->size) ;

   struct ET_HashChain *node = (struct ET_HashChain *) malloc(sizeof(struct ET_HashChain)) ;
   node->entry = item ;
   node->next = NIL ;

   if (hashTable->chainHead[hashVal] == NIL) {
      hashTable->chainHead[hashVal] = node ;
   }
   else {
      /* Add new entries to the end of the list.  this
         eliminates some annoying restart diffs that occurred before
         when new nodesets were added to the beginning of the list. */

      struct ET_HashChain *currNode ;
      currNode = hashTable->chainHead[hashVal] ;
      while ((item->seqId != currNode->entry->seqId) && (currNode->next != NIL)) {
         currNode = currNode->next ;
      }
      if (item->seqId == currNode->entry->seqId) {
         printf("ET_HashInsert: Node inserted twice\n") ;
         exit(-1) ;
      }
      else {
         currNode->next = node ;
      }
   }

   return ;
}

static struct ET_TreeNode *ET_HashFind(struct ET_HashTable *hashTable, ET_Idx_t seqId)
{
   struct ET_TreeNode *retVal = NIL ;
   unsigned hashVal = ET_HashFunction(seqId, hashTable->size) ;
   struct ET_HashChain *currNode = hashTable->chainHead[hashVal] ;

   while ((currNode != NIL) && (seqId != currNode->entry->seqId)) {
      currNode = currNode->next ;
   }
   if (currNode != NIL) {
      retVal = currNode->entry ;
   }

   return retVal ;
}

static struct ET_TreeNode *ET_TreeNodeCreate(int type, ET_Idx_t seqId, ET_Idx_t id)
{
   struct ET_TreeNode *retVal = (struct ET_TreeNode *) malloc(sizeof(struct ET_TreeNode)) ;
   retVal->type  = type ;
   retVal->seqId = seqId ;
   retVal->id    = id ;
   retVal->numChildren = 0 ;
   retVal->child = 0 ;

   return retVal ;
}

static void ET_TreeNodeAddChild(struct ET_TreeNode *parent, struct ET_TreeNode *child)
{
   int i ;
   int j ;
   for (i=0; i < parent->numChildren; ++i) {
      /* keep in mind -- loops are processed to completion, then segments are added */
      if (parent->type == ET_TypeLoop) {
         if (child->seqId < parent->child[i]->seqId) {
            break ;
         }
      }
      else /* parent->type == ET_TypeStartSeg */{
      }
   }
   parent->child = (struct ET_TreeNode **)
      realloc(parent->child, (parent->numChildren+1)*sizeof(struct ET_TreeNode *)) ;
   for (j=parent->numChildren; j>i; --j) {
      parent->child[j] = parent->child[j-1] ;
   }
   parent->child[i] = child ;
   ++parent->numChildren ;

   return ;
}

static void ET_TreeNodeAdd(struct ET_HashTable *hashTable,
                           struct ET_CallEdge *item, int id, int type)
{
   ET_Idx_t seqId       = item->seqId ;
   ET_Idx_t parentSeqId = item->parentSeqId ;
   struct ET_TreeNode *node ;

   if ((node = ET_HashFind(hashTable, seqId)) != NIL) {
      if (node->id == -1) {
         node->id = id ;
      }
      else if (node->id != id) {
         printf("ET_TreeNodeAdd:  Tree construction error\n") ;
         exit(-1) ;
      }
   }
   else /* seqId not registered in Hash Table */ {
      node = ET_TreeNodeCreate(type, seqId, id) ;
      ET_HashInsert(hashTable, node) ;
   }
   if (parentSeqId <= 0) {
      struct ET_TreeNode *root = ET_HashFind(hashTable, 0) ;
      ET_TreeNodeAddChild(root, node) ;
   }
   else {
      struct ET_TreeNode *parentNode ;
      if ((parentNode = ET_HashFind(hashTable, parentSeqId)) == 0) {
         parentNode = ET_TreeNodeCreate(ET_TypeLoop, parentSeqId, -1) ;
         ET_HashInsert(hashTable, parentNode) ;
      }
      ET_TreeNodeAddChild(parentNode, node) ;
   }
}

static struct ET_TreeNode *ET_CreateTree(int numLoops)
{
   ET_Idx_t loopId ;
   int size = numLoops/4 ;
   struct ET_HashTable *hashTable = ET_HashCreate(size) ;
   struct ET_TreeNode *root = ET_TreeNodeCreate(ET_TypeLoop, 0, 0) ;
   ET_HashInsert(hashTable, root) ;

   for (loopId = 1; loopId<=numLoops; ++loopId) {
      if (ET_loopSeqId[loopId].seqId > 0) {
         ET_TreeNodeAdd(hashTable, &ET_loopSeqId[loopId], loopId, ET_TypeLoop) ;
      }
      else {
         ET_Idx_t i ;
         for (i = -ET_loopSeqId[loopId].seqId; i != 0; i=ET_SeqIdOverflow[i].next) {
            ET_TreeNodeAdd(hashTable, &ET_SeqIdOverflow[i].edge, loopId, ET_TypeLoop) ;
         }
      }
   }
   return root ;
}

static void ET_ShowTree(struct ET_TreeNode *tree, int level)
{
   int i ;
   if (level != -1) {
#ifndef ET_NO_COUNT_ITERS
#ifdef ET_PAPI
      printf("%*cL %5d%*c%14lld  %10d\n", level+1, ' ',
#else
      printf("%*cL %5d%*c%15.1f  %10d\n", level+1, ' ',
#endif
             (int)tree->id, 60-(level+1), ' ',
             ET_loopStat[tree->seqId], ET_loopIter[tree->seqId]) ;
#else
#ifdef ET_PAPI
      printf("%*cL %5d%*c%14lld\n", level+1, ' ',
#else
      printf("%*cL %5d%*c%15.1f\n", level+1, ' ',
#endif
             (int)tree->id, 60-(level+1), ' ',
             ET_loopStat[tree->seqId]) ;
#endif
   }

   for (i=0; i<tree->numChildren; ++i) {
      ET_ShowTree(tree->child[i], level+1) ;
   }

   return ;
}

#endif

void ET_LogStats()
{
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
   int i ;
   int numLoops = ET_GetNextSeqId() ;
   int numPairs = ET_GetNextPairId() ;
   for (i=1; i<numLoops; ++i)
   {
      int loopId = ET_seqToLoop[i] ;
#ifndef ET_NO_COUNT_ITERS
#ifdef  ET_PAPI
      printf("%14lld %10d %s %d %s\n",
#else
      printf("%16.2f %10d %s %d %s\n",
#endif
             ET_loopStat[i], ET_loopIter[i],
             ET_loopLoc[loopId].fileName,
             ET_loopLoc[loopId].line,
             ET_loopLoc[loopId].funcName ) ;
#else
#ifdef  ET_PAPI
      printf("%14lld %s %d %s\n",
#else
      printf("%16.2f %s %d %s\n",
#endif
             ET_loopStat[i],
             ET_loopLoc[loopId].fileName,
             ET_loopLoc[loopId].line,
             ET_loopLoc[loopId].funcName ) ;
#endif
   }
   for (i=1; i<numPairs; ++i)
   {
      int segStart = ET_startSeqToId[ET_pairToSeq[i].start] ;
      int segStop  = ET_stopSeqToId[ET_pairToSeq[i].stop] ;
      
#ifdef  ET_PAPI
      printf("%14lld\n", ET_segStat[i]) ;
#else
      printf("%16f\n", ET_segStat[i]) ;
#endif
      printf("                %s %d %s\n",
             ET_startSegLoc[segStart].fileName,
             ET_startSegLoc[segStart].line,
             ET_startSegLoc[segStart].funcName ) ;
      printf("                %s %d %s\n", 
             ET_stopSegLoc[segStop].fileName,
             ET_stopSegLoc[segStop].line,
             ET_stopSegLoc[segStop].funcName ) ;
   }
#else
   /* Do not modify ShowTree to handle this output. */
   /* That could accidentally lead to a security error. */
   printf("ET_Logstats() currently unavailable as a post-processing feature.\n") ;
#endif
}

void ET_Dump()
{
   int i ;
   int numLoops = ET_GetNextSeqId() ;
#ifndef ET_POST_PROCESS_SEQ_TO_LOOP
   for (i=1; i<numLoops; ++i)
   {
      int loopId = ET_seqToLoop[i] ;
      struct ET_CallEdge *node = ET_findSeqNode(loopId, i) ;
#ifndef ET_NO_COUNT_ITERS
#ifdef ET_PAPI
      printf("%*cL %5d%*c%14lld  %10d\n", node->level+1, ' ',
#else
      printf("%*cL %5d%*c%15.1f  %10d\n", node->level+1, ' ',
#endif
             (int)loopId, 60-(node->level+1), ' ',
             ET_loopStat[i], ET_loopIter[i]) ;
#else
#ifdef ET_PAPI
      printf("%*cL %5d%*c%14lld\n", node->level+1, ' ',
#else
      printf("%*cL %5d%*c%15.1f\n", node->level+1, ' ',
#endif
             (int)loopId, 60-(node->level+1), ' ',
             ET_loopStat[i]) ;
#endif
   }
#else
   struct ET_TreeNode *tree = ET_CreateTree(numLoops-1) ;
   ET_ShowTree(tree, -1) ;
#endif
}

void ET_Summary()
{
   printf("\nLoops = %d, Sequence Points = %d\n",
          (int) (ET_GetNextLoopId()-1), (int) (ET_GetNextSeqId()-1) ) ;
}

#else /* ET_SIMPLE_LOOP_STATS */

void ET_LogStats()
{
   int loopId ;
   int numLoops = ET_GetNextLoopId() ;
   for (loopId=1; loopId<numLoops; ++loopId)
   {
#ifndef ET_NO_COUNT_ITERS
#ifdef  ET_PAPI
      printf("%14lld %10d %s %d %s\n",
#else
      printf("%16.2f %10d %s %d %s\n",
#endif
             ET_loopStat[loopId], ET_loopIter[loopId],
             ET_loopLoc[loopId].fileName,
             ET_loopLoc[loopId].line,
             ET_loopLoc[loopId].funcName ) ;
#else
#ifdef  ET_PAPI
      printf("%14lld %s %d %s\n",
#else
      printf("%16.2f %s %d %s\n",
#endif
             ET_loopStat[loopId],
             ET_loopLoc[loopId].fileName,
             ET_loopLoc[loopId].line,
             ET_loopLoc[loopId].funcName ) ;
#endif
   }
}

void ET_Dump()
{
   int loopId ;
   int numLoops = ET_GetNextLoopId() ;
   for (loopId=1; loopId<numLoops; ++loopId)
   {
#ifndef ET_NO_COUNT_ITERS
#ifdef ET_PAPI
      printf("L %5d   %14lld  %10d\n",
#else
      printf("L %5d   %15.1f  %10d\n",
#endif
             (int)loopId, ET_loopStat[loopId], ET_loopIter[loopId]) ;
#else
#ifdef ET_PAPI
      printf("L %5d   %14lld\n",
#else
      printf("L %5d   %15.1f\n",
#endif
             (int)loopId, ET_loopStat[loopId]) ;
#endif
   }
}

void ET_Summary()
{
   printf("\nLoops = %d\n", (int) (ET_GetNextLoopId()-1) ) ;
}

#endif /* ET_SIMPLE_LOOP_STATS */

void ET_SanityCheck()
{
   if (ET_loopLevel != 0)
   {
      printf("\nLoops not properly nested. Nest level is %d\n", ET_loopLevel) ;
   }
}

void ET_Init()
{
#ifdef ET_PAPI
   int retval ;
   retval = PAPI_library_init(PAPI_VER_CURRENT);
   if (retval != PAPI_VER_CURRENT) {
      printf("Papi not initialized properly\n") ;
      exit(-1) ;
   }
   retval = PAPI_create_eventset(&ET_EventSet);
   if (retval != PAPI_OK) {
      printf("Papi event set not created\n") ;
      exit(-1) ;
   }
   retval = PAPI_add_event(ET_EventSet, PAPI_TOT_INS);
   if (retval != PAPI_OK) {
      printf("PAPI_TOT_INS not available\n") ;
      exit(-1) ;
   }
   retval = PAPI_start(ET_EventSet);
   if (retval != PAPI_OK) {
      printf("Papi count not start counter for PAPI_TOT_INS\n") ;
      exit(-1) ;
   }
#endif
}

#ifdef __cplusplus
}  /* end of extern "C" */
#endif

