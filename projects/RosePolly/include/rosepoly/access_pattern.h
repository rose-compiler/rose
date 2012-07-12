

#ifndef ACCESS_PATTERN_H
#define ACCESS_PATTERN_H

#include <rosepoly/RosePollyMath.h>

class RosePollyCustom;

class AccessPattern
{
	vector<SgExpression*> Subs;
	SgExpression * refExp;
	
	Datum * parent;
	int ID;
	
	pollyMap * pattern;
	
	IO myIO;
	
public:
	
	AccessPattern( const AccessPattern& ap );
	AccessPattern( SgExpression * exp, IO i );
	
	~AccessPattern();
	
	void set_parent( Datum * p );
	void add_subscript( SgExpression * exp );
	void set_polly_map( pollyDomain * d, RosePollyCustom * c );
	void set_id( int i );
	
	int get_id() const;
	string get_name() const;
	SgExpression * get_refExp() const;
	Datum * get_parent() const;
	int get_dim() const;
	IO get_IO() const;
	bool has_valid_map() const;
	pollyMap * get_pattern() const;
	SgExpression * get_subscript( int pos ) const;
	void print( int ident ) const;
	
};


bool isRegularExp( SgNode * n );

AccessPattern * extract_pattern( SgPntrArrRefExp * arrRef, IO anIO );

bool extract_pattern_rec( AccessPattern * p, SgExpression * exp );

void search_for_access_patterns( SgExpression * exp, vector<AccessPattern*>& pList, IO anIO );

#endif




