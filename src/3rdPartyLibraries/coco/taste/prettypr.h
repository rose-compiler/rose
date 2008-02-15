/**********************************************************
**   PRETTYPR.H
**   Coco/R C Taste Example.
**   Adapted to C++ by Frankie Arzu <farzu@uvg.edu.gt>
**      from Moessenboeck's (1990) Oberon example
**
**   May 24, 1996  Version 1.06
**********************************************************/

void Append(char String[]);
/* Append String to results file */

void IndentNextLine(void);
/* Write line mark to results file and prepare to indent further lines
   by two spaces more than before */

void ExdentNextLine(void);
/* Write line mark to results file and prepare to indent further lines
   by two spaces less */

void NewLine(void);
/* Write line mark to results file but leave indentation as before */

void Indent(void);
void Exdent(void);


