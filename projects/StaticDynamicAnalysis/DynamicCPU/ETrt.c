
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
#include "cycle.h"

/****************/
/*    Macros    */
/****************/

#define ET_MAX_LOOPS              8190
#define ET_MAX_LOOP_SEQ_OVERFLOW  (ET_MAX_LOOPS+(ET_MAX_LOOPS/2))
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

/* for each loop, keep a pointer to __FILE__ and __LINE__ info */
/* This information should be user selectable, with respect to */
/* input source or output source */
static struct ET_LocInfo ET_loopLoc[ET_MAX_LOOPS] ;

struct ET_CallEdge
{
   ET_Idx_t seqId ;
   ET_Idx_t parentSeqId ;
   int level ;
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
} ET_SeqIdOverlow[ET_MAX_LOOP_SEQ_OVERFLOW] ;

/* Holds a sequence number cursor for each loop level */
/* Need to be careful here, because the code may */
/* contain recursion. */
static int ET_loopLevel ;
static ET_Idx_t ET_loopStack[ET_MAX_LOOP_DEPTH] ;
static ticks ET_loopTime[ET_MAX_LOOP_DEPTH] ;

/* this is purely for convenience */
static ET_Idx_t ET_seqToLoop[ET_MAX_SEQ_LOOP_MAP] ;

#if 1
#else
#define POST_PROCESS_SEQ_TO_LOOP
#endif

/****************/
/*  Loop Stats  */
/****************/

/* Accumulate information for loop Sequence IDs */
static double ET_loopStat[ET_MAX_LOOP_SEQ_OVERFLOW] ;

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
static ticks ET_startSegCounter ;

/* this is purely for convenience */
static ET_Idx_t ET_startSeqToId[ET_MAX_SEQ_LOOP_MAP] ;
static ET_Idx_t ET_stopSeqToId[ET_MAX_SEQ_LOOP_MAP] ;

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
static double ET_segStat[ET_MAX_SEGMENTS] ;

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

static ET_Idx_t RegisterSeg(struct ET_SegId *baseSegId, ET_Idx_t segId,
                            ET_Idx_t loopContext, ET_Idx_t (*seqFunc)(),
                            ET_Idx_t *ET_seqToId )
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
#ifndef POST_PROCESS_SEQ_TO_LOOP
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
#ifndef POST_PROCESS_SEQ_TO_LOOP
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
#ifndef POST_PROCESS_SEQ_TO_LOOP
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
#ifndef POST_PROCESS_SEQ_TO_LOOP
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
#ifndef POST_PROCESS_SEQ_TO_LOOP
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
#ifndef POST_PROCESS_SEQ_TO_LOOP
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
         if (ET_SeqIdOverlow[cursor].edge.seqId == seqId)
         {
            retVal = &ET_SeqIdOverlow[cursor].edge ;
            break ;
         }

         cursor = ET_SeqIdOverlow[cursor].next ;
      }
   }
   return retVal ;
}

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
   ET_currStartSegSeqId = RegisterSeg(ET_startSegId, segId,
                                      ET_loopStack[ET_loopLevel],
                                      ET_GetNextStartSegId,
                                      ET_startSeqToId ) ;
   ET_startSegCounter = getticks() ;

   return ;
}

void ET_StopSeg(ET_Idx_t segId)
{
   ticks stopTime = getticks() ;
   ET_Idx_t stopSegSeqId = RegisterSeg(ET_stopSegId, segId,
                                       ET_loopStack[ET_loopLevel],
                                       ET_GetNextStopSegId,
                                       ET_stopSeqToId ) ;
   ET_Idx_t pairId = RegisterPair(ET_currStartSegSeqId, stopSegSeqId) ;

   ET_segStat[pairId] += elapsed(stopTime, ET_startSegCounter) ;

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

         ET_SeqIdOverlow[cursor].edge.seqId       = ET_loopSeqId[loopId].seqId ;
         ET_SeqIdOverlow[cursor].edge.parentSeqId = ET_loopSeqId[loopId].parentSeqId ;
         ET_SeqIdOverlow[cursor].edge.level       = ET_loopSeqId[loopId].level ;
         ET_SeqIdOverlow[cursor].next             = ET_freeHead ;

         ET_loopSeqId[loopId].seqId       = -cursor ;
         ET_loopSeqId[loopId].parentSeqId = -32768 ; /* debugging sentinel */

         cursor = ET_freeHead ;
         ++ET_freeHead ;

         seqId = ET_GetNextSeqId() ;
         ET_SeqIdOverlow[cursor].edge.seqId       = seqId ;
         ET_SeqIdOverlow[cursor].edge.parentSeqId = parentSeqId ;
         ET_SeqIdOverlow[cursor].edge.level       = ET_loopLevel ;
         ET_SeqIdOverlow[cursor].next             = 0 ; /* end of chain */
#ifndef POST_PROCESS_SEQ_TO_LOOP
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
      ET_loopSeqId[loopId].level       = ET_loopLevel ;
#ifndef POST_PROCESS_SEQ_TO_LOOP
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
           cursor = ET_SeqIdOverlow[cursor].next)
      {
         lastCursor = cursor ;
         if (ET_SeqIdOverlow[cursor].edge.parentSeqId == parentSeqId)
         {
            seqId = ET_SeqIdOverlow[cursor].edge.seqId ;
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

         ET_SeqIdOverlow[lastCursor].next = cursor ;

         seqId = ET_GetNextSeqId() ;
         ET_SeqIdOverlow[cursor].edge.seqId       = seqId ;
         ET_SeqIdOverlow[cursor].edge.parentSeqId = parentSeqId ;
         ET_SeqIdOverlow[cursor].edge.level       = ET_loopLevel ;
         ET_SeqIdOverlow[cursor].next             = 0 ; /* end of chain */
#ifndef POST_PROCESS_SEQ_TO_LOOP
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
   ET_loopTime[ET_loopLevel]  = getticks() ;

#ifdef ET_DEBUG
   printf("Entering Loop %d, Seq %d\n", (int)loopId, (int)seqId) ;
#endif

   return ;
}

#ifdef ET_DEBUG
void ET_PopLoopSeqId(int loopId)
#else
void ET_PopLoopSeqId()
#endif
{
#ifdef ET_DEBUG
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
           cursor = ET_SeqIdOverlow[cursor].next)
      {
         if(ET_SeqIdOverlow[cursor].edge.seqId == ET_loopStack[ET_loopLevel])
            break ;
      }
      if (cursor == 0)
      {
         err = 1 ;
      }
      else
      {
         seqId = ET_SeqIdOverlow[cursor].edge.seqId ;
      }
   }
   if (err)
   {
      printf("loops not properly nested: %s (%d)\n",
             ET_loopLoc[loopId].fileName, ET_loopLoc[loopId].line) ;
   }
   printf("Exiting Loop %d, Seq %d\n", (int)loopId, (int)seqId) ;
#endif
   ET_loopStat[ET_loopStack[ET_loopLevel]] +=
      elapsed(getticks(),ET_loopTime[ET_loopLevel]) ;
   --ET_loopLevel ;
}

/* used to escape several levels of scope at once */
void ET_PopLoopSeqIds(int levels)
{
   int i ;
   ticks stop = getticks() ;

   for (i=0; i<levels; ++i)
   {
      ET_loopStat[ET_loopStack[ET_loopLevel]] +=
         elapsed(stop,ET_loopTime[ET_loopLevel]) ;
      --ET_loopLevel ;
   }
}

void ET_LogStats()
{
   int i ;
   int numLoops = ET_GetNextSeqId() ;
   int numPairs = ET_GetNextPairId() ;
   for (i=1; i<numLoops; ++i)
   {
      int loopId = ET_seqToLoop[i] ;
      printf("%16.2f %s %d %s\n", ET_loopStat[i],
             ET_loopLoc[loopId].fileName,
             ET_loopLoc[loopId].line,
             ET_loopLoc[loopId].funcName ) ;
   }
   for (i=1; i<numPairs; ++i)
   {
      int segStart = ET_startSeqToId[ET_pairToSeq[i].start] ;
      int segStop  = ET_stopSeqToId[ET_pairToSeq[i].stop] ;
      
      printf("%16f\n", ET_segStat[i]) ;
      printf("                %s %d %s\n",
             ET_startSegLoc[segStart].fileName,
             ET_startSegLoc[segStart].line,
             ET_startSegLoc[segStart].funcName ) ;
      printf("                %s %d %s\n", 
             ET_stopSegLoc[segStop].fileName,
             ET_stopSegLoc[segStop].line,
             ET_stopSegLoc[segStop].funcName ) ;
   }
}

void ET_SanityCheck()
{
   if (ET_loopLevel != 0)
   {
      printf("\nLoops not properly nested. Nest level is %d\n", ET_loopLevel) ;
   }
}

void ET_Summary()
{
   printf("\nLoops = %d, Sequence Points = %d\n",
          (int) (ET_GetNextLoopId()-1), (int) (ET_GetNextSeqId()-1) ) ;
}

void ET_Dump()
{
   int i ;
   int numLoops = ET_GetNextSeqId() ;
   for (i=1; i<numLoops; ++i)
   {
      int loopId = ET_seqToLoop[i] ;
      struct ET_CallEdge *node = ET_findSeqNode(loopId, i) ;
      printf("%*cL %5d%*c%15.1f\n", node->level+1, ' ',
             (int)loopId, 25-(node->level+1), ' ', ET_loopStat[i]) ;
   }
}
