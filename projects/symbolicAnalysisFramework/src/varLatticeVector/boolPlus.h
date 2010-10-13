#ifndef BOOLPLUS_H
#define BOOLPLUS_H

#include <string>
#include "rose.h"

class boolPlus
{
	int val;
	
	public:
	const static int UnsetVal = -1;
	const static int FalseVal = 0;
	const static int TrueVal = 1;
	
	static boolPlus Unset;
	static boolPlus False;
	static boolPlus True;
		
	boolPlus();
	
	boolPlus(bool initVal);
	
	boolPlus(int initVal);

	boolPlus(const boolPlus &initVal);
		
	void set(int newVal);

	void set(bool newVal);
	
	bool isUnset() const;
	
	bool isFalse() const;
	
	bool isTrue() const;
	
	bool operator == (const boolPlus &that) const;

	bool operator == (const bool &that) const;

	bool operator != (const boolPlus &that) const;

	bool operator != (const bool &that) const;
	
	bool operator < (const boolPlus &that) const;
	bool operator < (const bool &that) const;

	bool operator > (const boolPlus &that) const;
	bool operator > (const bool &that) const;
	
	bool operator >= (const boolPlus &that) const;
	bool operator >= (const bool &that) const;
	
	bool operator <= (const boolPlus &that) const;
	bool operator <= (const bool &that) const;
	
	boolPlus operator && (const boolPlus &that) const;
	boolPlus operator && (const bool &that) const;
	
	boolPlus operator || (const boolPlus &that) const;
	bool operator || (const bool &that) const;
	
	std::string str();
};

#endif
