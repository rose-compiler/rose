#ifndef BACKSTROKE_TESTCODEBUILDER_H
#define	BACKSTROKE_TESTCODEBUILDER_H

#include "builderTypes.h"

class BasicExpressionTest : public TestCodeBuilder
{
protected:
	virtual void build_();
public:
	BasicExpressionTest(const std::string& filename)
	: TestCodeBuilder(filename) {}
};


#endif	/* BACKSTROKE_TESTCODEBUILDER_H */

