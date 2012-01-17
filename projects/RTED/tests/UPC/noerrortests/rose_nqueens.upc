#include <stdio.h>
#include <upc_relaxed.h>
#include <upc.h>
#if WITH_UPC
/* with RTED */
#include "RuntimeSystem.h"
// instrument code
// .libs/lt-runtimeCheck nqueens.upc -rose:UPC -DWITH_UPC -I../../../ROSE/config -c -I../../../ROSE/projects/RTED -I. -I../..
// compile instrumented code
// upc -O0 -g -dwarf-2-upc -DWITH_UPC=1 -Wall -Wextra -g -I../../../ROSE/projects/RTED -I. -I../.. -c rose_nqueens.upc
// link instrumented code
// upc++link -dwarf-2-upc -o rose_nqueens.bin  rose_nqueens.o  RuntimeSystemUpc.o ParallelRTS.o -L./CppRuntimeSystem/.libs/ -lUpcRuntimeSystem
#endif
static const int ROUND_ROBIN = 0;
/*todo*/
typedef long mask_t;
int N;
int level;
int no_solutions = 0;
int method = 0;
mask_t basemask;
mask_t leftdiagmask;
mask_t centermask;
mask_t rightdiagmask;
shared[1] int sh_solutions[1UL * (THREADS )];

void NQueens(int cur_row,mask_t unsafe_cells)
{
  rted_ConfirmFunctionSignature("NQueens",3,((struct rted_TypeDesc []){{"SgTypeVoid", "", {0, 0}}, {"SgTypeInt", "", {0, 0}}, {"SgTypedefType", "", {0, 0}}}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&unsafe_cells))),sizeof(unsafe_cells),1,akStack,"unsafe_cells","unsafe_cells","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&cur_row))),sizeof(cur_row),1,akStack,"cur_row","cur_row","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int col;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&col))),sizeof(col),0,akStack,"col","col","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  mask_t vrlnbl_cells;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&vrlnbl_cells))),sizeof(vrlnbl_cells),0,akStack,"vrlnbl_cells","vrlnbl_cells","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  mask_t next_row_unsafe;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&next_row_unsafe))),sizeof(next_row_unsafe),0,akStack,"next_row_unsafe","next_row_unsafe","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  rted_EnterScope("if:42");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 41, __LINE__}));
  if (cur_row == N) {
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&no_solutions))),sizeof(no_solutions),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 43, __LINE__}));
    ++no_solutions;
  }
  else {
    rted_EnterScope("for:48");
{
      int RuntimeSystem_eval_once = 1;
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 47, __LINE__}));
      for (col = 0; col < N | (RuntimeSystem_eval_once && rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&col))),sizeof(col),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 47, __LINE__})) | (RuntimeSystem_eval_once = 0)); ++col) {
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
        rted_AccessVariable(rted_Addr(((char *)(&col))),sizeof(col),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 49, __LINE__}));
        vrlnbl_cells = (basemask << col);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
        rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&vrlnbl_cells))),sizeof(vrlnbl_cells),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 49, __LINE__}));
        rted_EnterScope("if:52");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
        rted_AccessVariable(rted_Addr(((char *)(&vrlnbl_cells))),sizeof(vrlnbl_cells),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 51, __LINE__}));
        if (!((unsafe_cells & vrlnbl_cells) != 0L)) {
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&vrlnbl_cells))),sizeof(vrlnbl_cells),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 53, __LINE__}));
          next_row_unsafe = (unsafe_cells | vrlnbl_cells);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&next_row_unsafe))),sizeof(next_row_unsafe),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 53, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&leftdiagmask))),sizeof(leftdiagmask),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 54, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&centermask))),sizeof(centermask),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 54, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&rightdiagmask))),sizeof(rightdiagmask),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 54, __LINE__}));
          next_row_unsafe = ((((next_row_unsafe & leftdiagmask) >> 1) | (next_row_unsafe & centermask)) | ((next_row_unsafe & rightdiagmask) << 1));
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
          rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&next_row_unsafe))),sizeof(next_row_unsafe),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 54, __LINE__}));
          rted_EnterScope("NQueens:60");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
          rted_AccessVariable(rted_Addr(((char *)(&next_row_unsafe))),sizeof(next_row_unsafe),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 59, __LINE__}));
          NQueens((cur_row + 1),next_row_unsafe);
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
          rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 63, __LINE__}));
        }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
        rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 60, __LINE__}));
      }
    }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 61, __LINE__}));
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 62, __LINE__}));
}

void do_job(int job,int no_jobs)
{
  rted_ConfirmFunctionSignature("do_job",3,((struct rted_TypeDesc []){{"SgTypeVoid", "", {0, 0}}, {"SgTypeInt", "", {0, 0}}, {"SgTypeInt", "", {0, 0}}}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&no_jobs))),sizeof(no_jobs),1,akStack,"no_jobs","no_jobs","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&job))),sizeof(job),1,akStack,"job","job","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int j;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&j))),sizeof(j),0,akStack,"j","j","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int row;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&row))),sizeof(row),0,akStack,"row","row","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int col;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&col))),sizeof(col),0,akStack,"col","col","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int conflict;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&conflict))),sizeof(conflict),0,akStack,"conflict","conflict","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  mask_t vrlnbl_cells;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&vrlnbl_cells))),sizeof(vrlnbl_cells),0,akStack,"vrlnbl_cells","vrlnbl_cells","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  mask_t unsafe_cells;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&unsafe_cells))),sizeof(unsafe_cells),0,akStack,"unsafe_cells","unsafe_cells","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 75, __LINE__}));
  j = (no_jobs / N);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&j))),sizeof(j),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 75, __LINE__}));
  unsafe_cells = 0;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&unsafe_cells))),sizeof(unsafe_cells),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 76, __LINE__}));
  row = 0;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&row))),sizeof(row),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 77, __LINE__}));
  conflict = 0;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&conflict))),sizeof(conflict),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 78, __LINE__}));
  rted_EnterScope("while:81");
  while((j >= 1) && !(conflict != 0)){
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&j))),sizeof(j),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 82, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&j))),sizeof(j),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 82, __LINE__}));
    col = ((job % (N * j)) / j);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
    rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&col))),sizeof(col),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 82, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&col))),sizeof(col),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 83, __LINE__}));
    vrlnbl_cells = (basemask << col);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
    rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&vrlnbl_cells))),sizeof(vrlnbl_cells),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 83, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&vrlnbl_cells))),sizeof(vrlnbl_cells),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 85, __LINE__}));
    conflict = (unsafe_cells & vrlnbl_cells);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
    rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&conflict))),sizeof(conflict),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 85, __LINE__}));
    rted_EnterScope("if:88");
    if (!(conflict != 0)) {
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&vrlnbl_cells))),sizeof(vrlnbl_cells),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 89, __LINE__}));
      unsafe_cells |= vrlnbl_cells;
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&leftdiagmask))),sizeof(leftdiagmask),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 90, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&centermask))),sizeof(centermask),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 90, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&rightdiagmask))),sizeof(rightdiagmask),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 90, __LINE__}));
      unsafe_cells = ((((unsafe_cells & leftdiagmask) >> 1) | (unsafe_cells & centermask)) | ((unsafe_cells & rightdiagmask) << 1));
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
      rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&unsafe_cells))),sizeof(unsafe_cells),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 90, __LINE__}));
      rted_EnterScope("if:96");
      if (j == 1) {
        rted_EnterScope("NQueens:98");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
        rted_AccessVariable(rted_Addr(((char *)(&unsafe_cells))),sizeof(unsafe_cells),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 97, __LINE__}));
        NQueens((row + 1),unsafe_cells);
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
        rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 63, __LINE__}));
      }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
      rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 98, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&row))),sizeof(row),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 100, __LINE__}));
      ++row;
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 100, __LINE__}));
      j /= N;
    }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 101, __LINE__}));
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 102, __LINE__}));
}

void distribute_work()
{
  rted_ConfirmFunctionSignature("distribute_work",1,((struct rted_TypeDesc []){{"SgTypeVoid", "", {0, 0}}}));
  int i;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&i))),sizeof(i),0,akStack,"i","i","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int job;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&job))),sizeof(job),0,akStack,"job","job","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int no_jobs;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&no_jobs))),sizeof(no_jobs),0,akStack,"no_jobs","no_jobs","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  rted_EnterScope("for:112");
{
    int RuntimeSystem_eval_once = 1;
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&level))),sizeof(level),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 111, __LINE__}));
    for (
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 111, __LINE__})), ((i = 0) , (no_jobs = N)); i < level | (RuntimeSystem_eval_once && rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&no_jobs))),sizeof(no_jobs),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 111, __LINE__})) | (rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&i))),sizeof(i),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 111, __LINE__})) | (RuntimeSystem_eval_once = 0))); ++i) {
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 113, __LINE__}));
      no_jobs *= N;
    }
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 114, __LINE__}));
  rted_EnterScope("if:117");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&ROUND_ROBIN))),sizeof(ROUND_ROBIN),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 116, __LINE__}));
  if (method == ROUND_ROBIN) {
    rted_EnterScope("upc_forall:119");
{
      int RuntimeSystem_eval_once = 1;
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&no_jobs))),sizeof(no_jobs),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 118, __LINE__}));
      upc_forall (job = 0; job < no_jobs | (RuntimeSystem_eval_once && rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&job))),sizeof(job),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 118, __LINE__})) | (RuntimeSystem_eval_once = 0)); ++job; job)
{
        rted_EnterScope("do_job:121");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
        rted_AccessVariable(rted_Addr(((char *)(&no_jobs))),sizeof(no_jobs),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 120, __LINE__}));
        do_job(job,no_jobs);
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
        rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 103, __LINE__}));
      }
    }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 121, __LINE__}));
  }
  else {
    rted_EnterScope("upc_forall:126");
{
      int RuntimeSystem_eval_once = 1;
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
      rted_AccessVariable(rted_Addr(((char *)(&no_jobs))),sizeof(no_jobs),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 125, __LINE__}));
      upc_forall (job = 0; job < no_jobs | (RuntimeSystem_eval_once && rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&job))),sizeof(job),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 125, __LINE__})) | (RuntimeSystem_eval_once = 0)); ++job; (job * (THREADS )) / no_jobs)
{
        rted_EnterScope("do_job:128");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
        rted_AccessVariable(rted_Addr(((char *)(&no_jobs))),sizeof(no_jobs),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 127, __LINE__}));
        do_job(job,no_jobs);
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
        rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 103, __LINE__}));
      }
    }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 128, __LINE__}));
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 129, __LINE__}));
}

extern int RuntimeSystem_original_main(int argc,char **argv,char **envp)
{
  rted_ConfirmFunctionSignature("RuntimeSystem_original_main",3,((struct rted_TypeDesc []){{"SgTypeInt", "", {0, 0}}, {"SgTypeInt", "", {0, 0}}, {"SgPointerType", "SgTypeChar", {2, 0}}}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgPointerType", "SgTypeChar", {2, 0}}),rted_Addr(((char *)(&envp))),sizeof(envp),1,akGlobal,"envp","envp","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgPointerType", "SgTypeChar", {2, 0}}),rted_Addr(((char *)(&argv))),sizeof(argv),1,akStack,"argv","argv","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&argc))),sizeof(argc),1,akStack,"argc","argc","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int i;
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&i))),sizeof(i),0,akStack,"i","i","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  rted_EnterScope("if:137");
  if ((argc != 3) && (argc != 4)) {
    rted_EnterScope("if:139");
    if ((MYTHREAD ) == 0) {
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
      rted_AccessArray(rted_Addr(((char *)(&argv[0]))),rted_Addr(((char *)(&argv[0]))),sizeof(argv[0]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 138, __LINE__}));
      printf("Usage: %s [N] [1v1] [CHUNK_FLAG]\n",argv[0]);
    }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 138, __LINE__}));
    int rstmt = (-1);
    return rstmt;
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 141, __LINE__}));
  rted_EnterScope("atoi:144");
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
  //~ rted_AccessArray(rted_Addr(((char *)(&argv[0]))),rted_Addr(((char *)(&argv[1]))),sizeof(argv[1]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 143, __LINE__}));
  //~ rted_AssertFunctionSignature("atoi",2,((struct rted_TypeDesc []){{"SgTypeInt", "", {0, 0}}, {"SgPointerType", "SgModifierType", {1, 0}}}),((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 143, __LINE__}));
  N = atoi(argv[1]);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&N))),sizeof(N),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 143, __LINE__}));
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 143, __LINE__}));
  rted_EnterScope("atoi:145");
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
  //~ rted_AccessArray(rted_Addr(((char *)(&argv[0]))),rted_Addr(((char *)(&argv[2]))),sizeof(argv[2]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 144, __LINE__}));
  //~ rted_AssertFunctionSignature("atoi",2,((struct rted_TypeDesc []){{"SgTypeInt", "", {0, 0}}, {"SgPointerType", "SgModifierType", {1, 0}}}),((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 144, __LINE__}));
  level = atoi(argv[2]);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&level))),sizeof(level),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 144, __LINE__}));
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 144, __LINE__}));
// 3 args, CHUNK method enabled
  rted_EnterScope("if:147");
  if (argc == 3) {
    rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&method))),sizeof(method),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 146, __LINE__}));
    method = 1;
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 146, __LINE__}));
  rted_EnterScope("if:149");
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 148, __LINE__}));
  if (level >= N) {
    rted_EnterScope("if:151");
    if ((MYTHREAD ) == 0)
      printf("lvl should be < N\n");
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 150, __LINE__}));
    int rstmt = (-1);
    return rstmt;
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 153, __LINE__}));
  rted_EnterScope("if:156");
  if ((N * 3) > 64) {
    rted_EnterScope("if:158");
    if ((MYTHREAD ) == 0)
      printf("64 bit word not enough\n");
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 157, __LINE__}));
    int rstmt = (-1);
    return rstmt;
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 160, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 162, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 162, __LINE__}));
  basemask = ((1 | (1 << N)) | (1 << (2 * N)));
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&basemask))),sizeof(basemask),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 162, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 163, __LINE__}));
  leftdiagmask = ((1 << N) - 1);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&leftdiagmask))),sizeof(leftdiagmask),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 163, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 164, __LINE__}));
  centermask = (leftdiagmask << N);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&centermask))),sizeof(centermask),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 164, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 165, __LINE__}));
  rightdiagmask = (centermask << N);
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&rightdiagmask))),sizeof(rightdiagmask),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 165, __LINE__}));
  rted_EnterScope("distribute_work:168");
  distribute_work();
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 130, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&N))),sizeof(N),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 169, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&no_solutions))),sizeof(no_solutions),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 169, __LINE__}));
  printf("# %d: Total number of solutions for N=%d: %d\n",(MYTHREAD ),N,no_solutions);
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
  rted_AccessVariable(rted_Addr(((char *)(&no_solutions))),sizeof(no_solutions),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 170, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
  rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&sh_solutions[0])))),rted_AddrSh(((shared char *)((shared char *)(&sh_solutions[MYTHREAD ])))),sizeof(sh_solutions[MYTHREAD ]),4,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 170, __LINE__}));
  sh_solutions[MYTHREAD ] = no_solutions;
/*  */
/* RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line) */
  rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 1}}),rted_AddrSh(((shared char *)((shared char *)(&sh_solutions[MYTHREAD ])))),sizeof(sh_solutions[MYTHREAD ]),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 170, __LINE__}));
/*  */
/* RS: UpcExitWorkzone() */
  rted_UpcExitWorkzone();
  upc_barrier ;
/*  */
/* RS: UpcEnterWorkzone() */
  rted_UpcEnterWorkzone();
  rted_EnterScope("if:174");
  if ((MYTHREAD ) == 0) {
    rted_EnterScope("for:176");
{
      int RuntimeSystem_eval_once = 1;
      for (i = 1; i < (THREADS ) | (RuntimeSystem_eval_once && rted_InitVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&i))),sizeof(i),0,"",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 175, __LINE__})) | (RuntimeSystem_eval_once = 0)); ++i) {
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
        rted_AccessVariable(rted_Addr(((char *)(&i))),sizeof(i),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 177, __LINE__}));
/*  */
/* RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message) */
        rted_AccessArray(rted_AddrSh(((shared char *)((shared char *)(&sh_solutions[0])))),rted_AddrSh(((shared char *)((shared char *)(&sh_solutions[i])))),sizeof(sh_solutions[i]),5,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 177, __LINE__}));
        no_solutions += sh_solutions[i];
      }
    }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
    rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 178, __LINE__}));
/*  */
/* RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str) */
    rted_AccessVariable(rted_Addr(((char *)(&no_solutions))),sizeof(no_solutions),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 180, __LINE__}));
    printf("Total number of solutions: %d\n",no_solutions);
  }
/*  */
/* RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt) */
  rted_ExitScope("todo-unparse-string",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 181, __LINE__}));
  int rstmt = 0;
/* RS : Insert Finalizing Call to Runtime System to check if error was detected (needed for automation) */
  rted_Checkpoint(((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 184, __LINE__}));
  return rstmt;
}

int main(int argc,char **argv,char **envp)
{
  rted_UpcAllInitialize();
/*  */
/* RS : Create Array Variable, paramaters : (name, manglname, typr, basetype, address, sizeof(type), array size, fromMalloc, filename, linenr, linenrTransformed, dimension info ...) */
  rted_CreateArray(((struct rted_TypeDesc ){"SgArrayType", "SgTypeInt", {1, 3}}),rted_AddrSh(((shared char *)((shared char *)sh_solutions))),sizeof(sh_solutions),akUpcSharedGlobal,1L,0,((unsigned long []){1, 1UL * (THREADS ) + 0}),"sh_solutions","sh_solutions","",((struct rted_SourceInfo ){"/home/pirkelbauer2/sources/rose-bin/projects/RTED/nqueens.upc", 33, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&ROUND_ROBIN))),sizeof(ROUND_ROBIN),1,akGlobal,"ROUND_ROBIN","ROUND_ROBIN","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&N))),sizeof(N),0,akGlobal,"N","N","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&level))),sizeof(level),0,akGlobal,"level","level","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&no_solutions))),sizeof(no_solutions),1,akGlobal,"no_solutions","no_solutions","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&method))),sizeof(method),1,akGlobal,"method","method","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&basemask))),sizeof(basemask),0,akGlobal,"basemask","basemask","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&leftdiagmask))),sizeof(leftdiagmask),0,akGlobal,"leftdiagmask","leftdiagmask","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&centermask))),sizeof(centermask),0,akGlobal,"centermask","centermask","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
/*  */
/* RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed) */
  rted_CreateVariable(((struct rted_TypeDesc ){"SgTypedefType", "", {0, 0}}),rted_Addr(((char *)(&rightdiagmask))),sizeof(rightdiagmask),0,akGlobal,"rightdiagmask","rightdiagmask","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
  int exit_code = RuntimeSystem_original_main(argc,argv,envp);
  rted_Close("RuntimeSystem.cpp:main");
  return exit_code;
}
