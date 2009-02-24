// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



#include "rewrite.h"
#include <vector>

#define DEBUG  0

/*********************************************************************************
 *                          The ReWrite Traversal                                *
 *********************************************************************************/

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class MyInheritedAttribute
{
	public:
		MyInheritedAttribute () {};
};

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class MySynthesizedAttribute
: public HighLevelRewrite::SynthesizedAttribute
{
	public:
		MySynthesizedAttribute() {
			
	        generationObjectCreatedInScope = true;
		
		};
         	MySynthesizedAttribute(const MySynthesizedAttribute& X) {
			((MySynthesizedAttribute*) this)->operator= (X);
		}

		MySynthesizedAttribute & operator= (const MySynthesizedAttribute& X) {
			generationObjectCreatedInScope = X.generationObjectCreatedInScope;
	        	HighLevelRewrite::SynthesizedAttribute::operator=(X);
			return *this;
		}
		MySynthesizedAttribute & operator+= (const MySynthesizedAttribute& X) {
			operator=(X);
			return *this;
		}


		bool generationObjNeeded() { return generationObjectCreatedInScope; }
		void setGenerationObjCreated(bool set){ generationObjectCreatedInScope = set; }

	private:
		bool generationObjectCreatedInScope;
	};

// tree traversal to test the rewrite mechanism
//! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
class MyTraversal
: public HighLevelRewrite::RewriteTreeTraversal<MyInheritedAttribute,MySynthesizedAttribute>
{
	public:
		MyTraversal () {
                		
		};

		// Functions required by the tree traversal mechanism
		MyInheritedAttribute evaluateRewriteInheritedAttribute (
				SgNode* astNode,
				MyInheritedAttribute inheritedAttribute );

		MySynthesizedAttribute evaluateRewriteSynthesizedAttribute (
				SgNode* astNode,
				MyInheritedAttribute inheritedAttribute,
				SubTreeSynthesizedAttributes synthesizedAttributeList );

       private:
		string         filename;
};


// Functions required by the tree traversal mechanism
MyInheritedAttribute
MyTraversal::evaluateRewriteInheritedAttribute (
		SgNode* astNode,
		MyInheritedAttribute inheritedAttribute )
{
	// Note that any constructor will do
	MyInheritedAttribute returnAttribute;

	return returnAttribute;
}

MySynthesizedAttribute
MyTraversal::evaluateRewriteSynthesizedAttribute (
		SgNode* astNode,
		MyInheritedAttribute inheritedAttribute,
		SubTreeSynthesizedAttributes synthesizedAttributeList )
{
	// Note that any constructor will do
	MySynthesizedAttribute returnAttribute;
	//cerr << "Inherited attribute eval" << endl;
	//Sg_File_Info *startinf  = astNode->get_startOfConstruct(); 
	//if(startinf) cout << " start info, file: " << startinf->get_filename() << ", line:" << startinf->get_line() << endl;
	
	switch(astNode->variantT())
	{
	        case V_SgFile:
			{
		            //set filename information
                            SgFile* sageFile = isSgFile(astNode);
			    ROSE_ASSERT(sageFile != NULL );
			    filename = sageFile->getFileName();
			    break;
			}
	        case V_SgGlobal:
			{
                        string includeString("#ifndef EMPTYINCLUDE \n #define EMPTYINCLUDE \n #include \"/home/saebjorn/temp/printDatastructureNEW/emptyInclude.h\" \n #endif\n");
                        returnAttribute.insert( astNode, includeString, HighLevelCollectionTypedefs::GlobalScope, HighLevelCollectionTypedefs::TopOfScope );
		    	cout << "Put header file GenRepresentation.h into global scope" << endl;
			break;
			}
		case V_SgBasicBlock:
			{
							
			ROSE_ASSERT( isSgBasicBlock(astNode) != NULL );
                        //get filename info.
			filename = astNode->get_file_info()->get_filename();
			ROSE_ASSERT( filename.length() > 0 );
		        

			//list<ControlStructureContainer*> queryFindCommentsInScope("//pragma","GenPrintCode_",isSgScopeStatement(astNode));
                        string classPointerName = "generateRepresentation";
			string topOfScopeString("//GenRepresentation*  " + classPointerName  + " =  new GenRepresentation();\n"); 
			string bottomOfScopeString("//"+ classPointerName+ "->writeToFileAsGraph(\"" + filename + ".dot\");\n"  + "//delete " + classPointerName + ";\n");

			
 
                        //create an object of the control structure in the sourcecode.
        		returnAttribute.insert( astNode, topOfScopeString, HighLevelCollectionTypedefs::LocalScope, HighLevelCollectionTypedefs::TopOfScope );
        		returnAttribute.insert( astNode, bottomOfScopeString, HighLevelCollectionTypedefs::LocalScope, HighLevelCollectionTypedefs::BottomOfScope );
                                


			
			}
       	}

	return returnAttribute;
}

/**************************************************************************************************
 *                               ReWrite Traversal Finished                                       *
 **************************************************************************************************/

int main ( int argc, char** argv) 
{
	SgProject* project = frontend(argc,argv);
	MyTraversal treeTraversal;
	MyInheritedAttribute inheritedAttribute;

	bool debug = false;
        if (debug == true){
		AstPDFGeneration pdfOut;
		pdfOut.generateInputFiles( project );
		AstDOTGeneration dotOut;
		dotOut.generateInputFiles( project, AstDOTGeneration::PREORDER );
        }
		
	// Ignore the return value since we don't need it
	
	treeTraversal.traverse(project,inheritedAttribute);

		return backend(project);
}












