/* Example classProblem.C from Andreas */

/*
   The problem occurs in sage_gen_be.C when the variable 'result' is returned as 
   NULL from 
   sage_gen_dynamic_init ( a_dynamic_init_ptr dip,
                        a_type_ptr         init_entity_type,
                        a_boolean          parenthesized_init,
                        a_boolean          force_parens)
   It is in the switch-statement case dik_nonconstant_aggregate that the 
   'result' is not set when you have a default constructor (line 15101).

   Extra info:
       parameter given to sage_gen_dynamic_init is
                  paranthesized_init = true
                  force_parens = false

*/

//If TestType has a default constructor it fails.
class TestType 
{
  int la;
public:
  TestType(){};
  ~TestType(){};
};

int main()
{
TestType* ptr = new TestType [20];

return 1;
}

