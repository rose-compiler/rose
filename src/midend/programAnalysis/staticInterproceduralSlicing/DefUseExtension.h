

namespace DUVariableAnalysisExt
{
        SgNode * getNextParentInterstingNode(SgNode* node);
        bool isDef(SgNode * node);
        bool isDef(SgNode * node,bool treadFunctionCallAsDef);
        bool isIDef(SgNode * node);
        bool isIUse(SgNode* node);
        bool test(SgNode* node);
        
        bool isUse(SgNode * node);
        bool isAssignmentExpr(SgNode*node);
        bool isFunctionParameter(SgNode*node);
        bool isPointerType(SgVarRefExp * ref);
        bool isComposedType(SgVarRefExp * ref);                                                                                                                                                                                                         
        bool isMemberVar(SgVarRefExp * ref);
        bool functionUsesAddressOf(SgVarRefExp * node,SgFunctionCallExp * call);
}
