Demos of DLX and MDCG
=====================

This project contains demos of the [Directive-based Language eXtension (DLX)](../../src/frontend/DLX)
and [Model Driven Code Generation (MDCG)](../../src/midend/MDCG) modules.
These modules are used to build small DLXs.

The DLX module is used to create the Frontend for the language.
MDCG is used to model the target runtime.
By runtime, we mean the collection of data-structures and functions that are used either in transformed code or generated code.
MDCG::StaticInitializer, the first of MDCG's tools, is also demonstrated.

Underlying the working of MDCG, the [Multiple File Builder (MFB)](../../src/midend/MFB) manages files and symbols accross files.

## Toy DLXs

These DLXs do not solve any problem they are just here to demonstrate the capabilities of the pair DLX/MDCG.

### Logger

First example, it is really artificial.
```c
void foo(int n, float * a, float * b) {
  int i;

#pragma logger log message("foo(%d)") where(both) cond(n%100, 5) params(n)
  for (i = 0; i < n; i++)
    a[i] += b[i];
}
```
Becomes:
```c
void foo(int n,float *a,float *b)
{
  int i;
  logger_log_before(0,n%100,n);
  for (i = 0; i < n; i++) 
    a[i] += b[i];
  logger_log_after(0,n,n);
}
```
And:
```c
#include "libLogger/liblogger.h" 

struct logger_param_t params_0[1UL] = {{sizeof(int )}};
int cond_0 = 20;
struct logger_cond_t conds_0[1UL] = {{sizeof(int ), &cond_0}};
struct logger_point_t points_0[1UL] = {{2, "foo(%d)", 1, params_0, 1, conds_0}};
struct logger_data_t logger_data = {1, points_0};
```

## Mini DLXs

This Section should contain example of small DLXs that solve some minor problem.

## Full DLXs

Real world DLX implemented using DLX/MDCG and other ressources from ROSE

### OpenACC

[RoseACC](https://github.com/tristanvdb/RoseACC-workspace) is ROSE Compiler implementation of the OpenACC standard.
It is using the module [Kernel from Loop Tree (KLT)](../../src/backend/KLT).

