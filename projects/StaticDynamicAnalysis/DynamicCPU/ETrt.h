#ifndef __ETRT__

#ifdef __cplusplus
extern "C" {
#endif

/* It may be higher performance to use an int datatype */
typedef int ET_Idx_t ;  

/* Not thread safe */
ET_Idx_t ET_RegisterLoop(const char *fileName, const char *funcName, int line) ;
ET_Idx_t ET_RegisterStartSeg(const char *fileName, const char *funcName, int line) ;
ET_Idx_t ET_RegisterStopSeg(const char *fileName, const char *funcName, int line) ;
void ET_PushLoopSeqId(ET_Idx_t loopId) ;
#ifndef ET_NO_COUNT_ITERS
void ET_PopLoopSeqId(int iterations) ;
#else
void ET_PopLoopSeqId() ;
#endif
void ET_PopLoopSeqIds(int levels) ;
void ET_StartSeg(ET_Idx_t segId) ;
void ET_StopSeg(ET_Idx_t segId) ;
void ET_StatAccum(ET_Idx_t seqId, double stat) ;
void ET_LogStats() ;
void ET_Dump() ;
void ET_SanityCheck() ;
void ET_Summary() ;
void ET_Init() ;

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#define __ETRT__
#endif
