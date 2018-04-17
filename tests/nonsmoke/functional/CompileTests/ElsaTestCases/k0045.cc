// 'operator delete' redeclared without throw()

// originally found in package drscheme_1:208-1

// a.ii:3:6: error: prior declaration of `operator delete' at <init>:1:1 had
// type `void ()(void *p) throw()', but this one uses `void ()(void
// */*anon*/)'

// ERR-MATCH: prior declaration of `operator delete'
  
// this would make both gcc and icc reject
//ERROR(1): void operator delete(void *) throw();

void operator delete(void *) {
}


// something else
extern int x;
//ERROR(2): typedef int x;

typedef int y;
//ERROR(3): extern int y;


// EOF
