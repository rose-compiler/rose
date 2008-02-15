//**********************************************************
//   CROSSREF.CPP
//   Coco/R C++ Taste Example.
//   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
//      from Moessenboeck's (1990) Oberon example
//
//   May 24, 1996  Version 1.06
//**********************************************************

#include <stdlib.h>
#include <string.h>
#include "crossref.hpp"

TreeNode::TreeNode(char *Name)
{
  RefList = RefLast = NULL;
  Left = NULL;
  Right = NULL;
  Okay = 0;
  DefinedBy = 0;
  strcpy(Text, Name);
}

TreeNode::~TreeNode()
{
  if (RefList) delete(RefList);
  if (Left) delete(Left);
  if (Right) delete(Right);
}

void TreeNode::IsDefined(int ref)
{
  if (DefinedBy == 0) {
    DefinedBy = ref;
    Okay = 1;
  } else Okay = 0; /* redefined */
}

void TreeNode::NewReference(int Reference)
{
  List *L;
  L = new List(Reference);
  if (RefLast) {
    if (RefLast->GetNum() != Reference) { RefLast->SetNext(L); RefLast = L; }
  } else RefList = RefLast = L;

}

void TreeNode::PrintSelf(FILE *outfile)
{
  List *Current;
  int i;

  fprintf(outfile, "%s", Text);
  i = strlen(Text);
  while (i++ <= 16) fprintf(outfile, " ");
  if (!Okay) fprintf(outfile, "?"); else fprintf(outfile, " ");
  fprintf(outfile, "%4d-", DefinedBy);
  if (RefList) {
    Current = RefList; i = 0;
    while (Current) {
      Current->Print(outfile);
      i++; if (i % 12 == 0) fprintf(outfile, "\n                    ");
      Current = Current->GetNext();
    }
  }
  fprintf(outfile, "\n");
}

void TreeNode::Print(FILE *f)
{
  if (Left) Left->Print(f);
  PrintSelf(f);
  if (Right) Right->Print(f);
}

List::List(int n)
{
  Number = n;
  Next = NULL;
}

List::~List()
{
  if (Next) delete Next;
}

void List::Print(FILE *outfile)
{
  fprintf(outfile, "%4d", Number);
}

List *List::GetNext()
{
  return Next;
}

void List::SetNext(List *n)
{
  Next = n;
}

RefTable::RefTable()
{
  Tree = NULL;
}

RefTable::~RefTable()
{
  if (Tree) delete Tree;
}

static void AddToTree (TreeNode * &Root, char Name[],
                       int Reference, int Defined)
{
  if (Root == NULL) {
    /* at a leaf - Name must now be inserted */
    Root = new TreeNode(Name);
    if (Defined == 1) Root->IsDefined(Reference);
    else Root->NewReference(Reference);
  }
  else if (strcmp(Name, Root->Text) > 0)
    AddToTree(Root->Right, Name, Reference, Defined);
  else if (strcmp(Name, Root->Text) < 0)
    AddToTree(Root->Left, Name, Reference, Defined);
  else {
    if (Defined) Root->IsDefined(Reference);
    else Root->NewReference(Reference);
  }
}

void RefTable::Add(char Name[], int Reference, int Defined)
{
  AddToTree(Tree, Name, Reference, Defined);
}

void RefTable::Print(FILE *lst)
{
  fprintf(lst, "\n\n");
  if (Tree) Tree->Print(lst);
}


