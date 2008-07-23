#!/bin/sh
# \
exec tclsh "$0" "$@"

if {[llength $argv] < 2} {error "Usage: $argv0 baseClassPairsFile outputFile"}
set baseClassPairs [exec grep "class Sg.*: public .*" [lindex $argv 0] | cut -f 2,5 -d \ ]
set outputFile [lindex $argv 1]
set of [open $outputFile w]

foreach {child base} $baseClassPairs {
  lappend children($base) $child
}

proc descendents {base} {
  global children
    if {![info exists children($base)]} {
      set children($base) {}
    }
  set result [list $base]
    foreach ch $children($base) {
      set result [concat $result [descendents $ch]]
    }
  return $result
}

proc getField {objectType fieldType name index} {
  return "let('$fieldType', \$$index, \"is${objectType}(\$0)->get_${name}()\")"
}

proc redirToParent {fields index} {
  set modeStr "-('SgNode*')"
  set i 0
  foreach f $fields {
    if {$i == $index} {
      lappend modeStr "+('$f')"
    } else {
      lappend modeStr "-('$f')"
    }
    incr i
  }
  return "rule(\[[join $modeStr ,\ ]\], \[cond(\"\$[expr {$index + 1}]\"), let('SgNode*', \$0, \"\$[expr {$index + 1}]->get_parent()\"), rescan\])"
}

proc iterateMemoryPools {fields pools} {
  set quotedPools ""
  foreach p $pools {lappend quotedPools '$p'}
  set modeStr "-('SgNode*')"
  foreach f $fields {
    lappend modeStr "-('$f')"
  }
  return "rule(\[[join $modeStr ,\ ]\], \[iterMemoryPools(\$0, \[[join $quotedPools ,]\]), makeGround(\$0, 'SgNode*'), rescan\])"
}

proc treeNode {tableName sageName fieldTypesAndNames} {
  # A field name beginning with - does not have its parent set to the
  # containing node, so we cannot find the containing node from it
  global of
  set fieldTypes ""
  set fieldNames ""
  foreach {t n} $fieldTypesAndNames {
    lappend fieldTypes $t
    if {[string index $n 0] == "-"} {set n [string range $n 1 end]}
    lappend fieldNames $n
  }
  puts $of ":- prim(${tableName}, [llength [concat x $fieldTypes]],"
  set mainModeStr "+('SgNode*')"
  foreach i $fieldTypes {lappend mainModeStr "-('$i')"}
  puts $of "\[rule(\[[join $mainModeStr ,\ ]\], \[cond(\"is${sageName}(\$0)\")"
  set i 0
  foreach {t n} $fieldTypesAndNames {
    incr i ;# Done first to offset for extra $0 entry in param list
    if {[string index $n 0] == "-"} {set n [string range $n 1 end]}
    puts $of ,[getField $sageName $t $n $i]
  }
  puts $of "\])"
  set i 0
  foreach {t n} $fieldTypesAndNames {
    if {[string index $n 0] != "-"} {
      puts $of ,[redirToParent $fieldTypes $i]
    }
    incr i
  }
  puts $of ,[iterateMemoryPools $fieldTypes [descendents $sageName]]
  puts $of "\])."
}

treeNode expression SgExpression {SgNode* -type}

proc binaryOp {tableName sageName} {
  treeNode $tableName $sageName {SgNode* lhs_operand SgNode* rhs_operand}
}

binaryOp binaryOperator SgBinaryOp
binaryOp addOperator SgAddOp
binaryOp addAssignOperator SgPlusAssignOp
binaryOp logicalAndOperator SgAndOp
binaryOp arrowOperator SgArrowExp
binaryOp arrowStarOperator SgArrowStarOp
binaryOp assignOperator SgAssignOp
binaryOp bitwiseAndOperator SgBitAndOp
binaryOp bitwiseOrOperator SgBitOrOp
binaryOp bitwiseXorOperator SgBitXorOp
binaryOp bitwiseAndAssignOperator SgAndAssignOp
binaryOp bitwiseOrAssignOperator SgIorAssignOp
binaryOp bitwiseXorAssignOperator SgXorAssignOp
binaryOp commaOperator SgCommaOpExp
binaryOp divideAssignOperator SgDivAssignOp
binaryOp divideOperator SgDivideOp
binaryOp dotOperator SgDotExp
binaryOp dotStarOperator SgDotStarOp
binaryOp equalOperator SgEqualityOp
binaryOp greaterThanOrEqualOperator SgGreaterOrEqualOp
binaryOp greaterThanOperator SgGreaterThanOp
# binaryOp integerDivideOperator SgIntegerDivideOp ; # Is this used?
binaryOp lessThanOrEqualOperator SgLessOrEqualOp
binaryOp lessThanOperator SgLessThanOp
binaryOp leftShiftOperator SgLshiftOp
binaryOp rightShiftOperator SgRshiftOp
binaryOp leftShiftAssignOperator SgLshiftAssignOp
binaryOp rightShiftAssignOperator SgRshiftAssignOp
binaryOp subtractOperator SgSubtractOp
binaryOp subtractAssignOperator SgMinusAssignOp
binaryOp modulusOperator SgModOp
binaryOp modulusAssignOperator SgModAssignOp
binaryOp multiplyOperator SgMultiplyOp
binaryOp multiplyAssignOperator SgMultAssignOp
binaryOp notEqualOperator SgNotEqualOp
binaryOp logicalOrOperator SgOrOp
binaryOp arrayIndexOperator SgPntrArrRefExp
# binaryOp scopeOperator SgScopeOp ; # Is this used?

treeNode functionCall SgFunctionCallExp {SgNode* function SgNode* args}

proc unaryOp {tableName sageName} {
  treeNode $tableName $sageName {SgNode* operand}
}

unaryOp unaryOperator SgUnaryOp
unaryOp addressOfOperator SgAddressOfOp
unaryOp bitwiseNotOperator SgBitComplementOp
unaryOp castOperator SgCastExp
# unaryOp expressionRoot SgExpressionRoot
unaryOp incrementOperator SgPlusPlusOp
unaryOp decrementOperator SgMinusMinusOp
unaryOp negateOperator SgMinusOp
unaryOp unaryPlusOperator SgUnaryAddOp
unaryOp logicalNotOperator SgNotOp
unaryOp dereferenceOperator SgPointerDerefExp
unaryOp throwOperator SgThrowOp

proc listNode {tableName sageName otherFields args} {
  global of
  set listFields $args
  set fields $otherFields
  foreach {ourName roseName roseType} $listFields {
    lappend fields int "-${roseName}().size"
  }
  treeNode $tableName $sageName $fields
  foreach {ourName roseName roseType} $listFields {
    puts $of ":- prim(${tableName}${ourName}, 3,"
    puts $of "        \[rule(\[+('SgNode*'), +('int'), -('SgNode*')\], \[cond(\"is${sageName}(\$0) && \$1 >= 0 && \$1 < is${sageName}(\$0)->get_${roseName}().size()\"), let('${roseType}::const_iterator', \$100, \"is${sageName}(\$0)->get_${roseName}().begin()\"), do(\"std::advance(\$100, \$1)\"), let('SgNode*', \$2, \"*(\$100)\")\]),"
    puts $of "         rule(\[+('SgNode*'), -('int'), -('SgNode*')\], \[cond(\"is${sageName}(\$0)\"), iterRange(\$1, 0, \"is${sageName}(\$0)->get_${roseName}().size()\"), makeGround(\$1, 'int'), rescan\]),"
    puts $of "         [iterateMemoryPools {int SgNode*} $sageName]\])."
  }
}

treeNode conditionalOperator SgConditionalExp {SgNode* conditional SgNode* true_exp SgNode* false_exp}
treeNode deleteExpression SgDeleteExp {SgNode* variable}
listNode expressionList SgExprListExp {} Element expressions SgExpressionPtrList

# Reference expressions
treeNode classReference SgClassNameRefExp {SgNode* -symbol}
treeNode functionReference SgFunctionRefExp {SgNode* -symbol}
treeNode memberFunctionReference SgMemberFunctionRefExp {SgNode* -symbol}
treeNode variableReference SgVarRefExp {SgNode* -symbol}

# Value expressions
treeNode valueExpression SgValueExp {}

# Symbols
treeNode symbol SgSymbol {}
treeNode variableSymbol SgVariableSymbol {SgNode* -declaration}

# Statements
treeNode statement SgStatement {}
treeNode breakStatement SgBreakStmt {}
treeNode continueStatement SgContinueStmt {}
treeNode caseStatement SgCaseOptionStmt {SgNode* condition SgNode* body}
treeNode defaultStatement SgDefaultOptionStmt {SgNode* body}
treeNode expressionStatement SgExprStatement {SgNode* expression}
treeNode gotoStatement SgGotoStatement {SgNode* label}
treeNode labelStatement SgLabelStatement {std::string -label}
treeNode returnStatement SgReturnStmt {SgNode* return_expr}

treeNode scopeStatement SgScopeStatement {}

listNode basicBlock SgBasicBlock {} Element statements SgStatementPtrList
treeNode ifStatement SgIfStmt {SgNode* conditional SgNode* true_body SgNode* false_body}
treeNode switchStatement SgSwitchStatement {SgNode* selector SgNode* body}
treeNode doWhileStatement SgDoWhileStmt {SgNode* condition SgNode* body}
treeNode forStatement SgForStatement {SgNode* for_init_stmt SgNode* test SgNode* increment_expr SgNode* loop_body}
listNode globalScope SgGlobal {} Member declarations SgDeclarationStatementPtrList
listNode namespaceDefinition SgNamespaceDefinitionStatement {SgNode* namespaceDeclaration} Member declarations SgDeclarationStatementPtrList
treeNode whileStatement SgWhileStmt {SgNode* condition SgNode* body}
treeNode functionDefinition SgFunctionDefinition {SgNode* -declaration SgNode* body}
listNode classDefinition SgClassDefinition {SgNode* -declaration} Member members SgDeclarationStatementPtrList BaseClass inheritances SgBaseClassPtrList

listNode variableDeclaration SgVariableDeclaration {} Element variables SgInitializedNamePtrList
treeNode initializedName SgInitializedName {std::string -name SgNode* -type SgNode* initializer}
treeNode classDeclaration SgClassDeclaration {std::string -name SgNode* -type SgNode* definition SgNode* -scope}
treeNode functionDeclaration SgFunctionDeclaration {std::string -name SgNode* parameterList SgNode* definition SgNode* -scope}
listNode functionParameterList SgFunctionParameterList {} Element args SgInitializedNamePtrList

treeNode declarationStatement SgDeclaration {}

# Types
treeNode type SgType {}
treeNode pointerType SgPointerType {SgNode* -base_type}
treeNode arrayType SgArrayType {SgNode* -base_type SgNode* index}
treeNode functionType SgFunctionType {SgNode* -return_type SgNode* argument_list}
listNode functionParameterTypeList SgFunctionParameterTypeList {} Element arguments SgTypePtrList
treeNode modifierType SgModifierType {SgNode* -base_type}
treeNode namedType SgNamedType {SgNode* -declaration}
treeNode classType SgClassType {SgNode* -declaration}
treeNode enumType SgEnumType {SgNode* -declaration}
treeNode typedefType SgTypedefType {SgNode* -declaration}
treeNode referenceType SgReferenceType {SgNode* -base_type}
treeNode boolType SgTypeBool {}
treeNode charType SgTypeChar {}
treeNode doubleType SgTypeDouble {}
treeNode floatType SgTypeFloat {}
treeNode intType SgTypeInt {}
treeNode longType SgTypeLong {}
treeNode longDoubleType SgTypeLongDouble {}
treeNode longLongType SgTypeLongLong {}
treeNode shortType SgTypeShort {}
treeNode signedCharType SgTypeSignedChar {}
treeNode signedIntType SgTypeSignedInt {}
treeNode signedLongType SgTypeSignedLong {}
treeNode signedShortType SgTypeSignedShort {}
treeNode stringType SgTypeString {}
treeNode unsignedCharType SgTypeUnsignedChar {}
treeNode unsignedIntType SgTypeUnsignedInt {}
treeNode unsignedLongType SgTypeUnsignedLong {}
treeNode unsignedLongLongType SgTypeUnsignedLongLong {}
treeNode unsignedShortType SgTypeUnsignedShort {}
treeNode voidType SgTypeVoid {}
treeNode wcharType SgTypeWchar {}

# Predicates

# CFG
puts $of {
:- prim(splitCfgNode, 3,
        [rule([+('VirtualCFG::CFGNode'), -('SgNode*'), -(int)],
	      [let('SgNode*', $1, "$0.getNode()"),
	       let(int, $2, "$0.getIndex()")]),
         rule([-('VirtualCFG::CFGNode'), +('SgNode*'), +(int)],
	      [let('VirtualCFG::CFGNode', $0, "VirtualCFG::CFGNode($1, $2)")])]).
:- prim(cfgNext, 2,
        [rule([+('VirtualCFG::CFGNode'), -('VirtualCFG::CFGNode')],
	      [iterCfgEdges($0, "outEdges", "target", $1),
               makeGround($1, 'VirtualCFG::CFGNode')]),
         rule([-('VirtualCFG::CFGNode'), +('VirtualCFG::CFGNode')],
	      [iterCfgEdges($1, "inEdges", "source", $0),
               makeGround($0, 'VirtualCFG::CFGNode')])]).
}

# Assorted
treeNode node SgNode {}
treeNode supportNode SgSupport {}
treeNode locatedNode SgLocatedNode {}

puts $of {
:- prim(parent, 2, % Parent then child
        [rule([-('SgNode*'), +('SgNode*')],
	      [cond("$1"), let('SgNode*', $0, "$1->get_parent()")]),
         rule([+('SgNode*'), '_'('SgNode*')],
              [cond("$0 && !$0->get_traversalSuccessorContainer.empty()")]),
         rule([+('SgNode*'), -('SgNode*')],
	      [cond("$0"), iterateChildren($0, $1), makeGround($1, 'SgNode*')])]).
}

# Compass
puts $of {
:- prim(compassParameter, 2, % +ParamName, -Value
        [rule([+('std::string'), -('std::string')],
	      [let('std::string', $1, "inputParameters[$0]")])]).

:- prim(compassError, 3, % +Node, +CheckerName, +Description
        [rule([+('SgNode*'), +('std::string'), +('std::string')],
              [do("output->addOutput(new Palette::CheckerOutput($0, $1, $2))")])]).
}

close $of
exit 0
