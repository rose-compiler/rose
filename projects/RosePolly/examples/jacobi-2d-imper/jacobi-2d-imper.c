#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <assert.h>

#include "decls.h"

#include "util.h"

extern int bar();

int main()
{
    int t, i, j;

	int TT = bar();
	int NN = bar();

    assert(N >= 32);
    assert(T >= 32);

    init_array();

#pragma rosePolly
{
    for (int t=0; t<TT; t++) {
        for (int i=2; i<NN-1; i++) {
            for (int j=2; j<NN-1; j++) {
                b[i][j]= 0.2*(a[i][j]+a[i][j-1]+a[i][1+j]+a[1+i][j]+a[i-1][j]);
            }
        }
        for (int i=2; i<NN-1; i++) {
            for (int j=2; j<NN-1; j++)   {
                a[i][j]=b[i][j];
            }
        }
    }
}

#ifdef TEST
    print_array();
#endif
    return 0;
}
