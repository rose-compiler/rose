/**********************************************************
**   PRETTYPR.C
**   Coco/R C Taste Example.
**   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
**      from Moessenboeck's (1990) Oberon example
**
**   May 24, 1996  Version 1.06
**********************************************************/

#include "prettypr.h"
#include <stdio.h>

static int Indentation = 0;

void Append (char String[])
{  
  printf("%s",String);
}

void NewLine(void)
{
  int i = 1;
  printf("\n");
  while (i <= Indentation) { printf(" "); i++; }
}

void IndentNextLine(void)
{ 
  Indentation += 2;
  NewLine();
}

void ExdentNextLine(void)
{  
  Indentation -= 2;
  NewLine();
}

void Indent(void)
{ 
  Indentation += 2;
}

void Exdent(void)
{ 
  Indentation -= 2;
}


