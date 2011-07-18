/*
 * layoutTransformation.C
 *
 *  Created on: Jul 11, 2011
 *      Author: sharma9
 */

#include "rose.h"
#include "transformationWorklist.h"
#include "SpecificationTraversal/specificationTraversal.C"
#include "interleaveAcrossArraysCheck.h"
#include "interleaveAcrossArraysTransformation.h"

int
main ( int argc, char* argv[] )
   {

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

     SgFile & specFile = project->get_file(0);

#if DEBUG
     specFile.get_file_info()->display(" Spec File");
#endif

  // Build the traversal object
     specificationTraversal specTraversal;

  // Call the traversal function (member function of AstSimpleProcessing)
  // starting at the project node of the AST, using a preorder traversal.
     specTraversal.traverse(&specFile,preorder);
     TransformationWorklist worklist = specTraversal.getTransformationWorkList();
     Transformation t = (worklist.getTransformation(0));
//
//     SgFile & transFile = project->get_file(1);
//     set<string> fileNames;
//     fileNames.insert(transFile.get_file_info()->get_filenameString());
//     interleaveAcrossArraysCheck checkTraversal(fileNames, &t);
//     checkTraversal.traverse(&transFile,preorder);
//
//     interleaveAcrossArraysTraversal transformationTraversal(fileNames, &t);
//     transformationTraversal.traverse(&transFile);

     // For each source file in the project
     	SgFilePtrList & ptr_list = project->get_fileList();
     	for (SgFilePtrList::iterator iter = ptr_list.begin(); iter
     			!= ptr_list.end(); iter++) {

     		if(iter == ptr_list.begin())
     			continue;

     		SgFile* sageFile = (*iter);
     		SgSourceFile * sfile = isSgSourceFile(sageFile);
     		ROSE_ASSERT(sfile);
     		SgGlobal *root = sfile->get_globalScope();
     		SgDeclarationStatementPtrList& declList = root->get_declarations();

     		//For each function body in the scope
     		for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p
     				!= declList.end(); ++p) {
     			SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
     			if (func == 0)
     				continue;
     			SgFunctionDefinition *defn = func->get_definition();
     			if (defn == 0)
     				continue;
     			//ignore functions in system headers, Can keep them to test robustness
     			if (defn->get_file_info()->get_filename()
     					!= sageFile->get_file_info()->get_filename())
     				continue;
     			SgBasicBlock *body = defn->get_body();
     			ROSE_ASSERT(body);

     			interleaveAcrossArraysCheck checkTraversal(&t);
				checkTraversal.traverse(body);

				interleaveAcrossArraysTraversal transformationTraversal(&t);
				transformationTraversal.traverse(body);
     		}
     	}

     return backend(project);
   }
