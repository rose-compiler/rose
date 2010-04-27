// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
//#include "rose.h"
#include "rose.h"
#include "helpFunctions.h"
#ifdef HAVE_INCLUDE_H
#include <config.h>
#endif

#include "ControlStructure.h"
#include "GenGeneration.h"


// Extra header for customizing the rewrite mechanism
#include "rewriteTemplateImpl.h"

#include <vector>

using namespace std;

#define DEBUG  0

/*********************************************************************************
 *                          The ReWrite Traversal                                *
 *********************************************************************************/

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class MyInheritedAttribute
{
public:
  MyInheritedAttribute ()
  {
  };
};

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class MySynthesizedAttribute:public
  HighLevelRewrite::SynthesizedAttribute
{
public:
  MySynthesizedAttribute ()
  {  generationObjectCreatedInScope = true;  };
  
  MySynthesizedAttribute (const MySynthesizedAttribute & X)
  { ((MySynthesizedAttribute *) this)->operator = (X); };

  MySynthesizedAttribute & operator = (const MySynthesizedAttribute & X)
  {
    generationObjectCreatedInScope = X.generationObjectCreatedInScope;
    HighLevelRewrite::SynthesizedAttribute::operator = (X);
    return *this;
  }
  MySynthesizedAttribute & operator += (const MySynthesizedAttribute & X)
  {
    operator = (X);
    return *this;
  }


  bool generationObjNeeded ()
   {  return generationObjectCreatedInScope; }
  void setGenerationObjCreated (bool set)
  { generationObjectCreatedInScope = set; }

private:
  bool generationObjectCreatedInScope;
};

// tree traversal to test the rewrite mechanism
//! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
class MyTraversal:
public
  HighLevelRewrite::RewriteTreeTraversal <
  MyInheritedAttribute, MySynthesizedAttribute >
{
public:
  MyTraversal ()
  {};

  // Functions required by the tree traversal mechanism
  MyInheritedAttribute
    evaluateRewriteInheritedAttribute (SgNode * astNode,
				       MyInheritedAttribute inheritedAttribute);
				      
  MySynthesizedAttribute
    evaluateRewriteSynthesizedAttribute (SgNode * astNode,
					 MyInheritedAttribute inheritedAttribute,
					 SubTreeSynthesizedAttributes synthesizedAttributeList);

					 
  string method_prefix;
  ostringstream* graphTypedefTranslations (Rose_STL_Container<SgTypedefType*> typeList);

  void writeCodeToFile ();
  void writeStaticGraphToFile ();
  void initialize (SgProject * project, vector<string>& argvList);
  void graphGenerateProject (SgProject * project);
 private:
  //The object which does the code-generation for the dynamic
  //graphing and the dot-files for the static-graphing.

  GenRepresentation * staticDataGraph;
  GenGeneration *codeGeneration; 
  //A mapping between the SgTypedefDeclaration* and the SgType* it
  //refers to.
  string filename;
};

/* The method:
 *     initialize()
 * Builds the TypedefTranslationTable and extracts all parameters
 * from the commandline.
 */
void
MyTraversal::initialize (SgProject * project, vector<string>& argvList)
{
  ROSE_ASSERT (project != NULL);
  codeGeneration = new GenGeneration ();
  staticDataGraph = codeGeneration->getPtrStaticGenRepresentation ();

  method_prefix = "save";
  int optionCount = sla (argvList, "--", "($)", "(gall|graph-all)", 1);
  if (optionCount > 0)
    graphGenerateProject (project);

}


/*
 * The function:
 *    writeCodeToFile()
 * writes the c++ code needed to generate dot code to graph the 
 * dynamic data structure. 
 *
 */
void
MyTraversal::writeCodeToFile ()
{
  ROSE_ASSERT (filename.length () > 0);
  codeGeneration->writeToFile ("CodeGenerated_" + StringUtility::stripPathFromFileName(filename));

}				/* End function: writeCodeToFile()  */

/*
 *  The function:
 *    writeStaticGraphToFile()
 *  writes a dot graph of the static data structure to file.
 *
 */
void
MyTraversal::writeStaticGraphToFile ()
{
  ROSE_ASSERT (filename.length () > 0);
  codeGeneration->writeStaticGraphToFile ("Static_" + StringUtility::stripPathFromFileName(filename) + ".dot");

}				/* End function: writeStaticGraphToFile() */

bool compare_typedefs(SgTypedefType* x, SgTypedefType* y){
    ROSE_ASSERT( x != NULL );
    ROSE_ASSERT( y != NULL );
 
    return x->get_name() < y->get_name();
};

ostringstream *
MyTraversal::graphTypedefTranslations (Rose_STL_Container<SgTypedefType * >typeList)
{
  string name;
  vector < SgType * > typeVector; 
  //sort the list before graphing


  // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
     printf ("Commented out sort() member function since it is not in std::vector class \n");
  // typeList.sort( compare_typedefs);

  ostringstream *codeGenerated = new ostringstream ();
  staticDataGraph->beginNode ("Type Translation Table",
			       GenRepresentation::Container,staticDataGraph);
  string classPointerName = "classReference";
  for (Rose_STL_Container< SgTypedefType * >::iterator mapElm = typeList.begin ();
       mapElm != typeList.end (); ++mapElm)
    {
      SgTypedefType *sageTypedefType =
       	isSgTypedefType (*mapElm);
      ROSE_ASSERT (sageTypedefType != NULL);

      typeVector = typeVectorFromType (sageTypedefType);
	  name = typeStringFromType (sageTypedefType);

      (*codeGenerated) << classPointerName + "->addVariable(\" " +
	    name + "\", \"" + sageTypedefType->get_name ().str () +  "\", \"\");\n";

	  //static
      staticDataGraph->addVariable (string(sageTypedefType->get_name ().str ()), "  " + name, "");

	}

  staticDataGraph->endNode (GenRepresentation::Container);
  return codeGenerated;
}

void
MyTraversal::graphGenerateProject (SgProject * project)
{
  ROSE_ASSERT (project != NULL);

  const SgFilePtrList& sageFilePtrList = project->get_fileList ();
  //Find  all fileNames in the project.

// DQ (9/3/2008): Use the new function with a clearer name.
// Rose_STL_Container< string > fileNamesInProjectList = project->getFileNames ();
  Rose_STL_Container< string > fileNamesInProjectList = project->getAbsolutePathFileNames();

  //Put the fileNames into a strctures which is more handy
  set < string > fileNamesInProject;
  for (Rose_STL_Container< string >::iterator elm = fileNamesInProjectList.begin ();
       elm != fileNamesInProjectList.end (); ++elm)
      fileNamesInProject.insert (*elm);

  staticDataGraph->setMapKey (project);

  staticDataGraph->beginNode ("The Current Project",
			       GenRepresentation::Container,project);
  staticDataGraph->endNode (GenRepresentation::Container);
  SgGlobal *sageGlobal = NULL;

  //Iterate over all files to find all class declarations
  map < string, const void *> sageFileReferences;

  for (unsigned int i = 0; i < sageFilePtrList.size (); i += 1)
    {
      const SgSourceFile *sageFile = isSgSourceFile (sageFilePtrList[i]);
      ROSE_ASSERT (sageFile != NULL);
      sageGlobal = sageFile->get_globalScope();
      ROSE_ASSERT (sageGlobal != NULL);

      //gets all the declarations from the global scope.
      SgDeclarationStatementPtrList declarationStmntPtrLst =
          	sageGlobal->get_declarations ();

      //sort the different SgDeclarationStmt's for cleaner graphing.
      Rose_STL_Container< SgTypedefType * > typedefDeclarationLst;
      Rose_STL_Container< SgNode * > globalDeclarationLst,globalDeclarationLst2,enumDeclarationLst,classDeclarationLst;

      for (SgDeclarationStatementPtrList::iterator declarationElm =
	   declarationStmntPtrLst.begin ();
	   declarationElm != declarationStmntPtrLst.end (); ++declarationElm)
       	{
	     SgDeclarationStatement *sageDeclStmtPtr =
	             isSgDeclarationStatement (*declarationElm);
	     ROSE_ASSERT (sageDeclStmtPtr != NULL);
	     string fileName = sageDeclStmtPtr->getFilenameString();
#ifdef GENGEN_DEBUG
         cout << fileName << endl;
#endif          
	  
         if ((fileName.find ("/usr/") == string::npos)) //|
	     // (fileNamesInProject.find (fileName) !=
	     //  fileNamesInProject.end ()))
	  switch (sageDeclStmtPtr->variantT ())
	    {
	    case V_SgVariableDeclaration:
         {
	      SgVariableDeclaration* sageVariableDeclaration = isSgVariableDeclaration(sageDeclStmtPtr);
          ROSE_ASSERT(sageVariableDeclaration != NULL);
          SgVariableDefinition* sageVariableDefinition = isSgVariableDefinition(sageVariableDeclaration->get_definition());
              
              
          if(sageVariableDefinition != NULL)
              globalDeclarationLst.push_back (sageDeclStmtPtr);
               
              //codeGeneration->printType(sageVariableDefinition->get_type());
	      break;
              }
        case V_SgTypedefDeclaration:
         {
          //globalDeclarationLst.push_back (sageDeclStmtPtr);
          SgTypedefDeclaration *typedefDeclaration =
	        isSgTypedefDeclaration (sageDeclStmtPtr);
          ROSE_ASSERT (typedefDeclaration != NULL);

		  SgType *sageType = typedefDeclaration->get_type ();
		  ROSE_ASSERT (sageType != NULL);
#ifdef GENGEN_DEBUG
                cout << "line 477 main.C. We have a typedefDeclaration"
                     << TransformationSupport::getTypeName(typedefDeclaration->get_type()) << endl;
#endif
                codeGeneration->printType(sageType);     
		  //typedefDeclarationLst.push_back (typedefDeclaration);

		  vector < SgType * >typeVector = typeVectorFromType (sageType);

          //If the typedef is a typedefinition of a class-type, add the variable to the list which
          //is to be graphed.
		  if (typeVector[typeVector.size () - 1]->variantT () ==   V_SgClassType)
		    {
		    SgClassType *sageClassType =
		      isSgClassType (typeVector[typeVector.size () - 1]);
		    ROSE_ASSERT (sageClassType != NULL);
            string typeName =
 	             TransformationSupport::getTypeName (sageClassType);
#ifdef GENGEN_DEBUG
            cout << "The typename in main.C line 494 is " << typeName << endl;
#endif
		    ROSE_ASSERT (sageClassType->get_declaration () != NULL);
            SgNodePtrVector scopesVector = findScopes (sageDeclStmtPtr);

	       	Rose_STL_Container<SgNode*> classDeclLst;
            if (typeName.length () > 0){
               classDeclLst =
			     findClassDeclarationsFromTypeName (scopesVector,typeName);

               //ROSE_ASSERT(isSgClassDeclaration(sageClassType->get_declaration()) != NULL);
               //codeGeneration->printClass(sageClassType->get_declaration(),"",""); 
	           //This is in the case of an anonymous union
	           if (classDeclLst.empty () == true)
			     {
			     SgClassDeclaration *clDecl =
			      isSgClassDeclaration (sageClassType->get_declaration ());
			     ROSE_ASSERT (clDecl != NULL);
                 classDeclLst.push_back(clDecl);
                                    
			     //  globalDeclarationLst.push_back (clDecl);
			     }//else 
               //globalDeclarationLst.merge(classDeclLst);

            // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
            // classDeclarationLst.merge(classDeclLst);
               classDeclarationLst.insert(classDeclarationLst.end(),classDeclLst.begin(),classDeclLst.end());
            }
		  }else{
            //The typedef is not of class type. Graph the translation of it in 
            //the typedefTranslationTable
            if(isSgTypedefType(typedefDeclaration->get_type())!=NULL)
              typedefDeclarationLst.push_back(typedefDeclaration->get_type());
          }

          //codeGeneration->printType(sageType);
          break;
                }
        case V_SgClassDeclaration:
          {
          SgClassDeclaration* sageClassDeclaration =  
                   isSgClassDeclaration(sageDeclStmtPtr);
          ROSE_ASSERT( sageClassDeclaration != NULL );

          SgType* sageType = sageClassDeclaration->get_type();
          ROSE_ASSERT(sageType != NULL);
          classDeclarationLst.push_back(sageClassDeclaration);
          //globalDeclarationLst.push_back(sageClassDeclaration);
          //codeGeneration->printType(sageType);
          }
/*	    case V_SgEnumDeclaration:
	      enumDeclarationLst.push_back (sageDeclStmtPtr);
	      break;
	    case V_SgTypedefDeclaration:
	      {
		//The typedefDeclarations is special since it may be an anonymous typedef.
		//The class declaration of the class in the anonymous typedef will be 
		//given a name by Sage and graphed separatedly from the translation of the
		//typedef itself. 

		SgTypedefDeclaration *typedefDeclaration =
		  isSgTypedefDeclaration (sageDeclStmtPtr);
		ROSE_ASSERT (typedefDeclaration != NULL);

		typedefDeclarationLst.push_back (typedefDeclaration);

		SgType *sageType = typedefDeclaration->get_type ();
		ROSE_ASSERT (typedefDeclaration->get_type () != NULL);

		vector < SgType * >typeVector = typeVectorFromType (sageType);

		if (typeVector[typeVector.size () - 1]->variantT () ==
		    V_SgClassType)
		  {
		    SgClassType *sageClassType =
		      isSgClassType (typeVector[typeVector.size () - 1]);
		    ROSE_ASSERT (sageClassType != NULL);
		    ROSE_ASSERT (sageClassType->get_declaration () != NULL);

		    classDeclarationLst.push_back (sageClassType->
						   get_declaration ());
		  }

		break;
	      }
*/
	    default:{
	      break;
            }

	  }
      }

   // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
      printf ("Commented out unique() member function since it is not in std::vector class \n");
   // classDeclarationLst.unique ();

      //Iterate over all the found class declarations and graph them.
      for (Rose_STL_Container<SgNode*>::iterator classDeclarationElm =
	   classDeclarationLst.begin ();
	   classDeclarationElm != classDeclarationLst.end ();
	   ++classDeclarationElm)
	{
          SgClassDeclaration *classDeclaration = isSgClassDeclaration (*classDeclarationElm);
	  ROSE_ASSERT (classDeclaration != NULL);
#ifdef GENGEN_DEBUG 
	  int lakke = 0;
	  cout << "\n\n**************************************\n\n";
	  cout << isSgClassDeclaration (classDeclaration)->
	    get_qualified_name ().str () << endl;
	  cout << isSgClassDeclaration (classDeclaration)->
	    get_mangled_qualified_name (lakke).str () << endl;
	  cout << isSgClassDeclaration (classDeclaration)->
	    get_mangled_qualified_name (lakke).str () << endl;

	  cout << "***************************************\n\n";
#endif

	  string fileName = classDeclaration->getFilenameString();

      //Because of a bug, some files in the GNU C Standard Library generates
      //an error message if processed by ROSE. Therefore all files from the 
          if(fileName.find("/usr")==string::npos)
	    {


	      //Static data graphing.
	      //If the current filename is interesting and we have not created a node in
	      //the static data representation for it, create a node.
	      if (sageFileReferences.find (fileName) ==
		  sageFileReferences.end ())
		{
		  sageFileReferences[fileName] = classDeclaration->getFilenameString().c_str();

		  if (staticDataGraph->
		      checkIfMapKeyExist (sageFileReferences[fileName]) ==
		      false)
		    staticDataGraph->setMapKey (sageFileReferences[fileName]);
		  staticDataGraph->beginNode (fileName,
					       GenRepresentation::Container,
                                               sageFileReferences[fileName]);
		  staticDataGraph->endNode (GenRepresentation::Container);
		  staticDataGraph->addEdge (project,
					    sageFileReferences[fileName],
					    sageFileReferences[fileName]);
		}


	      //If the class is to be graphed, graph it. I do not graph a forward-declartion. The first if-test 
	      //removes all forward declarations.
	      SgClassDefinition *sageClassDefinition =
		isSgClassDefinition (classDeclaration->get_definition ());
	      ROSE_ASSERT (sageClassDefinition != NULL);
	      //if (sageClassDefinition->get_declaration () == classDeclaration)
		if (classDeclaration->get_class_type () !=
		    SgClassDeclaration::e_union)
		  {
                    //AS(140804) REMOVED BECAUSE OF INTRODUCTION OF printType)
		    codeGeneration->printClass (classDeclaration, "", "");
		    //Static data graphing specific. Add an edge between the file-node and the
		    //class node.
		    staticDataGraph->addEdge (sageFileReferences[fileName],
					      classDeclaration,
					      classDeclaration);
		  }
	    }

	}
  //For now I will treat the enums and variable declarations in the same list.
  //AS(120804) enums cannot be treated well, have to skip them for now
  //globalDeclarationLst.merge (enumDeclarationLst);
  //globalDeclarationLst.merge(classDeclarationLst);
  //Graph the variable declarations in global space
  codeGeneration->printGlobalScope (sageGlobal, globalDeclarationLst);
  //codeGeneration->printGlobalScope (sageGlobal, declarationStmntPtrLst);

  //graph the translation of the typename to the TypedefDeclarations into
  //their corresponding types.
  graphTypedefTranslations (typedefDeclarationLst);

  }



}				/*End function graphAllVariables() */


// Functions required by the tree traversal mechanism
MyInheritedAttribute
  MyTraversal::evaluateRewriteInheritedAttribute (SgNode * astNode,
						  MyInheritedAttribute
						  inheritedAttribute)
{
  // Note that any constructor will do
  MyInheritedAttribute returnAttribute;

  return returnAttribute;
}

MySynthesizedAttribute
  MyTraversal::evaluateRewriteSynthesizedAttribute (SgNode * astNode,
						    MyInheritedAttribute
						    inheritedAttribute,
						    SubTreeSynthesizedAttributes
						    synthesizedAttributeList)
{
  // Note that any constructor will do
  MySynthesizedAttribute returnAttribute;

  switch (astNode->variantT ())
    {
    case V_SgFile:
      {
	//set filename information
	SgFile *sageFile = isSgFile (astNode);
	ROSE_ASSERT (sageFile != NULL);
	filename = StringUtility::stripPathFromFileName(sageFile->getFileName ());
	break;

      }
    case V_SgGlobal:
      {
    //Include GenRepresenation.h in global scope so that graphing
    //at runtime is possible
 	string includeString ("//#include \"GenRepresentation.h\"");
	returnAttribute.insert (astNode, includeString,
				HighLevelCollectionTypedefs::GlobalScope,
				HighLevelCollectionTypedefs::TopOfScope);
	break;
      }
    case V_SgBasicBlock:
      {
#ifdef GENGEN_DEBUG
	cout << endl << astNode->variantT () << endl;
#endif
	bool graphAllVariables = false;

	ROSE_ASSERT (isSgBasicBlock (astNode) != NULL);
	filename = StringUtility::stripPathFromFileName(astNode->get_file_info ()->get_filename ());
	ROSE_ASSERT (filename.length () > 0);

	//If a pragma statement is in one of the form described in the manual 
    //it is put in a list which information will be extracted from later.
	Rose_STL_Container< ControlStructureContainer * >controlStatements = queryFindCommentsInScope ("//pragma", "GenPrintCode_", isSgScopeStatement (astNode));

	//See if all variables should be graphed
	if(queryFindCommentsInScope ("//pragma", "GenPrintAllVariables",
				    isSgScopeStatement (astNode)).empty()!=true){
#ifdef GENGEN_DEBUG
   	    cout << "All variables will be printed" << endl;
#endif
	    graphAllVariables = true;
    }


	if ((controlStatements.empty () != true) || graphAllVariables == true)
	  {

	    map < string, ControlStructureContainer * >variableNamesTograph;
	    typedef Rose_STL_Container<ControlStructureContainer*>::iterator pragmaIterator;
	    //Find all parent scopes in namespace, so that it is possible to search for 
	    //a class declaration corresponding to the typename in the namespace.
	    SgNodePtrVector scopesVector = findScopes (astNode);
   	    //Find all variable declarations in scope so that it is later possible to 
        //graph the parts of it specified by the pragmas.
	    Rose_STL_Container<SgNode*> variableDeclarations = NodeQuery::querySubTree (astNode,NodeQuery::VariableDeclarations);
				       
	    string classPointerName = "generateRepresentation";
	    string topOfScopeString ("//GenRepresentation*  " +
				     classPointerName +
				     " =  new GenRepresentation();\n");
	    string bottomOfScopeString ("//" + classPointerName +
					"->writeToFileAsGraph(\"Dynamic_" +
					filename + ".dot\");\n" +
					"//delete " + classPointerName +
					";\n");
        //AS(010904) Commenting out rewrites because of problems with anon class-like structures
	    returnAttribute.insert (astNode, topOfScopeString,
				    HighLevelCollectionTypedefs::
				    SurroundingScope,
				    HighLevelCollectionTypedefs::TopOfScope);
	    returnAttribute.insert (astNode, bottomOfScopeString,
				    HighLevelCollectionTypedefs::
				    SurroundingScope,
				    HighLevelCollectionTypedefs::
				    BottomOfScope);
            
	    //Create an easier structure which can hold the variable names which should be graphed
	    for (pragmaIterator pragmaElm = controlStatements.begin ();
		 pragmaElm != controlStatements.end (); ++pragmaElm)
	      {
		ControlStructureContainer *controlStructure = *pragmaElm;

		//get the variable which corresponds to the variable name in the control structure
		string variableName = "";

		//If the parseing is unsucsessful parsePragmaStringLHS returns a sting ""
		variableName =
		  parsePragmaStringLHS (controlStructure->getPragmaString (),
					"GenPrintCode_", "=");
		ROSE_ASSERT (variableName != "");

		variableNamesTograph[variableName] = controlStructure;
#ifdef GENGEN_DEBUGP
		cout << variableName << endl;
		cout << controlStructure->getPragmaString () << " :: " << variableName << endl;
#endif
	      }


	    //Iterate over all variables and graph those specified by the pragmas
	    for (Rose_STL_Container< SgNode * >::iterator variableElement =
		 variableDeclarations.begin ();
		 variableElement != variableDeclarations.end ();
		 ++variableElement)
	      {
		SgVariableDeclaration *sageVariableDeclaration =
		  isSgVariableDeclaration (*variableElement);
		ROSE_ASSERT (sageVariableDeclaration != NULL);
		SgVariableDefinition *sageVariableDefinition =
		  isSgVariableDefinition (sageVariableDeclaration->
					  get_definition ());
		ROSE_ASSERT (sageVariableDefinition != NULL);

		Rose_STL_Container<SgInitializedName*> sageInitializedNameList =
		  sageVariableDeclaration->get_variables ();


		for (Rose_STL_Container<SgInitializedName*>::iterator k =
		     sageInitializedNameList.begin ();
		     k != sageInitializedNameList.end (); ++k)
		  {
		    SgInitializedName* elmVar = *k;
		    string variableName = elmVar->get_name ().str ();
#ifdef GENGEN_DEBUGP
		    cout << "The variablename is: " << variableName << endl;
#endif
		    if ((graphAllVariables == true)
			|| (variableNamesTograph.find (variableName) !=
			    variableNamesTograph.end ()))
		      {
			ROSE_ASSERT (variableName.length () > 0);

			SgType *sageType = isSgType (elmVar->get_type ());
			ROSE_ASSERT (sageType != NULL);

			vector < SgType * >typeVector =
			  typeVectorFromType (sageType);

			if (typeVector[0]->variantT () == V_SgPointerType)
			  {
			    Rose_STL_Container< SgNode * >classDeclarationList;
			    string typeName =
			      TransformationSupport::getTypeName (sageType);

			    if (typeVector[typeVector.size () - 1]->
				variantT () == V_SgClassType)
			      {
				SgClassType *classType =
				  isSgClassType (typeVector
						 [typeVector.size () - 1]);
				ROSE_ASSERT (classType != NULL);
				string sageTypeName =
				  TransformationSupport::getTypeName (classType);
				
				classDeclarationList =
				  findClassDeclarationsFromTypeName(scopesVector, sageTypeName);
				classDeclarationList.push_back (classType->get_declaration());


         // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
            printf ("Commented out unique() member function since it is not in std::vector class \n");
         // classDeclarationList.unique ();

				ROSE_ASSERT (classDeclarationList.empty () != true);
					     
				SgClassDeclaration *classDeclaration =
				  isSgClassDeclaration (classType->get_declaration ());
				ROSE_ASSERT (classDeclaration != NULL);

				string graphString;
				int cnt = 0;

				if (classDeclaration->get_class_type () ==
				    SgClassDeclaration::e_class)
				  graphString =
				    "//" + variableName + "->" + method_prefix + "(" + classPointerName 
				    +",\"" + variableName + "\",GenRepresentation::Container);\n";
				else
				  graphString =
				    "//" + method_prefix + "_" +  classDeclaration->get_mangled_qualified_name (cnt).str ()
				    + "(" + classPointerName + ",\"" + variableName + "\"," + variableName +
				    ",GenRepresentation::Container);\n";
                //AS(010904) commented out because of possible problem
                //          with anon class-like structures
			    MiddleLevelRewrite::insert (isSgStatement((variableNamesTograph[variableName])->getAssociatedStatement ()),
				   graphString, MidLevelCollectionTypedefs::SurroundingScope,MidLevelCollectionTypedefs::BeforeCurrentPosition);
				   
				for (Rose_STL_Container<SgNode *>::iterator classDeclarationElm =
				     classDeclarationList.begin ();
				     classDeclarationElm != classDeclarationList.end ();
				     ++classDeclarationElm)
				  {
				    classDeclaration =
				      isSgClassDeclaration
				      (*classDeclarationElm);
				    ROSE_ASSERT (classDeclaration != NULL);
#ifdef GENGEN_DEBUG
				    int lakke = 0;
				    cout <<
				      "\n\n**************************************\n\n";
				    cout << isSgClassDeclaration(classDeclaration)->get_qualified_name ().str () << endl;
				    cout << isSgClassDeclaration(classDeclaration)->get_mangled_qualified_name (lakke).str () << endl;
				    cout << isSgClassDeclaration(classDeclaration)->get_mangled_qualified_name (lakke).str () << endl;
				    cout <<
				      "***************************************\n\n";
#endif
				    codeGeneration->
				      printClass (classDeclaration, "", "");
				  }

			      }	/* end if(typeVector[typeVector.size()-1]->variantT() == V_SgClassType) */
			  }
			else
			  {
#ifdef GENGEN_DEBUG
			    cout << variableName << ": Not graphed since it is not pointer." << endl;
#endif

			  }
		      }		/* End if( graphVariable ) */
		  }		/* End iterator of SgInitializedName */
	      }			/* End iterator over variableDeclartions */
	  }			/* End if( start graphVariable) */
      }				/* End case SgBlock */
    default:
      break;
    }				/* End switch-case */


  return returnAttribute;
}				/* End method:  evaluateSynthesizedAttribute() */

/**************************************************************************************************
 *                               ReWrite Traversal Finished                                       *
 **************************************************************************************************/

int
main (int argc, char **argv)
{
  vector<string> argvList(argv, argv + argc);

  SgProject *project = frontend (argvList);
  AstTests::runAllTests (project);
  ////assert(frontEndErrorCode<3);
  MyTraversal treeTraversal;
  MyInheritedAttribute inheritedAttribute;


  // Ignore the return value since we don't need it
  treeTraversal.initialize (project, argvList);
  treeTraversal.traverse (project, inheritedAttribute);
  //Writes the generated code for dynamic graphing to file
  treeTraversal.writeCodeToFile ();
  //Writes the code for static data graphing to file
  treeTraversal.writeStaticGraphToFile ();
  bool debug = true;
  if (debug == true)
    {
      AstPDFGeneration pdfOut;
      pdfOut.generateInputFiles (project);
      AstDOTGeneration dotOut;
      dotOut.generateInputFiles (project, AstDOTGeneration::PREORDER);
    }


  return backend (project);
}
