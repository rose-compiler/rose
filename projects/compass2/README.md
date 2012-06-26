Compass2
========

A **static analysis** tool based on the [ROSE compiler framework](http://rosecompiler.org/).

## Configuration: `compass_parameters.xml`

A run-time configuration file can be used to tune your `Compass2` analyses.

### Target Directory

The AST constructed from the source code that you analyze is sure to contain third-party
library data. This could be the standard C++ libraries or the Boost C++ libraries, for example.
But more than likely, you only care about analyzing defects in the source code that you
explicitly specify. For this purpose, you can configure `Compass2` checkers to only analyze code
contained under a specific directory.

For example, this configuration will cause `Compass2` checkers to only emit analysis information
for source code anywhere under `/Users/too1/development/`.

```xml
  # `compass_parameters.xml` excerpt
  <general>
    <parameter name="target_directory">
      /export/tmp.too1/development/
    </parameter>
  </general>
```

**Important**: symbolic links are not handled properly at the moment.

## Add New Checker

For now, adding a checker involves a manual process, i.e. editing `compass_main.cpp`:

```C++
// compass_main.cpp

extern const Compass::Checker* const functionPointerChecker;
extern const Compass::Checker* const keywordMacroChecker;
extern const Compass::Checker* const nonGlobalCppDirectiveChecker;

...

traversals.push_back (functionPointerChecker);
traversals.push_back (keywordMacroChecker);
traversals.push_back (nonGlobalCppDirectiveChecker);
```
