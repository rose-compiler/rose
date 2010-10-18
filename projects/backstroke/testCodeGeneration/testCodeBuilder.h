#ifndef BACKSTROKE_TESTCODEBUILDER_H
#define	BACKSTROKE_TESTCODEBUILDER_H

#include "builderTypes.h"

class BasicExpressionTest : public TestCodeBuilder
{
public:
	BasicExpressionTest(SgProject* project, bool is_cxx_style = false)
	: TestCodeBuilder(project, is_cxx_style) {}

	virtual void build();
};

class ComplexExpressionTest : public TestCodeBuilder
{
	public:
	ComplexExpressionTest(SgProject* project, bool is_cxx_style = false)
	: TestCodeBuilder(project, is_cxx_style) {}

	virtual void build();
};


#endif	/* BACKSTROKE_TESTCODEBUILDER_H */

