/* Test code reporting bugs for C_only sent by Chadd Williams (2/12/2004) */

/****START CODE****/
void foo(){
        enum {X, Y} val = X;
}
/****END CODE*****/


/****START CODE****/
void oof(){
        int x;
        struct str { int y;}; /*declaring a struct here seems to be a problem*/
}
/****END CODE*****/


/****START CODE****/
void ofo(){

        int x = 9;

        if(x){
        }else
                ; /*this ; is the problem*/
}
/****END CODE*****/


/****START CODE****/
void bar(){
        while(1); /* putting { } around the ; fixes this*/
}
/****END CODE*****/


/****START CODE****/
void rab(){
        int x;
#if 1
        union { int x; float y;} un; /*similar to the struct above*/
#endif
}
/****END CODE*****/

#if 1
/****START CODE****/
#include <stdarg.h>
struct vastrs {
    va_list args;
};

void ap_scan_script_header_err_strs( int termarg, ...)
{
        struct vastrs strs;

        va_start(strs.args, termarg);/*odd, if no struct is involved, just
                                        a va_list this works fine*/
}
/****END CODE*****/
#endif

#if 1
/****START CODE****/
int foo2 ( register long r )
   {  /* the register keyword*/
   }
/****END CODE*****/
#endif

