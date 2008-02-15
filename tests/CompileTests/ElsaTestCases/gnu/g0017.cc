// g0017.cc
// example of a common header problem that gcc allows

// excerpted from /opt/qt-2.3.2/src/moc/mocgen.cpp (after preprocessing)

extern "C" {
# 574 "/usr/include/unistd.h" 3
extern int isatty (int __fd) throw () ;
}
extern int isatty  ( int )  ;
