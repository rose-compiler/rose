/*
 * layoutTransformation.C
 *
 *  Created on: Jul 11, 2011
 *      Author: sharma9
 */

#include <iostream>

#include "rose.h"
#include "transformationWorklist.h"
#include "SpecificationTraversal/specificationTraversal.C"
#include "interleaveAcrossArraysCheck.h"
#include "interleaveAcrossArraysTransformation.h"
#include "arrayPreprocessor.h"

using namespace std;

int main(int argc, char* argv[]) {

	SgProject* project = frontend(argc, argv);
	ROSE_ASSERT (project != NULL);

	/*
	 * We could have used Commandline Processing as an efficient way.
	 * However, this would cause the user to specify explicit include
	 * the specification file. To avoid the edg parse error,
	 * we grab the file directly.
	 */
	SgFile* specFile = isSgSourceFile((*project)[0]);

#if DEBUG
	specFile->get_file_info()->display(" Specification File");
#endif

	// Build the traversal object
	specificationTraversal specTraversal;
	// Call the traversal function (member function of AstSimpleProcessing)
	// starting at the project node of the AST, using a preorder traversal.
	specTraversal.traverse(specFile, preorder);

	// Remove the specification file from the list
	SgFilePtrList & file_list = project->get_fileList();
	file_list.erase(file_list.begin());

	ArrayPreprocessor(project);



	TransformationWorklist worklist = specTraversal.getTransformationWorkList();

	for(int transIndex = 0; transIndex < worklist.size() ; transIndex++)
		{
			Transformation t = (worklist.getTransformation(transIndex));

			// For each source file in the project
			SgFilePtrList & ptr_list = project->get_fileList();
			for (SgFilePtrList::iterator iter = ptr_list.begin(); iter != ptr_list.end(); iter++) {

				SgFile* sageFile = (*iter);
				SgSourceFile * sfile = isSgSourceFile(sageFile);
				ROSE_ASSERT(sfile);
				SgGlobal *root = sfile->get_globalScope();
				SgDeclarationStatementPtrList& declList = root->get_declarations();

				//For each function body in the scope
				for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end();
						++p) {
					SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
					if (func == 0)
						continue;
					SgFunctionDefinition *defn = func->get_definition();
					if (defn == 0)
						continue;
					//ignore functions in system headers, Can keep them to test robustness
					if (defn->get_file_info()->get_filename() != sageFile->get_file_info()->get_filename())
						continue;
					SgBasicBlock *body = defn->get_body();
					ROSE_ASSERT(body);

					LayoutOptions::LayoutType layoutType = t.getOption();
					switch(layoutType)
					{

						case LayoutOptions::InterleaveAcrossArrays:
						{
							interleaveAcrossArraysCheck checkTraversal(&t);
							checkTraversal.traverse(body);

							cout << " Calling Array Traversal " << endl;
							interleaveAcrossArraysTraversal transformationTraversal(&t);
							transformationTraversal.traverse(body);
							break;
						}

						default:
							cout << " Transformation Type :" << t.getOption() << " not handled. " << endl;
							break;
						}
				}
			}
		}

	return backend(project);
}
