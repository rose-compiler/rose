#ifndef ROSE_ARRAY_OPERAND_DATABASE_H
#define ROSE_ARRAY_OPERAND_DATABASE_H

// #include "arrayTransformationSupport.h"

class IndexOperandDataBase
{
	// Supporting class for information about index objects used within array statements
public:

	// Global counter (should be reset before each transformation or it will count all operands
	// in a whole program)
	static int counter;

	enum constructorValues
	{
		unknown = 0,
		notStaticlyKnown = 1,
		scalarValue = 2,
		baseBoundValues = 3,
		baseBoundStrideValues = 4
	};

	// Name of index object
	string indexVariableName;

	// unique value for each operand allows them to be associated with operations outside of ths query
	int operandIndex;

	// In case we can discove the values directly (later) put the values here
	constructorValues origin;
	int scalar;
	int base;
	int bound;
	int stride;

	// Ignored in intial imlementation (we assume a bound in the complexity of array statements)
	bool indexed;

	// Index object could be indexed
	vector<IndexOperandDataBase> indexOperandList;

	~IndexOperandDataBase();
	IndexOperandDataBase();
	IndexOperandDataBase( const string & name, IndexOperandDataBase::constructorValues origin = notStaticlyKnown );
	IndexOperandDataBase( const IndexOperandDataBase & X );
	IndexOperandDataBase & operator= ( const IndexOperandDataBase & X );

	bool operator== ( const IndexOperandDataBase & X ) const;
	bool operator< ( const IndexOperandDataBase & X ) const;
	bool operator> ( const IndexOperandDataBase & X ) const;

	// merge Y into X
	static void merge( vector<IndexOperandDataBase> & X, const vector<IndexOperandDataBase> & Y );
	void merge( const IndexOperandDataBase & X );

	// Used to generate unique identifiers (degree of uniqueness can be improved later)
	string generateIdentifierString() const;

	// generate a display string of the information in the index operand
	string displayString() const;
};

class ArrayOperandDataBase
{
	// Supporting class for information about each array operand in the array statement
public:
	// Global counter (should be reset before each transformation or it will count all operands
	// in a whole program)
	static int counter;

	// unique value for each operand allows them to be associated with operations outside of ths query
	int operandIndex;

	string arrayVariableName;
	bool indexed;
	vector<IndexOperandDataBase> indexOperandList;

	// Type of indexing associated with this variable
	ArrayTransformationSupport::IndexingAccessEnumType indexingAccessCode;

	// Type of indexing associated with this variable
	ArrayTransformationSupport::IndexingAccessEnumType getIndexingAccessCode() const;
	void setIndexingAccessCode( ArrayTransformationSupport::IndexingAccessEnumType code );

	bool isIndirectlyAddressed;// ignored in intial implementation
	vector<ArrayOperandDataBase> arrayOperandList;

	// Dimension of array operand (could be different for each operand)
	int dimension;

	~ArrayOperandDataBase();
	ArrayOperandDataBase();
	ArrayOperandDataBase( const string & name, int dim,
			ArrayTransformationSupport::IndexingAccessEnumType indexingCode );
	ArrayOperandDataBase( const ArrayOperandDataBase & X );
	ArrayOperandDataBase & operator= ( const ArrayOperandDataBase & X );

	bool operator== ( const ArrayOperandDataBase & X ) const;
	bool operator< ( const ArrayOperandDataBase & X ) const;
	bool operator> ( const ArrayOperandDataBase & X ) const;

	// merge Y into X
	static void merge( vector<ArrayOperandDataBase> & X, const vector<ArrayOperandDataBase> & Y );
	static bool contains( const vector<ArrayOperandDataBase> & X, const ArrayOperandDataBase & arrayInfo );
	static void appendInfo( vector<ArrayOperandDataBase> & X, ArrayOperandDataBase & arrayInfo );
	void merge( const ArrayOperandDataBase & X );

	void addIndexInformation( const string & indexName, IndexOperandDataBase::constructorValues origin );

	// Used to generate unique identifiers (degree of uniqueness can be improved later)
	string generateIndexIdentifierString() const;
	string generateIdentifierString() const;
	string generateIndexVariableArgumentList() const;

	int getDimension() const;
	void setDimension( int dim );

	// generate a display string of the information in the array operand
	string displayString() const;
};

// ################################################
//                  DATA BASE SUPPORT
// ################################################

class OperandDataBaseType
{
	// This is a container class for the collection of operands in a single statement
	// (or at least this is how we use it, as I recall).
public:

	// Database for array operand info
	vector<ArrayOperandDataBase> arrayOperandList;

	// User specified assertions that control levels of optimizations
	ArrayTransformationSupport::IndexingAccessEnumType transformationOption;

	// maximum dimension of array operands in database
	int dimension;

	// value of index into vector of operands
	int lastOperand;

	~OperandDataBaseType();
	OperandDataBaseType();

	// OperandDataBaseType( const string & name,
	//      IndexOperandDataBase::constructorValues origin = IndexOperandDataBase::notStaticlyKnown );
	OperandDataBaseType( const OperandDataBaseType & X );
	OperandDataBaseType & operator= ( const OperandDataBaseType & X );

	// specify the maximum dimension of all array operands in the array statement
	void setDimension(int dim);
	int getDimension() const;

	// number of operands in the database
	int size() const;

	// access function for variableName
	// string getVariableName() const;
	ArrayOperandDataBase & setVariableName ( string X );

	// access function for variableName
	list<string> getVariableNameList() const;
	void setVariableNameList ( const list<string> & X );

	// Support for specification of indexing semantics
	void addIndexInformation ( const string & variableName, const string & indexName );
	void addIntegerExpressionIndexInformation ( const string & variableName, const string & indexName );

	// Internal support for Index semantics specification
	void internalAddIndexInformation ( const string & variableName, const string & indexName,
			IndexOperandDataBase::constructorValues originalOrigin );

	// Compute the global properties of the indexing in the current statement
	ArrayTransformationSupport::IndexingAccessEnumType globalIndexingProperties() const;

	// Allow user to specify optimization assertions
	void setUserOptimizationAssertions( const list<int> & transformationOptions );

	// generate a display string of the information in the array operand
	string displayString() const;

	void removeDups();
};

// endif for ROSE_ARRAY_OPERAND_DATABASE_H
#endif

