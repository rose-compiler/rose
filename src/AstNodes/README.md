This directory contains input that describes ROSE's IR types
============================================================

Each file in this directory defines a single ROSE IR node class. The
names of the files match the names of the classes. Although these
files are named *.h and appear to be C++ code, they are actually
inputs to the `rosebud` tool.

See the generated documentaton for the `::Rosebud` namespace, or look
at the documentation directly in $ROSE/src/Rosebud/BasicTypes.h.

Generating code
---------------

See $ROSE_SRC/src/generated/README.md for instructions.

ROSETTA
-------

ROSETTA is the system that Rosebud intends to replace. Rosebud intends
to improve upon these ROSETTA problems:

1. Problems with ROSETTA's domain specific language (DSL):
    1. the DSL is undocumented
    2. the ROSE IR node type declarations are not written in C++
    3. the ROSETTA DSL is partly C++ function calls in a *.C file
    4. additional class member declarations are in text files
    5. additional member function implementations are in another text file
    6. documentation is stored far away in yet another text file
    7. the IR node type names are listed in yet another text file
    8. the text files are not understood by IDEs as being C++ code
    9. types of the data members are defined in ROSETTA's source code
    10. data member types occur more than once in ROSETTA's source
    11. ROSETTA doesn't allow abstract classes
    12. virtual accessors/mutators are not directly supported
    13. read-only properties are not directly supported
    14. serialization/deserialization is not directly supported
    15. it is difficult to conditionally define IR node types
2. Problems with ROSETTA's generated code:
    1. the code is ugly and hinders understanding during debugging
    2. individual source files are huge and make IDEs sluggish
    3. the file names are unrelated to type names and are hard to find
    4. things that could be virtual are copy-pasted into each class
3. Problems with ROSETTA's CxxGrammarMetaProgram generator:
    1. error messages seldom indicate where the problem lies
    2. errors are sent to standard output, not standard error
    3. the part on standard error is often from `assert(false)`
    4. the failure output seldom has information relevant to the cause
    5. figuring out the cause often involves debugging CxxGrammarMetaProgram
    6. some errors cause CxxGrammarMetaProgram to hang
4. Problems with extending ROSETTA:
    1. ROSETTA is monolithic and less modular than it could be
    2. adding a new feature requires understanding ROSETTA source code
    3. special cases are hard-coded throughout the source code
5. Problems with ROSETTA's style:
    1. naming conventions are not enforced or even warned
    2. lack of documentation is not a warning

How Rosebud fixes these

1. The DSL is C++-like and documented. All parts of an
   AST node class definition are in a single source file including all
   the documentation, conditional compilation, and extra class
   members.
   
2. The general policy of Rosebud is that as little of the definition
   should be generated as possible. The bulk of the definition should
   be either copied verbatim from the input or implemented using
   modern C++ features in C++ source files.

3. Rosebud should know as little as possible about types that appear
   in member declarations such as in property declarations. For
   instance, if generated code needs to handle a special case type,
   then it should do so through normal C++ mechanisms like trait
   templates, not special case code generation.
   
4. Each AST node definition can be surrounded by CPP conditional
   compilation directives, preceded by CPP file inclusion directives,
   etc. These are copied verbatim into the generated files.
   
5. Rosebud segregates node types into their own header and
   implementation files. At present, due to the "all compilation units
   must include <sage3basic.h>" policy and the size of that header,
   compilation times have drastically increased. However, we already
   know that if headers include only definitions they actually need,
   then compile times can be greatly decreased. For example, it
   currently takes about 30 seconds to compile each AST node
   implementation file, but tests have shown that these times can be
   reduced by an order of magnitude by more frugal use of #include.
   
6. Rosebud emits diagnostics with line and column information and
   emits the source lines to standard error as context for the
   diagnostic message. Only internal logic errors are handled with
   assertions.

7. Rosebud is intended to be modular rather than monolithic. The core
   tool can emit YAML from the C++-like input, which other tools can
   injest. YAML is well supported in most programming languages.
   
8. Rosebud does various style checks such as checking for the presence
   of documentation. This is currently built into the `rosebud` tool,
   but we intend to move these linters to small programs (or scripts)
   that operate on the YAML output.

9. Rosebud could eventually be self-hosted since it uses an AST to
   represent the parsed DSL.

FAQ
---

Q1. Can the name of an AST node class be changed?

A1. Yes. In order to change SgFoo to SgBar you should do a number of
things:
	1. Change the class name in the SgFoo.h Rosebud input file.
	2. Rename the Rosebud input file from SgFoo.h to SgBar.h.
	3. Rename Git repository file from SgFoo.h to SgBar.h.
	4. Remove SgFoo from the src/ROSETTA/astNodeList file if present.
	5. Run Rosebud to regenerate files.
	6. Rename Git repository generated file SgFoo.C to SgBar.C
	7. Check the modified generated files into Git
