// tps : Switching from rose.h to sage3 changed size from 18 MB to 8,2MB
#include <sage3basic.h>
//#include <boost/lexical_cast.hpp>
#include "CallGraph.h"

#ifdef HAVE_SQLITE3

using namespace sqlite3x;
#endif


// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#if 0
/************
  **  Check to see if two member functions from two different classes have the same name
  **  and the argument types match.
  ************/
bool is_functions_equal(SgMemberFunctionDeclaration* f1, SgMemberFunctionDeclaration* f2)
{
  bool functions_are_equal = false;


  //See if the function types match
  if( f1->get_name() == f2->get_name() )
  {
    SgTypePtrList& args_f1 = f1->get_type()->get_arguments();
    SgTypePtrList& args_f2 = f2->get_type()->get_arguments();

    //See if the arguments match
    if( args_f1.size() == args_f2.size() )
    {
      functions_are_equal = true;

      for(int i = 0; i < args_f1.size(); i++)
      {
        if( args_f1[i]->get_mangled().str() != args_f2[i]->get_mangled().str()  )
        {
          functions_are_equal = false;

        }
      }
    }else
      functions_are_equal = false;

  }

  return functions_are_equal;
};
#endif

/***************************************************
 * Get the vector of base types for the current type
 **************************************************/
std::vector<SgType*>
get_type_vector(SgType* currentType )
{
  std::vector<SgType*> returnVector;
  currentType = currentType->stripType( SgType::STRIP_TYPEDEF_TYPE );

  SgModifierType*  modType     = NULL;
  SgPointerType*   pointType   = NULL;
  SgReferenceType* refType     = NULL;
  SgArrayType*     arrayType   = NULL;
  SgTypedefType*   typedefType = NULL;

  while (true)
  {
    returnVector.push_back(currentType);

    if ( (modType = isSgModifierType(currentType)) )
    {
      currentType = modType->get_base_type();
    }
    else if ( (refType = isSgReferenceType(currentType)) )
    {
      currentType = refType->get_base_type();
    }
    else if ( (pointType = isSgPointerType(currentType)) )
    {
      currentType = pointType->get_base_type();
    }
    else if ( (arrayType = isSgArrayType(currentType)) )
    {
      currentType = arrayType->get_base_type();
    }
    else if ( (typedefType = isSgTypedefType(currentType)) )
    {
      // DQ (6/21/2005): Added support for typedef types to be uncovered by findBaseType()

      currentType = typedefType->get_base_type();
    }
    else {
      // Exit the while(true){} loop!
      break;
    }
  }
               
  return returnVector;
};
bool is_functions_types_equal(SgFunctionType* f1, SgFunctionType* f2);

bool is_types_equal( SgType* t1, SgType* t2 )
{
  bool types_are_equal = true;
  if( t1 == t2 )
    return true;

  std::vector<SgType*> f1_vec = get_type_vector( t1  );
  std::vector<SgType*> f2_vec = get_type_vector( t2  );

  if( f1_vec.size() == f2_vec.size() )
  {
    for( size_t i = 0; i < f1_vec.size(); i++ )
    {
      if( f1_vec[i]->variantT() == f2_vec[i]->variantT() )
      {
        //The named types do not point to the same declaration
        if( isSgNamedType(f1_vec[i]) != NULL && 
            ( isSgNamedType(f1_vec[i])->get_declaration()->get_firstNondefiningDeclaration() != 
              isSgNamedType(f2_vec[i])->get_declaration()->get_firstNondefiningDeclaration()    
            )
          )
          types_are_equal = false;

        if(  isSgModifierType( f1_vec[i] ) != NULL  )
        {
          SgTypeModifier& typeModifier1   = isSgModifierType( f1_vec[i] )->get_typeModifier();
          SgTypeModifier& typeModifier2   = isSgModifierType( f2_vec[i] )->get_typeModifier();

          if( typeModifier1.get_modifierVector () != typeModifier2.get_modifierVector ()   )
            types_are_equal = false;

          if( typeModifier1.get_upcModifier().get_modifier () !=
              typeModifier2.get_upcModifier().get_modifier () 
            )
            types_are_equal = false;

          if( typeModifier1.get_constVolatileModifier().get_modifier () !=
              typeModifier2.get_constVolatileModifier().get_modifier () 
            )
            types_are_equal = false;

          if( typeModifier1.get_elaboratedTypeModifier().get_modifier () !=
              typeModifier2.get_elaboratedTypeModifier().get_modifier () 
            )
            types_are_equal = false;
        }


        //Function types are not the same
        if( isSgFunctionType( f1_vec[i] ) != NULL &&
            is_functions_types_equal(isSgFunctionType( f1_vec[i]), isSgFunctionType( f2_vec[i]))
          )
          types_are_equal = false;

      
      }else{
        //Variant is different
        types_are_equal = false;
      }

      if( types_are_equal == false )
        break;

    }

  }else{
    //The size of the type vectors are not the same
    types_are_equal = false;
  }

  return types_are_equal;
};

bool is_functions_types_equal(SgFunctionType* f1, SgFunctionType* f2)
{
  bool functions_are_equal = false;

  //Optimization: Function type objects are the same for functions that
  //have exactly the same signature
  if( f1 == f2 )
    return true;

  //See if the function types match
  if(  is_types_equal( f1->get_return_type(), f1->get_return_type() ) == true )
  {
    SgTypePtrList& args_f1 = f1->get_arguments();
    SgTypePtrList& args_f2 = f2->get_arguments();

    //See if the arguments match

    if( args_f1.size() == args_f2.size() )
    {
      functions_are_equal = true;

      for(size_t i = 0; i < args_f1.size(); i++)
      {
        if( is_types_equal( args_f1[i], args_f2[i] ) == false  )
        {
          functions_are_equal = false;
          break;
        }

      }
    }// Different number of arguments
  }

  //std::cout << "is_functions_types_equal: " << f1->unparseToString() << " " << f2->unparseToString() <<  ( functions_are_equal == true ? " true " : " false " ) << std::endl;

  return functions_are_equal;
};


bool 
dummyFilter::operator() (SgFunctionDeclaration* node) const{
  return true;
};



Properties* createEmptyProperty(SgFunctionDeclaration* funcDecl, SgType* type)
{
  Properties *fctProps = new Properties();
  fctProps->functionDeclaration = NULL;
  fctProps->functionType = NULL;
  fctProps->invokedClass = NULL;
  fctProps->isPointer = false;
  fctProps->isPolymorphic = false;

  fctProps->functionType        = type;
  fctProps->functionDeclaration = funcDecl;

  return fctProps;
}


  bool 
FunctionData::isDefined () 
{ 
  return hasDefinition; 
}


//Only used when SOLVE_FUNCTION_CALLS_IN_DB is defined
Properties::Properties(){
  functionType = NULL;
  functionDeclaration = NULL;
  invokedClass = NULL;
  isPolymorphic = isPointer = false;
  nid=label=type=scope=functionName="not-set";
};

Properties::Properties(SgFunctionDeclaration* inputFunctionDeclaration){
  functionDeclaration = inputFunctionDeclaration;
  ROSE_ASSERT( functionDeclaration != NULL );

  functionType = inputFunctionDeclaration->get_type()->findBaseType();
  invokedClass = NULL;
  isPointer = isPolymorphic = false;
  nid=label=type=scope=functionName="not-set";
}

//Only used when SOLVE_FUNCTION_CALLS_IN_DB is defined
Properties::Properties(Properties* prop){
  isPointer=prop->isPointer;
  isPolymorphic=prop->isPolymorphic;
  invokedClass=prop->invokedClass;
  functionDeclaration=prop->functionDeclaration;
  functionType=prop->functionType;

  nid=prop->nid;;
  label=prop->label;;
  type=prop->type;
  scope=prop->scope;
  functionName=prop->functionName;

  hasDef=prop->hasDef;
  isPtr=prop->isPtr;
  isPoly=prop->isPoly;
};

Properties::Properties(std::string p_nid, std::string p_label, std::string p_type, std::string p_scope,
    bool p_hasDef, bool p_isPtr, bool p_isPoly)
: nid(p_nid), label(p_label), type(p_type), scope(p_scope),hasDef(p_hasDef), isPtr(p_isPtr),
  isPoly(p_isPoly)
{
  functionType = NULL;
  functionDeclaration = NULL;
  invokedClass = NULL;
  isPolymorphic = isPointer = false;

  //Filter out the parameters from the nid to get the function name
  functionName = nid.substr(0,nid.size()-label.size());
};




CallGraphBuilder::CallGraphBuilder( SgProject *proj)
{
  project = proj;
  graph = NULL;
}

  SgIncidenceDirectedGraph*
CallGraphBuilder::getGraph() 
{ 
  return graph; 
}




Rose_STL_Container<SgFunctionDeclaration*> 
CallTargetSet::solveFunctionPointerCallsFunctional(SgNode* node, SgFunctionType* functionType ) 
{ 
  Rose_STL_Container<SgFunctionDeclaration*> functionList;

  SgFunctionDeclaration* fctDecl = isSgFunctionDeclaration(node);
  ROSE_ASSERT( fctDecl != NULL );
  //if ( functionType == fctDecl->get_type() )
  //Find all function declarations which is both first non-defining declaration and
  //has a mangled name which is equal to the mangled name of 'functionType'
  if( functionType->get_mangled().getString() == fctDecl->get_type()->get_mangled().getString() )
  {
    //ROSE_ASSERT( functionType->get_mangled().getString() == fctDecl->get_mangled().getString() );

    SgFunctionDeclaration *nonDefDecl =
      isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );

    //The ROSE AST normalizes functions so that there should be a nondef function decl for
    //every function
    //ROSE_ASSERT( nonDefDecl != NULL );
    if( fctDecl == nonDefDecl )
      functionList.push_back( nonDefDecl );
    else
      functionList.push_back( fctDecl );
  }//else
  //ROSE_ASSERT( functionType->get_mangled().getString() != fctDecl->get_type()->get_mangled().getString() );

  return functionList; 
}



  std::vector<Properties*>
CallTargetSet::solveFunctionPointerCall( SgPointerDerefExp *pointerDerefExp, SgProject *project )
{
  SgFunctionDeclarationPtrList functionList;

  SgFunctionType *fctType = isSgFunctionType( pointerDerefExp->get_type()->findBaseType() );
  ROSE_ASSERT ( fctType );
  ROSE_ASSERT ( project );
  // SgUnparse_Info ui;
  // string type1str = fctType->get_mangled( ui ).str();
  string type1str = fctType->get_mangled().str();
  // cout << "Return type of function pointer " << type1str << "\n";

  // cout << " Line: " << pointerDerefExp->get_file_info()->get_filenameString() <<  
  //  " l" << pointerDerefExp->get_file_info()->get_line() << 
  //  " c" << pointerDerefExp->get_file_info()->get_col()  << std::endl;
  // getting all possible functions with the same type
  // DQ (1/31/2006): Changed name and made global function type symbol table a static data member.
  // SgType *ty = Sgfunc_type_table.lookup_function_type( fctType->get_mangled( ui ) );
  ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
  // SgType *ty = SgNode::get_globalFunctionTypeTable()->lookup_function_type( fctType->get_mangled( ui ) );
  // ROSE_ASSERT ( ty->get_mangled( ui ) == type1str );

  // if there are multiple forward declarations of the same function
  // there will be multiple nodes in the AST containing them
  // but just one link in the call graph
  //  list<SgNode *> fctDeclarationList = NodeQuery::querySubTree( project, V_SgFunctionDeclaration );
  //AS (09/23/06) Query the memory pool instead of subtree of project
  //AS (10/2/06)  Modified query to only query for functions or function templates
  //VariantVector vv = V_SgFunctionDeclaration;
  VariantVector vv;
  vv.push_back(V_SgFunctionDeclaration);
  vv.push_back(V_SgTemplateInstantiationFunctionDecl);

  functionList =  AstQueryNamespace::queryMemoryPool(std::bind2nd(std::ptr_fun(solveFunctionPointerCallsFunctional), fctType), &vv );
  //std::cout << "The size of the list: " << functionList.size() << std::endl;

  std::vector<Properties*> returnProperties;
  for( SgFunctionDeclarationPtrList::iterator funcItr = functionList.begin();
      funcItr != functionList.end(); ++funcItr ) 
  {
    Properties* fctProps = createEmptyProperty( *funcItr, (*funcItr)->get_type() );
    returnProperties.push_back(fctProps);

  }


  return returnProperties;

}


  std::vector<Properties*>
CallTargetSet::solveMemberFunctionPointerCall ( SgExpression *functionExp, ClassHierarchyWrapper *classHierarchy )
{
  SgBinaryOp *binaryExp = isSgBinaryOp( functionExp );
  ROSE_ASSERT ( isSgArrowStarOp( binaryExp ) || isSgDotStarOp( binaryExp ) );

  SgExpression *left = NULL, *right = NULL;
  SgClassType *classType = NULL;
  SgClassDefinition *classDefinition = NULL;
  std::vector<Properties*> functionList;
  SgMemberFunctionType *memberFunctionType = NULL;

  left  = binaryExp->get_lhs_operand();
  right = binaryExp->get_rhs_operand();

  printf ("binaryExp = %p = %s \n",binaryExp,binaryExp->class_name().c_str());
  printf ("left  = %p = %s \n",left,left->class_name().c_str());
  printf ("right = %p = %s \n",right,right->class_name().c_str());

  // left side of the expression should have class type
  classType = isSgClassType( left->get_type()->findBaseType() );
  ROSE_ASSERT ( classType != NULL );
  printf ("classType->get_declaration() = %p = %s \n",classType->get_declaration(),classType->get_declaration()->class_name().c_str());

  // DQ (2/23/2006): bug fix
  // classDefinition = isSgClassDeclaration( classType->get_declaration() )->get_definition();
  ROSE_ASSERT ( classType->get_declaration() != NULL );
  ROSE_ASSERT ( classType->get_declaration()->get_definingDeclaration() != NULL );
  SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classType->get_declaration()->get_definingDeclaration());
  ROSE_ASSERT ( definingClassDeclaration != NULL );
  classDefinition = definingClassDeclaration->get_definition();
  ROSE_ASSERT ( classDefinition != NULL);

  // right side of the expression should have member function type
  memberFunctionType = isSgMemberFunctionType( right->get_type()->findBaseType() );
  ROSE_ASSERT( memberFunctionType );
  //std::cout << "The member function type: " << memberFunctionType->get_mangled().getString() << std::endl;
  //std::cout << "The member function type: " << functionExp->unparseToString() << std::endl;

  SgDeclarationStatementPtrList &allMembers = classDefinition->get_members();
  for ( SgDeclarationStatementPtrList::iterator it = allMembers.begin(); it != allMembers.end(); it++ )
  {
    SgMemberFunctionDeclaration *memberFunctionDeclaration = isSgMemberFunctionDeclaration( *it );
    if ( memberFunctionDeclaration )
    {
      SgMemberFunctionDeclaration *nonDefDecl = isSgMemberFunctionDeclaration( memberFunctionDeclaration->get_firstNondefiningDeclaration() );
      if ( nonDefDecl )
        memberFunctionDeclaration = nonDefDecl;

      //FIXME: Make this use the is_functions_types_equal function
  //    if ( is_functions_types_equal(isSgMemberFunctionType(memberFunctionDeclaration->get_type()) , memberFunctionType) )
   if ( memberFunctionDeclaration->get_type()->unparseToString() == memberFunctionType->unparseToString() )

      {
        if ( !(  memberFunctionDeclaration->get_functionModifier().isPureVirtual() ) )
        {
          Properties* fctProps = createEmptyProperty(memberFunctionDeclaration,
              memberFunctionDeclaration->get_type());

          fctProps->invokedClass = memberFunctionDeclaration->get_class_scope();


          functionList.push_back( fctProps );
        }
        // cout << "PUSHING " << memberFunctionDeclaration << "\n";
      }

      // for virtual functions in polymorphic calls, we need to search down in the hierarchy of classes
      // and retrieve all declarations of member functions with the same type
      if ( ( memberFunctionDeclaration->get_functionModifier().isVirtual() ||
            memberFunctionDeclaration->get_functionModifier().isPureVirtual() ) && !isSgThisExp( left ) )
      {
        SgClassDefinitionPtrList subclasses = classHierarchy->getSubclasses( classDefinition );
        cout << "Virtual function " << memberFunctionDeclaration->get_mangled_name().str() << "\n";
        for ( SgClassDefinitionPtrList::iterator it_cls = subclasses.begin(); it_cls != subclasses.end(); it_cls++ )
        {
          SgClassDefinition *cls = isSgClassDefinition( *it_cls );
          SgDeclarationStatementPtrList &clsMembers = cls->get_members();

          for ( SgDeclarationStatementPtrList::iterator it_cls_mb = clsMembers.begin(); it_cls_mb != clsMembers.end(); it_cls_mb++ )
          {
            SgMemberFunctionDeclaration *cls_mb_decl = isSgMemberFunctionDeclaration( *it_cls_mb );
           
            if ( is_functions_types_equal(isSgMemberFunctionType(memberFunctionDeclaration->get_type()) , isSgMemberFunctionType(cls_mb_decl->get_type()) ) )
            {
              SgMemberFunctionDeclaration *nonDefDecl = isSgMemberFunctionDeclaration( cls_mb_decl->get_firstNondefiningDeclaration() );
              SgMemberFunctionDeclaration *defDecl = isSgMemberFunctionDeclaration( cls_mb_decl->get_definingDeclaration() );
              ROSE_ASSERT ( (!nonDefDecl && defDecl == cls_mb_decl) || (nonDefDecl == cls_mb_decl && nonDefDecl) );

              if ( nonDefDecl )
              {
                if ( !( nonDefDecl->get_functionModifier().isPureVirtual() ) && nonDefDecl->get_functionModifier().isVirtual() ){
                  Properties* fctProps = createEmptyProperty( nonDefDecl,
                      nonDefDecl->get_type() );
                  fctProps->invokedClass = nonDefDecl->get_class_scope();

                  functionList.push_back( fctProps );
                }
              }
              else
                if ( !( defDecl->get_functionModifier().isPureVirtual() ) && defDecl->get_functionModifier().isVirtual() )
                {
                  Properties* fctProps = createEmptyProperty(defDecl, defDecl->get_type() );
                  fctProps->invokedClass = defDecl->get_class_scope();

                  functionList.push_back( fctProps ); // == cls_mb_decl
                }
            }
          }
        }
      }
    }
  }

  cout << "Function list size: " << functionList.size() << "\n";
  return functionList;
}

  std::vector<Properties*>
CallTargetSet::solveMemberFunctionCall( SgClassType *crtClass, ClassHierarchyWrapper *classHierarchy,
    SgMemberFunctionDeclaration *memberFunctionDeclaration, bool polymorphic )
{
  std::vector<Properties*> functionList;
  ROSE_ASSERT ( memberFunctionDeclaration && classHierarchy );
  //  memberFunctionDeclaration->get_file_info()->display( "Member function we are considering" );

  SgDeclarationStatement *nonDefDeclInClass = NULL;
  nonDefDeclInClass = memberFunctionDeclaration->get_firstNondefiningDeclaration();
  SgMemberFunctionDeclaration *functionDeclarationInClass = NULL;

  // memberFunctionDeclaration is outside the class
  if ( nonDefDeclInClass )
  {
    //      nonDefDeclInClass->get_file_info()->display( "found nondefining" );
    functionDeclarationInClass = isSgMemberFunctionDeclaration( nonDefDeclInClass );
  }
  // in class declaration, since there is no non-defining declaration
  else
  {
    functionDeclarationInClass = memberFunctionDeclaration;
    //      functionDeclarationInClass->get_file_info()->display("declaration in class already");
  }

  ROSE_ASSERT ( functionDeclarationInClass );
  // we need the inclass declaration so we can determine if it is a virtual function
  if ( functionDeclarationInClass->get_functionModifier().isVirtual() && polymorphic )
  {
    SgFunctionDefinition *functionDefinition = memberFunctionDeclaration->get_definition();

    // if it's not pure virtual then
    // the current function declaration is a candidate function to be called
    if ( functionDefinition )
    {

      Properties* fctProps = createEmptyProperty( functionDeclarationInClass, 
          functionDeclarationInClass->get_type() );

      functionList.push_back( fctProps );

    }
    else
      functionDeclarationInClass->get_file_info()->display( "Pure virtual function found" );

    // search down the class hierarchy to get
    // all redeclarations of the current member function
    // which may be the ones being called via polymorphism
    SgClassDefinition *crtClsDef = NULL;
    // selecting the root of the hierarchy
    if ( crtClass )
    {
      SgClassDeclaration *tmp = isSgClassDeclaration( crtClass->get_declaration() );
      ROSE_ASSERT ( tmp );
      SgClassDeclaration* tmp2 = isSgClassDeclaration(tmp->get_definingDeclaration());
      ROSE_ASSERT (tmp2);
      crtClsDef = tmp2->get_definition();
      ROSE_ASSERT ( crtClsDef );
    }
    else
    {
      crtClsDef = isSgClassDefinition( memberFunctionDeclaration->get_scope() );
      ROSE_ASSERT ( crtClsDef );
    }

    // for virtual functions, we need to search down in the hierarchy of classes
    // and retrieve all declarations of member functions with the same type
    SgClassDefinitionPtrList subclasses = classHierarchy->getSubclasses( crtClsDef );
    functionDeclarationInClass = NULL;
    string f1 = memberFunctionDeclaration->get_mangled_name().str();
    string f2;
    for ( SgClassDefinitionPtrList::iterator it_cls = subclasses.begin(); it_cls != subclasses.end(); it_cls++ )
    {
      SgClassDefinition *cls = isSgClassDefinition( *it_cls );
      SgDeclarationStatementPtrList &clsMembers = cls->get_members();
      for ( SgDeclarationStatementPtrList::iterator it_cls_mb = clsMembers.begin(); it_cls_mb != clsMembers.end(); it_cls_mb++ )
      {
        SgMemberFunctionDeclaration *cls_mb_decl = isSgMemberFunctionDeclaration( *it_cls_mb );
        if (cls_mb_decl == NULL) continue;

        ROSE_ASSERT(cls_mb_decl != NULL);
        SgMemberFunctionType* funcType1 = isSgMemberFunctionType(memberFunctionDeclaration->get_type());
        SgMemberFunctionType* funcType2 = isSgMemberFunctionType(cls_mb_decl->get_type());
        if (funcType1 == NULL || funcType2 == NULL) continue;
        if ( is_functions_types_equal(funcType1, funcType2) )
        {
          SgMemberFunctionDeclaration *nonDefDecl =
            isSgMemberFunctionDeclaration( cls_mb_decl->get_firstNondefiningDeclaration() );
          SgMemberFunctionDeclaration *defDecl =
            isSgMemberFunctionDeclaration( cls_mb_decl->get_definingDeclaration() );

          // MD 2010/07/08 defDecl might be NULL
          // ROSE_ASSERT ( (!nonDefDecl && defDecl == cls_mb_decl) || (nonDefDecl == cls_mb_decl && nonDefDecl) );
          
          functionDeclarationInClass = (nonDefDecl) ? nonDefDecl : defDecl;
          ROSE_ASSERT ( functionDeclarationInClass );
          if ( !( functionDeclarationInClass->get_functionModifier().isPureVirtual() ) )
          {
            Properties* fctProps = createEmptyProperty( functionDeclarationInClass,
                functionDeclarationInClass->get_type() );

            functionList.push_back( fctProps );
          }
        }
      }
    }
  } // end if virtual
  // non virtual (standard) member function or call not polymorphic (or both)
  else
#if 0
    if ( functionDeclarationInClass->get_declarationModifier().get_storageModifier().isStatic() )
    {
      cout << "Found static function declaration called as member function " << functionDeclarationInClass << "\n";
      functionDeclarationInClass->get_file_info()->display("Error");
      ROSE_ASSERT(false);
    }
    else
#endif
    {
      // always pushing the in-class declaration, so we need to find that one
      SgDeclarationStatement *nonDefDeclInClass = NULL;
      nonDefDeclInClass = memberFunctionDeclaration->get_firstNondefiningDeclaration();
      SgMemberFunctionDeclaration *functionDeclarationInClass = NULL;

      // memberFunctionDeclaration is outside the class
      if ( nonDefDeclInClass )
        functionDeclarationInClass = isSgMemberFunctionDeclaration( nonDefDeclInClass );
      // in class declaration, since there is no non-defining declaration
      else
        functionDeclarationInClass = memberFunctionDeclaration;

      ROSE_ASSERT ( functionDeclarationInClass );
      //      			   cout << "Pushing non-virtual function declaration for function "
      //<< functionDeclarationInClass->get_name().str() << "   " << functionDeclarationInClass << "\n";
      Properties* fctProps = createEmptyProperty(functionDeclarationInClass,
          functionDeclarationInClass->get_type() );
      functionList.push_back( fctProps );

    }
  return functionList;
}



Rose_STL_Container<SgFunctionDeclaration*> solveFunctionPointerCallsFunctional(SgNode* node, SgFunctionType* functionType ) 
{ 
  Rose_STL_Container<SgFunctionDeclaration*> functionList;

  SgFunctionDeclaration* fctDecl = isSgFunctionDeclaration(node);
  ROSE_ASSERT( fctDecl != NULL );
  //if ( functionType == fctDecl->get_type() )
  //Find all function declarations which is both first non-defining declaration and
  //has a mangled name which is equal to the mangled name of 'functionType'
  if( functionType->get_mangled().getString() == fctDecl->get_type()->get_mangled().getString() )
  {
    //ROSE_ASSERT( functionType->get_mangled().getString() == fctDecl->get_mangled().getString() );

    SgFunctionDeclaration *nonDefDecl =
      isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );

    //The ROSE AST normalizes functions so that there should be a nondef function decl for
    //every function
    ROSE_ASSERT( nonDefDecl != NULL );
    if( fctDecl == nonDefDecl )
      functionList.push_back( nonDefDecl );
  }//else
  //ROSE_ASSERT( functionType->get_mangled().getString() != fctDecl->get_type()->get_mangled().getString() );

  return functionList; 
}

void 
CallTargetSet::getCallExpsForFunctionDefinition(SgFunctionDefinition* def, 
                                      Rose_STL_Container<SgFunctionCallExp*>& calls) {
  VariantVector vv(V_SgFunctionCallExp);
  Rose_STL_Container<SgNode*> returnSites = NodeQuery::queryMemoryPool(vv);
  Rose_STL_Container<SgNode*>::iterator site;
  for (site = returnSites.begin(); site != returnSites.end(); ++site) { 
    SgFunctionCallExp* callexp = isSgFunctionCallExp(*site);
    SgFunctionDeclaration* decl = callexp->getAssociatedFunctionDeclaration();
    if (decl == NULL) continue;
    SgFunctionDeclaration* defDecl = isSgFunctionDeclaration(decl->get_definingDeclaration());
    if (defDecl == NULL) continue;
    SgFunctionDefinition* candidateDef = defDecl->get_definition();
    if (candidateDef == def) calls.push_back(callexp);
  }

#if 0
     SgFunctionType* targetType = def->get_declaration()->get_type();
     
       if (decl == NULL) { // function pointer. match types
         SgExpression* candidate = callexp->get_function();
         SgFunctionType* candidateType = isSgFunctionType(candidate->get_type());
         //if (targetType == candidateType)
           //makeEdge(CFGNode(this, idx), (*site)->cfgForEnd(), result);
       } 
       else {
         SgFunctionDeclaration* defDecl = isSgFunctionDeclaration(decl->get_definingDeclaration());
         if (defDecl == NULL) { // virtual function. use class heirarchy
           std::cerr << "funCallExp not implemented for virtual functions" << std::endl;
         }
         else { // statically resolved function. use get_definition
           //makeEdge(CFGNode(this, idx), (*site)->cfgForEnd(), result);
         }
       }
     }
#endif
}

void 
CallTargetSet::getFunctionDefinitionsForCallExp(SgFunctionCallExp* call, 
                                      Rose_STL_Container<SgFunctionDefinition*>& defs) {
  
    SgFunctionDeclaration* decl = call->getAssociatedFunctionDeclaration();
    if (decl == NULL) return;
    SgFunctionDeclaration* defDecl = isSgFunctionDeclaration(decl->get_definingDeclaration());
    if (defDecl == NULL) return;
    SgFunctionDefinition* candidateDef = defDecl->get_definition();
    if (candidateDef != NULL) defs.push_back(candidateDef);

#if 0
    ClassHierarchyWrapper classHierarchy(SageInterface::getProject());
    Rose_STL_Container<Properties*> functionList;
    CallTargetSet::retrieveFunctionDeclarations(this, &classHierarchy, functionList);
    Rose_STL_Container<Properties*>::iterator prop;
    for (prop = functionList.begin(); prop != functionList.end(); prop++) {
      SgFunctionDeclaration* funcDecl = (*prop)->functionDeclaration;
      ROSE_ASSERT(funcDecl);
      SgFunctionDeclaration* decl = isSgFunctionDeclaration(funcDecl->get_definingDeclaration());
      if (decl == NULL) {
        // Causes excessive output for includes. 
        // std::cerr << "warning: no definition for " << funcDecl->get_qualified_name().str() << std::endl;
        continue;
      }
      SgFunctionDefinition* def = decl->get_definition();
      if (def == NULL) 
        std::cerr << "no definition for function in SgFunctionCallExp::cfgOutEdges: " << decl->get_name().str() << std::endl;
      else
        makeEdge(CFGNode(this, idx), def->cfgForBeginning(), result);
    }
}
else
makeEdge(CFGNode(this, idx), CFGNode(this, 3), result);
#endif
}

// Add the declaration for functionCallExp to functionList. In the case of 
// function pointers and virtual functions, append the set of declarations
// to functionList. 
void 
CallTargetSet::retrieveFunctionDeclarations(SgFunctionCallExp* functionCallExp, 
                         ClassHierarchyWrapper* classHierarchy,
                         Rose_STL_Container<Properties *>& functionList) {

  SgExpression* functionExp = functionCallExp->get_function();
  ROSE_ASSERT ( functionExp != NULL );

  switch ( functionExp->variantT() )
  {
    case V_SgArrowStarOp:
    case V_SgDotStarOp:
      {
        std::vector<Properties*> fD =
          CallTargetSet::solveMemberFunctionPointerCall( functionExp, classHierarchy );
        for (std::vector<Properties*>::iterator it = fD.begin(); it != fD.end(); it++ )
        {
          //			 ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
          ROSE_ASSERT((*it)->functionType ); functionList.push_back( *it );
          //                         if( (*it)-> )
          //                        std::cout << "Found member function Pointer Call " << (*it)->unparseToString() << std::endl;
        }
      }
      break;
    case V_SgDotExp:
    case V_SgArrowExp:
      {
        SgMemberFunctionDeclaration *memberFunctionDeclaration = NULL;
        SgClassType *crtClass = NULL;
        ROSE_ASSERT ( isSgBinaryOp( functionExp ) );

        SgExpression *leftSide = isSgBinaryOp( functionExp )->get_lhs_operand();
        SgType *leftType = leftSide->get_type()->findBaseType();
        crtClass = isSgClassType( leftType );

        if (SgMemberFunctionRefExp *memberFunctionRefExp =
            isSgMemberFunctionRefExp( isSgBinaryOp( functionExp )->get_rhs_operand() ) )
        {

          //AS(122805) In the case of a constructor initializer it is possible that a call to a constructor initializer may
          //return a type corresponding to an operator some-type() declared within the constructed class. An example is:
          //   struct Foo {
          //      operator  bool () const
          //          { return true; }
          //   };
          //
          //   struct Bar {
          //      bool foobar()
          //          { return Foo (); }
          //   };
          //where the call to the constructor of the class Foo will cause a call to the operator bool(), where bool corresponds
          //type of the member function foobar declared within Bar.


          if(isSgConstructorInitializer(leftSide)!= NULL)
          {
            SgClassDeclaration* constInit = isSgConstructorInitializer(leftSide)->get_class_decl();

            //ROSE_ASSERT(constInit!=NULL);
            if(constInit)
              crtClass = constInit->get_type();
            else{
              //AS(010306) A compiler constructed SgConstructorInitializer may wrap a function call which return a class type.
              //In an dot or arrow expression this returned class type may be used as an expression left hand side. To handle
              //this case the returned class type must be extracted from the expression list. An example demonstrating this is:
              //class Vector3d {
              //   public:
              //    Vector3d(){};
              //    Vector3d(const Vector3d &vector3d){};
              //   Vector3d     cross() const
              //        { return Vector3d();};
              //   void   GetZ(){};
              //};
              //void foo(){
              //  Vector3d vn1;
              //  (vn1.cross()).GetZ();
              //}

              SgExprListExp* expLst = isSgExprListExp(isSgConstructorInitializer(leftSide)->get_args());
              ROSE_ASSERT(expLst!=NULL);
              ROSE_ASSERT(expLst->get_expressions().size()==1);
              SgClassType* lhsClassType = isSgClassType(isSgFunctionCallExp(*expLst->get_expressions().begin())->get_type()->stripType(SgType::STRIP_TYPEDEF_TYPE) );
              //                        std::cout << "expLst:" << expLst->unparseToString() << " " << (*expLst->get_expressions().begin())->get_type(SgType::STRIP_TYPEDEF_TYPE)->class_name()  << std::endl;
              crtClass = lhsClassType;
            }

            ROSE_ASSERT(crtClass!=NULL);
          }

          memberFunctionDeclaration =
            isSgMemberFunctionDeclaration( memberFunctionRefExp->get_symbol()->get_declaration() );
          ROSE_ASSERT ( memberFunctionDeclaration && crtClass );

          //Set function to first non-defining declaration
          SgMemberFunctionDeclaration *nonDefDecl =
            isSgMemberFunctionDeclaration( memberFunctionDeclaration->get_firstNondefiningDeclaration() );
          if ( nonDefDecl )
            memberFunctionDeclaration = nonDefDecl;

          //Construct the Properties

          Properties* fctProps = createEmptyProperty(memberFunctionDeclaration, memberFunctionDeclaration->get_type()->findBaseType());

          ROSE_ASSERT ( isSgFunctionDeclaration( fctProps->functionDeclaration ) );
          ROSE_ASSERT ( fctProps->functionType );

          if ( !( isSgThisExp( leftSide ) || !( memberFunctionDeclaration->get_functionModifier().isVirtual() ) ) )
          {
            fctProps->isPolymorphic = true;
            fctProps->invokedClass = isSgClassDeclaration( crtClass->get_declaration()->get_definingDeclaration() )->get_definition();
            ROSE_ASSERT(fctProps->invokedClass!=NULL);
            //cout << "SET polymorphic on class " << fctProps->invokedClass->get_qualified_name().getString()
            //  << "\t" << fctProps << "\n";
          }

          //functionList.push_back( fctProps );

          // returns the list of all in-class declarations of functions potentially called
          // ( may be several because of polymorphism )
          bool polymorphic = false;
          if ( !isSgThisExp( leftSide ) )
            polymorphic = true;

          std::vector<Properties*> fD =
            CallTargetSet::solveMemberFunctionCall( crtClass, classHierarchy,
                memberFunctionDeclaration, polymorphic );
          for ( std::vector<Properties*>::iterator it = fD.begin(); it != fD.end(); it++ )
          {
            ROSE_ASSERT((*it)->functionType ); 
            functionList.push_back( *it );
            //std::cout << "Found member function Call " << (*it)->unparseToString() << std::endl;
          }
        }
      }
      break;
    case V_SgPointerDerefExp:
      {
        std::vector<Properties*> fD =
          CallTargetSet::solveFunctionPointerCall( isSgPointerDerefExp( functionExp ), SageInterface::getProject());
        for ( std::vector<Properties*>::iterator it = fD.begin(); it != fD.end(); it++ )
        {
          //                       ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
          functionList.push_back( *it );
        }
        /*
        //FIXME: Is the next sentence needed?
        Properties* fctProps = createEmptyProperty( NULL, 
        isSgFunctionType( isSgPointerDerefExp( functionExp )->get_type()->findBaseType() )  );

        fctProps->isPointer = true;
        ROSE_ASSERT ( fctProps->functionType );

        functionList.push_back( fctProps );
        */
      }
      break;
    case V_SgMemberFunctionRefExp:
    case V_SgFunctionRefExp:
      {
        SgFunctionDeclaration *fctDecl =
          isSgFunctionRefExp( functionExp ) ?
          isSgFunctionDeclaration( isSgFunctionRefExp( functionExp )->get_symbol()->get_declaration() )
          : isSgFunctionDeclaration( isSgMemberFunctionRefExp( functionExp )->get_symbol()->get_declaration() )
          ;
        ROSE_ASSERT ( fctDecl );
        SgFunctionDeclaration *nonDefDecl =
          isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );

        //Constrution Function Props
        if ( nonDefDecl )
          fctDecl = nonDefDecl;

        Properties* fctProps = createEmptyProperty(fctDecl,
            fctDecl->get_type()->findBaseType() );

        ROSE_ASSERT ( isSgFunctionDeclaration( fctProps->functionDeclaration ) );
        ROSE_ASSERT(fctProps->functionType ); 
        ROSE_ASSERT(fctProps->functionDeclaration != NULL);
        functionList.push_back( fctProps );
      }
      break;
    default:
      {
        cout << "Error, unexpected type of functionRefExp: " << functionExp->sage_class_name() << "!!!\n";
        ROSE_ASSERT ( false );
      }
  }
}

FunctionData::FunctionData ( SgFunctionDeclaration* inputFunctionDeclaration,
    SgProject *project, ClassHierarchyWrapper *classHierarchy )
{
  hasDefinition = false;

  functionDeclaration = inputFunctionDeclaration;
  properties = new Properties(inputFunctionDeclaration);
  SgFunctionDeclaration *defDecl =
    (
     inputFunctionDeclaration->get_definition() != NULL ? 
     inputFunctionDeclaration : isSgFunctionDeclaration( properties->functionDeclaration->get_definingDeclaration() )
    );

  if( inputFunctionDeclaration == defDecl )
    std::cout << " **** If you see this error message. Report to the ROSE team that a function declaration ****\n"
      << " **** has the defining declaration erroneously attached to the nondef decl               ****\n";

  if(defDecl != NULL  && defDecl->get_definition() == NULL)
  {
    defDecl = NULL;
    std::cout << " **** If you see this error message. Report to the ROSE team that a function declaration ****\n"
      << " **** has a defining declaration but no definition                                       ****\n";

  }


  //cout << "!!!" << inputFunctionDeclaration->get_name().str() << " has definition " << defDecl << "\n";
  //     cout << "Input declaration: " << inputFunctionDeclaration << " as opposed to " << functionDeclaration << "\n"; 

  // Test for a forward declaration (declaration without a definition)
  if ( defDecl )
  {
    SgFunctionDefinition* functionDefinition = defDecl->get_definition();
    ROSE_ASSERT ( defDecl );
    ROSE_ASSERT ( functionDefinition != NULL );
    hasDefinition = true;
    Rose_STL_Container<SgNode*> functionCallExpList;
    functionCallExpList = NodeQuery::querySubTree ( functionDefinition, V_SgFunctionCallExp );

    // printf ("functionCallExpList.size() = %zu \n",functionCallExpList.size());

    // list<SgFunctionDeclaration*> functionList;
    Rose_STL_Container<SgNode*>::iterator i = functionCallExpList.begin();

    // for all functions getting called in the body of the current function
    // we need to get their declarations, or the set of declarations for
    // function pointers and virtual functions
    while (i != functionCallExpList.end())
    {
      SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(*i);
      ROSE_ASSERT ( functionCallExp != NULL );
      CallTargetSet::retrieveFunctionDeclarations(functionCallExp, classHierarchy, functionList);
      i++;
    }
  }
}




  SgGraphNode*
findNode(SgGraph* graph, std::string nid)
{
// CH (4/16/2010): Use boost:unordered instead
//#ifdef _MSC_VER
#if 0
//#pragma message ("WARNING: commenting out use of hashmap in graph support for MSVC.")
    return NULL;
#else
  const rose_graph_string_integer_hash_multimap& nidToInt  = graph->get_string_to_node_index_multimap();
  const rose_graph_integer_node_hash_map &       intToNode = graph->get_node_index_to_node_map ();

  rose_graph_string_integer_hash_multimap::const_iterator iItr = nidToInt.find(nid);

  if( iItr != nidToInt.end() )
  {
    int n = iItr->second;
#if 1

    iItr++;

    int i = 0;
    for(;iItr != nidToInt.end(); ++iItr )
    {
      i++;
    };

    //The nid should match an unique int
    if( iItr != nidToInt.end() )
    {
      std::cout << "Error: nid " << nid << " occurs more than once. " << i << std::endl; 
    };

    ROSE_ASSERT( iItr == nidToInt.end() );
#endif

    return intToNode.find(n)->second;
  }else
    return NULL;
#endif
};


//Iterate over all edges in graph until an edge from->to is found. If not such edge
//exists return NULL
  SgGraphEdge*
findEdge (SgIncidenceDirectedGraph* graph, SgGraphNode* from, SgGraphNode* to)
{
  const rose_graph_integer_edge_hash_multimap & outEdges
    = graph->get_node_index_to_edge_multimap_edgesOut ();

  rose_graph_integer_edge_hash_multimap::const_iterator it = outEdges.find(from->get_index());

  for (;it!=outEdges.end();++it) {
    SgDirectedGraphEdge* graphEdge = isSgDirectedGraphEdge(it->second);
    ROSE_ASSERT(graphEdge!=NULL);
    if(graphEdge->get_from() == from && graphEdge->get_to() == to)
      return graphEdge;
  }


  return NULL;
};


  SgGraphNode*
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, SgFunctionDeclaration* functionDeclaration)
{
  Rose_STL_Container<SgGraphNode*>::iterator k = nodeList.begin();

  SgGraphNode* returnNode = NULL;

  while ( k != nodeList.end() )
  {

    if ( (*k)->get_SgNode() == functionDeclaration )
    {
      returnNode = *k;
      break;
    }
    k++;
  }

  if ( !returnNode )
    cout << "NO node found for " << functionDeclaration->get_name().str() << " " << functionDeclaration << "\n";
  // ROSE_ASSERT (returnNode != NULL);
  return returnNode;
}

  SgGraphNode*
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, string name )
{
  Rose_STL_Container<SgGraphNode*>::iterator k = nodeList.begin();

  SgGraphNode* returnNode = NULL;

  bool found = false;
  while ( !found && (k != nodeList.end()) )
  {
    if ((*k)->get_name() == name)
    {
      returnNode = *k;
      found = true;
    }
    k++;
  }

  //ROSE_ASSERT (returnNode != NULL);
  if ( !returnNode )
    cout << "No node found for " << name << "\n";
  return returnNode;
}


void
CallGraphBuilder::buildCallGraph (){
  buildCallGraph(dummyFilter());
}



  GetOneFuncDeclarationPerFunction::result_type 
GetOneFuncDeclarationPerFunction::operator()(SgNode* node )
{
  result_type returnType;
  SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node);
  if(funcDecl != NULL)
  {
    if( funcDecl->get_definingDeclaration () != NULL && node == funcDecl->get_definingDeclaration ())
      returnType.push_back(node);
    if( funcDecl->get_definingDeclaration () == NULL && node == funcDecl->get_firstNondefiningDeclaration () )
      returnType.push_back(node);
  }
  return returnType;
}

