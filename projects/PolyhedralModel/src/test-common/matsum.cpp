
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
	TestExpression e("a = a + b[i][j]");
	TestVariableLBL a("a"), b("b");
	TestVariableLBL i("i"), j("j");
	TestVariableLBL n("n");
	
	TestScopRoot root(&f);
	
	TestScopLoop for_i(0, &root, &i);
	// LB = 0
	for_i.addUpperBoundTerm(&n, 1);
	for_i.addUpperBoundTerm(NULL, -1);
	for_i.setIncrement(1);
	
	TestScopLoop for_j(0, &for_i, &j);
	// LB = 0
	for_j.addUpperBoundTerm(&n, 1);
	for_j.addUpperBoundTerm(NULL, -1);
	for_j.setIncrement(1);
	
	TestScopStatement s(0, &for_j, &e);
	
	root.Traverse();
	
	TestDataAccess & da = getDataAccess<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(&e);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s_w_a;
	da.addWrite(&a, s_w_a);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s_r_a;
	da.addRead(&a, s_r_a);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s_r_b;
		s_r_b.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s_r_b.back().push_back(std::pair<TestVariableLBL *, int>(&i, 1));
		s_r_b.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s_r_b.back().push_back(std::pair<TestVariableLBL *, int>(&j, 1));
	da.addRead(&b, s_r_b);

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

