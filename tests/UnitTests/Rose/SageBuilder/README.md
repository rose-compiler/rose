# SageBuilder

See Doxygen Documentation: http://rosecompiler.org/ROSE_HTML_Reference/namespaceSageBuilder.html.

# Example Output

```bash
$ make check
make[1]: Entering directory `/home/too1/projects/rose/UNIT-86-unit-tests/compilation/tests/UnitTests/Rose/SageBuilder'
make librose.la -C ../../../../src
make[2]: Entering directory `/home/too1/projects/rose/UNIT-86-unit-tests/compilation/src'
make[2]: `librose.la' is up to date.
make[2]: Leaving directory `/home/too1/projects/rose/UNIT-86-unit-tests/compilation/src'
make  SgTypeBuildersUnitTests ExpressionBuilderUnitTests
make[2]: Entering directory `/home/too1/projects/rose/UNIT-86-unit-tests/compilation/tests/UnitTests/Rose/SageBuilder'
  CXX      SgTypeBuildersUnitTests.o
  CXXLD    SgTypeBuildersUnitTests
  CXX      ExpressionBuilderUnitTests.o
  CXXLD    ExpressionBuilderUnitTests
make[2]: Leaving directory `/home/too1/projects/rose/UNIT-86-unit-tests/compilation/tests/UnitTests/Rose/SageBuilder'
make  check-local
make[2]: Entering directory `/home/too1/projects/rose/UNIT-86-unit-tests/compilation/tests/UnitTests/Rose/SageBuilder'
  TEST   SgTypeBuildersUnitTests
  TEST   ExpressionBuilderUnitTests
make[2]: Leaving directory `/home/too1/projects/rose/UNIT-86-unit-tests/compilation/tests/UnitTests/Rose/SageBuilder'
make[1]: Leaving directory `/home/too1/projects/rose/UNIT-86-unit-tests/compilation/tests/UnitTests/Rose/SageBuilder'

$ cat SgTypeBuildersUnitTests.test
======== CUT ========
+ ./SgTypeBuildersUnitTests
SgTypeBuildersUnitTests[22541] 0.00099s rose[TRACE]: Starting SgTypeBuilders tests...
SgTypeBuildersUnitTests[22541] 0.00132s rose[TRACE]: Starting BuildBoolTypeTests... Done.
SgTypeBuildersUnitTests[22541] 0.00214s rose[TRACE]: Starting BuildNullptrTypeTests... Done.
SgTypeBuildersUnitTests[22541] 0.00245s rose[TRACE]: Starting BuildCharTypeTests... Done.
SgTypeBuildersUnitTests[22541] 0.00272s rose[TRACE]: Starting BuildDoubleTypeTests... Done.
SgTypeBuildersUnitTests[22541] 0.00303s rose[TRACE]: Starting SgTypeBuilders tests... Done.
======== CUT ========
```

# Main

The `main` function first initializes the diagnostics (logging, assertions, etc.) required to run the test suite.

```C++
int main() {
  UnitTests::Diagnostics::initialize();
```

Afterward, it calls the test suites `RunUnitTests()` function to actually run the tests:

```C++
  UnitTests::SageBuilder::SgTypeBuilders::RunUnitTests();
```

# Test organization

## Namespacing

The `SgTypeBuilders` units are a group of related `SgType` builder functions provided by the `SageBuilder` ROSE API. Therefore, we use a dedicated namespace `SgTypeBuilders` to encapsulate all of the associated unit tests:

```C++
namespace Rose {
namespace Tests {
namespace UnitTests {
namespace SageBuilder {
namespace SgTypeBuilders {
namespace Common {
  ...
} } } } } }
```

Note `Rose::Tests::UnitTests` is an absolute namespacing for all unit tests, whereas `SageBuilder::SgTypeBuilders` is a logical grouping based on the actual namespace within the ROSE API, i.e. `SageBuilder`, and the internal grouping of units, i.e. `SgTypeBuilders`.

There is also a `namespace Common` to encapsulate convenience functions for all these unit tests. For example, templated functions that are common to all `SgTypeBuilders` units.

### Namespacing for unit test suite

There is a main entry functin `SageBuilder::SgTypeBuilders::RunUnitTests()` that should be used to actually run the `SgTypeBuilders` test suite:

```C++
//-----------------------------------------------------------------------------
static void RunUnitTests() {
//-----------------------------------------------------------------------------
  ...
  BuildBoolTypeTests::RunUnitTests();
  ...
```

### Namespacing for individual units

Each unit has its own namespace. Here is an example for the unit `buildBoolType`:

```C++
//-----------------------------------------------------------------------------
namespace BuildBoolTypeTests {
//-----------------------------------------------------------------------------
static void HandlesDefaultParameter() {
  ...
}

static void RunUnitTests() {
  ...
  HandlesDefaultParameter();
  ...
} }
```

The namespace contains a main function `RunUnitTests` which is used to actually run all of the tests within the `namespace BuildBoolTypeTests`. In this case, there is one test, `HandlesDefaultParameter()`.

# Logging

Each unit test provides a local logging object to trace status:

```C++
  Sawyer::Message::Stream tlog(mlog[TRACE] << "Starting BuildNullptrTypeTests");

  ...

  tlog << "... Done.\n";
```

