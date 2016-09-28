#define	FOO(x)	= x;
#define	EXTERN	
#define	INIT(var,val)	var = val
typedef struct __unnamed_class___F0_L5_C9__variable_declaration__variable_type_c_variable_name_type__DELIMITER___variable_declaration__variable_type__variable_name_v {
char type;
union __unnamed_class___F0_L7_C11_L0R_variable_declaration__variable_type___Pb__c__Pe___variable_name_L0R__scope__str__DELIMITER__L0R_variable_declaration__variable_type_i_variable_name_L0R__scope__val {
char *str;
int val;}v;}Value;
extern Value ValStack[];
extern int ValStackPtr;
static int Nargs;
#define	ARG(x)	(ValStack[ValStackPtr - Nargs + (x)])

int main()
{
// char* x  FOO("foobar");
//  int   x2  FOO(1);
  char *EndSentIg = "\"\')]}>";
  int EndSentIg1 = 1;
  ValStack[(ValStackPtr - Nargs) + 0];
  return 0;
}

