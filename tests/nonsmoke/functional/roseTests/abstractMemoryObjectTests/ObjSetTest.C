#include "rose.h"
#include <iostream>
#include <fstream>
#include <set>
#include "memory_object.h"
#include "memory_object_impl.h"
#include "ObjSetPtr.h"

using namespace AbstractMemoryObject;

// HELPER functions

Scalar* isObjSetScalar(ObjSet* _objset)
{
    ROSE_ASSERT(_objset != NULL);
    Scalar* scalar = dynamic_cast<Scalar*>(_objset);
    return scalar;
}

LabeledAggregate* isObjSetLabeledAggregate(ObjSet* _objset)
{
    ROSE_ASSERT(_objset != NULL);
    LabeledAggregate* laggregate = dynamic_cast<LabeledAggregate*>(_objset);
    return laggregate;
}

Array* isObjSetArray(ObjSet* _objset)
{
    ROSE_ASSERT(_objset != NULL);
    Array* array = dynamic_cast<Array*> (_objset);
    return array;
}

Pointer* isObjSetPointer(ObjSet* _objset)
{
   ROSE_ASSERT(_objset != NULL);
   Pointer* pointer = dynamic_cast<Pointer*> (_objset);
   return pointer;
}


class ObjSetCreate : public AstSimpleProcessing
{
public:
    std::set<ObjSet*> pointsToSet;
    std::set<ObjSetPtr> wPointsToSet;
    ObjSetCreate() { }
    virtual void visit(SgNode*);
    virtual void atTraversalEnd();
    bool existsObjSetPtr(ObjSetPtr& optr);
    bool existsObjSet(ObjSet* that);
};

bool ObjSetCreate::existsObjSetPtr(ObjSetPtr& _objsetptr)
{
    std::set<ObjSetPtr>::iterator it;
    for(it = wPointsToSet.begin(); it != wPointsToSet.end(); it++) {
        if(*it == _objsetptr)
            return true;
    }
    return false;
}

bool ObjSetCreate::existsObjSet(ObjSet* that)
{
    std::set<ObjSet*>::iterator it;
    for(it = pointsToSet.begin(); it != pointsToSet.end(); it++) {
        if(*(*it) == *that)
            return true;
    }
    return false;
}



void ObjSetCreate::visit(SgNode* sgn)
{
    ObjSet* objset_p;

    if( (isSgPntrArrRefExp(sgn) && !isSgPntrArrRefExp(sgn->get_parent()) )
       || isSgVarRefExp(sgn)
       || isSgExpression(sgn)
       || isSgType(sgn)
       || isSgSymbol(sgn)) {
        objset_p = ObjSetFactory::createObjSet(sgn);
        if(objset_p) {
            ObjSetPtr _oPtr(objset_p);
            if(!existsObjSet(objset_p)) {
                pointsToSet.insert(objset_p);
            }

            // if(!existsObjSetPtr(_oPtr)) {
            //     wPointsToSet.insert(_oPtr);
            // }
        }             
    }
}

void ObjSetCreate::atTraversalEnd()
{
    std::cout << "ObjSetCreate : -------\n";
    std::cout << "----------------------\n";
    std::set<ObjSet*>::iterator it;
    std::set<ObjSetPtr>::iterator wit;
    for(it = pointsToSet.begin(); it != pointsToSet.end(); it++) {
        std::cout << (*it)->toString() << std::endl;
    }
    std::cout << "----------------------\n";
    // for(wit = wPointsToSet.begin(); wit != wPointsToSet.end(); wit++) {
    //     std::cout << (*wit)->toString() << std::endl;
    // }
    std::cout << "----------------------\n";
}

 class ObjSetTestEquiv : public AstSimpleProcessing
 {
 public:
     ObjSetCreate* objc;
     std::set<ObjSet*> pointsToSet;
     //std::set<ObjSetPtr> wPointsToSet;
     ObjSetTestEquiv(ObjSetCreate* _that) : objc(_that) { }    
     virtual void visit(SgNode*);
     virtual void atTraversalEnd();
 };

void ObjSetTestEquiv::visit(SgNode* sgn)
{
    ObjSet* objset_p;

    if( (isSgPntrArrRefExp(sgn) && !isSgPntrArrRefExp(sgn->get_parent()) )
       || isSgVarRefExp(sgn)
       || isSgExpression(sgn)
       || isSgType(sgn)
       || isSgSymbol(sgn)) {
        objset_p = ObjSetFactory::createObjSet(sgn);
        if(objset_p) {
            ObjSetPtr _oPtr(objset_p);
            if(! (objc->existsObjSet(objset_p))) {
                pointsToSet.insert(objset_p);
            }
            // if(!objc->existsObjSetPtr(_oPtr)) {
            //     wPointsToSet.insert(_oPtr);
            // }
        }             
    }
}

void ObjSetTestEquiv::atTraversalEnd()
{
    std::cout << "----------------------\n";
    std::cout << "ObjSetTestEquiv : ----\n";
    std::set<ObjSet*>::iterator it;
    std::set<ObjSetPtr>::iterator wit;
    for(it = pointsToSet.begin(); it != pointsToSet.end(); it++) {
        std::cout << (*it)->toString() << std::endl;
    }
    std::cout << "----------------------\n";
    // for(wit = wPointsToSet.begin(); wit != wPointsToSet.end(); wit++) {
    //     std::cout << (*wit)->toString() << std::endl;
    // }
    std::cout << "----------------------\n";
}


int main(int argc, char* argv[])
{
    SgProject *project = frontend(argc, argv);
    ObjSetCreate objcreate;
    objcreate.traverseInputFiles(project, preorder);
    ObjSetTestEquiv test(&objcreate);
    test.traverseInputFiles(project, preorder);
    return 0;
}
