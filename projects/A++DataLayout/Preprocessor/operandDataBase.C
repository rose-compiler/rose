#include "arrayPreprocessor.h"
#include <algorithm>
#include <vector>
// ArrayTransformationSupport::IndexingAccessEnumType
//     OperandDataBaseType::transformationOption = ArrayTransformationSupport::UnknownIndexingAccess;

// #################################################
//               DATA BASE SUPPORT
// #################################################

OperandDataBaseType::~OperandDataBaseType() {
	dimension = -1;
	lastOperand = -1;
	transformationOption = ArrayTransformationSupport::UnknownIndexingAccess;
}

OperandDataBaseType::OperandDataBaseType() {
	dimension = -1;
	lastOperand = -1;
	transformationOption = ArrayTransformationSupport::UnknownIndexingAccess;
}

OperandDataBaseType::OperandDataBaseType(const OperandDataBaseType & X) {
	// Deep copy semantics
	((OperandDataBaseType*) this)->operator=(X);
}

OperandDataBaseType & OperandDataBaseType::operator=(const OperandDataBaseType & X) {
	arrayOperandList = X.arrayOperandList;
	dimension = X.dimension;
	lastOperand = X.lastOperand;
	transformationOption = X.transformationOption;

	return *this;
}

int OperandDataBaseType::getDimension() const {
	ROSE_ASSERT (dimension > -1);

	// error checking
	for (vector<ArrayOperandDataBase>::const_iterator i = arrayOperandList.begin();
			i != arrayOperandList.end(); i++) {
		if (dimension != (*i).getDimension()) {
			printf("(*i).arrayVariableName = %s  dimension = %d (*i).dimension = %d \n",
					(*i).arrayVariableName.c_str(), dimension, (*i).dimension);
		}

		ROSE_ASSERT (dimension == (*i).getDimension() );
	}

	return dimension;
}

void OperandDataBaseType::setDimension(int dim) {
	// specify the maximum dimension of all array operands in the array statement
	dimension = dim;
	ROSE_ASSERT ( dimension > -1 );
}

int OperandDataBaseType::size() const {
	return arrayOperandList.size();
}

// generate a display string of the information in the index operand
string OperandDataBaseType::displayString() const {
	ROSE_ASSERT (this != NULL);

	printf("In OperandDataBaseType::displayString(): dimension = %d \n", dimension);

	// string returnString = variableName + " ";
	// string returnString = string("Operand Data Base: dimension = ") + StringUtility::numberToString(dimension) + string(" [ ");
	string returnString = "[ ";

	// printf ("In ArrayAssignmentStatementTransformation::displayString() (arrayOperandList.size() = %d) \n",arrayOperandList.size());
	vector<ArrayOperandDataBase>::const_iterator i;
	for (i = arrayOperandList.begin(); i != arrayOperandList.end(); i++) {
		// printf ("Looping through array operands (array operand = %s) \n",(*i).displayString());
		returnString += (*i).displayString() + " ";
	}

	returnString += "] ";

	return returnString;
}

ArrayOperandDataBase &
OperandDataBaseType::setVariableName(string X) {
	// set a locally reference copy of the string (this will disappear soon)
	// variableName = X;

	// Make sure that the dimension is set before adding array operands to the data base
	ROSE_ASSERT ( dimension > -1 );

	// Make sure that the user specified transformation options have been setup (even if none wee
	// specified by the user).
	ROSE_ASSERT ( transformationOption > ArrayTransformationSupport::UnknownIndexingAccess );

	// Get the dimension and the transformation option from the DataBase since it has been setup previously
	ArrayOperandDataBase tempOperand(X, dimension, transformationOption);
	bool appendedNewArrayOperand = ArrayOperandDataBase::contains(arrayOperandList, tempOperand);

	if (appendedNewArrayOperand == FALSE) {
		// printf ("append database info to array operand already present (name = %s) \n",X.c_str());

		// Reset the counter (counts the dimension associated with each index)
		IndexOperandDataBase::counter = 0;

		// Put the array operand into the data base
		arrayOperandList.push_back(ArrayOperandDataBase(X, dimension, transformationOption));
	} else {
		// printf ("Variable already exists (name = %s) \n",X.c_str());
	}

	ROSE_ASSERT (arrayOperandList.size() > 0);

	// return a reference to the new ArrayOperandDataBase object (or the exisiting one if X was not added)
	vector<ArrayOperandDataBase>::iterator i, found;
	for (i = arrayOperandList.begin(); i != arrayOperandList.end(); i++) {
		if ((*i).arrayVariableName == X)
			found = i;
	}

	ROSE_ASSERT (found != arrayOperandList.end());

	// Mark this new array operand as the target for all operations that would add an index
	lastOperand++;
	ROSE_ASSERT (lastOperand >= 0);

	// Temporary limit to size of data base so that we can debug the implementation
	ROSE_ASSERT (lastOperand < 10);

	return *found;
}

#if 1
// This function should not be required now since the data base is now located in the accumulator
// attribute and is no longer located in the synthesized attribute (where it was in an earlier
// implementation).

// This is a useful function when the database is not folded into the assembly process

void OperandDataBaseType::setVariableNameList(const list<string> & X) {
	// This interface is to support the addition of a list of variables into the data base this occurs
	// when we add all the variables represented in a synthesized attribute to a new attribute (as
	// part of the the assemble process).

#if 0
	printf ("In OperandDataBaseType::setVariableNameList(): Now add the information in X to the current database \n");
	printf ("... X names -------------  (list size = %d): %s \n",X.size(),StringUtility::listToString(X).c_str());
	// printf ("... variableNameList names (list size = %d): %s \n",variableNameList.size(),StringUtility::listToString(variableNameList).c_str());
#endif

	bool appendedNewArrayOperand = FALSE;
	list<string>::const_iterator i;
	for (i = X.begin(); i != X.end(); i++) {
		// printf ("Looping through variable names \n");
		setVariableName(*i);
	}
}
#endif

void OperandDataBaseType::internalAddIndexInformation(const string & variableName,
		const string & indexName, IndexOperandDataBase::constructorValues origin) {
	// This function should take a list of index names and compare the list of index names against
	// those stored in the data base. Will this help us identify unique elements in the data base?

	// This function provides an internal mechanism set the sort of indexing that is applied to array operands

	bool processedInfo = FALSE;

	// printf ("In OperandDataBaseType::addIndexInformation(): variableName = %s \n",variableName.c_str());

#if 0
	vector<ArrayOperandDataBase>::iterator j;
	for (j = arrayOperandList.begin(); j != arrayOperandList.end(); j++)
	{
		// printf ("Iterating through array operands in arrayOperandList (looking for match) (*j).displayString() = %s \n",(*j).displayString().c_str());

		// see if the variable is already in the data base
		if ( (*j).arrayVariableName == variableName )
		{
			// ROSE_ASSERT (processedInfo == FALSE);
			processedInfo = TRUE;
			// printf ("Processing array operand: found a match (processedInfo = %s) \n",(processedInfo == TRUE) ? "TRUE" : "FALSE");
			(*j).addIndexInformation(indexName,origin);

			// printf ("Added Index to (*j).arrayVariableName = %s indexName = %s \n",
			//      (*j).arrayVariableName.c_str(),indexName.c_str());
		}
		else
		{
			// printf ("Processing array operand: (processedInfo = %s) \n",(processedInfo == TRUE) ? "TRUE" : "FALSE");
		}
	}
#else
	int lastOperandIndex = arrayOperandList.size() - 1;

	// error checking
	ROSE_ASSERT (lastOperandIndex == lastOperand);

	// Add the index to the index database in the array operand database element
	arrayOperandList[lastOperandIndex].addIndexInformation(indexName, origin);
#endif
}

void OperandDataBaseType::addIndexInformation(const string & variableName,
		const string & indexName) {
#if 1
	IndexOperandDataBase::constructorValues originalOrigin = IndexOperandDataBase::notStaticlyKnown;
	internalAddIndexInformation(variableName, indexName, originalOrigin);
#else
	bool processedInfo = FALSE;

	// printf ("In OperandDataBaseType::addIndexInformation(): variableName = %s \n",variableName.c_str());

	vector<ArrayOperandDataBase>::iterator j;
	for (j = arrayOperandList.begin(); j != arrayOperandList.end(); j++)
	{
		// printf ("Iterating through array operands in arrayOperandList (looking for match) (*j).displayString() = %s \n",(*j).displayString().c_str());

		// see if the variable is already in the data base
		if ( (*j).arrayVariableName == variableName )
		{
			ROSE_ASSERT (processedInfo == FALSE);
			processedInfo = TRUE;
			// printf ("Processing array operand: found a match (processedInfo = %s) \n",(processedInfo == TRUE) ? "TRUE" : "FALSE");
			(*j).addIndexInformation(indexName);

			// printf ("Added Index to (*j).arrayVariableName = %s indexName = %s \n",
			//      (*j).arrayVariableName.c_str(),indexName.c_str());
		}
		else
		{
			// printf ("Processing array operand: (processedInfo = %s) \n",(processedInfo == TRUE) ? "TRUE" : "FALSE");
		}
	}
#endif
}

void OperandDataBaseType::addIntegerExpressionIndexInformation(const string & variableName,
		const string & indexName) {
#if 1
	IndexOperandDataBase::constructorValues originalOrigin = IndexOperandDataBase::scalarValue;
	internalAddIndexInformation(variableName, indexName, originalOrigin);
#else
	bool processedInfo = FALSE;

	// printf ("In OperandDataBaseType::addIndexInformation(): variableName = %s \n",variableName.c_str());

	vector<ArrayOperandDataBase>::iterator j;
	for (j = arrayOperandList.begin(); j != arrayOperandList.end(); j++)
	{
		// printf ("Iterating through array operands in arrayOperandList (looking for match) (*j).displayString() = %s \n",(*j).displayString().c_str());

		// see if the variable is already in the data base
		if ( (*j).arrayVariableName == variableName )
		{
			ROSE_ASSERT (processedInfo == FALSE);
			processedInfo = TRUE;
			// printf ("Processing array operand: found a match (processedInfo = %s) \n",(processedInfo == TRUE) ? "TRUE" : "FALSE");
			(*j).addIndexInformation(indexName);

			// printf ("Added Index to (*j).arrayVariableName = %s indexName = %s \n",
			//      (*j).arrayVariableName.c_str(),indexName.c_str());
		}
		else
		{
			// printf ("Processing array operand: (processedInfo = %s) \n",(processedInfo == TRUE) ? "TRUE" : "FALSE");
		}
	}
#endif
}

void OperandDataBaseType::removeDups() {
	sort(arrayOperandList.begin(), arrayOperandList.end());
	vector<ArrayOperandDataBase>::iterator it;
	it = unique(arrayOperandList.begin(), arrayOperandList.end());
	arrayOperandList.resize(it - arrayOperandList.begin());
}

ArrayTransformationSupport::IndexingAccessEnumType OperandDataBaseType::globalIndexingProperties() const {
	// Compute the global properties of the indexing in the current statement

	return transformationOption;
}

void OperandDataBaseType::setUserOptimizationAssertions(
		const list<int> & inputTransformationOptions) {
	// Allow user to specify optimization assertions
	// transformationOptions = inputTransformationOptions;
	transformationOption = ArrayTransformationSupport::interpretTransformationOptions(
			inputTransformationOptions);
}

