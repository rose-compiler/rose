/*
 * StructUtil.hpp
 *
 *  Created on: Oct 19, 2012
 *      Author: Kamal Sharma
 */

#ifndef STRUCTUTIL_HPP_
#define STRUCTUTIL_HPP_

#include <string>
#include "rose.h"
#include "Common.hpp"
#include "FieldBlock.hpp"
#include "Field.hpp"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

class StructUtil {
public:
	StructUtil() {
	}
	;
	static void buildStructDeclarationAtStart(Field* field, int structNo,
			SgStatement* statement);
	static void buildStructDeclarationAfter(Field* field, int structNo,
			SgStatement* statement);
	static void buildStructDeclarationBefore(Field* field, int structNo,
			SgStatement* statement);

	static SgExpression* buildStructReference(Field* field, int structNo,
			SgScopeStatement* scope, SgExpression *index);

private:
	static SgType* buildFieldType(Field *field);
	static void buildStructDeclaration(Field* field, int structNo,
			SgStatement* statement, int position);
	static SgVariableDeclaration* buildStructMemberVariableDecl(Field* field,
			SgClassDefinition *structDefinition);

	static SgExpression* buildBlockedStructReference(Field* field, int structNo,
				SgScopeStatement* scope, SgExpression *index);
	static SgExpression* buildNonBlockedStructReference(Field* field, int structNo,
				SgScopeStatement* scope, SgExpression *index);
};

#endif /* STRUCTUTIL_HPP_ */
