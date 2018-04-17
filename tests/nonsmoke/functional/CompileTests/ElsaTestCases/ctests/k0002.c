// error: attempt to create an object of incomplete class `S'

// originally found in package diff

// ERR-MATCH: create incomplete class

// ok at global scope

struct S s;

// not ok if static (well, gcc allows it, so whatever)
//static struct S s3;

void foo()
{
  // not ok in local scope
  //ERROR(1): struct S s2;
}

struct S {};
