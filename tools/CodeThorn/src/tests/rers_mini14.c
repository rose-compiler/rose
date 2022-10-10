#include <stdio.h> 
#include <assert.h>

    int a17 = 1;
    int a7 = 0;
    int a20 = 1;
    int a8 = 15;
    int a12 = 8;
    int a16 = 5;
    int a21 = 1;
    int calculate_output(int input) {
        if((((a8==15)&&(((((a21==1)&&(((a16==5)||(a16==6))&&(input==1)))&&(a20==1))&&(a17==1))&&!(a7==1)))&&(a12==8))){
            a20 = 0;
            return 24;
            a20 = 1;
            return -1;
        } 
        if((((((((a17==1)&&(a7==1))&&!(a20==1))&&(a8==15))&&(a12==8))&&(a16==5))&&(a21==1))){
        } 
        if(((((((!(a17==1)&&!(a7==1))&&(a20==1))&&(a8==13))&&(a12==8))&&(a16==5))&&(a21==1))){
        } 
        return -2; 
    }
int main()
{
    int output = -1;
    while(1)
    {
        int input;
        scanf("%d", &input);        
        output = calculate_output(input);
        if(output == -2)
            fprintf(stderr, "Invalid input: %d\n", input);
            printf("%d\n", output);
    }
}
