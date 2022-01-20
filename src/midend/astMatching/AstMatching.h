#ifndef AST_MATCHING_H
#define AST_MATCHING_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "matcherparser_decls.h"
#include "MatchOperation.h"
#include "RoseAst.h"
#include <list>
#include <set>

class SgNode;

class MatchOperation;

/* update documentation:
   make doxygen_docs -C build_tree/docs/Rose
   webbrowser build_tree/docs/Rose/ROSE_WebPages/ROSE_HTML_Reference/classAstMatching.html
*/

/**

The AstMatching class allows to specify arbitrary large patterns
to be matched on any subtree in the AST. The AstMatching class comes with its own internal parser.
The match expression is provided as a string and checked for syntax errors. 
Type names of AST nodes can be used to specify arbitrarily large
AST patterns. Additionally variables and some operators are available
to allow the specification of complex patterns and determine the address of AST nodes through values stored in variables.
Subtrees can also be
ignored in the matching by using '_'. The binary operator '|' allows
to combine different matching subexpressions into one
expression.

In the following example we match assignments with variables on both
sides, such as x=y, and assign the pointer to the respective AST node to variable $R. This variable can then be accessed by name in a C++ map to obtain the pointer.

@code
    AstMatching m;
    AstMatchingResult res=m.match("$R=AssignOp(SgVarRef,SgVarRef)",astRoot);
@endcode

The variable 'astRoot' is a pointer to some node in the AST. The matcher can be started at the root of the AST, but also at any other node of the AST.

In the above example all subtrees representing an assign operation
with two variables as operands would be matched. The dollar sign
denotes a variable. In the above example the pointers to the matched
subtrees are assigned to the variable $R. The result with all matched
assignments is stored in the variable res of type
AstMatchingResult. The matching result is a set of maps where each map
represents the results for one successful match and holds pairs of a
variable name and a pointer to the respective AST subtree. For example, if there are n assignments in a program, then there will be n maps, and in each map varibale "$R" can be accessed to get the respective pointer to the matched assignment operator. A match expression can have arbitrary many variables, e.g. we could also use AssignOp($LHS=SgVarRef,$RHS=SgVarRef) to obtain all pairs of LHS and RHS pointers of above assignment operators with two variables. More generally, we can also use "$R=AssignOp($LHS,$RHS)", to obtain all three pointers for each match. The pointer values to the respective AST nodes can be accessed by the variable name in the result map (see Section \ref matchresults for more details)

@section variables Variables

Variables are used to specify that pointers to matched subtrees are
stored in the matching result. An arbitrary number of variables can be
used and two forms of use are supported. A variable is denoted with a
leading dollar sign an arbitrary number of letters and underscores (a
single underscore is used as wildcard). A variable assignment notation
can be used to assign the pointers of a specified pattern to a
variable. For example, $R=SgAssignOp(SgVarRef,_,_) is matched with all
assignments which have a variable on the left hand side and some
expression on the right hand side. Alternatively we can also use
$R=SgAssignOp($X=SgVarRef,$Y=_) - in this case we also store a pointer
to the matched variable node and a pointer to the expression on the
rhs in the match result. For the expression $Y=_ we can also simply
write $Y as a shorthand, thus we can also use
$R=SgAssignOp($X=SgVarRef,$Y) instead.  The assignment of variables to
variables, such as $Z=$Y, is not allowed.

@section nullvalues Null-values

Null values can be explicitely matched by using "null" in a match
expression. For example $X=SgForStatement(_,_,_,_,null) would match
all SgForStatement-terms with the 5th argument being 0.

@section wildcard Wildcard '_' for matching any subtree

Wildcards for certain arguments allow for matching nodes with specific arity.
For example, if we use SgAssignOp($LHS,_) we can
match all assignment nodes in the AST, bind the left hand side to $LHS, but ignore the right hand side of the expression.
In other words, '_' allows for any node or null pointer value to be present. Note, patterns in those subtrees are still matched in contrast to when using the '#' operator (see \ref hashop).

The wildcard can also be used for the node name. For example, "$A=_(_,_)" matches all binary nodes. Note that matches are only included in the matching result if at least one variable is bound.

@section ellipseop Operator '..'

This operator can be used in match expressions to specify that an arbitrary
number of arguments can follow. For example we can use
SgBlock($First,..) to match the very first statement in a
SgBlock. Since SgBlocks can have arbitrary arity this is quite useful in
this respect. The operator '..' can only be used at most once when
specifying the arity of a node, but arbitrary often in a match
pattern, e.g. SgBlock(SgForStatement($Cond,..),..) is OK, but
SgBlock(_,..,_,..) is not.

@section orop Operator "|" 

This operator allows to combine multiple match expressions. For
example "SgAddOp($L,$R)|SgSubOp($L,$R)" will match either a SgAddOp
and bind pointers to its two children to $L and $R, or it will match
SgSubOp and bind $L and $R to the children of SgSubOp. Using the same variable multiple times allows to bind similar information in different nodes at different positions (e.g. the body in different loop constructs). 
 The operator '|' performs a short-circuit evaluation, thus,
matching is performed from left to right and the matching stops as soon as one of the patterns can be successfully matched.

@section hashop Operator '#'

Placement of operator '#' in a match expression allows to exclude arbitrary subtrees from applying the match operation in subsequent matches. I.e. the marked subtrees are not traversed. For example if we only want to match the for-statements at the outer most level, but no nested for statements, we can use:

@code
    $FOR=SgForStatement(_,_,_,#_)
@endcode

This matches only the outer for-statements, as the body (4th argument) is excluded from applying the match operator. Without '#' we would also match the inner loops.

@section examples Examples

<ol>
<li> match("$R=AssignOp(_,_)",astRoot);\n
  Match all assignment operators in an AST.

<li> match("$R=SgAssignOp(SgVarRefExp,SgIntVal),astRoot);\n
  Match all assignment operators with a variable on the lhs and an integer value on the rhs.
<li> match("$FORROOT=SgForStatement(_,_,_,#_)",astRoot);\n
  Match all outer most for loops, but no nested for-loops. The operator '#' ensures that the match expression is not applied on the AST representing the body of the for-statement (4th argument). The pointer to the root of the AST representing the for-loop is bound to $FORROOT.

<li> match("$N=_(null)",astRoot);\n
  Match all nodes with arity 1 and a single null value. The main purpose for such match-expressions is to perform consistency checks in the AST.

<li> match("$N=SgInitializedName(null)",astRoot); // many of those exist in a default ROSE AST\n
  Specifically match all SgInitializedName nodes with a null pointer.

<li> match("SgForStatement($LoopCond,_,_,_)|SgWhile($LoopCond,_)|SgDoWhile(_,$LoopCond)",astRoot);\n
  Match different Loop constructs and bind variable $LoopCond to the respective loop condition.

<li> match("SgAssignOp(SgVarRef,SgAddOp($X,$Y))",astRoot)\n
  Match assignments with a variable on the rhs and an add-operator on the rhs(root). The pointers to the sub-ASTs representing the lhs and rhs of the add-operator are bound to variables $X and $Y for each match in the AST:

<li> match("$Func=SgFunctionCallExp($FuncRef,$Params)",astRoot)\n
  Match all function calls and bind variable $Func to the root of each such expression, bind $FuncRef to the SgFunctionRefExp (which can be used to obtain the name) and $Params to the AST representing the parameters:
</ol>

@section matchresults Accessing matching results

The results are collected in a std::list of std::maps. Each map
represents on successful match at one location in the AST and contains
all the bound variables. The variables can be accessed by name and
using the random access operator. The number of elements (=maps) in
the list corresponds to the number of matched patterns in the AST.

The pointers to matched patterns in the AST can be accessed as follows:
e.g.
@code
    1: AstMatching m;
    2: MatchResult res=m.performMatching("$R=SgInitalizedName($X)",root);
    3: SgNode* myRvariable=res["$R"];
    4: SgNode* myXvariable=res["$X"];
@endcode

In line 1 the AstMatching object is created. In line 2 the
match-expression and the root node of the AST is provided to the
matching mechanism and the results are computed. In line 3 the
variable $R (and in line 4 variable $X) is accessed. This pointer
value refers to the node in the AST which was matched successfully in
the match expression.  The matching can be performed on any AST
subtree of interest, by letting 'root' point to the respective AST
subtree when the match operation is started.

The number of elements in the match result shows how often the match expression was successfully matched and at least one variable was bound. Consequently, res.size()==0 means that the pattern could not be matched anywhere.

@section howtowrite How to get started writing match expressions

Knowing the exact structure of an AST with all the names of nodes can be difficult. Therefore, an existing AST (or any of its subtrees) can be printed in the exact same format as the matcher expects as input.

@code
#include "AstTerm.h"
...
cout<<AstTerm::astTermWithNullValuesToString(root);
@endcode

Prints on stdout for an AST with root node 'root' the entire AST in the exact same format as the function performMatching expects as first argument. The string can be refined with additional variables and operators. The namespace AstTerm contains some more functions to print the AST with number values, properly indented as a tree, with types, etc.

*/

class AstMatching {
 public:
  AstMatching();
  ~AstMatching();

  /** This is the main function to be called for matching. The match expression is provided as a string (which is being parsed and checked for syntax errors), and then the matching is performed. Any node in the AST can be used as a root node. The match result (a map) is returned. */ 
  MatchResult performMatching(std::string matchExpression, SgNode* root);

  /** Constructor as a all-in-one solution. It performs the matching and stores the result. This requires to use function getResult() to access the result. */
  AstMatching(std::string matchExpression,SgNode* root);

  /** Allows to access the match result if the match expression was provided as an argument to the constructor. It can also be used to access results accumulated from multiple performed match operations. */
  MatchResult getResult();

  /** This function is only for information purposes. It prints the
     sequence of internal instructions that are generated by the match expression frontend (invoked by performMatching). The instructions constitute its own small language. Internally, the instructions are inserted in a polymorphic list and by executing each instruction, the matching, binding of variables, and (possibly) skipping of substrees is performed. The list of instructions is provided in a human readable format.
  */
  void printMatchOperationsSequence();

  /** This flag is useful when reusing the same matcher object for
     performing multiple matches. It allows to keep record of all nodes that
     have been marked by a previous match using the '#' operator. The
     subtrees of those nodes are not traversed in the subsequent matches.
   */
  void setKeepMarkedLocations(bool keepMarked);

  /** This function is only for information purposes. It prints the
     set of all marked nodes (marked with the '#' operator).
  */
  void printMarkedLocations();

 private:
  bool performSingleMatch(SgNode* node, MatchOperationList* matchOperationSequence);
  void performMatchingOnAst(SgNode* root);
  void performMatching();
  void generateMatchOperationsSequence();

 private:
  std::string _matchExpression;
  SgNode* _root;
  MatchOperationList* _matchOperationsSequence;
  MatchStatus _status;
  bool _keepMarkedLocations;
};

#endif
