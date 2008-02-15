// c0002.c
// enum coverage

enum Blah;
enum Blah;
enum Blah {x};
//ERROR(1): enum Blah {y};
enum Blah;
