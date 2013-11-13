/*
 * StructUtil.cpp
 *
 *  Created on: Oct 19, 2012
 *      Author: Kamal Sharma
 */

#include "StructUtil.hpp"
#include "TALCAttribute.cpp"

SgType* StructUtil::buildFieldType(Field *field) {
	SgType *type;
	switch (field->getType()) {
	case FieldTypes::INT:
		type = buildIntType();
		break;
	case FieldTypes::FLOAT:
		type = buildFloatType();
		break;
	case FieldTypes::DOUBLE:
		type = buildDoubleType();
		break;
	default:
		traceerror<< " Unknown Field Type " << endl;
		exit(1);
	}

	return type;
}

SgExpression* StructUtil::buildBlockedStructReference(Field* field,
		int structNo, SgScopeStatement* scope, SgExpression *index) {

	int blocksize = field->getFieldBlock()->getBlockSize();
	string structVarName = "s" + boost::lexical_cast < string > (structNo);

	// s0
	SgVarRefExp *structVarRef = buildVarRefExp(structVarName);

	// s0[i/4]
	SgPntrArrRefExp *structArrayRef = buildPntrArrRefExp(structVarRef,
			buildDivideOp(copyExpression(index), buildIntVal(blocksize)));

	// x
	string name = "_" + field->getName();
	SgVarRefExp *memberVarRef = buildVarRefExp(name);

	// .x[i%4]
	SgPntrArrRefExp *memberArrayRef = buildPntrArrRefExp(memberVarRef,
			buildModOp(copyExpression(index), buildIntVal(blocksize)));

	// s0[i/4].x[i%4]
	SgDotExp *structReference = buildDotExp(structArrayRef, memberArrayRef);
	ROSE_ASSERT(structReference);
	trace << " Struct Ref: " << structReference->unparseToString().c_str()
			<< endl;

	fixVariableReferences(structReference);
	trace << " After fix " << endl;
	return structReference;
}

/**
 * Internal Method to build struct reference
 * e.g. s0[i].x
 * @param field
 * @param structNo
 * @param scope
 * @param index
 * @return
 */
SgExpression* StructUtil::buildNonBlockedStructReference(Field* field,
		int structNo, SgScopeStatement* scope, SgExpression *index) {

	string structVarName = "s" + boost::lexical_cast < string > (structNo);

	// s0
	SgVarRefExp *structVarRef = buildVarRefExp(structVarName, scope);

	// s0[i]
	SgPntrArrRefExp *structArrayRef = buildPntrArrRefExp(structVarRef,
			copyExpression(index));

	// .x
	string name = "_" + field->getName();
	SgDotExp *structReference = buildDotExp(structArrayRef,
			buildVarRefExp(name, scope));
	ROSE_ASSERT(structReference);
	trace << " Struct Ref: " << structReference->unparseToString().c_str()
			<< endl;

	fixVariableReferences(structReference);
	return structReference;
}

SgExpression* StructUtil::buildStructReference(Field* field, int structNo,
		SgScopeStatement* scope, SgExpression *index) {

	// Check if field is blocked
	if (field->getFieldBlock()->isBlocked()) {
		return buildBlockedStructReference(field, structNo, scope, index);
	} else {
		return buildNonBlockedStructReference(field, structNo, scope, index);
	}
}

/**
 * Internal method to build a struct member
 * @param field
 * @param structDefinition
 * @return
 */
SgVariableDeclaration* StructUtil::buildStructMemberVariableDecl(Field* field,
		SgClassDefinition *structDefinition) {
	SgVariableDeclaration* structVar;
	string name = "_" + field->getName();

	// Check if field is blocked i.e. int x[4] or simple int x
	if (field->getFieldBlock()->isBlocked()) {
		int blocksize = field->getFieldBlock()->getBlockSize();
		structVar = buildVariableDeclaration(name,
				buildArrayType(buildFieldType(field), buildIntVal(blocksize)),
				NULL, structDefinition);
	} else {
		structVar = buildVariableDeclaration(name,
				buildFieldType(field), NULL, structDefinition);
	}

	return structVar;
}

/**
 * Internal Method
 * @param field
 * @param structNo
 * @param statement
 * @param position 1- At Start, 2- After statement, 3- Before statement
 */
void StructUtil::buildStructDeclaration(Field* field, int structNo,
		SgStatement* statement, int position) {

	SgScopeStatement* scope = getScope(statement);
	ROSE_ASSERT(scope);

	// Build the struct
	string structName = "fb" + boost::lexical_cast < string > (structNo);
	SgClassDeclaration *structDeclaration = SageBuilder::buildStructDeclaration(
			structName, scope);
	SgClassDefinition *structDefinition = structDeclaration->get_definition();
	foreach(Field * f, field->getFieldBlock()->getFieldList())
	{
		SgVariableDeclaration* structVar = buildStructMemberVariableDecl(f,
				structDefinition);
		ROSE_ASSERT(structVar);
		structDefinition->append_member(structVar);
	}

	switch (position) {
	case 1:
		prependStatement(structDeclaration, scope);
		break;
	case 2:
		insertStatementAfter(statement, structDeclaration);
		break;
	case 3:
		insertStatementBefore(statement, structDeclaration);
		break;
	default:
		traceerror<< " Illegal Option " << endl; exit(1);
	}

	trace << " Struct Decl: " << structDeclaration->unparseToString() << endl;

	// Build struct variable struct fb0 *s0;
	string structVarName = "s" + boost::lexical_cast < string > (structNo);
	SgVariableDeclaration *structVariable = buildVariableDeclaration(
			structVarName, buildPointerType(structDeclaration->get_type()),
			NULL, scope);
	ROSE_ASSERT(structVariable);
	// Add TALC Attribute which will be useful in the pass
	// to know that this struct was generated by TALC
	TALCAttribute *attribute = new TALCAttribute();
	attribute->setStructDecl(true);
	structVariable->addNewAttribute("TALCAttribute",attribute);
	insertStatementAfter(structDeclaration, structVariable);
	trace << " Struct Var: " << structVariable->unparseToString() << endl;

	// Build s1 = (reinterpret_cast < struct g1 * >  (xdd));
	SgVarRefExp *castName = buildVarRefExp(field->getName(), scope);
	ROSE_ASSERT(castName);

	SgExpression *castExp = castName;
	// Calculate subtraction offset (xdd-1) or (xdd-(pos*block)) if necessary
	if (field->getPosition() != 0) {
		int position = field->getPosition();
		if (field->getFieldBlock()->isBlocked())
			castExp = buildSubtractOp(castName,
					buildMultiplyOp(
							buildIntVal(field->getFieldBlock()->getBlockSize()),
							buildIntVal(position)));
		else
			castExp = buildSubtractOp(castName, buildIntVal(position));
	}

	SgCastExp *ptrCast = buildCastExp(castExp,
			buildPointerType(structDeclaration->get_type()),
			SgCastExp::e_reinterpret_cast);

	SgStatement *assignStatement = buildAssignStatement(
			buildVarRefExp(structVarName, scope), ptrCast);
	ROSE_ASSERT(assignStatement);
	insertStatementAfter(structVariable, assignStatement);
	trace << " Struct Cast: " << assignStatement->unparseToString() << endl;

}

/**
 * Build Struct Declaration At the start of current scope
 * @param field
 * @param structNo
 * @param statement
 */
void StructUtil::buildStructDeclarationAtStart(Field* field, int structNo,
		SgStatement* statement) {
	buildStructDeclaration(field, structNo, statement, 1);
}

/**
 * Build Struct Declaration after current statement
 * @param field
 * @param structNo
 * @param statement
 */
void StructUtil::buildStructDeclarationAfter(Field* field, int structNo,
		SgStatement* statement) {
	buildStructDeclaration(field, structNo, statement, 2);
}

/**
 * Build Struct Declaration before current statement
 * @param field
 * @param structNo
 * @param statement
 */
void StructUtil::buildStructDeclarationBefore(Field* field, int structNo,
		SgStatement* statement) {
	buildStructDeclaration(field, structNo, statement, 3);
}

