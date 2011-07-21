#ifndef __MPI_NON_BLOCKING_CODE_MOTION_H__
#define __MPI_NON_BLOCKING_CODE_MOTION_H__


#include "DependenceGraph.h"
#include "MotionInformation.h"
#include "CallGraphQuery.h"
#include <list>
#include <vector>

namespace MPIOptimization 
{
        /**************************************************************
        * Class Motion Information
        *
        *
        **************************************************************/
        class MPINonBlockingCodeMotion{

        public:
                MPINonBlockingCodeMotion(SgProject* proj);
                ~MPINonBlockingCodeMotion();

                int run();
                void retrieveMPICalls();
                void retrieveMPICalls2();
                void traverseCallGraph();

        private:
                MPINonBlockingCodeMotion() {};

        protected:
                SgProject* project;
                SystemDependenceGraph* sdg;
                std::list<MotionInformation*> motionInfoList;
                CallGraphQuery* cgQuery;

                std::list<SgExprStatement*> mpiFunctionCalls;
                std::list<SgExprStatement*> mpiSendFunctionCallList;
                std::list<SgExprStatement*> mpiRecvFunctionCallList;
                std::list<SgExprStatement*> mpiCommFuncCallList;

        protected:
                void moveCode(SgNode* from, SgNode* to, bool insertAfterDest);
                void sliceOutAsNewBlock(BlockMotionInformation* blockInfo);
                void cleanUpAfterMove(SgNode* startPath);

                bool isMovableOutside(SgBasicBlock* block, SgNode* statement);

                std::list<SgExprStatement*> getMPIFunctionCalls() { return mpiFunctionCalls; }
                std::list<SgExprStatement*> getMPISendFunctionCallList() { return mpiSendFunctionCallList; }
                std::list<SgExprStatement*> getMPIRecvFunctionCallList() { return mpiRecvFunctionCallList; }
                std::list<SgExprStatement*> getMPICommFuncCallList() { return mpiCommFuncCallList; }
                std::string getFunctionCallName(SgExprStatement* expr);

                void processStatementMotion(MotionInformation* info);
                void processBlockMotion(BlockMotionInformation* blockInfo);
                void processCallSiteMotion(MotionInformation* info);
                bool isScopeStatement(SgNode* node);
        };
}

#endif //__MPI_NON_BLOCKING_CODE_MOTION_H__
