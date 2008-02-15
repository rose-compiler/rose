//**********************************************************
//   CROSSREF.HPP
//   Coco/R C++ Taste Example.
//   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
//      from Moessenboeck's (1990) Oberon example
//
//   May 24, 1996  Version 1.06
//**********************************************************

#ifndef CROSSREF_H
#define CROSSREF_H

#include <stdio.h>

class List {
  private:
    int Number;
    List *Next;
  public:
    List(int n);
    ~List();
    void Print(FILE *outfile);
    List *GetNext();
    void SetNext(List *n);
    int  GetNum() { return Number; }
};

const int NameLength = 24;

class TreeNode {
  public:
    char Text[NameLength - 1];
    TreeNode *Left, *Right;
  private:
    int DefinedBy;
    int Okay;
    List *RefList, *RefLast;
  public:
    TreeNode(char *Name);
    ~TreeNode();
    void NewReference(int Reference);
    void IsDefined(int ref);
    void PrintSelf(FILE *outfile);
    void Print(FILE *outfile);
};

class RefTable {
  private:
    TreeNode *Tree;
  public:
    RefTable();     /* Initialise a new (empty) Table */
    ~RefTable();

    void Add(char Name[], int Reference, int Defining);
    /* Add Name to Table with given Reference, specifying whether this is a
       Defining (as opposed to an applied occurrence) */

    void Print(FILE *lst);
    /* List out cross reference Table on output device */
};

#endif // CROSSREF_H


