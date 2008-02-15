/**********************************************************
**   CROSSREF.H
**   Coco/R C Taste Example.
**   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
**      from Moessenboeck's (1990) Oberon example
**
**   May 24, 1996  Version 1.06
**********************************************************/

#include <stdio.h>
#define  NameLength  24

typedef struct NODES* TREES;
typedef TREES TABLES;
typedef struct REFS* QUEUES;

typedef struct NODES {
  char Text[NameLength - 1];
  TABLES LeftTree, RightTree;
  int DefinedBy;
  int Okay;
  QUEUES RefList;
} NODES;

typedef struct REFS {
  int Number;
  QUEUES Next;
} REFS;


void Create(void);
/* Initialise a new (empty) Table */

void Add(char Name[], int Reference, int Defining);
/* Add Name to Table with given Reference, specifying whether this is a
   Defining (as opposed to an applied occurrence) */

void List(FILE *lst);
/* List out cross reference Table on output device */


