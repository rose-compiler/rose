#include "rose.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "GenGeneration.h"


using namespace std;

#define STR_ADDVARIABLE(VAR1,VAR2,VAR3)  classPointerName + "->addVariable(\"" + VAR1 + "\", \"" + VAR2 + " \", "+VAR3+");\n"
#define VAR_ADDVARIABLE(VAR1,VAR2,VAR3)  classPointerName + "->addVariable("+VAR1+",\"" + VAR2 + "\", \"" + VAR3 + "\" ); \n"

NodeQuerySynthesizedAttributeType
querySolverClassFields2 (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;


  SgClassDefinition *sageClassDefinition = isSgClassDefinition (astNode);
  ROSE_ASSERT(sageClassDefinition != NULL);
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
    }

  return returnNodeList;
}				/* End function querySolverClassFields() */


void GenGeneration::printType(SgType* sageType)
   {
     ROSE_ASSERT(sageType != NULL);
  
     vector<SgType*> typeVector =
          typeVectorFromType (sageType);

     switch(typeVector[0]->variantT()){
         case V_SgClassType:
         case V_SgReferenceType:
         case V_SgPointerType:
	       if (typeVector[typeVector.size () - 1]->
	        	variantT () == V_SgClassType)
		    {
        	 string typeName =
	        	      TransformationSupport::getTypeName (sageType);

             Rose_STL_Container< SgNode * > classDeclarationList;

 	         SgClassType *classType =
		 	  isSgClassType (typeVector[typeVector.size () - 1]);
              		ROSE_ASSERT (classType != NULL);

			 string sageTypeName =
			  TransformationSupport::getTypeName (classType);

#if GENGEN_DEBUG               
             cout << "The file name  is: " << classType->get_declaration()->
	                 get_file_info ()->get_filename () << endl;
	         cout << "The line number is " << classType->get_declaration()->
	                 get_file_info ()->get_line () << endl;
             cout << "The typename is " << sageTypeName << endl;          
#endif
             SgClassDeclaration* classDeclaration = isSgClassDeclaration (classType->get_declaration());
             ROSE_ASSERT (classDeclaration != NULL);
       
             if (classDeclaration->get_class_type () != SgClassDeclaration::e_union)
		        printClass (classDeclaration, "", "");

            }else 
               cout << "not printed" << endl;/* End if class-type */
            break;
	        /* End case V_SgPointerType */
         default:
            break;
     }
};


// The function 
//     GenRepresentation::classTypePrint
// graphs a variable of SgClassType.
AttributeType* GenGeneration::classTypePrint(AttributeType* attribute, SgClassDeclaration* thisClassDeclaration, SgVariableDeclaration* sageVariableDeclaration,
                              string variablePrefix, 
                              GenRepresentation::TypeOfPrint typeOfPrint){
     
      //Handle  the printout of instances of a class-like structure.

      //Find the class declaration which correspond to the typename to see if it is a union
      //If it is not a union it may be a base-class, but that is not important here.
      ROSE_ASSERT(attribute != NULL);
      ROSE_ASSERT(sageVariableDeclaration!=NULL);
      ROSE_ASSERT((typeOfPrint==GenRepresentation::Container)|(typeOfPrint==GenRepresentation::Contained));


      Rose_STL_Container<SgInitializedName*> sageInitializedNameList =
     	sageVariableDeclaration->get_variables ();
      ROSE_ASSERT(sageInitializedNameList.size()==1);
      SgInitializedName* elmVar = *sageInitializedNameList.begin ();
      ROSE_ASSERT(elmVar!=NULL);
      SgType *variableType = elmVar->get_type ();
      ROSE_ASSERT (variableType != NULL);
      string variableName = elmVar->get_name ().str ();
      ROSE_ASSERT (variableName.length () > 0);
 
      //A vector which translates the SgType* into a vector of types which tells if the variable
      //is a SgPointer followed by a SgClassType and so on.

      //AS(230904) Not sure why the following code is not working. Look into it later.
      //SgVariableDefinition* sageVariableDefinition = sageVariableDeclaration->get_definition();
      //ROSE_ASSERT(sageVariableDefinition!=NULL);
      //SgType *variableType = sageVariableDefinition->get_type ();
      ROSE_ASSERT(variableType!=NULL);
      vector < SgType * >typeVector = typeVectorFromType (variableType);

      //The type name of the SgType of this SgVariableDeclaration
      string typeName = typeStringFromType (variableType);

      //The contract defines that the SgType must have a classType in the end of it's vector
      SgClassType* classType = isSgClassType (typeVector[typeVector.size () - 1]);
      ROSE_ASSERT(classType!=NULL);

      //This method is only designed to handle SgClassType, [SgReferenceType, SgClassType]
      //and [SgPointerType, SgClassType].
      //ROSE_ASSERT(typeVector.size()<=2);
      SgClassDeclaration *sageClassDeclaration =
			isSgClassDeclaration (classType->get_declaration ());
      ROSE_ASSERT (sageClassDeclaration != NULL);
#if GENGEN_DEBUG
      cout << "The name of the class in the classtype is \"" <<
              sageClassDeclaration->get_name ().str () << "\"" << endl;
#endif
      //A class may inherit from other classes, and the fields of those classes will be
      //treated like fields in the current class. 
      //classFields is a list of all the fields declared in the current class declaration
      //and the classes the current class declaration inherits from.
      Rose_STL_Container<SgNode*> classFields;
      SgClassDefinition* sageClassDefinition = thisClassDeclaration->get_definition();
      ROSE_ASSERT( sageClassDefinition != NULL );

   // DQ (9/4/2005): This has been changed to a list of pointers
   // SgBaseClassList baseClassList = sageClassDefinition->get_inheritances ();
      SgBaseClassPtrList baseClassList = sageClassDefinition->get_inheritances ();

      //sageClassDeclarationList is a list of all class declarations for the classes 
      //the current class inherits from and also the class declaration of the current class.
      Rose_STL_Container< SgClassDeclaration * >sageClassDeclarationList;
      sageClassDeclarationList.push_back (thisClassDeclaration);

      typedef SgBaseClassPtrList::iterator SgBaseClassPtrListIterator;

         for (SgBaseClassPtrListIterator baseClassElm = baseClassList.begin ();
	   baseClassElm != baseClassList.end (); ++baseClassElm)
	   {
  // SBaseClass baseClass = *baseClassElm;
	 // sageClassDeclarationList.push_back (baseClass.get_base_class ());
	    SgBaseClass* baseClass = *baseClassElm;
	    sageClassDeclarationList.push_back (baseClass->get_base_class ());
	   }

      //Make a joint list of the variables from the current class from the Class Fields of the current class
      //and all classes it inherit from. 
      for (Rose_STL_Container<SgClassDeclaration * >::iterator classDeclarationElm =
	   sageClassDeclarationList.begin ();
	   classDeclarationElm != sageClassDeclarationList.end ();
	   ++classDeclarationElm)
	{
	  SgClassDeclaration *sageClassElm =
	    isSgClassDeclaration (*classDeclarationElm);
          Rose_STL_Container<SgNode*> tempVariableList;
	  //cout << "The name of the class in NodeQuery is:" << TransformationSupport::getTypeName(sageClassElm->get_type()) << endl;
	   //  cout << "The file name is" << sageClassElm->getFileName()<< endl; 
	  ROSE_ASSERT (sageClassElm != NULL);
	  //querySolverClassFields2(sageClassElm->get_definition());
	  if ( sageClassElm->get_class_type () != SgClassDeclaration::e_union )
	    {
	      tempVariableList =
		querySolverClassFields2 (sageClassElm->
					 get_definition ());
	    }
/*	  else
	    b{
	      cout <<
		"The union is interpreted as a variable. Not class. Terminating."
		<< endl;
	      ROSE_ASSERT(1==2);
	    }*/

  // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
  // classFields.merge (tempVariableList);
	  classFields.insert(classFields.end(),tempVariableList.begin(),tempVariableList.end());

	}
#if GENGEN_DEBUG
      if (classFields.empty () == true)
	{
	  cout << "List of class fields is empty" << endl;
	  cout << "The class name is: " << TransformationSupport::
	    getTypeName (sageClassDeclaration->get_type ()) << endl;
	  cout << "The file name  is: " << sageClassDeclaration->
	    get_file_info ()->get_filename () << endl;
	  cout << "The line number is " << sageClassDeclaration->
	    get_file_info ()->get_line () << endl;
	}
#endif


  // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
     printf ("Commented out unique() member function since it is not in std::vector class \n");
  // classFields.unique ();

      //Get the conventional name of the class. This is not a unique name.
      //SgClassDeclaration::get_mangled_name() is a unique class-name.
      string name = "";
         name = sageClassDeclaration->get_name ().str ();


      //A dummy attribute type which is used in the case where only the
      //static graph is needed in the sub-tree. The dynamic code generated 
      //for this sub-tree is then discarded.     
      AttributeType* justStatic  = new AttributeType(new ostringstream());
      justStatic->set(AttributeType::TopOfCurrentScope);
      justStatic->set(AttributeType::AfterCurrentPosition);
      justStatic->set(AttributeType::BottomOfCurrentScope);

      /* 
          AS(230904) Commented out because this was only used to see if there is
          an union which is inherited from. This is not allowed. But since that
          is code which nobody is likely to write the test is a little bit 
          far-fetched and increases complexity unnecessarily 
      classDeclarationList = findClassDeclarationsFromTypeName (scopesVector,
			        TransformationSupport::getTypeName(variableType));
      //This is in the case of an anonymous union
      if (classDeclarationList.empty () == true)
         {
           // typeOfPrint = GenRepresentation::Contained;

	   SgClassDeclaration *clDecl =
	      isSgClassDeclaration (classType->get_declaration ());
	   ROSE_ASSERT (clDecl != NULL);

           classDeclarationList.push_back (clDecl);
         }
 			classDeclarationList.unique ();
     */
			



      //Get the class-name for display in the graph. This is not a unique name. If a unique
      //name is needed, use SgClassDeclaration::get_mangled_name().
      string className = "";
	 className = sageClassDeclaration->get_name ().str ();

      //Find what the current class type is. It is later used to define how to treat the
      //class-like structure.
      TypeDef classStructureType = UnknownListElement;
      if(sageClassDeclaration->get_class_type () == SgClassDeclaration::e_class)
         {
          classStructureType = an_class_type;
         
          //An anonymous class is a class which is declared like following
          //         class { ... } className;
          //There can only be one instance of such a clas, and it is not possible
          //to inherit from it. ROSE gives it a name "__T". Until there is a better
          //way to sort these instances out, the name will be used.
          if(name.length() >=3)
             if(name.substr(0,3) == "__T")
                classStructureType = an_anonymous_class;

      }else if( sageClassDeclaration->get_class_type () == SgClassDeclaration::e_struct){
          classStructureType = an_struct_type;
 
          //An anonymous stuct is a struct which is declared like following
          //         struct { ... } structName;
          //There can only be one instance of such a class. ROSE gives them the name
          //"__T...". Until there is a better way to sort these instances out, the name
          //will be used.
          if(className.length() >=3)
            if(className.substr(0,3) == "__T")
              classStructureType = an_anonymous_struct;
                               
                    
      }else if(sageClassDeclaration->get_class_type () == SgClassDeclaration::e_union){
          //Unions is treated like a variable since the interpretation of a union is a
          //variable with a type which can vary. This is implemented through structs, and
          //therefore there may be anonymous unions, unions and typedefs with union type.
          //All these cases are treated in the same way.
          classStructureType = an_union_type;
     }


     if( (classStructureType == an_union_type) | (classStructureType == an_anonymous_struct)
         | (classStructureType == an_anonymous_class)  )
      {
          //Anonymous class-like structures are accessed through variable declarations in
          //the scope where the variable is defined. The code to graph it is generated in
          //that scope and not in a separate function. Code was incorporated in such a way
          //because there is no way to define a unique name and generate a function to 
          //access the class fields otherwise.

          // Because of the special handling of unions as a variable
          // it is allways printed as GenRepresentation::Contained.
          // This will also disallow structures which has a 
          // Union inside a Union (do not make sence, could just be
          // one Union instead).

          /*
          AS(230904) Commented out because this was only used to see if there is
          an union which is inherited from. This is not allowed. But since that
          is code which nobody is likely to write the test is a little bit 
          far-fetched and increases complexity unnecessarily 

          if (classDeclarationList.size () > 1)
             {
		cerr << "\n\nFound more than one instance of a class from"+
                        "typename when it is not possible to inherit from a"+     
                        "union. Terminating\n\n";
			exit (1);
	     }
          */

          //If there is a union the control variable is the variable which tells which 
          //of the fields in the union is valid at a certain time in the execution path.
          string variableControl = "";

          //find the control statement for the variable if it exist. 
	      //AS(011504) small fix make it so that pointer to graphing of class-like structure inside union will 
	      //not be followed unless the following criteria is fulfilled. Store result in a variable.

          if(classStructureType == an_union_type){
	       (*attribute) ( classPointerName + "->beginNode(\"Union  " + variableName +
			      "\", GenRepresentation::Contained);\n",
                              AttributeType::AfterCurrentPosition);
               //begin static
	       staticDataGraph->beginNode ( "Union  " +variableName,
					 GenRepresentation::Contained,sageClassDeclaration);
               //end stati

          }else if(classStructureType == an_anonymous_struct) {
	       (*attribute) ( classPointerName + "->beginNode(\"Struct  " + variableName +
			      "\", GenRepresentation::Contained);\n",
                              AttributeType::AfterCurrentPosition);
               //begin static
	       staticDataGraph->beginNode ("Struct  " + variableName,
					 GenRepresentation::Contained,sageClassDeclaration);
               //end static

          }else if(classStructureType == an_anonymous_class) {
	       (*attribute) ( classPointerName + "->beginNode(\"Class  " + variableName +
			      "\", GenRepresentation::Contained);\n",
                              AttributeType::AfterCurrentPosition);
               //begin static
	       staticDataGraph->beginNode ("Class  " + variableName,
					 GenRepresentation::Contained,sageClassDeclaration);
	       //end stati(classStructureType == an_anonymous_class)
          }else {
               cout << "In GenGeneration::printClassAsVariable a class-like structure\n"
                    << "which is not anonymous or union can not be treated as variable.\n";
               ROSE_ASSERT( false == true );
          }

          printClassAsVariable(attribute, sageClassDeclaration, classFields,
		               variablePrefix+variableName);

	      (*attribute) ( classPointerName + "->endNode(GenRepresentation::Contained);\n",
                         AttributeType::AfterCurrentPosition);
	      //static
	      staticDataGraph->endNode (GenRepresentation::Contained);

      }else if( (classStructureType == an_struct_type) | (classStructureType == an_class_type) )
	  {
	      //When we have a SgPointerType it is GenRepresentation::Container
	      //When it is a SgReferenceType or SgClassType it is GenRepresentation::Contained
                           
          //printClass(sageClassDeclaration,"","");

	      if (typeOfPrint == GenRepresentation::Contained)
	        {
               string typeName = typeStringFromType(sageClassDeclaration->get_type());// + " ab " + variableName + " ab ";

               //The case where we have an contained Class 
	           if (sageClassDeclaration->get_class_type () == SgClassDeclaration::e_class)
	              {
		           (*attribute) ( variablePrefix + variableName + "." + method_prefix +
				                  "(" + classPointerName + ", \"" +
				                  variableName +"\", GenRepresentation::Contained ); \n",
                                  AttributeType::AfterCurrentPosition);
                                   
                    //AS(030904) Changed from identifying on behalf on subClassDeclaration 
                    //   to variableDeclaration because there must be a test to see if the
                    //   checkIfMapKeyExist in the static graph, and to check on the 
                    //   class declaration would be too restrictive in the case of a
                    //   class reference.

		            if (staticDataGraph->checkIfMapKeyExist(sageVariableDeclaration) == false)
                       {
                         staticDataGraph->setMapKey (sageVariableDeclaration);
		                 staticDataGraph->beginNode ( "Class  " + 
                                TransformationSupport::getTypeName(sageClassDeclaration->get_type ()), 
                                typeOfPrint,sageVariableDeclaration);
             			 if (traversedTypes.find (variableType) != traversedTypes.end ())
		                    {
			                 traversedTypes.insert (variableType);
			                 //I do not catch the generated code for dynamic
                             //graphing since a method is called at runtime.
                             //(see code above)	
                             printCSVariables(justStatic, sageClassDeclaration, 
                                    querySolverClassFields2(sageClassDeclaration->get_definition ()),
					                TransformationSupport::getTypeName (sageClassDeclaration->get_type ()));
		                    }
	                      staticDataGraph->endNode (typeOfPrint);
                       }
               }else{//End if(sageClassDeclaration->get_class_type() == SgClassDeclaration::e_class)
                   //The case where we have an contained Struct
		           int cnt = 0;
		           string struct_name="";
		              struct_name = sageClassDeclaration->get_mangled_qualified_name (cnt).str ();
		     			
		           (*attribute) ( method_prefix + "_" + struct_name + "(" + classPointerName 
				      			 + ",\"" + variableName + "\",&" + variablePrefix + variableName +
				                 + ", GenRepresentation::Contained);\n",
                                 AttributeType::AfterCurrentPosition);
		           //static
                   //AS(030904) Changed from identifying on behalf on subClassDeclaration 
                   //   to variableDeclaration because there must be a test to see if the
                   //   checkIfMapKeyExist in the static graph, and to check on the 
                   //   class declaration would be too restrictive in the case of a
                   //   class reference.

		           if (staticDataGraph->checkIfMapKeyExist(sageVariableDeclaration) == false)
                      {
		               staticDataGraph->setMapKey (sageVariableDeclaration);
		               staticDataGraph->beginNode ( "Struct  " + 
                                 TransformationSupport::getTypeName(sageClassDeclaration->get_type ()),
		                         typeOfPrint,sageVariableDeclaration);

                       //Do not catch the generated code for dynamic
                       //graphing since a method is called at runtime.
                       //(see code above)	
			           printCSVariables (justStatic, sageClassDeclaration,
                               querySolverClassFields2(sageClassDeclaration->get_definition ()),
	                           TransformationSupport::getTypeName(sageClassDeclaration->get_type ()));
                       staticDataGraph->endNode (typeOfPrint);
			       }
               }//End specialiced code for a contained struct
               	       	    
               staticDataGraph->increaseMapKeyNumber (sageVariableDeclaration);
               staticDataGraph->addVariable (typeName,variableName,sageVariableDeclaration);
#if RUNTIME_DEBUG
               (*attribute) ("printf(\"Here we print" + variableName+"\\n\");",AttributeType::AfterCurrentPosition); 
#endif
		       (*attribute) ( STR_ADDVARIABLE(typeName, variableName,"\"\""), AttributeType::AfterCurrentPosition);

          }else if (typeOfPrint == GenRepresentation::Container)
	           {
               //A container is a pointer to a class-like structure which is to be graphed.
               //A union can not be defined as a GenRepresentation::Container, because unions
               //are treated like a variable.

               string runtimeVarAccess = variablePrefix+variableName;
#if RUNTIME_DEBUG
               (*attribute) ("printf(\"Here we print" + variableName+"\\n\");",AttributeType::AfterCurrentPosition); 
#endif

               //Check to see if the variable is an array of SgClassType*               
               if((typeVector.size()==3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
                  (*attribute) ( classPointerName + "->beginNode(\"Array "+typeName+ variableName +
			                   "\", GenRepresentation::Contained);\n",
                              AttributeType::AfterCurrentPosition);

                  (*attribute) ( "for(int GCelm = 0; GCelm < (sizeof(" +variablePrefix+variableName+ ")/sizeof("+variablePrefix+variableName+"[0])); ++GCelm){", AttributeType::AfterCurrentPosition); 
                  runtimeVarAccess=runtimeVarAccess+"[GCelm]";
               }

               GC_CONDITIONAL_PRINT (runtimeVarAccess,AttributeType::AfterCurrentPosition);
		       (*attribute) ( "\n{\n" + classPointerName + "->increaseMapKeyNumber(",
                              AttributeType::AfterCurrentPosition);  
			   (*attribute) ( runtimeVarAccess + ");\n"+ classPointerName + "->addEdge(" ,AttributeType::AfterCurrentPosition);
		       (*attribute) ( runtimeVarAccess +
				  			  + "," + runtimeVarAccess + ",classReference);\n"
       		                  + VAR_ADDVARIABLE(runtimeVarAccess, typeName, variableName)
                              + "} else   " 
                              + STR_ADDVARIABLE(typeName , variableName, "\"\""),
                              AttributeType::AfterCurrentPosition);
               if((typeVector.size()==3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
    		       (*attribute)("}\n", AttributeType::AfterCurrentPosition);
                   (*attribute) ( classPointerName + "->endNode(GenRepresentation::Contained);\n",
                         AttributeType::AfterCurrentPosition);
               }

               //AS(210904)
               staticDataGraph->increaseMapKeyNumber (sageVariableDeclaration);
               staticDataGraph->addVariable (typeName, variableName,
			                        sageVariableDeclaration);

               if((typeVector.size()==3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
                  (*attribute) ( "for(int GCelm = 0; GCelm < (sizeof(" +variablePrefix+variableName+ ")/sizeof("+variablePrefix+variableName+"[0])); ++GCelm){", AttributeType::BottomOfCurrentScope); 
               }
               GC_CONDITIONAL_PRINT(runtimeVarAccess,AttributeType::BottomOfCurrentScope);
		       (*attribute) ("   if (" + classPointerName +
		                     "->checkIfMapKeyHasBeenSaved(" + 
		                     runtimeVarAccess + ") == false ){\n",
                             AttributeType::BottomOfCurrentScope);
#if RUNTIME_DEBUG
		       (*attribute) ("printf(\"Here we print" + variableName+"\");",AttributeType::BottomOfCurrentScope); 
#endif

               if (sageClassDeclaration->get_class_type () == SgClassDeclaration::e_class)
		           (*attribute) (runtimeVarAccess + "->" + method_prefix + "(" +
		                         classPointerName + ", \"" + variableName + "\", GenRepresentation::Container ); \n",
                                 AttributeType::BottomOfCurrentScope);
		       else{
		           int cnt = 0;
		           string struct_name="";
			           struct_name = sageClassDeclaration->get_mangled_qualified_name (cnt).str ();

	               (*attribute) (method_prefix + "_" +
		                   struct_name + "(" + classPointerName + ",\"" +
		                   variableName + "\"," +runtimeVarAccess +
		                   ", GenRepresentation::Container);\n",
                           AttributeType::BottomOfCurrentScope);
		       }

		       (*attribute)("}\n", AttributeType::BottomOfCurrentScope);
               if((typeVector.size()==3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL))
    		       (*attribute)("}\n", AttributeType::BottomOfCurrentScope);
       
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
      }else{
        cerr << "This case do not exist" << endl;
        ROSE_ASSERT (1 == 2);

      }	/* End switch case swithc(classDeclaration->get_type()) */

      //As described earlier justStatic is a dummy when the only interesting code
      //for the sub-tree is the static graph code. Therefore the dynamic code is
      //discarded with.
      delete justStatic->get();
      delete justStatic;

      return attribute;
};

AttributeType* GenGeneration::classTypePrintAsVariable(AttributeType* attribute, SgClassDeclaration* sageClassDeclaration,
                              SgVariableDeclaration* sageVariableDeclaration,
                              Rose_STL_Container<SgNode*> classFields, string variableControl,string unionVariableName, string variablePrefix, 
                              GenRepresentation::TypeOfPrint typeOfPrint){
	

      //This method handles the printout of a varible declaration of class type, when that class type is of an anonymous class-like
      //type. It also handles unions.
      //         attribute is the return attribute
      //         sageClassDeclaration is the class declaration of the class which contains the variable declaration
      //         sageVariableDeclaration is the variable declaration
      //         classFields is the fields in the sageClassDeclaration

      //Handle  the prinTout of instances of a class inside the class. To do: unions, structs.
      //Handle  the printout of instances of a class-like structure.


      //Speculative test to see if it possible to find the name and type of the class from SgInitializedName.
      Rose_STL_Container<SgInitializedName*> sageInitializedNameList = sageVariableDeclaration->get_variables ();
	  ROSE_ASSERT(sageInitializedNameList.size()==1);
      SgInitializedName* elmVar = *sageInitializedNameList.begin ();
      ROSE_ASSERT(elmVar!=NULL);
      SgType *variableType = elmVar->get_type ();
      ROSE_ASSERT (variableType != NULL);
      string variableName = elmVar->get_name ().str ();
      ROSE_ASSERT (variableName.length () > 0);
      ROSE_ASSERT (sageClassDeclaration!=NULL);
      ostringstream variableAddress;
      variableAddress << sageVariableDeclaration << variableName;
      string variableMangledName = variableAddress.str();
           
      //Find the class declaration which correspond to the typename to see if it is a union
      //If it is not a union it may be a base-class, but that is not important here.
      ROSE_ASSERT(attribute != NULL);
      ROSE_ASSERT(sageVariableDeclaration!=NULL);
      ROSE_ASSERT((typeOfPrint==GenRepresentation::Container)|(typeOfPrint==GenRepresentation::Contained));
      //A vector which translates the SgType* into a vector of types which tells if the variable
      //is a SgPointer followed by a SgClassType and so on.

      //AS(230904) Not sure why the following code is not working. Look into it later.
      //SgVariableDefinition* sageVariableDefinition = sageVariableDeclaration->get_definition();
      //ROSE_ASSERT(sageVariableDefinition!=NULL);
      //SgType *variableType = sageVariableDefinition->get_type ();
      ROSE_ASSERT(variableType!=NULL);
      vector < SgType * >typeVector = typeVectorFromType (variableType);

      //The type name of the SgType of this SgVariableDeclaration
      string typeName = typeStringFromType (variableType);

      //The contract defines that the SgType must have a classType in the end of it's vector
      SgClassType* classType = isSgClassType (typeVector[typeVector.size () - 1]);
      ROSE_ASSERT(classType!=NULL);
     
      SgClassDeclaration* subClassDeclaration =
  	  isSgClassDeclaration (classType->get_declaration ());
      ROSE_ASSERT (subClassDeclaration != NULL);

      string className=""; 
	  className = subClassDeclaration->get_name ().str ();

      //This method is only designed to handle SgClassType, [SgReferenceType, SgClassType]
      //and [SgPointerType, SgClassType].
      ROSE_ASSERT(typeVector.size()<=2);
      if((sageClassDeclaration->get_class_type()==SgClassDeclaration::e_union)&(variableControl.length()<=0)){
         ROSE_ASSERT(1==2);
      }
      //A class may inherit from other classes, and the fields of those classes will be
      //treated like fields in the current class. 
      //classFields is a list of all the fields declared in the current class declaration
      //and the classes the current class declaration inherits from.
      Rose_STL_Container<SgNode*> unionFields;
      SgClassDefinition* sageClassDefinition = subClassDeclaration->get_definition();
      ROSE_ASSERT( sageClassDefinition != NULL );
      SgBaseClassPtrList baseClassPtrList = sageClassDefinition->get_inheritances ();

      AttributeType* justStatic  = new AttributeType(new ostringstream());
      justStatic->set(AttributeType::TopOfCurrentScope);
      justStatic->set(AttributeType::AfterCurrentPosition);
      justStatic->set(AttributeType::BottomOfCurrentScope);

      //sageClassDeclarationList is a list of all class declarations for the classes 
      //the current class inherits from and also the class declaration of the current class.
      Rose_STL_Container< SgClassDeclaration * >sageClassDeclarationList;
      sageClassDeclarationList.push_back (subClassDeclaration);

      typedef SgBaseClassPtrList::iterator SgBaseClassPtrListIterator;

      for (SgBaseClassPtrListIterator baseClassElm = baseClassPtrList.begin ();
	       baseClassElm != baseClassPtrList.end (); ++baseClassElm)
	     {

	      SgBaseClass* baseClass = *baseClassElm;
	      sageClassDeclarationList.push_back (baseClass->get_base_class ());
	     }

      //Make a joint list of the variables from the current class from the Class Fields of the current class
      //and all classes it inherit from. 
      for (Rose_STL_Container< SgClassDeclaration * >::iterator classDeclarationElm =
	   sageClassDeclarationList.begin ();
	   classDeclarationElm != sageClassDeclarationList.end ();
	   ++classDeclarationElm)
	     {
	      SgClassDeclaration *sageClassElm =
	                            isSgClassDeclaration (*classDeclarationElm);
          Rose_STL_Container<SgNode*> tempVariableList;
	      //cout << "The name of the class in NodeQuery is:" << TransformationSupport::getTypeName(sageClassElm->get_type()) << endl;
	      //  cout << "The file name is" << sageClassElm->getFileName()<< endl; 
	      ROSE_ASSERT (sageClassElm != NULL);
	      //querySolverClassFields2(sageClassElm->get_definition());
	      if ( sageClassElm->get_class_type () != SgClassDeclaration::e_union )
	         {
	          tempVariableList = querySolverClassFields2 (sageClassElm->get_definition ());
	         }
	      /*else
	       {
	         cout <<
		     "The union is interpreted as a variable. Not class. Terminating."
		     << endl;
	         ROSE_ASSERT(1==2);
	       }*/

      // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
      // unionFields.merge (tempVariableList);
	      unionFields.insert(unionFields.end(),tempVariableList.begin(),tempVariableList.end());

	    }
#if GENGEN_DEBUG
      if (unionFields.empty () == true)
	   {
	    cout << "List of class fields is empty" << endl;
	    cout << "The class name is: " << TransformationSupport::
	         getTypeName (subClassDeclaration->get_type ()) << endl;
	    cout << "The file name  is: " << subClassDeclaration->
	         get_file_info ()->get_filename () << endl;
	    cout << "The line number is " << subClassDeclaration->
	         get_file_info ()->get_line () << endl;
	   }
#endif

   // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
      printf ("Commented out unique() member function since it is not in std::vector class \n");
   // unionFields.unique ();

      //Get the conventional name of the class. This is not a unique name.
      //SgClassDeclaration::get_mangled_name() is a unique class-name.
      string name = "";
         name = subClassDeclaration->get_name ().str ();


 
      //AS(011504) small fix make it so that pointer to graphing of class-like structure inside union will 
      //not be followed unless the following criteria is fulfilled. Store result in a variable.
      if(sageClassDeclaration->get_class_type() == SgClassDeclaration::e_union){
 	   (*attribute) ( checkPragmaRHSUnionControl (unionFields,
                          classFields, variableControl, variablePrefix)+ "{\n ", 
                          AttributeType::AfterCurrentPosition);
      }

            //Create a unique name for the variable from the pointer
      //address to the SgVariable declaration

      string variableNameAccess = 
	      unionVariableName + "." + variableName;


      //Find what type of class it is for use in cateorisation
      TypeDef classStructureType = UnknownListElement;

      //The pragma list is in the case of a union declaration the list of all
      //pragmas and comments which satisy the criteria for specifying a 
      //control variable. 
      //        #pragma GenUnionControl_variableName = (..==..)&(..!=..)...
      Rose_STL_Container<SgNode*> pragmaList;
 
      if(subClassDeclaration->get_class_type () == SgClassDeclaration::e_class){
          classStructureType = an_class_type;
 
          //An anonymous class is a class which is declared like following
          //         class { ... } className;
          //There can only be one instance of such a clas, and it is not possible
          //to inherit from it. ROSE gives it a name "__T". Until there is a better
          //way to sort these instances out, the name will be used.
          if(className.length() >=3){
             if(className.substr(0,3) == "__T")
                classStructureType = an_anonymous_class;
          }

      }else if( subClassDeclaration->get_class_type () == SgClassDeclaration::e_struct){
          classStructureType = an_struct_type;
          
          //An anonymous struct is a struct which is declared like following
          //         struct { ... } structName;
          //There can only be one instance of such a class. ROSE gives them the name
          //"__T...". Until there is a better way to sort these instances out, the name
          //will be used.
                   
          if(className.length() >=3)
              if(className.substr(0,3) == "__T")
                  classStructureType = an_anonymous_struct;
                               
                    
      }else if(subClassDeclaration->get_class_type () == SgClassDeclaration::e_union){
          //Unions is treated like a variable since the interpretation of a union is a
          //variable with a type which can vary. This is implemented through structs, and
          //therefore there may be anonymous unions, unions and typedefs with union type.
          //All these cases are treated in the same way.
          classStructureType = an_union_type;

      }

                    

      if (typeOfPrint == GenRepresentation::Contained)
       {
        if( (classStructureType == an_union_type) | (classStructureType == an_anonymous_struct)
            | (classStructureType == an_anonymous_class)  )
           {
	       // Because of the special handling of unions as a variable
	       // it is allways printed as GenRepresentation::Contained.
	       // This will also disallow structures which has a 
	       // Union inside a Union (do not make sence, could just be
	       // one Union instead).

#if RUNTIME_DEBUG
           (*attribute) ( "printf(\"\\n Conatined Class " +variableName + "\\n\");",
                              AttributeType::AfterCurrentPosition);
#endif
           staticDataGraph->increaseMapKeyNumber (sageVariableDeclaration);
                   
	       //find the control statement for the variable if it exist. 
	       //AS(011504) small fix make it so that pointer to graphing of class-like structure inside union will 
	       //not be followed unless the following criteria is fulfilled. Store result in a variable.
	       //end stati
           /*	    (*attribute) ("   if (" + classPointerName +
	                  "->checkIfMapKeyHasBeenSaved(&" + 
	                  variableNameAccess + ") == false )\n" 
                          + classPointerName + "->setMapKey(&"+variableNameAccess");\n", 
                          AttributeType::AfterCurrentPosition);*/
           if(classStructureType == an_union_type){

	          (*attribute) ( classPointerName +
			             "->beginNode(" +
			              "\"Union  " + variableName +
			              "\", GenRepresentation::Contained);\n",
                          AttributeType::AfterCurrentPosition);
              //begin static
	          staticDataGraph->beginNode ("Struct  " + variableName,
			                 GenRepresentation::
							 Contained,subClassDeclaration);
			  //end static
 
           }else if(classStructureType == an_anonymous_struct) {
	          (*attribute) ( classPointerName +
			      "->beginNode( \"Struct  " + variableName +
			      "\", GenRepresentation::Contained);\n",
                              AttributeType::AfterCurrentPosition);
              //begin static
		      staticDataGraph->beginNode ("Struct  " + variableName,
	                GenRepresentation::Contained, subClassDeclaration); 
              //end stati
			    
           }else if(classStructureType == an_anonymous_class) {
	          (*attribute) ( classPointerName +
			      "->beginNode(\" Class  " + variableName +
			      "\", GenRepresentation::Contained);\n",
                              AttributeType::AfterCurrentPosition);
              //begin static
		      staticDataGraph->beginNode ("Class  " + variableName,
			        GenRepresentation::Contained,subClassDeclaration);
	          //end stati(classStructureType == an_anonymous_class)
           }else{
              cout << "In GenGeneration::printClassAsVariable a class-like structure\n"
                   << "which is not anonymous or union can not be treated as variable.\n"; 
              ROSE_ASSERT( false == true );
           }

           //s(classStructureType == an_anonymous_class)tatic
#if GENGEN_DEBUG
           cout << "Before printClassAsVariable " << className << endl;
#endif
           Rose_STL_Container<SgNode*> subClassDeclarationLst = querySolverClassFields2(sageClassDeclaration->get_definition());
           ROSE_ASSERT(subClassDeclarationLst.empty() == false );
	       printClassAsVariable(attribute, subClassDeclaration,subClassDeclarationLst,
	                         variableNameAccess,unionVariableName+".");
	       (*attribute) ( classPointerName +"->endNode(GenRepresentation::Contained);\n",
                           AttributeType::AfterCurrentPosition);
	       //static
	       staticDataGraph->endNode (GenRepresentation::Contained);
                            
           if(classStructureType == an_anonymous_class)
              (*attribute) ( "\n}\n", AttributeType::AfterCurrentPosition);
			  
	    }else if((classStructureType == an_class_type)|(classStructureType==an_struct_type)){		
           typeName = typeStringFromType(subClassDeclaration->get_type());// + " ab " + variableName + " ab ";

           //The case where we have an contained Class 
	       if (subClassDeclaration->get_class_type () == SgClassDeclaration::e_class)
               {
	           (*attribute) (variableNameAccess + "." + method_prefix +
	                      "(" + classPointerName + ", \"" + variableName +
		        	      "\", GenRepresentation::Contained ); \n",
                              AttributeType::AfterCurrentPosition);
               //AS(030904) Changed from identifying on behalf on subClassDeclaration 
               //   to variableDeclaration because there must be a test to see if the
               //   checkIfMapKeyExist in the static graph, and to check on the 
               //   class declaration would be too restrictive in the case of a
               //   class reference.
		       if (staticDataGraph->checkIfMapKeyExist(sageVariableDeclaration) == false){
		         staticDataGraph->setMapKey (sageVariableDeclaration);
		         staticDataGraph->beginNode ("Class  " + 
                         TransformationSupport::getTypeName(subClassDeclaration->get_type ()), 
                         typeOfPrint,sageVariableDeclaration);
                 if (traversedTypes.find (variableType) != traversedTypes.end ()){
		            traversedTypes.insert (variableType);
			     //Do not catch the generated code for dynamic
                 //graphing since a method is called at runtime.
                 //(see code above)	
                 printCSVariables(justStatic,subClassDeclaration, 
                            querySolverClassFields2(subClassDeclaration->get_definition ()),
			                TransformationSupport::getTypeName (subClassDeclaration->get_type ()),
                            variableNameAccess);
			   }    
               staticDataGraph->endNode (typeOfPrint);
               }
	       }else{
             //The case where we have an contained Struct
		     int cnt = 0;     
             string struct_name="";
	           struct_name = subClassDeclaration->get_mangled_qualified_name (cnt).str ();

		     (*attribute) (method_prefix + "_" +
			      struct_name + "(" + classPointerName +
			      ",\"" + variableName +
			      "\",&" + variableNameAccess +
			      ", GenRepresentation::Contained);\n",
                              AttributeType::AfterCurrentPosition);
		     //static 
             //AS(030904) Changed from identifying on behalf on subClassDeclaration 
             //   to variableDeclaration because there must be a test to see if the
             //   checkIfMapKeyExist in the static graph, and to check on the 
             //   class declaration would be too restrictive in the case of a
             //   class reference.

		     if (staticDataGraph->checkIfMapKeyExist(sageVariableDeclaration) == false){
		         staticDataGraph->setMapKey (sageVariableDeclaration);
		         staticDataGraph->beginNode ("Struct  " + 
                        TransformationSupport::getTypeName(subClassDeclaration->get_type ()),
	                    typeOfPrint,sageVariableDeclaration);

                 //Do not catch the generated code for dynamic
                 //graphing since a method is called at runtime.
                 //(see code above)	
		         printCSVariables (justStatic,subClassDeclaration, 
                        querySolverClassFields2(subClassDeclaration->get_definition ()),
	                    TransformationSupport::getTypeName(subClassDeclaration->get_type ()),"");

		         staticDataGraph->endNode (typeOfPrint);
              }  
           }//End case contained struct
                             
		   /* End if(classDeclarationList.empty() != true */
                           
        } /* end if classStructureType equals an_class_type or an_struct_type */
          
        if(sageClassDeclaration->get_class_type() == SgClassDeclaration::e_union)
           (*attribute) ( "\n} /* End of checking control variable of union */\n",
                          AttributeType::AfterCurrentPosition);
      }else if (typeOfPrint == GenRepresentation::Container){
           string runtimeVarAccess = variableNameAccess;
                       
        if(isSgPointerType(typeVector[0])!=NULL)
           if((classStructureType==an_struct_type)|(classStructureType==an_class_type)){

             string runtimeVarAccess = variableNameAccess;

             if((typeVector.size()==3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
                (*attribute) ( "for(int GCelm = 0; GCelm < (sizeof(" +variableNameAccess+ ")/sizeof("+variableNameAccess+"[0])); ++GCelm){", AttributeType::BottomOfCurrentScope); 
                runtimeVarAccess=runtimeVarAccess+"[GCelm]";
             }

             GC_CONDITIONAL_PRINT(runtimeVarAccess,AttributeType::BottomOfCurrentScope);                

             (*attribute) ("bool followPointerTo" +
	                       variableMangledName +" = false; // as default do not follow pointer\n", 
                           AttributeType::TopOfCurrentScope);
#if RUNTIME_DEBUG
       	     (*attribute) ("printf(\"Here we print " + variableName+"\");", 
                              AttributeType::BottomOfCurrentScope);
#endif
             (*attribute) ( "if(followPointerTo" + variableMangledName + " == true) \n"
                       + "   if (" + classPointerName + "->checkIfMapKeyHasBeenSaved("
		               + runtimeVarAccess + ") == false )" + "{\n",
                       AttributeType::BottomOfCurrentScope);


	         if (subClassDeclaration->get_class_type () == SgClassDeclaration::e_class)
	            (*attribute) (variableName + "->" + method_prefix + "(" +
		                  classPointerName + ", \"" + variableName + "\", GenRepresentation::Container ); \n",
                                  AttributeType::BottomOfCurrentScope);
		     else{
		        int cnt = 0;
                string struct_name = "";
		           struct_name = subClassDeclaration->get_mangled_qualified_name (cnt).str ();

		        (*attribute) (method_prefix + "_" +
		                  struct_name + "(" + classPointerName + ",\"" +
		                  variableName + "\"," +runtimeVarAccess +
		                  ", GenRepresentation::Container);\n",
                          AttributeType::BottomOfCurrentScope);
		     }
                 
             (*attribute)("}\n", AttributeType::BottomOfCurrentScope);
                                   //printClass(subClassDeclaration,"","");
             if((typeVector.size()==3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL))
   		       (*attribute)("}\n", AttributeType::BottomOfCurrentScope);

           }

        //The case where the variable is a pointer
        //AS(070904) commented out to test concept
        if((typeVector.size()==3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
           (*attribute) ( classPointerName + "->beginNode(\"Array "+typeName+ variableName +
	                      "\", GenRepresentation::Contained);\n",
                          AttributeType::AfterCurrentPosition);
           (*attribute) ( "for(int GCelm = 0; GCelm < (sizeof(" +variableNameAccess+ ")/sizeof("+variableNameAccess+"[0])); ++GCelm){", AttributeType::AfterCurrentPosition); 
        }
        GC_CONDITIONAL_PRINT (runtimeVarAccess,AttributeType::AfterCurrentPosition);
        (*attribute) ( "\n{\n followPointerTo" + variableMangledName + " = true; \n"
                       + classPointerName +"->increaseMapKeyNumber(" + runtimeVarAccess + ");\n"
                       + classPointerName + "->addEdge(" 
	                   + variableNameAccess + "," + runtimeVarAccess+",classReference);\n",
                       AttributeType::AfterCurrentPosition);

#if RUNTIME_DEBUG
        (*attribute) ("printf(\"Printing variable : " + variableName + "\");\n",AttributeType::AfterCurrentPosition);
#endif
        (*attribute) (VAR_ADDVARIABLE(runtimeVarAccess, typeName, variableName)
                           + "}else\n"+ 
                           STR_ADDVARIABLE(typeName,variableName,"\"NULL\""),
                            AttributeType::AfterCurrentPosition);
         if((typeVector.size()==3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
    		       (*attribute)("}\n", AttributeType::AfterCurrentPosition);
                   (*attribute) ( classPointerName + "->endNode(GenRepresentation::Contained);\n",
                         AttributeType::AfterCurrentPosition);
         }

        if(sageClassDeclaration->get_class_type() == SgClassDeclaration::e_union)
	  	  (*attribute) ( "\n} // End of checking control variable of union\n",
                         AttributeType::AfterCurrentPosition);

        //static
        staticDataGraph->increaseMapKeyNumber (sageVariableDeclaration);
	    staticDataGraph->addVariable (typeName,variableName,"");

      }	/* End if(printType == GenRepresentation::Container */

      delete justStatic->get();
      delete justStatic;
      return attribute;

};
 
void GenGeneration::printGlobalScope(SgGlobal* sageGlobal, Rose_STL_Container<SgNode*> globalDeclarationLst)
  {
#if GENGEN_DEBUG
     cout << "Generating code to print Global Scope." << endl;
#endif
     ROSE_ASSERT(sageGlobal != NULL);
	     /* Here I will insert graphing of globalDeclarationLst */
     staticDataGraph->beginNode ("globalScope",
	       GenRepresentation::Container, sageGlobal);

    string bta = "int GCpointerCheck (void *ptr)\n { \n int isvalid=1;\n if(NULL=ptr) isvalid=0;\n else if(GC_base(ptr)!=NULL)\n isvalid=0;\n return isvalid;};";

	//generate code for starting the dynamic graphing method for the variable declarations in 
    //in the global scope


	(*dotout) << "\nvoid " << "globalScope_" +  
	   method_prefix << "( GenRepresentation* " + classPointerName +
	   ", GenRepresentation::TypeOfPrint printType = GenRepresentation::Container){\n";
     //The address to the GenRepresentation object is used as an unique name for the 
     //global scope. As long as that is not done elsewhere it is ok.
    (*dotout) << classPointerName << "->beginNode(\"GlobalScope\", printType,"+ classPointerName + ");\n";

    AttributeType* attribute = new AttributeType(dotout);
    attribute->set(AttributeType::TopOfCurrentScope);
    attribute->set(AttributeType::AfterCurrentPosition);
    attribute->set(AttributeType::BottomOfCurrentScope);

 // DQ (11/8/2006): It is now an error to try to call a IR node constructor without any 
 // parameters (Sg_File_Info is now required, is no other arguments are used).
 // SgClassDeclaration* dummy = new SgClassDeclaration();
    SgClassDeclaration* dummy = new SgClassDeclaration(Sg_File_Info::generateDefaultFileInfo());

    string dummyStr = "dummy";
    SgName dummyName(dummyStr.c_str());
    dummy->set_name(dummyName);
    printCSVariables (attribute, dummy,globalDeclarationLst, "", "::");
    //list < SgNode * >classNotPrinted =
    //	queryClassNotPrinted (attribute, globalDeclarationLst, printedClasses);


    staticDataGraph->endNode (GenRepresentation::Container);
    //generate code to end the dynamic graphing method
    (*dotout) << classPointerName << "->endNode( printType );\n";
    (*dotout) << "\n}" << "/* End method: " << "globalScope" +
	   method_prefix << " */ \n";
    dotout = attribute->get();
    /*typedef list < SgNode * >::iterator variableIterator;
      if (globalDeclarationLst.empty () != true)
	for (variableIterator variableListElement = classNotPrinted.begin ();
	     variableListElement != classNotPrinted.end ();
	     ++variableListElement)
	  {
	    SgNode *element = isSgNode (*variableListElement);
	    ROSE_ASSERT (element != NULL);

	    printClass (element, "", "");

	  }
*/
     ROSE_ASSERT(isSgProject(sageGlobal->get_parent()->get_parent()) != NULL);
     staticDataGraph->addEdge(sageGlobal->get_parent()->get_parent(), sageGlobal, sageGlobal);
#if GENGEN_DEBUG
     cout << "Finished Generating code for global scope." << endl << endl;
#endif




  };


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
Rose_STL_Container< SgNode * >GenGeneration::queryClassNotPrinted (AttributeType* attribute, Rose_STL_Container<
						      SgNode * >classFields,
						      set <
						      SgNode *
						      >printedClasses,
                                                      TypeDef classDeclType,
						      string
						      variablePrefix)
{
  typedef
    Rose_STL_Container<
  SgNode * >::iterator
    variableIterator;
 /* typedef
    SgInitializedNameList::iterator
    variableNameIterator;
*/
  Rose_STL_Container< SgNode * >classNotPrinted;

  if (classFields.empty () != true)
    for (variableIterator variableListElement = classFields.begin ();
	 variableListElement != classFields.end (); ++variableListElement)
      {

	SgVariableDeclaration *
	  sageVariableDeclaration =
	  isSgVariableDeclaration (*variableListElement);
	ROSE_ASSERT (sageVariableDeclaration != NULL);

	Rose_STL_Container<SgInitializedName*>
	  sageInitializedNameList = sageVariableDeclaration->get_variables ();


	for (Rose_STL_Container<SgInitializedName*>::iterator variableNameListElement =
	     sageInitializedNameList.begin ();
	     variableNameListElement != sageInitializedNameList.end ();
	     ++variableNameListElement)
	  {
	    Rose_STL_Container< SgNode * >classDeclarationList;
	    SgInitializedName*
	      elmVar = *variableNameListElement;
        ROSE_ASSERT(elmVar!=NULL);
	    string
	      variableName = elmVar->get_name ().str ();

          //Create a unique name for the variable from the pointer
          //address to the SgVariable declaration
          ostringstream variableAddress;
          variableAddress << sageVariableDeclaration << variableName;
          string variableMangledName = variableAddress.str();

	    SgType *
	      variableType = elmVar->get_type ();
	    ROSE_ASSERT (variableType != NULL);

	    string
	      typeName = TransformationSupport::getTypeName (variableType);
	    ROSE_ASSERT (typeName.length () > 0);

	    SgNodePtrVector
	      scopesVector = findScopes (sageVariableDeclaration);

	    //SgTypedefDeclaration *typedefDeclaration =
	    //      findTypedefFromTypeName (scopesVector, typeName);

	    vector < SgType * >typeVector = typeVectorFromType (variableType);
       
	//    if (typeVector[0]->variantT () == V_SgPointerType)
              //AS(180804) added a clause to ensure that only class types is graphed
/*	      if ((typeVector[0]->variantT () ==
		  V_SgPointerType) && (isSgClassType(typeVector[typeVector.size()-1]) != NULL))
*/
          
	      if(isSgClassType(typeVector[typeVector.size()-1]) != NULL)

		{
		  //The class declaration was found in a TypedefDeclaration
                  ROSE_ASSERT(isSgClassType(typeVector[typeVector.size()-1]) != NULL);
		  SgClassDeclaration *
		    sageClassDeclaration =
		    isSgClassDeclaration (isSgClassType
					  (typeVector
					   [typeVector.size () -
					    1])->get_declaration ());
		  ROSE_ASSERT (sageClassDeclaration != NULL);
		  string
		    className =
		    TransformationSupport::
		    getTypeName (typeVector[typeVector.size () - 1]);


                  TypeDef classStructureType = UnknownListElement;                  
                  if(sageClassDeclaration->get_class_type () ==
	                   SgClassDeclaration::e_class){
                      classStructureType = an_class_type;
          
                      if(className.length() >=3){
                           if(className.substr(0,3) == "__T")
                              classStructureType = an_anonymous_class;
                      }

                     }else if( sageClassDeclaration->get_class_type () ==
	                       SgClassDeclaration::e_struct){
                               classStructureType = an_struct_type;
                              
                               if(className.length() >=3)
                                  if(className.substr(0,3) == "__T")
                                    classStructureType = an_anonymous_struct;
                               
                    
                    }else if(sageClassDeclaration->get_class_type () ==
	                     SgClassDeclaration::e_union)
                             classStructureType = an_union_type;



		  //Since unions are treated as variables exempt unions from treatment as a class.                 
                    if((classStructureType == an_class_type) | (classStructureType == an_struct_type))
		      {
	        	//Handle  the printout of instances of a class inside the class. To do: unions, structs.
                        if(isSgPointerType(typeVector[0])!=NULL){
			//if class not printed add it to returnlist.
			if (printedClasses.find (sageClassDeclaration) ==
			    printedClasses.end ())
			  classNotPrinted.push_back (sageClassDeclaration);

			break;
                        }
		      }
		   else if((classStructureType==an_union_type)|(classStructureType==an_anonymous_struct)
                           |(classStructureType==an_anonymous_class))
		      {

                        string variableNameAccess = variablePrefix+variableName+".";
			Rose_STL_Container< SgNode * >unionVariableList =
			  querySolverClassFields2 (sageClassDeclaration->
						   get_definition ());
			//NodeQuery::querySubTree (sageClassDeclaration->get_definition(),
			//                       querySolverClassFields2);
			//generate the code needed for printing the classes inside the union, and accept
			//the references to the classes inside the
			Rose_STL_Container< SgNode * >queryReturnList =
			  queryClassNotPrinted (attribute, unionVariableList,
						printedClasses, classStructureType, variableNameAccess);

      // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
		// classNotPrinted.merge (queryReturnList);
			classNotPrinted.insert(classNotPrinted.end(),queryReturnList.begin(),queryReturnList.end());

			break;
		      }
		    else{
		      //not possible
		      ROSE_ABORT ();
		      break;
		    }		
		}		/* if(typeVector[typeVector.size()-1]->variantT() == V_SgClassType) */


	  }			/* End iterator over sageInitializedNameList */
      }

  return classNotPrinted;
};				/* End function: queryClassNotPrinted() */
GenGeneration::GenGeneration ()
{
  dotout = new ostringstream ();
  header = new ostringstream ();
  (*dotout) << "int GCpointerCheck (void *ptr)\n { \n int isvalid=1;\n if(NULL==ptr) isvalid=0;\n else if(GC_base(ptr)!=NULL)\n isvalid=0;\n return isvalid;\n};\n";
  (*header) << "\n#define CGRAPHPP_USE_GC 1\n";
  (*header) << "int GCpointerCheck (void *ptr);\n";
  
  classPointerName = "generateRepresentation";
  nameOfConstructedClass = "StuctureSaver";
  method_prefix = "save";
  staticDataGraph = new GenRepresentation ();
};

GenGeneration::~GenGeneration ()
{
  delete dotout;
  delete header;
  delete staticDataGraph;
};

void
GenGeneration::clear ()
{
  delete dotout;
  delete header;
  dotout = new ostringstream ();
  header = new ostringstream ();
};


GenRepresentation *
GenGeneration::getPtrStaticGenRepresentation ()
{
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

  TypeDef classStructureType;

  string structGraphClassName = "GenStructGraph";

  SgClassDeclaration *sageClassDeclaration = isSgClassDeclaration (node);
#if GENGEN_DEBUG
  cout << endl << "Start graphing class" << endl;
  cout << "The file name  is: " << node->
	    get_file_info ()->get_filename () << endl;
	  cout << "The line number is " << node->
	    get_file_info ()->get_line () << endl;
#endif
  AttributeType* attribute = new AttributeType(dotout);
  ROSE_ASSERT (sageClassDeclaration != NULL);

  if(sageClassDeclaration->get_class_type() != SgClassDeclaration::e_union){
  //AS(180408) Can not see why this test is needed. Removed.
  //if (sageClassDeclaration->get_definition ()->get_declaration () !=
  //    sageClassDeclaration)
  //  cout << "The printClass" << endl;
  //else
   if (printedClasses.find (node) == printedClasses.end ())
    {
      	name = sageClassDeclaration->get_name ().str ();


      //Find what the current class type is
     if(sageClassDeclaration->get_class_type () ==  SgClassDeclaration::e_class){
        classStructureType = an_class_type;
          
        /* if(name.length() >=3){
           if(name.substr(0,3) == "__T")
              classStructureType = an_anonymous_class;
        }*/

     }else if( sageClassDeclaration->get_class_type () == SgClassDeclaration::e_struct){
	    classStructureType = an_struct_type;
        /*
         if(name.length() >=3){
          if(name.substr(0,3) == "__T")
             classStructureType = an_anonymous_struct;
        }*/
                    
     }else if(sageClassDeclaration->get_class_type () == SgClassDeclaration::e_union) {
	    classStructureType = an_union_type;
    } else {
      ROSE_ASSERT (!"Don't have a value for classStructureType");
    }
      

     if ((classStructureType == an_class_type) || (classStructureType == an_anonymous_class) )
	    {
	    //generate code for starting the graphing method for a class
	    (*attribute) << "\nvoid " + getScopeString(sageClassDeclaration) + name + "::" +
	                    method_prefix + "( GenRepresentation* " + classPointerName +
	                    ", string className, GenRepresentation::TypeOfPrint printType = GenRepresentation::Container){\n";
	    (*attribute) << name + "* classReference = this;\n";
#if RUNTIME_DEBUG
        (*attribute) << "printf(\"\\nClass " + getScopeString(sageClassDeclaration) + name+"\\n\");";
#endif
        (*header) << "\nvoid " << getScopeString(sageClassDeclaration) << name << "::" +
	                    method_prefix << "( GenRepresentation* " + classPointerName +
	                    ", string className, GenRepresentation::TypeOfPrint printType );\n";
     }else if ((classStructureType == an_struct_type) | (classStructureType==an_anonymous_struct)){
        //AS(240804) If the class is an anonymous type ROSE gives it a name which
        //           starts with '__T'. In the special case where that type is an
        //           struct and at the same time an anonymous type a separate method 
        //           for graphing it is not created. 
	 
        //generate code for starting the graphing method for a struct
	    int cnt = 0;
	    string struct_name, struct_mangled_name;
	       struct_mangled_name = sageClassDeclaration->get_mangled_qualified_name (cnt).str ();
	       struct_name = sageClassDeclaration->get_name ().str ();

	    (*attribute) << "\nvoid " + method_prefix + "_" + struct_mangled_name + "( GenRepresentation* " +
         	    classPointerName + ", string className," + getScopeString(sageClassDeclaration) + struct_name +
	            "* classReference, GenRepresentation::TypeOfPrint printType = GenRepresentation::Container){\n";
        (*header) << "\nvoid " << method_prefix + "_" +  struct_mangled_name << "( GenRepresentation* " +
	  	        classPointerName + ", string className," + getScopeString(sageClassDeclaration) + struct_name +
	            "* classReference, GenRepresentation::TypeOfPrint printType );\n";
#if RUNTIME_DEBUG
        (*attribute) << "printf(\"\\nClass " + getScopeString(sageClassDeclaration) + name+"\\n\");";
#endif



     }else if(classStructureType==an_union_type)
	   {
	   cerr << "Defined fair use of unions not fulfilled. Terminating.\n";
	   ROSE_ASSERT(1==2);
	   }

     attribute->set(AttributeType::TopOfCurrentScope);
     attribute->set(AttributeType::AfterCurrentPosition);
     attribute->set(AttributeType::BottomOfCurrentScope);
     cout << "THE NAME IS" << name << endl << endl;
     printedClasses.insert (node);
     Rose_STL_Container< SgNode * >variableList, tempVariableList;
     Rose_STL_Container< SgClassDeclaration * >sageClassDeclarationList;

     sageClassDeclarationList.push_back (sageClassDeclaration);
     //find all classes which the current class inherit from 
     SgClassDefinition *classDefinition =
          	isSgClassDefinition (sageClassDeclaration->get_definition ());
     ROSE_ASSERT (classDefinition != NULL);

     SgBaseClassPtrList baseClassPtrList = classDefinition->get_inheritances ();


     typedef SgBaseClassPtrList::iterator SgBaseClassPtrListIterator;

     for (SgBaseClassPtrListIterator baseClassElm = baseClassPtrList.begin ();
	   baseClassElm != baseClassPtrList.end (); ++baseClassElm)
	   {
	    SgBaseClass* baseClass = *baseClassElm;
	    sageClassDeclarationList.push_back (baseClass->get_base_class ());
	   }
     //Make a joint list of the variables from the current class from the Class Fields of the current class
     //and all classes it inherit from. 
     for (Rose_STL_Container< SgClassDeclaration * >::iterator classDeclarationElm =
	   sageClassDeclarationList.begin ();
	   classDeclarationElm != sageClassDeclarationList.end ();
	   ++classDeclarationElm)
	 {
	  SgClassDeclaration *sageClassElm =
	    isSgClassDeclaration (*classDeclarationElm);
	  ROSE_ASSERT (sageClassElm != NULL);

	  if ( sageClassElm->get_class_type () != SgClassDeclaration::e_union )
	    {
	      tempVariableList = querySolverClassFields2 (sageClassElm->get_definition ());
	  }else{
	      cout <<"The union is interpreted as a variable. Not class. Terminating."	<< endl;
	      ROSE_ASSERT(1==2);
	  }

  // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
  // variableList.merge (tempVariableList);
	  variableList.insert(variableList.end(),tempVariableList.begin(),tempVariableList.end());

	 }
     if (variableList.empty () == true)
	    {
	  cout << "variableList is empty" << endl;
	  cout << "The class name is: " << TransformationSupport::
	    getTypeName (sageClassDeclaration->get_type ()) << endl;
	  cout << "The file name  is: " << sageClassDeclaration->
	    get_file_info ()->get_filename () << endl;
	  cout << "The line number is " << sageClassDeclaration->
	    get_file_info ()->get_line () << endl;
	 }
     // ROSE_ASSERT( variableList.empty() != true);


  // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
     printf ("Commented out unique() member function since it is not in std::vector class \n");
  // variableList.unique ();


     //Generate code which keeps track of which pointers has been followed before or not.
     //A parenthesis to make sure that if checkIfMapKeyExists(classReference)==true it is not
     //graphed, because then it has already been graphed.
     (*attribute) ("if( " + classPointerName + "->checkIfMapKeyHasBeenSaved(classReference) == false){\n"
                   +classPointerName + "->setMapKeySaved(classReference);\n"
                   +"if( " + classPointerName + "->checkIfMapKeyExist(classReference) == false)\n"
                   +classPointerName + "->setMapKey(classReference);\n",
                     AttributeType::AfterCurrentPosition);
     //Generate static dot representation
     staticDataGraph->setMapKeySaved (node);
     if (staticDataGraph->checkIfMapKeyExist (node) == false)
	    staticDataGraph->setMapKey (node);

      //Generate code which begins the class

     string inheritance = "";
     if (baseClassPtrList.empty () != true)
	   {
	    inheritance = " : " +
	   	    TransformationSupport::getTypeName ((*baseClassPtrList.begin ())->get_base_class ()->get_type ()->findBaseType ());
		}

      //Generate code which graphs the contents of the class and the ends it.
     if ((classStructureType == an_class_type) | (classStructureType == an_anonymous_class))
	   {
	   //dynamic
	   (*attribute) ( classPointerName + "->beginNode(\" Class  "
	                + name + " \"+className, printType, classReference);\n", AttributeType::AfterCurrentPosition);
	  //static
	  staticDataGraph->beginNode ("Class " + name + inheritance,
				       GenRepresentation::Container,node);
          printCSVariables (attribute, sageClassDeclaration, variableList, name);
	  //dynamic
	  (*attribute) ( classPointerName + "->endNode( printType );\n", AttributeType::AfterCurrentPosition);
	  //static
	  staticDataGraph->endNode (GenRepresentation::Container);


	}
      else if ( (classStructureType==an_struct_type) | (classStructureType==an_anonymous_struct) )
	{
	  //dynamic
	  (*attribute) ( classPointerName + "->beginNode(\"Struct  "
	    + name + " \"+className, printType,classReference);\n", AttributeType::AfterCurrentPosition);
	  //static
	  staticDataGraph->beginNode ("Struct  " + name + inheritance,
					GenRepresentation::Container,node);
          printCSVariables (attribute,sageClassDeclaration,variableList, name);
	  (*attribute) ( classPointerName + "->endNode( printType );\n",
                        AttributeType::AfterCurrentPosition);
	  //static
	  staticDataGraph->endNode (GenRepresentation::Container);


	}

   // DQ (9/26/2007): Moved from std::list to std::vector uniformly within ROSE.
      printf ("Commented out unique() member function since it is not in std::vector class \n");
   // variableList.unique ();

      cout << "The class name is: " << TransformationSupport::
	getTypeName (sageClassDeclaration->get_type ()) << endl;
      cout << "The file name  is: " << sageClassDeclaration->
	get_file_info ()->get_filename () << endl;
      cout << "The line number is " << sageClassDeclaration->
	get_file_info ()->get_line () << endl;


      //Generate code for graphing class-like structures referenced to within the scope of this
      //class-like structure if that has not been done before and then end the current method.
      //It also generates code for class-like structures referenced to when needed.

      Rose_STL_Container< SgNode * >classNotPrinted =
	queryClassNotPrinted (attribute, variableList, printedClasses);

      if ( (classStructureType == an_class_type) | (classStructureType == an_anonymous_class) )
         //Two } is one for ending method and one to correspond to the checkifMapKeyExist==false because
         //if it does it has already been graphed. 
	     (*attribute) ( string("\n}}") + "// End method: " + name + "::" +
	                    method_prefix + "\n", AttributeType::BottomOfCurrentScope);
      else if ( classStructureType == an_struct_type )
	     (*attribute) (string("\n}}") + "// End method: " + method_prefix + "_" +
	                   name + "\n", AttributeType::BottomOfCurrentScope);
      dotout = attribute->get();
      //Generate code for printing the class-like strucures referenced in the current class-like structure
      //which has not been printed yet.
      typedef Rose_STL_Container< SgNode * >::iterator variableIterator;
      if (classNotPrinted.empty () != true)
        if(classStructureType!=an_anonymous_struct)
	      for (variableIterator variableListElement = classNotPrinted.begin ();
	           variableListElement != classNotPrinted.end (); ++variableListElement)
	  	  {
	       SgNode *element = isSgNode (*variableListElement);
	       ROSE_ASSERT (element != NULL);

	       printClass (element, "", "");

	      }


    }				/* End if(current class not printed) */
   }
};				/* End function: printClass() */



/*
 *  The function:
 *        printCClassAssVariable()
 *  takes as a first parameter a list of the fields of the union, as a second parameter is the name.
 *  The scope of this method is to generate the code needed for antoher class-like structure to
 *  print out the variables inside the union. The idea is that the union is treated like a
 *  variable and if no control-variable which defines what variable is valid exists, it will not
 *  inspect the values because of ambiguity.
 *
 */
AttributeType*
GenGeneration::printClassAsVariable (AttributeType* attribute, SgClassDeclaration* classDeclaration,
				     const Rose_STL_Container<SgNode*> classFields,
				    const string unionVariableName, const string variablePrefix)
{
  ROSE_ASSERT( classDeclaration != NULL );
  Rose_STL_Container< ControlStructureContainer * >pragmaList;
  Rose_STL_Container<SgNode*> unionFields = querySolverClassFields2(classDeclaration->get_definition()); 
  AttributeType* justStatic  = new AttributeType(new ostringstream());
  justStatic->set(AttributeType::TopOfCurrentScope);
  justStatic->set(AttributeType::AfterCurrentPosition);
  justStatic->set(AttributeType::BottomOfCurrentScope);

  TypeDef classStructureType = UnknownListElement;
  string name="";
	name = classDeclaration->get_name ().str ();


  //Find what the current class type is
  if(classDeclaration->get_class_type () ==
     SgClassDeclaration::e_class){
     classStructureType = an_class_type;
     if(name.length() >=3){
       if(name.substr(0,3) == "__T")
         classStructureType = an_anonymous_class;
     }else{
       cout << name.substr(0,3) << endl;
       cout << "A struct which in not anonymous should not"
            << "be graphed as a variable" << endl;
        ROSE_ASSERT(1==2);

     }

   }else if( classDeclaration->get_class_type () ==
             SgClassDeclaration::e_struct){
             
             if(name.length() >=3){
                if(name.substr(0,3) == "__T")
                   classStructureType = an_anonymous_struct;
             }else{
                cout << name.substr(0,3) << endl;
                cout << "A struct which in not anonymous should not"
                     << "be graphed as a variable" << endl;
                ROSE_ASSERT(1==2);
             }
                    
  }else if(classDeclaration->get_class_type () ==
           SgClassDeclaration::e_union){
           classStructureType = an_union_type;
           //find if comments or pragmas in the scope of this union has 
           //defined which variable is valid in the union
           pragmaList=queryFindCommentsInScope("//pragma", "GenUnionControl_",
		classDeclaration->get_definition ());
           if (pragmaList.empty () == true)
              cout << "THE PRAGMA LIST IN printUnionVariables IS EMPTY" << endl;


  }else{
       cout << "This case in printClassAsVariable do not exist" << endl;
       ROSE_ASSERT(1==2);
  }

  ROSE_ASSERT (unionVariableName.length () > 0);
  string unionControlVariableName, variableControl;

  //iterate over all variables inside the union
  for (Rose_STL_Container< SgNode * >::iterator variableListElement = unionFields.begin ();
       variableListElement != unionFields.end (); ++variableListElement)
    {
      SgVariableDeclaration *sageVariableDeclaration =
	isSgVariableDeclaration (*variableListElement);
      ROSE_ASSERT (sageVariableDeclaration != NULL);
      cout << "The file name  is: " << sageVariableDeclaration->
	    get_file_info ()->get_filename () << endl;
	  cout << "The line number is " << sageVariableDeclaration->
	    get_file_info ()->get_line () << endl;

      
      Rose_STL_Container<SgInitializedName*> sageInitializedNameList =
	sageVariableDeclaration->get_variables ();


      for (Rose_STL_Container<SgInitializedName*>::iterator variableNameListElement =
	   sageInitializedNameList.begin ();
	   variableNameListElement != sageInitializedNameList.end ();
	   ++variableNameListElement)
	{

	  variableControl = "";

	  SgInitializedName* elmVar = *variableNameListElement;
      ROSE_ASSERT(elmVar!=NULL);
  string variableName = elmVar->get_name ().str ();

          //Create a unique name for the variable from the pointer
          //address to the SgVariable declaration
          ostringstream variableAddress;
          variableAddress << sageVariableDeclaration << variableName;
          string variableMangledName = variableAddress.str();
          //The nested name to use when accessing a variable in this 
          //class-like structure
          //AS(200904) Account for variable name
          string variableNameAccess = 
	      unionVariableName + "." + variableName;



	  ROSE_ASSERT (variableName.length () > 0);
          cout << "The name of the variable is: " << variableName << endl;
         
	  SgType *variableType = elmVar->get_type ();
	  ROSE_ASSERT (variableType != NULL);

	  //string typeName = TransformationSupport::getTypeName (variableType);
          string typeName = typeStringFromType(variableType);
	  ROSE_ASSERT (typeName.length () > 0);
	  //find the control statement for the variable if it exist.
          if(classStructureType == an_union_type){ 
	      for (Rose_STL_Container< ControlStructureContainer * >::iterator i =
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


          }
	  if ((variableControl.length () < 1) & (classStructureType == an_union_type))
	    {
	      //the case where we do not have a control statement for the variable. Do not make sence to
	      //access value since it is impossible to know its validity.

	      //Handle the printout of variables declared inside the header of the class
#if RUNTIME_DEBUG  
          (*attribute) ("printf(\"Printing variable : " + variableName + "\");\n",AttributeType::AfterCurrentPosition);
#endif
	      (*attribute) (STR_ADDVARIABLE(typeName,variableName,"\"CONTROL NOT DEFINED\""),
                    AttributeType::AfterCurrentPosition);
	      //static
	      staticDataGraph->addVariable (typeName, variableName, "");

	    }
	  else
	    {
	      //The case where a control-variable is defined
	      GenRepresentation::TypeOfPrint typeOfPrint =
		GenRepresentation::Container;


	      SgNodePtrVector scopesVector =
		findScopes (sageVariableDeclaration);

	      vector < SgType * >typeVector =
		typeVectorFromType (variableType);
#if RUNTIME_DEBUG
            (*attribute) ("printf(\"Printing variable : " + variableName + "\");\n",AttributeType::AfterCurrentPosition);
#endif
	      switch (isSgType (typeVector[0])->variantT ())
		{
		case V_SgClassType:
		case V_SgReferenceType:
		  typeOfPrint = GenRepresentation::Contained;
		case V_SgPointerType:
		  {
		    Rose_STL_Container< SgNode * >classDeclarationList;

                    if((typeVector.size()<=2)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
                     //Handle the caser where there is a SgClassType, [SgReferenceType, SgClassType]
                     //and [SgPointerType, SgClassType].
                     classTypePrintAsVariable(attribute, classDeclaration, sageVariableDeclaration,classFields, 
                             variableControl, unionVariableName, variablePrefix, typeOfPrint);
        
                    }else if((typeVector.size()<=3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
                     //ABRA
                     ROSE_ASSERT(true==false);
                     classTypePrintAsVariable(attribute, classDeclaration, sageVariableDeclaration,classFields, 
                             variableControl, unionVariableName, variablePrefix, typeOfPrint);
       

                    }else if((typeVector.size()<=2)){
                      (*attribute) ( "if(" + variableNameAccess + " != NULL)\n"
                                     + STR_ADDVARIABLE(typeName, variableName, variableNameAccess)
                                     + "else\n"
                                     + STR_ADDVARIABLE(typeName,variableName,"\"not implemented\""),
                                    AttributeType::AfterCurrentPosition);
                
                    }else{

		        (*attribute) ( STR_ADDVARIABLE(typeName, variableName, "\"not implemented\""),AttributeType::AfterCurrentPosition);
		        //static
		        staticDataGraph->addVariable (typeName,variableName, " ");
                    }

		    break;
		  }		/* End case V_SgPointerType */

		default:
		  {
		    //Handle the printout of variables declared inside the header of the class
                    if(classDeclaration->get_class_type() == SgClassDeclaration::e_union)
                        (*attribute) ( checkPragmaRHSUnionControl (unionFields,
	                         classFields, variableControl, variablePrefix), 
                                 AttributeType::AfterCurrentPosition);

		    (*attribute) ( STR_ADDVARIABLE(typeName,variableName,variableNameAccess),
                          AttributeType::AfterCurrentPosition);
	            //static
		    staticDataGraph->addVariable (typeName, variableName,"");
		    break;
		  }		/* End case default */

		}		/* End switch(hackType) */
	    }			/* else */

	}			/* End iteration over variableNameList */
    }				/* End iteration over unionFields */

delete justStatic->get();
delete justStatic;
return attribute;
}				/* End function: printClassAsVariable() */



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
GenGeneration::printClassNotPrinted (Rose_STL_Container< SgNode * >classNotPrinted,
				     string name, string variablePrefix)
{
  string variableName;
  typedef Rose_STL_Container< SgNode * >::iterator variableIterator;

  for (variableIterator variableListElement = classNotPrinted.begin ();
       variableListElement != classNotPrinted.end (); ++variableListElement)
    {

      SgClassDeclaration *sageClassDeclaration =
	          isSgClassDeclaration (*variableListElement);
      ROSE_ASSERT (sageClassDeclaration != NULL);
	     variableName = sageClassDeclaration->get_name ().str ();

      //Handle  the printout of instances of a class inside the class. To do: unions, structs.


      (*dotout) << "if (classReference-> " + variableName + " != NULL )\n";	//{\n   ";
      (*dotout) << "   if ( " + classPointerName +
	               "->checkIfMapKeyExist(" + variablePrefix  + variableName +
	               ") == false )\n";
      (*dotout) << "   {\n";
      (*dotout) << "   " << classPointerName << "->setMapKey(" + variablePrefix
	            << variableName << "," << "classReference" << ");\n";
      if (sageClassDeclaration->get_class_type () == SgClassDeclaration::e_class)
	 	(*dotout) << "" + variablePrefix +
	                 variableName << "->" << method_prefix + "(" + classPointerName +
	                 ",  GenRepresentation::Container ); \n";
      else
	    {
	    int cnt = 0;
	    string struct_name;
	       struct_name = sageClassDeclaration->get_mangled_qualified_name (cnt).str ();

	    (*dotout) << method_prefix + "_" + struct_name << "(" +
	                 classPointerName + "," + variablePrefix + variableName +
	                 ", GenRepresentation::TypeOfPrint printType = GenRepresentation::Container);\n";
	    (*dotout) << "   }\n";
	    }

    }//End for-loop over variables
};				/* End function: printClassNotPrinted */

/*
 * The function
 *     printVariables()
 * takes as a first parameter a list<SgNode*> which is the fields of a class-like structure. As a second parameter it takes the name
 * of the class-like strucure. As a third parameter it takes the methor_prefix of the generated code. As a fourth parameter it takes
 * the class declaration of the class-like structure. The scope of the funciton is to generate code to graph a class-like
 * structure.
 * 
 */

AttributeType*
GenGeneration::printCSVariables (AttributeType* attribute, SgClassDeclaration* thisClassDeclaration, Rose_STL_Container< SgNode * >classFields, string name, string variablePrefix)
{

  // Since this method may be used to recursively statically print 
  // class-like structures contained within another class-like 
  // structure it is possible to not take the code generated to
  // dynamically graph the data strucute into account. 

  //generate code to graph the variables of the class-like structure
  typedef Rose_STL_Container< SgNode * >::iterator variableIterator;
  if (classFields.empty () == true)
    {
      cout << "There is no Class Fields in class:" << name << endl;
  }else
    for (variableIterator variableListElement = classFields.begin ();
	 variableListElement != classFields.end (); ++variableListElement)
      {
       SgNode* node = *variableListElement;
       cout << "The file name  is: " << node->
	      get_file_info ()->get_filename () << endl;
	   cout << "The line number is " << node->
	      get_file_info ()->get_line () << endl;


	   SgVariableDeclaration *sageVariableDeclaration =
	        isSgVariableDeclaration (*variableListElement);
	   ROSE_ASSERT (sageVariableDeclaration != NULL);

	   Rose_STL_Container<SgInitializedName*> sageInitializedNameList =
	                      sageVariableDeclaration->get_variables ();
	   //typedef SgInitializedNameList::iterator variableNameIterator;

	for (Rose_STL_Container<SgInitializedName*>::iterator variableNameListElement =
	     sageInitializedNameList.begin ();
	     variableNameListElement != sageInitializedNameList.end ();
	     ++variableNameListElement)
	  {

	   SgInitializedName* elmVar = *variableNameListElement;
       ROSE_ASSERT(elmVar!=NULL);
	   string variableName = elmVar->get_name ().str ();


	   //AS (01/26/04)
	   //ROSE_ASSERT (name.length () > 0);

	   SgType *variableType = elmVar->get_type ();
	   ROSE_ASSERT (variableType != NULL);

	   string typeName =
	      typeStringFromType (variableType);
	   ROSE_ASSERT (typeName.length () > 0);
       cout << "The name of the variable is: " << variableName << endl;

	   //Handle graphing of different types of variables
	   GenRepresentation::TypeOfPrint typeOfPrint = GenRepresentation::Container;
	   
       SgNodePtrVector scopesVector = findScopes (sageVariableDeclaration);
	      
	   //Makes sure that this is actually a class-typedef
	   //SgType* sageType = variableType;

	   vector < SgType * >typeVector = typeVectorFromType (variableType);

	   switch (isSgType (typeVector[0])->variantT ())
	     {
	      case V_SgClassType:
	      case V_SgReferenceType:
               {
		     typeOfPrint = GenRepresentation::Contained;
             if((typeVector.size()<=2)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
                 SgClassType* classType = isSgClassType(typeVector[typeVector.size()-1]);
                 SgClassDeclaration *sageClassDeclaration =
			           isSgClassDeclaration (classType->get_declaration ());
                 ROSE_ASSERT (sageClassDeclaration != NULL);

                 string name="";
	             name = sageClassDeclaration->get_name ().str ();

/*                     if(name.length() >=3)
                       if(name.substr(0,3) == "__T"){
                         printClassAsVariable(attribute, sageClassDeclaration, classFields, variableName,
                                              variablePrefix);
                         //classTypePrintAsVariable(attribute,sageVariableDeclaration, classFields, "",
                         //         "","",typeOfPrint);
                         break;
                     }*/
                     /*if(sageClassDeclaration->get_class_type() == SgClassDeclaration::e_union){
                         //classTypePrintAsVariable(attribute,sageVariableDeclaration, classFields, "",
                         //         "","",typeOfPrint);
                         printClassAsVariable(attribute, sageClassDeclaration, classFields, variableName,
                                              variablePrefix);
                         break;
                     }*/
                 classTypePrint(attribute, thisClassDeclaration,sageVariableDeclaration, variablePrefix,
                                    typeOfPrint);
 
                   
             }else{
		        //Handle the case where the type is not a SgClassType
#if RUNTIME_DEBUG
               (*attribute) ("printf(\"Printing variable : " + variableName + "\");\n",AttributeType::AfterCurrentPosition);
#endif
		       (*attribute) ( STR_ADDVARIABLE(typeName,variableName, "\"not implemented\""),AttributeType::AfterCurrentPosition);
		       //static
		       staticDataGraph->addVariable (typeName,variableName, "", "");
		     }
                 

		  break;
                }
	      case V_SgPointerType:
		     {
		     Rose_STL_Container< SgNode * >classDeclarationList;
                  
             if((typeVector.size()<=2)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
                //Handle the caser where there is a SgClassType, [SgReferenceType, SgClassType]
                //and [SgPointerType, SgClassType].
                SgClassType* classType = isSgClassType(typeVector[typeVector.size()-1]);
                SgClassDeclaration *sageClassDeclaration =
			            isSgClassDeclaration (classType->get_declaration ());
                ROSE_ASSERT (sageClassDeclaration != NULL);

                string name="";
	                 name = sageClassDeclaration->get_name ().str ();

                     classTypePrint(attribute, thisClassDeclaration, sageVariableDeclaration, variablePrefix,typeOfPrint);

             }else if((typeVector.size()<=3)&(isSgClassType(typeVector[typeVector.size()-1])!=NULL)){
                //Handle the caser where there is a SgClassType, [SgReferenceType, SgClassType]
                //and [SgPointerType, SgClassType].
                SgClassType* classType = isSgClassType(typeVector[typeVector.size()-1]);
                SgClassDeclaration *sageClassDeclaration =
	             		isSgClassDeclaration (classType->get_declaration ());
                ROSE_ASSERT (sageClassDeclaration != NULL);

                string name="";
	                name = sageClassDeclaration->get_name ().str ();
                //ABRA
                classTypePrint(attribute, thisClassDeclaration, sageVariableDeclaration, variablePrefix,
                 typeOfPrint);

             } else if(typeVector.size()<=2){
                (*attribute) ( "if(" + variablePrefix + variableName + " != NULL)\n"
                               + STR_ADDVARIABLE(typeName,variableName,variablePrefix+variableName)
                               + "else\n"
                               + STR_ADDVARIABLE(typeName,variableName,"\"not implemented\""),AttributeType::AfterCurrentPosition);
             }else{
            
		        //Handle the case where the type is not a SgClassType
#if RUNTIME_DEBUG
                (*attribute) ("printf(\"Printing variable : " + variableName + "\");\n",AttributeType::AfterCurrentPosition);
#endif
		        (*attribute) ( STR_ADDVARIABLE(typeName, variableName, "\"not implemented\""),AttributeType::AfterCurrentPosition);
		        //static
		        staticDataGraph->addVariable (typeName,variableName, "", "");
		     }

		     break;
		    }		/* end case class                  cout << "Default case!" << endl;-like structure */
	      default:
		    {
             cout << "Default case in GenGeneration.C line2495!" << endl;
		     //Handle the printout of variables declared inside the header of the class
#if RUNTIME_DEBUG
             (*attribute) ("printf(\"Printing variable : " + variableName + "\");\n",AttributeType::AfterCurrentPosition);
#endif
		     (*attribute) ( STR_ADDVARIABLE(typeName, variableName, variablePrefix+variableName),AttributeType::AfterCurrentPosition);
		     //static
		     staticDataGraph->addVariable (typeName, variableName, "");
			 break;
		    }		/* End case default */
	}			/* End switch variableType->variantT() */
	}			/* End itaration over variableNameList */
    }				/* End iteration over variableList */

return attribute;
}				/* End function: printVariables() */

/*
 *   The function
 *       writeToFile()
 *   writes the generated c++ code to file. As a first parameter it takes a string which is the filename.
 */
void
GenGeneration::writeToFile (string filename)
{
  ofstream dotFile (filename.c_str ());
  dotFile << (*dotout).str ();
  string headerName = filename + ".h";
  ofstream headerFile(headerName.c_str () );
  headerFile << (*header).str();
}				/* End function: writeToFile() */

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
   cerr << "Function has to be implemented for librarys union control structure.\n";
   return "";
};
