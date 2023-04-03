This directory contains input that describes ROSE's IR types
============================================================

Each file in this directory defines a single ROSE IR node class. The
names of the files match the names of the classes. Although these
files are named *.h and appear to be C++ code, they are actually
inputs to the `rosebud` tool. The Rosebud system is described here.

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

* Problems with ROSETTA's domain specific language (DSL):
  * the DSL is undocumented
  * the ROSE IR node type declarations are not written in C++
  * the ROSETTA DSL is partly C++ function calls in a *.C file
  * additional class member declarations are in text files
  * additional member function implementations are in another text file
  * documentation is stored far away in yet another text file
  * the IR node type names are listed in yet another text file
  * the text files are not understood by IDEs as being C++ code
  * types of the data members are defined in ROSETTA's source code
  * data member types occur more than once in ROSETTA's source
  * ROSETTA doesn't allow abstract classes
  * virtual accessors/mutators are not directly supported
  * read-only properties are not directly supported
  * serialization/deserialization is not directly supported
  * it is difficult to conditionally define IR node types
* Problems with ROSETTA's generated code:
  * the code is ugly and hinders understanding during debugging
  * individual source files are huge and make IDEs sluggish
  * the file names are unrelated to type names and are hard to find
  * things that could be virtual are copy-pasted into each class
* Problems with ROSETTA's CxxGrammarMetaProgram generator:
  * error messages seldom indicate where the problem lies
  * errors are sent to standard output, not standard error
  * the part on standard error is often from `assert(false)`
  * the failure output seldom has information relevant to the cause
  * figuring out the cause often involves debugging CxxGrammarMetaProgram
  * some errors cause CxxGrammarMetaProgram to hang
* Problems with extending ROSETTA:
  * ROSETTA is monolithic and less modular than it could be
  * adding a new feature requires understanding ROSETTA source code
  * special cases are hard-coded throughout the source code
* Problems with ROSETTA's style:
  * naming conventions are not enforced or even warned
  * lack of documentation is not a warning
