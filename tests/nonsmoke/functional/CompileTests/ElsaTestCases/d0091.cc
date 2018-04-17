//  mysql-3.23.54a-11/sql_lex-D6bf.i

// member operators are allowed to be static

typedef unsigned int size_t;
struct String {
  static void *operator new(size_t size) throw() { return 0; }
};
