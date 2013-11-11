/*
 * TALCAttribute.cpp
 *
 *  Created on: Oct 30, 2012
 *      Author: Kamal Sharma
 *
 *  This file is used to add attributes
 *  to struct declaration for ROSE.
 */

#include "rose.h"

class TALCAttribute : public AstAttribute
{


public:
	// Constructor
	TALCAttribute(): TALCStructDecl(false) { };

	bool isStructDecl()
	{
		return TALCStructDecl;
	}

	void setStructDecl(bool val)
	{
		TALCStructDecl = val;
	}

private:
	// To determine if this struct was
	// create by TALC
	bool TALCStructDecl;
};


