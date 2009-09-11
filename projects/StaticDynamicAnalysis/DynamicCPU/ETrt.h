#ifndef __ETRT__

/*******************************************************/
/* Exascale Trace -- gather loop level information     */
/*                   for exascale architecture design. */
/*******************************************************/

#if 0
#define ET_DEBUG 1
#endif

/* It may be higher performance to use an int datatype */
typedef short ET_Idx_t ;  

/* Not thread safe */
ET_Idx_t ET_RegisterLoop(const char *fileName, const char *funcName, int line) ;
ET_Idx_t ET_RegisterStartSeg(const char *fileName, const char *funcName, int line) ;
ET_Idx_t ET_RegisterStopSeg(const char *fileName, const char *funcName, int line) ;
void ET_PushLoopSeqId(ET_Idx_t loopId) ;
#ifdef ET_DEBUG
void ET_PopLoopSeqId(int loopId) ;
#else
void ET_PopLoopSeqId() ;
#endif
void ET_PopLoopSeqIds(int levels) ;
void ET_StartSeg(ET_Idx_t segId) ;
void ET_StopSeg(ET_Idx_t segId) ;
void ET_StatAccum(ET_Idx_t seqId, double stat) ;
void ET_LogStats() ;
void ET_SanityCheck() ;
void ET_Summary() ;

#define __ETRT__
#endif
