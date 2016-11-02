#define FOO(x) = x;
#define EXTERN
#define INIT(var, val) var = val

typedef struct {
      char type;
          union {
                    char *str;
                            int val;
                                } v;
} Value;


extern Value ValStack[];
extern int ValStackPtr;
static int Nargs;
#define ARG(x) (ValStack[ValStackPtr - Nargs + (x)])

int main()
{
 // char* x  FOO("foobar");
//  int   x2  FOO(1);
EXTERN INIT(   char      *EndSentIg, "\"')]}>");
EXTERN INIT(   int      EndSentIg1, 1);
ARG(0);
};
