
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

	TestFunction f("matmul");
	TestExpression e1("c[i][j] = 0"), e2("c[i][j] += a[i][k] * b[k][j]");
	TestVariableLBL a("a"), b("b"), c("c");
	TestVariableLBL i("i"), j("j"), k("k");
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
	
	TestScopStatement s1(0, &for_j, &e1);
	
	TestScopLoop for_k(1, &for_j, &k);
	// LB = 0
	for_k.addUpperBoundTerm(&n, 1);
	for_k.addUpperBoundTerm(NULL, -1);
	for_k.setIncrement(1);
	
	TestScopStatement s2(0, &for_k, &e2);
	
	root.Traverse();
	
	TestDataAccess & da1 = getDataAccess<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(&e1);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s1_w_c;
		s1_w_c.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s1_w_c.back().push_back(std::pair<TestVariableLBL *, int>(&i, 1));
		s1_w_c.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s1_w_c.back().push_back(std::pair<TestVariableLBL *, int>(&j, 1));
	da1.addWrite(&c, s1_w_c);
	
	TestDataAccess & da2 = getDataAccess<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(&e2);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s2_w_c;
		s2_w_c.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_w_c.back().push_back(std::pair<TestVariableLBL *, int>(&i, 1));
		s2_w_c.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_w_c.back().push_back(std::pair<TestVariableLBL *, int>(&j, 1));
	da2.addWrite(&c, s2_w_c);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s2_r_c;
		s2_r_c.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_r_c.back().push_back(std::pair<TestVariableLBL *, int>(&i, 1));
		s2_r_c.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_r_c.back().push_back(std::pair<TestVariableLBL *, int>(&j, 1));
	da2.addRead(&c, s2_r_c);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s2_r_a;
		s2_r_a.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_r_a.back().push_back(std::pair<TestVariableLBL *, int>(&i, 1));
		s2_r_a.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_r_a.back().push_back(std::pair<TestVariableLBL *, int>(&k, 1));
	da2.addRead(&a, s2_r_a);
	std::vector<std::vector<std::pair<TestVariableLBL *, int> > > s2_r_b;
		s2_r_b.push_back(std::vector<std::pair<TestVariableLBL *, int> >());
			s2_r_b.back().push_back(std::pair<TestVariableLBL *, int>(&k, 1));
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

