void foo() {
#if 1
"x";
#endif
#if 1
"'";
#if 1
"\"";
#endif
"\n";
#if 1
"\0";
#endif
"\00";
#if 1
"\000";
#endif
"\\";
#if 1
"ab";
#endif
#endif
}
