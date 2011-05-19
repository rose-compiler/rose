
#include "test-common/PolyhedricDependency-test.hpp"
#include "test-common/PolyhedricContainer-test.hpp"
#include "test-common/ScopTree-test.hpp"
#include "common/ScheduleSpace.hpp"
#include "common/Exception.hpp"

#include <iostream>

using namespace PolyhedricAnnotation;
using namespace TestCommon;
using namespace ScopTree_;

int main() {

	TestFunction f("matcolsum");
	TestExpression e1("a[i] = 0"), e2("a[i] = a[i] + b[i][j]");
	TestVariableLBL a("a"), b("b");
	TestVariableLBL i("i"), j("j");
	TestVariableLBL n("n");
	
	TestScopRoot root(&f);
	
	TestScopLoop for_i(0, &root, &i);
	// LB = 0
	for_i.addUpperBoundTerm(&n, 1);
	for_i.addUpperBoundTerm(NULL, -1);
	for_i.setIncrement(1);
	
	TestScopStatement s1(0, &for_i, &e1);
	
	TestScopLoop for_j(1, &for_i, &j);
	// LB = 0
	for_j.addUpperBoundTerm(&n, 1);
	for_j.addUpperBoundTerm(NULL, -1);
	for_j.setIncrement(1);
	
	TestScopStatement s2(0, &for_j, &e2);
	
	root.Traverse();
	
	
	TestDataAccess & da1 = getDataAccess<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(&e1);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s1_w_a;
		s1_w_a.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s1_w_a.back().push_back(std::pair<TestVariableLBL *, int>(&i, 1));
	da1.addWrite(&a, s1_w_a);
	
	TestDataAccess & da2 = getDataAccess<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(&e2);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s2_w_a;
		s2_w_a.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_w_a.back().push_back(std::pair<TestVariableLBL *, int>(&i, 1));
	da2.addWrite(&a, s2_w_a);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s2_r_a;
		s2_r_a.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_r_a.back().push_back(std::pair<TestVariableLBL *, int>(&i, 1));
	da2.addRead(&a, s2_r_a);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s2_r_b;
		s2_r_b.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_r_b.back().push_back(std::pair<TestVariableLBL *, int>(&i, 1));
		s2_r_b.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_r_b.back().push_back(std::pair<TestVariableLBL *, int>(&j, 1));
	da2.addRead(&b, s2_r_b);
	
	//getPolyhedralProgram<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(&f).print(std::cout);

	std::vector<PolyhedricDependency::Dependency<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> *> * deps =
		PolyhedricDependency::ComputeDependencies<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(
			getPolyhedralProgram<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(&f)
		);
	
	Scheduling::ScheduleSpace<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> sched(
			getPolyhedralProgram<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(&f), 
			*deps
	);
/*	
	std::cout << "# ==================================================== Dependencies" << std::endl;
	size_t cnt = 1;
	std::vector<PolyhedricDependency::Dependency<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> *>::iterator it;
	for (it = deps->begin(); it != deps->end(); it++) {
		std::cout << "# ---------------------------------------------------  Dependency " << cnt++ << std::endl;
		(*it)->print(std::cout);
	}
*/	
	return 0;
}

