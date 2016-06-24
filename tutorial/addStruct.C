#include "rose.h"

using namespace std;

#define SOURCE_POSITION Sg_File_Info::generateDefaultFileInfoForTransformationNode()

SgClassDeclaration*
buildClassDeclarationAndDefinition (string name, SgScopeStatement* scope)
   {
  // This function builds a class declaration and definition 
  // (both the defining and nondefining declarations as required).

  // This is the class definition (the fileInfo is the position of the opening brace)
     SgClassDefinition* classDefinition   = new SgClassDefinition(SOURCE_POSITION);
     assert(classDefinition != NULL);

  // Set the end of construct explictly (where not a transformation this is the location of the closing brace)
     classDefinition->set_endOfConstruct(SOURCE_POSITION);

  // This is the defining declaration for the class (with a reference to the class definition)
     SgClassDeclaration* classDeclaration = new SgClassDeclaration(SOURCE_POSITION,name.c_str(),SgClassDeclaration::e_struct,NULL,classDefinition);
     assert(classDeclaration != NULL);
     classDeclaration->set_endOfConstruct(SOURCE_POSITION);

  // Set the defining declaration in the defining declaration!
     classDeclaration->set_definingDeclaration(classDeclaration);

  // Set the non defining declaration in the defining declaration (both are required)
     SgClassDeclaration* nondefiningClassDeclaration = new SgClassDeclaration(SOURCE_POSITION,name.c_str(),SgClassDeclaration::e_struct,NULL,NULL);
     assert(classDeclaration != NULL);
     nondefiningClassDeclaration->set_endOfConstruct(SOURCE_POSITION);
     nondefiningClassDeclaration->set_scope(scope); // scope is needed for createType()
     nondefiningClassDeclaration->set_type(SgClassType::createType(nondefiningClassDeclaration));

  // Set the internal reference to the non-defining declaration
     classDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);
     classDeclaration->set_type (nondefiningClassDeclaration->get_type());

  // Set the defining and no-defining declarations in the non-defining class declaration!
     nondefiningClassDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);
     nondefiningClassDeclaration->set_definingDeclaration(classDeclaration);

  // Set the nondefining declaration as a forward declaration!
     nondefiningClassDeclaration->setForward();

  // Liao (2/13/2008), symbol for the declaration
     SgClassSymbol* mysymbol = new SgClassSymbol(nondefiningClassDeclaration);
     scope->insert_symbol(name, mysymbol);

  // Don't forget the set the declaration in the definition (IR node constructors are side-effect free!)!
     classDefinition->set_declaration(classDeclaration);

  // set the scope explicitly (name qualification tricks can imply it is not always the parent IR node!)
     classDeclaration->set_scope(scope);

  //set parent
     classDeclaration->set_parent(scope);
     nondefiningClassDeclaration->set_parent(scope);

  // some error checking
     assert(classDeclaration->get_definingDeclaration() != NULL);
     assert(classDeclaration->get_firstNondefiningDeclaration() != NULL);
     assert(classDeclaration->get_definition() != NULL);

     ROSE_ASSERT(classDeclaration->get_definition()->get_parent() != NULL);

     return classDeclaration;
   }

SgVariableDeclaration*
buildStructVariable ( SgScopeStatement* scope,
              vector<SgType*> memberTypes, vector<string> memberNames,
              string structName = "", string varName = "", SgAggregateInitializer *initializer = NULL )
   {
     ROSE_ASSERT(memberTypes.size() == memberNames.size());
     SgClassDeclaration* classDeclaration = buildClassDeclarationAndDefinition(structName,scope);
     vector<SgType*>::iterator typeIterator       = memberTypes.begin();
     vector<string>::iterator  memberNameIterator = memberNames.begin();
     while (typeIterator != memberTypes.end())
        {
       // printf ("Adding data member type = %s variable name = %s \n",(*typeIterator)->unparseToString().c_str(),memberNameIterator->c_str());
          SgVariableDeclaration* memberDeclaration = new SgVariableDeclaration(SOURCE_POSITION,*memberNameIterator,*typeIterator,NULL);
          memberDeclaration->set_endOfConstruct(SOURCE_POSITION);

          classDeclaration->get_definition()->append_member(memberDeclaration);

          memberDeclaration->set_parent(classDeclaration->get_definition());
        // Liao (2/13/2008) scope and symbols for member variables
          SgInitializedName* initializedName = *(memberDeclaration->get_variables().begin());
          initializedName->set_file_info(SOURCE_POSITION);
          initializedName->set_scope(classDeclaration->get_definition());

        // set nondefning declaration pointer
         memberDeclaration->set_firstNondefiningDeclaration(memberDeclaration);

         SgVariableSymbol* variableSymbol = new SgVariableSymbol(initializedName);
         classDeclaration->get_definition()->insert_symbol(*memberNameIterator,variableSymbol);

          typeIterator++;
          memberNameIterator++;
        }

     SgClassType* classType = new SgClassType(classDeclaration->get_firstNondefiningDeclaration());
     SgVariableDeclaration* variableDeclaration = new SgVariableDeclaration(SOURCE_POSITION,varName,classType,initializer);
     variableDeclaration->set_endOfConstruct(SOURCE_POSITION);

   //Liao (2/13/2008) scope and symbols for struct variable
     SgInitializedName* initializedName = *(variableDeclaration->get_variables().begin());
     initializedName->set_file_info(SOURCE_POSITION);
     initializedName->set_scope(scope);

     SgVariableSymbol* variableSymbol = new SgVariableSymbol(initializedName);
     scope->insert_symbol(varName,variableSymbol);

  //set nondefining declaration 
    variableDeclaration->set_firstNondefiningDeclaration(variableDeclaration);

  // This is required, since it is not set in the SgVariableDeclaration constructor
     initializer->set_parent(variableDeclaration);

     variableDeclaration->set_variableDeclarationContainsBaseTypeDefiningDeclaration(true);
     variableDeclaration->set_baseTypeDefiningDeclaration(classDeclaration->get_definingDeclaration());

     classDeclaration->set_parent(variableDeclaration);

     return variableDeclaration;
   }


// ******************************************
//              MAIN PROGRAM
// ******************************************
int
main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
     assert(project != NULL);

     vector<SgType*> memberTypes;
     vector<string>  memberNames;

     string name = "a";
     for (int i = 0; i < 10; i++)
        {
          memberTypes.push_back(SgTypeInt::createType());
          name = "_" + name;
          memberNames.push_back(name);
        }

  // Build the initializer
     SgExprListExp* initializerList = new SgExprListExp(SOURCE_POSITION);
     initializerList->set_endOfConstruct(SOURCE_POSITION);
     SgAggregateInitializer* structureInitializer = new SgAggregateInitializer(SOURCE_POSITION,initializerList);
     structureInitializer->set_endOfConstruct(SOURCE_POSITION);

  // Build the data member initializers for the structure (one SgAssignInitializer for each data member)
     for (unsigned int i = 0; i < memberNames.size(); i++)
        {
       // Set initial value to "i"
          SgIntVal* value = new SgIntVal(SOURCE_POSITION,i);
          value->set_endOfConstruct(SOURCE_POSITION);
          SgAssignInitializer* memberInitializer = new SgAssignInitializer(SOURCE_POSITION,value);
          memberInitializer->set_endOfConstruct(SOURCE_POSITION);
          structureInitializer->append_initializer(memberInitializer);
          memberInitializer->set_parent(structureInitializer);
        }

  // Access the first file and add a struct with data members specified
     SgSourceFile* file = isSgSourceFile((*project)[0]);
     ROSE_ASSERT(file != NULL);
     SgVariableDeclaration* variableDeclaration = buildStructVariable(file->get_globalScope(),memberTypes,memberNames,"X","x",structureInitializer);
     file->get_globalScope()->prepend_declaration(variableDeclaration);
     variableDeclaration->set_parent(file->get_globalScope());

    AstTests::runAllTests(project);
  // Code generation phase (write out new application "rose_<input file name>")
     return backend(project);
   }
