#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <boost/foreach.hpp>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

bool RtedTransformation::hasClassConstructor(SgClassDeclaration* classdec) {
	bool constr = false;
	std::string classname = classdec->get_name().str();
	// find all functions in class and if there is a function that
	// has the same name as the class --- bingo

	vector<SgNode*> results = NodeQuery::querySubTree(classdec,
			V_SgMemberFunctionDeclaration);
	vector<SgNode*>::const_iterator classIt = results.begin();
	for (; classIt != results.end(); classIt++) {
		SgMemberFunctionDeclaration* mf = isSgMemberFunctionDeclaration(
				*classIt);
		std::string funcname = mf->get_name().str();
		//cerr << " ++++++++++++++++ comparing class : " << classname << " against func: " << funcname << endl;
		if (classname == funcname) {
			constr = true;
			break;
		}
	}
	return constr;
}

void RtedTransformation::visit_isClassDefinition(SgClassDefinition* cdef) {
	//cerr << "Found class definition : " << cdef->unparseToString() << endl;
	vector<RtedClassElement*> elements;

	// We want to skip compiler generated template instantiation classes.
	// Depending on compiler version, etc., we may visit classes such as:
	//
	//    class std::basic_ostream< char, std::char_traits< char >>::sentry
	//    struct std::basic_string< char, std::char_traits< char >, allocator< char >>::_Rep
	//
	// Since we're not intstrumenting these classes, we shouldn't register their
	// types.
	if (cdef -> get_file_info() -> isCompilerGenerated()
			&& cdef -> get_declaration() -> get_file_info() -> isCompilerGenerated()) {

		//cerr << "Skipping compiler generated class" << cdef->unparseToString() <<endl;
		return;
	}
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
			// for now, change all modifiers from private to public
			SgAccessModifier& am = varDecl->get_declarationModifier().get_accessModifier();
			if (am.isPrivate() || am.isProtected()) am.setPublic();

			if( not_static ) {

				Rose_STL_Container<SgInitializedName*> vars = varDecl->get_variables();
				Rose_STL_Container<SgInitializedName*>::const_iterator itvar = vars.begin();
				for (;itvar!=vars.end();++itvar) {
					SgInitializedName* initName = *itvar;
					string name = initName->get_mangled_name();
					string type = initName->get_type()->class_name();
					VariantT variant = initName->get_type()->variantT();
					//cerr << " *********** VarientT = " << getSgVariant(variant) << endl;

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
			//cerr << " Declaration not handled : " << sgElement->class_name() << endl;
		}
	}

	ROSE_ASSERT(cdef);
	ROSE_ASSERT(cdef->get_declaration()->get_type());
	string classmanglnametype=cdef->get_declaration()->get_type()->class_name();
//	string classmanglname=cdef->get_mangled_name().str(); //problem line!!! need to debug more. fails under 32bit
	SgName classmname = cdef->get_qualified_name();
	string classmanglname=classmname.str();
	int elementssize = elements.size();
	SgExpression* sizeofop = buildSizeOfOp( cdef->get_declaration()->get_type() );
	std::vector<RtedClassElement*> _elements = elements;
	RtedClassDefinition* cd = new RtedClassDefinition(cdef,
			classmanglname,
			classmanglnametype,
			elementssize,
			sizeofop,
			_elements);
	std::map<SgClassDefinition*,RtedClassDefinition*>::const_iterator it =
	class_definitions.find(cdef);
	if (it==class_definitions.end()) {
		cerr << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Found ClassDefinition " << cdef->get_qualified_name().str() << endl;
		cerr << ">>>>>>>>>>>>>>> cdef = " << cdef << endl;
		class_definitions[cdef]=cd;
	}
}

void RtedTransformation::insertRegisterTypeCall(RtedClassDefinition* rtedClass) {
	ROSE_ASSERT(rtedClass);
	SgStatement* stmt;
	SgScopeStatement* scope;

	bool global_stmt = false;

	// FIXME 2: This will cause multiple invocations to registertype
	//    e.g. int foo() { struct t { int x; } v; }
	//         int main() { foo(); foo(); }
	//
	// we want to call register type before type is used, but where it's in
	// scope
	stmt = getSurroundingStatement(rtedClass->classDef);
	SgStatement* origStmt = stmt;
	//cerr <<"@@@@ registerType : " << stmt->unparseToString() << endl;
	//	abort();

	SgScopeStatement* globalScope = NULL;
	scope = stmt -> get_scope();
	//cerr << "  the surrounding statement is (1): " << stmt->class_name()
	//		<< " for : "
	//		<< rtedClass->classDef->get_declaration()->get_name().str() << endl;
	while (isSgClassDefinition(stmt) || isSgClassDeclaration(stmt)
	// for nested classes, we care about the scope of the outermost class
			|| isSgClassDeclaration(scope) || isSgClassDefinition(scope)) {
		origStmt = stmt;
		stmt = isSgStatement(stmt -> get_parent());
		scope = stmt -> get_scope();
		//cerr << "  the surrounding statement is (2): " << stmt->class_name()
		//		<< endl;
	}
	cerr << " -----------> stmt : " << stmt->unparseToString() << " ::::: "<<
		stmt->class_name() <<  " isSgGlobal(stmt) : " << isSgGlobal(stmt) <<
			"   getScope: " << isSgGlobal(stmt->get_scope()) << endl;
	if (!stmt || isSgGlobal(stmt)    || isSgGlobal(stmt->get_scope())
	// catch the case where a class (or union) is within another class - this is consider	to be global
			|| (isSgVariableDeclaration(stmt) && isSgClassDefinition(
					stmt->get_parent()))) {
		globalScope = scope;
		if (stmt) {
			// the classdefinition is global --
			// is there a global variable?
			SgVariableDeclaration* stmtVar = getGlobalVariableForClass(
					isSgGlobal(globalScope), isSgClassDeclaration(origStmt));
			if (stmtVar) {
				stmt = appendToGlobalConstructor(stmt->get_scope(),
						rtedClass->classDef->get_declaration()->get_name());
				scope = isSgScopeStatement(globalFunction);
				ROSE_ASSERT(scope);
				cerr << "Current scope = " << scope->class_name() << endl;
				ROSE_ASSERT(isSgStatement(stmt));
				ROSE_ASSERT(isNormalScope( scope));
				global_stmt = true;
			} else {
				stmt = mainFirst;
				scope = stmt->get_scope();
				global_stmt = false;
			}
		} else {
			stmt = mainFirst;
			scope = stmt->get_scope();
			global_stmt = false;
		}
	} else {
		cerr << "  the surrounding statement is (3): " << stmt->class_name()
				<< endl;
		//    stmt = mainFirst;
		scope = stmt->get_scope();
	}

	if (isSgStatement(stmt)) {

		string name = rtedClass->manglClassName;
		string typeC = rtedClass->classType;

		int ctype = rtedClass->classDef->get_declaration()->get_class_type();
		string isunionType = "0";
		//cerr << " Register Type: " << typeC << endl;
		if (ctype == SgClassDeclaration::e_union)
			isunionType = "1";
		// FIXME 2: we do this all over the place, but it may not work with
		// namespaces
		// if( isSgGlobal( scope )) {
		//    scope = mainBody;
		//    global_stmt = true;
		//}


		ROSE_ASSERT(scope);
		if (isNormalScope(scope)) {// || isSgClassDefinition(scope)) {
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
			elements *= 6; // for each element pass name, type, basetype, indirection_level and offset
			elements += 4; // ClassName , ClassType and Union? ,sizeOfClass

			BOOST_FOREACH( RtedClassElement* element, rtedClass -> elements )
{			elements += element -> extraArgSize();
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
		appendExpression(arg_list, buildString(isunionType));
		appendExpression(arg_list, rtedClass->sizeClass);

		// search for classDef in classesNamespace that contains all introduced RTED:Classes
		std::map<SgClassDefinition*, SgClassDefinition*>::const_iterator
		cit = classesInRTEDNamespace.find(rtedClass->classDef);
		SgClassDefinition* rtedModifiedClass = NULL;
		if (cit!=classesInRTEDNamespace.end()) {
			rtedModifiedClass = cit->second;
			cerr << "Found rtedModifiedClass : " << rtedModifiedClass->get_qualified_name().str() << endl;
			cerr << "Found rtedModifiedClass Declaration: " << rtedModifiedClass->get_declaration()->unparseToString()  << endl;
			cerr << "Found rtedModifiedClass Declaration Type: " << rtedModifiedClass->get_declaration()->get_type()->unparseToString() << endl;
			cerr << "Address of rtedModifiedClass : " << rtedModifiedClass->get_declaration()->get_type() << endl;
			cerr << "Address of defining rtedModifiedClass : " << rtedModifiedClass->get_declaration()->get_definingDeclaration() << endl;
			cerr << "Address of nondefining rtedModifiedClass : " << rtedModifiedClass->get_declaration()->get_firstNondefiningDeclaration() << endl;
			cerr << "Address of rtedModifiedClass->get_declaration()->get_type()->get_declaration() : " << rtedModifiedClass->get_declaration()->get_type()->get_declaration()
			<< "    Addr of rtedModifiedClass->get_declaration()->get_firstNondefiningDeclaration() : " << rtedModifiedClass->get_declaration()->get_firstNondefiningDeclaration() << endl;
			ROSE_ASSERT(rtedModifiedClass->get_declaration()->get_type()->get_declaration() == rtedModifiedClass->get_declaration()->get_firstNondefiningDeclaration());
		} else
		cerr << "Did not find rtedModifiedClass using : " << rtedClass->classDef->get_qualified_name().str() << endl;

		// go through each element and add name, type, basetype, offset and size
		std::vector<RtedClassElement*> elementsC = rtedClass->elements;
		//if (rtedModifiedClass) elementsC = rtedModifiedClass->elements;
		std::vector<RtedClassElement*>::const_iterator itClass = elementsC.begin();
		for (;itClass!=elementsC.end();++itClass) {
			RtedClassElement* element = *itClass;
			string manglElementName = element->manglElementName;
			SgDeclarationStatement* sgElement = element->sgElement;
			//SgExpression* sgElements = deepCopy(sgElement);
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


			//		 cerr << " +++++++++++++ Looking for class : " << rtedClass->classDef->get_declaration()->get_qualified_name().str()
			//	 << "    found : " << rtedModifiedClass << "   " << rtedModifiedClass->get_declaration()->get_qualified_name().str() << endl;

			// tps (09/04/2009) : Added support to call offset of on C++ classes
			SgExpression* nullPointer = NULL;
			ROSE_ASSERT(rtedClass->classDef->get_declaration());
			bool classHasConstructor=hasClassConstructor(rtedClass->classDef->get_declaration());
			classHasConstructor=false;
			if (rtedModifiedClass==NULL) {
				cerr << "rtedModifiedClass==NULL" << endl;
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
				nullPointer = buildCastExp(buildIntVal(0),
						buildPointerType(rtedModifiedClass->get_declaration()->get_type()));
				cerr << "################ SOMETHING WRONG ? rtedModifiedClass :" << rtedModifiedClass
				<< "   rtedClass : " << rtedClass->classDef << endl;
				cerr << "What is the type : " << rtedModifiedClass->get_declaration()->get_type()->unparseToString() << endl;
				//ROSE_ASSERT(rtedModifiedClass->get_declaration()!=rtedClass->classDef->get_declaration());
				//ROSE_ASSERT(rtedModifiedClass->get_declaration()==rtedClass->classDef->get_declaration());
				//abort();
			}

			SgExpression* derefPointer = buildPointerDerefExp(nullPointer);
			SgVariableDeclaration* varDecl = isSgVariableDeclaration(sgElement);
			SgClassDefinition* cdeftest = isSgClassDefinition(varDecl->get_parent());
			cerr << " varDecl->get_parent() : " << varDecl->get_parent()->class_name() << endl;
			cerr << " rtedModifiedClass : " << rtedModifiedClass << endl;
			cerr << " cdeftest : " << cdeftest << endl;
			ROSE_ASSERT(cdeftest);
			//if (rtedModifiedClass)
			//ROSE_ASSERT(cdeftest==rtedModifiedClass);
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
					SgExpression* minus = buildSubtractOp(andOp, buildIntVal(0));
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
		SgFunctionRefExp* memRef_r = buildFunctionRefExp( roseRegisterTypeCall);
		SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
				arg_list);
		SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
		//cerr <<"@@@@@ Creating regType for : " << stmt->unparseToString() << endl;
		//abort();
		if (origStmt->get_file_info()->isCompilerGenerated())
		return;
		if( global_stmt ) {
			globalFunction->prepend_statement(exprStmt);
			ROSE_ASSERT(isSgGlobal(globalScope));
			ROSE_ASSERT(isSgClassDeclaration(origStmt));
			//mainBody -> prepend_statement( exprStmt );
			//next we need to find the variableDeclaration
			// for that class and call:
			SgVariableDeclaration* stmtVar =
			getGlobalVariableForClass(isSgGlobal(globalScope),isSgClassDeclaration(origStmt));
			ROSE_ASSERT(stmtVar);
			//appendGlobalConstructor(globalScope,origStmt);
			appendGlobalConstructor(globalScope,stmtVar);
			appendGlobalConstructorVariable(globalScope,stmtVar);
		}
		else {
			// insert it after if it is a classdefinition, otherwise
			// if it is a class reference, we do it before
			// check for assign initializer
			SgVariableDeclaration* vardecl = isSgVariableDeclaration(stmt);
			bool insertBefore=true;
			if (vardecl) {
				SgDeclarationStatement * sdecl = vardecl->get_baseTypeDefiningDeclaration ();
				if (isSgClassDeclaration(sdecl))
				insertBefore=false;
			}
			// is it a struct (classdecl) below the variable? then append afterwards
			// else before
			cerr << "isSgClassDefinition(stmt): " << isSgClassDefinition(stmt) <<
			" isSgClassDeclaration(stmt): " << isSgClassDeclaration(stmt) <<
			"   :: " << stmt->class_name() <<
			"   " << stmt->unparseToString()<<endl;
			//			abort();
			if (insertBefore)
			insertStatementBefore( isSgStatement( stmt ), exprStmt );
			else
			insertStatementAfter( isSgStatement( stmt ), exprStmt );
		}
	}
	else {
		cerr
		<< "RuntimeInstrumentation :: Surrounding Block is not Block! : "
		<< name << " : " << scope->class_name() << "    stmt:" << stmt->unparseToString() << endl;
		ROSE_ASSERT(false);
	}
} else {
	cerr
	<< "RuntimeInstrumentation :: Surrounding Statement could not be found! "
	<< stmt->class_name() << endl;
	ROSE_ASSERT(false);
}

}

#endif
