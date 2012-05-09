#include <rose.h>

using namespace std;

#include <rosepoly/data.h>
#include <rosepoly/Nodes.h>
#include <rosepoly/access_pattern.h>

Datum::Datum() {}

Datum::Datum( const Datum& d )
: mySymbol(d.get_symbol()), myType(d.get_type()), myIO(d.get_IO())
{
	for ( int i = 0 ; i < d.num_patterns() ; i++ ) {
		AccessPattern * pat = new AccessPattern(*d.get_pattern(i));
		myPatterns.push_back( pat );
		pat->set_parent(this);
	}
}

Datum::Datum( SgVariableSymbol * s, Type t, IO i ) : 
mySymbol(s), myType(t), myIO(i) {}

Datum::~Datum()
{
	for ( int i = 0 ; i < myPatterns.size() ; i++ )
		delete(myPatterns[i]);
}

Datum& Datum::operator = ( const Datum& d )
{
	mySymbol = d.get_symbol();
	myType = d.get_type();
	myIO = d.get_IO();
	myPatterns.clear();
	
	for ( int i = 0 ; i < d.num_patterns() ; i++ ) {
		AccessPattern * pat = new AccessPattern(*d.get_pattern(i));
		myPatterns.push_back( pat );
		pat->set_parent(this);
	}
}

string Datum::get_name() const
{
	return mySymbol->get_name().getString();
}

SgVariableSymbol * Datum::get_symbol() const { return mySymbol; }

Type Datum::get_type() const { return myType; }

void Datum::add_pattern( AccessPattern * pat )
{
	pat->set_id(myPatterns.size());
	myPatterns.push_back(pat);
	pat->set_parent(this);
	set_IO(pat->get_IO());
}

int Datum::num_patterns() const { return myPatterns.size(); }

void Datum::set_IO( IO i )
{
	if ( myIO == LOCAL_POINTER || myIO == LOCAL )
		return;
	
	switch(i)
	{
		case LOCAL:
		case INDUCTION:
		case PARAMETER:
		case LOCAL_POINTER:
		case INOUT:
			myIO = i;
			break;
		case IN:
			if ( myIO == OUT )
				myIO = INOUT;
			else if ( myIO == NOVALUE )
				myIO = IN;
			break;
		case OUT:
			if ( myIO == IN )
				myIO = INOUT;
			else if ( myIO == NOVALUE )
				myIO = OUT;
			break;
	}
}

IO Datum::get_IO() const { return myIO; }

bool Datum::is_parameter() const
{
	if ( myIO == PARAMETER )
		return true;
	
	if ( myIO == INDUCTION )
		return false;
	
	for ( int i = 0 ; i < myPatterns.size() ; i++ ) {
		if ( myPatterns[i]->get_IO() != IN || myPatterns[i]->get_dim() != 0 )
			return false;
	}
	return true;
}

AccessPattern * Datum::get_pattern( int pos ) const { return myPatterns[pos]; }

void Datum::print( int ident ) const
{
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<get_name()<<" is ";
	
	switch(myIO)
	{
		case IN:
			cout<<"IN";
			break;
		case OUT:
			cout<<"OUT";
			break;
		case INOUT:
			cout<<"INOUT";
			break;
		case LOCAL:
			cout<<"LOCAL";
			break;
		case INDUCTION:
			cout<<"INDUCTION";
			break;
		case PARAMETER:
			cout<<"PARAMETER";
			break;
		case LOCAL_POINTER:
			cout<<"LOCAL POINTER";
			break;
		default:
			cout<<"(no value.)";
	}
	cout<<endl;
}


