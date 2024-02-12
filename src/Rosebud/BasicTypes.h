#ifndef Rosebud_BasicTypes_H
#define Rosebud_BasicTypes_H

#include <Sawyer/Clexer.h>

/** Rosebud is a tool to generate abstract syntax trees.
 *
 *  The Rosebud tool (`rosebud` command) reads class definitions for abstract syntax tree (AST) vertex types and generates C++
 *  source code.
 *
 *  The <b>definition language</b> is C++-like, enabling any C++ programmer to author or modify AST vertex classes. The C++-like
 *  language means the input is natively understood by integrated development environments (IDEs) and code editors. Rosebud aims to
 *  make common class definitions easy, and more complex definitions possible. The language supports Doxygen-style comments,
 *  delayed-decision conditional compilation, independent AST class hierarchies, abstract classes with pure virtual member
 *  functions, read-only properties, and more.
 *
 *  The Rosebud tool uses <b>modern translation techniques</b> such as referential lexical tokens and partial parsing. Rosebud can
 *  process the 400 "SgAsm" AST definitions and generate code in a fraction of a second. Its diagnostic messages (warnings and
 *  errors) follow the LLVM style, are readable by almost all IDEs, include the input source line and position indicators, and are
 *  colorized. Rosebud's AST for its intermediate representation is self hosted and fully documented.
 *
 *  Depending on which backend is chosen, the generated code uses <b>modern C++ features</b> such as reference counted pointers for
 *  clearly documented vertex ownership and safe, automatic deletion; exception safe adjustment to the tree edges (vertex
 *  connectivity); immediate tree consistency checks with violations reported by exceptions; and automatic, read-only parent
 *  pointers. Generated definitions make heavy use of automatically-generated forward declarations to achieve efficient compilation
 *  (although this is currently heavily counteracted by the policy that all ROSE source code includes the huge "sage3basic.h" or
 *  "rose.h" header files).
 *
 *  @section rosebud_language Input Language
 *
 *  The input language is a subset of the C preprocessor and C++. As such, the input easily understood by experienced C++
 *  programmers and can be operated on in almost any integrated development environment or code editor.
 *
 *  The various code-generation backends have slightly different rules for what they support. The documentation below indicates
 *  which backends support which input language features when support is not universal. The "yaml" backend supports everything since
 *  it simply emits the AST as a YAML document.
 *
 *  @subsection rosebud_language_file Input files
 *
 *  Each AST vertex class definition appears in its own header file. The header file name reflects the name of the class, and the
 *  directory path [reflects the namespace containing the class (Sawyer)] [is chosen based on the set of `rosebud` command-line
 *  options that are needed (ROSETTA)]. These files are typically named with a ".h" extension so that IDEs and code editors can
 *  recognize them as C++ without any special configuration. For instance, the [@ref Rose::Sarif::Log class is defined in
 *  "src/AstNodes/Rose/Sarif/Log.h"] [@c SgAsmGenericSection class is defined in "src/AstNodes/BinaryAnalysis" (ROSETTA)].
 *
 *  @li <b>Include-once:</b> The header file should not include any C preprocessor compile-once mechanism, since this will be
 *  generated automatically.
 *
 *  @li <b>Feature tests:</b> [The "featureTests.h" file is automatically included into generated code and the feature test macro is
 *  specified on the `rosebud` command-line. (ROSETTA)] [If a class is to be conditionally compiled, then all necessary preprocessor
 *  directivies must be specified. (Sawyer)]
 *
 *  @li <b>Definition headers:</b> Any headers needed by the class definition should be included. These preprocessor directives will
 *  be copied verbatim to the generated header file. One should strive to include as few headers as possible, and as few definitions
 *  as possible. Most of the time all that's needed are forward declarations.
 *
 *  @li <b>Implementation headers:</b> Any headers needed by the implementation of automatically generated member functions must
 *  be included. The inclusion of these headers [should be surrounded by an `#ifdef ROSE_IMPL` (Sawyer)] [is copied verbatim to
 *  the generated header (ROSETTA)].
 *
 *  @li <b>Conditional compilation:</b> Any conditional compilation directives for the class as a whole must be entirely outside
 *  the class definition and there cannot be more than one class definition when the directives are ignored. This is because Rosebud
 *  parses the class definition as if the preprocessor directives were not present.
 *
 *  @li <b>Namespaces:</b> [If the class should be defined within a namespace then the namespace must be specified using named C++
 *  `namespace` specifiers around the class definition and the choice of directory path should mirror the namespace
 *  hierarchy. (Sawyer)] [All class definitions are assumed to exist within the root namespace. (ROSETTA)]
 *
 *  @li <b>Post-definition code:</b> Any code appearing after the class definition is [ignored (ROSETTA)] [copied verbatim (Sawyer)
 *  to the generated header file.  [ROSETTA doesn't have any mechanism to copy class post-definition text to the generated file, so
 *  any conditional compilation directives that started before the class will have automatically generated code to close them, which
 *  may or may not match the closing directives in the input. (ROSETTA)]
 *
 *  @subsection rosebud_language_class Class definition
 *
 *  A Rosebud class definition is similar to a C++ class definition. The class is introduced by a Doxygen double-asterisk C-style
 *  comment, followed by an optional list of C++ attributes (some of which are special Rosebud attributes), followed by the
 *  word "class", the class name, an optional base class list, an opening curly brace, member specifications, a closing curly brace,
 *  and the terminating semi-colon.
 *
 *  The following Rosebud class attributes are understood:
 *
 *  @li The `Rosebud::abstract` attribute indicates that the class cannot be instantiated. This is useful for base classes where one
 *  wants to be able to only instantiate its derived classes. [The ROSETTA backend only partially honors this: the last argument of
 *  `NEW_NONTERMINAL_MACRO` is set to false, but the generated class can still be instantiated. In fact, adding a pure virtual
 *  function to the class to force it to never be instantiated causes compilation errors in generated code. (ROSETTA)]
 *
 *  @li The `Rosebud::no_constructors` attribute prevents the backend from generating certain kinds of constructors. This is useful
 *  when the user wants to define his own constructors. [Although the usual C++ constructors are generated, no `instance` allocating
 *  constructors are generated. (Sawyer)]
 *
 *  @li The `Rosebud::no_destructor` attribute [prevents the generation of a destructor so the user can define his own destructor
 *  (Sawyer)] [is ignored and a destructor is always generated (ROSETTA)].
 *
 *  @li The `Rosebud::suppress` attribute prevents the backend from generating any code at all for this class. This is useful when
 *  Rosebud needs to know the class definition in order to generate code for subclasses, but should not generate the base class.
 *
 *  @li [The `Rosebud::tag` attribute takes a single argument which is a symbol to use as the ROSETTA-generated type tag when it
 *  needs to be different than the automatically generated tag. The auto-generated take is the type name without the leading "Sg"
 *  and with a "Tag" suffix. For instance, the tag for the class `SgAsmInstruction` would default to `AsmInstructionTag`. (ROSETTA)]
 *
 *  @subsection rosebud_language_properties Property members
 *
 *  A property is a ROSE IR feature where a value is stored in an object and is accessed and modified with special generated
 *  functions. The Rosebud syntax for a property looks like a C++ data member. It consists of the Doxygen documentation comment, the
 *  list of C++ attributes one or more of which must be a Rosebud attribute, the data member type, the data member name, the
 *  optional initialization constexpr, and the terminating semicolon.
 *
 *  @li Properties should be named using nouns or noun phrases; they should not be verbs or verb phrases since little to no
 *  computation occurs when accessing or changing their values. The ROSE policy is that these names be camelCase and Rosebud can be
 *  configured to emit warnings when this policy is violated.
 *
 *  @li The Doxygen documentation should be immediately prior to the property declaration described above.
 *
 *  @li Access specifiers (`public`, `protected`, or `private`), if present, must appear before the Doxygen comment. [They are
 *  ignored. (ROSETTA)] [They influence the generated accessors and mutators, but not the generated data member. (Sawyer)].  In the
 *  generated code, the property is always stored in a [private (ROSETTA)] [hidden (Sawyer)] data member.
 *
 *  @li Conditional compilation, if present, must surround the entire property definition and its Doxygen comment. If the
 *  conditional compilation directives are ignored, then there must still be only one declaration per property. This is because
 *  Rosebud parses the class definition as if the preprocessor directives were absent. [Conditional compilation of properties is
 *  not supported because there is no mechanism to pass this information through ROSETTA. (ROSETTA)]
 *
 *  The only thing that sets a property apart from a normal data member is the presence of at least one Rosebud attribute. The
 *  following Rosebud attributes are understood:
 *
 *  @li The `Rosebud::property` attribute marks what would otherwise appear to be a data member declaration as a property
 *  declaration. Since the presence of any Rosebud attribute is sufficient, the "property" attribute is only necessary when no other
 *  Rosebud attribute is specified.
 *
 *  @li The `Rosebud::data` attribute takes exactly one argument which is a symbol naming the data member that will be declared to
 *  store the property value. [This attribute has no user-visible effect since all property data members are hidden
 *  anyway. (Sawyer)]
 *
 *  @li The `Rosebud::accessors` attribute, which requires an argument list even if it's empty, specifies the symbols to use as the
 *  accessor member function names. The default accessor name is [the same as the property name (Sawyer)] [formed by prepending
 *  "get_" to the property name, resulting in an unusual combination of snake- and camel-case (ROSETTA)]. If the attribute argument
 *  list is empty, then no accessor functions are generated, [making for a rather useless attribute since there's no other way
 *  to access it. (Sawyer)] [but the attribute can still be accessed directly through its data member name (ROSETTA)].
 *
 *  @li The `Rosebud::mutators` attribute, which requires an argument list even if it's empty, specifies the symbols to use as the
 *  mutator member function names. The default accessor name is [the same as the property name (Sawyer)] [formed by prepending
 *  "set_" to the property name, resulting in an unusual combination of snake- and camel-case (ROSETTA)]. If the attribute argument
 *  list is empty, then no mutator functions are generated, but the property might still be modifiable through a non-const accessor
 *  (see `Rosebud::large`).
 *
 *  @li The `Rosebud::ctor_arg` means that the property's value will be passed to a generated constructor. Rosebud generates a C++
 *  constructor that takes arguments (possibly empty) for all the `ctor_arg` properties in the current class and all base
 *  classes. [If the user doesn't want this constructor, use the class `Rosebud::no_constructor` attribute. (Sawyer)] [Rosebud also
 *  generates a default constructor and a constructor that takes arguments only for the `ctor_args` properties for the current class
 *  (no base classes) and the unspecified properties are initialized to their default values (ROSETTA)]. Additional constructors can
 *  be defined by the user in the typical C++ manner.
 *
 *  @li The `Rosebud::cloneptr` attribute [causes the `copy` member function to allocate a new instance of the pointed object using
 *  the compile-time type of that object and its copy constructor] [is not supported (Sawyer)]. This is intended to be a temporary
 *  attribute since it can easily be done directly in C++ in many ways.
 *
 *  @li The `Rosebud::no_serialize` attribute indicates that the property should not participate in serialization or
 *  deserialization. Normally, if one or more serialization backends are used, they will generate code to serialize and deserialize
 *  every property.
 *
 *  @li The `Rosebud::large` attribute indicates that the property value is large and expensive to copy. Therefore, in addition to
 *  any other mutators, the accessors will be overloaded to return a non-const reference to the property's data member. Use this
 *  sparingly because it will not be compatible with all planned Rosebud backends (e.g., it is not possible to generate property
 *  observers or thread safe code if the property value can be modified directly through a reference.
 *
 *  @li The `Rosebud::not_null` attribute [indicates that the property value cannot be a null pointer and any attempt to assign
 *  a null pointer will result in an exception being thrown (Sawyer)] [is ignored (ROSETTA)].
 *
 *  @li The `Rosebud::rosetta` attribute [indicates that the property should be generated in a manner compatible with
 *  ROSETTA-generated properties (ROSETTA)] [is ignored (Sawyer)].
 *
 *  @li The `Rosebud::traverse` attribute [indicates that a property is a pointer to another AST vertex and that the edge formed by
 *  the pointer is part of a tree data structure, i.e., the pointee's parent pointer is assumed to point back to the object that
 *  points to that pointee (ROSETTA)] [is ignored except it produces an error diagnostic if the property's type is not `Edge<T>` or
 *  `EdgeVector<T>` (Sawyer)].
 *
 *  @section rosebud_backends Backend Generators
 *
 *  The `rosebud` tool parses the definitions from the input files (described above) to create an intermediate representation (AST)
 *  and then one or more "backends" traverse the AST to emit C++ source code or other information. The three main backends are:
 *
 *  @li `--backend=yaml` produces a structured YAML document describing the intermediate representation. This output can be easily
 *  parsed by other stand-alone translators. For instance, a translator to generate the alphabetical list of class names is a single
 *  line shell script using the free `yq` tool.
 *
 *  @li `--backend=rosetta` produces input for the ROSETTA translator. Although ROSETTA has a number of problems (detailed below),
 *  it is also not feasible to entirely replace it in one fell swoop. Therefore, our plan is to use this backend to incrementally
 *  rewrite the ROSETTA definitions as Rosebud definitions, and have Rosebud produce the ROSETTA input. To this end, all the binary
 *  analysis AST vertex types (about 400 in total) have been converted. It is proving challenging to translate other ROSETTA classes
 *  due to their heavy use of ROSETTA macros instead of C++ language features, and the impedance mismatch in how ROSETTA and C++
 *  define class hierarchies (bottom up versus top down).
 *
 *  @li `--backend=sawyer` produces C++ source code that uses the more modern @ref Sawyer::Tree::Vertex "Sawyer::Tree" API and is an
 *  experimental backend that provides reference counted AST vertices using smart pointers; automatic, read-only parent pointers and
 *  immediate tree consistency checks; multiple, independent AST class hierarchies; traversals that can use lambda functions;
 *  exception safety; hidden data members; complete documentation; and unit tests. Eventually we hope to support more exotic
 *  features like signals and slots, thread safety, and user-defined class invariants. This backend is used by the @ref Rose::Sarif
 *  API and Rosebud itself.
 *
 *  @subsection rosebud_backend_rosetta Features of the ROSETTA backend
 *
 *  The ROSETTA backend does not use a special type to indicate that a pointer data member is also an edge in the tree. Instead,
 *  the data member is marked with the `Rosebud::traverse` attribute, like this:
 *
 *  @code
 *  class SgParent: public SgNode {
 *  public:
 *      // This property is an edge in the tree
 *      [[using Rosebud: rosetta, traverse]]
 *      SgChild *child = nullptr;
 *
 *      // This property is not a tree edge.
 *      [[Rosebud::rosetta]]
 *      SgChild *other = nullptr;
 *  };
 *  @endcode
 *
 *  The accessor and mutator functions are, by default, named by prepending "get_" and "set_". However, much code in ROSE accesses
 *  the property data members directly by prepending "p_". This is a bad practice since it bypasses any other actions (such as
 *  invariant checking, mutex aquisition, signaling, etc) that might also need to occur when a property is modified.
 *
 *  @code
 *  parent->p_other = node;  // don't do this
 *  parent->set_other(node); // do this instead
 *  @endcode
 *
 *  Each vertex class has an automatically generated `parent` pointer that must be manually adjusted so the child points back to the
 *  parent, like this:
 *
 *  @code
 *  parent->set_child(child);
 *  if (child)
 *      child->set_parent(parent);
 *  @endcode
 *
 *  Detaching a node is similarly difficult:
 *
 *  @code
 *  if (auto child = parent->get_child()) {
 *      parent->set_child(nullptr);
 *      child->set_parent(nullptr);
 *  }
 *  @endcode
 *
 *  Failing to set the child's parent pointer correctly is a detectable runtime error, but the check is delayed until the user
 *  remembers to call the appropriate whole-tree checking function at a later tmie. Similarly for other programming errors such as
 *  creating a cycle or having multiple vertices pointing to the same child--checks are delayed until some point at which the user
 *  remembers to check the tree consistency.
 *
 *  A ROSETTA-generated type should not have edges that point to individual children intermixed with vectors that point to many
 *  children. In order to do that reliably one must create an intermediate type to hold the vector:
 *
 *  @code
 *  class SgArgumentList: public SgNode {
 *  public:
 *      [[Rosebud::traverse]]
 *      std::vector<SgArgument*> arguments;
 *  };
 *
 *  class SgFunctionCall: public SgNode {
 *  public:
 *      [[Rosebud::traverse]]
 *      SgName *name;
 *
 *      [[Rosebud::traverse]]
 *      SgArgumentList *arguments = nullptr;
 *  };
 *
 *  SgFunctionCall* makeBinaryCall(SgName *name, SgArgument *arg1, SgArgument *arg2) {
 *      auto call = new SgFunctionCall;
 *      call->set_name(name);
 *      if (name)
 *          name->set_parent(call);
 *      auto argList = new SgArgumentList;
 *      call->set_arguments(argList);
 *      argList->set_parent(call);
 *      argList->get_arguments().push_back(arg1);
 *      if (arg1)
 *          arg1->set_parent(argList);
 *      argList->get_arguments().push_back(arg2);
 *      if (arg2)
 *          arg2->set_parent(argList);
 *      return call;
 *  }
 *  @endcode
 *
 *  Since ROSETTA uses raw pointers, it is up to the user to know when an object is no longer part of any tree and no longer
 *  pointed to by anything else in ROSE. Sometimes an analysis will cache its results, and thus have pointers into the AST. The
 *  best practice with ROSETTA-generated code is to never free any AST vertex.
 *
 *  @code
 *  if (auto child = parent->get_child()) {
 *      parent->set_child(nullptr);
 *      child->set_parent(nullptr);
 *      SageInterface::deleteAST(child); // UNSAFE -- never do this!
 *  }
 *  @endcode
 *
 *  @subsection rosebud_backend_sawyer Features of the Sawyer backend
 *
 *  The Sawyer backend generates AST vertex classes that inherit from @ref Sawyer::Tree::Vertex, and it uses smart pointers. The
 *  smart pointer types have the names `Ptr` and `ConstPtr` both as members of the vertex class and by appending those names to the
 *  name of the class. The latter are useful in places like header files where one doesn't want the compiler to spend time parsing
 *  the class definition and only incomplete types are needed.  Unlike the ROSETTA backend, the smart pointer data members do not
 *  need to be explicitly initialized to nullptr (their constructors take care of that). An AST edge is indicated with the type
 *  `Edge`, like this:
 *
 *  @code
 *  class SgParent: public SgNode {
 *  public:
 *      // This property is an edge in the tree
 *      [[Rosebud::property]]
 *      Edge<SgChild> child;
 *
 *      // This property is not a tree edge
 *      [[Rosebud::property]]
 *      SgChildPtr other;
 *  @endcode
 *
 *  The accessor and mutator functions are, by default, named the same thing as the property. The data members for the properties
 *  are effectively hidden by being given names that include a random string. Therefore the bad practice of setting the data member
 *  directly is no longer possible--all property modifications must go through the generated mutators.
 *
 *  @code
 *  parent->p_other = node;      // compile-time error
 *  parent->other(node);         // do this instead
 *  parent->forceSetOther(node); // or do this in the defining class if there are no public mutators
 *  @endcode
 *
 *  Each vertex class has an automatically generated `parent` pointer. Unlike ROSETTA, this pointer does not need to be adjusted
 *  explicitly, and trying to do so is a compile-time error.
 *
 *  @code
 *  parent->child(child); // now child->parent() == parent
 *  @endcode
 *
 *  Detaching a node is similarly easy:
 *
 *  @code
 *  parent->child(nullptr); // previous child (if any) now has a null parent pointer
 *  @endcode
 *
 *  Failing to set the parent correctly is not possible like it is with the ROSETTA backend. Also, the Sawyer backend immediately
 *  checks that the tree is not malformed in these ways, which are all exception safe:
 *
 *  @li If the vertex being assigned as a child already has a parent, then an exception is thrown.
 *
 *  @li If the vertex is being assigned as its own child, then an exception is thrown.
 *
 *  @li If the vertex being assigned as a child is also the root of the parent's tree, then an exception is thrown. Because of its
 *  nontrivial cost, this check is enabled only when ROSE is configured for debugging (when the C preprocessor `NDEBUG` symbol is
 *  undefined).
 *
 *  The Sawyer backend supports mixing scalar and vector edge types within a single AST vertex class. Unlike the ROSETTA backend,
 *  there is no need to define an intermediate class to hold the vector.
 *
 *  @code
 *  class SgFunctionCall: public SgNode {
 *  public:
 *      [[Rosebud::property]]
 *      Edge<SgName> name;
 *
 *      [[Rosebud::property]]
 *      EdgeVector<SgArgument> arguments;
 *  };
 *
 *  SgFunctionCall* makeBinaryCall(const SgName::Ptr &name, const SgArgument::Ptr &arg1, const SgArgument::Ptr &arg2) {
 *      auto call = SgFunctionCall::instance();
 *      call->name(name);                      // name->parent() == call
 *      call->arguments().push_back(arg1);     // arg1->parent() == call
 *      call->arguments().push_back(arg2);     // arg2->parent() == call
 *      return call;
 *  }
 *  @endcode
 *
 *  Since Sawyer uses reference counting pointers, objects that are no longer part of a tree and which are no longer pointed to by
 *  anything else in the program are automatically and immediately deleted. The user should not evern explicitly delete AST
 *  vertices. However, the user must be careful not to introduce self-referential data structures since cycles will prevent the
 *  objects from being deleted even if there are no other pointers to the data structure. This backend does not yet support weak
 *  pointers.
 *
 *  @code
 *  parent->child(nullptr); // previous child is deleted if no other references
 *  @endcode
 *
 *  @section rosebud_building Building and running Rosebud
 *
 *  Rosebud is distributed as part of the ROSE library source code under the same license. It is built and installed automatically
 *  as part of building and installing the ROSE library.
 *
 *  The ROSE library has a number of AST class hierarchies that are generated by Rosebud. Some of these are also passed through the
 *  ROSETTA `CxxGrammarMetaProgram` tool using Rosebud's `--backend=rosetta` switch, and others are translated to C++ directly using
 *  Rosebud's `--backend=sawyer` switch.  Other switches are also necessary in certain situations. The `generate` script makes sure
 *  the correct `rosebud` commands are run to produce all generated files. The generated files are currently checked into the ROSE
 *  library source repository for improved stability while Rosebud is being developed.
 *
 *  To regenerate files, go to the "src/Rosebud" directory in the build tree and build the `rosebud` executable by running your
 *  preferred build system. Then generate files by running this command from that same directory, replacing "$ROSE_SOURCE" with
 *  the top-level directory of the ROSE library repository:
 *
 *  @code
 *  $ $ROSE_SOURCE/src/Rosebud/generate ./rosebud
 *  @endcode
 *
 *  You may need to add the `--source=$ROSE_SOURCE` switch before "./rosebud" if the script cannot determine the locaton of the
 *  ROSE source directory.
 *
 *  The following directories are used:
 *
 *  @li $ROSE_SOURCE/src/Rosebud contains the source code for the `rosebud` command, its backends, and this documentation.
 *
 *  @li $ROSE_SOURCE/src/AstNodes contains the input files for the `rosebud` command. These files are organized into directories
 *  according to either the namespace (for the Sawyer backend) or "Sg" prefix (for the ROSETTA backend).
 *
 *  @li $ROSE_SOURCE/src/generated contains the files generated by `rosebud`. The ROSETTA backend also modifies the
 *  `CxxGrammarMetaProgram` source code in the $ROSE_SOURCE/src/ROSETTA/src directory, and thus ROSETTA needs to be recompiled and
 *  re-run after any changes are made to Rosebud inputs.
 *
 *  @section rosebud_vs_rosetta Comparison with ROSETTA
 *
 *  ROSETTA is/was the previous AST vertex class definition mechanism and is still used for most parts of the main AST. It has
 *  served well for many years, but its main drawbacks are:
 *
 *  @li A complicated, undocumented input language (sequence of API calls) that makes a class definition look nothing like a C++
 *  class and splits parts of a definition into many parts stored in different files (class name list, ROSETTA API calls,
 *  documentation files, member declarations, member implementations, and member special types). As a result, few members of the
 *  ROSE team are comfortable making non-trivial modifications to the AST types. Furthermore, since ROSETTA defines a class
 *  hierarchy from bottom up, it's extremely difficult to incrementally replace ROSETTA with a more modern system that follows C++'s
 *  top-down order.
 *
 *  @li A cumbersome, slow method of generating code that takes many seconds. It produces C++ headers and source files measuring in
 *  the hundreds of thousands of lines and contributing multiplicatively to the lengthy ROSE library build times even though efforts
 *  have been made to improve the situation. Editing these files with IDEs and viewing them in debuggers can be agonizingly slow.
 *  The generated code uses unconventonal indentation without much thought to human readability, and the presence of ROSETTA macros
 *  has resulted in a high degree of duplication in the generated code.  Diagnostics from the ROSETTA generator (the
 *  `CxxGrammarMetaProgram` command) seldom indicate the location of the problem, are often sent to standard output instead of
 *  standard error, and are typically ungracefully handled by a call `assert(0)`, an infinite loop, or a segmentation fault. The
 *  generated accessors, mutators, and data members are named with unconventional combination of snake- and camel-case.
 *
 *  @li A convoluted mechanism for documentation that requires the documentation to be in a separate file from the class definition
 *  and to be repeated three times (once per data member, accessor, and mutator) with each copy different only in the name of the
 *  cross reference to the entity being documented. It's not surprising that most AST classes and their member functions are
 *  undocumented.
 *
 *  @li Inability to support often used and much desired C++ mechanisms in generated code, such as abstract classes with pure
 *  virtual member functions, clear vertex ownership, safe subtree deletion, consistent parent/child pointers, traversals with
 *  lambdas, and avoidance of dynamic down-casting. */
namespace Rosebud {

/** A token parsed from the input file. */
using Token = Sawyer::Language::Clexer::Token;

/** A stream of tokens from the input file. */
using TokenStream = Sawyer::Language::Clexer::TokenStream;

/** How to obtain text when converting a sequence of tokens to a string. */
enum class Expand {
    NONE,                                               /**< Each token's [begin,end) individually. */
    INTER,                                              /**< From first token's begin to last token's end. */
    PRIOR                                               /**< From first token's prior to last token's end. */
};

/** When something should be done. */
enum class When {
    NEVER,                                              /**< Never do it. */
    ALWAYS,                                             /**< Always do it. */
    AUTO                                                /**< Sometimes do it. */
};

class Generator;
using GeneratorPtr = std::shared_ptr<Generator>;        /**< Shared-ownership pointer to a @ref Generator. */
class Serializer;
using SerializerPtr = std::shared_ptr<Serializer>;      /**< Shared-ownership pointer to a @ref Serializer. */

// AST node types
namespace Ast {
class Node;
using NodePtr = std::shared_ptr<Node>;                  /**< Shared-ownership pointer to a @ref Node. */
class TokenList;
using TokenListPtr = std::shared_ptr<TokenList>;        /**< Shared-ownership pointer to a @ref TokenList. */
class ArgumentList;
using ArgumentListPtr = std::shared_ptr<ArgumentList>;  /**< Shared-ownership pointer to a @ref ArgumentList. */
class CppStack;
using CppStackPtr = std::shared_ptr<CppStack>;          /**< Shared-ownership pointer to a @ref CppStack. */
class Attribute;
using AttributePtr = std::shared_ptr<Attribute>;        /**< Shared-ownership pointer to a @ref Attribute. */
class Definition;
using DefinitionPtr = std::shared_ptr<Definition>;      /**< Shared-ownership pointer to a @ref Definition. */
class Property;
using PropertyPtr = std::shared_ptr<Property>;          /**< Shared-ownership pointer to a @ref Property. */
class Class;
using ClassPtr = std::shared_ptr<Class>;                /**< Shared-ownership pointer to a @ref Class. */
class File;
using FilePtr = std::shared_ptr<File>;                  /**< Shared-ownership pointer to a @ref File. */
class Project;
using ProjectPtr = std::shared_ptr<Project>;            /**< Shared-ownership pointer to a @ref Project. */
} // namespace

} // namespace
#endif
