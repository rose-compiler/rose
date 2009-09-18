
int main(void)
{
    int a = 0, b = 0;
    struct aab {
        char * name[5];
        int *ptrA;
    } ss;
   
    *ss.ptrA = a + b;  /* the pointer ss.ptrA is uninitialized */

    return 0; 
}
