
#include "test-common/PolyhedricContainer-test.hpp"
#include "common/PolyhedricDependency.hpp"

// For generic QUAST usage.

const Polyhedron & getDomain(const std::pair<TestCommon::TestExpression *, size_t> obj);

const std::vector<LinearExpression> & getScattering(const std::pair<TestCommon::TestExpression *, size_t> obj);

size_t getDimension(const std::pair<TestCommon::TestExpression *, size_t> obj);

size_t getExtraDimension(const std::pair<TestCommon::TestExpression *, size_t> obj);

