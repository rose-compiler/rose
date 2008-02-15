#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ArrayPaddingTransform.h"
#include <strings.h>
#include "rose_debug.h"

#include "array_class_interface.h"
#include "padding_tools.h"


// CW: !!! ATTENTION !!!
// CW: this info must be created and initialized properly
// somewhere else. We use it here to parametrize the
// padding transformation
extern cacheInfo roseTargetCacheInfo;


ArrayPaddingTransform::ArrayPaddingTransform()
:globalScope(NULL),currentScope(NULL), noOfArraysPadded(0),
totalNoOfArrays(0),noOfConsideredArrays(0)
{
}

ArrayPaddingTransform::~ArrayPaddingTransform()
{
	// CW: the global global scope is created dynamically
	// so we have to delete it here
	if(globalScope!=NULL) delete globalScope;
}

void ArrayPaddingTransform::printStatistics(ostream &os)
{
	os << "========================================" << endl;
	os << "====== Array Padding Statistics : ======" << endl;
	os << "========================================" << endl;

	os << "= Total number of A++ arrays:    " << totalNoOfArrays  << endl;
	os << "= Arrays considered for padding: " << noOfConsideredArrays  << endl;
	os << "= Total number of arrays padded: " << noOfArraysPadded  << endl;
		
	os << "========================================" << endl;		
}

// CW: traverse tree, collect information needed for transformation, store
// the data in aptScope objects, and perform array padding transformation
ROSE_StatementBlock* ArrayPaddingTransform::pass(ROSE_StatementBlock* inputTree)
{
	ROSE_ASSERT(inputTree!=NULL);
	
	// CW: collect data needed for transformation and store data in class.
	traverse(inputTree,aptGlobalScopeTag);
	
	// CW: !!! ATTENTION !!!
	// CW: add code here...
	// CW: add creation of reference information and add reference
	// info to arrayInfo objects for the arrays which are referenced
	
	// CW: perform transformation with stored data
	transform();
		
	// CW: return transformated ROSE_Node tree for other passes!
	// since we only change the sage tree and we do not touch
	// the root node we can safely return the original input tree
	return inputTree;
}

// CW: this function initializes the padding condition list
// and and applys a recursive transform function to the global
// scope
void ArrayPaddingTransform::transform()
{
	// CW: choose the padding conditions we want to apply
	// we might want to controll this with an program argument
	List<intra_padding_condition> ipcList;
	ipcList.addElement(condition_linpad1);
	ipcList.addElement(condition_linpad2);
	ipcList.addElement(condition_intrapadlite);
	ipcList.addElement(condition_intrapad);

	ROSE_ASSERT(globalScope!=NULL);

	// CW: start recursive transformation
	transform(*globalScope,roseTargetCacheInfo,ipcList);
}

// CW: examinates every array declaration within the scope
// and applies padding library function on it. If an array
// information for a variable is changed it calls a function
// which applies the code transformation. The function
// calls itself for each subscope
void ArrayPaddingTransform::transform(const aptScope& scope,const cacheInfo& cInfo,
const List<intra_padding_condition>& ipcList)
{
	// CW: we check every array declaration we have found and where
	// we have figured out enougth information to apply padding
	for(int i=0;i<scope.getNoOfArrayDeclInfos();i++)
	{
		arrayDeclInfo& arrayDecl = scope.getArrayDeclInfo(i);
		
		// CW: we take our old arrayInfo, apply padding, and
		// compare our new arrayInfo with the old one. If it
		// has changed we have do do a code transformation
		arrayInfo oldInfo=arrayDecl.getArrayInfo();
		arrayInfo newInfo=getNeededIntraPadding(cInfo,oldInfo,ipcList);
		
		if(newInfo!=oldInfo)
		{
			// CW: we want to do a code transformation since we have
			// found a better array size
			applyTransformation(scope.getArrayDeclInfo(i),newInfo);
		};
	};

	// CW: apply transformation to all subscopes
	for(int j=0;j<scope.getNoOfSubScopes();j++)
	{
		transform(scope.getSubScope(j),cInfo,ipcList);
	};	
}

// CW: apply code transformation to variable declaration sage subtree
void ArrayPaddingTransform::applyTransformation(const arrayDeclInfo& declInfo, const arrayInfo& aInfo)
{
	SgVariableDeclaration* varDeclStmt= declInfo.getArrayDeclaration();
	SgInitializedNameList& varList= varDeclStmt->get_variables();

	SgInitializedNameList::iterator i=varList.begin();

	Boolean transformed=FALSE;
	
	for(i; i != varList.end(); i++)
	{
		// CW: only apply code transformation to variable
		// we are looking for
		if((*i).get_name()==declInfo.getArrayName()){
		
			ROSE_ASSERT(transformed==FALSE);
		
			// CW: the variable is initialized with a constructor
			// we want to change the arguments for that constructor
			// so that an array with a new size is created
			SgInitializedName *constrName=(*i).get_named_item();
			ROSE_ASSERT(constrName!=NULL);
			SgConstructorInitializer *varConstructor = isSgConstructorInitializer(constrName->get_initializer());
			ROSE_ASSERT(varConstructor!=NULL);
			SgExprListExp* constructorArgs=varConstructor->get_args();
			ROSE_ASSERT(constructorArgs!=NULL);
		
			// CW: we got the constructor argument expression list...
			SgExpressionPtrList& argumentPtrList = constructorArgs->get_expressions();
			ROSE_ASSERT(argumentPtrList.size()!=0);
			ROSE_ASSERT(argumentPtrList.size()==aInfo.getNumberOfDimensions());

			// CW: remove old expressions and replace them through new expressions
			// which represent the array sizes
			argumentPtrList.erase(argumentPtrList.begin(),argumentPtrList.end());

			for(int i=1; i<=aInfo.getNumberOfDimensions();i++)
			{
				const int colSize = aInfo.getColSize(i);
				// CW: !!! ATTENTION !!!
				// CW: we might want to copy the file info from the old
				// expressions
				SgIntVal *colSizeExpr = new SgIntVal(NULL_FILE,colSize);
				colSizeExpr->set_parent(constructorArgs);
				argumentPtrList.insert(argumentPtrList.end(),colSizeExpr);
			};

			// CW: we have performed a transformation
			transformed=TRUE;

			// CW: for statistics
			noOfArraysPadded++;
		}
	};
	
	// CW: make sure that we have applied a transformation
	ROSE_ASSERT(transformed==TRUE);
}

// CW: call apropriate traverse function for statement type
void ArrayPaddingTransform::traverse(ROSE_Statement *inputStmt)
{
	ROSE_ASSERT(inputStmt!=NULL);
	
	// CW: get type of subtree, do typecast, and call apropriate
	// traverse  function. If the design is improved then this could be
	// done by the virtual function call mechanism.
	switch (inputStmt->getVariant())
	{
        case ROSE_DoWhileStatementTag:
			traverse((ROSE_DoWhileStatement *)inputStmt);
			break;
			
        case ROSE_ElseWhereStatementTag:
			traverse((ROSE_ElseWhereStatement *)inputStmt);
			break;
			
        case ROSE_ExpressionStatementTag:
			traverse((ROSE_ExpressionStatement *)inputStmt);
			break;
			
        case ROSE_ForStatementTag:
			traverse((ROSE_ForStatement *)inputStmt);
			break;
			
        case ROSE_IfStatementTag:
			traverse((ROSE_IfStatement *)inputStmt);
			break;
			
		case ROSE_ReturnStatementTag:
			traverse((ROSE_ReturnStatement *)inputStmt);
			break;
			
        case ROSE_WhereStatementTag:
			traverse((ROSE_WhereStatement *)inputStmt);
			break;
			
        case ROSE_WhileStatementTag:
			traverse((ROSE_WhileStatement *)inputStmt);
			break;
			
		case ROSE_C_StatementTag:
			traverse((ROSE_C_Statement *)inputStmt);
			break;
			
        case ROSE_StatementBlockTag:
			traverse((ROSE_StatementBlock *)inputStmt);
			break;
			
        case ROSE_ArrayVariableDeclarationTag:
			traverse((ROSE_ArrayVariableDeclaration *)inputStmt);
			break;
			
        case ROSE_C_VariableDeclarationTag:
			traverse((ROSE_C_VariableDeclaration *)inputStmt);
			break;
			
        case ROSE_IndexingVariableDeclarationTag:
			traverse((ROSE_IndexingVariableDeclaration *)inputStmt);
			break;
			

		default:
			// CW: !!! ATTENTION !!!
			// CW: unknown tag, so we cannot do a transformation
			// CW: if you add a new ROSE_Node type, then add type
			// cast
			ROSE_ABORT(); // force an abort here for now
			break;
	}
}

// CW: not implemented yet!!!
// CW: we have found an array reference.
void ArrayPaddingTransform::traverse(ROSE_ExpressionStatement *inputStmt)
{
	// CW: not implemented yet	
	const SgNode *node=inputStmt->getSageSubTree();
	const SgExprStatement *exprStmt=isSgExprStatement((SgNode *)node);
	ROSE_ASSERT(exprStmt!=NULL);
	const SgExpression *expr = exprStmt->get_the_expr();
	ROSE_ASSERT(expr!=NULL);
#if 0
	showSgExpression(cout,(SgExpression *)expr,"ArrayPaddingTransform::traverse(ROSE_ExpressionStatement *inputStmt)");
#endif
	// CW: !!! ATTENTION !!!
	// CW: identify A++ array which is referenced
	// lookup array in scope. if we cannot find it we are finished
	// else place reference information in scope
	// later or mabey even here we would like to check
	// whether the references are uniformly generated.
	// if they are we want the reference constants
	// to be added to that arrayInfo object.
	// We also might want to change the scope type
	// to aptLoopScopeTag if we have found an Index or
	// Range object as refernce parameter since the semantics
	// are then like a loop, even if the surounding scope is
	// not a loop.
	// double array c; c(0,0)=1.0 is not detected as a ROSE_ExpressionStatement
	// so I guess that we have to fix this if it is a bug or also handle
	// the ROSE_C_Statement part more carefully.
}

// CW: ---------------------------------------------------------------
// CW: We found an ArrayVariableDeclaration. Try to find out as much as
// CW: possible for padding algorithm and store array data in scopes if
// CW: we found enougth data to perform arrray padding
// CW: ---------------------------------------------------------------
void ArrayPaddingTransform::traverse(ROSE_ArrayVariableDeclaration* inputDeclaration)
{
	// CW: statistics
	totalNoOfArrays++;

	// CW: compiler complains that this is an anachronismn
	// well since i want transformate the SgNode it might be a good
	// idea to change the return value type of getSageSubTree...
	SgNode *node=inputDeclaration->getSageSubTree();
	
	SgVariableDeclaration* varDeclStmt= isSgVariableDeclaration(node);
	ROSE_ASSERT(ArrayClassSageInterface::isROSEArrayVariableDeclaration((SgVariableDeclaration *)varDeclStmt));
	
	SgInitializedNameList& varList= varDeclStmt->get_variables();
	
	// CW: for every variable declared in that declaration statement
	SgInitializedNameList::iterator i=varList.begin();
	for(i; i != varList.end(); i++)
	{
		ROSE_ASSERT((*i).get_type()->variant()==T_CLASS);
		
		// CW: find out element size (if result == -1 then we were not
		// able to find it out)
		const int elemSize = getArrayElementSize(isSgClassType((*i).get_type()));

		// CW: get array sizes from constructor arguments (if the result
		// == NULL then we were not able to find the array sizes at compile
		// time)
		List<const int> arraySizes;
		getArrayDimensionSizes((*i).get_named_item(),arraySizes);
		// CW: get the array sizes from constructor arguments.
		List<const int> arrayBases;
		getArrayDimensionLowerBounds((*i).get_named_item(),arrayBases);
		
		// CW: get base address of array. So far not implemented
		const int baseAddress = getArrayBaseAddress();
		
		if((arraySizes.getLength()>0) && (arrayBases.getLength()>0)
			&& (baseAddress>=0) && (elemSize>0))
		{
			// CW: if we have collected everything we need, we can add 
			// a new array info to
			arrayInfo newInfo(elemSize,baseAddress,arraySizes,arrayBases);
			arrayDeclInfo newDeclInfo(newInfo,varDeclStmt,(*i).get_name());
			currentScope->addDeclaration(newDeclInfo);
			noOfConsideredArrays++;	
		};
	};
}


// CW: ---------------------------------------------------------------
// CW: traverse every statement within the block
// CW: ---------------------------------------------------------------
void ArrayPaddingTransform::traverse(ROSE_StatementBlock* inputBlock, const aptScopeType& subScopeType)
{
	ROSE_ASSERT(inputBlock != NULL);

	// CW: we will change scope! Save this scope, so that
	// we can use it again if we are going up in the program tree
	aptScope *parentScope=currentScope;

	if(parentScope!=NULL){
		// CW: create new sub scope. We will need it to collect data.
		// and add new scope to parent scope
		currentScope = parentScope->addSubScope(aptScope(subScopeType,parentScope));

	}
	else{
		
		ROSE_ASSERT(subScopeType==aptGlobalScopeTag);
		globalScope = new  aptScope(subScopeType,NULL);
		currentScope = globalScope;
	}

	// CW: traverse sub tree
		
	for (int i=0; i < inputBlock->getNumberOfStatements(); i++)
	{
		// CW: calls traverse(ROSE_Statement *)
		traverse((*inputBlock)[i]);
	};
	
	
	// CW: we are going up. Hence, restore current scope.
	currentScope=parentScope;

}

// CW: ---------------------------------------------------------------
// CW: go down in subtree...
// CW: ---------------------------------------------------------------

// CW: traverse if and else tree and add them to current scope
// CW: the are added on the same level. So, that references in these
// CW: scopes are equally handled.
void ArrayPaddingTransform::traverse(ROSE_IfStatement *inputStmt)
{
	traverse(inputStmt->getTrueRoseStatementBlock(),aptNonLoopScopeTag);

	ROSE_StatementBlock* falseBlock = inputStmt->getFalseRoseStatementBlock();
	if(falseBlock!=NULL)
	{
		traverse(falseBlock,aptNonLoopScopeTag);
	};
}

// CW: we have found a loop!!!
void ArrayPaddingTransform::traverse(ROSE_ForStatement *inputStmt)
{
	traverse(inputStmt->getRoseForStatementBlock(),aptLoopScopeTag);
}

// CW: I'm not sure what a elseWhere stmt is, so this might be wrong
void ArrayPaddingTransform::traverse(ROSE_ElseWhereStatement *inputStmt)
{
	traverse(inputStmt->getRoseElseWhereStatementBlock(),aptNonLoopScopeTag);
}

// CW: I'm not sure what a where stmt is, so this might be wrong
void ArrayPaddingTransform::traverse(ROSE_WhereStatement *inputStmt)
{
	traverse(inputStmt->getRoseWhereStatementBlock(),aptNonLoopScopeTag);
}



// CW: ---------------------------------------------------------------
// CW: nothing to collect for these types...
// CW: ---------------------------------------------------------------
void ArrayPaddingTransform::traverse(ROSE_ReturnStatement *inputStmt)
{}

// CW: !!! ATTENTION !!!
// CW: this function collects also the following case
// doubleArray c(11,11)
// c(1,1) = 2;
// unfortunately we want to collect this as an array reference
// Dan told me that he wants to add a new ROSE_Node tree type
// which will represent that case. I do not implement the
// detection here and Dan will add the easier handling later
void ArrayPaddingTransform::traverse(ROSE_C_Statement *inputStmt)
{}

void ArrayPaddingTransform::traverse(ROSE_C_VariableDeclaration* inputDeclaration)
{}

void ArrayPaddingTransform::traverse(ROSE_IndexingVariableDeclaration* inputDeclaration)
{}


// CW: ---------------------------------------------------------------
// CW: these types doesn't seem to be implemented. So, I'm not able
// CW: to do something here
// CW: ---------------------------------------------------------------
void ArrayPaddingTransform::traverse(ROSE_DoWhileStatement *inputStmt)
{
	// CW: !!! ATTENTION !!!
	// CW: remove the abort if it becomes a problem
	// It will still work but not recognice the references
	// and array variable declarations within the block
	ROSE_ABORT();
}

void ArrayPaddingTransform::traverse(ROSE_WhileStatement *inputStmt)
{
	// CW: !!! ATTENTION !!!
	// CW: remove the abort if it becomes a problem
	// It will still work but not recognice the references
	// and array variable declarations within the block
	ROSE_ABORT();
}


// CW: returns size of one array element
int ArrayPaddingTransform::getArrayElementSize(SgType * type)
{
	ROSE_ASSERT(ArrayClassSageInterface::isROSEArrayType(type));
	
	// CW: !!! ATTENTION !!!
	// CW: takes size of the type on the machine on which ROSE
	// is running....
	// CW: you must add new "else if" if we have new array types
	if(ArrayClassSageInterface::isROSEdoubleArrayType(type)) return sizeof(double);
	else if(ArrayClassSageInterface::isROSEfloatArrayType(type)) return sizeof(float);
	else if(ArrayClassSageInterface::isROSEintArrayType(type)) return sizeof(int);
	else return -1;
}

// CW: collects array sizes from constructor arguments
// if it is not possible to identify an array size at compile-time
// for any dimension it returns NULL;
void ArrayPaddingTransform::getArrayDimensionSizes(SgInitializedName *constrName, List<const int>& sizes)
{
	ROSE_ASSERT(constrName!=NULL);
	ROSE_ASSERT(sizes.getLength()==0);
	
	SgConstructorInitializer *varConstructor = isSgConstructorInitializer(constrName->get_initializer());

	// CW: if we have a constructor at all the we might be able
	// to find soemthing out...
	if(varConstructor!=NULL){
		SgExprListExp* constructorArgs=varConstructor->get_args();
		ROSE_ASSERT(constructorArgs!=NULL);

		const SgExpressionPtrList& argumentPtrList = constructorArgs->get_expressions();

		// CW: when no arguments are specified, then I guess we are not
		// able to identify the array size
		if(argumentPtrList.size()>0){


			// CW: !!! ATTENTION !!!
			// CW: add check for copy constructor...
			// checkForCopyConstructor() and make recursive call

			int arraySizes[ROSE_MAX_ARRAY_DIMENSION];
			int currentDim=0;
			SgExpressionPtrList::const_iterator i = argumentPtrList.begin();

			bool sizesAreOk=TRUE;

			while(sizesAreOk && i!=argumentPtrList.end())
			{
#if USE_SAGE3
				arraySizes[currentDim] = getIntegerValueOfExpr( (*i), sizesAreOk );
#else
				arraySizes[currentDim] = getIntegerValueOfExpr((*i).irep(),sizesAreOk);
#endif
				// CW: if we are not able to get the size for one dimension
				// then we have lost...
				if(sizesAreOk){
					currentDim++;
					i++; // CW: goto next argument
				}
			}

			// CW: build a list with the array sizes
			if(sizesAreOk){
				for(int j=0;j<currentDim;j++)
				{
					sizes.addElement(*new const int(arraySizes[j]));
				}
			}
		}
	}
}


// CW: get integer value from expression an return it
// if it is not possible to get the integer value the
// parameter valueIsOk is FALSE otherwise TRUE
int ArrayPaddingTransform::getIntegerValueOfExpr(SgExpression *expr, Boolean& valueIsOk)
{
	ROSE_ASSERT(expr!=NULL);
	
	// CW: !!! ATTENTION !!!
	// add more cases to improve the number of cases
	// which are regognized. Range R(0,1024); A(R,R) should be recognized
	// for example

	switch(expr->variant())
	{
		case INT_VAL:
		{
			SgIntVal *valueExpr=isSgIntVal(expr);
			int value = valueExpr->get_value();
			ROSE_ASSERT(value>0);
			valueIsOk=TRUE;
			return value;
			break;
		}
		case SHORT_VAL:
		{
			SgShortVal *valueExpr=isSgShortVal(expr);
			short value = valueExpr->get_value();
			ROSE_ASSERT(value>0);
			valueIsOk=TRUE;
			return value;
			break;
		}
		case UNSIGNED_SHORT_VAL:
		{
			SgUnsignedShortVal *valueExpr=isSgUnsignedShortVal(expr);
			unsigned short value = valueExpr->get_value();
			ROSE_ASSERT(value>0);
			valueIsOk=TRUE;
			return value;
			break;
		}
		case UNSIGNED_INT_VAL:
		{
			SgUnsignedIntVal *valueExpr=isSgUnsignedIntVal(expr);
			unsigned int value = valueExpr->get_value();
			ROSE_ASSERT(value>0);
			valueIsOk=TRUE;
			return value;
			break;
		}
		default:
			valueIsOk=FALSE;
			return -1;
			break;
	}
}

unsigned long ArrayPaddingTransform::getArrayBaseAddress()
{
	// CW: !!! ATTENTION !!!
	// CW: to be implemented for inter array padding
	// need a different argument list, but o.k. for now
	return 0;
}

void ArrayPaddingTransform::getArrayDimensionLowerBounds(SgInitializedName *constrName, List<const int>& bases)
{
	// CW: !!! ATTENTION !!!
	// CW: to be implemented. This is just a temporal workaround...
	// but it is o.k. as long as we do not do inter array padding since the
	// array padding algoritms are not relying on are lower bounds
	
	ROSE_ASSERT(constrName!=NULL);
	ROSE_ASSERT(bases.getLength()==0);
	
	SgConstructorInitializer *varConstructor = isSgConstructorInitializer(constrName->get_initializer());

	if(varConstructor!=NULL){
		SgExprListExp* constructorArgs=varConstructor->get_args();
		ROSE_ASSERT(constructorArgs!=NULL);
		
		const SgExpressionPtrList& argumentPtrList = constructorArgs->get_expressions();

		if(argumentPtrList.size()!=0){
			for(int i=0;i<argumentPtrList.size();i++)
			{
				bases.addElement(*new const int(0));
			}
		}
	}
}
