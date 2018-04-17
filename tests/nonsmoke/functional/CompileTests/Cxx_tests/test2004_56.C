/*
In test programs such as the following:

calling get_qualified_name() on the reference to type b (a SgClassType)
returns "a" rather than "a::b".  Using get_name() correctly returns "b"
for this example.  

Thank you,
Jeremiah Willcock
*/

struct a {
  struct b {};
  b* x;
};

