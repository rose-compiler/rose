
#include "test-common/PolyhedricDependency-test.hpp"

const Polyhedron & getDomain(const std::pair<TestCommon::TestExpression *, size_t> obj) {
	return PolyhedricAnnotation::getDomain<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(obj.first).getDomainPolyhedron();
}

const std::vector<LinearExpression> & getScattering(const std::pair<TestCommon::TestExpression *, size_t> obj) {
	return PolyhedricAnnotation::getScattering<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(obj.first).getScattering();
}

size_t getDimension(const std::pair<TestCommon::TestExpression *, size_t> obj) {
	return PolyhedricAnnotation::getDomain<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(obj.first).getNumberOfIterators();
}

size_t getExtraDimension(const std::pair<TestCommon::TestExpression *, size_t> obj) {
	return PolyhedricAnnotation::getDomain<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(obj.first).getNumberOfGlobals();
}

template <>
bool isBottom<std::pair<TestCommon::TestExpression *, size_t> >(std::pair<TestCommon::TestExpression *, size_t> p) {
	return p.first == NULL;
}

template <>
std::pair<TestCommon::TestExpression *, size_t> getBottom<std::pair<TestCommon::TestExpression *, size_t> >() {
	return std::pair<TestCommon::TestExpression *, size_t>(NULL, 0);
}

template <>
std::ostream & operator << (std::ostream & out, const std::pair<TestCommon::TestExpression *, size_t> & p) {
	out << "< " << *p.first << ", " << p.second << " >";
	return out;
}


