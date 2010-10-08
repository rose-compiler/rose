#ifndef LOGICAL_H
#define LOGICAL_H

class LogicalCond
{
	public:
	// the basic logical operations that must be supported by any implementation of 
	// a logical condition: NOT, AND and OR
	// Return true if this causes the LogicalCond object to change and false otherwise.
	//virtual bool notUpd()=0;
	virtual bool andUpd(LogicalCond& that)=0;
	virtual bool orUpd(LogicalCond& that)=0;

	/*// returns true if this logical condition is false and false otherwise
	virtual bool isFalse()=0;*/
	
	// returns true if this logical condition must be true and false otherwise
	virtual bool mayTrue()=0;

	// returns true if this logical condition may be false and false otherwise
	bool mustFalse()
	{
		return !mayTrue();
	}
	
/*	virtual bool setToTrue()=0;
	virtual bool setToFalse()=0;*/

	// returns a copy of this LogicalCond object
	virtual LogicalCond* copy()=0;
	
	virtual string str(string indent="")=0;
};

#endif
