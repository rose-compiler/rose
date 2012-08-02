#include <stdio.h>
#include <math.h>
#include <upc_relaxed.h>
#if WITH_UPC
/* with RTED */
#include "RuntimeSystem.h"
// instrument code
// runtimeCheck heattrans.upc -rose:UPC -rose:upc_threads 8 -DWITH_UPC -I../../../ROSE/config -c -I../../../ROSE/projects/RTED -I. -I../..
// compile instrumented code
// upc -O0 -g -dwarf-2-upc -DWITH_UPC=1 -Wall -Wextra -fupc-threads-8 -I../../../ROSE/projects/RTED -I. -I../.. -c rose_heattrans.upc
// link instrumented code
// upc++link -dwarf-2-upc -o rose_heattrans.bin rose_heattrans.o RuntimeSystemUpc.o ParallelRTS.o -L./CppRuntimeSystem/.libs/ -lUpcRuntimeSystem
#endif
#define N         32
#define Q         (N-1)
#define BLOCKSIZE 1
shared[1] double grids[2UL][32UL][32UL][32UL];
shared[1] double dTmax_local[8UL];

void initialize()
{
  rted_ConfirmFunctionSignature("initialize",1,((struct rted_TypeDesc []){{"SgTypeVoid", "", {0, 0}}}));
// boundary values on the edges
  rted_EnterScope("for:30");
{
    int RuntimeSystem_eval_once = 1;
    for (int a = 1; a < (32 - 1) | (RuntimeSystem_eval_once && rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&a))),sizeof(a),"a","a",1,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__})) | (rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&a))),sizeof(a),0,akStack,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 29, __LINE__})) | (RuntimeSystem_eval_once = 0))); ++a) {
      rted_EnterScope("upc_forall:32");
{
        int RuntimeSystem_eval_once = 1;
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
        rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),sizeof(grids[0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 31, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
        rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),sizeof(grids[0][0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 31, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
        rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][0][a])))),sizeof(grids[0][0][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 31, __LINE__}));
        upc_forall (int b = 1; b < (32 - 1) | (RuntimeSystem_eval_once && rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&b))),sizeof(b),"b","b",1,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__})) | (rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&b))),sizeof(b),0,akStack,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 31, __LINE__})) | (RuntimeSystem_eval_once = 0))); ++b; grids[0][0][a] + b)
{
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),sizeof(grids[0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),sizeof(grids[0][0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][0][a])))),sizeof(grids[0][0][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][0][a][b])))),sizeof(grids[0][0][a][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1])))),sizeof(grids[1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][0])))),sizeof(grids[1][0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][0][a])))),sizeof(grids[1][0][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][0][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][0][a][b])))),sizeof(grids[1][0][a][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
          grids[0][0][a][b] = (grids[1][0][a][b] = 1.0);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[1][0][a][b])))),sizeof(grids[1][0][a][b]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[0][0][a][b])))),sizeof(grids[0][0][a][b]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 33, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),sizeof(grids[0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a])))),sizeof(grids[0][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][b])))),sizeof(grids[0][a][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][b][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][b][0])))),sizeof(grids[0][a][b][0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1])))),sizeof(grids[1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a])))),sizeof(grids[1][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][b])))),sizeof(grids[1][a][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][b][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][b][0])))),sizeof(grids[1][a][b][0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
          grids[0][a][b][0] = (grids[1][a][b][0] = 1.0);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][b][0])))),sizeof(grids[1][a][b][0]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][b][0])))),sizeof(grids[0][a][b][0]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 34, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),sizeof(grids[0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a])))),sizeof(grids[0][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][0])))),sizeof(grids[0][a][0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][0][b])))),sizeof(grids[0][a][0][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1])))),sizeof(grids[1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a])))),sizeof(grids[1][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][0])))),sizeof(grids[1][a][0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][0][b])))),sizeof(grids[1][a][0][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
          grids[0][a][0][b] = (grids[1][a][0][b] = 1.0);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][0][b])))),sizeof(grids[1][a][0][b]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][0][b])))),sizeof(grids[0][a][0][b]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 35, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),sizeof(grids[0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][(32 - 1)])))),sizeof(grids[0][(32 - 1)]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][(32 - 1)][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][(32 - 1)][a])))),sizeof(grids[0][(32 - 1)][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][(32 - 1)][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][(32 - 1)][a][b])))),sizeof(grids[0][(32 - 1)][a][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1])))),sizeof(grids[1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][(32 - 1)])))),sizeof(grids[1][(32 - 1)]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][(32 - 1)][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][(32 - 1)][a])))),sizeof(grids[1][(32 - 1)][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][(32 - 1)][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][(32 - 1)][a][b])))),sizeof(grids[1][(32 - 1)][a][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
          grids[0][(32 - 1)][a][b] = (grids[1][(32 - 1)][a][b] = 1.0);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[1][(32 - 1)][a][b])))),sizeof(grids[1][(32 - 1)][a][b]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[0][(32 - 1)][a][b])))),sizeof(grids[0][(32 - 1)][a][b]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 37, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),sizeof(grids[0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a])))),sizeof(grids[0][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][b])))),sizeof(grids[0][a][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][b][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][b][(32 - 1)])))),sizeof(grids[0][a][b][(32 - 1)]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1])))),sizeof(grids[1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a])))),sizeof(grids[1][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][b])))),sizeof(grids[1][a][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][b][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][b][(32 - 1)])))),sizeof(grids[1][a][b][(32 - 1)]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
          grids[0][a][b][(32 - 1)] = (grids[1][a][b][(32 - 1)] = 1.0);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][b][(32 - 1)])))),sizeof(grids[1][a][b][(32 - 1)]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][b][(32 - 1)])))),sizeof(grids[0][a][b][(32 - 1)]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 38, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&a))),sizeof(a),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&b))),sizeof(b),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),sizeof(grids[0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a])))),sizeof(grids[0][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][(32 - 1)])))),sizeof(grids[0][a][(32 - 1)]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][(32 - 1)][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][(32 - 1)][b])))),sizeof(grids[0][a][(32 - 1)][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1])))),sizeof(grids[1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a])))),sizeof(grids[1][a]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][(32 - 1)])))),sizeof(grids[1][a][(32 - 1)]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][(32 - 1)][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][(32 - 1)][b])))),sizeof(grids[1][a][(32 - 1)][b]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
          grids[0][a][(32 - 1)][b] = (grids[1][a][(32 - 1)][b] = 1.0);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[1][a][(32 - 1)][b])))),sizeof(grids[1][a][(32 - 1)][b]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[0][a][(32 - 1)][b])))),sizeof(grids[0][a][(32 - 1)][b]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 39, __LINE__}));
        }
      }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
      rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 40, __LINE__}));
    }
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 41, __LINE__}));
// initial value
  rted_EnterScope("for:45");
{
    int RuntimeSystem_eval_once = 1;
    for (int z = 1; z < (32 - 1) | (RuntimeSystem_eval_once && rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&z))),sizeof(z),"z","z",1,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__})) | (rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&z))),sizeof(z),0,akStack,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 44, __LINE__})) | (RuntimeSystem_eval_once = 0))); ++z) {
      rted_EnterScope("for:47");
{
        int RuntimeSystem_eval_once = 1;
        for (int y = 1; y < (32 - 1) | (RuntimeSystem_eval_once && rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&y))),sizeof(y),"y","y",1,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__})) | (rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&y))),sizeof(y),0,akStack,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 46, __LINE__})) | (RuntimeSystem_eval_once = 0))); ++y) {
          rted_EnterScope("upc_forall:49");
{
            int RuntimeSystem_eval_once = 1;
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
            rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),sizeof(grids[0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 48, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
            rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][z])))),sizeof(grids[0][z]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 48, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
            rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][z][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][z][y])))),sizeof(grids[0][z][y]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 48, __LINE__}));
            upc_forall (int x = 1; x < (32 - 1) | (RuntimeSystem_eval_once && rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&x))),sizeof(x),"x","x",1,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__})) | (rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&x))),sizeof(x),0,akStack,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 48, __LINE__})) | (RuntimeSystem_eval_once = 0))); ++x; grids[0][z][y] + x)
{
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
              rted_AccessVariable(rted_Addr(((char *)(&z))),sizeof(z),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 50, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
              rted_AccessVariable(rted_Addr(((char *)(&y))),sizeof(y),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 50, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
              rted_AccessVariable(rted_Addr(((char *)(&x))),sizeof(x),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 50, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
              rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),sizeof(grids[0]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 50, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
              rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][z])))),sizeof(grids[0][z]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 50, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
              rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][z][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][z][y])))),sizeof(grids[0][z][y]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 50, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
              rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0][z][y][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[0][z][y][x])))),sizeof(grids[0][z][y][x]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 50, __LINE__}));
              grids[0][z][y][x] = 2.0;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
              rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[0][z][y][x])))),sizeof(grids[0][z][y][x]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 50, __LINE__}));
            }
          }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
          rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 51, __LINE__}));
        }
      }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
      rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 52, __LINE__}));
    }
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 53, __LINE__}));
}

extern int RuntimeSystem_original_main(int argc,char **argv,char **envp)
{
  rted_ConfirmFunctionSignature("RuntimeSystem_original_main",1,((struct rted_TypeDesc []){{"SgTypeInt", "", {0, 0}}}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgPointerType", "SgTypeChar", {2, 0}}),rted_Addr(((char *)(&envp))),sizeof(envp),"envp","envp",1,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgPointerType", "SgTypeChar", {2, 0}}),rted_Addr(((char *)(&argv))),sizeof(argv),"argv","argv",1,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&argc))),sizeof(argc),"argc","argc",1,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  double dTmax;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&dTmax))),sizeof(dTmax),"dTmax","dTmax",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  double dT;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&dT))),sizeof(dT),"dT","dT",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  double epsilon;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&epsilon))),sizeof(epsilon),"epsilon","epsilon",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int x;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&x))),sizeof(x),"x","x",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int y;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&y))),sizeof(y),"y","y",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int z;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&z))),sizeof(z),"z","z",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  double T;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&T))),sizeof(T),"T","T",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int nr_iter;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&nr_iter))),sizeof(nr_iter),"nr_iter","nr_iter",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
// source grid
  int sg;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&sg))),sizeof(sg),"sg","sg",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
// destination grid
  int dg;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&dg))),sizeof(dg),"dg","dg",0,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  rted_EnterScope("initialize:70");
  initialize();
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 54, __LINE__}));
  epsilon = .0001;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&epsilon))),sizeof(epsilon),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 71, __LINE__}));
  nr_iter = 0;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&nr_iter))),sizeof(nr_iter),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 72, __LINE__}));
  sg = 1;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&sg))),sizeof(sg),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 73, __LINE__}));
  dg = !(sg != 0);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&dg))),sizeof(dg),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 74, __LINE__}));
/*  */
/* RS: UpcExitWorkzone() */
  rted_UpcExitWorkzone();
  upc_barrier ;
/*  */
/* RS: UpcEnterWorkzone() */
  rted_UpcEnterWorkzone();
  rted_EnterScope("do:79");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&epsilon))),sizeof(epsilon),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 119, __LINE__}));
  do {
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&sg))),sizeof(sg),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 80, __LINE__}));
    dg = sg;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
    rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&dg))),sizeof(dg),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 80, __LINE__}));
    sg = !(sg != 0);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
    rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&sg))),sizeof(sg),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 81, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&nr_iter))),sizeof(nr_iter),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 82, __LINE__}));
    ++nr_iter;
    dTmax = 0.0;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
    rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&dTmax))),sizeof(dTmax),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 84, __LINE__}));
    rted_EnterScope("for:86");
{
      int RuntimeSystem_eval_once = 1;
      for (z = 1; z < (32 - 1) | (RuntimeSystem_eval_once && rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&z))),sizeof(z),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 85, __LINE__})) | (RuntimeSystem_eval_once = 0)); ++z) {
        rted_EnterScope("for:88");
{
          int RuntimeSystem_eval_once = 1;
          for (y = 1; y < (32 - 1) | (RuntimeSystem_eval_once && rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&y))),sizeof(y),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 87, __LINE__})) | (RuntimeSystem_eval_once = 0)); ++y) {
            rted_EnterScope("upc_forall:90");
{
              int RuntimeSystem_eval_once = 1;
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
              rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg])))),sizeof(grids[sg]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 89, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
              rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z])))),sizeof(grids[sg][z]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 89, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
              rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y])))),sizeof(grids[sg][z][y]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 89, __LINE__}));
              upc_forall (x = 1; x < (32 - 1) | (RuntimeSystem_eval_once && rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&x))),sizeof(x),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 89, __LINE__})) | (RuntimeSystem_eval_once = 0)); ++x; grids[sg][z][y] + x)
{
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&sg))),sizeof(sg),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&y))),sizeof(y),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&x))),sizeof(x),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&sg))),sizeof(sg),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&y))),sizeof(y),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&x))),sizeof(x),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&sg))),sizeof(sg),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&z))),sizeof(z),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&x))),sizeof(x),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&sg))),sizeof(sg),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&z))),sizeof(z),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&x))),sizeof(x),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&sg))),sizeof(sg),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&z))),sizeof(z),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&y))),sizeof(y),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&sg))),sizeof(sg),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&z))),sizeof(z),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&y))),sizeof(y),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg])))),sizeof(grids[sg]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z + 1])))),sizeof(grids[sg][z + 1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z + 1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z + 1][y])))),sizeof(grids[sg][z + 1][y]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z + 1][y][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z + 1][y][x])))),sizeof(grids[sg][z + 1][y][x]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg])))),sizeof(grids[sg]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z - 1])))),sizeof(grids[sg][z - 1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z - 1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z - 1][y])))),sizeof(grids[sg][z - 1][y]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z - 1][y][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z - 1][y][x])))),sizeof(grids[sg][z - 1][y][x]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg])))),sizeof(grids[sg]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z])))),sizeof(grids[sg][z]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y + 1])))),sizeof(grids[sg][z][y + 1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y + 1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y + 1][x])))),sizeof(grids[sg][z][y + 1][x]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg])))),sizeof(grids[sg]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z])))),sizeof(grids[sg][z]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y - 1])))),sizeof(grids[sg][z][y - 1]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y - 1][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y - 1][x])))),sizeof(grids[sg][z][y - 1][x]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg])))),sizeof(grids[sg]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z])))),sizeof(grids[sg][z]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y])))),sizeof(grids[sg][z][y]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y][x + 1])))),sizeof(grids[sg][z][y][x + 1]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg])))),sizeof(grids[sg]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z])))),sizeof(grids[sg][z]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y])))),sizeof(grids[sg][z][y]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y][x - 1])))),sizeof(grids[sg][z][y][x - 1]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
                T = ((((((grids[sg][z + 1][y][x] + grids[sg][z - 1][y][x]) + grids[sg][z][y + 1][x]) + grids[sg][z][y - 1][x]) + grids[sg][z][y][x + 1]) + grids[sg][z][y][x - 1]) / 6.0);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
                rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&T))),sizeof(T),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 91, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&sg))),sizeof(sg),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 99, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&z))),sizeof(z),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 99, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&y))),sizeof(y),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 99, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&x))),sizeof(x),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 99, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg])))),sizeof(grids[sg]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 99, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z])))),sizeof(grids[sg][z]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 99, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y])))),sizeof(grids[sg][z][y]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 99, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[sg][z][y][x])))),sizeof(grids[sg][z][y][x]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 99, __LINE__}));
                dT = (T - grids[sg][z][y][x]);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
                rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&dT))),sizeof(dT),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 99, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&dg))),sizeof(dg),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&z))),sizeof(z),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&y))),sizeof(y),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&x))),sizeof(x),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&T))),sizeof(T),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[dg])))),sizeof(grids[dg]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[dg][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[dg][z])))),sizeof(grids[dg][z]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[dg][z][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[dg][z][y])))),sizeof(grids[dg][z][y]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
                rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&grids[dg][z][y][0])))),rted_AddrSh(((shared char *)((shared char *)(&grids[dg][z][y][x])))),sizeof(grids[dg][z][y][x]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
                grids[dg][z][y][x] = T;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
                rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&grids[dg][z][y][x])))),sizeof(grids[dg][z][y][x]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 100, __LINE__}));
                rted_EnterScope("fabs:103");
                rted_EnterScope("if:103");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                rted_AccessVariable(rted_Addr(((char *)(&dT))),sizeof(dT),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 102, __LINE__}));
                rted_AssertFunctionSignature("fabs",2,((struct rted_TypeDesc []){{"SgTypeDouble", "", {0, 0}}, {"SgTypeDouble", "", {0, 0}}}),((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 102, __LINE__}));
                if (dTmax < fabs(dT)) {
                  rted_EnterScope("fabs:103");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
                  rted_AccessVariable(rted_Addr(((char *)(&dT))),sizeof(dT),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 102, __LINE__}));
                  rted_AssertFunctionSignature("fabs",2,((struct rted_TypeDesc []){{"SgTypeDouble", "", {0, 0}}, {"SgTypeDouble", "", {0, 0}}}),((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 102, __LINE__}));
                  dTmax = fabs(dT);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
                  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&dTmax))),sizeof(dTmax),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 102, __LINE__}));
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
                  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 102, __LINE__}));
                }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
                rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 102, __LINE__}));
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
                rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 102, __LINE__}));
              }
            }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
            rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 103, __LINE__}));
          }
        }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
        rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 104, __LINE__}));
      }
    }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 105, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&dTmax))),sizeof(dTmax),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 107, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
    rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&dTmax_local[0])))),rted_AddrSh(((shared char *)((shared char *)(&dTmax_local[MYTHREAD ])))),sizeof(dTmax_local[MYTHREAD ]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 107, __LINE__}));
    dTmax_local[MYTHREAD ] = dTmax;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
    rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&dTmax_local[MYTHREAD ])))),sizeof(dTmax_local[MYTHREAD ]),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 107, __LINE__}));
/*  */
/* RS: UpcExitWorkzone() */
    rted_UpcExitWorkzone();
    upc_barrier ;
/*  */
/* RS: UpcEnterWorkzone() */
    rted_UpcEnterWorkzone();
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
    rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&dTmax_local[0])))),rted_AddrSh(((shared char *)((shared char *)(&dTmax_local[0])))),sizeof(dTmax_local[0]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 110, __LINE__}));
    dTmax = dTmax_local[0];
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
    rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&dTmax))),sizeof(dTmax),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 110, __LINE__}));
    rted_EnterScope("for:113");
{
      int RuntimeSystem_eval_once = 1;
      for (int i = 1; i < 8 | (RuntimeSystem_eval_once && rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&i))),sizeof(i),"i","i",1,"",((struct rted_SourceInfo ){"transformation", -1, __LINE__})) | (rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&i))),sizeof(i),0,akStack,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 112, __LINE__})) | (RuntimeSystem_eval_once = 0))); ++i) {
        rted_EnterScope("if:115");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
        rted_AccessVariable(rted_Addr(((char *)(&i))),sizeof(i),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 114, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
        rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&dTmax_local[0])))),rted_AddrSh(((shared char *)((shared char *)(&dTmax_local[i])))),sizeof(dTmax_local[i]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 114, __LINE__}));
        if (dTmax < dTmax_local[i]) {
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)(&dTmax))),sizeof(dTmax),0,akUndefined,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 115, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&i))),sizeof(i),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 115, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
          rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&dTmax_local[0])))),rted_AddrSh(((shared char *)((shared char *)(&dTmax_local[i])))),sizeof(dTmax_local[i]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 115, __LINE__}));
          dTmax = dTmax_local[i];
        }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
        rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 115, __LINE__}));
      }
    }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 116, __LINE__}));
/*  */
/* RS: UpcExitWorkzone() */
    rted_UpcExitWorkzone();
    upc_barrier ;
/*  */
/* RS: UpcEnterWorkzone() */
    rted_UpcEnterWorkzone();

    if (MYTHREAD == 0) printf("  -> %d\n", nr_iter);
  }while (dTmax > epsilon);
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 119, __LINE__}));
/*  */
/* RS: UpcExitWorkzone() */
  rted_UpcExitWorkzone();
  upc_barrier ;
/*  */
/* RS: UpcEnterWorkzone() */
  rted_UpcEnterWorkzone();
  rted_EnterScope("if:124");
  if ((MYTHREAD ) == 0) {
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&nr_iter))),sizeof(nr_iter),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 125, __LINE__}));
    printf("%d iterations\n",nr_iter);
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 126, __LINE__}));
  int rstmt = 0;
/* RS : Insert Finalizing Call to Runtime System to check if error was detected (needed for automation) */
  rted_Checkpoint(((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 129, __LINE__}));
  return rstmt;
}

int main(int argc,char **argv,char **envp)
{
  rted_UpcAllInitialize();
/*  */
/* RS : Create Array Variable, paramaters : (name, manglname, typr, basetype, address, sizeof(type), array size, fromMalloc, filename, linenr, linenrTransformed, dimension info ...) */
  rted_CreateArray(((struct rted_TypeDesc ){"SgArrayType", "SgTypeDouble", {4, 31}}),rted_AddrSh(((shared char *)((shared char *)grids))),sizeof(grids),1L,0,((unsigned long []){4, 2UL + 0, 32UL + 0, 32UL + 0, 32UL + 0}),"grids","grids","",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 23, __LINE__}));
/*  */
/* RS : Create Array Variable, paramaters : (name, manglname, typr, basetype, address, sizeof(type), array size, fromMalloc, filename, linenr, linenrTransformed, dimension info ...) */
  rted_CreateArray(((struct rted_TypeDesc ){"SgArrayType", "SgTypeDouble", {1, 3}}),rted_AddrSh(((shared char *)((shared char *)dTmax_local))),sizeof(dTmax_local),1L,0,((unsigned long []){1, 8UL + 0}),"dTmax_local","dTmax_local","",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/heattrans.upc", 24, __LINE__}));
  int exit_code = RuntimeSystem_original_main(argc,argv,envp);
  rted_Close("RuntimeSystem.cpp:main");
  return exit_code;
}
