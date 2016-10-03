// cc.in7
// problem with "x;" .. ?

// this is actually a valid ambiguity.. in syntax like
//   class Foo {
//     ...
//   };
// the declarator is missing, and it thinks of "x;" the 
// same way potentially..

int const c;

// With the 'typedef', the code is illegal; gcc says
// "declaration does not declare anything".  It's messing
// up the idempotency test so I'm just going to fix it.
/*typedef*/ int x;

int main()
{
  x;        // is this illegal?
}

int strcmp(char const *s1, char const *s2);
