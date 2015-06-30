TileK
=======

TileK is a project to demonstrate tools used by RoseACC.
RoseACC is a compiler for OpenACC an extension of C/C++.
OpenACC uses directives (C preprocessor's #pragma) to offload
computationaly intensive sections of code into computation accelerators.

## Language

In TileK region of C/C++ codes are outlined and loops can be tiled.
Simplest example:

![equation](http://bit.ly/13TNM1G)

![equation](http://bit.ly/13TNjwp)

```c++
void foo(int n, float ** a, float b) {
  #pragma tilek kernel data(a[0:n][0:n], b)
  #pragma tilek loop tile(static, 64) tile(dynamic)
  for (int i = 0; i < n; i++)
    a[i][i]+=b;
}
```

![equation](http://bit.ly/1vUshEJ)

```c++
void bar(int n, int m, int p, int q, int r, float ** a, float ** b, float ** c, float ** d, float ** e) {
  float ** f = malloc(n * q * sizeof(float));
  float ** g = malloc(n * r * sizeof(float));

  #pragma tilek kernel data(a[0:n][0:p], b[0:p][0:q], c[0:q][0:r], d[0:r][0:m], e[0:n][0:n], f[0:n][0:q], g[0:n][0:r])
  {
    #pragma tilek loop tile(dynamic)
    for (int i = 0; i < n; i++)
      #pragma tilek loop tile(dynamic)
      for (int j = 0; j < q; j++) {
        f[i][j] = 0;
        #pragma tilek loop tile(dynamic)
        for (int k = 0; k < p; k++)
          f[i][j] += a[i][k] * b[k][j];
      }
    #pragma tilek loop tile(dynamic)
    for (int i = 0; i < n; i++)
      #pragma tilek loop tile(dynamic)
      for (int j = 0; j < r; j++) {
        g[i][j] = 0;
        #pragma tilek loop tile(dynamic)
        for (int k = 0; k < q; k++)
          g[i][j] += f[i][k] * c[k][j];
      }
    #pragma tilek loop tile(dynamic)
    for (int i = 0; i < n; i++)
      #pragma tilek loop tile(dynamic)
      for (int j = 0; j < m; j++) {
        e[i][j] = 0;
        #pragma tilek loop tile(dynamic)
        for (int k = 0; k < r; k++)
          e[i][j] += g[i][k] * d[k][j];
      }
  }
}
```

## Runtime

```c
#ifndef __LOOP_H__
#define __LOOP_H__

struct tile_t {
  enum tile_kind_e {
    e_tile_kind_1,
    e_tile_kind_2,
  } kind;
  int param;
};

struct loop_desc_t {
  int num_tiles;
  struct tile_t * tiles;
};

struct loop_t {
  int lower_bound;
  int upper_bound;
  int increment;
};

#endif /* __LOOP_H__ */
```

```c
#ifndef __KERNEL_H__
#define __KERNEL_H__

typedef void (*kernel_func_ptr)(void*);

struct kernel_desc_t {
  int num_args;
  int num_loops;
  struct loop_desc_t * loop_desc;
  kernel_func_ptr func_ptr;
};

struct kernel_t {
  struct kernel_desc_t * desc;
  void ** data;
  int * param;
  struct loop_t * loops;
};

struct kernel_t * build_kernel(
    struct kernel_desc_t * desc
);

void execute_kernel(
    struct kernel_t * kernel
);

extern struct kernel_desc_t my_kernel_desc[];

#endif /* __KERNEL_H__ */
```

## Generated Code

### Application

```c++
void foo(int n, float ** a, float b) {
  struct kernel_t * kernel = build_kernel(&(my_kernel_desc[0]));

  kernel->args[0] = &(a[0][0]);
  kernel->args[0] = &(a[0][0]);
  kernel->args[0] = &(a[0][0]);

  kernel->loops[0].lower_bound = 0;
  kernel->loops[0].upper_bound = n;
  kernel->loops[0].increment = 1;

  execute_kernel(kernel);
  

  #pragma tilek kernel data(a[0:n][0:n], b)
  #pragma tilek loop tile(static, 64) tile(dynamic)
  for (int i = 0; i < n; i++)
    a[i][i]+=b;
}
```

### Descriptor

```c
void foo(int n, float ** a, float b) {
  #pragma tilek data arg(a[0:n][0:n], b)
  #pragma tilek loop tile(static, 64) tile(dynamic)
  for (int i = 0; i < n; i++)
    a[i][i]+=b;
}
```

### Kernel

```c
void foo(int n, float ** a, float b) {
  #pragma tilek data arg(a[0:n][0:n], b)
  #pragma tilek loop tile(static, 64) tile(dynamic)
  for (int i = 0; i < n; i++)
    a[i][i]+=b;
}
```

## Content 

TileK is a demo project with many modules:
 * TileK compiler based on modules:
    * DLX: Frontend/Midend for directives
    * MFB: Midend handles header and source files
    * MDCG: Midend provides models for code generation
    * KLT: Backend
 * Vizualization used to explain what is happening

## Usage

### Building TileK

```sh
export PROJECT_DIR=/media/ssd/projects/currents # For my convenience, NOT used by Makefile
export ROSE_DIR=$PROJECT_DIR/RoseACC-workspace/install_dir/
export BOOST_DIR=/media/ssd/boost/install/1_45_0
export LD_LIBRARY_PATH=$BOOST_DIR/lib:$ROSE_DIR/lib:$AST_TO_GRAPHVIZ_DIR/lib:$LD_LIBRARY_PATH
make tilek
```

### Building Vizualizations

```sh
export AST_TO_GRAPHVIZ_DIR=$PROJECT_DIR/ast-to-graphviz/
export LD_LIBRARY_PATH=$BOOST_DIR/lib:$ROSE_DIR/lib:$AST_TO_GRAPHVIZ_DIR/lib:$LD_LIBRARY_PATH
make viz
```

### 

