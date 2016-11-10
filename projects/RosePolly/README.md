# **RosePolly**
A polyhedral compilation framework that is designed to be a modular and extensible API as opposed to a
monolithic executable. As a result, users are encouraged to synthesize custom compilation, analysis and profiling flows and passes using the well-defined layered class hierarchy of RosePolly.

## TODO
1. Implement a Cloog-to-RoseIR pass

2. Make the "detect_hyperplane_properties" a RosePolly member. Get h_types and l_types from the 
   sequential schedule as well.

3. Propagate maps to symbol table entries using the pollyMath interface

4. Remove "normalize_matrices" method from the permutability cst method -> it is isl_specific

## Dependencies

- ISL: Integer Set Library
- GMP
- CLOOG

## Example

Compile and link with `libpoly`:

``` bash
$ g++ /Users/konstantinid1/Internship/Source/projects/RosePolly/src/main.cpp -I/Users/konstantinid1/Internship/Install/rosepoly-0.1.0/include -L/Users/konstantinid1/Internship/Install/rosepoly-0.1.0/lib -lrosepoly -I/Users/konstantinid1/Internship/Install/include -L/Users/konstantinid1/Internship/Install/lib -lrose -I/Users/konstantinid1/Internship/Tools/CLOOG/include -L/Users/konstantinid1/Internship/Tools/CLOOG/lib -lcloog-isl -DCLOOG_INT_GMP -I/Users/konstantinid1/Internship/Tools/ISL/include -L/Users/konstantinid1/Internship/Tools/ISL/lib -lisl -DISL -I/Users/konstantinid1/Development/opt/include  
```

Run our `libpoly` translator on a test file:

``` bash
$ ./a.out /Users/konstantinid1/Internship/Source/projects/RosePolly/matmul.c"/usr/include/sys/cdefs.h", line 140: warning: incompatible redefinition of          macro "__dead2" (declared at line 162 of          "/Users/konstantinid1/Internship/Install/include/gcc_HEADERS/rose_edg          _required_macros_and_functions.h")
  #define	__dead2		__attribute__((__noreturn__))
         	^

"/usr/include/sys/cdefs.h", line 141: warning: incompatible redefinition of
          macro "__pure2" (declared at line 163 of
          "/Users/konstantinid1/Internship/Install/include/gcc_HEADERS/rose_edg
          _required_macros_and_functions.h")
  #define	__pure2		__attribute__((__const__))
         	^


[Looking for Candidates (annotated) . . . ]
    [Initializing Data]
    [Putting Graph Together]
    [Scanning for Structure Parameters]
[Found(1) ]


[Building Polyhedral Model for Kernel(<<0>>)]

  [Evaluating Kernel against the Pollyhedral constraints] . . . PASS !

[FINISHED]

[PLUTO]
  [POLYHEDRAL MODEL of Kernel (0)]

  [STATEMENT : C[i][j] =((1 * C[i][j]) +((1 * A[i][k]) * B[k][j]));]
  __domain__
  [N] -> { [i, j, k] : i >= 0 and i <= -1 + N and j >= 0 and j <= -1 + N and k >= 0 and k <= -1 + N }

  __reads__
  C
  [N] -> { [i, j, k] -> [i, j] : i >= 0 and i <= -1 + N and j >= 0 and j <= -1 + N and k >= 0 and k <= -1 + N }
  A
  [N] -> { [i, j, k] -> [i, k] : i >= 0 and i <= -1 + N and j >= 0 and j <= -1 + N and k >= 0 and k <= -1 + N }
  B
  [N] -> { [i, j, k] -> [k, j] : i >= 0 and i <= -1 + N and j >= 0 and j <= -1 + N and k >= 0 and k <= -1 + N }

  __writes__
  C
  [N] -> { [i, j, k] -> [i, j] : i >= 0 and i <= -1 + N and j >= 0 and j <= -1 + N and k >= 0 and k <= -1 + N }

  __seq_schedule__
  [N] -> { [i, j, k] -> [0, i, 0, j, 0, k, 0] }

  __transformation__
  [N] -> { [i, j, k] -> [i, j, k] }

  __loop_types__
    PARALLEL
    PARALLEL
    PIPELINE


[CLOOG]

for (c1=0;c1<=N-1;c1++) {
  for (c2=0;c2<=N-1;c2++) {
    for (c3=0;c3<=N-1;c3++) {
      S0(c1,c2,c3);
    }
  }
}
```
