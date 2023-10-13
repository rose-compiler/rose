This directory contains input that describes ROSE's IR types
============================================================

Each file in this directory defines a single ROSE IR node class. The
names of the files match the names of the classes. Although these
files are named *.h and appear to be C++ code, they are actually
inputs to the `rosebud` tool. The Rosebud system is described here.

Generating code
---------------

See $ROSE_SRC/src/generated/README.md for instructions.

The input language
------------------

These header files, although they look like C++, are actually inputs
for the Rosebud code generator. The differences from C++ are:

* Each file must contain exactly one class definition for a ROSE IR
  type. The name of the file is normally the same as that of the
  class.
   
* Any C preprocessor conditional compilation directives that are
  opened before the start of the class definition must remain open
  until after the end of the class definition. E.g., you cannot
  have a `#if X` before the class that matches an `#else` or
  something inside the class body.
  
* A class definition body (the stuff between the definition's curly
  braces) can contain all the C++ stuff that normally appears in a
  class definition. However, you'll want to watch out that you don't
  explicitly define something that is also generated (e.g., default
  constructors and the destructor). What gets generated depends on
  the midends and backends that are employed for a particular run.
  
* Doxygen documentation is limited to one block comment per item;
  i.e., Doxygen comments are not combined. Furthermore, the only C++
  tokens that can separate the Doxygen comment from the entity it
  documents are access specifiers (e.g., "public:").
  
* A "property" is a ROSE IR concept. A property stores a value in the
  class and has zero or more accessors and/or mutators. In the
  ROSETTA style, a property named "foo" has a data member named
  `p_foo`, an accessor named `get_foo`, and a mutator named
  `set_foo`. Other styles are possible.
  
* A property declaration looks like a data member declaration that is
  decorated with at least one attribute in the `Rosebud`
  namespace. The attributes must appear before the start of the data
  member declaration. E.g., `[[Rosebud::property]] int foo = 0;` They
  can, of course, be on their own line since the file is parsed
  similarly to C++.
  
* The finer details of a property are controlled by the presence of
  property attributes. The set of attributes has been chosen so that
  the default common behavior generally doesn't require an attribute;
  only the uncommon behaviors require attributes.
  
* Rosebud understands "using" in attribute lists. If you have more
  than one Rosebud attribute, you can say the "Rosebud" part just
  once, like this: `[[using Rosebud: rosetta, ctor_arg]]` which is
  equivalent to `[[Rosebud::rosetta, Rosebud::ctor_arg]]`. Attributes
  can also be split among multiple lists, so `[[Rosebud::rosetta]]
  [[Rosebud::ctor_arg]]` is the same thing.
  
* Properties can be surrounded by C preprocessor conditional
  compilation directives. These can surround individual properties or
  multiple properties, but must be closed before the end of the class
  definition. However, it is not currently possible to use
  conditional compilation to define alternate declarations for a
  single property (the property is either all or nothing).
   
* Because Rosebud input does not pass through a C preprocessor (it
  cannot, for technical reasons related to ROSETTA), it is not
  possible to use CPP macros in property declarations.
  
* Any code appearing after the class definition is ignored since there
  is no way to pass this code to ROSETTA. This includes C preprocessor
  directives. Any conditional compilation that was opened prior to the
  class will be closed with `#endif` automatically, although it is
  probably wise to also close them explicitly for the sake of IDEs
  that are presenting/editing the Rosebud input. (FIXME: this should
  cause a Rosebud error to be emitted.)
  
Class attributes
----------------

A class can be preceded by one or more of the following Rosebud
attributes.

* The `Rosebud::abstract` attribute indicates that the class cannot
  be instantiated. This is useful for base classes where we only want
  to be able to instantiate its derived classes.

  Note: The ROSETTA backend only partially honors this. The last
  argument of NEW_NONTERMINAL_MACRO is set to false, but the generated
  class can still be instantiated. In fact, adding a pure virtual
  function to the class to force it to never be instantiated causes
  problems in ROSETTA-generated code.

Property attributes
-------------------

* The `Rosebud::property` attribute marks what would otherwise appear
  to be a data member declaration as a property declaration. Since the
  presence of any Rosebud attribute is sufficient, the "property"
  attribute only necessary when no other Rosebud attribute is
  specified.
  
* The `Rosebud::data` attribute takes exactly one argument which is a
  symbol naming the data member that will be declared to store the
  property value.  The default depends on the backend (see below).
  
* The `Rosebud::accessors` attribute, which requires an argument list
  even if it's empty, specifies the symbols to use as the accessor
  member function names. The default depends on the backend (see
  below).  If the attribute argument list is empty, then no accessor
  functions are generated.
  
* The `Rosebud::mutators` attribute, which requires an argument list
  even if it's empty, specifies the symbols to use as the mutator
  member function names. The default depends on the backend (see
  below). If the attribute argument list is empty, then no mutator
  functions are generated.
  
* The `Rosebud::ctor_arg` means that the property's value will be
  passed to a generated constructor. Besides the default constructor,
  each IR type can have one additional generated constructor. This
  constructor has an argument for each "ctor_arg" property in the
  order they are declared starting with those in the base class,
  recursively.
  
* The `Rosebud::cloneptr` attribute causes the `copy` member function
  to allocate a new instance of the pointed object using the
  compile-time type of that object and its copy constructor. This is
  intended to be a temporary attribute replaced in the future by a
  smart pointer type. Besides, the ROSETTA-generated code in this case
  seems to incorrectly handle pointers to polymorphic types.

* The `Rosebud::no_serialize` attribute indicates that the property
  should not participate in serialization or
  deserialization. Normally, if one or more serialization backends are
  used, they will generate code to serialize and deserialize every
  property.
  
* The `Rosebud::large` attribute indicates that the property value is
  large and expensive to copy. Therefore, in addition to any other
  mutators, the accessors will be overloaded to return a non-const
  reference to the property's data member. Use this sparingly because
  it will not be compatible with all planned Rosebud backends (e.g.,
  it is not possible to generate property observers or thread safety
  if the property value can be modified directly.
  
* The `Rosebud::rosetta` attribute indicates that the property should
  be generated in a manner compatible with ROSETTA-generated
  properties. This does not necessarily mean that a ROSETTA backend is
  necessary.
  
* The `Rosebud::traverse` attribute is used by the ROSETTA backend to
  indicate that a property is a pointer to another IR node and that
  the edge formed by the pointer is part of a tree data
  structure. I.e., the pointee's parent pointer is assumed to point
  back to the object that points to that pointee.
  
Midends
-------

No mid-ends are defined at this time. The intention is that two kinds
of midend analysis could be supported:

* analysis that's linked into the `rosebud` program and operates
  directly on the Rosebud IR.
  
* standalone analysis written in any language and which inputs a
  machine-readable representation of the Rosebud IR and produces a
  new machine-readable IR for consumption by one or more backends.

Backends
--------

A backend is responsible for producing code, primarily the C++ class
definitions corresponding to the Rosebud inputs. Backends, like
midends, come in two forms:

* backends that are linked into the `rosebud` program and consume the
  Rosebud IR directly.
  
* standalone backends written in any language and which input a
  machine-readable representation of the Rosebud IR.
  
Rosebud currently ships with two backends:

* The YAML backend, invoked with `--backend=yaml`, is an internal
  backend that traverses the Rosebud IR and produces YAML output
  that's intended to be consumed by external midend and backend tools.
  
* The ROSETTA backend, invoked with `--backend=rosetta`, is an
  internal backend that traverses the Rosesbud IR and ROSETTA input
  and C++ implementations of accessors, mutators, constructors,
  etc. Although this backend generates ROSETTA input, its goal is to
  gradually produce more of the C++ directly in order to wean off
  ROSETTA. See below for why we might want to do this.
  
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

1. The DSL is C++-like and documented (this README). All parts of an
   AST node class definition is in a single source file including all
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
