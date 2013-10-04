/*
 Enable memory-alignment specific hardware optimizations such as
 doubleword load and packed SSE.  Also, allow user to request
 that a C restrict attribute be added to pointer variables.
 Author: Jeff Keasler
 Modifications by : Kamal Sharma
 */

#include "rose.h"
#include "Common.hpp"
#include "HaOptUtil.hpp"



#define ANCHOR Sg_File_Info::generateDefaultFileInfoForTransformationNode()

namespace HAOPTUtil {

using namespace SageInterface;
using namespace SageBuilder;
using namespace std;

typedef std::map<SgScopeStatement *, std::string> PragDict_t;
static PragDict_t prags;
static std::map<std::string, SgModifierType *> typeCache;
static std::map<SgScopeStatement *, SgStatement *> pragmaPointStatement;

/*******************************/
/*     Support Functions       */
/*******************************/

static int GetIntBound(SgNode *exp) {
	int retVal = -1; /* error */
	SgValueExp *e = isSgValueExp(exp);
	if ((e != 0) && SageInterface::isStrictIntegerType(e->get_type())
			&& SageInterface::isScalarType(e->get_type())) {
		unsigned long long val = SageInterface::getIntegerConstantValue(e);

		if (val < 0)
			val = 0;

		if (val >= sizeof(int) * 8)
			val = sizeof(int) * 8;

		retVal = (int) val;
	}
	return retVal;
}

static int GetLoopLowerBound(SgNode *loop) {
	bool foundBound = false;
	int retVal = -1;
	SgExpression *val;
	SgForStatement* forstmt = isSgForStatement(loop);

	Rose_STL_Container<SgNode* > testList =
	NodeQuery::querySubTree( *((forstmt->get_init_stmt()).begin()), V_SgAssignOp);

	if (testList.size() > 0) // assignment statement
			{
		if (testList.size() == 1) {
			SgAssignOp * assignop = isSgAssignOp((*testList.begin()));
			if (assignop != 0) {
				val = assignop->get_rhs_operand();
				foundBound = true;
			}
		}
	} else // variable declaration case
	{
		Rose_STL_Container<SgNode* > testList2 =
		NodeQuery::querySubTree( *((forstmt->get_init_stmt()).begin()), V_SgAssignInitializer );

		if (testList2.size()==1) {
			SgAssignInitializer* init = isSgAssignInitializer((*testList2.begin()));
			if(init != NULL) {
				val = init->get_operand_i();
				foundBound = true;
			}
		}
	}

	if (foundBound) {
		retVal = GetIntBound(val);
	}

	return retVal;
}

static int GetLoopUpperBound(SgNode *loop) {
	SgExpression *val;
	bool incr = false;
	bool foundBound = false;
	int retVal = -1;
	SgForStatement* forstmt = isSgForStatement(loop);

	SgBinaryOp *binop = isSgBinaryOp(
			isSgExprStatement(forstmt->get_test())->get_expression());

	if (binop != NULL) {
		val = binop->get_rhs_operand();
		if (isSgLessThanOp(binop)) {
			foundBound = true;
		} else if (isSgLessOrEqualOp(binop)) {
			foundBound = true;
			incr = true;
		}
	}

	if (foundBound) {
		retVal = GetIntBound(val);
		if (incr && (retVal < sizeof(int) * 8))
			++retVal;
	}

	return retVal;
}

/*************************************/
/*     Pointer Transformations       */
/*************************************/

/* scope: will be a function body (Basic block) or a variable declaration */
bool DoPointer(SgInitializedName *name, SgStatement *scope, CompilerTypes::TYPE compiler, bool applyRestrict) {
	/* get handle to full type with modifiers */
	SgType *origType = name->get_type();

	SgPointerType *ptr = isSgPointerType(
			origType->stripType(SgType::STRIP_MODIFIER_TYPE));
	if (ptr != NULL) /* recognize type "... * [const]" */
	{
		/* get handle to fundamental array type with modifiers */
		SgType *baseType = ptr->get_base_type();
		ROSE_ASSERT(baseType);

		/* Only modify single indirection for now... */
		if (!isSgPointerType(baseType->stripType(SgType::STRIP_MODIFIER_TYPE))
				&& name->get_name().str()[0] != 0) /* skip unnamed formal arguments */
				{
			enum HATYPE {
				HA_NOTYPE, HA_DOUBLE, HA_INT, HA_FLOAT, HA_LONG, HA_SHORT
			};
			HATYPE haType;

			/* get native atomic datatype (i.e. double, int, ...) */
			SgType *rawType = baseType->findBaseType();
			ROSE_ASSERT(rawType);

			/* In order of likelihood for science code... */
			if (isSgTypeDouble(rawType))
				haType = HA_DOUBLE;
			else if (isSgTypeInt(rawType))
				haType = HA_INT;
			else if (isSgTypeFloat(rawType))
				haType = HA_FLOAT;
			else if (isSgTypeLong(rawType))
				haType = HA_LONG;
			else if (isSgTypeShort(rawType))
				haType = HA_SHORT;
			else
				haType = HA_NOTYPE;

			/* Only apply transformations to recognized types */
			if (haType != HA_NOTYPE) {
				switch (compiler) {
				case CompilerTypes::GCC: {
#if 0
					/* right before pointer name */
					formalArgDirective = " __attribute__ ((aligned (16))) ";

					SageInterface::addTextForUnparser(
							name, " __attribute__ ((aligned (16))) ",
							AstUnparseAttribute::e_before );
#endif
				}
					break;

				case CompilerTypes::ICC:
				case CompilerTypes::XLC: {
					/* by default, don't tell compiler array is aligned */
					bool applyAlign = false;

					/* Used to check for *only* array with constant indices */
					unsigned int arrCover = 0;
					int bitCount = 0;

					/* use to check for *only* declared locally and passed to */
					/* lower level functions.  Needed for good inline expansion. */
					int safeVarUse = 0;
					int varRefCount = 0;

					SgScopeStatement *localScope = isSgScopeStatement(scope);
					SgScopeStatement *closestScope = localScope;
					if (localScope == NULL) {
						SgBasicBlock *funcBody = NULL;
						SgFunctionDefinition *funcDef;
						for (SgNode *scan = scope; scan != 0;
								scan = scan->get_parent()) {
							if (closestScope == NULL)
								closestScope = isSgScopeStatement(scan);

							if ((funcDef = isSgFunctionDefinition(scan)) != NULL) {
								funcBody = funcDef->get_body();
								ROSE_ASSERT(funcBody);
								break;
							}
						}
						localScope = isSgScopeStatement(funcBody);
						if (localScope == NULL) {
							break;
							/* this is a global declaration */
							/* set localScope = globalScope */
						}
					}

					/* Only apply align operator if array indexed access */
					/* occurs through a simple index variable, or if the first */
					/* several array locations are accessed to meet alignment */
					/* criteria, short a[0-7], int|float [0-3], long|double[0-1]. */

					/* scan all SgVarRefExp within this scope, looking for criteria */Rose_STL_Container<SgNode*> varRefExp =
					NodeQuery::querySubTree(closestScope, V_SgVarRefExp);

					for (Rose_STL_Container<SgNode*>::iterator r_itr = varRefExp.begin();
					r_itr != varRefExp.end(); ++r_itr)
					{
						SgVarRefExp* var = isSgVarRefExp(*r_itr);
						ROSE_ASSERT(var);

						SgInitializedName *varInitName =
						var->get_symbol()->get_declaration();
						ROSE_ASSERT(varInitName);

						if (name == varInitName) {
							++varRefCount;

							if (isSgExprListExp(var->get_parent())) {
								++safeVarUse;
							}
							else if (isSgCastExp(var->get_parent()) &&
							isSgExprListExp(var->get_parent()->get_parent())) {
								++safeVarUse;
							}
							else if (isSgAssignOp(var->get_parent()) &&
							isSgBinaryOp(var->get_parent())->
							get_lhs_operand_i() == var) {
								++safeVarUse;
							}
							else if (isSgAddressOfOp(var->get_parent())) {
								++safeVarUse;
								/* might want to warn on this one. */
							}

							/* Check for use as Simple Array */
							SgPntrArrRefExp *arrRef = isSgPntrArrRefExp(var->get_parent());
							if (arrRef == NULL)
							continue;

							SgBinaryOp *binOp = isSgBinaryOp(arrRef);
							ROSE_ASSERT(binOp);
							SgVarRefExp *leftOp =
							isSgVarRefExp(binOp->get_lhs_operand_i());
							if (leftOp != var)
							continue;

							/* check for case where all array indices are constant */
							//SgIntVal *iVal;
							//SgLongIntVal *lVal;
							SgExpression *subscript = binOp->get_rhs_operand_i();
							if (subscript != 0 &&
							SageInterface::isStrictIntegerType(subscript->get_type()) &&
							SageInterface::isScalarType(subscript->get_type()) ) {
								SgValueExp *subValue = isSgValueExp(subscript);
								if (subValue != 0) {
									if (SageInterface::getIntegerConstantValue(subValue) < sizeof(int)*8) {
										arrCover |= ( 1 <<
										(int) SageInterface::getIntegerConstantValue(subValue) );
									}
								}
								else /* check for loop access of array */{
									/* check loop with constant lower and upper bound */

									SgVarRefExp *subVar = isSgVarRefExp(subscript);
									if (subVar != 0) {
										SgInitializedName *subVarName =
										subVar->get_symbol()->get_declaration();

										SgNode *scanNode = isSgNode(subVar);
										while (1) { /* scope search */
											do {
												scanNode = scanNode->get_parent();
											}
											while(scanNode != closestScope && !isSgForStatement(scanNode) );

											if (scanNode == closestScope)
											break;

											/* scanNode is a for-statement at this point */

											SgInitializedName *loopVarName =
											SageInterface::getLoopIndexVariable(scanNode);
											if (loopVarName == subVarName) {
												int lb = GetLoopLowerBound(scanNode);
												int ub = GetLoopUpperBound(scanNode);
												if ( (lb >= 0) && (ub >= 0) && (lb < ub)) {
													for (int i = lb; i < ub; ++i) {
														arrCover |= ( 1 << i );
													}
												} /* loop ub and lb are constants */
												else {
													/* upper and lower bound not constant, so *assume* */
													/* a large swatch of the array is covered and so */
													/* the base pointer is aligned? */
													applyAlign = true;
													break;
												}
											} /* loop var matches subscript var */
											break;
										} /* loop scope search */
									} /* subscript is a simple variable*/
								} /* loop check */
							} /* subscripted array having integral subscript type */
						}
					}

					for (int ii = 0; ii < (sizeof(int) * 8); ++ii) {
						if ((arrCover & (1u << ii)) == 0)
							break;
						else
							++bitCount;
					}

					/* check for all array indices as constants */
					if (bitCount != 0) {
						switch (haType) {
						/* check for 16 byte alignment */
						case HA_LONG:
						case HA_DOUBLE:
							if ((bitCount % 2) == 0)
								applyAlign = true;
							break;

						case HA_INT:
						case HA_FLOAT:
							if ((bitCount % 4) == 0)
								applyAlign = true;
							break;

						case HA_SHORT:
							if ((bitCount % 8) == 0)
								applyAlign = true;
							break;

						default:
							;
						}
					}

					if (varRefCount == safeVarUse) {
						/* if (isSgVariableDeclaration(scope)) */
						applyAlign = true;
					}

					//if (applyAlign == false)
					//	break;

					SgExprListExp *alignArgs = new SgExprListExp(ANCHOR);
					SgExprStatement *alignfuncStmt;
					if (compiler == CompilerTypes::ICC) {
						alignArgs->append_expression(
								buildVarRefExp(name, localScope));
						alignArgs->append_expression(
								isSgExpression(new SgIntVal(ANCHOR, 16)));

						alignfuncStmt = buildFunctionCallStmt(
								SgName("__assume_aligned"), new SgTypeVoid(),
								alignArgs, localScope);
					} else /* XLC */
					{
						trace << " XLC disjoint :" << name->get_name().str() << endl;
						/* build required pragma for this function (accumulation) */
						if (prags.find(localScope) == prags.end()) {
							prags[localScope] = std::string("disjoint( *")
									+ name->get_name().str();
						} else {
							prags[localScope] += std::string(", *")
									+ name->get_name().str();
						}

						// Disjoint pragma should be inserted after the last
						// declaration
						if(isSgVariableDeclaration(scope))
						{
							pragmaPointStatement[localScope] = scope;
						}

						alignArgs->append_expression(
								isSgExpression(new SgIntVal(ANCHOR, 16)));
						alignArgs->append_expression(
								buildVarRefExp(name, localScope));

						alignfuncStmt = buildFunctionCallStmt(
								SgName("__alignx"), new SgTypeVoid(), alignArgs,
								localScope);
						/* save list of names for this function then build pragma disjoint */
					}

					if (isSgBasicBlock(scope)) {
						/* handle formal function arguments */
						isSgBasicBlock(scope)->prepend_statement(alignfuncStmt);
					} else {
						/* handle locally declared pointers */
						((SgStatement *) scope->get_parent())->insert_statement(
								scope, isSgStatement(alignfuncStmt), false);
						alignfuncStmt->set_parent(localScope);
					}
				}
					break;

				case CompilerTypes::PGI: {
#if 0
					/* not available ??? (investigate) */
					formalArgDirective = " ";
#endif
				}
					break;

				case CompilerTypes::UNKNOWN:
					/* User may have requested restrict keyword only */
					break;

				default: {
					printf("formal arg: compiler not supported\n");
					exit(-1);
				}
				}

				/* user has requested that restrict be added to qualified pointers */
				if (applyRestrict) {
					int ii;
					char origTypeName[128];
					char stripTypeName[128];

					strcpy(origTypeName, origType->unparseToString().c_str());
					strcpy(stripTypeName,
							origType->stripType(SgType::STRIP_MODIFIER_TYPE)->unparseToString().c_str());

					/* isolate pointer modifiers */
					for (ii = 0; origTypeName[ii] == stripTypeName[ii]; ++ii) {
						/* exit if strings are identical */
						if (stripTypeName[ii] == 0)
							break;
					}

					if (!strstr(&origTypeName[ii], "restrict")) {
						/* we need to add a restrict modifier */
						SgModifierType *restrictMod;

						if (typeCache.find(origTypeName) == typeCache.end()) {
							/* build the type */
							bool ptrIsConst = (
									strstr(&origTypeName[ii], "const") ?
											true : false);

							SgPointerType * tmpPtr = new SgPointerType(
									baseType);
							ROSE_ASSERT(tmpPtr);

							restrictMod = new SgModifierType(tmpPtr);
							restrictMod->get_typeModifier().setRestrict();
							if (ptrIsConst)
								restrictMod->get_typeModifier().get_constVolatileModifier().setConst();
							typeCache[origTypeName] = restrictMod;
						} else
							restrictMod = typeCache[origTypeName];

						name->set_type(restrictMod);
					}
				}
			}
#ifdef HA_DEBUG
			else
			{
				printf("Won't transform pointer to %s\n",
						baseType->unparseToString().c_str());
			}
#endif
		}
	}
	return (ptr ? true : false);
}

// Kamal : Added this for TALC Struct
void DoStructDecl(SgInitializedName *name, bool applyRestrict)
{

	// Check if user has requested restrict
	if (!applyRestrict)
		return;

	SgPointerType *basePntrType = isSgPointerType(name->get_type());
	ROSE_ASSERT(basePntrType);
	//SgPointerType * tmpPtrType = new SgPointerType(basePntrType);
	SgModifierType *restrictMod = new SgModifierType(basePntrType) ;
	restrictMod->get_typeModifier().setRestrict();
	name->set_type(restrictMod);
}


/*************************************/
/*      Array Transformations        */
/*************************************/

/* scope: will be a function body (Basic block) or a variable declaration */
bool DoArray(SgInitializedName *name, SgStatement *scope, CompilerTypes::TYPE compiler) {
	SgArrayType *arr = isSgArrayType(name->get_type());
	if (arr != NULL) {
		switch (compiler) {
		case CompilerTypes::GCC: {
#if 0
			/* right before semicolon, after declaration */
			varDeclDirective = " __attribute__ ((aligned (16))) ";
#endif
			SageInterface::addTextForUnparser(scope,
					" __attribute__ ((aligned (16))) ",
					AstUnparseAttribute::e_after);
		}
			break;

		case CompilerTypes::ICC: {
#if 0
			/* before variable declaration */
			varDeclDirective = " __declspec(align(16)) ";
#endif
			SageInterface::addTextForUnparser(scope, " __declspec(align(16)) ",
					AstUnparseAttribute::e_after);
		}
			break;

		case CompilerTypes::XLC: {
			/* double __align(16) x[16] */
			SageInterface::addTextForUnparser(scope,
					" __attribute__ ((aligned(16))) ",
					AstUnparseAttribute::e_after);
		}
			break;

		case CompilerTypes::PGI: {
#if 0
			/* after decl, before semicolon */
			varDeclDirective = " __attribute__ ((aligned (16))) ";
#endif
		}
			break;

		case CompilerTypes::UNKNOWN:
			/* User may have requested restrict keyword only */
			break;

		default: {
			printf("local var: compiler not supported\n");
			exit(-1);
		}
		}
	}
	return (arr ? true : false);
}

void DoPragma(SgProject *project, CompilerTypes::TYPE compilerType) {
	/***************************************************************/
	/* xlC requires a "disjoint" pragma and extern __alignx() decl */
	/***************************************************************/

	if ((compilerType == CompilerTypes::XLC) && (!prags.empty())) {
		/* Add required Pragmas */
		for (PragDict_t::const_iterator itr = prags.begin(); itr != prags.end();
				++itr) {
			std::string pragText = (*itr).second + ")";
			SgPragmaDeclaration *prag = buildPragmaDeclaration(pragText,
					(*itr).first);
			SgScopeStatement *scope = (*itr).first;
			if (pragmaPointStatement.find(scope) == pragmaPointStatement.end()) {
				isSgBasicBlock(scope)->prepend_statement(prag);
			}
			else
			{
				insertStatementAfter(pragmaPointStatement[scope], prag);
			}
		}

		/* add declaration for intrinsic __alignx function */
		Rose_STL_Container<SgNode*> locatedNodes =
		NodeQuery::querySubTree(project, V_SgLocatedNode);

		SgLocatedNode *firstNode = isSgLocatedNode(*locatedNodes.begin());
		ROSE_ASSERT(firstNode);

		SageInterface::attachArbitraryText(firstNode,
				std::string(
						"\nextern\n#ifdef __cplusplus\n\"builtin\"\n#endif\nvoid __alignx(int n, const void *addr) ;\n\n"));
	}
}

}


