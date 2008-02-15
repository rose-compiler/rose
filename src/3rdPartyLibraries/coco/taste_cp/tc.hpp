//**********************************************************
//   TC.HPP
//   Coco/R C++ Taste Example.
//   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
//      from Moessenboeck's (1990) Oberon example
//   Thanks to Pat Terry for the C++ "Machine" Class.
//
//   May 24, 1996  Version 1.06
//   Jun 16, 1998  Version 1.08 (Minor changes)
//**********************************************************

#include "cr_error.hpp"

#define MEMSIZE 15000

class Machine {
  public:
    Machine(CRError *E);
    void Emit(int op);
    void Emit2(int op, int val);
    void Emit3(int op, int level, int adr);
    void Fixup(int adr);
    void Interpret();
    int progStart;     /* address of first instruction of main program */
    int pc;            /* program counter */

  private:
    unsigned char code[MEMSIZE];
    int generatingCode;
    int stack[1000];
    int top;
    int base;
    int Next();
    int Next2();
    void Push(int val);
    int Pop();
    int Up(int level);
    CRError *Error;
};

/* operators */

#define PLUS   0
#define MINUS  1
#define TIMES  2
#define SLASH  3
#define EQU    4
#define LSS    5
#define GTR    6

/* opcodes */

#define ADD    0
#define SUB    1
#define MUL    2
#define DIVI   3
#define EQU    4
#define LSS    5
#define GTR    6
#define LOAD   7
#define LIT    8
#define STO    9
#define CALL  10
#define RET   11
#define RES   12
#define JMP   13
#define FJMP  14
#define HALTc 15
#define NEG   16
#define READ  17
#define WRITE 18


