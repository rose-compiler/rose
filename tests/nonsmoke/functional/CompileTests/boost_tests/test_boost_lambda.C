/*
  This file demonstrates that ROSE 0.8.8a fails processing a for_each loop with
  the boost lambda library
  BUT ROSE 0.8.7a CAN process this file without problems!

  ROSE 0.8.8a binary: defaultTranslator -3 FAILS (Aborted)
  ROSE 0.8.7a source: defaultTranslator -5 OK

  Author: Markus Schordan, 17-Aug-2006
*/

#include<iostream>
#include<algorithm>
#include<vector>
#include<list>
#include<boost/lambda/lambda.hpp>


using namespace boost;

int main()
{
	typedef double numeric_type;

	// container ======
	//
	std::vector<numeric_type> mycontainer(4);

	// -------------------------------------------------
	//
	// == Lambda
	//
	std::for_each(mycontainer.begin(), mycontainer.end(), 
			boost::lambda::_1 +=1.0 );			

	return 0;
}
