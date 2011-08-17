#include "arrayPreprocessor.h"

// #include "arrayAssignmentStatementSynthesizedAttribute.h"

int ArrayOperandDataBase::counter = 0;

string ArrayOperandDataBase::displayString() const {
	string returnString = arrayVariableName + " ( ";

	// printf ("In ArrayOperandDataBase::displayString() (indexOperandList.size() = %d) \n",indexOperandList.size());
	vector<IndexOperandDataBase>::const_iterator i;
	for (i = indexOperandList.begin(); i != indexOperandList.end(); i++) {
		// printf ("In ArrayOperandDataBase::displayString(): index operand = %s \n",(*i).displayString());
		returnString += (*i).displayString() + " ";
	}

	returnString += ") ";
	return returnString;
}

int ArrayOperandDataBase::getDimension() const {
	ROSE_ASSERT (dimension > -1);
	return dimension;
}

void ArrayOperandDataBase::setDimension(int dim) {
	// specify the maximum dimension of all array operands in the array statement
	dimension = dim;
}

ArrayTransformationSupport::IndexingAccessEnumType ArrayOperandDataBase::getIndexingAccessCode() const {
	// default value used for testing
	// return ArrayTransformationSupport::VariableSizeVariableStride;
	// return OperandDataBaseType::transformationOptions;
	return indexingAccessCode;
}

void ArrayOperandDataBase::setIndexingAccessCode(
		ArrayTransformationSupport::IndexingAccessEnumType code) {
	// specify the maximum dimension of all array operands in the array statement
	indexingAccessCode = code;
}

string ArrayOperandDataBase::generateIndexVariableArgumentList() const {
	// Used to generate unique identifiers (degree of uniqueness can be improved later)
	string returnString;

	// printf ("In ArrayOperandDataBase::displayString() (indexOperandList.size() = %d) \n",indexOperandList.size());
	vector<IndexOperandDataBase>::const_iterator i;
	for (i = indexOperandList.begin(); i != indexOperandList.end(); i++) {
		// printf ("In ArrayOperandDataBase::displayString(): index operand = %s \n",(*i).displayString());
		if (i == indexOperandList.begin())
			returnString += (*i).indexVariableName;
		else
			returnString += string(",") + (*i).indexVariableName;
	}

	return returnString;
}

string ArrayOperandDataBase::generateIndexIdentifierString() const {
	// Used to generate unique identifiers (degree of uniqueness can be improved later)
	string returnString;

	// printf ("In ArrayOperandDataBase::displayString() (indexOperandList.size() = %d) \n",indexOperandList.size());
	// for (vector<IndexOperandDataBase>::iterator i = indexOperandList.begin(); i != indexOperandList.end(); i++)
	vector<IndexOperandDataBase>::const_iterator i;
	for (i = indexOperandList.begin(); i != indexOperandList.end(); i++) {
		// printf ("In ArrayOperandDataBase::displayString(): index operand = %s \n",(*i).displayString());
		returnString += string("_") + (*i).generateIdentifierString();
	}

	return returnString;
}

string ArrayOperandDataBase::generateIdentifierString() const {
	// Used to generate unique identifiers (degree of uniqueness can be improved later)
	string returnString = string("_") + arrayVariableName + generateIndexIdentifierString();

#if 0
	// printf ("In ArrayOperandDataBase::displayString() (indexOperandList.size() = %d) \n",indexOperandList.size());
	for (vector<IndexOperandDataBase>::iterator i = indexOperandList.begin(); i != indexOperandList.end(); i++)
	{
		// printf ("In ArrayOperandDataBase::displayString(): index operand = %s \n",(*i).displayString());
		returnString += string("_") + (*i).generateIdentifierString();
	}
#endif
	return returnString;
}

ArrayOperandDataBase::~ArrayOperandDataBase() {
}

ArrayOperandDataBase::ArrayOperandDataBase() {
	operandIndex = -1;
	arrayVariableName = "";
	indexed = FALSE;
	// indexOperandList      = 0;

	isIndirectlyAddressed = FALSE;
	// arrayOperandList      = 0;

	indexingAccessCode = ArrayTransformationSupport::UnknownIndexingAccess;
}

ArrayOperandDataBase::ArrayOperandDataBase(const string & name, int dim,
		ArrayTransformationSupport::IndexingAccessEnumType indexingCode) {
	operandIndex = counter++;
	arrayVariableName = name;

	dimension = dim;

	indexed = FALSE;
	// indexOperandList      = 0;

	isIndirectlyAddressed = FALSE;
	// arrayOperandList      = 0;

	// indexingAccessCode    = ArrayTransformationSupport::UnknownIndexingAccess;
	indexingAccessCode = indexingCode;
}

ArrayOperandDataBase::ArrayOperandDataBase(const ArrayOperandDataBase & X) {
	// Deep copy semantics
	((ArrayOperandDataBase*) this)->operator=(X);
}

ArrayOperandDataBase &
ArrayOperandDataBase::operator=(const ArrayOperandDataBase & X) {
	operandIndex = X.operandIndex;
	arrayVariableName = X.arrayVariableName;
	indexed = X.indexed;
	indexOperandList = X.indexOperandList;

	isIndirectlyAddressed = X.isIndirectlyAddressed;
	arrayOperandList = X.arrayOperandList;

	dimension = X.dimension;
	ROSE_ASSERT (dimension > -1);

	indexingAccessCode = X.indexingAccessCode;
	ROSE_ASSERT (indexingAccessCode > 0);

	return *this;
}

bool ArrayOperandDataBase::operator==(const ArrayOperandDataBase & X) const {
	return (arrayVariableName == X.arrayVariableName) ? TRUE : FALSE;
}

bool ArrayOperandDataBase::operator<(const ArrayOperandDataBase & X) const {
	return (arrayVariableName < X.arrayVariableName) ? TRUE : FALSE;
}

bool ArrayOperandDataBase::operator>(const ArrayOperandDataBase & X) const {
	return (arrayVariableName > X.arrayVariableName) ? TRUE : FALSE;
}

void ArrayOperandDataBase::merge(const ArrayOperandDataBase & X) {
	// merge the input database information into the local database

	// printf ("In ArrayOperandDataBase::merge(X): arrayVariableName = %s \n",arrayVariableName.c_str());

	ROSE_ASSERT (arrayVariableName == X.arrayVariableName);

	// don't change the operand index unless this one has never been set
	operandIndex = (operandIndex >= 0) ? operandIndex : X.operandIndex;

	indexed = indexed || X.indexed;

	// merge arg2 into arg1
	IndexOperandDataBase::merge(indexOperandList, X.indexOperandList);

	// support for indirect addressing
	isIndirectlyAddressed = isIndirectlyAddressed || X.isIndirectlyAddressed;
	// arrayOperandList      = X.arrayOperandList;
}

void ArrayOperandDataBase::merge(vector<ArrayOperandDataBase> & X,
		const vector<ArrayOperandDataBase> & Y) {
	// traverse Y and add elements to X if they represent new operands

	// printf ("In ArrayOperandDataBase::merge(X,Y): merging list Y into list X \n");
	// printf ("X.size() = %d  Y.size() = %d \n",X.size(),Y.size());

	vector<ArrayOperandDataBase> listOfArrayOperandsToAdd;
	vector<ArrayOperandDataBase>::const_iterator i;
	for (i = Y.begin(); i != Y.end(); i++) {
		// printf ("Iterating through array operands in Y (*i).displayString() = %s \n",(*i).displayString().c_str());
		vector<ArrayOperandDataBase>::iterator j;
		for (j = X.begin(); j != X.end(); j++) {
			// printf ("Iterating through array operands in X (*j).displayString() = %s \n",(*j).displayString().c_str());
			if (*j == *i) {
				// printf ("Processing array operand: calling (*j).merge(*i) \n");
				(*j).merge(*i);
			} else {
				// printf ("Processing array operand: calling X.push_back(*i) \n");
				listOfArrayOperandsToAdd.push_back(*i);
			}
		}

	}

	// Now add the identified array operands to the X list     
	vector<ArrayOperandDataBase>::iterator k;
	for (k = listOfArrayOperandsToAdd.begin(); k != listOfArrayOperandsToAdd.end(); k++) {
		// printf ("Adding identified array operand to X list: (*k).displayString() = %s \n",(*k).displayString().c_str());
		X.push_back(*k);
	}

	// IndexOperandDataBase::merge( X, Y );

#if 0
	printf ("Exiting in ArrayOperandDataBase::merge() \n");
	ROSE_ABORT();
#endif
}

bool ArrayOperandDataBase::contains(const vector<ArrayOperandDataBase> & X,
		const ArrayOperandDataBase & arrayInfo) {
	// Search the X list and look for a related copy of arrayInfo (return TRUE if found)

	bool returnValue = FALSE;

	// printf ("In ArrayOperandDataBase::contains(): arrayInfo.displayString() = %s \n",arrayInfo.displayString());
	vector<ArrayOperandDataBase>::const_iterator i;
	for (i = X.begin(); i != X.end(); i++) {
		// printf ("Iterating through array operands in X (looking for match) (*i).displayString() = %s \n",(*i).displayString().c_str());
		if (*i == arrayInfo) {
			returnValue = TRUE;
			// printf ("Processing array operand: found a match (returnValue = %s) \n",(returnValue == TRUE) ? "TRUE" : "FALSE");
		} else {
			// printf ("Processing array operand: (returnValue = %s) \n",(returnValue == TRUE) ? "TRUE" : "FALSE");
		}
	}

	return FALSE;
}

void ArrayOperandDataBase::addIndexInformation(const string & indexName,
		IndexOperandDataBase::constructorValues origin) {
	// printf ("In ArrayOperandDataBase::addIndexInformation(): indexName = %s \n",indexName.c_str());

	indexOperandList.push_back(IndexOperandDataBase(indexName, origin));
}

