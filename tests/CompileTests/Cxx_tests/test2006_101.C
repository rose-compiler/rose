/*
test input

By C. Liao


----------------------------------------
bash-2.05b$ addFunctionDeclaration inputTest.c
rose_inputTest.c:18: error: parse error before `;' token
rose_inputTest.c:19: error: parse error before `;' token

*/

#if 0
   Example output

int my_function(int &var_name)
{
++var_name;
}

/*
test input
By C. Liao
*/

struct X
{
float b[10];
}

;
struct X (*_pp_A;    //broken here!!
struct X (*_pp_D;   // broken here!!
struct X *_pp_C;
int (*_pp_B)[10][5];

int main()
{
return 0;
}

#endif



typedef int typedefType;
typedefType (*arrayPointer)[5];


struct X
{
float b[10];
};

#if __cplusplus
typedef X typedefTypeX;
#else
typedef struct X typedefTypeX;
#endif
typedefTypeX (*arrayPointerX)[5];
typedefTypeX arrayX[5];

#if 1
struct X (*_pp_A)[10][5];
struct X (*_pp_D)[10];
struct X *_pp_C;
int (*_pp_B)[10][5];

#endif
