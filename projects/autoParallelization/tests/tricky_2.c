//file:///nfs/apps/intel/cc/9.1.045/doc/main_cls/index.htm
void add (int k, float *a, float *b)
{  for (int i = 1; i < 10000; i++)
    a[i] = a[i+k] + b[i];
}

/*
 * Intel compiler reports
  add serial loop: line 2
       anti data dependence assumed from line 2 to line 2, due to "a"
       flow data dependence assumed from line 2 to line 2, due to "a"
       flow data dependence assumed from line 2 to line 2, due to "a"
does not know the value of k, the compiler assumes the iterations depend on each other, for example if k equals -1, even if the actual case is otherwise

 */ 



