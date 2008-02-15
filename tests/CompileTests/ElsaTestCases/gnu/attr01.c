// attr01.c
// some elementary properties of attribute specifiers, that
// is, just the __attribute__((...)) thing itself

// http://gcc.gnu.org/onlinedocs/gcc-3.1/gcc/Attribute-Syntax.html

// very simple usage
int x __attribute__ ((    aligned (16)    ));

// now, variations on that designed to test the syntax of
// what goes inside the (( and ))

// attribute sequence may be empty
int x __attribute__ ((    /*nothing*/    ));

// attributes themselves may be empty
int x __attribute__ ((    /*nothing*/, /*nothing*/    ));

// a word, such as an identifier or a reserved word
int x __attribute__ ((    unused, const    ));

// a word followed by "parameters", which themselves might be

  // an identifier
  int x __attribute__ ((    func(identifer)    ));

  // an identifier followed by a comma and a non-empty comma-separated
  // list of expressions
  int x __attribute__ ((    func(identifer, 1)    ));
  int x __attribute__ ((    func(identifer, 1, 2)    ));
  int x __attribute__ ((    func(identifer, 1, 2, 3+4)    ));

  // a possibly empty comma-separated list of expressions, such as
  // integer or string literals
  int x __attribute__ ((    func()    ));
  int x __attribute__ ((    func(1)    ));
  int x __attribute__ ((    func(1, 2)    ));
  int x __attribute__ ((    func(1, 2, 3+4)    ));
  int x __attribute__ ((    func(1, 2, 3+4, "five")    ));

// an attribute specifier list is a sequence of one or more
// __attribute__ specifiers; every occurrence of __attribute__ appears
// to be as part of an attribute specifier list, so it should be legal
// to put lots of these guys together
int x __attribute__ ((a_word));
int x __attribute__ ((a_word)) __attribute__ ((another_word));
int x __attribute__ ((a_word)) __attribute__ ((another_word)) __attribute((yet_another_word));;


// EOF
