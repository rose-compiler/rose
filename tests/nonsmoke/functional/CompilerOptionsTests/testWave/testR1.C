#define FOO int x;
#define FOO2 int y
#define FOO3 int z  

typedef struct epp_s
{
  int ELOW;
}EPP;

#define ELOW (epp->ELOW)

int foo(EPP* epp)
{
  ELOW=0;
}
#define FII 2
int main(){
  FOO;
  FOO2;
  FOO3;
  x=FII;

};
