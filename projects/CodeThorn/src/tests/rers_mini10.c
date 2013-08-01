// this is a reduced example to pinpoint a bug in the ltl solver

#include <stdio.h>
    int a17 = 1;
    int a7 = 0;
    int a20 = 1;
    int a12 = 8;
    int a16 = 5;
    int a21 = 1;
    int calculate_output(int input) {
        if((((((((a21==1)&&(((a16==5)||(a16==6))&&(input==1)))))&&!(a7==1)))&&(a12==8))){
            return 26;
        } else if((((((!(a17==1)&&(((input==4)))))&&(a20==1)))&&(a16==4))){
        } else if(((((a12==8)&&((((a17==1)&&((input==3)&&((a16==5)))))))))){
            a17 = 0;
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
            printf("%d\n", output);
    }
}
