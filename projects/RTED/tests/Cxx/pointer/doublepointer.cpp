#include <stdlib.h>

int main() {
    short **arrayA;
    arrayA=new short*[123];

    for (int i=0; i < 123; ++i) 
        arrayA[i]=new short[100];

    for ( int i=0; i < 123; i++) 
        for ( int j=0; j < 100; j++) 
            arrayA[i][j]=rand();

    //should fail here: line 16
    short value=arrayA[172][99];

    for (int i=0; i < 123; ++i) 
        delete[] arrayA[i];
    delete[] arrayA;
    return 0;
}
