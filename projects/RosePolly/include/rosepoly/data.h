
#ifndef DATA_H
#define DATA_H

class AccessPattern;

typedef enum {
		NOTYPE = 0,
		INT = 1,
		FLOAT = 2,
		DOUBLE = 3
} Type;

typedef enum {
	NOVALUE = -1,
	IN		= 0,
	OUT		= 1,
	INOUT	= 2,
	LOCAL	= 3,
	PARAMETER = 4,
	INDUCTION = 5,
	LOCAL_POINTER = 6
} IO;

class Datum {
	
	SgVariableSymbol * mySymbol;
	Type myType;
	IO myIO;
	
	vector<AccessPattern*> myPatterns;
	
public:
	
	Datum();
	Datum( const Datum& d );
	Datum( SgVariableSymbol * s, Type t, IO i );
	
	Datum& operator = ( const Datum& d );
	void add_pattern( AccessPattern * pat );
	void set_IO( IO i );
	
	AccessPattern * get_pattern( int pos ) const;
	int num_patterns() const;
	string get_name() const;
	IO get_IO() const;
	bool is_parameter() const;
	SgVariableSymbol * get_symbol() const;
	Type get_type() const;
	
	~Datum();
	
	void print( int ident ) const;
	
};

typedef map<string,Datum> symbol_table;

#endif

