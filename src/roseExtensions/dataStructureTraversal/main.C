// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include "sage3basic.h"
#ifdef HAVE_INCLUDE_H
#include <config.h>
#endif
#include "helpFunctions.h"
#include "ControlStructure.h" 
#include "GenGeneration.h"


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
  {

    generationObjectCreatedInScope = true;

  };
  MySynthesizedAttribute (const MySynthesizedAttribute & X)
  {
    ((MySynthesizedAttribute *) this)->operator = (X);
  }

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


  bool
  generationObjNeeded ()
  {
    return generationObjectCreatedInScope;
  }
  void
  setGenerationObjCreated (bool set)
  {
    generationObjectCreatedInScope = set;
  }

private:
  bool generationObjectCreatedInScope;
};

// tree traversal to test the rewrite mechanism
//! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
class
  MyTraversal:
  public
  HighLevelRewrite::RewriteTreeTraversal <
  MyInheritedAttribute,
  MySynthesizedAttribute >
{
public:
  MyTraversal ()
  {
  };

  // Functions required by the tree traversal mechanism
  MyInheritedAttribute
    evaluateRewriteInheritedAttribute (SgNode * astNode,
                                       MyInheritedAttribute
                                       inheritedAttribute);

  MySynthesizedAttribute
    evaluateRewriteSynthesizedAttribute (SgNode * astNode,
                                         MyInheritedAttribute
                                         inheritedAttribute,
                                         SubTreeSynthesizedAttributes
                                         synthesizedAttributeList);

  string
    method_prefix;
  void
  writeCodeToFile ();
  void
  writeStaticGraphToFile ();
  void
  initialize(SgProject* project, int argc, char* argv []);
  void 
  graphGenerateProject(SgProject* project);
private:
  //The object which does the code-generation for the dynamic
  //graphing and the dot-files for the static-graphing.
  GenGeneration * codeGeneration;
  //A mapping between the SgTypedefDeclaration* and the SgType* it
  //refers to.
  map<SgTypedefDeclaration*, SgType*> typedefTranslationTable; 
  string
    filename;
};

/* The method:
 *     initialize()
 * Builds the TypedefTranslationTable and extracts all parameters
 * from the commandline.
 */
void
MyTraversal::initialize(SgProject* project, int argc, char* argv[]){
    ROSE_ASSERT( project != NULL );
    codeGeneration = new GenGeneration ();
    typedefTranslationTable = codeGeneration->buildTypedefTranslationTable(project);
    method_prefix = "save";
    int optionCount = sla(&argc, argv, "--", "($)", "(gall|graph-all)",1);
    if( optionCount > 0 )
        graphGenerateProject(project);

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
  codeGeneration->writeToFile ("CodeGenerated_" + filename);

}                               /* End function: writeCodeToFile()  */

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
  codeGeneration->writeStaticGraphToFile ("Static_" + filename + ".dot");

}                               /* End function: writeStaticGraphToFile() */

void
MyTraversal::graphGenerateProject(SgProject* project){
        GenRepresentation* staticDataGraph= codeGeneration->getPtrStaticGenRepresentation();
        ROSE_ASSERT (project != NULL);

        const SgFilePtrList sageFilePtrList = *project->get_fileList ();
        //Find  all fileNames in the project.
        list<string> fileNamesInProjectList = project->getFileNames();
        //Put the fileNames into a strctures which is more handy
        set<string>  fileNamesInProject;
        for(list<string>::iterator elm = fileNamesInProjectList.begin(); 
                        elm != fileNamesInProjectList.end(); ++elm){
                fileNamesInProject.insert(*elm);
        }

        staticDataGraph->setMapKey(project);

        staticDataGraph->beginClass(project, "The Current Project", GenRepresentation::Container);
        staticDataGraph->endClass(GenRepresentation::Container);
        

        map<SgTypedefDeclaration*, SgType*> mapSubset;

        cout << typedefTranslationTable.size() << endl;
        
        //Iterate over all files to find all class declarations
        map<string,void*> sageFileReferences; 
        for (unsigned int i = 0;  i < sageFilePtrList.size (); i += 1)
          {
            const SgFile *sageFile = isSgFile (sageFilePtrList[i]);
            ROSE_ASSERT (sageFile != NULL);
            SgGlobal *sageGlobal = sageFile->get_root ();
            ROSE_ASSERT (sageGlobal != NULL);

            SgClassDeclaration *classDeclaration;
            list < SgNode * >classDeclarationList =
              NodeQuery::querySubTree (sageGlobal->get_parent(),
                                       NodeQuery::ClassDeclarations);
            list < SgNode * >structDeclarationList =
              NodeQuery::querySubTree (sageGlobal->get_parent(),
                                       NodeQuery::StructDeclarations);
             list < SgNode * > typedefDeclarationList =
              NodeQuery::querySubTree (sageGlobal->get_parent(),
                                       NodeQuery::TypedefDeclarations);

            //find the class declarations hidden inside the typedefs and add them to the graphing
            for (list < SgNode * >::iterator typedefElm =
                 typedefDeclarationList.begin ();
                 typedefElm != typedefDeclarationList.end ();
                 ++typedefElm){
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(*typedefElm);
                    ROSE_ASSERT(typedefDeclaration != NULL);
                    SgClassType* sageClassType = NULL;
                    
                    if(isSgClassType(typedefTranslationTable[typedefDeclaration]) != NULL)
                            sageClassType = isSgClassType( typedefTranslationTable[typedefDeclaration] );
/*                  if(sageClassType == NULL){
                            cerr << "The typedefs name is: \"" << TransformationSupport::getTypeName(typedefDeclaration->get_type()) << "\"" << endl;
                            cerr << "The typedefs filename is: \"" << typedefDeclaration->getFileName() << "\"" << ":" << typedefDeclaration->get_file_info()->get_line() << endl;

                            cerr << "A SgTypedefDeclaration was not found in the typedefTranslationTable. Terminating\n";
                            exit(1);
                    }
*/                          
                    
                    if(sageClassType != NULL){
                            ROSE_ASSERT( sageClassType->get_declaration() != NULL);
                            string fileName = sageClassType->get_declaration()->getFileName();
                            classDeclarationList.push_back(sageClassType->get_declaration());
                        
                            if((fileName.find("/home/saebjorn/")!=string::npos) | 
                                            (fileNamesInProject.find(fileName)!=fileNamesInProject.end()))
                            {
                                    if(typedefTranslationTable.find(typedefDeclaration) != typedefTranslationTable.end())
                                            mapSubset[typedefDeclaration] = typedefTranslationTable.find(typedefDeclaration)->second;
                                    

                            }
                    }
           }

           //codeGeneration->graphTypedefTranslationTable(mapSubset);   

            classDeclarationList.merge (structDeclarationList);
            classDeclarationList.unique();
            //Iterate over all the found class declarations and graph them.
            for (list < SgNode * >::iterator classDeclarationElm =
                 classDeclarationList.begin ();
                 classDeclarationElm != classDeclarationList.end ();
                 ++classDeclarationElm)
              {
                classDeclaration =
                  isSgClassDeclaration (*classDeclarationElm);
                ROSE_ASSERT (classDeclaration != NULL);
#ifdef DEBUG_CGRAPHPP
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

                string fileName = classDeclaration->getFileName();
                
        
                if((fileName.find("/home/saebjorn/")!=string::npos)|
                                (fileNamesInProject.find(fileName)!=fileNamesInProject.end())){


                        //Static data graphing.
                        //If the current filename is interesting and we have not created a node in
                        //the static data representation for it, create a node.
                        if(sageFileReferences.find(fileName) == sageFileReferences.end()){
                                sageFileReferences[fileName] =classDeclaration->getFileName();
        
                                if( staticDataGraph->checkIfMapKeyExist(sageFileReferences[fileName]) == false) 
                                        staticDataGraph->setMapKey(sageFileReferences[fileName]);
                                staticDataGraph->beginClass(sageFileReferences[fileName], fileName, GenRepresentation::Container);
                                staticDataGraph->endClass(GenRepresentation::Container);
                                staticDataGraph->addEdge(project,sageFileReferences[fileName], sageFileReferences[fileName]);
                        }
                        
                        
                        //If the class is to be graphed, graph it. I do not graph a forward-declartion. The first if-test 
                        //removes all forward declarations.
                        SgClassDefinition* sageClassDefinition = isSgClassDefinition(classDeclaration->get_definition());
                        ROSE_ASSERT( sageClassDefinition != NULL);
                        if(sageClassDefinition->get_declaration() == classDeclaration)
                        if(classDeclaration->get_class_type() != SgClassDeclaration::e_union){
                                codeGeneration->printClass (classDeclaration, "", "");
                                //Static data graphing specific. Add an edge between the file-node and the
                                //class node.
                                staticDataGraph->addEdge(sageFileReferences[fileName], classDeclaration, classDeclaration);
                        }
                }

              }
          }


} /*End function graphAllVariables() */


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
 // case V_SgFile:
    case V_SgSourceFile:
    case V_SgBinaryComposite:
      {
        //set filename information
        SgFile *sageFile = isSgFile (astNode);
        ROSE_ASSERT (sageFile != NULL);
        filename = sageFile->getFileName ();


        break;

      }
    case V_SgGlobal:
      {
        string includeString ("//#include \"GenRepresentation.h\"");
        returnAttribute.insert (astNode, includeString,
                                HighLevelCollectionTypedefs::GlobalScope,
                                HighLevelCollectionTypedefs::TopOfScope);
        cout << "Put header file GenRepresentation.h into global scope" <<
          endl;


        break;
      }
    case V_SgBasicBlock:
      {

        bool graphAllVariables = false;

        ROSE_ASSERT (isSgBasicBlock (astNode) != NULL);
        //get filename info.
        filename = astNode->get_file_info ()->get_filename ();
        ROSE_ASSERT (filename.length () > 0);
        
        //find all the pragma-statemants which tells what to graph
        //list<SgNode*> pragmaStatements = NodeQuery::querySubTree(astNode, new SgName("GenPrintCode_"),queryNodePragmaDeclarationFromName);
        list < ControlStructureContainer * >controlStatements =
          queryFindCommentsInScope ("//pragma", "GenPrintCode_",
                                    isSgScopeStatement (astNode));
        //See if all variables should be graphed
        list < ControlStructureContainer * >pragmaPrintAllStatements; /*=
          queryFindCommentsInScope ("//pragma", "GenPrintAllVariables",
                                    isSgScopeStatement (astNode));*/
       
        if(pragmaPrintAllStatements.empty() != true){
              cout << "pragmaprint: " << pragmaPrintAllStatements.size() << endl;
              cout << "All variables will be printed" << endl; 
              graphAllVariables = true;
        }


        //list<ControlStructureContainer*> queryFindCommentsInScope("//pragma","GenPrintCode_",isSgScopeStatement(astNode));
        if((controlStatements.empty() != true)|graphAllVariables == true){
                string classPointerName = "generateRepresentation";
                string topOfScopeString ("//GenRepresentation*  " + classPointerName +
                                 " =  new GenRepresentation();\n");
                string bottomOfScopeString ("//" + classPointerName +
                                    "->writeToFileAsGraph(\"" + filename +
                                    ".dot\");\n" + "//delete " +
                                    classPointerName + ";\n");
                returnAttribute.insert (astNode, topOfScopeString,
                                        HighLevelCollectionTypedefs::
                                        SurroundingScope,
                                        HighLevelCollectionTypedefs::
                                        TopOfScope);
                returnAttribute.insert (astNode, bottomOfScopeString,
                                        HighLevelCollectionTypedefs::
                                        SurroundingScope,
                                        HighLevelCollectionTypedefs::
                                        BottomOfScope);

                //Create an easier structure which can hold the variable names which should be graphed
                map<string,ControlStructureContainer*> variableNamesTograph;
                typedef list <ControlStructureContainer*>::iterator pragmaIterator; 
                for ( pragmaIterator pragmaElm = controlStatements.begin (); pragmaElm != controlStatements.end (); ++pragmaElm)
                {
                     ControlStructureContainer *controlStructure = *pragmaElm;

                     //get the variable which corresponds to the variable name in the control structure
                     string variableName = "";

                     //If the parseing is unsucsessful parsePragmaStringLHS returns a sting ""
                     variableName =
                        parsePragmaStringLHS (controlStructure->getPragmaString (),
                                    "GenPrintCode_", "=");
                     ROSE_ASSERT( variableName != "" );

                     variableNamesTograph[variableName]=controlStructure;

                     cout << variableName << endl;                   
#ifdef DEBUG_CGRAPHPP
                     cout << "*****************" << controlStructure->
                          getPragmaString () << " :: " << variableName << "**************" << endl;
#endif
                }
                //find all parent scopes in namespace, so that it is possible to search for 
                //a class declaration corresponding to the typename in the namespace.
                SgNodePtrVector scopesVector = findScopes (astNode);
                //find all variable declarations in scope so that it is later possible to graph the parts of it specified by the pragmas.
                list < SgNode * > variableDeclarations =
                       NodeQuery::querySubTree (astNode, NodeQuery::VariableDeclarations);


                //Iterate over all variables and graph those specified by the pragmas
                for (list < SgNode * >::iterator variableElement =
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

                SgInitializedNameList sageInitializedNameList =
                  sageVariableDeclaration->get_variables ();

                
                for (SgInitializedNameList::iterator k = sageInitializedNameList.begin ();
                     k != sageInitializedNameList.end (); ++k)
                  {
                    SgInitializedName elmVar = *k;

                    string variableName = elmVar.get_name ().str ();

                    cout << "The variablename is: " << variableName << endl;
                    if( (graphAllVariables == true) || (variableNamesTograph.find(variableName) != variableNamesTograph.end()) )
                    {
                    ROSE_ASSERT (variableName.length () > 0);

                    SgType *sageType = isSgType (elmVar.get_type ());
                    ROSE_ASSERT(sageType != NULL);

                    if (sageType->variantT () == V_SgPointerType)
                      {
                        list<SgNode*> classDeclarationList;
                        string typeName =
                          TransformationSupport::getTypeName (sageType);

                        cout << "The typename is: "<< typeName << endl;


                        SgTypedefDeclaration *typedefDeclaration =
                                findTypedefFromTypeName (scopesVector, typeName);
                  
                        SgType* sageType = NULL;
                        SgClassType* classType = NULL;

                        if(typedefDeclaration != NULL){
                                cout << "found typedef" << endl;
                                //sageType = isSgType(typedefTranslationTable[typedefDeclaration]);

                                cout << endl << typedefTranslationTable.size() << endl;
                                if(typedefTranslationTable.find(typedefDeclaration) != typedefTranslationTable.end()){
                                        sageType = typedefTranslationTable.find(typedefDeclaration)->second;
                                        ROSE_ASSERT( typedefTranslationTable[typedefDeclaration] != NULL );
                                        cout <<  TransformationSupport::getTypeName(typedefTranslationTable.find(typedefDeclaration)->second)<<endl;
                                        ROSE_ASSERT(sageType != NULL);
                                        cout << "The typedefDeclaration was found in the table"<< TransformationSupport::getTypeName(sageType)<<endl;
                                }
                                classType = isSgClassType(sageType);
                 
                                if(classType != NULL){
                                        string sageTypeName = TransformationSupport::getTypeName(classType);
                                        cout << "The typename of the corresponding class is:" << sageTypeName << endl;
                                        if( sageTypeName != "" )
                                                classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, sageTypeName);
                                        else
                                                classDeclarationList.push_back(classType->get_declaration());
                                }
                        }else
                                classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, typeName);

                        
                        /*list < SgNode * >classDeclarationList =
                          findClassDeclarationsFromTypeName (scopesVector,
                                                             typeName);*/

                        ROSE_ASSERT (classDeclarationList.empty () != true);
                        SgClassDeclaration *classDeclaration =
                          isSgClassDeclaration (*classDeclarationList.
                                                begin ());
                        ROSE_ASSERT (classDeclaration != NULL);

                        string graphString;
                        int cnt = 0;
                
                        if (classDeclaration->get_class_type () ==
                            SgClassDeclaration::e_class)
                          graphString =
                            "//" + variableName + "->" + method_prefix + "(" +
                            classPointerName + ",\"" + variableName +
                            "\",GenRepresentation::Container);\n";
                        else{
                          graphString =
                            "//" + method_prefix + "_" +
                            classDeclaration->
                            get_mangled_qualified_name (cnt).str () + "(" +
                            classPointerName + ",\"" + variableName + "\"," +
                            variableName +
                            ",GenRepresentation::Container);\n";
                        }
                        //returnAttribute.insert((*i)->getAssociatedStatement(), graphString, HighLevelCollectionTypedefs::LocalScope, HighLevelCollectionTypedefs::BeforeCurrentPosition);
                        MiddleLevelRewrite::
                          insert (isSgStatement
                                  ((variableNamesTograph[variableName])->getAssociatedStatement ()),
                                  graphString,
                                  MidLevelCollectionTypedefs::
                                  SurroundingScope,
                                  MidLevelCollectionTypedefs::
                                  BeforeCurrentPosition);

                        for (list<SgNode*>::iterator classDeclarationElm =
                             classDeclarationList.begin ();
                             classDeclarationElm !=
                             classDeclarationList.end ();
                             ++classDeclarationElm)
                          {
                            classDeclaration =
                              isSgClassDeclaration (*classDeclarationElm);
                            ROSE_ASSERT (classDeclaration != NULL);
#ifdef DEBUG_CGRAPHPP
                            int lakke = 0;
                            cout <<
                              "\n\n**************************************\n\n";
                            cout << isSgClassDeclaration (classDeclaration)->
                              get_qualified_name ().str () << endl;
                            cout << isSgClassDeclaration (classDeclaration)->
                              get_mangled_qualified_name (lakke).
                              str () << endl;
                            cout << isSgClassDeclaration (classDeclaration)->
                              get_mangled_qualified_name (lakke).
                              str () << endl;

                            cout <<
                              "***************************************\n\n";
#endif
                            codeGeneration->printClass (classDeclaration, "",
                                                        "");
                          }


                      }
                    else
                      {

                        cout << variableName <<
                          ": Not graphed since it is not pointer." << endl;

                      }
                    } /* End if( graphVariable ) */ 
                  }             /* End iterator of SgInitializedName */
              } /* End iterator over variableDeclartions */ 
        } /* End if( start graphVariable) */
      } /* End case SgBlock */              
    } /* End switch-case */      


  return returnAttribute;
} /* End method:  evaluateSynthesizedAttribute() */

/**************************************************************************************************
 *                               ReWrite Traversal Finished                                       *
 **************************************************************************************************/

int
main (int argc, char **argv)
{
  SgProject *project = frontend (argc, argv);
  AstTests::runAllTests(project);
  MyTraversal treeTraversal;
  MyInheritedAttribute inheritedAttribute;


  bool debug = true;
  if (debug == true)
    {
      AstPDFGeneration pdfOut;
      pdfOut.generateInputFiles (project);
      AstDOTGeneration dotOut;
      dotOut.generateInputFiles (project, AstDOTGeneration::PREORDER);
    }

  // Ignore the return value since we don't need it

  treeTraversal.initialize(project, argc, argv);
  treeTraversal.traverse (project, inheritedAttribute);
  //Writes the generated code for dynamic graphing to file
  treeTraversal.writeCodeToFile ();
  //Writes the code for static data graphing to file
  treeTraversal.writeStaticGraphToFile ();

  return backend (project);
}
