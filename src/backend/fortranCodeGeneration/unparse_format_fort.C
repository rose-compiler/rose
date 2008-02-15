/* unparse_format_fort.C
 *
 * Formatting control for Fortran unparser
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "unparse_format_fort.h"

#include <iomanip>


int UnparseFormatFort::DefaultMaxLineLength = 78;

int UnparseFormatFort::DefaultTabIndent = 2;
int UnparseFormatFort::DefaultMaxIndent = 60;

int UnparseFormatFort::DefaultLineContIndent = 4;

int UnparseFormatFort::MaxDblDigitsDisplay = 32;

int UnparseFormatFort::IntBufferSize = 64; // easily holds log(2^128)
int UnparseFormatFort::DblBufferSize = 512;

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

UnparseFormatFort::UnparseFormatFort(ostream* nos, UnparseFormatHelp *_h)
{
  os = nos;
  os->precision(16);
  h = _h;

  curLine = 1;
  curColumn = 0;
  
  curIndent = 0;
  curStmtIndent = 0;
  
  maxLineLength = DefaultMaxLineLength;
  maxIndent = DefaultMaxIndent;
  
  prevnode = 0;
}

UnparseFormatFort::~UnparseFormatFort()
{
  os->flush(); // flush all pending writes
  delete h;
}


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

void
UnparseFormatFort::format(SgLocatedNode* node, 
			  SgUnparse_Info& info, FormatOpt opt)
{
  // See comments at declaration.
  
  VariantT v = node->variantT();
  VariantT v1 = (prevnode) ? prevnode->variantT() : (VariantT)0;
  
  // is this a true program header (and not simply one within an INTERFACE).
  bool isProgHeader = ((v1 != V_SgInterfaceStatement) 
		       && (v == V_SgProgramHeaderStatement 
			   || v == V_SgProcedureHeaderStatement
			   || v == V_SgModuleStatement));
		       
  // was the previous statement a compound statement, i.e. did it have
  // a BasicBlock within it
  bool wasCompound = 
    (v1 == V_SgProgramHeaderStatement 
     || v1 == V_SgProcedureHeaderStatement
     || v1 == V_SgModuleStatement
     || v1 == V_SgInterfaceStatement
     || v1 == V_SgIfStmt
     || v1 == V_SgFortranDo || v1 == V_SgWhileStmt
     || v1 == V_SgSwitchStatement);
  
  switch (opt) {
    
  case FORMAT_BEFORE_STMT:
    // short circuit certain nodes
    if (v == V_SgFile || v == V_SgGlobal || v == V_SgBasicBlock) {
      break;
    }
    // short circuit 'statements' that are really expressions
    // (e.g. conditional within an IF)
    if (info.inConditional()) {
      break;
    }
    
    prevnode = node;
    // add additional spaces before routines
    if (isProgHeader) {
      insert_newline(1); 
    }
    // add newline after end of non-compound statements.  Compound
    // statements are handled by the basic block formatting.
    if (v1 != (VariantT)0 && !wasCompound) {
      insert_newline(1, -getUndoLineContinuationIndent());
    }
    break;
  case FORMAT_AFTER_STMT:
    // add additional space after routine
    if (isProgHeader) {
      insert_newline(1); 
    }
    os->flush();
    break;
    
  case FORMAT_BEFORE_DIRECTIVE:
    maxLineLength = -1;
    insert_newline(1);
    break;
  case FORMAT_AFTER_DIRECTIVE:
    maxLineLength = DefaultMaxLineLength;
    break;
    
  case FORMAT_BEFORE_BASIC_BLOCK1:
    break;
  case FORMAT_BEFORE_BASIC_BLOCK2:
    insert_newline(1, -getUndoLineContinuationIndent());
    indent(DefaultTabIndent);
    curStmtIndent += DefaultTabIndent;
    break;
    
  case FORMAT_AFTER_BASIC_BLOCK1: {
    int adjust = -getUndoLineContinuationIndent() - DefaultTabIndent;
    insert_newline(1, adjust);
    curStmtIndent -= DefaultTabIndent;
    break;
  }
  case FORMAT_AFTER_BASIC_BLOCK2:
    break;
    
  default:
    ROSE_ASSERT(false);
  }  
}

void
UnparseFormatFort::insert_newline(int num, int iamnt)
{
  // add lines
  for (int i = 0 ; i < num; i++) {
    (*os) << endl;
  }
  if (num > 0) {
    curLine += num;
    os->flush();
  }
  
  // indent and adjust indentation
  if (num > 0) {
    curColumn = 0;
    if (iamnt >= 0) {
      insert_space(curIndent); // current indent
      indent(iamnt);           // additional indent
    }
    else {
      // manually adjust indent
      curIndent += iamnt;
      if (curIndent < 0) { curIndent = 0; }
      insert_space(curIndent);
    }
  }
}

void
UnparseFormatFort::indent(int num)
{
  if (num > 0) {
    int proposedColumn = curColumn + num;
    int amnt = (proposedColumn > maxIndent) ? 0 : num;
    curIndent += amnt;
    insert_space(amnt);
  } 
}

void
UnparseFormatFort::insert_space(int num)
{
  for (int i = 0; i < num; i++) {
    (*os) << " ";
  }
  if (num > 0) {
    curColumn += num;
  }
}


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

UnparseFormatFort& 
UnparseFormatFort::operator<<(string x)
{
  fortput(x.c_str());
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(const char* const x)
{
  fortput(x);
  return *this;
}

UnparseFormatFort&
UnparseFormatFort::operator<<(char x)
{
  char val[] = "'c'";
  val[1] = x;
  fortput(val);
  return *this;
}

UnparseFormatFort&
UnparseFormatFort::operator<<(unsigned char x)
{
  unsigned char val[] = "'c'";
  val[1] = x;
  fortput((const char*)val);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(short x)
{
  char buf[IntBufferSize];
  int ret = snprintf(buf, IntBufferSize, "%hd", x);
  ROSE_ASSERT(ret >= 0 && ret < IntBufferSize);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(unsigned short x)
{
  char buf[IntBufferSize];
  int ret = snprintf(buf, IntBufferSize, "%hu", x);
  ROSE_ASSERT(ret >= 0 && ret < IntBufferSize);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(int x)
{
  char buf[IntBufferSize];
  int ret = snprintf(buf, IntBufferSize, "%d", x);
  ROSE_ASSERT(ret >= 0 && ret < IntBufferSize);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(unsigned int x)
{
  char buf[IntBufferSize];
  int ret = snprintf(buf, IntBufferSize, "%hu", x);
  ROSE_ASSERT(ret >= 0 && ret < IntBufferSize);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(long x)
{
  char buf[IntBufferSize];
  int ret = snprintf(buf, IntBufferSize, "%ld", x);
  ROSE_ASSERT(ret >= 0 && ret < IntBufferSize);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(unsigned long x)
{
  char buf[IntBufferSize];
  int ret = snprintf(buf, IntBufferSize, "%lu", x);
  ROSE_ASSERT(ret >= 0 && ret < IntBufferSize);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(long long x)
{
  char buf[IntBufferSize];
  int ret = snprintf(buf, IntBufferSize, "%ld", x);
  ROSE_ASSERT(ret >= 0 && ret < IntBufferSize);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(unsigned long long x)
{
  char buf[IntBufferSize];
  int ret = snprintf(buf, IntBufferSize, "%lu", x);
  ROSE_ASSERT(ret >= 0 && ret < IntBufferSize);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(float x)
{
  // Generate in non-exponential form unless this is a very large number.
  char buf[DblBufferSize];
  int ret = snprintf(buf, DblBufferSize, "%0.8f", x);
  ROSE_ASSERT(ret >= 0 && ret < DblBufferSize);

  if (strlen(buf) > MaxDblDigitsDisplay) {
    ret = snprintf(buf, DblBufferSize, "%8e", x);
  }
  ROSE_ASSERT(ret >= 0 && ret < DblBufferSize);

  removeTrailingZeros(buf);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(double x)
{
  // Generate in non-exponential form unless this is a very large number.
  char buf[DblBufferSize];
  int ret = snprintf(buf, DblBufferSize, "%0.15f", x);
  ROSE_ASSERT(ret >= 0 && ret < DblBufferSize);

  if (strlen(buf) > MaxDblDigitsDisplay) {
    ret = snprintf(buf, DblBufferSize, "%16e", x);
  }
  ROSE_ASSERT(ret >= 0 && ret < DblBufferSize);

  removeTrailingZeros(buf);
  fortput(buf);
  return *this;
}

UnparseFormatFort& 
UnparseFormatFort::operator<<(long double x)
{
  char buf[DblBufferSize];
  int ret = snprintf(buf, DblBufferSize, "%1.32lf", x);
  ROSE_ASSERT(ret >= 0 && ret < DblBufferSize);

  removeTrailingZeros(buf);
  fortput(buf);
  return *this;
}


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

void
UnparseFormatFort::fortput(const char* const x)
{
  if (!x) {
    return;
  }
  
  int len = strlen(x);
  bool fitsOnCurLine = ((curColumn + len) < maxLineLength);
  bool fitsOnNxtLine = 
    ((curIndent + DefaultLineContIndent + len) < maxLineLength);

  if (fitsOnCurLine) {
    // 'x' fits on the line
    (*os) << x;
    curColumn += len;
  }
  else if (!fitsOnCurLine && fitsOnNxtLine) {
    // 'x' won't fit on the line but it will fit on the next line
    (*os) << '&'; // no need to increment the column
    insert_newline(1, DefaultLineContIndent);
    (*os) << x;
    curColumn += len;
  }
  else {
    // 'x' must be split
    for (const char* p = x; (*p != '\0'); ++p) {
      if ( !((curColumn + 1) < maxLineLength) ) { // add 1 for '&'
	// split and continue the line
	(*os) << '&'; // no need to increment the column
	insert_newline(1, DefaultLineContIndent);
	(*os) << '&';
	curColumn++;
      }
      (*os) << (*p);
      curColumn++;
    }
  }
}

void
UnparseFormatFort::removeTrailingZeros(char* const inputString)
{
  // Supporting function for formating floating point numbers
  int i = strlen(inputString)-1;
  // replace trailing zero with a null character (string terminator)
  while ( (i > 0) && (inputString[i] == '0') ) {
    // Leave the trailing zero after the '.' (generate "2.0" rather
    // than "2.")  this makes the output easier to read and more clear
    // that it is a floating point number.
    if (inputString[i-1] != '.') {
      inputString[i] = '\0';
    }
    i--;
  }
}

int 
UnparseFormatFort::getUndoLineContinuationIndent()
{
  // if (curStmtIndent < curIndent) then a line has been continued and
  // we should adjust it.
  ROSE_ASSERT(curIndent >= curStmtIndent);
  return (curIndent - curStmtIndent);
}
