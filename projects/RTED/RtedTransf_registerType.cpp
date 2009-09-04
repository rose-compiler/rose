#include <rose.h>
#include <string>
#include <boost/foreach.hpp>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"


using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


bool
RtedTransformation::hasClassConstructor(SgClassDeclaration* classdec) {
  bool constr=false;
  std::string classname = classdec->get_name().str();
  // find all functions in class and if there is a function that
  // has the same name as the class --- bingo

  vector<SgNode*> results = NodeQuery::querySubTree(classdec,V_SgMemberFunctionDeclaration);
  vector<SgNode*>::const_iterator classIt = results.begin();
  for (;classIt!=results.end();classIt++) {
    SgMemberFunctionDeclaration* mf = isSgMemberFunctionDeclaration(*classIt);
    std::string funcname = mf->get_name().str();
    //cerr << " ++++++++++++++++ comparing class : " << classname << " against func: " << funcname << endl;
    if (classname==funcname) {
      constr=true;
      break;
    }
  }
  return constr;
}

void RtedTransformation::visit_isClassDefinition(SgClassDefinition* cdef) {
  cerr << "Found class definition : " << cdef->unparseToString() << endl;
  vector<RtedClassElement*> elements;

  Rose_STL_Container<SgDeclarationStatement*> members = cdef->get_members();
  Rose_STL_Container<SgDeclarationStatement*>::const_iterator itMem = members.begin();
  for (;itMem!=members.end();++itMem) {
    SgDeclarationStatement* sgElement = *itMem;
    ROSE_ASSERT(sgElement);
    SgVariableDeclaration* varDecl = isSgVariableDeclaration(sgElement);
    if (varDecl) {
      bool not_static = !(
			  varDecl -> get_declarationModifier().get_storageModifier().isStatic()
			  );

      if( not_static ) {
	Rose_STL_Container<SgInitializedName*> vars = varDecl->get_variables();
	Rose_STL_Container<SgInitializedName*>::const_iterator itvar = vars.begin();
	for (;itvar!=vars.end();++itvar) {
	  SgInitializedName* initName = *itvar;
	  string name = initName->get_mangled_name();
	  string type = initName->get_type()->class_name();

	  RtedClassElement* el;
	  if( isSgArrayType( initName -> get_type() )) {
	    RTedArray* arrayRted = new RTedArray( true, initName, NULL, false );
	    populateDimensions( arrayRted, initName, isSgArrayType( initName -> get_type() ));
	    el = new RtedClassArrayElement( name, type, sgElement, arrayRted );
	  } else {
	    el = new RtedClassElement(name,type,sgElement);
	  }
	  elements.push_back(el);
	}
      }
    } else {
      // TODO 2: handle this case
      cerr << " Declaration not handled : " << sgElement->class_name() << endl;
    }
  }

  RtedClassDefinition* cd = new RtedClassDefinition(cdef,
						    cdef->get_mangled_name().str(),
						    cdef->get_declaration()->get_type()->class_name(),
						    elements.size(),
						    buildSizeOfOp( cdef->get_declaration()->get_type() ),
						    elements);
  std::map<SgClassDefinition*,RtedClassDefinition*>::const_iterator it =
    class_definitions.find(cdef);
  if (it==class_definitions.end())
    class_definitions[cdef]=cd;
}


void RtedTransformation::insertRegisterTypeCall(RtedClassDefinition* rtedClass
						) {
  ROSE_ASSERT(rtedClass);
  SgStatement* stmt;


  bool global_stmt = false;

  // FIXME 2: This will cause multiple invocations to registertype
  //    e.g. int foo() { struct t { int x; } v; }
  //         int main() { foo(); foo(); }
  //
  // we want to call register type before type is used, but where it's in
  // scope
  stmt = getSurroundingStatement( rtedClass->classDef );
  while(  isSgClassDefinition( stmt )
          || isSgClassDeclaration( stmt )) {

    stmt = isSgStatement( stmt -> get_parent());
  }
  if( !stmt || isSgGlobal(stmt) ) {
    stmt = mainFirst;
    global_stmt = true;
  }

  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    string name = rtedClass->manglClassName;
    string typeC = rtedClass->classType;

    // FIXME 2: we do this all over the place, but it may not work with
    // namespaces
    // if( isSgGlobal( scope )) {
    //    scope = mainBody;
    //    global_stmt = true;
    //}


    ROSE_ASSERT(scope);
    if ( isNormalScope( scope )) {
      // insert new stmt before first Statement in main
  
      /*
      //instrumented calls for all defined structs/classes:
      RTSI_registerType("A", //name of class
      sizeof(A),
      "b", "SgInt", offsetof(A,b)
      "c", "SgChar",offsetof(A,c) ); //for each member the name ,type and offset

      RTSI_registerType("B", sizeof(B), 
      "instanceOfA", "A", offsetof(B,instanceOfA),
      "d", "SgDouble", offsetof(B,d) );
      */

      int elements = rtedClass->nrOfElements; // elements passed to function
      elements*=6; // for each element pass name, type, basetype, indirection_level and offset
      elements+=3; // ClassName , ClassType and sizeOfClass

      BOOST_FOREACH( RtedClassElement* element, rtedClass -> elements ) {
	elements += element -> extraArgSize();
      }

      // tps (09/04/2009) added 3 parameters, filename, line, linetransformed
      SgExpression* nrElements = buildIntVal(elements+3);


      SgExprListExp* arg_list = buildExprListExp();
      appendExpression(arg_list, nrElements);

      SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
      SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
      appendExpression(arg_list, filename);
      appendExpression(arg_list, linenr);
      SgExpression* linenrTransformed = buildString("x%%x");
      appendExpression(arg_list, linenrTransformed);

      
      appendExpression(arg_list, buildString(name));
      appendExpression(arg_list, buildString(typeC));
      appendExpression(arg_list, rtedClass->sizeClass);
      
      // go through each element and add name, type, basetype, offset and size
      std::vector<RtedClassElement*> elementsC = rtedClass->elements;
      std::vector<RtedClassElement*>::const_iterator itClass = elementsC.begin();
      for (;itClass!=elementsC.end();++itClass) {
	RtedClassElement* element = *itClass;
	string manglElementName = element->manglElementName;
	SgDeclarationStatement* sgElement = element->sgElement;
	//SgExpression* sgElementCopy = deepCopy(sgElement);
	SgExpression* elemName = buildString(manglElementName);

	// FIXME 1: This will not work for references.  Consider:
	//
	//    #include <iostream>
	//    using namespace std;
	//
	//    int i;
	//    struct T {
	//        int p;
	//        int& r;
	//
	//        T() : r(i) {}
	//    };
	//
	//    int main() { cout << offsetof( T, r ) << endl; }
	//
	// and see discussion (esp. follow ups) at:
	//    http://gcc.gnu.org/ml/gcc/2003-11/msg00279.html
	//
	// build a function call for offsetof(A,d);
	appendExpression(arg_list, elemName);

	// build  (size_t )(&( *((struct A *)0)).x);
	ROSE_ASSERT(rtedClass->classDef);
	// cerr << " Type: " <<  rtedClass->classDef->get_declaration()->get_type()->class_name() << endl;

	// search for classDef in classesNamespace that contains all introduced RTED:Classes
	std::map<SgClassDefinition*, SgClassDefinition*>::const_iterator
	  cit = classesInRTEDNamespace.find(rtedClass->classDef);
	SgClassDefinition* rtedModifiedClass = NULL;
	if (cit!=classesInRTEDNamespace.end())
	  rtedModifiedClass = cit->second;
	//		 cerr << " +++++++++++++ Looking for class : " << rtedClass->classDef->get_declaration()->get_qualified_name().str()
	//	 << "    found : " << rtedModifiedClass << "   " << rtedModifiedClass->get_declaration()->get_qualified_name().str() << endl;

	// tps (09/04/2009) : Added support to call offset of on C++ classes
	SgExpression* nullPointer = NULL;
	ROSE_ASSERT(rtedClass->classDef->get_declaration());
	bool classHasConstructor=hasClassConstructor(rtedClass->classDef->get_declaration());
	if (rtedModifiedClass==NULL) {
	  // this is a C or C++ class in a source file without constructor
	  if ( classHasConstructor==false)
	    nullPointer = buildCastExp(buildIntVal(0),
				       buildPointerType(rtedClass->classDef->get_declaration()->get_type()));
	  else // or C++ class in a source file with constructor!
	    nullPointer = buildCastExp(buildIntVal(1),
				       buildPointerType(rtedClass->classDef->get_declaration()->get_type()));
	} else {
	  // this is a C++ class in a header file 
	  ROSE_ASSERT(rtedModifiedClass);
	  nullPointer = buildCastExp(buildIntVal(1),
				     buildPointerType(rtedModifiedClass->get_declaration()->get_type()));
	}

	SgExpression* derefPointer = buildPointerDerefExp(nullPointer);
	SgVariableDeclaration* varDecl =	isSgVariableDeclaration(sgElement);
	if (varDecl) {
	  SgVarRefExp* varref = buildVarRefExp(varDecl);

	  // append the base type (if any) of pointers and arrays
	  SgType* type = varDecl->get_variables()[ 0 ]->get_type();
	  appendTypeInformation( type, arg_list, true, false );

	  SgExpression* dotExp = buildDotExp(derefPointer,varref);
	  SgExpression* andOp = buildAddressOfOp(dotExp);
	  SgExpression* castOp = NULL;
	  if (rtedModifiedClass==NULL && classHasConstructor==false)
	    castOp = buildCastExp(andOp, size_t_member);
	  else {
	    SgExpression* minus = buildSubtractOp(andOp, buildIntVal(1));
	    castOp = buildCastExp(minus, size_t_member);
	  }

	  appendExpression(arg_list, castOp);
	  appendExpression(arg_list, buildSizeOfOp( dotExp ));

	  // add extra type info (e.g. array dimensions)
	  element -> appendExtraArgs( arg_list );
	} else {
	  cerr << " Declarationstatement not handled : " << sgElement->class_name() << endl;

	}
      }


      ROSE_ASSERT(roseRegisterTypeCall);
      //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
      SgFunctionRefExp* memRef_r = buildFunctionRefExp(  roseRegisterTypeCall);
      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
							    arg_list);
      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);


      if( global_stmt ) {
	mainBody -> prepend_statement( exprStmt );
      } else {
	// insert it after if it is a classdefinition, otherwise 
	// if it is a class reference, we do it before
	// check for assign initializer
	insertStatementAfter( isSgStatement( stmt ), exprStmt );
	//insertStatementBefore( isSgStatement( stmt ), exprStmt );
      }
    }
    else {
      cerr
	<< "RuntimeInstrumentation :: Surrounding Block is not Block! : "
	<< name << " : " << scope->class_name() << endl;
      ROSE_ASSERT(false);
    }
  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << stmt->class_name() << endl;
    ROSE_ASSERT(false);
  }

}
