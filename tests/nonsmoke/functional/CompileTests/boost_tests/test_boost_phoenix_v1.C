/*
  This file demonstrates that ROSE 0.8.8a can process Boost Phoenix V1.0
  Note: this test fails with Phoenix V2.0, see test_phoenix_v2.cc
  Author: Markus Schordan, 17-Aug-2006
*/

#include<iostream>
#include<algorithm>
#include<vector>
#include<list>

#include<boost/spirit/phoenix.hpp>

using namespace boost;

int main()
{
	typedef double numeric_type;

	// container ======
	//
	std::vector<numeric_type> mycontainer(4);
	std::vector<numeric_type>::iterator it;

	std::for_each(mycontainer.begin(), mycontainer.end(), 
			phoenix::arg1 += 1.0 );

	return 0;
}
