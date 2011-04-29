#include "sage3basic.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include "GenGeneration.h"


NodeQuerySynthesizedAttributeType
querySolverClassFields2 (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;


  SgClassDefinition *sageClassDefinition = isSgClassDefinition (astNode);
 
  if (sageClassDefinition != NULL)
    {

            ROSE_ASSERT (sageClassDefinition->get_declaration () != NULL);
          SgDeclarationStatementPtrList declarationStatementPtrList =
            sageClassDefinition->get_members ();

          typedef SgDeclarationStatementPtrList::iterator LI;

                  for (LI i = declarationStatementPtrList.begin ();
                       i != declarationStatementPtrList.end (); ++i)
                    {
                      SgNode *listElement = *i;

                      if (isSgVariableDeclaration (listElement) != NULL)
                        returnNodeList.push_back (listElement);

                    }
                //}
            }

          return returnNodeList;
        }                       /* End function querySolverClassFields()*/ 



        /*
         *  The function:
         *       querySolverVariableDeclarationFromName()
         *  takes as a first parameter a SgNode*, and as a second
         *  parameter a SgNode* which is a SgName*. If the 
         *  astNode is a variable declaration it is added to
         *  the returned list<SgNode*> if it 
         *  match the name.
         */
        NodeQuerySynthesizedAttributeType
        querySolverVariableDeclarationFromName (SgNode * astNode,
                                                SgNode * variableNameNode)
        {
          SgName *variableNameObject = isSgName (variableNameNode);
          ROSE_ASSERT (variableNameObject != NULL);
          string variableName = variableNameObject->str ();

          ROSE_ASSERT (variableName.size () > 0);
          ROSE_ASSERT (astNode != 0);
          NodeQuerySynthesizedAttributeType returnNodeList;

          if (astNode->variantT () == V_SgVariableDeclaration)
            {
              SgVariableDeclaration *sageVariableDeclaration =
                isSgVariableDeclaration (astNode);
              ROSE_ASSERT (sageVariableDeclaration != NULL);

              SgInitializedNameList sageInitializedNameList =
                sageVariableDeclaration->get_variables ();

              typedef SgInitializedNameList::iterator variableIterator;

              for (variableIterator variableListElement =
                   sageInitializedNameList.begin ();

                   variableListElement != sageInitializedNameList.end ();
                   ++variableListElement)
                {
                  SgInitializedName elmVar = *variableListElement;

                  string name = elmVar.get_name ().str ();
                  ROSE_ASSERT (name.length () > 0);

                  //add astNode to list=<> if it matches the name criteria
                  //specified in SgName*.
                  if (name == variableName)
                    returnNodeList.push_back (astNode);
                }
            }
          return returnNodeList;
        }                               /* End function querySolverVariableDeclarations() */


        /*
         *   The function:
         *        queryClassNotPrinted()
         *   takes as a first parameter a list<SgNode*> of fields inside a class-like structure. As a second parameter it
         *   take a set<SgNdoe*> which specifies which class-like variables has been printed before. As a third parameter
         *   it takes the method_prefix of the generated code. As a last paramter is a fix intended for the case where
         *   you print out class-like variables inside a union. Since the union is treated like a variable inside another
         *   class-like stucture, there has to be a reference to the scope of the union when a class-like variable is
         *   accessed inside the union. The scope of this function is to genereate code to print out class-like 
         *   structures pointed to within another class-like structure. It will generate code for class-like structures
         *   if it has not been generated before.
         *
         *
         */
        list < SgNode * >GenGeneration::queryClassNotPrinted (list <
                                                              SgNode * >classFields,
                                                              set <
                                                              SgNode *
                                                              >printedClasses,
                                                              string
                                                              unionVariableName)
        {
          typedef list < SgNode * >::iterator variableIterator;
          typedef SgInitializedNameList::iterator variableNameIterator;

          list < SgNode * >classNotPrinted;
          string variablePrefix = "";

          if(classFields.empty() != true)
          for (variableIterator variableListElement = classFields.begin ();
               variableListElement != classFields.end (); ++variableListElement)
            {

              SgVariableDeclaration *
                sageVariableDeclaration =
                isSgVariableDeclaration (*variableListElement);
              ROSE_ASSERT (sageVariableDeclaration != NULL);

              SgInitializedNameList
                sageInitializedNameList = sageVariableDeclaration->get_variables ();

              
              for (variableNameIterator variableNameListElement =
                   sageInitializedNameList.begin ();
                   variableNameListElement != sageInitializedNameList.end ();
                   ++variableNameListElement)
                {
                  list<SgNode*> classDeclarationList;
                  SgInitializedName elmVar = *variableNameListElement;
                  string variableName =  elmVar.get_name().str ();

                  SgType  *variableType =  elmVar.get_type ();
                  ROSE_ASSERT (variableType != NULL);

                  string typeName = TransformationSupport::getTypeName (variableType);
                  ROSE_ASSERT (typeName.length () > 0);

                  SgNodePtrVector scopesVector =
                        findScopes (sageVariableDeclaration);
                  
                  SgTypedefDeclaration *typedefDeclaration =
                        findTypedefFromTypeName (scopesVector, typeName);

                  SgType* sageType;
                  SgClassType* classType = NULL;
                  if(typedefDeclaration != NULL){
                          if(typedefTranslationTable.find(typedefDeclaration) != typedefTranslationTable.end())
                                  sageType = typedefTranslationTable.find(typedefDeclaration)->second;
                          ROSE_ASSERT( sageType != NULL );

                          classType = isSgClassType(sageType);
                          if(classType != NULL){
                                string sageTypeName = TransformationSupport::getTypeName(sageType);
                                if( sageTypeName != "" )
                                    classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, typeName);
                                else
                                    classDeclarationList.push_back(classType->get_declaration());
                          }
                  }else
                          classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, typeName);
                  
                  if (classDeclarationList.empty() != true)
                  if (variableType->variantT () == V_SgPointerType)
                    {

                      SgClassDeclaration* sageClassDeclaration;
                      list < SgNode * >classDeclarationList;

                      if(classType != NULL){
                             //The class declaration was found in a TypedefDeclaration
                              sageClassDeclaration =
                                isSgClassDeclaration (classType->get_declaration ());
                              ROSE_ASSERT (sageClassDeclaration != NULL);
                              string className;
                              if( sageClassDeclaration->get_name().str() == NULL ){
                                      className = "";
                                      classDeclarationList.push_back(sageClassDeclaration);
                                      ROSE_ASSERT( classDeclarationList.empty() == false );
                              }else{                          
                                      className = sageClassDeclaration->get_name().str();
                                      classDeclarationList =
                                     findClassDeclarationsFromTypeName (findScopes
                                                           (sageVariableDeclaration),
                                                           className);
                              }

                      }else{
                             
                      //Find the class declaration which correspond to the typename to see if it is a union
                      classDeclarationList =
                        findClassDeclarationsFromTypeName (findScopes
                                                           (sageVariableDeclaration),
                                                           typeName);
                      //If the class corresponding to the TypeName is not found we have a problem.
                      }
                      ROSE_ASSERT (classDeclarationList.empty () == false);

                      for (list < SgNode * >::iterator classIterator =
                           classDeclarationList.begin ();
                           classIterator != classDeclarationList.end ();
                           ++classIterator)
                        {
                          sageClassDeclaration =
                            isSgClassDeclaration (*classIterator);
                          ROSE_ASSERT (sageClassDeclaration != NULL);

                          //Since unions are treated as variables exempt unions from treatment as a class.                 
                          if (sageClassDeclaration->get_class_type () !=
                              SgClassDeclaration::e_union)
                            {
                              //Handle  the printout of instances of a class inside the class. To do: unions, structs.
                              if (unionVariableName.length () > 0)
                                {
                                  (*dotout) << "if (followPointerTo" + unionVariableName + variableName + " == true )\n";       //{\n   ";
                                  variablePrefix = unionVariableName + "->";
                                }
                              else
                                (*dotout) << "if (classReference-> " + variableName + " != NULL )\n";   //{\n   ";


                              (*dotout) << "   if (" + classPointerName +
                                "->checkIfMapKeyHasBeenSaved(classReference->" +
                                variablePrefix + variableName + ") == false )\n";
                              (*dotout) << "   {\n";
                              if (sageClassDeclaration->get_class_type () ==
                                  SgClassDeclaration::e_class)
                                (*dotout) << "classReference->" + variablePrefix +
                                  variableName << "->" << method_prefix + "(" +classPointerName + ", \"" + variableName + 
                                  "\", GenRepresentation::Container ); \n";
                              else
                                {
                                  int cnt = 0;
                                  string struct_name;
                                  if(sageClassDeclaration->get_name().str() == NULL)
                                         struct_name = "";
                                  else 
                                    struct_name = sageClassDeclaration->
                                      get_mangled_qualified_name (cnt).str ();

                                  (*dotout) << method_prefix + "_" +
                                    struct_name << "(" + classPointerName + ",\"" +
                                    variableName + "\",classReference->" +
                                    variablePrefix + variableName +
                                    ", GenRepresentation::Container);\n";
                                }

                              (*dotout) << "   }\n";

                              //if class not printed add it to returnlist.
                              if (printedClasses.find (sageClassDeclaration) ==
                                  printedClasses.end ())
                                classNotPrinted.push_back (sageClassDeclaration);

                            }
                          else
                            {
                              list < SgNode * >unionVariableList =
                                NodeQuery::querySubTree (sageClassDeclaration,
                                                         NodeQuery::UnionFields);
                              //generate the code needed for printing the classes inside the union, and accept
                              //the references to the classes inside the
                              list < SgNode * >queryReturnList =
                                queryClassNotPrinted (unionVariableList,
                                                      printedClasses, variableName);
                              classNotPrinted.merge (queryReturnList);


                            }
                        }               /* End iterator over classDeclarationList */
                    }
                }                       /* End iterator over sageInitializedNameList */
            }

          return classNotPrinted;
        };                              /* End function: queryClassNotPrinted() */

        GenGeneration::GenGeneration ()
        {
          dotout = new ostringstream ();

          classPointerName = "generateRepresentation";
          nameOfConstructedClass = "StuctureSaver";
          method_prefix = "save";
          staticDataGraph = new GenRepresentation ();
        };

        GenGeneration::~GenGeneration ()
        {
          delete dotout;
          delete staticDataGraph;
        };

        void
        GenGeneration::clear ()
        {
          delete dotout;
          dotout = new ostringstream ();
        };
        void GenGeneration::graphTypedefTranslationTable(map<SgTypedefDeclaration*, SgType*> mapSubset){
                staticDataGraph->beginArray(staticDataGraph, "Typedef Translation Table Subset", GenRepresentation::Container);
                typedef map< SgTypedefDeclaration*, SgType*>::iterator mapIterator;

                for(mapIterator mapElm= mapSubset.begin(); mapElm != mapSubset.end(); ++mapElm){
                        SgTypedefDeclaration* sageTypedefDeclaration = isSgTypedefDeclaration(mapElm->first);
                
                        string name = "";

                        if(isSgClassType(mapElm->second)){
                                SgClassDeclaration* sageClassDeclaration = isSgClassDeclaration(isSgClassType(mapElm->second)->get_declaration());
                                ROSE_ASSERT( sageClassDeclaration != NULL);
                        
                                
                                if(sageClassDeclaration->get_class_type() == SgClassDeclaration::e_class)
                                        name = " class ";
                                else if(sageClassDeclaration->get_class_type() == SgClassDeclaration::e_struct)
                                        name = " struct ";
                                else
                                        name = " union ";
                        }
 
                                                
                        if (mapElm == typedefTranslationTable.begin ())
                        {
                  
                                (*dotout) << classPointerName + "->addVariable(\"typedef " + name +
                                        TransformationSupport::getTypeName(mapElm->second) + "\", \"" + sageTypedefDeclaration->get_name().str() +
                                        "\", \"\");\n";
                  //static
                  staticDataGraph->addVariable ("typedef "+name + TransformationSupport::getTypeName(mapElm->second) , sageTypedefDeclaration->get_name().str(), "");

                }
              else
                {

                  
                (*dotout) << classPointerName + "->addVariable(\"typedef " +
                        TransformationSupport::getTypeName(mapElm->second) + "\", \"" + sageTypedefDeclaration->get_name().str() +
                        "\", \"\");\n";
                //static
                staticDataGraph->addVariable ("typedef "+TransformationSupport::getTypeName(mapElm->second) , sageTypedefDeclaration->get_name().str(), "","",true);

                }

                        
                        



                }       

                staticDataGraph->endArray(GenRepresentation::Container);

        }
        void GenGeneration::graphTypedefTranslationTable(){
                

                staticDataGraph->beginArray(staticDataGraph, "Typedef Translation Table", GenRepresentation::Container);
                typedef map< SgTypedefDeclaration*, SgType*>::iterator mapIterator;

                for(mapIterator mapElm= typedefTranslationTable.begin(); mapElm != typedefTranslationTable.end(); ++mapElm){
                        SgTypedefDeclaration* sageTypedefDeclaration = isSgTypedefDeclaration(mapElm->first);
                
                        string name = "";

                        if(isSgClassType(mapElm->second)){
                                SgClassDeclaration* sageClassDeclaration = isSgClassDeclaration(isSgClassType(mapElm->second)->get_declaration());
                                ROSE_ASSERT( sageClassDeclaration != NULL);
                        
                                
                                if(sageClassDeclaration->get_class_type() == SgClassDeclaration::e_class)
                                        name = " class ";
                                else if(sageClassDeclaration->get_class_type() == SgClassDeclaration::e_struct)
                                        name = " struct ";
                                else
                                        name = " union ";
                        }
 
                                                
                        if (mapElm == typedefTranslationTable.begin ())
                        {
                  
                                (*dotout) << classPointerName + "->addVariable(\"typedef " + name +
                                        TransformationSupport::getTypeName(mapElm->second) + "\", \"" + sageTypedefDeclaration->get_name().str() +
                                        "\", \"\");\n";
                  //static
                  staticDataGraph->addVariable ("typedef "+name + TransformationSupport::getTypeName(mapElm->second) , sageTypedefDeclaration->get_name().str(), "");

                }
              else
                {

                  
                (*dotout) << classPointerName + "->addVariable(\"typedef " +
                        TransformationSupport::getTypeName(mapElm->second) + "\", \"" + sageTypedefDeclaration->get_name().str() +
                        "\", \"\");\n";
                //static
                staticDataGraph->addVariable ("typedef "+TransformationSupport::getTypeName(mapElm->second) , sageTypedefDeclaration->get_name().str(), "","",true);

                }

                        
                        



                }       

                staticDataGraph->endArray(GenRepresentation::Container);

        };

        /* 
         * The function:
         *      buildTypedefTranslationTable()
         * takes as a parameter a SgProject*. It will return a map<SgTypedefDeclaration*, SgType*>
         * where the idea is that all SgTypedefDeclaration* are unique, and therefore it is 
         * possible to create a map with it's corresponding type for easy access.  
         */
        map< SgTypedefDeclaration*, SgType*> 
        GenGeneration::buildTypedefTranslationTable(SgProject* project){
                ROSE_ASSERT (project != NULL);
                const SgFilePtrList sageFilePtrList = *project->get_fileList ();

                //Iterate over all global scopes in the all the files the project spans.
                for (unsigned int i = 0; i < sageFilePtrList.size (); i += 1)
                  {
                    const SgFile *sageFile = isSgFile (sageFilePtrList[i]);
                    ROSE_ASSERT (sageFile != NULL);
                    SgGlobal *sageGlobal = sageFile->get_root ();
                    ROSE_ASSERT (sageGlobal != NULL);

                    SgTypedefDeclaration* typedefDeclaration;
                    
                    //Find all TypedefdefDeclarations in current global scope.
                   list < SgNode * > typedefDeclarationList =
                      NodeQuery::querySubTree (sageGlobal,
                                               NodeQuery::TypedefDeclarations);

                   typedefDeclarationList.unique();

                   //Iterate over all SgTypedefDeclarations in current global scope,
                   //and find corresponding SgType*.
                   for (list < SgNode * >::iterator typedefDeclarationElm =
                         typedefDeclarationList.begin ();
                         typedefDeclarationElm != typedefDeclarationList.end ();
                         ++typedefDeclarationElm)
                      {
                        typedefDeclaration =
                          isSgTypedefDeclaration (*typedefDeclarationElm);
                        ROSE_ASSERT (typedefDeclaration != NULL);
                        //We only register a typedef once
                        ROSE_ASSERT (typedefTranslationTable.find (typedefDeclaration) == typedefTranslationTable.end ());
                       
                        SgType* typedefBaseType = typedefDeclaration->get_base_type();
                        ROSE_ASSERT(typedefBaseType != NULL );
                        SgType* baseType  = typedefBaseType->findBaseType(); 
                       
                        
                        //If the SgTypedefDeclarations has a base type which is of SgTypedefType, find a
                        //SgType* which is not of type SgTypedefType. That is the corresponging SgType*.        
                        while(isSgTypedefType(baseType) != NULL) 
                                baseType = isSgTypedefType(baseType)->get_base_type();
        /*              cout << "The name of the typedef is:" << typedefDeclaration->get_name().str() ;
                        string baseName = TransformationSupport::getTypeName(baseType);

                        cout << "  The correpsonding basetype is:" << baseName << endl;

                        if(isSgClassType(baseType) == NULL)
                                cout << "It is NOT a CLASS TYPE." << endl;
        */              
                        ROSE_ASSERT( baseType != NULL );
                        typedefTranslationTable[typedefDeclaration] = baseType;

                      }
                  }

        return typedefTranslationTable;


        } /* End method: buildTypedefTranslationTable */        



        GenRepresentation* GenGeneration::getPtrStaticGenRepresentation(){
                return staticDataGraph;
        }
        /*
         *  The function
         *       printClass()
         *  takes as a first parameter a SgNode*. If that SgNode* is a SgClassDeclaration it will
         *  print it out. As a second parameter it takes a label denoting and as a third option
         *  an option (color etc). The scope of the method is to generate code to graph
         *  class-like strucures, excluding unions which is treated like a variable..
         *  
         */
        void
        GenGeneration::printClass (SgNode * node, string nodelabel, string option)
        {

          string name = "";
          string structGraphClassName = "GenStructGraph";

          SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (node);


          ROSE_ASSERT (sageClassDeclaration != NULL);
          if(sageClassDeclaration->get_definition()->get_declaration() != sageClassDeclaration)
                  cout << "The printClass" << endl;
          else
          if (printedClasses.find (node) == printedClasses.end ())
          {  
              if(sageClassDeclaration->get_name().str() == NULL)
                      name = "";
              else
                  name = sageClassDeclaration->get_name ().str ();

              if (sageClassDeclaration->get_class_type () ==
                  SgClassDeclaration::e_class)
                {
                  //generate code for starting the graphing method for a class
                  (*dotout) << "\nvoid " << name + "::" +
                    method_prefix << "( GenRepresentation* " + classPointerName +
                    ", string className, GenRepresentation::TypeOfPrint printType = GenRepresentation::Container){\n";
                  (*dotout) << name + "* classReference = this;\n";
                }
              else if (sageClassDeclaration->get_class_type () ==
                       SgClassDeclaration::e_struct)
                {
                  //generate code for starting the graphing method for a struct
                  int cnt = 0;
                  string struct_name, struct_mangled_name;
                  if(sageClassDeclaration->get_name().str() == NULL){
                          struct_mangled_name = "";
                          struct_name = "";
                  }else{
                          struct_mangled_name = sageClassDeclaration->get_mangled_qualified_name (cnt).str ();
                          struct_name = sageClassDeclaration->get_name().str();
                  }
                  (*dotout) << "\nvoid " << method_prefix + "_" +
                    struct_mangled_name << "( GenRepresentation* " + classPointerName +
                    ", string className," + struct_name +
                    "* classReference, GenRepresentation::TypeOfPrint printType = GenRepresentation::Container){\n";
                }
              else
                {
                  cerr << "Defined fair use of unions not fulfilled. Terminating.\n";
                    
                  exit (1);
                }

              printedClasses.insert (node);
              list < SgNode * >variableList, tempVariableList;
              list < SgClassDeclaration * >sageClassDeclarationList;


              sageClassDeclarationList.push_back (sageClassDeclaration);
              //find all classes which the current class inherit from 
              SgClassDefinition *classDefinition =
                isSgClassDefinition (sageClassDeclaration->get_definition ());
              ROSE_ASSERT (classDefinition != NULL);

              SgBaseClassList baseClassList = classDefinition->get_inheritances ();

              typedef SgBaseClassList::iterator SgBaseClassListIterator;
              if(baseClassList.empty() != true);
              for (SgBaseClassListIterator baseClassElm = baseClassList.begin ();
                   baseClassElm != baseClassList.end (); ++baseClassElm)
                {

                  SgBaseClass baseClass = *baseClassElm;
                  sageClassDeclarationList.push_back (baseClass.get_base_class ());
                }
              //Make a joint list of the variables from the current class from the Class Fields of the current class
              //and all classes it inherit from. 
              for (list < SgClassDeclaration * >::iterator classDeclarationElm =
                   sageClassDeclarationList.begin ();
                   classDeclarationElm != sageClassDeclarationList.end ();
                   ++classDeclarationElm)
                {
                  SgClassDeclaration* sageClassElm = isSgClassDeclaration(*classDeclarationElm);
                  /*cout << "The name of the class in NodeQuery is:" << TransformationSupport::getTypeName(sageClassElm->get_type()) << endl;
                  cout << "The file name is" << sageClassElm->getFileName()<< endl; */
                  ROSE_ASSERT( sageClassElm != NULL );
                  ROSE_ASSERT( sageClassDeclaration != NULL );
                  //querySolverClassFields2(sageClassElm->get_definition());
                  if (sageClassDeclaration->get_class_type () ==
                      SgClassDeclaration::e_class){
                          tempVariableList = querySolverClassFields2(sageClassDeclaration->get_definition());
                /*        tempVariableList =
                                NodeQuery::querySubTree (sageClassElm->get_definition()->get_parent(),
                                               NodeQuery::ClassFields, NodeQuery::ChildrenOnly);*/
                  }else if (sageClassDeclaration->get_class_type () ==
                           SgClassDeclaration::e_struct){
                    tempVariableList = querySolverClassFields2(sageClassDeclaration->get_definition());
/*                  tempVariableList = 
                      NodeQuery::querySubTree (sageClassElm->get_definition()->get_parent(),
                                               NodeQuery::StructFields, NodeQuery::ChildrenOnly);*/
                  }else{
                          cout << "The union is interpreted as a variable. Not class. Terminating." << endl;
                          exit(1);
                  }
                  variableList.merge (tempVariableList);

                }
              if(variableList.empty() == true){
                      cout << "variableList is empty" << endl;
                //      variableList = querySolverClassFields2(sageClassDeclaration->get_definition());//NodeQuery::querySubTree(sageClassDeclaration->get_definition()->get_parent(), NodeQuery::StructFields, NodeQuery::ChildrenOnly);
              cout << "The class name is: " << TransformationSupport::getTypeName(sageClassDeclaration->get_type()) << endl;
              cout << "The file name  is: " << sageClassDeclaration->get_file_info()->get_filename() << endl;
              cout << "The line number is " <<  sageClassDeclaration->get_file_info()->get_line() << endl;
              }
             // ROSE_ASSERT( variableList.empty() != true);
              variableList.unique ();



              //Generate code which keeps track of which pointers has been followed before or not.
              (*dotout) << classPointerName << "->setMapKeySaved(classReference);";
              (*dotout) << "if( " + classPointerName +
                "->checkIfMapKeyExist(classReference) == false)\n";
              (*dotout) << "  " + classPointerName + "->setMapKey(classReference);\n";

              //Generate static dot representation
              staticDataGraph->setMapKeySaved (node);
              if (staticDataGraph->checkIfMapKeyExist (node) == false)
                staticDataGraph->setMapKey (node);

              //Generate code which begins the class

              string inheritance = "";
              if (baseClassList.empty () != true)
                {
                  inheritance =
                    " : " +
                    TransformationSupport::getTypeName ((*baseClassList.begin ()).
                                                        get_base_class ()->
                                                        get_type ()->findBaseType ());
                }

              if (sageClassDeclaration->get_class_type () ==
                  SgClassDeclaration::e_class)
                {
                  //dynamic
                  (*dotout) << classPointerName << "->beginClass(classReference, \" "
                    << name << " \"+className, printType);\n";
                  //static
                  staticDataGraph->beginClass (node, name + inheritance,
                                               GenRepresentation::Container);

                }
              else if (sageClassDeclaration->get_class_type () ==
                       SgClassDeclaration::e_struct)
                {
                  //dynamic
                  (*dotout) << classPointerName << "->beginStruct(classReference, \""
                    << name << " \"+className, printType);\n";
                  //static
                  staticDataGraph->beginStruct (node, name + inheritance,
                                                GenRepresentation::Container);

                }
              variableList.unique ();

              //Generate code which graphs the contents of the class and the ends it.
              if (sageClassDeclaration->get_class_type () ==
                  SgClassDeclaration::e_class)
                {
                  printCSVariables (variableList, name, sageClassDeclaration);
          //dynamic
          (*dotout) << classPointerName << "->endClass( printType );\n";
          //static
          staticDataGraph->endClass (GenRepresentation::Container);
        }
      else if (sageClassDeclaration->get_class_type () ==
               SgClassDeclaration::e_struct)
        {
          printCSVariables (variableList, name, sageClassDeclaration);
          (*dotout) << classPointerName << "->endStruct( printType );\n";
          //static
          staticDataGraph->endStruct (GenRepresentation::Container);

        }

      //Generate code for graphing class-like structures referenced to within the scope of this
      //class-like structure if that has not been done before and then end the current method.
      //It also generates code for class-like structures referenced to when needed.
      list < SgNode * >classNotPrinted =
        queryClassNotPrinted (variableList, printedClasses);

      if (sageClassDeclaration->get_class_type () ==
          SgClassDeclaration::e_class)
        (*dotout) << "\n}" << "/* End method: " << name + "::" +
          method_prefix << " */ \n";
      else if (sageClassDeclaration->get_class_type () ==
               SgClassDeclaration::e_struct)
        (*dotout) << "\n}" << "/* End method: " << method_prefix + "_" +
          name << " */ \n";

      //Generate code for printing the class-like strucures referenced in the current class-like structure
      //which has not been printed yet.
      typedef list < SgNode * >::iterator variableIterator;

      if(classNotPrinted.empty() != true) 
      for (variableIterator variableListElement = classNotPrinted.begin ();
           variableListElement != classNotPrinted.end ();
           ++variableListElement)
        {
          SgNode *element = isSgNode (*variableListElement);
          ROSE_ASSERT (element != NULL);

          printClass (element, "", "");

        }



    }                           /* End if(current class not printed) */
};                              /* End function: printClass() */



/*
 *  The function:
 *        printUnionVariables()
 *  takes as a first parameter a list of the fields of the union, as a second parameter is the name.
 *  The scope of this method is to generate the code needed for antoher class-like structure to
 *  print out the variables inside the union. The idea is that the union is treated like a
 *  variable and if no control-variable which defines what variable is valid exists, it will not
 *  inspect the values because of ambiguity.
 *
 */
void
GenGeneration::printUnionVariables (list < SgNode * >unionFields,
                                    list < SgNode * >classFields,
                                    list <
                                    ControlStructureContainer * >pragmaList,
                                    string unionVariableName)
{
  ROSE_ASSERT (unionVariableName.length () > 0);
  string unionControlVariableName, variableControl;

  cout << "In printUnionVariables" << endl;
  //iterate over all variables inside the union
  if(unionFields.empty() != true); 
  for (list < SgNode * >::iterator variableListElement = unionFields.begin ();
       variableListElement != unionFields.end (); ++variableListElement)
    {
      SgVariableDeclaration *sageVariableDeclaration =
        isSgVariableDeclaration (*variableListElement);
      ROSE_ASSERT (sageVariableDeclaration != NULL);

      SgInitializedNameList sageInitializedNameList =
        sageVariableDeclaration->get_variables ();

      
      for (SgInitializedNameList::iterator variableNameListElement =
           sageInitializedNameList.begin ();
           variableNameListElement != sageInitializedNameList.end ();
           ++variableNameListElement)
        {

          variableControl = "";

          SgInitializedName elmVar = *variableNameListElement;
 
          string variableName = elmVar.get_name ().str ();
          ROSE_ASSERT (variableName.length () > 0);

          SgType *variableType = elmVar.get_type ();
          ROSE_ASSERT (variableType != NULL);

          string typeName = TransformationSupport::getTypeName (variableType);
          ROSE_ASSERT (typeName.length () > 0);
          //find the control statement for the variable if it exist. 
          for (list < ControlStructureContainer * >::iterator i =
               pragmaList.begin (); i != pragmaList.end (); ++i)
            {
              variableControl = "";
              ControlStructureContainer *controlStructure = *i;
              ROSE_ASSERT (controlStructure != NULL);
              //get the variable which corresponds to the variable name in the control structure
              string controlString =
                parsePragmaStringRHS (controlStructure->getPragmaString (),
                                      "GenUnionControl_", variableName);

              //if the control variable exist set the name variableControl to that name
              if (controlString.length () > 0)
                {
                  variableControl = controlString;
                  break;
                }
            }
/* AS(011904) removed to allow removal of ostringstream from GenRepresentation interface
          (*dotout) <<
            "delete variableValue;\n variableValue = new ostringstream();\n";
*/
          if (variableControl.length () < 1)
            {
              //the case where we do not have a control statement for the variable. Do not make sence to
              //access value since it is impossible to know its validity.

              //AS(01/19/04) removed to allow removal of ostringstream from GenRepresentation interface
              //(*dotout) << "(*variableValue) << \"CONTROL NOT DEFINED\";";

              //Handle the printout of variables declared inside the header of the class
              if ((variableListElement == unionFields.begin ())
                  && (variableNameListElement ==
                      sageInitializedNameList.begin ()))
                {
                  (*dotout) << classPointerName + "->addVariable(\"" +
                    typeName + "\", \"" + variableName +
                    "\", \"CONTROL NOT DEFINED\");\n";
                  //static
                  staticDataGraph->addVariable (typeName, variableName, "");

                }
              else
                {
                  (*dotout) << classPointerName + "->addVariable(\"" +
                    typeName + "\", \"" + variableName +
                    "\", \"CONTROL NOT DEFINED\", \"\", true );\n";
                  //static
                  staticDataGraph->addVariable (typeName, variableName, "",
                                                "", true);

                }
            }
          else
            {
            //The case where a control-variable is defined
            GenRepresentation::TypeOfPrint typeOfPrint = GenRepresentation::Container;
 
            //When the variable-types can be traversed this is the correct code. The
            //reason for this is that it is not possible to find out if it is a
            //SgReference or not.
            //For now a hack has to be implemented.

            //    switch( variableType->variantT ())
            // {
            //  case V_SgReferenceType:
            //    cout << "Should be statically graphing a reference here" << endl;
            //    typeOfPrint = GenRepresentation::Contained;
            //  case V_SgPointerType:
            //  {
           


             //BEGIN HACK
             list<SgNode*> classDeclarationList;        
             //The initial state is that it is not a class. We want to find out if this leads
             //to a class.
             SgNodePtrVector scopesVector =
                findScopes (sageVariableDeclaration);
             SgTypedefDeclaration *typedefDeclaration =
                findTypedefFromTypeName (scopesVector, typeName);
             //Makes sure that this is actually a class-typedef
  
             SgType* sageType=variableType;
             SgClassType* classType = NULL;
             if(typedefDeclaration != NULL){

              if(typedefTranslationTable.find(typedefDeclaration) != typedefTranslationTable.end())
                      sageType = typedefTranslationTable.find(typedefDeclaration)->second;


              ROSE_ASSERT(sageType != NULL);
              classType = isSgClassType(sageType);
             if(classType != NULL){
                  string sageTypeName = TransformationSupport::getTypeName(sageType);
                  if( sageTypeName != "" )
                      classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, sageTypeName);
                  else{
                      ROSE_ASSERT( classType->get_declaration() != NULL );
                      classDeclarationList.push_back(classType->get_declaration());
                  }
                    
              }
             }else
                 classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, typeName);
             
              classDeclarationList.unique();
              
              //the case where we have a control statement for this variable.
              int hackType = 0; //This is the default case
              if( variableType->variantT () == V_SgPointerType)
                   hackType = 2; //V_SgPointerType
              else if(classDeclarationList.empty() != true)
                   hackType = 1; //V_SgReferenceType

              switch( hackType)
                {
               case 1: // V_SgReferenceType
                     typeOfPrint = GenRepresentation::Contained;
               case 2: //V_SgPointerType
                {
                //END HACK 
             
        
                //The concept of a ClassDeclaration list is used to see if the variable
                //is of type union, class or struct. When the island in variables is 
                //fixed, just use isSgClassType(variableType)->get_class_type() and
                //remove this unnecessary complexity. This is actually needed if you
                //want the SgReferenceType to work well in the static representation.
              
               

                //The initial state is that it is not a class. We want to find out if this leads
                //to a class. Commented out until the types of the SgVariableDeclaration can be
                //traversed.

                
                  // AS(020104) REMOVED. Put in
                  //list<SgNode*> classDeclarationList; 
                 //The initial state is that it is not a class. We want to find out if this leads
                  //to a class.
                  /*              SgNodePtrVector scopesVector =
                    findScopes (sageVariableDeclaration);
                  SgTypedefDeclaration *typedefDeclaration =
                    findTypedefFromTypeName (scopesVector, typeName);
                  //Makes sure that this is actually a class-typedef
  
                  SgType* sageType=variableType;
                  SgClassType* classType = NULL;
                  if(typedefDeclaration != NULL){

                          if(typedefTranslationTable.find(typedefDeclaration) != typedefTranslationTable.end())
                                 sageType = typedefTranslationTable.find(typedefDeclaration)->second;


                          ROSE_ASSERT(sageType != NULL);
                          classType = isSgClassType(sageType);
                          if(classType != NULL){
                                  string sageTypeName = TransformationSupport::getTypeName(sageType);
                                  if( sageTypeName != "" )
                                          classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, sageTypeName);
                                  else{
                                          ROSE_ASSERT( classType->get_declaration() != NULL );
                                          classDeclarationList.push_back(classType->get_declaration());
                                  }
                          }
                  }else
                          classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, typeName);

                  classDeclarationList.unique();
                  */

              switch(variableType->variantT()){
                      case V_SgClassType:
                              break;
                      case V_SgComplex:
                      case V_SgTypeBool:
                      case V_SgEnumType:
                      case V_SgTypeChar:
                      case V_SgTypeVoid:
                      case V_SgTypeInt:
                      case V_SgTypeDouble:
                      case V_SgTypeFloat:
                      case V_SgTypeLong:
                      case V_SgTypeLongDouble:
                      case V_SgTypeEllipse:
                      case V_SgTypeGlobalVoid:
                      case V_SgTypeLongLong:
                      case V_SgTypeShort:
                      case V_SgTypeSignedChar:
                      case V_SgTypeSignedInt:
                      case V_SgTypeSignedLong:
                      case V_SgTypeSignedShort:
                      case V_SgTypeString:
                      case V_SgTypeUnknown:
                      case V_SgTypeUnsignedChar:
                      case V_SgTypeUnsignedInt:
                      case V_SgTypeUnsignedLong:
                      case V_SgTypeUnsignedShort:
                      case V_SgTypeUnsignedLongLong:
                      case V_SgReferenceType: 
                      case V_SgPointerType:
                      case V_SgModifierType:
                      case V_SgNamedType:
                      case V_SgTypedefType:
                      case V_SgPointerMemberType:
                      case V_SgArrayType: 
                      case V_SgFunctionType:
                      case V_SgMemberFunctionType:
                      case V_SgTypeWchar:
                      case V_SgTypeDefault:
                              if ((variableListElement == unionFields.begin ())
                                              && (variableNameListElement ==
                                                      sageInitializedNameList.begin ()))
                              {
                                      (*dotout) << classPointerName + "->addVariable(\"" +
                                              typeName + "\", \"" + variableName +
                                              "\", \"not implemented\");\n";
                                      //static
                                      staticDataGraph->addVariable (typeName, variableName,
                                                      " ");
                              }
                              else
                              {
                                      (*dotout) << classPointerName + "->addVariable(\"" +
                                              typeName + "\", \"" + variableName +
                                              "\",\"not implemented\""+ 
                                               ", \"\", true );\n";
                                      //static
                                      staticDataGraph->addVariable (typeName, variableName,
                                                      " ", "", true);
                              }

                              break;
                      default:
                    /*        printf ("default reached in switch within TransformationSupport::getTypeName type->sage_class_name() = %s variant = %d \n",
                                              sageType->sage_class_name(),sageType->variant());*/
                              ROSE_ABORT();
                              break;

              }


              cout << "BEFORE classdeclarationlist.empty" << endl;
                  if (classDeclarationList.empty() != true)
                    {
                      //Handle  the printout of instances of a class inside the class. To do: unions, structs.


                      SgClassDeclaration* sageClassDeclaration = isSgClassDeclaration(*classDeclarationList.begin());
                      ROSE_ASSERT( sageClassDeclaration != NULL );
                   

                      //The case where the variable is a reference
                      //(*dotout) << "if ( (" + unionControlVariableName + " == " + variableControl + ") & (" + unionVariable                        Name + "->" + variableName + " != NULL ) ){\n";        //{\n   ";
                      //AS(011504) small fix make it so that pointer to graphing of class-like structure inside union will 
                      //not be followed unless the following criteria is fulfilled. Store result in a variable.
                      (*dotout) << "bool followPointerTo" +
                        unionVariableName + variableName +
                        " = false; // as default do not follow pointer\n";
                      (*dotout) << checkPragmaRHSUnionControl (unionFields,
                                                               classFields,
                                                               variableControl)
                        << "{\n";
                      (*dotout) << "if(classReference->" + unionVariableName +
                        "->" + variableName + " != NULL )\n";
                      GC_CONDITIONAL_PRINT (unionVariableName + "->" +
                                            variableName);
                      (*dotout) << "{\n followPointerTo" + unionVariableName +
                        variableName + " = true; \n";
                      
                      staticDataGraph->
                        increaseMapKeyNumber (sageVariableDeclaration);
                      
                      if(typeOfPrint == GenRepresentation::Contained){
                        cout << "IN CONTAINED" << endl;
                       (*dotout) << classPointerName +
                        "->increaseMapKeyNumber(&classReference->" +
                        variableName + ");\n";
                    

                       if(sageClassDeclaration->get_class_type() ==
                          SgClassDeclaration::e_class){
                           (*dotout) << "classReference->" + 
                          variableName << "->" << method_prefix + "(" +classPointerName + ", \"" + variableName + 
                          "\", GenRepresentation::Contained ); \n";
                           //static
                         if (staticDataGraph->checkIfMapKeyExist (sageClassDeclaration) == false)
                                    staticDataGraph->setMapKey (sageClassDeclaration); 
                          staticDataGraph->beginClass(sageClassDeclaration,TransformationSupport::getTypeName(sageClassDeclaration->get_type())  , typeOfPrint);
                          printCSVariables(querySolverClassFields2(sageClassDeclaration->get_definition()),
                                               TransformationSupport::getTypeName(sageClassDeclaration->get_type()), sageClassDeclaration);
                          staticDataGraph->endClass(typeOfPrint);
                       }else{
                          //static
                          if (staticDataGraph->checkIfMapKeyExist (sageClassDeclaration) == false)
                                    staticDataGraph->setMapKey (sageClassDeclaration);
                          staticDataGraph->beginStruct(sageClassDeclaration,TransformationSupport::getTypeName(sageClassDeclaration->get_type()), typeOfPrint);
                          printCSVariables(querySolverClassFields2(sageClassDeclaration->get_definition()),
                          TransformationSupport::getTypeName(sageClassDeclaration->get_type()), sageClassDeclaration);
                          staticDataGraph->endStruct(typeOfPrint);
                       }

                      if ((variableListElement == unionFields.begin ())
                          && (variableNameListElement ==
                              sageInitializedNameList.begin ()))
                        {


                          (*dotout) << classPointerName + "->addVariable(\"" +
                            typeName + "\", \"" + variableName +
                            "\",&classReference->" + unionVariableName + "->" +
                            variableName + ") ); \n";
        
                          (*dotout) << "}else{\n"        
                            << classPointerName + "->addVariable(\"" + typeName +
                            "\", \"" + variableName + " \", \"NULL\"); \n}\n";
                          //static
                          //static
                          staticDataGraph->addVariable (typeName,
                                                        variableName, "");

                        }
                      else
                        {
                          (*dotout) << classPointerName + "->addVariable(\"" +
                            typeName + "\", \"" + variableName +
                            "\", &classReference->" + unionVariableName +
                            "->" + variableName + ",\"\", true ); \n";

                          (*dotout) << "}else{\n"        
                            << classPointerName + "->addVariable(\"" + typeName +
                            "\", \"" + variableName +
                            " \", \"NULL\",\"\", true ); \n}\n";
                          //static
                          staticDataGraph->addVariable (typeName,
                                                        variableName, "", "",
                                                        true);

                        }
                      
                      (*dotout) << "\n}else{\n";
                      //(*dotout) << "(*variableValue) << \" not defined \";\n " <<
                      classPointerName + "->addVariable(\"" + typeName +
                        "\", \"" + variableName +
                        " \", \"not defined\",\"\", true ); ";
                      (*dotout) << "\n}\n";
                     /* End if(classDeclarationList.empty() != true */


                      }else if(typeOfPrint == GenRepresentation::Container){
                        cout << "IN CONTAINER" << endl;
                      //The case where the variable is a pointer
                     (*dotout) << classPointerName +
                        "->increaseMapKeyNumber(classReference->" +
                        variableName + ");\n";
                      (*dotout) << classPointerName + "->addEdge(" +
                        "classReference->" + unionVariableName + "->" +
                        variableName + "," + "classReference->" +
                        unionVariableName + "->" + variableName +
                        ",classReference);\n";
                      

                       if ((variableListElement == unionFields.begin ())
                          && (variableNameListElement ==
                              sageInitializedNameList.begin ()))
                        {
                          (*dotout) << classPointerName + "->addVariable(\"" +
                            typeName + "\", \"" + variableName +
                            "\",classReference->" + unionVariableName + "->" +
                            variableName + ") ); \n";
        
                          (*dotout) << "}else{\n"        
                            << classPointerName + "->addVariable(\"" + typeName +
                            "\", \"" + variableName + " \", \"NULL\"); \n}\n";
                          //static
                          //static
                          staticDataGraph->addVariable (typeName,
                                                        variableName, "");

                        }
                      else
                        {
                          (*dotout) << classPointerName + "->addVariable(\"" +
                            typeName + "\", \"" + variableName +
                            "\", classReference->" + unionVariableName +
                            "->" + variableName + ",\"\", true ); \n";

                          (*dotout) << "}else{\n"        
                            << classPointerName + "->addVariable(\"" + typeName +
                            "\", \"" + variableName +
                            " \", \"NULL\",\"\", true ); \n}\n";
                          //static
                          staticDataGraph->addVariable (typeName,
                                                        variableName, "", "",
                                                        true);

                        }
                      
                      (*dotout) << "\n}else{\n";
                      //(*dotout) << "(*variableValue) << \" not defined \";\n " <<
                      classPointerName + "->addVariable(\"" + typeName +
                        "\", \"" + variableName +
                        " \", \"not defined\",\"\", true ); ";
                      (*dotout) << "\n}\n";
                    }/* End if(printType == GenRepresentation::Container */

                    } /* End if(classDeclarationList.empty() != true */

                    
                  break;
                } /* End case V_SgPointerType */

                default:
                  {
                  //Handle the printout of variables declared inside the header of the class
                  if ((variableListElement == unionFields.begin ())
                      && (variableNameListElement ==
                          sageInitializedNameList.begin ()))
                    {
                      (*dotout) << classPointerName + "->addVariable(\"" +
                        typeName + "\", \"" + variableName +
                        "\", classReference->" + unionVariableName + "->" +
                        variableName + ");\n";
                      //static
                      staticDataGraph->addVariable (typeName, variableName,
                                                    "");
                    }
                  else
                    {
                      (*dotout) << classPointerName + "->addVariable(\"" +
                        typeName + "\", \"" + variableName +
                        "\", classReference->" + unionVariableName + "->" +
                        variableName + ", \"\", true );\n";
                      //static
                      staticDataGraph->addVariable (typeName, variableName,
                                                    "", "", true);
                    }
                 
 
                  break;
                  } /* End case default */

                } /* End switch(hackType) */
            } /* else */

        }/* End iteration over variableNameList */
    }  /* End iteration over unionFields */
}                               /* End function: printUnionVariables() */


/*
 *  The function
 *      printClassNotPrinted()
 *  takes as a first parameter a list<SgNode*>. This list has to be a list og SgClassDeclarations
 *  if not the program will ASSERT to exit. SECOND PARAMETER SUPERFLUOUS? As a third parameter it
 *  takes a string which is the method prefix of the generated save method for a class. This method
 *  is supposed to support the printVariables() in printing generate code which prints a class if
 *  it has not been printed before.
 *  
 */

void
GenGeneration::printClassNotPrinted (list < SgNode * >classNotPrinted,
                                     string name)
{
  string variableName;
  typedef list < SgNode * >::iterator variableIterator;

  for (variableIterator variableListElement = classNotPrinted.begin ();
       variableListElement != classNotPrinted.end (); ++variableListElement)
    {

      SgClassDeclaration *sageClassDeclaration =
        isSgClassDeclaration (*variableListElement);
      ROSE_ASSERT (sageClassDeclaration != NULL);
      if( sageClassDeclaration->get_name ().str() == NULL )
              variableName = "";
      else
      variableName = sageClassDeclaration->get_name ().str ();

      //Handle  the printout of instances of a class inside the class. To do: unions, structs.

      (*dotout) << "if (classReference-> " + variableName + " != NULL )\n";     //{\n   ";
      (*dotout) << "   if ( " + classPointerName +
        "->checkIfMapKeyExist(classReference->" + variableName +
        ") == false )\n";
      (*dotout) << "   {\n";
      (*dotout) << "   " << classPointerName << "->setMapKey(classReference->"
        << variableName << "," << "classReference" << ");\n";
      if (sageClassDeclaration->get_class_type () ==
          SgClassDeclaration::e_class)
        (*dotout) << "classReference->" +
          variableName << "->" << method_prefix + "(" + classPointerName +
          ",  GenRepresentation::Container ); \n";
      else
        {
          int cnt = 0;
          string struct_name;
          if( sageClassDeclaration->get_name().str() == NULL )
                  struct_name = "";
          else
            struct_name = sageClassDeclaration->get_mangled_qualified_name (cnt).str ();
          (*dotout) << method_prefix + "_" + struct_name << "(" +
            classPointerName + ",classReference->" + variableName +
            ", GenRepresentation::TypeOfPrint printType = GenRepresentation::Container);\n";

          (*dotout) << "   }\n";
        }

    }
};                              /* End function: printClassNotPrinted */

/*
 * The function
 *     printVariables()
 * takes as a first parameter a list<SgNode*> which is the fields of a class-like structure. As a second parameter it takes the name
 * of the class-like strucure. As a third parameter it takes the methor_prefix of the generated code. As a fourth parameter it takes
 * the class declaration of the class-like structure. The scope of the funciton is to generate code to graph a class-like
 * structure.
 * 
 */

void
GenGeneration::printCSVariables (list < SgNode * >classFields, string name,
                                 SgClassDeclaration * sageClassDeclaration)
{
  string unionControlVariableName;
  string variableToPrintName = "";
  SgClassDeclaration::class_types class_type;
  cout << "The class name is: " << TransformationSupport::getTypeName(sageClassDeclaration->get_type()) << endl;
  cout << "The file name  is: " << sageClassDeclaration->get_file_info()->get_filename() << endl;
  cout << "The line number is " <<  sageClassDeclaration->get_file_info()->get_line() << endl;

  
  //Define which type of class-like structure is dealt with here.
  if (sageClassDeclaration->get_class_type () == SgClassDeclaration::e_class)
    class_type = SgClassDeclaration::e_class;
  else if (sageClassDeclaration->get_class_type () ==
           SgClassDeclaration::e_struct)
    class_type = SgClassDeclaration::e_struct;
  else
    {
      cerr << "This method is not made to handle unions. Terminating.\n";
      exit (1);
    }
  //If there is any unions in the scope of this class see if there are any controlvariable defined  
  SgClassDefinition *sageClassDefinition =
    sageClassDeclaration->get_definition ();
  ROSE_ASSERT (sageClassDefinition != NULL);
  list < ControlStructureContainer * >pragmaList =
    queryFindCommentsInScope ("//pragma", "GenUnionControl_",
                              sageClassDefinition);

  //generate code to graph the variables of the class-like structure
  //removed (01/19/04) to allow for removal of ostringstream from GenRepresentation interface
  //(*dotout) << "ostringstream* variableValue = new ostringstream();\n";
  typedef list < SgNode * >::iterator variableIterator;
  if(classFields.empty() == true){
          cout << "There is no Class Fields in class:" << name << endl;
          
  }else
  for (variableIterator variableListElement = classFields.begin ();
       variableListElement != classFields.end (); ++variableListElement)
    {

      SgVariableDeclaration *sageVariableDeclaration =
        isSgVariableDeclaration (*variableListElement);
      ROSE_ASSERT (sageVariableDeclaration != NULL);

      SgInitializedNameList sageInitializedNameList =
        sageVariableDeclaration->get_variables ();

      typedef SgInitializedNameList::iterator variableNameIterator;

      for (variableNameIterator variableNameListElement =
           sageInitializedNameList.begin ();
           variableNameListElement != sageInitializedNameList.end ();
           ++variableNameListElement)
        {
 
          SgInitializedName elmVar = *variableNameListElement;
          string variableName = elmVar.get_name ().str ();


          //AS (01/26/04)
          //ROSE_ASSERT (name.length () > 0);

          SgType *variableType = elmVar.get_type ();
          ROSE_ASSERT (variableType != NULL);

          string typeName = TransformationSupport::getTypeName (variableType);
          ROSE_ASSERT (typeName.length () > 0);
          //(*dotout) <<
          // "delete variableValue;\n variableValue = new ostringstream();\n";
      /*      case V_SgReferenceType: 
                 {
                 ROSE_ASSERT ( isSgReferenceType(type)->get_base_type() != NULL );
                 typeName = getTypeName(isSgReferenceType(type)->get_base_type());
                 break;
                 }
                 case V_SgPointerType:
                 {
                 ROSE_ASSERT ( isSgPointerType(type)->get_base_type() != NULL );
                 typeName = getTypeName(isSgPointerType(type)->get_base_type());
                 break;
                 }
                 case V_SgModifierType:
                 {
                 ROSE_ASSERT ( isSgModifierType(type)->get_base_type() != NULL );
                 typeName = getTypeName(isSgModifierType(type)->get_base_type());
                 break;
                 }
                 case V_SgArrayType: 
                 {
                 ROSE_ASSERT ( isSgArrayType(type)->get_base_type() != NULL );
                 typeName = getTypeName(isSgArrayType(type)->get_base_type());
                 break;
                 } */

          //Handle graphing of different types of variables
          GenRepresentation::TypeOfPrint typeOfPrint = GenRepresentation::Container;
          //When the variable-types can be traversed this is the correct code. The
          //reason for this is that it is not possible to find out if it is a
          //SgReference or not.
          //For now a hack has to be implemented.

          //      switch( variableType->variantT ())
          // {
          //  case V_SgReferenceType:
          //    cout << "Should be statically graphing a reference here" << endl;
          //    typeOfPrint = GenRepresentation::Contained;
          //  case V_SgPointerType:
          //  {
           
          //begin hack 
          int hackType = 0; //This is my default value.
          list<SgNode*> classDeclarationList;
          
          SgNodePtrVector scopesVector =
              findScopes (sageVariableDeclaration);
          SgTypedefDeclaration *typedefDeclaration =
              findTypedefFromTypeName (scopesVector, typeName);
          //Makes sure that this is actually a class-typedef
          SgType* sageType = variableType;
          SgClassType* classType = NULL;
          if(typedefDeclaration != NULL){
                if(typedefTranslationTable.find(typedefDeclaration) != typedefTranslationTable.end())
                      sageType = typedefTranslationTable.find(typedefDeclaration)->second;

                ROSE_ASSERT( sageType != NULL );
                classType = isSgClassType(sageType);
                if(classType != NULL)
                    classDeclarationList.push_back(classType->get_declaration());
                  
          }else{
                classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, typeName);
              
                if( classDeclarationList.empty() == true )
                    if( isSgClassType(variableType) ){
                              
                      SgClassDeclaration* clDecl = isSgClassDeclaration(isSgClassType(variableType)->get_declaration());
                      ROSE_ASSERT( clDecl != NULL );
                      classDeclarationList.push_back(clDecl);
                   }
              
          }
                  
          if( variableType->variantT () == V_SgPointerType)
            hackType = 2; //V_SgPointerType
          else if(classDeclarationList.empty() != true)
            hackType = 1; //V_SgReferenceType

           switch( hackType)
            {
            case 1: // V_SgReferenceType
                typeOfPrint = GenRepresentation::Contained;
            case 2: //V_SgPointerType
              { 
                //end hack
              //The concept of a ClassDeclaration list is used to see if the variable
              //is of type union, class or struct. When the island in variables is 
              //fixed, just use isSgClassType(variableType)->get_class_type() and
              //remove this unnecessary complexity. This is actually needed if you
              //want the SgReferenceType to work well in the static representation.
              
               

              //The initial state is that it is not a class. We want to find out if this leads
              //to a class. Commented out until the types of the SgVariableDeclaration can be
              //traversed.
              /*
               list<SgNode*> classDeclarationList;
               SgNodePtrVector scopesVector =
                findScopes (sageVariableDeclaration);
              SgTypedefDeclaration *typedefDeclaration =
                findTypedefFromTypeName (scopesVector, typeName);
              //Makes sure that this is actually a class-typedef
              SgType* sageType = variableType;
              SgClassType* classType = NULL;
              if(typedefDeclaration != NULL){
                  if(typedefTranslationTable.find(typedefDeclaration) != typedefTranslationTable.end())
                        sageType = typedefTranslationTable.find(typedefDeclaration)->second;

                  ROSE_ASSERT( sageType != NULL );
                  classType = isSgClassType(sageType);
                  if(classType != NULL)
                          classDeclarationList.push_back(classType->get_declaration());
                  
              }else{
                  classDeclarationList = findClassDeclarationsFromTypeName(scopesVector, typeName);
              
                  if( classDeclarationList.empty() == true )
                      if( isSgClassType(variableType) ){
                              
                              SgClassDeclaration* clDecl = isSgClassDeclaration(isSgClassType(variableType)->get_declaration());
                              ROSE_ASSERT( clDecl != NULL );
                              classDeclarationList.push_back(clDecl);
                      }
              }
              */  
              ROSE_ASSERT(sageType != NULL);
              //Handle the case where the type is not a SgClassType
              switch(sageType->variantT()){
                      case V_SgClassType:
                              break;
                      case V_SgComplex:
                      case V_SgTypeBool:
                      case V_SgEnumType:
                      case V_SgTypeChar:
                      case V_SgTypeVoid:
                      case V_SgTypeInt:
                      case V_SgTypeDouble:
                      case V_SgTypeFloat:
                      case V_SgTypeLong:
                      case V_SgTypeLongDouble:
                      case V_SgTypeEllipse:
                      case V_SgTypeGlobalVoid:
                      case V_SgTypeLongLong:
                      case V_SgTypeShort:
                      case V_SgTypeSignedChar:
                      case V_SgTypeSignedInt:
                      case V_SgTypeSignedLong:
                      case V_SgTypeSignedShort:
                      case V_SgTypeString:
                      case V_SgTypeUnknown:
                      case V_SgTypeUnsignedChar:
                      case V_SgTypeUnsignedInt:
                      case V_SgTypeUnsignedLong:
                      case V_SgTypeUnsignedShort:
                      case V_SgTypeUnsignedLongLong:
                      case V_SgReferenceType: 
                      case V_SgPointerType:
                      case V_SgModifierType:
                      case V_SgNamedType:
                      case V_SgTypedefType:
                      case V_SgPointerMemberType:
                      case V_SgArrayType: 
                      case V_SgFunctionType:
                      case V_SgMemberFunctionType:
                      case V_SgTypeWchar:
                      case V_SgTypeDefault:
                              if ((variableListElement == classFields.begin ())
                                      && (variableNameListElement ==
                                      sageInitializedNameList.begin ()))
                              {
                                      (*dotout) << classPointerName + "->addVariable(\"" +
                                              typeName + "*\", \"" + variableName +
                                              "\", \"not implemented\");\n";
                                      //static
                                      staticDataGraph->addVariable (typeName, variableName, "","",false);
                              }
                              else
                              {
                                      (*dotout) << classPointerName + "->addVariable(\"" +
                                              typeName + "*\", \"" + variableName +
                                              "\",\"not implemented\"" + 
                                              ", \"\", true );\n";
                                      //static
                                      staticDataGraph->addVariable (typeName, variableName,
                                                      "", "", true);

                              }

                              break;
                      default:
                    /*        printf ("default reached in switch within TransformationSupport::getTypeName type->sage_class_name() = %s variant = %d \n",
                                              sageType->sage_class_name(),sageType->variant());*/
                              ROSE_ABORT();
                              break;

              }

              //Handle the case where the type is a SgClassType
              if (classDeclarationList.empty() != true)
                {

                  //Handle  the printout of instances of a class inside the class. To do: unions, structs.

                  //Find the class declaration which correspond to the typename to see if it is a union
                  //If it is not a union it may be a base-class, but that is not important here.
                     
                      
                  SgClassDeclaration* sageClassDeclaration =
                        isSgClassDeclaration (*classDeclarationList.begin ());
                  ROSE_ASSERT (sageClassDeclaration != NULL);
                  
                  switch(sageClassDeclaration->get_class_type ())
                    {
                    case  SgClassDeclaration::e_union :
                    {
                      // Because of the special handling of unions as a variable
                      // it is allways printed as GenRepresentation::Contained.
                      // This will also disallow structures which has a 
                      // Union inside a Union (do not make sence, could just be
                      // one Union instead).

                      if (classDeclarationList.size () > 1)
                        {
                          cerr <<
                            "\n\nFound more than one instance of a class from"
                            <<
                            "typename when it is not possible to inherit from a"
                            << "union. Terminating\n\n";
                          exit (1);
                        }



                      string variableControl = "";

                      //find the control statement for the variable if it exist. 
                      for (list < ControlStructureContainer * >::iterator i =
                           pragmaList.begin (); i != pragmaList.end (); ++i)
                        {
                          variableControl = "";
                          ControlStructureContainer *controlStructure = *i;
                          ROSE_ASSERT (controlStructure != NULL);
                          //get the variable which corresponds to the variable name in the control structure
                          string controlString =
                            parsePragmaStringRHS (controlStructure->
                                                  getPragmaString (),
                                                  "GenUnionControl_",
                                                  variableName);
                          cout << "*****************" << controlStructure->
                            getPragmaString () << " :: " << controlString <<
                            "**************\n";


                          if (controlString.length () > 0)
                            variableControl = controlString;
                          break;

                        }
                      (*dotout) << classPointerName +
                        "->beginUnion(classReference->" + variableName +
                        ", \"" + variableName +
                        "\", GenRepresentation::Contained);\n";
                      //static
                      staticDataGraph->beginUnion (sageClassDeclaration,
                                                   variableName,
                                                   GenRepresentation::
                                                   Contained);
                      //end stati
                     if(sageClassDeclaration->get_name().str() != NULL)
                              cout << sageClassDeclaration->sage_class_name()<< endl;
                      else
                              cout << "no class name" << endl;
                       
                      cout << sageClassDeclaration->get_file_info()->get_filename() << endl;
                      cout << sageClassDeclaration->get_file_info()->get_line() << endl;

                  /*    printUnionVariables (querySolverClassFields2(sageClassDeclaration->get_definition()),
                                           classFields,
                                           queryFindCommentsInScope
                                           ("//pragma", "GenUnionControl_",
                                            sageClassDeclaration->
                                            get_definition ()), variableName);
*/
                  printUnionVariables (NodeQuery::
                                           querySubTree (sageClassDeclaration,
                                                         NodeQuery::
                                                         UnionFields),
                                           classFields,
                                           queryFindCommentsInScope
                                           ("//pragma", "GenUnionControl_",
                                            sageClassDeclaration->
                                            get_definition ()), variableName);
                      (*dotout) << classPointerName +
                        "->endUnion(GenRepresentation::Contained);\n";
                      //static
                      staticDataGraph->
                        endUnion (GenRepresentation::Contained);
                      break;
                    }
                    case SgClassDeclaration::e_class:
                    case SgClassDeclaration::e_struct:
                    {
                      //When we have a SgPointerType it is GenRepresentation::Container
                      //When it is a SgReferenceType it is GenRepresentation::Contained
                      if( typeOfPrint == GenRepresentation::Contained){
                        //AS(02/01/04) REMOVED. Do not treat the variable as a pointer.
                        //(*dotout) << "   if (" + classPointerName +
                        //      "->checkIfMapKeyHasBeenSaved(classReference->" +
                        //       "&"+variableName + ") == false )\n";
                        //        (*dotout) << "   {\n";
                         

                              if (sageClassDeclaration->get_class_type () ==
                                  SgClassDeclaration::e_class){
                                (*dotout) << "classReference->" + 
                                  variableName << "->" << method_prefix + "(" +classPointerName + ", \"" + variableName + 
                                  "\", GenRepresentation::Contained ); \n";
                              if (staticDataGraph->checkIfMapKeyExist (sageClassDeclaration) == false)
                                            staticDataGraph->setMapKey (sageClassDeclaration);
                              staticDataGraph->beginClass(sageClassDeclaration,TransformationSupport::getTypeName(sageClassDeclaration->get_type()), typeOfPrint);
                              printCSVariables(querySolverClassFields2(sageClassDeclaration->get_definition()),
                                               TransformationSupport::getTypeName(sageClassDeclaration->get_type()), sageClassDeclaration);
                              staticDataGraph->endClass(typeOfPrint);
                              }else
                                {
                                  int cnt = 0;
                                  string struct_name;
                                  if(sageClassDeclaration->get_name().str() == NULL)
                                         struct_name = "";
                                  else 
                                    struct_name = sageClassDeclaration->
                                      get_mangled_qualified_name (cnt).str ();

                                  (*dotout) << method_prefix + "_" +
                                    struct_name << "(" + classPointerName + ",\"" +
                                    variableName + "\",&classReference->" +
                                    variableName +
                                    ", GenRepresentation::Contained);\n";
                                  //static 
                                    if (staticDataGraph->checkIfMapKeyExist (sageClassDeclaration) == false)
                                          staticDataGraph->setMapKey (sageClassDeclaration);    
                                    staticDataGraph->beginStruct(sageClassDeclaration,TransformationSupport::getTypeName(sageClassDeclaration->get_type()), typeOfPrint);
                                    printCSVariables(querySolverClassFields2(sageClassDeclaration->get_definition()),
                                               TransformationSupport::getTypeName(sageClassDeclaration->get_type()), sageClassDeclaration);
                                    staticDataGraph->endStruct(typeOfPrint);
                                }

                             
                              //AS(02/01/04) REMOVED.
                              //(*dotout) << "   }\n";
                      }else if( typeOfPrint == GenRepresentation::Container){

                      (*dotout) << "if ( " + variableName + " != NULL )\n";
                      GC_CONDITIONAL_PRINT (variableName);
                      (*dotout) << "{\n" << classPointerName +
                        "->increaseMapKeyNumber(classReference->" +
                        variableName + ");\n";
                      (*dotout) << classPointerName +
                        "->addEdge(classReference->" + variableName +
                        ",classReference->" + variableName +
                        ",classReference);\n";
                      }

                      staticDataGraph->
                        increaseMapKeyNumber (sageVariableDeclaration);
                      if ((variableListElement == classFields.begin ())
                          && (variableNameListElement ==
                              sageInitializedNameList.begin ()))
                        {
                          if( typeOfPrint == GenRepresentation::Container){
                          (*dotout) << classPointerName + "->addVariable(\"" +
                            typeName + "\", \"" + variableName +
                            "\",\"\", classReference->" + variableName +
                            " ); \n";

                          (*dotout) << "}else   ";
                          //(*dotout) << "(*variableValue) << \" NULL \"; \n";
                          }

                          (*dotout) << classPointerName +
                            "->addVariable(\"" + typeName + "\", \"" +
                            variableName + " \", \"\");\n";
                              
                          //(*dotout) << "\n}\n";


                          staticDataGraph->addVariable (typeName,
                                                        variableName,
                                                        sageVariableDeclaration);

                        }
                      else
                        {
                          if(typeOfPrint == GenRepresentation::Container){
                          (*dotout) << classPointerName + "->addVariable(\"" +
                            typeName + "\", \"" + variableName +
                            "\", classReference->" + variableName +
                            ", \"\",true ); \n";
                          (*dotout) << "}else\n   ";
                          //(*dotout) << "(*variableValue) << \" NULL \"; \n";
                          }

                          (*dotout) << classPointerName +
                            "->addVariable(\"" + typeName + "\", \"" +
                            variableName +
                            " \", \"NULL\",\"\", true ); \n";

                          //(*dotout) << "\n}\n";
                          staticDataGraph->addVariable (typeName,
                                                        variableName,
                                                        sageVariableDeclaration,
                                                        "", true);

                        }

                       

                 
                      //Add edges from the variable
                      /* 
                         for(list<SgNode*>::iterator classDeclElm = classDeclarationList.begin();
                         classDeclElm != classDeclarationList.end(); ++classDeclElm)

                         {

                         staticDataGraph->addEdge(*classDeclElm, sageVariableDeclaration,
                         sageClassDeclaration);


                         }
                       */
                      break;
                    }
                    default:
                      cerr << "This case do not exist" << endl;
                      ROSE_ASSERT( 1 == 2);
                      break;

                    } /* End switch case swithc(classDeclaration->get_type()) */
                }               /* end printouts of class inside a class */
              break;
            }                   /* end case class-like structure */
            default:
              {
              //AS(011904) removed to allow removal of ostringstream* from this
              //    level of abstraction
              //(*dotout) << "(*variableValue) << classReference->" + variableName + ";";

              //Handle the printout of variables declared inside the header of the class
              if ((variableListElement == classFields.begin ())
                  && (variableNameListElement ==
                      sageInitializedNameList.begin ()))
                {
                  (*dotout) << classPointerName + "->addVariable(\"" +
                    typeName + "\", \"" + variableName +
                    "\", classReference->" + variableName + ");\n";
                  //static
                  staticDataGraph->addVariable (typeName, variableName, "");
                }
              else
                {
                  (*dotout) << classPointerName + "->addVariable(\"" +
                    typeName + "\", \"" + variableName +
                    "\", classReference->" + variableName +
                    ", \"\", true );\n";
                  //static
                  staticDataGraph->addVariable (typeName, variableName,
                                                "", "", true);

                }
              break;
            } /* End case default */
           } /* End switch variableType->variantT() */


        } /* End itaration over variableNameList */

    } /* End iteration over variableList */
  // (*dotout) << "delete variableValue;\n";
}                               /* End function: printVariables() */

/*
 *   The function
 *       writeToFile()
 *   writes the generated c++ code to file. As a first parameter it takes a string which is the filename.
 */
void
GenGeneration::writeToFile (string filename)
{
  ofstream dotfile (filename.c_str ());
  dotfile << (*dotout).str ();
}                               /* End function: writeToFile() */

void
GenGeneration::writeStaticGraphToFile (string filename)
{
  ROSE_ASSERT (filename.length () > 0);
  staticDataGraph->writeToFileAsGraph (filename);
}

/*
 * The function
 *      generateCodeFindActiveUnionVariableTypeName()
 * take as a parameter the declaration of the control-variable of a union. It returns the type-name which is currently
 * valid in the union which is controlled by the control-variable.
 */

string
  GenGeneration::
generateCodeFindActiveUnionVariableTypeName (SgVariableDeclaration *
                                             controlVariableDeclarataion)
{
  cerr <<
    "Function has to be implemented for librarys union control structure.\n";
  return "";
};
