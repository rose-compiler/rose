//**********************************************************
//   TC.CPP
//   Coco/R C++ Taste Example.
//   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
//      from Moessenboeck's (1990) Oberon example
//   Thanks to Pat Terry for the C++ "Machine" class,
//   and for many improvements
//
//   May 24, 1996  Version 1.06
//   Jun 16, 1998  Version 1.08 (Minor changes)
//**********************************************************

#include "tc.hpp"
#include <stdio.h>
#include <stdlib.h>

Machine::Machine(CRError *E)
{
  pc = 1;
  generatingCode = 1;
  Error = E;
}

void Machine::Emit (int op)
{ 
  if (generatingCode) {
    if (pc >= MEMSIZE - 5) { Error->ReportError(125); generatingCode = 0; }
    else code[pc++] = op;
  }
}

void Machine::Emit2 (int op, int val)
{
  if (generatingCode) {
    Emit(op);
    code[pc++] = val / 256;
    code[pc++] = val % 256;
  }
}

void Machine::Emit3 (int op, int level, int val)
{
  if (generatingCode) {
    Emit(op);
    code[pc++] = level;
    code[pc++] = val / 256;
    code[pc++] = val % 256;
  }
}

void Machine::Fixup (int adr)
{
  if (generatingCode) {
    code[adr++] = pc / 256;
    code[adr++] = pc % 256;
  }
}

int Machine::Next()
{
  pc++;
  return code[pc-1];
}

int Machine::Next2()
{
  int x, y;
  x = code[pc++];
  y = code[pc++];
  return x * 256 + y;
}

void Machine::Push(int val)
{ 
  stack[top++] = val;
}

int Machine::Pop()
{  
  top--;
  return stack[top];
}

int Machine::Up(int level)
{
  int b = base;
  while (level > 0) { b = stack[b]; level--; }
  return b;
}

void Machine::Interpret()
{
  int val, a, lev;

  printf("Interpreting\n");
  pc = progStart;
  base = 0;
  top = 3;
  while (1) {
    switch (Next()) {
    case LOAD:
      lev = Next();
      a = Next2();
      Push(stack[Up(lev) + a]);
      break;
    case LIT:
      Push(Next2());
      break;
    case STO:
      lev = Next();
      a = Next2();
      stack[Up(lev) + a] = Pop();
      break;
    case ADD:
      val = Pop(); Push(Pop() + val);
      break;
    case SUB:
      val = Pop(); Push(Pop() - val);
      break;
    case MUL:
      val = Pop(); Push(Pop() * val);
      break;
    case DIVI:
      val = Pop();
      if (!val) { printf("Divide by zero\n"); exit(1); }
      Push(Pop() / val);
      break;
    case EQU:
      val = Pop(); Push(Pop() == val);
      break;
    case LSS:
      val = Pop(); Push(Pop() < val);
      break;
    case GTR:
      val = Pop(); Push(Pop() > val);
      break;
    case CALL:
      Push(Up(Next()));
      Push(base);
      Push(pc + 2);
      pc = Next2();
      base = top - 3;
      break;
    case RET:
      top = base;
      base = stack[top+1];
      pc = stack[top+2];
      break;
    case RES:
      top += Next2();
      break;
    case JMP:
      pc = Next2();
      break;
    case FJMP:
      a = Next2();
      if (Pop() == 0) pc = a;
      break;
    case HALTc:
      return;
    case NEG:
      Push(-Pop());
      break;
    case READ:
      lev = Next();
      a = Next2();
      printf("? ");
      scanf("%d", &val);
      stack[Up(lev) + a] = val;
      break;
    case WRITE:
      printf("%d\n", Pop());
      break;
    default:
      exit(1);
    }
  }
}

