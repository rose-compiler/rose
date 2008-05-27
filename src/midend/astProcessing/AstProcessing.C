// Original Author (AstProcessing classes): Markus Schordan
// Rewritten by: Gergo Barany
// $Id: AstProcessing.C,v 1.10 2008/01/25 02:25:48 dquinlan Exp $

// For information about the changes introduced during the rewrite, see
// the comment in AstProcessing.h

#ifndef ASTPROCESSING_C
#define ASTPROCESSING_C

#include "sage3.h"

#include "AstProcessing.h"

template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
setNodeSuccessors(SgNode* node, SuccessorsContainer& succContainer)
{
    AstSuccessorsSelectors::selectDefaultSuccessors(node, succContainer);
}


// The default constructor of the internal tree traversal class
template<class InheritedAttributeType, class SynthesizedAttributeType>
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
SgTreeTraversal() 
  : useDefaultIndexBasedTraversal(true),
    traversalConstraint(false),
    fileToVisit(NULL),
    synthesizedAttributes(new SynthesizedAttributesList())
{
}

// The destructor of the internal tree traversal class
template<class InheritedAttributeType, class SynthesizedAttributeType>
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
~SgTreeTraversal(void)
{
    ROSE_ASSERT(synthesizedAttributes != NULL);
    delete synthesizedAttributes;
    synthesizedAttributes = NULL;
}

template<class InheritedAttributeType, class SynthesizedAttributeType>
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
SgTreeTraversal(const SgTreeTraversal &other)
  : useDefaultIndexBasedTraversal(other.useDefaultIndexBasedTraversal),
    traversalConstraint(other.traversalConstraint),
    fileToVisit(other.fileToVisit),
    synthesizedAttributes(other.synthesizedAttributes->deepCopy())
{
}

template<class InheritedAttributeType, class SynthesizedAttributeType>
const SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType> &
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
operator=(const SgTreeTraversal &other)
{
    useDefaultIndexBasedTraversal = other.useDefaultIndexBasedTraversal;
    traversalConstraint = other.traversalConstraint;
    fileToVisit = other.fileToVisit;

    ROSE_ASSERT(synthesizedAttributes != NULL);
    delete synthesizedAttributes;
    synthesizedAttributes = other.synthesizedAttributes->deepCopy();

    return *this;
}

template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
set_useDefaultIndexBasedTraversal(bool val)
{
    useDefaultIndexBasedTraversal = val;
}

// GB (05/30/2007): This was completely rewritten from the old version to
// use much more efficient comparisons now available to us; it also never
// causes visits to included files, something that happened from time to
// time with the old version.
template<class InheritedAttributeType, class SynthesizedAttributeType>
bool 
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::inFileToTraverse(SgNode* node)
   {
  // If traversing without constraint, just continue.
  // if (!traversalConstraint)
     if (traversalConstraint == false)
          return true;

  // DQ (1/21/2008): Recently added SgAsmNodes for binaries also do not 
  // have a SgFileInfo object.
  // Of all the nodes to be traversed, only SgProject is allowed to have
  // a NULL file info; go ahead and try to traverse it (even though this
  // does not make sense since it is not within any file).
     if (node->get_file_info() == NULL)
        {
       // DQ (1/20/2008): This fails for binary files, why is this!
       // if (isSgProject(node) == NULL)
       //      printf ("What node is this: node = %p = %s \n",node,node->class_name().c_str()); // SageInterface::get_name(node).c_str());
       // ROSE_ASSERT(isSgProject(node) != NULL);

          ROSE_ASSERT(isSgProject(node) != NULL || isSgAsmNode(node) != NULL);
          return true;
        }

  // Traverse compiler generated code and code generated from
  // transformations, unless it is "frontend specific" like the stuff in
  // rose_edg_required_macros_and_functions.h.
     bool isFrontendSpecific = node->get_file_info()->isFrontendSpecific();
     bool isCompilerGeneratedOrPartOfTransformation;
     if (isFrontendSpecific)
        {
          isCompilerGeneratedOrPartOfTransformation = false;
        }
       else
        {
          isCompilerGeneratedOrPartOfTransformation = node->get_file_info()->isCompilerGenerated() || node->get_file_info()->isTransformation();
        }

  // Traverse this node if it is in the file we want to visit.
     bool isRightFile = node->get_file_info()->isSameFile(fileToVisit);

  // This function is meant to traverse input files in the sense of not
  // visiting "header" files (a fuzzy concept). But not every #included file
  // is a header, "code" (another fuzzy concept) can also be #included; see
  // test2005_157.C for an example. We want to traverse such code, so we
  // cannot rely on just comparing files.
  // The following heuristic is therefore used: If a node is included from
  // global scope or from within a namespace definition, we guess that it is
  // a header and don't traverse it. Otherwise, we guess that it is "code"
  // and do traverse it.
     bool isCode = node->get_parent() != NULL
                   && !isSgGlobal(node->get_parent())
                   && !isSgNamespaceDefinitionStatement(node->get_parent());

     bool traverseNode;
     if (isCompilerGeneratedOrPartOfTransformation || isRightFile || isCode)
          traverseNode = true;
       else
          traverseNode = false;

     return traverseNode;
   }

// MS: 03/22/02
// function to traverse all ASTs representing inputfiles (excluding include files), 
template<class InheritedAttributeType, class SynthesizedAttributeType>
void SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
traverseInputFiles(SgProject* projectNode,
        InheritedAttributeType inheritedValue,
        t_traverseOrder travOrder)
{
    SgFilePtrListPtr fList = projectNode->get_fileList();
    for (SgFilePtrList::iterator fl_iter = fList->begin();
        fl_iter != fList->end(); fl_iter++)
    {
        ROSE_ASSERT(*fl_iter!=0);
        traverseWithinFile((*fl_iter), inheritedValue, travOrder);
    }
}

//////////////////////////////////////////////////////
//// TOP DOWN BOTTOM UP PROCESSING IMPLEMENTATION ////
//////////////////////////////////////////////////////


// MS: 04/25/02
template <class InheritedAttributeType, class SynthesizedAttributeType>
SynthesizedAttributeType 
AstTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType>::
traverse(SgNode* node, InheritedAttributeType inheritedValue)
{
    // this is now explicitly marked as a pre *and* post order traversal
    return SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>
        ::traverse(node, inheritedValue, preandpostorder);
}

// MS: 04/25/02
template <class InheritedAttributeType, class SynthesizedAttributeType>
SynthesizedAttributeType 
AstTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType>::
traverseWithinFile(SgNode* node, InheritedAttributeType inheritedValue)
{
    // this is now explicitly marked as a pre *and* post order traversal
    return SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::traverseWithinFile(node, inheritedValue, preandpostorder);
}

////////////////////////////////////////////
//// TOP DOWN PROCESSING IMPLEMENTATION ////
////////////////////////////////////////////

// MS: 04/25/02
template <class InheritedAttributeType>
DummyAttribute
AstTopDownProcessing<InheritedAttributeType>::
evaluateSynthesizedAttribute(SgNode* astNode,
        InheritedAttributeType inheritedValue,
        typename AstTopDownProcessing<InheritedAttributeType>::SynthesizedAttributesList l)
{
    // call the cleanup function
    destroyInheritedValue(astNode, inheritedValue);
    // return value is not used
    DummyAttribute a = defaultDummyAttribute;
    return a;
}

// MS: 07/30/04
template <class InheritedAttributeType>
DummyAttribute
AstTopDownProcessing<InheritedAttributeType>::
defaultSynthesizedAttribute(InheritedAttributeType inh)
{
    // called but not used
    DummyAttribute a = defaultDummyAttribute;
    return a;
}

// MS: 04/25/02
template <class InheritedAttributeType>
void
AstTopDownProcessing<InheritedAttributeType>::
traverse(SgNode* node, InheritedAttributeType inheritedValue)
{
    // "top down" is now marked as a pre *and* post order traversal because
    // there is a post order component (the call to destroyInheritedAttribute)
    SgTreeTraversal<InheritedAttributeType, DummyAttribute>
        ::traverse(node, inheritedValue, preandpostorder);
}

// MS: 09/30/02
template <class InheritedAttributeType>
void
AstTopDownProcessing<InheritedAttributeType>::
traverseWithinFile(SgNode* node, InheritedAttributeType inheritedValue)
{
    // "top down" is now marked as a pre *and* post order traversal because
    // there is a post order component (the call to destroyInheritedAttribute)
    SgTreeTraversal<InheritedAttributeType, DummyAttribute>::traverseWithinFile(node, inheritedValue, preandpostorder);
}

/////////////////////////////////////////////
//// BOTTOM UP PROCESSING IMPLEMENTATION ////
/////////////////////////////////////////////

// MS: 04/25/02
template <class SynthesizedAttributeType>
DummyAttribute
AstBottomUpProcessing<SynthesizedAttributeType>::
evaluateInheritedAttribute(SgNode* astNode, DummyAttribute inheritedValue)
{
    /* called but not used */
    DummyAttribute a = defaultDummyAttribute;
    return a;
}

// MS: 30/07/04
template <class SynthesizedAttributeType>
SynthesizedAttributeType AstBottomUpProcessing<SynthesizedAttributeType>::
defaultSynthesizedAttribute()
{
    // GB (8/6/2007): This can give "may not be initialized" warnings when
    // compiling with optimization (because -O flags cause gcc to perform
    // data-flow analysis). I wonder how this might be fixed.
    SynthesizedAttributeType s = SynthesizedAttributeType();
    return s;
}

// MS: 30/07/04
template <class SynthesizedAttributeType>
SynthesizedAttributeType AstBottomUpProcessing<SynthesizedAttributeType>::
defaultSynthesizedAttribute(DummyAttribute inheritedValue)
{
    return defaultSynthesizedAttribute();
}

// MS: 04/25/02
template <class SynthesizedAttributeType>
SynthesizedAttributeType AstBottomUpProcessing<SynthesizedAttributeType>::
evaluateSynthesizedAttribute(SgNode* astNode,
        DummyAttribute inheritedValue,
        SynthesizedAttributesList l)
{
    return evaluateSynthesizedAttribute(astNode, l);
}

// MS: 04/25/02
template <class SynthesizedAttributeType>
SynthesizedAttributeType AstBottomUpProcessing<SynthesizedAttributeType>::
traverse(SgNode* node)
{
    static DummyAttribute da;
    return SgTreeTraversal<DummyAttribute, SynthesizedAttributeType>
        ::traverse(node, da, postorder);
}

// MS: 04/25/02
template <class SynthesizedAttributeType>
SynthesizedAttributeType AstBottomUpProcessing<SynthesizedAttributeType>::
traverseWithinFile(SgNode* node)
{
    static DummyAttribute da;
    return SgTreeTraversal<DummyAttribute, SynthesizedAttributeType>::traverseWithinFile(node, da, postorder);
}

// MS: 04/25/02
template <class SynthesizedAttributeType>
void AstBottomUpProcessing<SynthesizedAttributeType>::
traverseInputFiles(SgProject* projectNode)
{
    static DummyAttribute da;
    // GB (8/6/2007): This is now a postorder traversal; this did not really
    // matter until now, but now evaluateSynthesizedAttribute is only called
    // for traversals that have the postorder bit set.
    SgTreeTraversal<DummyAttribute, SynthesizedAttributeType>
        ::traverseInputFiles(projectNode, da, postorder);
}

// MS: 07/29/04
template <class InheritedAttributeType, class SynthesizedAttributeType>
SynthesizedAttributeType SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
defaultSynthesizedAttribute(InheritedAttributeType inh)
{
    // we provide 'inh' but do not use it in the constructor of 's' to allow primitive types
    SynthesizedAttributeType s = SynthesizedAttributeType(); 
    return s;
}

// MS: 09/30/02
template <class InheritedAttributeType, class SynthesizedAttributeType>
SynthesizedAttributeType
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
traverseWithinFile(SgNode* node,
        InheritedAttributeType inheritedValue,
        t_traverseOrder treeTraversalOrder)
{
  // DQ (1/18/2006): debugging
     ROSE_ASSERT(this != NULL);
     traversalConstraint = true;

     SgFile* filenode = isSgFile(node);
     ROSE_ASSERT(filenode != NULL); // this function will be extended to work with all nodes soon

     // GB (05/30/2007): changed to a SgFile* instead of a file name,
     // comparisons are much cheaper this way
     fileToVisit = filenode;

     ROSE_ASSERT(inFileToTraverse(node) == true);

     SynthesizedAttributeType synth = traverse(node, inheritedValue, treeTraversalOrder);
     traversalConstraint = false;
     return synth;
}

// GB (06/31/2007): Wrapper function around the performTraversal()
// function that does the real work; when that function is done, we call
// traversalResult() to get the final result off the stack of synthesized
// attributes.
template <class InheritedAttributeType, class SynthesizedAttributeType>
SynthesizedAttributeType SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
traverse(SgNode *node, InheritedAttributeType inheritedValue,
        t_traverseOrder treeTraversalOrder)
{
    // make sure the stack is empty
    synthesizedAttributes->resetStack();
    ROSE_ASSERT(synthesizedAttributes->debugSize() == 0);

    // notify the concrete traversal class that a traversal is starting
    atTraversalStart();

    // perform the actual traversal
    performTraversal(node, inheritedValue, treeTraversalOrder);

    // notify the traversal that we are done
    atTraversalEnd();

    // get the result off the stack
    return traversalResult();
}

// MS: 03/22/02
// GB (05/30/2007): This used to be called traverse and return a synthesized
// attribute. Because the memory management was changed and the synthesized
// attributes are now kept on a stack, we return void, and traverse() then
// calls a special function to get the final result off the stack.
// All the visited flag stuff was taken out.
template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
performTraversal(SgNode* node,
        InheritedAttributeType inheritedValue,
        t_traverseOrder treeTraversalOrder)
{
    // 1. node can be a null pointer, only traverse it if !=0.
    //    (since the SuccessorContainer is order preserving we require 0 values as well!)
    // 2. inFileToTraverse is false if we are trying to go to a different file (than the input file)
    //    and only if traverseInputFiles was invoked, otherwise it's always true

    if (node && inFileToTraverse(node))
    {
        // In case of a preorder traversal call the function to be applied to each node of the AST
        // GB (7/6/2007): Because AstPrePostProcessing was introduced, a
        // treeTraversalOrder can now be pre *and* post at the same time! The
        // == comparison was therefore replaced by a bit mask check.
        if (treeTraversalOrder & preorder)
            inheritedValue = evaluateInheritedAttribute(node, inheritedValue);
  
        // Visit the traversable data members of this AST node.
        // GB (09/25/2007): Added support for index-based traversals. The useDefaultIndexBasedTraversal flag tells us
        // whether to use successor containers or direct index-based access to the node's successors.
        AstSuccessorsSelectors::SuccessorsContainer succContainer;
        size_t numberOfSuccessors;
        if (!useDefaultIndexBasedTraversal)
        {
            setNodeSuccessors(node, succContainer);
            numberOfSuccessors = succContainer.size();
        }
        else
        {
            numberOfSuccessors = node->get_numberOfTraversalSuccessors();
        }

        for (size_t idx = 0; idx < numberOfSuccessors; idx++)
        {
            SgNode *child = NULL;

            if (useDefaultIndexBasedTraversal)
               {
              // ROSE_ASSERT(node->get_traversalSuccessorByIndex(idx) != NULL || node->get_traversalSuccessorByIndex(idx) == NULL);
                 child = node->get_traversalSuccessorByIndex(idx);
               }
              else
               {
              // ROSE_ASSERT(succContainer[idx] != NULL || succContainer[idx] == NULL);
                 child = succContainer[idx];
               }

            if (child != NULL)
            {
                // performTraversal computes the synthesized attribute for the child and pushes it onto the stack
                performTraversal(child, inheritedValue, treeTraversalOrder);
            }
            else
            {
                // null pointer (not traversed): we put the default value(s) of SynthesizedAttribute onto the stack
                if (treeTraversalOrder & postorder)
                    synthesizedAttributes->push(defaultSynthesizedAttribute(inheritedValue));
            }
        }
 
        // In case of a postorder traversal call the function to be applied to each node of the AST
        // GB (7/6/2007): Because AstPrePostProcessing was introduced, a
        // treeTraversalOrder can now be pre *and* post at the same time! The
        // == comparison was therefore replaced by a bit mask check.
        // The call to evaluateInheritedAttribute at this point also had to be
        // changed; it was never elegant anyway as we were not really
        // evaluating attributes here.
        if (treeTraversalOrder & postorder)
        {
            // Now that every child's synthesized attributes are on the stack:
            // Tell the stack how big the stack frame containing those
            // attributes is to be, and pass that frame to
            // evaluateSynthesizedAttribute(); then replace those results by
            // pushing the computed value onto the stack (which pops off the
            // previous stack frame).
            synthesizedAttributes->setFrameSize(numberOfSuccessors);
            ROSE_ASSERT(synthesizedAttributes->size() == numberOfSuccessors);
            synthesizedAttributes->push(evaluateSynthesizedAttribute(node, inheritedValue, *synthesizedAttributes));
        }
    }
    else // if (node && inFileToTraverse(node))
    {
        if (treeTraversalOrder & postorder)
            synthesizedAttributes->push(defaultSynthesizedAttribute(inheritedValue));
    }
} // function body

// GB (05/30/2007)
template <class InheritedAttributeType, class SynthesizedAttributeType>
SynthesizedAttributeType SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
traversalResult()
{
    // If the stack of synthesizedAttributes contains exactly one object, then
    // that one is the valid final result of the computation, so we should
    // return it. Otherwise, either there are no objects on the stack (because
    // the traversal didn't use any attributes), or there are more than one
    // (usually because the traversal exited prematurely by throwing an
    // exception); in this case, just return a default attribute.
    if (synthesizedAttributes->debugSize() == 1)
    {
        return synthesizedAttributes->pop();
    }
    else
    {
        static SynthesizedAttributeType sa;
        return sa;
    }
}

// GB (05/30/2007)
template <class InheritedAttributeType, class SynthesizedAttributeType>
void
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
atTraversalStart()
{
}

template <class InheritedAttributeType, class SynthesizedAttributeType>
void
SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::
atTraversalEnd()
{
}

template <class InheritedAttributeType, class SynthesizedAttributeType>
void
AstTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType>::
atTraversalStart()
{
}

template <class InheritedAttributeType, class SynthesizedAttributeType>
void
AstTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType>::
atTraversalEnd()
{
}

template <class InheritedAttributeType>
void
AstTopDownProcessing<InheritedAttributeType>::
destroyInheritedValue(SgNode*, InheritedAttributeType)
{
}

template <class InheritedAttributeType>
void
AstTopDownProcessing<InheritedAttributeType>::
atTraversalStart()
{
}

template <class InheritedAttributeType>
void
AstTopDownProcessing<InheritedAttributeType>::
atTraversalEnd()
{
}

template <class SynthesizedAttributeType>
void
AstBottomUpProcessing<SynthesizedAttributeType>::
atTraversalStart()
{
}

template <class SynthesizedAttributeType>
void
AstBottomUpProcessing<SynthesizedAttributeType>::
atTraversalEnd()
{
}

#endif
