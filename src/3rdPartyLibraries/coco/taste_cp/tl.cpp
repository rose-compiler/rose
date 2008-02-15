//**********************************************************
//   TL.CPP
//   Coco/R C++ Taste Example.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   May 24, 1996  Version 1.06
//   Jun 16, 1998  Version 1.08 (Minor changes)
//**********************************************************

#include "tl.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

SymTable::SymTable(CRError *E)
{
  Error = E;
  topScope = NULL;
  curLevel = 0;
  undefObj = (Objectnode *) malloc(sizeof(struct Objectnode));
  strcpy(undefObj->name, "");
  undefObj->type = UNDEF;
  undefObj->kind = VARS;
  undefObj->adr = 0;
  undefObj->level = 0;
  undefObj->next = NULL;
}

void SymTable::EnterScope()
{
  Object scope;
  scope = (Objectnode *) malloc(sizeof(struct Objectnode));
  strcpy(scope->name, "");
  scope->type = UNDEF;
  scope->kind = SCOPES;
  scope->locals = NULL;
  scope->nextAdr = 3;
  scope->next = topScope;
  topScope = scope;
  curLevel++;
}

void SymTable::LeaveScope()
{ 
  topScope = topScope->next;
  curLevel--;
}

int SymTable::DataSpace ()
{ 
  return topScope->nextAdr - 3;
}

Object SymTable::NewObj (char name[], int kind)
{
  Object obj, p;
  obj = (Objectnode *) malloc(sizeof(struct Objectnode));
  strcpy(obj->name, name);
  obj->type = UNDEF;
  obj->kind = kind;
  obj->level = curLevel;

  p = topScope->locals;
  while (p!=NULL) {
    if (strcmp(p->name, name) == 0) Error->ReportError(117);
    p = p->next;
  }
  obj->next = topScope->locals;
  topScope->locals = obj;
  if (kind == VARS) {
    obj->adr = topScope->nextAdr;
    topScope->nextAdr++;
  }
  return obj;
}

Object SymTable::Obj(char name[])
{
  Object obj, scope;
  scope = topScope;

  while (scope != NULL) {
    obj = scope->locals;
    while (obj != NULL) {
      if (strcmp(obj->name, name) == 0) return obj;
      obj = obj->next;
    }
    scope = scope->next;
  }
  Error->ReportError(118);
  return undefObj;
}

