/**********************************************************
**   TL.H
**   Coco/R C Taste Example.
**   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
**      from Moessenboeck's (1990) Oberon example
**
**   May 24, 1996  Version 1.06
**   Jun 16, 1998  Version 1.08 (Minor changes)
**********************************************************/

/* object kinds */

#define VARS   0
#define PROCS  1
#define SCOPES 2

/* types */

#define UNDEF  0
#define INT    1
#define BOOL   2

typedef struct Objectnode *Object;
typedef struct Objectnode {
  char name[15];      /* name of the object */
  int  type;          /* type of the object (undef for procs) */
  Object next;        /* to next object in same scope */

  int kind;
  int adr;            /* address in memory or start of proc */
  int level;          /* nesting level of declaration */

  Object locals;      /* to locally declared objects */
  int nextAdr;        /* next free address in this scope */
} Objectnode;

extern Object undefObj; /* object node for erroneous symbols */
extern int curLevel;    /* nesting level of current scope */

void tl_init(void);
void EnterScope(void);
void LeaveScope(void);
int  DataSpace(void);
Object NewObj(char name[], int kind);
Object Obj(char name[]);


