#ifndef __ETRT__

/* It may be higher performance to use an int datatype */
typedef short ET_Idx_t ;  

/* Not thread safe */
ET_Idx_t ET_RegisterLoop(const char *fileName, const char *funcName, int line) ;
ET_Idx_t ET_RegisterStartSeg(const char *fileName, const char *funcName, int line) ;
ET_Idx_t ET_RegisterStopSeg(const char *fileName, const char *funcName, int line) ;
void ET_PushLoopSeqId(ET_Idx_t loopId) ;
void ET_PopLoopSeqId(int iterations) ;
void ET_PopLoopSeqIds(int levels) ;
void ET_StartSeg(ET_Idx_t segId) ;
void ET_StopSeg(ET_Idx_t segId) ;
void ET_StatAccum(ET_Idx_t seqId, double stat) ;
void ET_LogStats() ;
void ET_SanityCheck() ;
void ET_Summary() ;
void ET_Init() ;

#define __ETRT__
#endif
