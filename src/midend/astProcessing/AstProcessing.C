// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// Original Author (AstProcessing classes): Markus Schordan
// Rewritten by: Gergo Barany
// $Id: AstProcessing.C,v 1.10 2008/01/25 02:25:48 dquinlan Exp $

// For information about the changes introduced during the rewrite, see
// the comment in AstProcessing.h

// GB (05/30/2007): This was completely rewritten from the old version to
// use much more efficient comparisons now available to us; it also never
// causes visits to included files, something that happened from time to
// time with the old version.
bool 
SgTreeTraversal_inFileToTraverse(SgNode* node, bool traversalConstraint, SgFile* fileToVisit)
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

          if (isSgProject(node) == NULL && isSgAsmNode(node) == NULL)
          {
               printf ("Error: SgTreeTraversal_inFileToTraverse() --- node->get_file_info() == NULL: node = %p = %s \n",node,node->class_name().c_str());
               SageInterface::dumpInfo(node);
          }

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
       // DQ (11/14/2008): Implicitly defined functions in Fortran are not marked as compiler generated 
       // (the function body is at least explicit in the source file), but the function declaration IR 
       // nodes is marked as coming from file == NULL_FILE and it is also marked as "outputInCodeGeneration"
       // So it should be traversed so that we can see the function body and so that it can be a proper 
       // part of the definition of the AST.
       // isCompilerGeneratedOrPartOfTransformation = node->get_file_info()->isCompilerGenerated() || node->get_file_info()->isTransformation();
          bool isOutputInCodeGeneration = node->get_file_info()->isOutputInCodeGeneration();
          isCompilerGeneratedOrPartOfTransformation = node->get_file_info()->isCompilerGenerated() || node->get_file_info()->isTransformation() || isOutputInCodeGeneration;
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
