void foo()
{
    int a, b, i, j;
    (a, b)= (i=0, j=0);
    for(i=0, j=0; i < 10, j < 10; i=j+1, j=i+1) { 
    }
}
