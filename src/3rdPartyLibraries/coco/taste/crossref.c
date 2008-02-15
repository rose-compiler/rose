/**********************************************************
**   CROSSREF.C
**   Coco/R C Taste Example.
**   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
**      from Moessenboeck's (1990) Oberon example
**
**   May 24, 1996  Version 1.06
**********************************************************/

#include <stdlib.h>
#include <string.h>
#include "crossref.h"

static TABLES Table;
FILE *outfile;

void Create(void)
{
  Table=NULL;
}

static void NewReference(TABLES Leaf, int Reference)
{
  QUEUES Latest;

  Latest = (struct REFS *) malloc(sizeof(struct REFS));
  Latest->Number = Reference;
  if (Leaf->RefList == NULL) Latest->Next = Latest;
  else {
    Latest->Next = Leaf->RefList->Next;
    Leaf->RefList->Next = Latest;
  }
  Leaf->RefList = Latest;
}

TABLES AddToTree(TABLES Root, char Name[], int Reference, int Defined)
{
  if (Root == NULL) {
    /*at a leaf - Name must now be inserted*/
    Root = (struct NODES *) malloc(sizeof(struct NODES));
    strcpy(Root->Text, Name);
    Root->LeftTree = NULL;
    Root->RightTree = NULL;
    Root->Okay = 0;
    Root->RefList = NULL;
    if (Defined == 1) {
      Root->DefinedBy = Reference;
      Root->Okay = 1;
    }
    if (Defined == 0) {
      Root->DefinedBy = 0;
      NewReference(Root, Reference);
      Root->Okay = 0;
    }
  }
  else if (strcmp(Name, Root->Text) > 0)
    Root->RightTree = AddToTree(Root->RightTree, Name, Reference, Defined);
  else if (strcmp(Name, Root->Text) < 0)
    Root->LeftTree = AddToTree(Root->LeftTree, Name, Reference, Defined);
  else {
    if (Defined) {
      if (Root->DefinedBy == 0) {
        Root->DefinedBy = Reference;
        Root->Okay = 1;
      }
      else {
        Root->Okay = 0;
      } /*redefined*/
    } else
      if (Defined == 0) {
        if (Root->RefList == NULL || Reference != Root->RefList->Number)
          NewReference(Root, Reference);
      }
  }
  return Root;
}

void Add(char Name[], int  Reference, int Defined)
{
  Table = AddToTree(Table, Name, Reference, Defined);
}

static void OneEntry(TABLES ThisNode)
{
  QUEUES First, Current;
  int I, J, L;

  I = 0;
  fprintf(outfile, "%s", ThisNode->Text);
  L = strlen(ThisNode->Text);
  while (L <= 16) {
    fprintf(outfile, " ");
    L++;
  }
  if (!ThisNode->Okay) fprintf(outfile, "?");
  else fprintf(outfile, " ");
  fprintf(outfile, "%4d-", ThisNode->DefinedBy);
  if (ThisNode->RefList != NULL) {
    First = ThisNode->RefList->Next;
    Current = First;
    do {
      fprintf(outfile, "%4d", Current->Number);
      Current = Current->Next;
      I++;
      if (I % 12 == 0) fprintf(outfile, "\n                    ");
    } while (Current != First); /* again */
  }
  fprintf(outfile, "\n");
}

static void ListT(TABLES Table)
{
  if (Table != NULL) {
    ListT(Table->LeftTree);
    OneEntry(Table);
    ListT(Table->RightTree);
  }
}

void List(FILE *lst)
{
  outfile = lst;
  fprintf(outfile, "\n\n");
  ListT(Table);
}


