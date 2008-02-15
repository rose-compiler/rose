README for Gary_Unparser by Gary Lee 08/03/99 

This document is to provide general information about the design and
features of the unparser. For detailed explainations of the logic
within each function, please refer to the comments embedded in each
file.

Files associated with the Gary_Unparser class
---------------------------------------------
Gary_Unparser.h
Gary_Unparser.C
modified_sage.C
unparse_stmt.C
unparse_expr.C

Files associated with the Options class
---------------------------------------
options.h
options.C

The general flow control of the Unparser
---------------------------------------- 
The unparser is created by instantiating an object of Gary_Unparser.
The unparser starts generating C++ source code once the run_unparser()
function is called. This function first tries to unparse any
directives at the beginning of the file. Once finished, it enters a
loop that calls unparseStatement(stmt, info) with the first statement
of the file. Everytime the unparseStatement is called, we first call
the unparseDirectives function and check whether there are any
directives to be unparsed. If the line number of the directive is less
than the line number of the next statement to be unparsed, then we
unparse the directive. The unparseStatement function then determines
the variant (or type) of statement and calls the corresponding unparse
function to handle that particular type of statement (this is done by
the switch statement). Some statements may contain other statements in
their body. For example, "if" statements have a true and false body
which may also have statements in them. In this case, the
unparseStatement function is called again to handle the statements in
the "if" statement. Thus, the unparsing of statements is a recursive
process. Statements within statements are unparsed recursively. Once
the whole statement is unparsed, we unparse the next statement until
there are no more statements left to unparse. In this case, we exit
the loop and unparse any final directives that may be at the end of
the file. Then the run of the unparser is finished.
 
Gary_Unparser.{h,C}
-------------------
The header file (Gary_Unparser.h) contains the declarations for all
the functions defined in all *.C files and the member data fields of
the class.  Gary_Unparser.C contains the implementation of the
constructors, destructor, formatting functions, functions that unparse
directives, and some auxiliary functions for formatting.

Formatting 
----------
There are two general functions for formatting. The format function
uses the information provided by the File_Info object from each Sage
statement and formats according to the statement's line and column
number of the input source file. If no file information is available
from the statement, then the formatting is handled by the
format_transformation function, which formats to a style that I have
specified. Transformations typically have no file information since
they are introduced and are not part of the original source file.

The format function uses the line and column number of the previous
statement or directive unparsed and the statement to be unparsed to
determine the number of newlines and spaces to insert in the unparsed
file before actually unparsing the statement. The number of newlines
to insert is determined by taking the line number of the statement to
be unparsed and subtracting the line number of the previous statement
or directive. Since the difference between these numbers is the number
of lines between the statements, we know how many newlines to
insert. The number of spaces to indent is simply determined by asking
for the column number of the statement. We insert as many spaces as
the column number. However, declaration statements are a special case
when determining how far to indent. The column information returned
for declarations is specified by the name of the declaration, which is
often preceded by modifiers, types, or class names. Thus, the
special_cases auxiliary function determines the length that precedes
the name of the declaration and subtracts this from the column number
of the declaration.  After subtracting, we have the number of spaces
that we should indent.

The format_transformation function formats without file information. I
have specified my own formatting scheme by beginning each statement on
the next line. Thus, no more than one statement can be on one
line. This also means that no statement is more than one line apart
since I only insert one newline after each statement. The line
information of the previous node is set to be the line after the
statement that was just unparsed. This information is used to unparse
the next statement. The number of spaces to indent is determined by
the depth of the basic block. For every depth, I add TABINDENT
(defined in Gary_Unparser.h) spaces before unparsing. Once a basic
block is entered, all statements in the block are indented by the
number indented for the basic block + TABINDENT. When a basic block is
exited, TABINDENT is subtracted for the next statement to be
unparsed. NOTE: Introduced comments as part of the transformation are
not unparsed because of Sage's inability to recognize comments.

Unparsing Directives 
-------------------- 
The function unparseDirectives is called before each statement is
unparsed. A directive is unparsed if its line number is less than the
line number of the statement to be unparsed. As long as this is true,
each directive that precedes the statement will be unparsed before the
next statement. The directives are stored in a list of class
ROSEAttributesList, which contains all the directives in the input
source file. This list is traversed as each directive is
unparsed. Once we reach the end of the list, there are no more
directives to unparse.

Like statements, directives must also be formatted. The algorithm used
to format directives is very similar to the one used for
statements. We use the line number from the previous statement or
directive unparsed and subtract from the line number of the directive
to be unparsed. This determines the number of newlines to
insert. Determining how many spaces to indent is currently handled by
exception handling, since the information of the directive's column
number is not provided. A reasonable heuristic that I have implemented
uses the column number of the previous statement or directive unparsed
as the directive's own column number. 

Another case with exception handling is forward declarations. If the
next statement to be unparsed is a forward declaration, then we cannot
proceed as normal because the line number of the forward declaration
is actually the line number of where the declaration is defined. In
this case, if the definition resides at a line number that is much
greater than the current directive line number, then all the
directives before the definition of the declaration will be
unparsed. Remember that we unparse a directive as long as the line
number of the directive is less than the line number of the
statement. To prevent this from happening, the line number of the
forward declaration is set to be one after the line number after the
previous statement unparsed. Once forward declarations can provide us
with correct the line number, the exception handling will not be
needed.

The unparseFinalDirectives function is like the unparseDirectives 
function except that it unparses directives only after all the 
statements in the file have been unparsed. 

modified_sage.C
---------------
This C++ file contains the general unparse function for expressions,
functions that test for operator overloaded functions, several
auxiliary functions to help the printing of operator overloaded
functions, and some helper unparse functions.

Operator Overloading
--------------------
The printing of operator overloaded functions is actually handled
quite rigourously by the unparser since there are many cases to
consider.  An option defined in the Options class determines whether
the operator overloaded function should be printed fully
(i.e. "operator+") or primitive form (i.e. "+"). The unparser provided
by Sage automatically unparses operator overloaded functions as the
former, but with the option of printing them as the latter, several
auxiliary functions are defined in this file to help determine the
different kinds of operator overloaded functions (i.e. unary or
binary, unary prefix or postfix) and the correct precedence to
determine whether parenthesis are needed between these operators. The
algorithm that controls the printing of operator overloaded functions
will be explained in more detail under the unparse_expr.C section of
this README.

Auxiliary Unparse Functions
---------------------------
The unparse_helper function is called whenever function parameters are
unparsed in a function declaration. For functions that have many
parameters, the number of characters printed on a line is kept to
prevent line wrapping. The length of each parameter is added
cumulatively until the MAXCHARSONLINE limit is reached. Then a newline
is added to start unparsing more parameters on the next line. This
feature is also used to prevent line wrapping for long statements.

The printSpecifier function prints out certain keywords that may
precede a declaration statement. The functions printSpecifier1 and
printSpecifier2 are just the two halves from printSpecifier.
printSpecifier1 prints out "private," "protected," or "public" if
necessary. printSpecifier2 prints out modifiers and keywords in front
of the declarations. The reason for creating two functions is to
prevent the printing of "private", "protected", or "public" after we
have formatted (inserted the correct number of newlines and spaces for
the declaration) the statement. printSpecifier is normally called
after the format function is called in every statement unparse
function. If "private", "protected", or "public" is to be printed,
then the resulting unparsed file would have these keywords formatted
on the same line as the declaration itself. To prevent this,
printSpecifier1 is called before the format function. Then
printSpecifier2 is called after the format function to unparse any
keywords that may precede before the declaration.

unparse_stmt.C
--------------
This C++ file contains the unparse functions for all kinds of
statements.  Each function first checks that the statement is in the
input source file before calling the format function because the file
information from other files is not wanted to format the statements
correctly. If the statement is not in the input source file, then the
format_transformations function is called to format the statement in a
design that I have specified. The check is as follows:

if (!strcmp(getFileName(stmt), getFileName()) && 
    !opt.get_forced_transformation_opt()) {

  format(stmt);
  ...
  ...
}
else format_transformations(stmt, info);

opt.get_forced_transformation_opt() will be explained under the
description of the Options class. 

Once formatted, each function unparses the statement differently,
depending on the type of statement. There are two added unparse
functions, one to handle "else if" cases, and the other to handle
"where" statements. Each of these functions is preceded by a
description of how they are unparsed.

Forward Declaration Exception Handling
--------------------------------------
Forward declarations, which were briefly explained earlier, do not
have correct file information. The line and column number of a forward
declaration refers to where the definition of the declaration
resides. However, if the definition does not reside in the same file
as the declaration, then the file information provided is
correct. This is most common with header files. File information is
also correct if the definition of a member declaration follows
immediately in a class definition. This is the only case in which
correct file information is provided when both the declaration and
definition reside in the same file. If the definition of member
declarations are defined outside the class in the same file, then the
line and column number erroneously refer to the definition. To handle
forward declarations, the type of file is first determined with the
following check:

if (strstr(mfuncdecl_stmt->get_file_info()->get_filename(), ".C") != NULL) {
  ...exception handling...
}
 
The function strstr returns NULL if there is no instance of ".C" in
the filename. Thus, only C files will use the exception handler to
unparse forward declarations. Header files will not use exception
handling.

RESTRICTION: If we are unparsing a header file, then exception
handling is not used. In most cases, header files only contain
declarations of classes and functions, so correct file information is
provided. The unparser depends on this notion to unparse the header
files correctly. If the unparser is given a header file where
definitions are declared outside the class declaration, then the
unparser will fail to unparse correctly. 

The general solution of the exception handler is to set the line
number of the forward declaration one after the previous statement
(prevnode) or directive unparsed (prevdir_line_num). This prevents a
massive number of newlines being inserted or directives being unparsed
before they should.

Prevention of Line Wrapping 
---------------------------
Exception Handling for bad file information in Sage expressions

The motivation to prevent linewrapping comes from unparsing statements
that are enormously long. This problem stems from the fact that the
file information provided by expressions is useless for formatting. If
expressions could be formatted, then the original position of the
expressions could also be preserved like statements. Two member fields
of the Gary_Unparser class handle the prevention of line wrapping:
chars_on_line and global_indent. The idea is to insert a newline at an
appropriate place to break a statement when the number of characters
unparsed (chars_on_line) on a line exceeds the MAXCHARSONLINE variable
(defined in Gary_Unparser.h). When something is unparsed, the length
of the unparsed is added to chars_on_line. chars_on_line is reset to 0
whenever a newline is inserted. global_indent is used to determine how
far to indent once a newline is inserted to prevent line wrapping. The
value of global_indent is determined by the insert_spaces function
because the argument given to insert_spaces is the amount indented for
the statement. global_indent is set to the value of this argument
whenever indent_spaces is called.

The linewrap_newline member field just keeps track on whether the last
statement we unparsed required line breaks.  If so, then we insert a
newline for the next statement to be unparsed if the line number of
the statement is less than or equal to the line number of the previous
statement. In normal cases, the statement would be unparsed on the
same line as the previous since no newlines are needed.  But the
reason the statement line is now less than the previous statement line
is that the number of newlines we inserted exceeded the amount of
linebreaks that was probably in the original file (remember that
everytime a newline is inserted, the previous line number is
incremented). In this case, it is not appropriate to unparse the
statement on the same line since the original source code most likely
had the two statements on different lines.

Newlines are added (only if the number of characters on a line exceeds
MAXCHARSONLINE) after unparsing a unary or binary operator and after
unparsing a comma in an argument list.

unparse_expr.C
--------------
This C++ file contains the unparse functions for all kinds of
expressions. 

Operator Overloading (revisted) 
-------------------------------
NOTE: The following section describes how operator overloaded functions
are unparsed when the overload_opt is FALSE.

Operator Overloading is mainly dealt by two functions:
unparseBinaryExpr and unparseFuncCall. unparseBinaryExpr deals with
unparsing member operator overloaded functions while unparseFuncCall
unparses friend or non-member operator overloaded functions.  All
operator overloaded function calls are represented as
SgFunctionCallExp (which calls the unparseFuncCall function). The
unparseFuncCall function is divided into two parts. The first part
handles friend or non-member operator overloaded functions. The second
part handles member operator overloaded functions and calls
unparseBinaryExpr. A check is performed to decided which part will be
executed to unparse the function. If the function is a unary operator,
dot or arrow expression (which represents all member functions), or a
non-operator overloaded function, execution is transferred to the
second part. Otherwise, all non-member operator overloaded functions
are handled in the first part.

The first part uses an in-order tree traversal method through the
program tree to unparse the operands of the operator in the right
order. The original Sage unparser prints operator overloaded functions
exactly like non-operator function calls. The function name is
unparsed first, followed by the arguments. Operator overloaded
functions are unparsed similarly. The operator (function name) is
printed first and the operands (arguments) are printed after the
operator. For example, "x + y" is unparsed as "operator+(x,y)." Since
the goal is to unparse "x + y" as "x + y", the left operand is
unparsed first, then the operator, and finally the right
operand. Thus, I used a in-order tree traversal method, which first
unparses everything on the left side of the operator, the operator,
and everything on the right side of the operator. The unparse calls on
the left and right side of the operator are recursive (call the
unparseFuncCall function) to handle any level of operator overloaded
functions nested within operator overloaded functions.

The second part takes care of member operator overloaded
functions. These functions are represented as the right side of a
SgDotExp or SgArrowExp. Since a SgDotExp or SgArrowExp is a binary
expression, the unparseBinaryExpr function is called. When the left
operand, operator, and right operand are finished unparsing, execution
is returned back to the unparseFuncCall function. Then the arguments
of the function are unparsed. Sage originally prints member operator
overloaded functions such that "x = y" is unparsed as "x.operator=(y)"
The "." is the operator, x and operator= are the left and right
operands and y is the function argument. The printing of the "." and
"operator" are suppressed when unparsing to produce "x = y". The same
logic follows for unary operators and other binary operators.

Printing of Parenthesis 
----------------------- 
With the option to suppress the printing of operator overloaded
functions, many parenthesis that were once necessary are now not
needed. As a result, many nested combinations of operator overloaded
functions appear to have excessive parenthesis printed around them,
creating cluttered code. To alleviate the problem, the solution Sage
uses to determine the printing of parenthesis around primitive
operators is used. The precedence information provided by every
expression is used to determine where parenthesis are needed to unparse
the correct relationship between operators. Because precedence
information is only stored in primitive operators, I needed to provide
the same information for operator overloaded functions. The
binary_precedence function supplies this information and is called in
the unparseFuncCall function (where the comparison of precedence is
made between operators).

The option of not printing "this" in front of member data fields and
functions also creates unnecessary parenthesis. The parenthesis that
surround "this", the arrow operator, and the member data field or
function (for example, "(this->data)") produces "(data)" when the
option is FALSE. A check is made in the unparseBinaryExpr function to
suppress the parenthesis when the this option is FALSE and the lhs of
the binary expression is a SgThisExp (the boolean this_op monitors the
check).

The parenthesis printed from the unparseExprList function are also
unnecessary in most cases, but not all. This unparse function is most
often called by the unparseConInit function. Parenthesis are necessary
when unparsing the argument list of a constructor in a variable
declaration statement. Except for the case mentioned above,
parenthesis are not necessary around expression lists with only one
element (or constructor initializers with only one argument). The
isOneElementList auxiliary function checks if the argument list of the
constructor only has one element. If so, parenthesis are not needed
around the individual arguments of a function call (when unparsing the
function call's arguments in unparseFuncCall) and around an array
reference (the RemovePareninExprList function).

options.{C,h} 
------------- 
The Options class controls several aspects of the unparsing. An
Options object is created either by the default constructor (which
turns off (FALSE) all options), or by the constructor that allows one
to specify which options are turned on and off. Once created, the
Options object is passed in as a parameter of the Gary_Unparser
constructor.

Description of each option
--------------------------
1. auto_opt: TRUE- prints "auto" in front of variable declarations
             FALSE- suppresses "auto" in front of variable
             declarations 

2. linefile_opt: TRUE- prints line and file information (by
		 Gary_Unparser::output) as pragmas after each statement
		 FALSE- does not print line and file information

3. overload_opt: TRUE- prints out function call of overloaded
		 operators for all non-primitive types. For example,
		 prints out "operator+" instead of "+."
		 FALSE- only prints the operator. For example, "++"
		 instead of "operator++"
	     
4. boolnum_opt: TRUE- prints booleans as numbers (0 and 1) 
		FALSE- prints booleans as words ("FALSE" and "TRUE")

5. this_opt: TRUE- prints "this" in front of data or function members
	     FALSE- suppresses "this" in front of data or function
	     members (Exception: There are cases when "this" must be
	     printed while the option is off.)

6. caststring_opt: TRUE- prints "const char*" in front of strings
		   FALSE- suppresses "const char*" in front of strings

7. forced_transformation_format:
		TRUE- formats the unparsed code in a design that I have
		specified (without the use of file information)
		FALSE- formats the unparsed code by using the file 
		information provided by each statement 

RESTRICTION: Option 7 only works well without comments embedded in the
source code. Because line numbers of statements are set as each
statement is unparsed (the format_transformation algorithm), the
original line information of each statement is overwritten. However,
the original line numbers for comments are still preserved. As a
result, the original line number differences between statements and
comments are lost and extra newlines are inserted in most cases.
Nevertheless, this option was built only for the purposes of
simulating transformations of every kind of statement since currently,
the transformations are only introduced with variable declaration and
for statements. I set this option TRUE to purposely not use the file
information provided and to aid me in implementing the
format_transformations function for every kind of statement.

Printing of Debugging Information 
---------------------------------
Debugging information is printed to standard output by calling the
function printdebugInfo. The function printDebugInfo takes either a
literal string or integer and a boolean (to indicate whether to insert
a newline) as arguments. If the DEBUG variable is defined in
Gary_Unparser.h, then all calls to printDebugInfo output debugging
information to standard output (cout). No debugging information is
printed if the DEBUG variable is not defined.
