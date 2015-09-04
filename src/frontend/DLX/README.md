
Directive-based Languages eXtensions (DLX)
==========================================

This module provides abstraction to describe a *Directive-based Languages eXtensions* (DLX).
We call DLX, a subclass of language that can be defined for directive.
OpenMP, OpenACC, HMPP, and many other language using directives 
In a DLX, directive are of the form: `language construct clause_1(param, ...) ... clause_n(...)`.
Where:
 * `language` is a label representing the name of the language ("omp", "acc", "hmpp")
 * `construct` is a label and it represents a function of the language
 * `clause_X` are labels too, they are arguments to the construct
 * `param` can be anything... particularly it can a piece of c or c++ code

For more information on how to use the DLX module, you can look at the exemples at [DLX/MFB/MDCG Demos](../../../projects/demos-dlx-mfb-mdcg).
In this mini-project we demonstrate how the three modules [DLX](.), [MFB](../../midend/MFB), and [MDCG](../../midend/MDCG) can be used together to build *DLXs*.

## How to use DLX?

We will show how to build a new language using the DLX module.

### Demo language

Our fake language is called `mylang`.
It has three constructs:

| name | associated AST nodes  |
|------|-----------------------|
| `f1` | function declarations |
| `f2` |  class declarations   |
| `f3` | executable statements |

Each construct accept some of these five clauses:

| name |        parameters        | accepting construct |
|------|--------------------------|---------------------|
| `p1` |                          | `f1` `f2` `f3`      |
| `p2` | label: 'a', 'b', or 'c'  | `f1` `f3`           |
| `p3` | list of constant integer | `f1`                |
| `p4` | list of expressions      | `f2`                |
| `p5` | list of data section     | `f3`                |

```c

#pragma f1 p2(c)
void bar();

#pragma f1 p1 p2(b) p3(4,2,8)
void foo() {
  int n;
  int * a;

  #pragma f3 p5(a[0:n])
  bar();

  #pragma f2 p1 p2(c) p5()
  struct B {
    // ...
  };
  struct B * b;

  #pragma f3 p1 p2(a) p5(b[0:n])
  {
    // ...
  }
}
```

### Implementation

#### Language Description

The DLX module is made of template classes that are specialized for a given language.
The first step of creating a new language is too create a language description.
Such description come in the form of a structure.

We encourage DLX users to call it `struct language_t` and to place it in a namespace (named after your language) nested in the DLX namespace.

```c++
namespace DLX {
namespace MyLang {

struct language_t {
  typedef std::map<construct_kinds_e, std::string> construct_label_map_t;
  typedef std::vector<std::string> label_set_t;
  typedef std::map<clause_kinds_e, label_set_t> clause_labels_map_t;
  typedef std::map<directives_relation_e, std::string> directives_relation_label_map_t;

  /// Name of the laguage
  static std::string language_label;

  /// Enumeration of the construct of the language.
  enum construct_kinds_e {
    e_mylang_construct_f1,
    e_mylang_construct_f2,
    e_mylang_construct_f3,
    e_construct_last
  };

  /// Associates a label to each construct kind
  static construct_label_map_t s_construct_labels;
  /// To handle blank construct
  static construct_kinds_e s_blank_construct;

  /// Enumeration of the clauses of the language.
  enum clause_kinds_e {
    e_mylang_clause_p1,
    e_mylang_clause_p2,
    e_mylang_clause_p3,
    e_mylang_clause_p4,
    e_mylang_clause_p5,
    e_clause_last
  };
  /// Associates a set of labels to each clause kind
  static clause_labels_map_t s_clause_labels;

  /// Kinds of relations that can exist between directives
  enum directives_relation_e {
    e_relation_last
  };
  /// Used to output the directive graph (GraphViz format)
  static directives_relation_label_map_t s_directives_relation_labels;

  /// Initialize the language object (fills language_label, s_construct_labels, s_blank_construct, s_clause_labels, s_directives_relation_labels)
  static void init();
};

}
}
```

For each construct, we provide a specialization of:
```c++
template <enum DLX::MyLang::language_t::construct_kinds_e construct_kind>
DLX::Directives::generic_construct_t<DLX::MyLang::language_t>::assoc_nodes_t<construct_kind>;
```
These specializations are used to store the AST nodes associated with the constructs.

We encourage the user to store the original pragma declaration and parent scope as they can be useful later.

```c++
namespace DLX {
namespace Directives {

template <>
template <>
struct generic_construct_t<MyLang::language_t>::assoc_nodes_t<MyLang::language_t::e_mylang_construct_f1> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
  SgFunctionDeclaration * function_declaration;
};

template <>
template <>
struct generic_construct_t<MyLang::language_t>::assoc_nodes_t<MyLang::language_t::e_mylang_construct_f2> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
  SgClassDeclaration * class_declaration;
};

template <>
template <>
struct generic_construct_t<MyLang::language_t>::assoc_nodes_t<MyLang::language_t::e_mylang_construct_f3> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
  SgStatement * executable_statement;
};

}
}
```

For each clause, we need to provide a specialization of:
```c++
template <enum DLX::MyLang::language_t::clause_kinds_e clause_kind>
DLX::Directives::generic_clause_t<DLX::MyLang::language_t>::assoc_nodes_t<clause_kind>;
```
These specializations are used to store the parameters passed to the clauses.

> Data sections are provided by DLX::Frontend. They permit to describe a section of an array like `a[3:5:2]` which represents the array `{a[3], a[5], a[7], a[9], a[11], a[13]}`.
> Data sections accept multidimensionnal array.
> General form of data sections: `arr[b0:l0:s0][b1:l1:s1]...` where `bi`, `li`, and `si` are respectively the base, length and stride for the ith dimension.

```c++
namespace DLX {
namespace Directives {

template <>
template <>
struct generic_clause_t<MyLang::language_t>::parameters_t<MyLang::language_t::e_mylang_clause_p1> {
  // no parameter
};

template <>
template <>
struct generic_clause_t<MyLang::language_t>::parameters_t<MyLang::language_t::e_mylang_clause_p2> {
   enum label_e {
    e_label_a, e_label_b, e_label_c
  } label;
};

template <>
template <>
struct generic_clause_t<MyLang::language_t>::parameters_t<MyLang::language_t::e_mylang_clause_p3> {
  std::vector<int> int_list;
};

template <>
template <>
struct generic_clause_t<MyLang::language_t>::parameters_t<MyLang::language_t::e_mylang_clause_p4> {
  std::vector<SgExpression *> expr_list;
};

template <>
template <>
struct generic_clause_t<MyLang::language_t>::parameters_t<MyLang::language_t::e_mylang_clause_p5> {
  std::vector<Frontend::data_sections_t> data_section_list;
};

}
}
```

We are done with the description of the language.

#### Parsing the Clauses' Parameters

For each clause, we need to provide a specialization of:
```c++
template <>
template <typename language_t::clause_kinds_e kind>
static bool Frontend<language_t>::parseClauseParameters(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<language_t, kind> * clause
);
```
This method of the Frontend object is used to parse each clause parameters.
DLX provides a [Parser](include/DLX/Core/parser.hpp) class. It provides usefull primitive for parsing.
Parsing of piece of c code is possible using ROSE's AstFromString library.
```c++
template <>
template <>
bool Frontend<MyLang::language_t>::parseClauseParameters<MyLang::language_t::e_mylang_clause_p1>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<MyLang::language_t, MyLang::language_t::e_mylang_clause_p1> * clause
) {
  return true;
}

template <>
template <>
bool Frontend<MyLang::language_t>::parseClauseParameters<MyLang::language_t::e_mylang_clause_p2>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<MyLang::language_t, MyLang::language_t::e_mylang_clause_p2> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.consume('('));
  parser.skip_whitespace();
  if (parser.consume("a"))
    clause->parameters.position = Directives::generic_clause_t<MyLang::language_t>::parameters_t<MyLang::language_t::e_mylang_clause_>::e_label_a;
  else if (parser.consume("b"))
    clause->parameters.position = Directives::generic_clause_t<MyLang::language_t>::parameters_t<MyLang::language_t::e_mylang_clause_>::e_label_b;
  else if (parser.consume("c"))
    clause->parameters.position = Directives::generic_clause_t<MyLang::language_t>::parameters_t<MyLang::language_t::e_mylang_clause_>::e_label_c;
  else assert(false);
  parser.skip_whitespace();
  assert(parser.consume(')'));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<MyLang::language_t>::parseClauseParameters<MyLang::language_t::e_mylang_clause_p3>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<MyLang::language_t, MyLang::language_t::e_mylang_clause_p3> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.parse_list<int>(clause->parameters.int_list, '(', ')', ','));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<MyLang::language_t>::parseClauseParameters<MyLang::language_t::e_mylang_clause_p4>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<MyLang::language_t, MyLang::language_t::e_mylang_clause_p4> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.parse_list<SgExpression *>(clause->parameters.expr_list, '(', ')', ','));

  directive_str = parser.getDirectiveString();
  return true;
}

template <>
template <>
bool Frontend<MyLang::language_t>::parseClauseParameters<MyLang::language_t::e_mylang_clause_p5>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  Directives::clause_t<MyLang::language_t, MyLang::language_t::e_mylang_clause_p5> * clause
) {
  DLX::Frontend::Parser parser(directive_str, directive_node);

  assert(parser.parse_list<Frontend::data_sections_t>(clause->parameters.data_section_list, '(', ')', ','));

  directive_str = parser.getDirectiveString();
  return true;
}

```

#### Getting associated AST nodes 

For each construct, we need to provide a specialization of:
```c++
template <>
template <typename language_t::construct_kinds_e kind>
static bool Frontend<language_t>::findAssociatedNodes(
  SgLocatedNode * directive_node,
  Directives::construct_t<language_t, kind> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
);
```
This method of the Frontend object is used to collect AST nodes associated to each directive (depends on the type of construct).
```c++
template <>
template <>
bool Frontend<MyLang::language_t>::findAssociatedNodes<MyLang::language_t::e_mylang_construct_f1>(
  SgLocatedNode * directive_node,
  Directives::construct_t<MyLang::language_t, MyLang::language_t::e_mylang_construct_f1> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.function_declaration = isSgFunctionDeclaration(SageInterface::getNextStatement(construct->assoc_nodes.pragma_decl));
  assert(construct->assoc_nodes.function_declaration != NULL);

  return true;
}

template <>
template <>
bool Frontend<MyLang::language_t>::findAssociatedNodes<MyLang::language_t::e_mylang_construct_f2>(
  SgLocatedNode * directive_node,
  Directives::construct_t<MyLang::language_t, MyLang::language_t::e_mylang_construct_f2> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.class_declaration = isSgClassDeclaration(SageInterface::getNextStatement(construct->assoc_nodes.pragma_decl));
  assert(construct->assoc_nodes.class_declaration != NULL);

  return true;
}

template <>
template <>
bool Frontend<MyLang::language_t>::findAssociatedNodes<MyLang::language_t::e_mylang_construct_f3>(
  SgLocatedNode * directive_node,
  Directives::construct_t<MyLang::language_t, MyLang::language_t::e_mylang_construct_f3> * construct,
  const std::map<SgLocatedNode *, directive_t *> & translation_map
) {
  construct->assoc_nodes.pragma_decl = isSgPragmaDeclaration(directive_node);
  assert(construct->assoc_nodes.pragma_decl != NULL);
  construct->assoc_nodes.parent_scope = isSgScopeStatement(construct->assoc_nodes.pragma_decl->get_parent());
  assert(construct->assoc_nodes.parent_scope != NULL);
  construct->assoc_nodes.executable_statement = SageInterface::getNextStatement(construct->assoc_nodes.pragma_decl);
  assert(construct->assoc_nodes.executable_statement != NULL);
  assert(
    isSgBasicBlock(construct->assoc_nodes.parent_scope)      ||
    isSgIfStmt(construct->assoc_nodes.parent_scope)          ||
    isSgForStatement(construct->assoc_nodes.parent_scope)    ||
    isSgWhileStmt(construct->assoc_nodes.parent_scope)       ||
    isSgDoWhileStmt(construct->assoc_nodes.parent_scope)     ||
    isSgSwitchStatement(construct->assoc_nodes.parent_scope)
  );

  return true;
}
```

#### Boiler Plate part

The design of DLX makes construct and clause objects templates of enumerations values (see [constructs](include/DLX/Core/constructs.hpp) and [clauses](include/DLX/Core/clauses.hpp)).
C++ cannot resolve template with value arguments if they are not statically known, consider:
```c++
enum X { X_a, X_b };

template <enum X v>
void foo();

void bar(enum X v) {
  foo<v>();
}
```
It is not valid as the compiler cannot know at compile time which version of `foo` it should call.
The function `bar` need to be written as follow:
```c++
void bar(enum X v) {
  switch (v) {
    case X_a:
      foo<X_a>();
      break;
    case X_b:
      foo<X_b>();
      break;
  }
}
```

To implement a language with DLX, we need to provide specialization of four disambiguation functions.
```c++
namespace DLX {
namespace Directives {

/// Allocate the corresponding construct specialization using `new`
template <class language_tpl>
generic_construct_t<language_tpl> * buildConstruct(
  typename language_tpl::construct_kinds_e kind
);

/// Calls the corresponding specialization of template <enum DLX::MyLang::language_t::construct_kinds_e construct_kind> Frontend::Frontend<MyLang::language_t>::findAssociatedNodes(...)
template <class language_tpl>
bool findAssociatedNodes(
  SgLocatedNode * directive_node,
  generic_construct_t<language_tpl> * construct,
  const std::map<SgLocatedNode *, Directives::directive_t<language_tpl> *> & translation_map
);

/// Allocate the corresponding clause specialization using `new`
template <class language_tpl>
generic_clause_t<language_tpl> * buildClause(typename language_tpl::clause_kinds_e kind);

/// Calls the corresponding specialization of template <enum DLX::MyLang::language_t::clause_kinds_e clause_kind> Frontend::Frontend<MyLang::language_t>::parseClauseParameters(...)
template <class language_tpl>
bool parseClauseParameters(std::string & directive_str, SgLocatedNode * directive_node, generic_clause_t<language_tpl> * clause);

}
}
```

See [DLX's tests directory](../../../tests/frontend/DLX) for examples. Basically, it is just four switch statement.

## TODO

### Checks

DLX's Frontend is missing a framework to make semantic check on the directives and associated code.

### Error Handling

Currently incorrect or unsupported codes result in failures, normally with assertion or exit.
Correct error handling with, at least, meaningful output are neccessary.

### Logs

We need to add more outputs. Verbosity need to be controllable.

