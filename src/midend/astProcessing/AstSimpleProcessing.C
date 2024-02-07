

// Original Author (AstProcessing classes): Markus Schordan
// Rewritten by: Gergo Barany
// $Id: AstSimpleProcessing.C,v 1.4 2008/01/08 02:56:39 dquinlan Exp $

// See comments in AstProcessing.h for list of changes during the rewrite

#ifndef ASTSIMPLEPROCESSING_C
#define ASTSIMPLEPROCESSING_C

#include "sage3basic.h"

#include "AstProcessing.h" 
#include "AstSimpleProcessing.h"

//////////////////////////////////////////
//// SIMPLE PROCESSING IMPLEMENTATION ////
//////////////////////////////////////////

// this has to be put in a different file than the other 3 templated versions to compile&link with gcc always

// MS: 04/25/02
DummyAttribute
AstSimpleProcessing::evaluateInheritedAttribute(SgNode* astNode,
        DummyAttribute /*inheritedValue*/)
{
    visit(astNode);
    // dummy attribute, return default
    DummyAttribute a = defaultDummyAttribute;
    return a;
}

// GB (8/3/2007): There are now two methods in AstSimpleProcessing that call
// the visit() method. However, only one of these will be invoked for each
// node, depending on the traversal order flag.
DummyAttribute 
AstSimpleProcessing::evaluateSynthesizedAttribute(SgNode* astNode, DummyAttribute, SynthesizedAttributesList)
{
    visit(astNode);
    // dummy attribute, return default
    DummyAttribute a = defaultDummyAttribute;
    return a;
}

DummyAttribute
AstPrePostProcessing::evaluateInheritedAttribute(SgNode* astNode,
        DummyAttribute /*inheritedValue*/)
{
    preOrderVisit(astNode);
    // dummy attribute, return default
    DummyAttribute a = defaultDummyAttribute;
    return a;
}

DummyAttribute 
AstPrePostProcessing::evaluateSynthesizedAttribute(SgNode* astNode,
        DummyAttribute /*inheritedValue*/,
        SynthesizedAttributesList)
{
    postOrderVisit(astNode);
    DummyAttribute a = defaultDummyAttribute;
    return a;
}

// MS: 07/30/04
DummyAttribute 
AstSimpleProcessing::defaultSynthesizedAttribute(DummyAttribute /*inheritedValue*/)
{
    DummyAttribute a = defaultDummyAttribute;
    return a;
}

DummyAttribute 
AstPrePostProcessing::defaultSynthesizedAttribute(DummyAttribute /*inheritedValue*/)
{
    DummyAttribute a = defaultDummyAttribute;
    return a;
}

// GB: 7/6/2007
void 
AstPrePostProcessing::traverse(SgNode* node)
{
    static DummyAttribute da;
    SgTreeTraversal<DummyAttribute, DummyAttribute>::traverse(node, da, preandpostorder);
}

// MS: 04/25/02
void 
AstSimpleProcessing::traverse(SgNode* node, t_traverseOrder treeTraversalOrder)
{
    static DummyAttribute da;
    SgTreeTraversal<DummyAttribute, DummyAttribute>::traverse(node, da, treeTraversalOrder);
}

// GB: 7/6/2007
void 
AstPrePostProcessing::traverseWithinFile(SgNode* node)
{
    static DummyAttribute da;
    SgTreeTraversal<DummyAttribute, DummyAttribute>::traverseWithinFile(node, da, preandpostorder);
}

// MS: 09/30/02
void 
AstSimpleProcessing::traverseWithinFile(SgNode* node, t_traverseOrder treeTraversalOrder)
{
    static DummyAttribute da;
    SgTreeTraversal<DummyAttribute, DummyAttribute>::traverseWithinFile(node, da, treeTraversalOrder);
}

// MS: 04/25/02
void 
AstPrePostProcessing::traverseInputFiles(SgProject* projectNode)
{
    static DummyAttribute da;
    SgTreeTraversal<DummyAttribute, DummyAttribute>::
        traverseInputFiles(projectNode, da, preandpostorder);
}

// MS: 04/25/02
void 
AstSimpleProcessing::traverseInputFiles(SgProject* projectNode, t_traverseOrder treeTraversalOrder)
{
    static DummyAttribute da;
    SgTreeTraversal<DummyAttribute, DummyAttribute>::
        traverseInputFiles(projectNode, da, treeTraversalOrder);
}

// GB: 06/05/2007
void
AstPrePostProcessing::atTraversalStart()
{
}

void
AstPrePostProcessing::atTraversalEnd()
{
}

void
AstSimpleProcessing::atTraversalStart()
{
}

void
AstSimpleProcessing::atTraversalEnd()
{
}

#endif
