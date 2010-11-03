#ifndef BACKSTROKE_TESTCODEBUILDER_H
#define	BACKSTROKE_TESTCODEBUILDER_H

#include "builderTypes.h"

class BasicExpressionTest : public TestCodeBuilder
{
public:
	BasicExpressionTest(const std::string& name, bool is_cxx_style = false)
	: TestCodeBuilder(name, is_cxx_style) {}

protected:
	virtual void build();
};

class ComplexExpressionTest : public TestCodeBuilder
{
	public:
	ComplexExpressionTest(const std::string& name, bool is_cxx_style = false)
	: TestCodeBuilder(name, is_cxx_style) {}

protected:
	virtual void build();
};


#endif	/* BACKSTROKE_TESTCODEBUILDER_H */

