#include "arrayPreprocessor.h"

int IndexOperandDataBase::counter = 0;

string IndexOperandDataBase::displayString() const {
	string returnString = indexVariableName;

	// printf ("In IndexOperandDataBase::displayString() indexVariableName = %s \n",indexVariableName);

	return returnString;
}

string IndexOperandDataBase::generateIdentifierString() const {
	string returnString;

#if 0
	switch (origin)
	{
		case unknown: returnString = "unknown";
		break;
		case notStaticlyKnown: returnString = "notStaticlyKnown";
		break;
		case scalarValue: returnString = "scalar0";
		break;
		case baseBoundValues: returnString = "scalar1";
		break;
		case baseBoundStrideValues: returnString = "scalar2";
		break;
		default:
		printf ("Error: default reached in IndexOperandDataBase::generateIdentifierString() \n");
		ROSE_ABORT();
		break;
	}
#else
	returnString = indexVariableName;
#endif

	return returnString;
}

IndexOperandDataBase::~IndexOperandDataBase() {
}

IndexOperandDataBase::IndexOperandDataBase() {
	indexVariableName = "";

	operandIndex = 0;

	origin = notStaticlyKnown;
	scalar = 0;
	base = 0;
	bound = -1;
	stride = 1;

	// not used at present
	indexed = FALSE;
	// indexOperandList = 0;
}

IndexOperandDataBase::IndexOperandDataBase(const string & name, constructorValues inputOrigin) {
	// This constructor allows the specification of an index expression (a string) and additional info
	// about how its origin.

	indexVariableName = name;

	// Count the axis associated with the index
	operandIndex = counter++;

	// origin = notStaticlyKnown;
	origin = inputOrigin;

	scalar = 0;
	base = 0;
	bound = -1;
	stride = 1;

	// not used at present
	indexed = FALSE;
	// indexOperandList = 0;
}

IndexOperandDataBase::IndexOperandDataBase(const IndexOperandDataBase & X) {
	// Deep copy semantics
	((IndexOperandDataBase*) this)->operator=(X);
}

IndexOperandDataBase &
IndexOperandDataBase::operator=(const IndexOperandDataBase & X) {
	// name of index object
	indexVariableName = X.indexVariableName;

	operandIndex = X.operandIndex;

	origin = X.origin;
	scalar = X.scalar;
	base = X.base;
	bound = X.bound;
	stride = X.stride;

	// not used at present
	indexed = X.indexed;
	indexOperandList = X.indexOperandList;

	return *this;
}

bool IndexOperandDataBase::operator==(const IndexOperandDataBase & X) const {
	return (indexVariableName == X.indexVariableName) ? TRUE : FALSE;
}

bool IndexOperandDataBase::operator<(const IndexOperandDataBase & X) const {
	return (indexVariableName < X.indexVariableName) ? TRUE : FALSE;
}

bool IndexOperandDataBase::operator>(const IndexOperandDataBase & X) const {
	return (indexVariableName > X.indexVariableName) ? TRUE : FALSE;
	// return (*this <= X) ? FALSE : TRUE;
}

void IndexOperandDataBase::merge(const IndexOperandDataBase & X) {
	// merge the input database information into the local database

	// printf ("In IndexOperandDataBase::merge(X): indexVariableName = %s \n",indexVariableName.c_str());

	ROSE_ASSERT (indexVariableName == X.indexVariableName);

	// don't change the operand index unless this one has never been set
	operandIndex = (operandIndex >= 0) ? operandIndex : X.operandIndex;

	origin = X.origin;
	scalar = X.scalar;
	base = X.base;
	bound = X.bound;
	stride = X.stride;

	indexed = indexed || X.indexed;

	// Not used currently
	// merge arg2 into arg1
	// IndexOperandDataBase::merge(indexOperandList,X.indexOperandList);
}

void IndexOperandDataBase::merge(vector<IndexOperandDataBase> & X,
		const vector<IndexOperandDataBase> & Y) {
	// traverse Y and add elements to X if they represent new operands

	// printf ("In IndexOperandDataBase::merge(X,Y): merging list Y into list X \n");
	// printf ("In IndexOperandDataBase::merge(X,Y): X.size() = %d  Y.size() = %d \n",X.size(),Y.size());

	vector<IndexOperandDataBase> listOfIndexOperandsToAdd;
	vector<IndexOperandDataBase>::const_iterator i;
	for (i = Y.begin(); i != Y.end(); i++) {
		// printf ("Iterating through array operands in Y (*i).displayString() = %s \n",(*i).displayString().c_str());
		vector<IndexOperandDataBase>::iterator j;
		for (j = X.begin(); j != X.end(); j++) {
			// printf ("Iterating through array operands in X (*j).displayString() = %s \n",(*j).displayString().c_str());
			if (*j == *i) {
				// printf ("Processing index operand: calling (*j).merge(*i) \n");
				(*j).merge(*i);
			} else {
				// printf ("Processing index operand: calling X.push_back(*i) \n");
				X.push_back(*i);
			}
		}
	}

	// Now add the identified index operands to the X list     
	vector<IndexOperandDataBase>::iterator k;
	for (k = listOfIndexOperandsToAdd.begin(); k != listOfIndexOperandsToAdd.end(); k++) {
		// printf ("Adding identified index operand to X list: (*k).displayString() = %s \n",(*k).displayString().c_str());
		X.push_back(*k);
	}

	// IndexOperandDataBase::merge( X, Y );
#if 0
	printf ("Exiting in IndexOperandDataBase::merge() \n");
	ROSE_ABORT();
#endif
}

