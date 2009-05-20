
#include <rose.h>
#include <boost/lexical_cast.hpp>
#include "CallGraph.h"

#ifdef HAVE_SQLITE3
using namespace sqlite3x;
#endif

bool var_SOLVE_FUNCTION_CALLS_IN_DB = false;

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

bool 
dummyFilter::operator() (SgFunctionDeclaration* node) const{
  return true;
};




bool 
CallGraphNode::isDefined () 
 { 
   return hasDefinition; 
 }

void 
CallGraphNode::Dump() const 
{ 
  
   printf ("NODE: function declaration = %p label = %s \n", (var_SOLVE_FUNCTION_CALLS_IN_DB == true ? properties->functionDeclaration: functionDeclaration ) ,label.c_str());

}

std::string 
CallGraphNode::toString() const 
 { 
   return label;
 }

std::string 
CallGraphEdge::toString() const 
 { 
   return label;
 }

void 
CallGraphEdge::Dump() const 
 { 
   printf ("EDGE: label = %s \n",label.c_str()); 
 }

CallGraphEdge::CallGraphEdge ( std::string label ) : MultiGraphElem( NULL ), label( label )
 {
   if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
     properties = new FunctionProperties();

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
};




CallGraphNode::CallGraphNode ( std::string label, SgFunctionDeclaration* fctDeclaration, SgType *ty,
	        bool hasDef, bool isPtr, bool isPoly, SgClassDefinition *invokedCls )
	   : MultiGraphElem( NULL ), label( label )
  {
  hasDefinition = hasDef;
  properties = new FunctionProperties();
  properties->isPointer = isPtr;;
  properties->isPolymorphic = isPoly;
  properties->invokedClass = invokedCls;
  if ( properties->isPolymorphic )
    cout << "IsPolymorphic is set to true on "
         << properties->invokedClass->get_qualified_name().getString() << "\n";
  properties->functionDeclaration = fctDeclaration;
  properties->functionType = ty;
  if ( properties->functionDeclaration != NULL )
    {
       ROSE_ASSERT ( isSgFunctionDeclaration( properties->functionDeclaration ) );
       ROSE_ASSERT ( ty && ty == properties->functionDeclaration->get_type()->findBaseType() );
     }
 }

CallGraphNode::CallGraphNode ( std::string label, FunctionProperties *fctProps, bool hasDef ) : MultiGraphElem ( NULL )
 {
   this->label = label;
   properties = fctProps;
   hasDefinition = hasDef;
 }



CallGraphNode::CallGraphNode ( std::string label, SgFunctionDeclaration* fctDeclaration, bool hasDef )
	   : MultiGraphElem(NULL), label(label), functionDeclaration(fctDeclaration) {
   hasDefinition = hasDef;

 }



CallGraphBuilder::CallGraphBuilder( SgProject *proj )
  {
    project = proj;
    graph = NULL;
  }

CallGraphCreate*
CallGraphBuilder::getGraph() 
  { 
    return graph; 
  }

CallGraphDotOutput::CallGraphDotOutput( CallGraphCreate & graph ) : GraphDotOutput<CallGraphCreate>(graph), callGraph(graph) 
  {}


namespace CallTargetSet
{


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
			//ROSE_ASSERT( nonDefDecl != NULL );
			if( fctDecl == nonDefDecl )
				functionList.push_back( nonDefDecl );
			else
			    functionList.push_back( fctDecl );
		}//else
		//ROSE_ASSERT( functionType->get_mangled().getString() != fctDecl->get_type()->get_mangled().getString() );

		return functionList; 
	}



	SgFunctionDeclarationPtrList
		solveFunctionPointerCall( SgPointerDerefExp *pointerDerefExp, SgProject *project )
		{
			SgFunctionDeclarationPtrList functionList;

			SgFunctionType *fctType = isSgFunctionType( pointerDerefExp->get_type()->findBaseType() );
			ROSE_ASSERT ( fctType );
			ROSE_ASSERT ( project );
			// SgUnparse_Info ui;
			// string type1str = fctType->get_mangled( ui ).str();
			string type1str = fctType->get_mangled().str();
			cout << "Return type of function pointer " << type1str << "\n";

			cout << " Line: " << pointerDerefExp->get_file_info()->get_filenameString() <<  
				" l" << pointerDerefExp->get_file_info()->get_line() << 
				" c" << pointerDerefExp->get_file_info()->get_col()  << std::endl;
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
			std::cout << "The size of the list: " << functionList.size() << std::endl;
			return functionList;

		}
}


namespace CallTargetSet
{
SgFunctionDeclarationPtrList
solveMemberFunctionPointerCall ( SgExpression *functionExp, ClassHierarchyWrapper *classHierarchy )
   {
     SgBinaryOp *binaryExp = isSgBinaryOp( functionExp );
     ROSE_ASSERT ( isSgArrowStarOp( binaryExp ) || isSgDotStarOp( binaryExp ) );//|| isSgArrowExp ( binaryExp ) || isSgDotExp ( binaryExp ) );

     SgExpression *left = NULL, *right = NULL;
     SgClassType *classType = NULL;
     SgClassDefinition *classDefinition = NULL;
     SgFunctionDeclarationPtrList functionList;
     string type1str;
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
     type1str = memberFunctionType->get_mangled().getString();

     SgDeclarationStatementPtrList &allMembers = classDefinition->get_members();
     for ( SgDeclarationStatementPtrList::iterator it = allMembers.begin(); it != allMembers.end(); it++ )
        {
          SgMemberFunctionDeclaration *memberFunctionDeclaration = isSgMemberFunctionDeclaration( *it );
          if ( memberFunctionDeclaration )
             {
               SgMemberFunctionDeclaration *nonDefDecl = isSgMemberFunctionDeclaration( memberFunctionDeclaration->get_firstNondefiningDeclaration() );
               if ( nonDefDecl )
                    memberFunctionDeclaration = nonDefDecl;

               SgType *possibleType = memberFunctionDeclaration->get_type();
               string type2str = possibleType->get_mangled().getString();
               if ( type1str == type2str )
                  {
                    if ( !(  memberFunctionDeclaration->get_functionModifier().isPureVirtual() ) )
                         functionList.push_back( memberFunctionDeclaration );
                 // cout << "PUSHING " << memberFunctionDeclaration << "\n";
                  }

            // for virtual functions in polymorphic calls, we need to search down in the hierarchy of classes
            // and retrieve all declarations of member functions with the same type
               if ( ( memberFunctionDeclaration->get_functionModifier().isVirtual() ||
                      memberFunctionDeclaration->get_functionModifier().isPureVirtual() ) && !isSgThisExp( left ) )
                  {
                    SgClassDefinitionPtrList subclasses = classHierarchy->getSubclasses( classDefinition );
                    cout << "Virtual function " << memberFunctionDeclaration->get_mangled_name().str() << "\n";
                    string name2 = memberFunctionDeclaration->get_qualified_name().getString() +
                    memberFunctionDeclaration->get_mangled_name().getString();
                    for ( SgClassDefinitionPtrList::iterator it_cls = subclasses.begin(); it_cls != subclasses.end(); it_cls++ )
                       {
                         SgClassDefinition *cls = isSgClassDefinition( *it_cls );
                         SgDeclarationStatementPtrList &clsMembers = cls->get_members();
                         for ( SgDeclarationStatementPtrList::iterator it_cls_mb = clsMembers.begin(); it_cls_mb != clsMembers.end(); it_cls_mb++ )
                            {
                              SgMemberFunctionDeclaration *cls_mb_decl = isSgMemberFunctionDeclaration( *it_cls_mb );
                              string name3 = cls_mb_decl->get_qualified_name().getString() +
                              cls_mb_decl->get_mangled_name().getString();
                              string type3str = cls_mb_decl->get_type()->get_mangled().getString();
                              if ( name2 == name3 )
                                 {
                                   SgMemberFunctionDeclaration *nonDefDecl = isSgMemberFunctionDeclaration( cls_mb_decl->get_firstNondefiningDeclaration() );
                                   SgMemberFunctionDeclaration *defDecl = isSgMemberFunctionDeclaration( cls_mb_decl->get_definingDeclaration() );
                                   ROSE_ASSERT ( (!nonDefDecl && defDecl == cls_mb_decl) || (nonDefDecl == cls_mb_decl && nonDefDecl) );

                                   if ( nonDefDecl )
                                      {
                                        if ( !( nonDefDecl->get_functionModifier().isPureVirtual() ) && nonDefDecl->get_functionModifier().isVirtual() )
                                             functionList.push_back( nonDefDecl );
                                      }
                                     else
                                        if ( !( defDecl->get_functionModifier().isPureVirtual() ) && defDecl->get_functionModifier().isVirtual() )
                                             functionList.push_back( defDecl ); // == cls_mb_decl
                                 }
                           }
                       }
                  }
             }
        }

     cout << "Function list size: " << functionList.size() << "\n";
     return functionList;
   }

SgFunctionDeclarationPtrList
solveMemberFunctionCall( SgClassType *crtClass, ClassHierarchyWrapper *classHierarchy,
						  SgMemberFunctionDeclaration *memberFunctionDeclaration, bool polymorphic )
{
  SgFunctionDeclarationPtrList functionList;
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
	functionList.push_back( functionDeclarationInClass );
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

	      f2 = cls_mb_decl->get_mangled_name().str();
	      if ( f1 == f2 )
		{
		  SgMemberFunctionDeclaration *nonDefDecl =
		    isSgMemberFunctionDeclaration( cls_mb_decl->get_firstNondefiningDeclaration() );
		  SgMemberFunctionDeclaration *defDecl =
		    isSgMemberFunctionDeclaration( cls_mb_decl->get_definingDeclaration() );
		  ROSE_ASSERT ( (!nonDefDecl && defDecl == cls_mb_decl) || (nonDefDecl == cls_mb_decl && nonDefDecl) );
		  if ( nonDefDecl )
		    functionDeclarationInClass = nonDefDecl;
		  else
		    functionDeclarationInClass = defDecl;
		  ROSE_ASSERT ( functionDeclarationInClass );
		  if ( !( functionDeclarationInClass->get_functionModifier().isPureVirtual() ) )
		    functionList.push_back( functionDeclarationInClass );
		}
	    }
	}
    } // end if virtual
  // non virtual (standard) member function or call not polymorphic (or both)
  else
    if ( functionDeclarationInClass->get_declarationModifier().get_storageModifier().isStatic() )
      {
	cout << "Found static function declaration called as member function " << functionDeclarationInClass << "\n";
	exit( 1 );
      }
    else
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
	functionList.push_back ( functionDeclarationInClass );
      }
  return functionList;
}
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



FunctionData::FunctionData ( SgFunctionDeclaration* inputFunctionDeclaration,
			     SgProject *project, ClassHierarchyWrapper *classHierarchy )
   {
     hasDefinition = false;
     properties = new FunctionProperties();
     properties->functionDeclaration = inputFunctionDeclaration;
     ROSE_ASSERT( properties->functionDeclaration != NULL );

     properties->functionType = inputFunctionDeclaration->get_type()->findBaseType();
     properties->invokedClass = NULL;
     properties->isPointer = properties->isPolymorphic = false;
     SgFunctionDeclaration *defDecl =
       isSgFunctionDeclaration( properties->functionDeclaration->get_definingDeclaration() );

     //cout << "!!!" << inputFunctionDeclaration->get_name().str() << " has definition " << defDecl << "\n";
     //     cout << "Input declaration: " << inputFunctionDeclaration << " as opposed to " << functionDeclaration << "\n"; 

     // Test for a forward declaration (declaration without a definition)
     if ( defDecl )
       {
	 SgFunctionDefinition* functionDefinition = defDecl->get_definition();
	 ROSE_ASSERT ( functionDefinition != NULL );
	 hasDefinition = true;
	 Rose_STL_Container<SgNode*> functionCallExpList;
	 functionCallExpList = NodeQuery::querySubTree ( functionDefinition, V_SgFunctionCallExp );
	 
	 // printf ("functionCallExpList.size() = %zu \n",functionCallExpList.size());
	 
	 // list<SgFunctionDeclaration*> functionListDB;
	 Rose_STL_Container<SgNode*>::iterator i = functionCallExpList.begin();

	  // for all functions getting called in the body of the current function
	  // we need to get their declarations, or the set of declarations for
	  // function pointers and virtual functions
          while (i != functionCallExpList.end())
             {
	       FunctionProperties *fctProps = new FunctionProperties();
	       fctProps->functionDeclaration = NULL;
	       fctProps->functionType = NULL;
	       fctProps->invokedClass = NULL;
	       fctProps->isPointer = fctProps->isPolymorphic = false;

               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(*i);
               ROSE_ASSERT ( functionCallExp != NULL );

               SgExpression* functionExp = functionCallExp->get_function();
               ROSE_ASSERT ( functionExp != NULL );

	       switch ( functionExp->variantT() )
		 {
		 case V_SgArrowStarOp:
		 case V_SgDotStarOp:
		   {
		     /*
		     SgFunctionDeclarationPtrList fD =
		       CallGraphFunctionSolver::solveMemberFunctionPointerCall( functionExp, classHierarchy );
		     for ( SgFunctionDeclarationPtrList::iterator it = fD.begin(); it != fD.end(); it++ )
		       {
			 ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
			 functionListDB.push_back( *it );
		       }
		     */
		     SgBinaryOp *binaryExp = isSgBinaryOp( functionExp );
		     ROSE_ASSERT ( isSgArrowStarOp( binaryExp ) || isSgDotStarOp( binaryExp ) );//|| isSgArrowExp ( binaryExp ) || isSgDotExp ( binaryExp ) );

		     SgExpression *left, *right;
		     SgClassType *classType;
 
		     left = binaryExp->get_lhs_operand();
		     right = binaryExp->get_rhs_operand();
		     // left side of the expression should have class type
		     classType = isSgClassType( left->get_type()->findBaseType() );
		     ROSE_ASSERT ( classType );
		     if ( !isSgThisExp( left ) ||
			  isSgPointerDerefExp( left ) && isSgThisExp( isSgPointerDerefExp( left )->get_operand() ))
		       {
			 fctProps->invokedClass = isSgClassDeclaration( classType->get_declaration() )->get_definition();
			 fctProps->isPolymorphic = true;
		       }
		     else
		       {
			 fctProps->invokedClass = NULL;
			 fctProps->isPolymorphic = false;
		       }
		     fctProps->functionDeclaration = NULL;
		     fctProps->functionType = isSgMemberFunctionType( right->get_type()->findBaseType() );
		     ROSE_ASSERT ( fctProps->functionType );
		     fctProps->isPointer = true;
		     functionListDB.push_back( fctProps );
		   }
		   break;
		 case V_SgDotExp:
		 case V_SgArrowExp:
		   {
		     SgMemberFunctionDeclaration *memberFunctionDeclaration = NULL;
		     SgClassType *crtClass = NULL;
		     fctProps->isPointer = false;
		     ROSE_ASSERT ( isSgBinaryOp( functionExp ) );

		     SgExpression *leftSide = isSgBinaryOp( functionExp )->get_lhs_operand();
		     SgType *leftType = leftSide->get_type()->findBaseType();
		     crtClass = isSgClassType( leftType );

		     SgMemberFunctionRefExp *memberFunctionRefExp =
		       isSgMemberFunctionRefExp( isSgBinaryOp( functionExp )->get_rhs_operand() );
		     memberFunctionDeclaration =
		       isSgMemberFunctionDeclaration( memberFunctionRefExp->get_symbol()->get_declaration() );
		     ROSE_ASSERT ( memberFunctionDeclaration && crtClass );
		     SgMemberFunctionDeclaration *nonDefDecl =
		       isSgMemberFunctionDeclaration( memberFunctionDeclaration->get_firstNondefiningDeclaration() );
		     if ( nonDefDecl )
		       memberFunctionDeclaration = nonDefDecl;

		     fctProps->functionDeclaration = memberFunctionDeclaration;
		     ROSE_ASSERT ( isSgFunctionDeclaration( memberFunctionDeclaration ) );
		     fctProps->functionType = fctProps->functionDeclaration->get_type()->findBaseType();
		     ROSE_ASSERT ( fctProps->functionType );
		     if ( isSgThisExp( leftSide ) || !( memberFunctionDeclaration->get_functionModifier().isVirtual() ) )
		       {
			 fctProps->isPolymorphic = false;
			 fctProps->invokedClass = NULL;
		       }
		     else
		       {
			 fctProps->isPolymorphic = true;
			 fctProps->invokedClass = isSgClassDeclaration( crtClass->get_declaration() )->get_definition();
			 cout << "SET polymorphic on class " << fctProps->invokedClass->get_qualified_name().getString()
			      << "\t" << fctProps << "\n";
		       }
		     functionListDB.push_back( fctProps );
		   }
		   break;
		 case V_SgPointerDerefExp:
		   {
		     fctProps->isPointer = true;
		     fctProps->isPolymorphic = false;
		     fctProps->invokedClass = NULL;
		     fctProps->functionDeclaration = NULL; 
		     fctProps->functionType =
		       isSgFunctionType( isSgPointerDerefExp( functionExp )->get_type()->findBaseType() );
		     ROSE_ASSERT ( fctProps->functionType );
		     functionListDB.push_back( fctProps );
		   }
		   break;
		 case V_SgMemberFunctionRefExp:
		   {
		     SgMemberFunctionDeclaration *mbFctDecl =
		       isSgMemberFunctionRefExp( functionExp )->get_symbol()->get_declaration();
		     ROSE_ASSERT ( mbFctDecl );
		     SgMemberFunctionDeclaration *nonDefDecl =
		       isSgMemberFunctionDeclaration( mbFctDecl->get_firstNondefiningDeclaration() );
		     if ( nonDefDecl )
		       mbFctDecl = nonDefDecl;
		     ROSE_ASSERT( mbFctDecl );
		     fctProps->functionDeclaration = mbFctDecl;
		     fctProps->functionType = mbFctDecl->get_type()->findBaseType();
		     ROSE_ASSERT ( fctProps->functionType );
		     fctProps->isPointer = false;
		     fctProps->isPolymorphic = false;
		     fctProps->invokedClass = NULL;
		     functionListDB.push_back( fctProps );

		     /*
		     functionListDB.push_back( mbFctDecl );
		     */
		   }
		   break;
		 case V_SgFunctionRefExp:
		   {
		     SgFunctionDeclaration *fctDecl =
		       isSgFunctionDeclaration( isSgFunctionRefExp( functionExp )->get_symbol()->get_declaration() );
		     ROSE_ASSERT ( fctDecl );
		     SgFunctionDeclaration *nonDefDecl =
		       isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );
		     if ( nonDefDecl )
		       fctProps->functionDeclaration = nonDefDecl;
		     else
		       fctProps->functionDeclaration = fctDecl;
		     ROSE_ASSERT ( isSgFunctionDeclaration( fctProps->functionDeclaration ) );
		     fctProps->functionType = fctProps->functionDeclaration->get_type()->findBaseType();
		     ROSE_ASSERT ( fctProps->functionType );
		     fctProps->isPointer = false;
		     fctProps->isPolymorphic = false;
		     fctProps->invokedClass = NULL;
		     functionListDB.push_back( fctProps );
		   }
		   break;
		 default:
		   {
		     cout << "Error, unexpected type of functionRefExp: " << functionExp->sage_class_name() << "!!!\n";
		     ROSE_ASSERT ( false );
		   }
		 }
	       ROSE_ASSERT ( !( fctProps->functionDeclaration ) || isSgFunctionDeclaration( fctProps->functionDeclaration ) );
	       ROSE_ASSERT ( isSgType( fctProps->functionType ) );
               i++;
             }
	}
   }


FunctionData::FunctionData ( SgFunctionDeclaration* inputFunctionDeclaration, bool hasDef,
			     SgProject *project, ClassHierarchyWrapper *classHierarchy )
{
  hasDefinition = hasDef;
  functionDeclaration = inputFunctionDeclaration;
  ROSE_ASSERT( functionDeclaration != NULL );
  SgFunctionDeclaration *defDecl =
    isSgFunctionDeclaration( functionDeclaration->get_definingDeclaration() );
  
  //cout << " " << functionDeclaration->get_name().str() << " has definition " << functionDefinition << "\n";
  // cout << "Input declaration: " << inputFunctionDeclaration << " as opposed to " << functionDeclaration << "\n";
  
  // Test for a forward declaration (declaration without a definition)
  if ( defDecl )
    {
      SgFunctionDefinition* functionDefinition = defDecl->get_definition();
      ROSE_ASSERT ( functionDefinition != NULL );
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
	  
	  SgExpression* functionExp = functionCallExp->get_function();
	  ROSE_ASSERT ( functionExp != NULL );
	  //cout << "Function expression " << functionExp->sage_class_name() << "\n";
	  
	  switch ( functionExp->variantT() )
	    {
	    case V_SgArrowStarOp:
	    case V_SgDotStarOp:
	      {
		SgFunctionDeclarationPtrList fD =
		  CallTargetSet::solveMemberFunctionPointerCall( functionExp, classHierarchy );
		for ( SgFunctionDeclarationPtrList::iterator it = fD.begin(); it != fD.end(); it++ )
		  {
		    ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
		    functionList.push_back( *it );
		  }
	      }
	      break;
	    case V_SgDotExp:
	    case V_SgArrowExp:
	      {
		SgMemberFunctionDeclaration *memberFunctionDeclaration = NULL;
		SgClassType *crtClass = NULL;
		bool polymorphic = false;
		
		ROSE_ASSERT ( isSgBinaryOp( functionExp ) );
		SgExpression *leftSide = isSgBinaryOp( functionExp )->get_lhs_operand();
		if ( isSgThisExp( leftSide ) )
		  polymorphic = false;
		else
		  polymorphic = true;
		
		SgMemberFunctionRefExp *memberFunctionRefExp =
		  isSgMemberFunctionRefExp( isSgBinaryOp( functionExp )->get_rhs_operand() );
		SgType *leftType = leftSide->get_type();
		leftType = leftType->findBaseType();

		crtClass = isSgClassType( leftType );
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
                if(isSgConstructorInitializer(leftSide)!= NULL){
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
                        SgClassType* lhsClassType = isSgClassType(isSgFunctionCallExp(*expLst->get_expressions().begin())->get_type());
                        
                        crtClass = lhsClassType;
                        }

                        
                        ROSE_ASSERT(crtClass!=NULL);
                }
		memberFunctionDeclaration =
		  isSgMemberFunctionDeclaration( memberFunctionRefExp->get_symbol()->get_declaration() );
		ROSE_ASSERT ( memberFunctionDeclaration && crtClass );
                ROSE_ASSERT ( crtClass != NULL );
	
		// returns the list of all in-class declarations of functions potentially called
		// ( may be several because of polymorphism )
		SgFunctionDeclarationPtrList fD =
		  CallTargetSet::solveMemberFunctionCall( crtClass, classHierarchy,
								    memberFunctionDeclaration, polymorphic );
		for ( SgFunctionDeclarationPtrList::iterator it = fD.begin(); it != fD.end(); it++ )
		  {
		    ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
		    functionList.push_back( *it );
		  }
	      }
	      break;
	    case V_SgPointerDerefExp:
	      // function pointer:
	      // a. can be returned by another function
	      // b. can be pointed to by a variable
	      {
		SgFunctionDeclarationPtrList fD =
		  CallTargetSet::solveFunctionPointerCall( isSgPointerDerefExp( functionExp ), project );
		for ( SgFunctionDeclarationPtrList::iterator it = fD.begin(); it != fD.end(); it++ )
		  {
		    ROSE_ASSERT ( isSgFunctionDeclaration( *it ) );
		    functionList.push_back( *it );
		  }
	      }
	      break;
	    case V_SgMemberFunctionRefExp:
	      {
		SgMemberFunctionDeclaration *mbFctDecl =
		  isSgMemberFunctionRefExp( functionExp )->get_symbol()->get_declaration();
		ROSE_ASSERT ( mbFctDecl );
		SgMemberFunctionDeclaration *nonDefDecl =
		  isSgMemberFunctionDeclaration( mbFctDecl->get_firstNondefiningDeclaration() );
		if ( nonDefDecl )
		  mbFctDecl = nonDefDecl;
		functionList.push_back( mbFctDecl );
	      }
	      break;
	    case V_SgFunctionRefExp:
	      {
		//cout << "Member fct or fct ref\n";
		SgFunctionDeclaration *fctDecl =
		  isSgFunctionDeclaration( isSgFunctionRefExp( functionExp )->get_symbol()->get_declaration() );
		ROSE_ASSERT ( fctDecl );
		SgFunctionDeclaration *nonDefDecl =
		  isSgFunctionDeclaration( fctDecl->get_firstNondefiningDeclaration() );
		if ( nonDefDecl )
		  functionList.push_back( nonDefDecl );
		else
		  functionList.push_back( fctDecl );
	      }
	      break;
	    default:
	      {
		cout << "Error, unexpected type of functionRefExp: " << functionExp->sage_class_name() << "!!!\n";
		ROSE_ASSERT ( false );
	      }
	    }
	  i++;
	}
    }
}


CallGraphNode*
findNode ( Rose_STL_Container<CallGraphNode*> & nodeList, SgFunctionDeclaration* functionDeclaration )
   {
     Rose_STL_Container<CallGraphNode*>::iterator k = nodeList.begin();

     CallGraphNode* returnNode = NULL;

     bool found = false;
     while ( !found && (k != nodeList.end()) )
        {

          if ( ( var_SOLVE_FUNCTION_CALLS_IN_DB == true &&
                 (*k)->properties->functionDeclaration == functionDeclaration ) 
               ||
               ( var_SOLVE_FUNCTION_CALLS_IN_DB == false && 
                 (*k)->functionDeclaration == functionDeclaration) )
             {
               returnNode = *k;
               found = true;
             }
          k++;
        }

     if ( !returnNode )
       cout << "NO node found for " << functionDeclaration->get_name().str() << " " << functionDeclaration << "\n";
     // ROSE_ASSERT (returnNode != NULL);
     return returnNode;
   }

CallGraphNode*
findNode ( Rose_STL_Container<CallGraphNode*> & nodeList, string name )
   {
     Rose_STL_Container<CallGraphNode*>::iterator k = nodeList.begin();

     CallGraphNode* returnNode = NULL;

     bool found = false;
     while ( !found && (k != nodeList.end()) )
        {
          if ((*k)->label == name)
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


#if 0
template<typename Predicate>
void
CallGraphBuilder::buildCallGraph (Predicate pred)
   {
     Rose_STL_Container<FunctionData *> callGraphData;

  //AS (09/23/06) Query the memory pool instead of subtree of project
     VariantVector vv( V_SgFunctionDeclaration );
     AstQueryNamespace::DefaultNodeFunctional defFunc;
    Rose_STL_Container<SgNode *> functionList = NodeQuery::queryMemoryPool(defFunc, &vv );


  //   list<SgNode *> functionList = NodeQuery::querySubTree ( project, V_SgFunctionDeclaration );
     
     ClassHierarchyWrapper classHierarchy( project );
     Rose_STL_Container<SgNode *>::iterator i = functionList.begin();

     printf ("Inside of buildCallGraph functionList.size() = %zu \n",functionList.size());

     while ( i != functionList.end() )
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration( *i );
          ROSE_ASSERT ( functionDeclaration != NULL );


          bool hasDef = false;
      

          if ( var_SOLVE_FUNCTION_CALLS_IN_DB == true && functionDeclaration->get_definition() != NULL )
	   {
	     // printf ("Insert function declaration containing function definition \n");
	     hasDef = true;
           }

	  // determining the in-class declaration
	  if ( isSgMemberFunctionDeclaration( functionDeclaration ) )
	    {
	      // always saving the in-class declaration, so we need to find that one
	      SgDeclarationStatement *nonDefDeclInClass =
		isSgMemberFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
	      // functionDeclaration is outside the class (so it must have a definition)
	      if ( nonDefDeclInClass )
		  functionDeclaration = isSgMemberFunctionDeclaration( nonDefDeclInClass );
	    }
	  else
	    {
	      // we need to have only one declaration for regular functions as well
	      SgFunctionDeclaration *nonDefDecl =
		isSgFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
	      if ( nonDefDecl )
		functionDeclaration = nonDefDecl;
	    }
	  FunctionData* functionData = var_SOLVE_FUNCTION_CALLS_IN_DB ? new FunctionData( functionDeclaration, project, &classHierarchy )
            : new FunctionData( functionDeclaration, hasDef, project, &classHierarchy ) ;

          if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
	     *i = functionDeclaration;

          //AS(032806) Filter out functions baced on criteria in predicate
          if(pred(functionDeclaration)==true) 
             callGraphData.push_back( functionData );
          i++;
        }

     // Build the graph
     CallGraphCreate *returnGraph = new CallGraphCreate();
     ROSE_ASSERT (returnGraph != NULL);

     Rose_STL_Container<FunctionData *>::iterator j = callGraphData.begin();

     printf ("Build the node list callGraphData.size() = %zu \n",callGraphData.size());

     Rose_STL_Container<CallGraphNode *> nodeList;
     while ( j != callGraphData.end() )
        {
          string functionName;
          SgFunctionDeclaration* id;
          if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
          {
	  ROSE_ASSERT ( (*j)->properties->functionDeclaration );
	  functionName = (*j)->properties->functionDeclaration->get_mangled_name().getString();
          id = (*j)->properties->functionDeclaration;

          }else
          {
             functionName = (*j)->functionDeclaration->get_name().str();
             id = (*j)->functionDeclaration;

	  cout << "Function: "
	       << (*j)->properties->functionDeclaration->get_scope()->get_qualified_name().getString() +
	    (*j)->properties->functionDeclaration->get_mangled_name().getString()
	       << " has declaration " << (*j)->isDefined() << "\n";

          }

	  // Generate a unique name to test against later
	  SgDeclarationStatement *nonDefDeclInClass =
	    isSgMemberFunctionDeclaration( id->get_firstNondefiningDeclaration() );
	  if ( nonDefDeclInClass )
	    ROSE_ASSERT ( id == nonDefDeclInClass );
          CallGraphNode* node = (var_SOLVE_FUNCTION_CALLS_IN_DB == true ? new CallGraphNode( functionName, (*j)->properties, (*j)->isDefined() )
             : new CallGraphNode(functionName, id, (*j)->isDefined())  );

          if(var_SOLVE_FUNCTION_CALLS_IN_DB == true )
              ROSE_ASSERT ( node->properties->functionType );

          nodeList.push_back( node );
	  /*
	  // show graph
	  cout << "Function " << functionName << "   " << id << " has pointers to:\n";
	  list <FunctionProperties *> &fL = (*j)->functionList;
          list<FunctionProperties *>::iterator k = fL.begin();
 	  while (k != fL.end())
	    {
	      cout << "\tfunction: " << *k << "\n";
	      k++;
	    }
	  */
          returnGraph->addNode( node );
          j++;
        }

     j = callGraphData.begin();
     cout << "NodeList size: " << nodeList.size() << "\n";
     int totEdges = 0;
     while (j != callGraphData.end())
        {
	  //          printf ("Calling findNode in outer loop (*j)->functionDeclaration->get_name() = %s \n",(*j)->functionDeclaration->get_name().str());
          CallGraphNode* startingNode = ( var_SOLVE_FUNCTION_CALLS_IN_DB == true ? findNode( nodeList, (*j)->properties->functionDeclaration ) :
             findNode(nodeList,(*j)->functionDeclaration) );
          ROSE_ASSERT (startingNode != NULL);

          Rose_STL_Container<FunctionProperties *> & functionList = ( var_SOLVE_FUNCTION_CALLS_IN_DB == true ? (*j)->functionListDB :
              (*j)->functionList);
          Rose_STL_Container<FunctionProperties *>::iterator k = functionList.begin();

          if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
          {

            while ( k != functionList.end() )
            {
              ROSE_ASSERT ( (*k)->functionType );
              string label = "POINTER";

              CallGraphEdge* edge = new CallGraphEdge( " " );
              if ( (*k)->functionDeclaration )
                edge->label = (*k)->functionDeclaration->get_mangled_name().getString();
              ROSE_ASSERT ( edge != NULL );
              edge->properties = *k;
              // if we have a pointer (no function declaration) or a virtual function, create dummy node
              if ( !( (*k)->functionDeclaration ) || (*k)->isPolymorphic )
              {
                CallGraphNode *dummy;
                if ( (*k)->functionDeclaration && (*k)->functionDeclaration->get_definingDeclaration() )
                  dummy = new CallGraphNode( "DUMMY", *k, true );
                else
                  dummy = new CallGraphNode( "DUMMY", *k, false );
                returnGraph->addNode( dummy );

                returnGraph->addEdge( startingNode, dummy, edge );
              }
              else
              {
                CallGraphNode *endNode = findNode( nodeList, ( *k )->functionDeclaration );
                ROSE_ASSERT ( endNode );
                if(returnGraph->edgeExist(startingNode,endNode)==false)
                  returnGraph->addEdge( startingNode, endNode, edge );
                cout << "\tEndNode "
                  << (*k)->functionDeclaration->get_name().str() << "\t" << endNode->isDefined() << "\n";
              }
              totEdges++;
              k++;
            }
          }else{

            while (k != functionList.end())
            {
              CallGraphNode* endingNode = findNode( nodeList, *k );
              /*
                 if ( !endingNode )
                 endingNode = findNode( nodeList,
                 ( *k )->get_qualified_name().getString() +
                 ( *k )->get_mangled_name().getString(), 1 );
               */
              if ( endingNode )
              {
                ROSE_ASSERT (endingNode != NULL);

                CallGraphEdge* edge = new CallGraphEdge(endingNode->functionDeclaration->get_name().getString());
                ROSE_ASSERT (edge != NULL);
                if(returnGraph->edgeExist(startingNode,endingNode)==false)
                  returnGraph->addEdge(startingNode,endingNode,edge);
                totEdges++;
              }
              else
              {
                cout << "COULDN'T FIND: " << ( *k )->functionDeclaration->get_qualified_name().str() << "\n";
                //isSgFunctionDeclaration( *k )->get_file_info()->display("AVOIDED CALL");
              }
              k++;
            }
          }
	  j++;
        }

     cout << "Total number of edges: " << totEdges << "\n";
     // printf ("Return graph \n");

     graph = returnGraph;
   }
#endif

template<typename Predicate>
void
CallGraphBuilder::buildCallGraph (Predicate pred)
{
  if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
  {
    Rose_STL_Container<FunctionData *> callGraphData;

    //AS (09/23/06) Query the memory pool instead of subtree of project
    VariantVector vv( V_SgFunctionDeclaration );
    AstQueryNamespace::DefaultNodeFunctional defFunc;
    Rose_STL_Container<SgNode *> functionList = NodeQuery::queryMemoryPool(defFunc, &vv );


    //   list<SgNode *> functionList = NodeQuery::querySubTree ( project, V_SgFunctionDeclaration );

    ClassHierarchyWrapper classHierarchy( project );
    Rose_STL_Container<SgNode *>::iterator i = functionList.begin();

    printf ("Inside of buildCallGraph functionList.size() = %zu \n",functionList.size());

    while ( i != functionList.end() )
    {
      SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration( *i );
      ROSE_ASSERT ( functionDeclaration != NULL );

      // determining the in-class declaration
      if ( isSgMemberFunctionDeclaration( functionDeclaration ) )
      {
        // always saving the in-class declaration, so we need to find that one
        SgDeclarationStatement *nonDefDeclInClass =
          isSgMemberFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
        // functionDeclaration is outside the class (so it must have a definition)
        if ( nonDefDeclInClass )
          functionDeclaration = isSgMemberFunctionDeclaration( nonDefDeclInClass );
      }
      else
      {
        // we need to have only one declaration for regular functions as well
        SgFunctionDeclaration *nonDefDecl =
          isSgFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
        if ( nonDefDecl )
          functionDeclaration = nonDefDecl;
      }
      FunctionData* functionData = new FunctionData( functionDeclaration, project, &classHierarchy );
      //*i = functionDeclaration;

      //AS(032806) Filter out functions baced on criteria in predicate
      if(pred(functionDeclaration)==true) 
        callGraphData.push_back( functionData );
      i++;
    }

    // Build the graph
    CallGraphCreate *returnGraph = new CallGraphCreate();
    ROSE_ASSERT (returnGraph != NULL);

    Rose_STL_Container<FunctionData *>::iterator j = callGraphData.begin();

    printf ("Build the node list callGraphData.size() = %zu \n",callGraphData.size());

    Rose_STL_Container<CallGraphNode *> nodeList;
    while ( j != callGraphData.end() )
    {
      string functionName;
      ROSE_ASSERT ( (*j)->properties->functionDeclaration );
      functionName = (*j)->properties->functionDeclaration->get_mangled_name().getString();

      // Generate a unique name to test against later
      SgFunctionDeclaration* id = (*j)->properties->functionDeclaration;
      SgDeclarationStatement *nonDefDeclInClass =
        isSgMemberFunctionDeclaration( id->get_firstNondefiningDeclaration() );
      if ( nonDefDeclInClass )
        ROSE_ASSERT ( id == nonDefDeclInClass );
      CallGraphNode* node = new CallGraphNode( functionName, (*j)->properties, (*j)->isDefined() );
      cout << "Function: "
        << (*j)->properties->functionDeclaration->get_scope()->get_qualified_name().getString() +
        (*j)->properties->functionDeclaration->get_mangled_name().getString()
        << " has declaration " << (*j)->isDefined() << "\n";
      nodeList.push_back( node );
      /*
      // show graph
      cout << "Function " << functionName << "   " << id << " has pointers to:\n";
      list <FunctionProperties *> &fL = (*j)->functionList;
      list<FunctionProperties *>::iterator k = fL.begin();
      while (k != fL.end())
      {
      cout << "\tfunction: " << *k << "\n";
      k++;
      }
       */
      ROSE_ASSERT ( node->properties->functionType );
      returnGraph->addNode( node );
      j++;
    }

    j = callGraphData.begin();
    cout << "NodeList size: " << nodeList.size() << "\n";
    int totEdges = 0;
    while (j != callGraphData.end())
    {
      //          printf ("Calling findNode in outer loop (*j)->functionDeclaration->get_name() = %s \n",(*j)->functionDeclaration->get_name().str());
      CallGraphNode* startingNode = findNode( nodeList, (*j)->properties->functionDeclaration );
      ROSE_ASSERT (startingNode != NULL);

      Rose_STL_Container<FunctionProperties *> & functionList = (*j)->functionListDB;
      Rose_STL_Container<FunctionProperties *>::iterator k = functionList.begin();

      while ( k != functionList.end() )
      {
        ROSE_ASSERT ( (*k)->functionType );
        string label = "POINTER";

        CallGraphEdge* edge = new CallGraphEdge( " " );
        if ( (*k)->functionDeclaration )
          edge->label = (*k)->functionDeclaration->get_mangled_name().getString();
        ROSE_ASSERT ( edge != NULL );
        edge->properties = *k;
        // if we have a pointer (no function declaration) or a virtual function, create dummy node
        if ( !( (*k)->functionDeclaration ) || (*k)->isPolymorphic )
        {
          CallGraphNode *dummy;
          if ( (*k)->functionDeclaration && (*k)->functionDeclaration->get_definingDeclaration() )
            dummy = new CallGraphNode( "DUMMY", *k, true );
          else
            dummy = new CallGraphNode( "DUMMY", *k, false );
          returnGraph->addNode( dummy );

          returnGraph->addEdge( startingNode, dummy, edge );
        }
        else
        {
          CallGraphNode *endNode = findNode( nodeList, ( *k )->functionDeclaration );
          ROSE_ASSERT ( endNode );
          if(returnGraph->edgeExist(startingNode,endNode)==false)
            returnGraph->addEdge( startingNode, endNode, edge );
          cout << "\tEndNode "
            << (*k)->functionDeclaration->get_name().str() << "\t" << endNode->isDefined() << "\n";
        }
        totEdges++;
        k++;
      }
      j++;
    }

    cout << "Total number of edges: " << totEdges << "\n";
    // printf ("Return graph \n");

    graph = returnGraph;


  }else{
    Rose_STL_Container<FunctionData*> callGraphData;

    // list<SgNode*> functionList = NodeQuery::querySubTree ( project, V_SgFunctionDeclaration );
    //AS (09/23/06) Query the memory pool instead of subtree of project
    VariantVector vv(V_SgFunctionDeclaration);
    AstQueryNamespace::DefaultNodeFunctional defFunc;
    Rose_STL_Container<SgNode *> functionList = NodeQuery::queryMemoryPool(defFunc, &vv );


    ClassHierarchyWrapper classHierarchy( project );
    Rose_STL_Container<SgNode*>::iterator i = functionList.begin();

    printf ("Inside of buildCallGraph functionList.size() = %zu \n",functionList.size());

    while (i != functionList.end())
    {
      SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
      ROSE_ASSERT (functionDeclaration != NULL);
      bool hasDef = false;

      if ( functionDeclaration->get_definition() != NULL )
      {
        // printf ("Insert function declaration containing function definition \n");
        hasDef = true;
      }

      // determining the in-class declaration
      if ( isSgMemberFunctionDeclaration( functionDeclaration ) )
      {
        // always saving the in-class declaration, so we need to find that one
        SgDeclarationStatement *nonDefDeclInClass =
          isSgMemberFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
        // functionDeclaration is outside the class (so it must have a definition)
        if ( nonDefDeclInClass )
          functionDeclaration = isSgMemberFunctionDeclaration( nonDefDeclInClass );
      }
      else
      {
        // we need to have only one declaration for regular functions as well
        SgFunctionDeclaration *nonDefDecl =
          isSgFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
        if ( nonDefDecl )
          functionDeclaration = nonDefDecl;
      }

      FunctionData* functionData = new FunctionData( functionDeclaration, hasDef, project, &classHierarchy );
      *i = functionDeclaration;


      //AS(032806) Filter out functions baced on criteria in predicate
      if(pred(functionDeclaration)==true) 
        callGraphData.push_back( functionData );
      i++;
    }

    // Build the graph
    CallGraphCreate *returnGraph = new CallGraphCreate();
    ROSE_ASSERT (returnGraph != NULL);

    Rose_STL_Container<FunctionData*>::iterator j = callGraphData.begin();

    // printf ("Build the node list callGraphData.size() = %zu \n",callGraphData.size());

    Rose_STL_Container<CallGraphNode*> nodeList;
    while (j != callGraphData.end())
    {
      string functionName = (*j)->functionDeclaration->get_name().str();

      // Generate a unique name to test against later
      SgFunctionDeclaration* id = (*j)->functionDeclaration;

      SgDeclarationStatement *nonDefDeclInClass =
        isSgMemberFunctionDeclaration( id->get_firstNondefiningDeclaration() );
      if ( nonDefDeclInClass )
        ROSE_ASSERT ( id == nonDefDeclInClass );

      CallGraphNode* node = new CallGraphNode(functionName, id, (*j)->isDefined());
      nodeList.push_back(node);
      // show graph
      /*
         cout << "Function " << functionName << " " << id << " has pointers to:\n";
         list <SgFunctionDeclaration *> &fL = (*j)->functionList;
         list<SgFunctionDeclaration*>::iterator k = fL.begin();
         while (k != fL.end())
         {
         cout << "\tfunction: " << ( *k )->get_name().str() << ( *k ) << "\n";
         k++;
         }
       */

      returnGraph->addNode(node);
      j++;
    }

    j = callGraphData.begin();
    // cout << "NodeList size: " << nodeList.size() << "\n";
    int totEdges = 0;
    while (j != callGraphData.end())
    {
      // printf ("Calling findNode in outer loop (*j)->functionDeclaration->get_name() = %s \n",(*j)->functionDeclaration->get_name().str());
      CallGraphNode* startingNode = findNode(nodeList,(*j)->functionDeclaration);
      ROSE_ASSERT (startingNode != NULL);

      Rose_STL_Container<SgFunctionDeclaration*> & functionList = (*j)->functionList;
      Rose_STL_Container<SgFunctionDeclaration*>::iterator k = functionList.begin();

      // printf ("Now iterate over the list (size = %d) for function %s \n",
      // functionList.size(),(*j)->functionDeclaration->get_name().str());
      while (k != functionList.end())
      {
        CallGraphNode* endingNode = findNode( nodeList, *k );
        /*
           if ( !endingNode )
           endingNode = findNode( nodeList,
           ( *k )->get_qualified_name().getString() +
           ( *k )->get_mangled_name().getString(), 1 );
         */
        if ( endingNode )
        {
          ROSE_ASSERT (endingNode != NULL);

          CallGraphEdge* edge = new CallGraphEdge(endingNode->functionDeclaration->get_name().getString());
          ROSE_ASSERT (edge != NULL);
          if(returnGraph->edgeExist(startingNode,endingNode)==false)
            returnGraph->addEdge(startingNode,endingNode,edge);
          totEdges++;
        }
        else
        {
          cout << "COULDN'T FIND: " << ( *k )->get_qualified_name().str() << "\n";
          //isSgFunctionDeclaration( *k )->get_file_info()->display("AVOIDED CALL");
        }
        k++;
      }
      j++;
    }

    cout << "Total number of edges: " << totEdges << "\n";
    // printf ("Return graph \n");

    graph = returnGraph;
  }
}

void
CallGraphBuilder::classifyCallGraph ()
{
  ROSE_ASSERT(graph != NULL);
  
  // printf ("Output the graph information! (number of nodes = %zu) \n",graph->size());
  
  int counter = 0;

  // This iteration over the graph verifies that we have a valid graph!
  CallGraphCreate::NodeIterator nodeIterator;
  for (nodeIterator = graph->GetNodeIterator(); !nodeIterator.ReachEnd(); ++nodeIterator)
    {
      // printf ("In loop using node iterator ... \n");
      // DAGBaseNodeImpl* nodeImpl = nodeIterator.Current();
      CallGraphNode* node = *nodeIterator;
      
      ROSE_ASSERT (node != NULL);

      string filename;
      if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
      {
        filename = "./__pointers";
        if ( node->properties->functionDeclaration )
        {
          ROSE_ASSERT (node->properties->functionDeclaration->get_file_info() != NULL);
          ROSE_ASSERT (node->properties->functionDeclaration->get_file_info()->get_filename() != NULL);
          string tmp = node->properties->functionDeclaration->get_file_info()->get_filename();
          filename = tmp;
          if ( node->properties->functionDeclaration->get_file_info()->isCompilerGenerated() )
            filename = "/__compilerGenerated";
          if ( node->properties->functionDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() )
            filename = "./__compilerGenearatedToBeUnparsed";
        }
      }else{

        ROSE_ASSERT (node->functionDeclaration != NULL);
        ROSE_ASSERT (node->functionDeclaration->get_file_info() != NULL);
        // ROSE_ASSERT (node->functionDeclaration->get_file_info()->getCurrentFilename() != NULL);
        // string filename = node->functionDeclaration->get_file_info()->getCurrentFilename();
        ROSE_ASSERT (node->functionDeclaration->get_file_info()->get_filename() != NULL);
        filename = node->functionDeclaration->get_file_info()->get_filename();
        if ( node->functionDeclaration->get_file_info()->isCompilerGenerated() )
          filename = "/__compilerGenerated";
        if ( node->functionDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() )
          filename = "./__compilerGenearatedToBeUnparsed";
        // printf ("function location = %s \n",filename.c_str());
      }
      

      // If not in the sub graph map then add it and increment the counter!
      if (graph->getSubGraphMap().find(filename) == graph->getSubGraphMap().end())
	{
            // This must be done on the DOT graph
            // graph->addSubgraph(filename);
	  graph->getSubGraphMap()[filename] = counter;
	  graph->subGraphNames[counter] = filename;
	  counter++;
	}
    }
  
  ROSE_ASSERT (graph->getSubGraphMap().size() == graph->subGraphNames.size());
  for (unsigned int i = 0; i < graph->getSubGraphMap().size(); i++)
    {
      // printf ("subgraphMap[%d] = %s \n",i,graph->subGraphNames[i].c_str());
      
      // Make sure that the filename are correct
      ROSE_ASSERT(graph->subGraphNames[i] == graph->subGraphNames[graph->getSubGraphMap()[graph->subGraphNames[i]]]);
      ROSE_ASSERT(graph->getSubGraphMap()[graph->subGraphNames[i]] == (int) i);
    }
}


int 
CallGraphDotOutput::getVertexSubgraphId ( GraphNode & v )
   {
     CallGraphNode & node = dynamic_cast<CallGraphNode &>(v);

     if ( (var_SOLVE_FUNCTION_CALLS_IN_DB == true  && node.properties->functionDeclaration)
         ||
          (var_SOLVE_FUNCTION_CALLS_IN_DB == false && node.functionDeclaration)
        )
       {

	 string filename;
         if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
           filename = node.properties->functionDeclaration->get_file_info()->get_filename();
         else
           filename = node.functionDeclaration->get_file_info()->get_filename();

	 int returnValue = callGraph.getSubGraphMap()[filename];
	 return returnValue;
       }

     // when recreating the graph, we have lost the function declaration
     return -2;
   }


void
GenerateDotGraph ( CallGraphCreate *graph, string fileName )
   {
     ROSE_ASSERT(graph != NULL);

     //     printf ("Building the GraphDotOutput object ... \n");
     CallGraphDotOutput output(*graph);
     output.writeToDOTFile(fileName, "Call Graph");
   }



#ifdef HAVE_SQLITE3
// creates a db and tables for storage of graphs
void
CallGraphDotOutput::createCallGraphSchema ( sqlite3x::sqlite3_connection& gDB, string dbName )
{
  
  //Query query = (*gDB)->getQuery();
  gDB.executenonquery("CREATE TABLE IF NOT EXISTS Graph (gid, pgid, filename TEXT);");
  gDB.executenonquery("CREATE TABLE  IF NOT EXISTS Nodes (nid TEXT, gid INTEGER, label TEXT, def INTEGER, type TEXT, scope TEXT, PRIMARY KEY (nid, scope)););");
  gDB.executenonquery("CREATE TABLE  IF NOT EXISTS Edges  (nid1 TEXT, nid2 TEXT, label TEXT, type TEXT, objClass TEXT, PRIMARY KEY (nid1, nid2, type));");

  cout << "Tables created\n";
}

// DQ (7/28/2005): Don't include the data base
// reads from DB the current maximal index of a subgraph
int
CallGraphDotOutput::GetCurrentMaxSubgraph ( sqlite3x::sqlite3_connection&  gDB )
{
  int rows = sqlite3x::sqlite3_command(gDB, "SELECT MAX(gid) FROM Graph;").executeint();
  assert(rows >= 0);

  return rows;
}
// DQ (7/28/2005): Don't include the data base

// DQ (7/28/2005): Don't include the data base
// writes the subgraph, edge, and node info to DB
void
CallGraphDotOutput::writeSubgraphToDB( sqlite3x::sqlite3_connection& gDB )
{
  // writing the Graph table
  cout << "Writing graph...\n";
  /*
  for ( CallGraphCreate::NodeIterator it = callGraph.GetNodeIterator(); !it.ReachEnd(); it++ )
    {
      CallGraphNode *node = *it;
      SgFunctionDeclaration *fctDecl = isSgFunctionDeclaration( node->properties->functionDeclaration );
      if ( fctDecl )
	cout << "Node: " << fctDecl->get_scope()->get_qualified_name().str()
	     << fctDecl->get_mangled_name().str() << "\t" << node->hasDefinition << "\n";
    }
  */

  int crtSubgraph = GetCurrentMaxSubgraph(gDB);
  for (unsigned int i=0; i < callGraph.subGraphNames.size(); i++)
    {
      string filename = callGraph.subGraphNames[i];
      string filenameWithoutPath = basename(filename.c_str());
      string subGraphName = filenameWithoutPath;
      ostringstream st;
      // TODO: define a hierarchical structure, don't use -1 as a default parent
      st << "INSERT INTO Graph VALUES (" << i + crtSubgraph << ", " << -1 << ", " << "\"" << filename << "\");";
      string sts = st.str();
      gDB.executenonquery( sts.c_str() );
    }

  cout << "Writing nodes and edges...\n";
  //----------------------------------------------------------
  // writing the Nodes & Edges tables
  for (CallGraphCreate::NodeIterator i = callGraph.GetNodeIterator(); !i.ReachEnd(); ++i)
    {
      CallGraphNode *node = *i;//.Current();
      ROSE_ASSERT ( node );
      if ( isSgFunctionDeclaration( node->properties->functionDeclaration ) )
	{
      string filename = "POINTER_FUNCTION", mnglName = "POINTER", scope = "NULL";
      int nV = getVertexSubgraphId(*node);
      if ( node->properties->functionDeclaration )
	{
	  filename = node->properties->functionDeclaration->get_file_info()->get_filename();
	  mnglName = node->properties->functionDeclaration->get_qualified_name().getString() +
	    node->properties->functionDeclaration->get_mangled_name().getString();
	  scope = node->properties->functionDeclaration->get_scope()->get_qualified_name().getString();
	}

      ostringstream st;
      ROSE_ASSERT ( node->properties->functionType );
      string mnglType = node->properties->functionType->get_mangled().getString();
      /*
      clsName = isSgClasssDefinition( node->functionDeclaration->get_scope() );
      if ( clsDef )
	{
	  clsName = clsDef->get_qualified_name().getString();
	  //cout << "CLSCLSCLS: " << clsName << "\n";
	}      
      */
      int isDef = node->isDefined() ? 1 : 0;
      string lbl = "POINTER_LABEL";

      if ( node->properties->functionDeclaration )
	lbl = node->properties->functionDeclaration->get_mangled_name().getString();

      string command = "INSERT INTO Nodes VALUES (?,?,?,?,?,?);";
  

      sqlite3_command cmd(gDB, command.c_str());

      if ( isDef )
	{
	  command = "DELETE FROM Nodes WHERE nid = \"" + mnglName + "\";";
//          gDB.executenonquery(command.c_str());

          cmd.bind(1, mnglName);
          cmd.bind(2, nV);
          cmd.bind(3, lbl);
          cmd.bind(4, 1);
          cmd.bind(5, mnglType);
          cmd.bind(6, scope);

	}
      else
	{

          cmd.bind(1, mnglName);
          cmd.bind(2, nV);
          cmd.bind(3, lbl);
          cmd.bind(4, 0);
          cmd.bind(5, mnglType);
          cmd.bind(6, scope);

	}



      cmd.executenonquery();

      cout << command << "\n";

      CallGraphCreate::EdgeIterator ei;
      CallGraphCreate::EdgeDirection dir = CallGraphCreate::EdgeOut;

      for ( ei = callGraph.GetNodeEdgeIterator(node, dir)  ; !ei.ReachEnd(); ++ei )
	{
	  CallGraphEdge *edge = ei.Current();
	  CallGraphNode *end;
	  ostringstream st;
//	  end = dynamic_cast<CallGraphNode *>( getTargetVertex(*edge) );
	  end = dynamic_cast<CallGraphNode *>(callGraph.GetEdgeEndPoint( edge, GraphAccess::EdgeOut) );

	  ROSE_ASSERT ( end );
	  string n2mnglName = "POINTER";
 
	  string typeF = "", cls = "", nid2 = "NULL";
	  if ( edge->properties->isPolymorphic )
	    {
	      ROSE_ASSERT ( isSgClassDefinition( edge->properties->invokedClass ) );
	      cls = edge->properties->invokedClass->get_qualified_name().getString();
	    }

	  ROSE_ASSERT ( end->properties->functionType );
	  typeF = edge->properties->functionType->get_mangled().getString();
	  if ( !( edge->properties->isPointer ) )
	    {
	      ROSE_ASSERT ( end->properties->functionDeclaration &&
			    end->properties->functionDeclaration == edge->properties->functionDeclaration );
	      n2mnglName = edge->properties->functionDeclaration->get_qualified_name().getString() +
		edge->properties->functionDeclaration->get_mangled_name().getString();
	    }
	  st << "INSERT INTO Edges VALUES (\"" << mnglName << "\", \"" << n2mnglName << "\", \"" << edge->label
	     << "\", \"" << edge->properties->functionType->get_mangled( ).getString() << "\", \"" << cls << "\");";
	  cout << st.str() << "\n";
	  gDB.executenonquery(st.str().c_str());

	  /*
       	  st << "INSERT INTO EdgesOld VALUES (\"" << mnglName << "\", \"" <<
	    n2mnglName << "\", \"" << edge->label << "\");";
	  gDB->execute(st.str().c_str());
	  */
	}
	}
    }
  cout << "Done writing to DB\n";
}
// DQ (7/28/2005): Don't include the data base

// DQ (7/28/2005): Don't include the data base
// remove nodes and edges of functions defined in the files stored in a specified database
// the default value for the database name is "__filter.db"
void
CallGraphDotOutput::filterNodesByDB ( string dbName, string filterDB )
{
  cout << "Filtering system calls...\n";
  sqlite3x::sqlite3_connection gDB(dbName.c_str());

  string command = "ATTACH DATABASE \"" + filterDB + "\" AS filter;";
  gDB.executenonquery(command.c_str());

  command = "CREATE TEMP TABLE gb AS SELECT g.gid as gid FROM Graph g, filter.files f WHERE f.filename = g.filename;";
  gDB.executenonquery(command.c_str());

  command = "CREATE TEMP TABLE nb AS SELECT nid FROM Nodes n, gb WHERE n.gid = gb.gid;";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Nodes WHERE nid IN (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Edges WHERE nid1 in (SELECT nb.nid FROM nb) OR nid2 in (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  gDB.close();
  cout << "Done filtering\n";
}
// DQ (7/28/2005): Don't include the data base

// DQ (7/28/2005): Don't include the data base
// remove nodes and edges of functions defined in the specified file
void
CallGraphDotOutput::filterNodesByFilename ( string dbName, string filterFile )
{
  cout << "Filtering system calls...\n";
  sqlite3x::sqlite3_connection gDB(dbName.c_str());
  string command;

  command = "CREATE TEMP TABLE gb AS SELECT g.gid as gid FROM Graph g WHERE g.filename = " + filterFile + ";";
  gDB.executenonquery(command.c_str());

  command = "CREATE TEMP TABLE nb AS SELECT nid FROM Nodes n, gb WHERE n.gid = gb.gid;";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Nodes WHERE nid IN (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Edges WHERE nid1 in (SELECT nb.nid FROM nb) OR nid2 in (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  gDB.close();
  cout << "Done filtering\n";
}
// DQ (7/28/2005): Don't include the data base

// DQ (7/28/2005): Don't include the data base
// remove nodes and edges of functions with a specific name
void
CallGraphDotOutput::filterNodesByFunction ( string dbName, SgFunctionDeclaration *function )
{
  ROSE_ASSERT ( function );
  cout << "Filtering system calls...\n";
  string functionName = function->get_qualified_name().getString() + function->get_mangled_name().getString();

  sqlite3x::sqlite3_connection gDB(dbName.c_str());

  string command;

  command = "DELETE FROM Nodes WHERE nid = " + functionName + ";";
  gDB.executenonquery(command.c_str());

  command =  "DELETE FROM Edges WHERE nid1 = " + functionName + "  OR nid2 = " + functionName + ";";
  gDB.executenonquery(command.c_str());

  gDB.close();
  cout << "Done filtering\n";
}
// DQ (7/28/2005): Don't include the data base

// DQ (7/28/2005): Don't include the data base
// solve function pointers based on type
// TODO: virtual function pointers are not properly solved ( they are treated as regular member function pointers )
void
CallGraphDotOutput::solveFunctionPointers( string dbName )
{
  cout << "Solving function pointers...\n";

  sqlite3x::sqlite3_connection gDB(dbName.c_str());

  string command = "";
  command = "INSERT INTO Edges SELECT e.nid1, nid, n1.label, e.type, e.objClass from Nodes n1, Edges e WHERE "
    "e.nid2 = \"POINTER\" AND e.type = n1.type;";// AND e.objClass = n1.scope;";
  gDB.executenonquery(command.c_str());
 
  command = "DELETE FROM Edges WHERE nid2 = \"POINTER\" AND objClass = \"\";";
  gDB.executenonquery(command.c_str());

  cout << command << "\t" << "CLEANUP\n";

  gDB.close();
  cout << "Done with function pointers\n";
}

// DQ (7/28/2005): Don't include the data base
// solve virtual function calls ( but not via pointers )
void
CallGraphDotOutput::solveVirtualFunctions( string dbName, string dbHierarchy )
{
  cout << "Solving virtual function calls...\n";
  sqlite3x::sqlite3_connection gDB(dbName.c_str());

  string command = "";
  // determine descendants of edges.objClass
  {
   

    cout << "Classes with virtual functions called:\n";

    //rows->showResult();

    std::vector<std::vector<std::string> > objClassEdges;
  
    {
      command = "SELECT * from Edges WHERE objClass <> \"\";"; // AND nid2 <> \"POINTER\";";
      sqlite3_command cmd(gDB, command.c_str());

      sqlite3x::sqlite3_reader r = cmd.executereader();

      while( r.read() )
      {
        std::vector<std::string> objClassEdgesElem;
        string nid1     = r.getstring(0); objClassEdgesElem.push_back(nid1);
        string nid2     = r.getstring(1); objClassEdgesElem.push_back(nid2);
        string lbl2     = r.getstring(2); objClassEdgesElem.push_back(lbl2);
        string fType    = r.getstring(3); objClassEdgesElem.push_back(fType);
        string objClass = r.getstring(4); objClassEdgesElem.push_back(objClass);

        objClassEdges.push_back(objClassEdgesElem);
      }
    }

    command = "DELETE FROM Edges WHERE objClass <> \"\";";// AND nid2 <> \"POINTER\";";
    gDB.executenonquery( command.c_str() );

    command = "DELETE FROM Nodes WHERE nid = \"DUMMY\";";
    gDB.executenonquery( command.c_str() );


    for(unsigned int i = 0 ; i < objClassEdges.size(); i++)
    {

      string nid1     = objClassEdges[i][0];
      string nid2     = objClassEdges[i][1];
      string lbl2     = objClassEdges[i][2];
      string fType    = objClassEdges[i][3];
      string objClass = objClassEdges[i][4];

      ClassHierarchyWrapper clsHierarchy ( "ClassHierarchy" );
      std::list<string> subclasses = clsHierarchy.getSubclasses( objClass );
      subclasses.push_back( objClass );
      for ( std::list<string>::iterator it = subclasses.begin(); it != subclasses.end(); it++ )
      {
        if ( nid2 == "POINTER" )
          command = "SELECT * FROM Nodes WHERE scope = \"" + *it + "\" AND type = \"" + fType + "\";";
        else
          command = "SELECT * FROM Nodes WHERE scope = \"" + *it + "\" AND label = \"" + lbl2 + "\";";// AND def = 1;";

        sqlite3_command cmd(gDB, command.c_str());

        sqlite3x::sqlite3_reader r = cmd.executereader();

        cout << "Now executing: " << command;

        std::vector<string> inserts;
        while( r.read() )
        {
          string nnid  = r.getstring(0);
          //	      int gid = (*itr)[1];
          string lbl   = r.getstring(2);
          //	      int is_def = (*itr)[3];
          string fType = r.getstring(4) ;
          string scope = r.getstring(3);
          command = "INSERT INTO Edges VALUES ( \"" + nid1 + "\", \"" + nnid + "\", \"" + lbl + "\", \"" + fType +
            "\", \"\" );";
          inserts.push_back(command);
        }

        for(unsigned int i = 0 ; i < inserts.size() ; i++)
          gDB.executenonquery(inserts[i].c_str());
      }
    }
  }


  cout << "Done with virtual functions\n";
}

// DQ (7/28/2005): Don't include the data base
// remove nodes and edges of functions defined in the files of a specified directory
void
CallGraphDotOutput::filterNodesByDirectory ( string dbName, string directory )
{
  cout << "Filtering system calls...\n";
  sqlite3x::sqlite3_connection gDB(dbName.c_str());

  string command;

  command = "CREATE TEMP TABLE gb AS SELECT g.gid as gid FROM Graph g WHERE g.filename LIKE \"" + directory + "%\";";
  gDB.executenonquery(command.c_str());

  command = "CREATE TEMP TABLE nb AS SELECT nid FROM Nodes n, gb WHERE n.gid = gb.gid;";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Nodes WHERE nid IN (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  command = "DELETE FROM Edges WHERE nid1 in (SELECT nb.nid FROM nb) OR nid2 in (SELECT nb.nid FROM nb);";
  gDB.executenonquery(command.c_str());

  gDB.close();
  cout << "Done filtering\n";
}
// DQ (7/28/2005): Don't include the data base

// DQ (7/28/2005): Don't include the data base
// generate a graph from the DB
// TODO: as is now, clustering info is lost
CallGraphCreate *
CallGraphDotOutput::loadGraphFromDB ( string dbName )
{
  cout << "Loading...\n";
  CallGraphCreate *graph = new CallGraphCreate();

  sqlite3x::sqlite3_connection gDB(dbName.c_str());

  string loadNodes = "SELECT * FROM Nodes;",
//    loadGraph = "SELECT * FROM Graph;",
    loadEdges = "SELECT * FROM Edges;";

  Rose_STL_Container<CallGraphNode *> nodeList;

  {
    sqlite3_command cmd(gDB, loadNodes.c_str());
    sqlite3x::sqlite3_reader r = cmd.executereader();


  
    while(r.read()) 
    {
      string nid   = r.getstring(0);
      string label = r.getstring(2);
      int is_def   = boost::lexical_cast<int>(r.getstring(3));
      string typeF = r.getstring(4);
      string scope = r.getstring(5);

      CallGraphNode *n = new CallGraphNode( nid, NULL, NULL, is_def, false, false, NULL );

      graph->addNode(n);
      //     cout << "Node pushed: " << nid << "\t" << n << "\n";
      nodeList.push_back(n);
    }
  }

  sqlite3_command cmd(gDB, loadNodes.c_str());
  sqlite3x::sqlite3_reader r = cmd.executereader();

  while(r.read())
    {
      string nid1 = r.getstring(0),
	nid2  = r.getstring(1),
	label = r.getstring(2);

      // find end points of edges, by their name
      CallGraphNode* startingNode = findNode(nodeList, nid1);
      CallGraphNode* endNode = findNode(nodeList, nid2);
      assert(startingNode);

      if (endNode)
	{
	  //cout << "Nodes retrieved: " << nid1 << "\t" << startingNode << "\t"
	  //   << nid2 << "\t" << endNode << "\n";
	  CallGraphEdge *e = new CallGraphEdge(label);
          if(graph->edgeExist(startingNode,endNode)==false)
	     graph->addEdge(startingNode, endNode, e);
	}
    }

  cout << "After recreating the graph\n";
  return graph;
}

// save graph to DB,  i == 0 - don't generate new dbase
int
CallGraphDotOutput::writeToDB ( int i, string dbName )
{
  sqlite3x::sqlite3_connection gDB(dbName.c_str());
  createCallGraphSchema(gDB, dbName);
  writeSubgraphToDB(gDB);
  return 0;
}


//AS(5/1/2009): Largely a code copy of the other writeSubgraphToDB(..) function.
//Need to experiment to see if the other one will do the job.
#if 0
// DQ (7/28/2005): Don't include the data base
// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
// writes the subgraph, edge, and node info to DB
void
CallGraphDotOutput::writeSubgraphToDB( GlobalDatabaseConnection *gDB )
{
  // writing the Graph table
  cout << "Writing graph...\n";
  int crtSubgraph = GetCurrentMaxSubgraph(gDB);
  for (unsigned int i=0; i < callGraph.subGraphNames.size(); i++)
    {
      string filename = callGraph.subGraphNames[i];
      string filenameWithoutPath = basename(filename.c_str());
      string subGraphName = filenameWithoutPath;
      ostringstream st;
      // TODO: define a hierarchical structure, don't use -1 as a default parent
      st << "INSERT INTO Graph VALUES (" << i + crtSubgraph << ", " << -1 << ", " << "\"" << filename << "\");";
      string sts = st.str();
      gDB->execute(sts.c_str());
    }

  cout << "Writing nodes and edges...\n";
  //----------------------------------------------------------
  // writing the Nodes & Edges tables
  for (CallGraphCreate::NodeIterator i = callGraph.GetNodeIterator(); !i.ReachEnd(); ++i)
    {
      CallGraphNode *node = i.Current();
      assert(node);
      string filename = node->functionDeclaration->get_file_info()->get_filename();
      
      int nV = getVertexSubgraphId(*node);
      
      ostringstream st;
      string clsName = "";
      SgClassDefinition *clsDef = isSgClassDefinition( node->functionDeclaration->get_scope() );

      // get_scope returns NULL when it shouldn't....
      if ( clsDef )
	{
	  clsName = clsDef->get_qualified_name().getString();
	  //cout << "CLSCLSCLS: " << clsName << "\n";
	}
      
      /*get_mangled_name()*/
      string mnglName = node->functionDeclaration->get_qualified_name().getString() +
	node->functionDeclaration->get_mangled_name().getString();
      int isDef = node->isDefined() ? 1 : 0;
      string command;
      
      if ( isDef )
	{
	  command = "DELETE FROM Nodes WHERE nid = \"" + mnglName + "\";";
	  Query *q = gDB->getQuery();
	  q->set(command.c_str());
	  gDB->execute();

	  st << "INSERT INTO Nodes VALUES (\"" << mnglName  << "\", " << nV << ", \""
	     << node->toString() << "\", \"" << 1 << "\");";
	  command = st.str();
	}
      else
	{
	  st << "INSERT INTO Nodes VALUES (\"" << mnglName  << "\", " << nV << ", \""
	     << node->toString() << "\", \"" << 0 << ", \");";
	  command = st.str();
	}

      Query *q = gDB->getQuery();
      q->set( command.c_str() );
      cout << command << "\n";
      gDB->execute();
      
      CallGraphCreate::EdgeIterator ei;
      CallGraphCreate::EdgeDirection dir = CallGraphCreate::EdgeOut;
      
      for (ei = callGraph.GetNodeEdgeIterator(node, dir) ; !ei.ReachEnd(); ++ei)
	{
	  CallGraphEdge *edge = ei.Current();
	  CallGraphNode *end;
	  ostringstream st;

	  // string n1mnglName = node->functionDeclaration->get_mangled_name().getString();
	  string clsName = "";
	  assert ( node->functionDeclaration );
	  SgClassDefinition *clsDef = isSgClassDefinition( node->functionDeclaration->get_scope() );
	  
	  // get_scope returns NULL... is it NOT virtual???
	  if ( clsDef )
	    clsName = clsDef->get_qualified_name().getString();
	  
	  end = dynamic_cast<CallGraphNode *>(getTargetVertex(*edge));
	  assert(end);
	  string n2mnglName = end->functionDeclaration->get_qualified_name().getString() +
	    end->functionDeclaration->get_mangled_name().getString();
	  
	  st << "INSERT INTO Edges VALUES (\"" << mnglName << "\", \"" <<
	    n2mnglName << "\", \"" << edge->label << "\");";
	  gDB->execute(st.str().c_str());
	}
    }
  cout << "Done writing to DB\n";
}
// DQ (7/28/2005): Don't include the data base
#endif

#endif
