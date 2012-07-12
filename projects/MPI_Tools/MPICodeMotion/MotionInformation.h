#ifndef __MOTION_INFORMATION_H__
#define __MOTION_INFORMATION_H__


#include "DependenceGraph.h"
#include "CallGraphQuery.h"

namespace MPIOptimization
{
        /**************************************************************
        * Class Motion Information
        *
        *
        **************************************************************/
        class MotionInformation
        {
                public:
                        MotionInformation(SgExprStatement* _src, CallGraphQuery* _cgQuery);
                        virtual ~MotionInformation();

                protected :
                        MotionInformation() {}

                protected:
                        std::vector<SgNode*> dataDependenceList;
                        std::vector<SgNode*> mpiDependenceList;
                        std::vector<SgNode*> blockDependenceList;
                        std::vector<SgNode*> callSiteDependenceList;

                        // move this statement
                        SgExprStatement* src;

                        // the innermost scope the src node belongs to
                        SgScopeStatement* scopeDependence;

                        // the function definition containing src node
                        SgFunctionDefinition* fDef;

                        // to here
                        SgNode* dest;

                        bool insertAfterDest;

                        CallGraphQuery* cgQuery;


                public:
                        virtual void findDataDependence(SystemDependenceGraph* sdg); 
                        virtual void findMPICallDependence(std::list<SgExprStatement*>* mpiFunctionCalls); 
                        void findBlockDependence(); 
                        virtual void findCallSiteDependence(SystemDependenceGraph* sdg);

                        std::vector<SgNode*>* getDataDependenceList() { return &dataDependenceList; }
                        std::vector<SgNode*>* getMPIDependenceList() { return &mpiDependenceList; }
                        std::vector<SgNode*>* getBlockDependenceList() { return &blockDependenceList; }
                        std::vector<SgNode*>* getCallSiteDependenceList() { return &callSiteDependenceList; }

                        void setSrc(SgExprStatement* _src) {src = _src;}
                        SgExprStatement* getSrc() { return src; }

                        virtual void setScopeDependence();
                        void setScopeDependence(SgScopeStatement* expr) { scopeDependence = expr; }
                        SgNode* getScopeDependence() { return scopeDependence; }

                        void setFunctionDefinition();
                        void setFunctionDefinition(SgFunctionDefinition* _fDef) { fDef = _fDef; }
                        SgFunctionDefinition* getFunctionDefinition() { return fDef; }

                        virtual void setDest();
                        SgNode* getDest() { return dest;}

                        void setInsertAfterDest(bool _value) { insertAfterDest = _value; }
                        bool getInsertAfterDest() { return insertAfterDest; }

                        //string toString();

                protected:
                        SgNode* getChildStatement(SgNode* input);
                        SgNode* getParentStatement(SgNode* input);
                        SgNode* leastCommonAncestor(SgNode* a, SgNode* b);
        };


        /**************************************************************
        * Class Block Motion Information
        *
        * contains all the information, for a block, 
        * that up to where this block can be moved
        *
        **************************************************************/
        class BlockMotionInformation : public MotionInformation
        {
                public:
                        BlockMotionInformation(MotionInformation* info, CallGraphQuery* _cgQuery, SgNode* _sliceStart): MotionInformation(info->getSrc(), _cgQuery), sliceStart(_sliceStart), child(info)
                        {
                        }

                        virtual ~BlockMotionInformation()
                        { sliceStart = NULL; child = NULL; }

                        //void computeDependence(SystemDependenceGraph* sdg);

                        virtual void findDataDependence(SystemDependenceGraph* sdg); 
                        virtual void findMPICallDependence(std::list<SgExprStatement*>* mpiFunctionCalls); 
                        //virtual void findBlockDependence(); 

                        SgNode* getSliceStart() { return sliceStart; }
                        void setSliceStart(SgNode* _sliceStart) { sliceStart = _sliceStart; }

                        virtual void setScopeDependence();
                        virtual void setDest();

                protected:
                        SgNode* sliceStart;
                        MotionInformation* child;
        };

        /**************************************************************
        * Class Call Site Motion Information
        *
        *
        **************************************************************/
        /*
        class CallSiteMotionInformation : public MotionInformation
        {
                public:
                        CallSiteMotionInformation(SgNode* callSite): src(callSite);

                        virtual ~CallSiteMotionInformation()
                        {
                        }

                        virtual void findDataDependence(SystemDependenceGraph* sdg);
                        virtual void findMPICallDependence(std::list<SgExprStatement*>* mpiFunctionCalls);
                        virtual void findBlockDependence();

                        virtual void setDest();

                        void setFunctionDefinition(SgFunctionDefinition* _fDef) { fDef = _fDef; }

                protected:
                        SgFunctionDefinition* fDef;

        };
        */
}
#endif
