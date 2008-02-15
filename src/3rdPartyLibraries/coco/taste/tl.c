/**********************************************************
**   TL.C
**   Coco/R C Taste Example.
**   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
**      from Moessenboeck's (1990) Oberon example
**
**   May 24, 1996  Version 1.06
**   Jun 16, 1998  Version 1.08 (Minor changes)
**********************************************************/

#include "tl.h"
#include <stdlib.h>
#include <string.h>

Object undefObj;   /* object node for erroneous symbols */
int curLevel = 0;  /* nesting level of current scope */

Object topScope = NULL;  /* topmost procedure scope */

extern SemError (int n);

void EnterScope(void)
{
  Object  scope;
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

void LeaveScope(void)
{ 
  topScope = topScope->next;
  curLevel--;
}

int DataSpace(void)
{ 
  return topScope->nextAdr - 3;
}

Object NewObj (char name[], int kind)
{
  Object obj, p;
  obj = (Objectnode *) malloc(sizeof(struct Objectnode));
  strcpy(obj->name, name);
  obj->type = UNDEF;
  obj->kind = kind;
  obj->level = curLevel;

  p = topScope->locals;
  while (p!=NULL) {
    if (strcmp(p->name, name) == 0) SemError(117);
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

Object Obj(char name[])
{
  Object  obj, scope;
  scope = topScope;

  while (scope != NULL) {
    obj = scope->locals;
    while (obj != NULL) {
      if (strcmp(obj->name, name) == 0) return obj;
      obj = obj->next;
    }
    scope = scope->next;
  }
  SemError(118);
  return undefObj;
}

void tl_init(void)
{
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


