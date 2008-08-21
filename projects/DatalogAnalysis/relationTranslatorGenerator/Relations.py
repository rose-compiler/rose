
#produces a string for assigning tuple elemetns to the expressions given
def setTuple( elements ):
    s= ''
    for i in range(len(elements)):
        s += "            tuple[%d]= %s;\n"%(i,elements[i] );
    return s

#like setTuple, but calls getElement on the expressions
def setTupleE( elements ):
    return setTuple( [ 'getElement(%s)'%e for e in elements ] )

#sets the documentation strings
def setDoc( doc ):
    s= ''
    for i in range(len(doc)):
        s += "        doc[%d]= \"%s\";\n"%(i,doc[i] );
    return s


#creates a hash representing one rule for relations
def relation( name, arity, types, set_tuple, set_doc, pred = None ):
    return  {
                'relation': name,
                'arity':arity,
                'types':types,
                'set_tuple':set_tuple,
                'set_doc':set_doc,
                'pred':pred
            }

#conveniently builds binary expression relations
def binaryExp( name, typelist ):
    return relation( name, 3, typelist,
            setTupleE( [
                    'node','node->get_lhs_operand_i()','node->get_rhs_operand_i()'
                ] ),
            setDoc( [ name, 'lhs', 'rhs' ] )
        )
#conveniently builds unary expression relations
def unaryExp( name, typelist ):
    return relation(
                name, 2, typelist,
                setTupleE( [ 'node', 'node->get_operand_i()' ] ),
                setDoc( [ name, 'exp' ] )
            )


#convneniently builds unary Relations, meanin there is no child.
#this is distinct from a unary expression relation.
def unaryRelation( name, typelist ):
    return relation(
                name, 1, typelist,
                setTupleE( ['node'] ),setDoc( [name] )
            )


NodeRelations= [

unaryRelation( 'stmt', ['SgStatement'] ),
unaryRelation( 'exp', ['SgExpression'] ),
unaryRelation( 'literal', ['SgValueExp'] ),
unaryRelation( 'symbol', ['SgSymbol'] ),
unaryRelation( 'type', ['SgType'] ),
unaryRelation( 'decl', ['SgDeclarationStatement'] ),

relation( 'expType', 2, ['SgExpression'],
        setTupleE( ['node','node->get_type()'] ),
        setDoc( ['exp','type'] )
    ),

binaryExp('binaryE',['SgBinaryOp'] ),
binaryExp('addE',['SgAddOp'] ),
binaryExp('andAssignE',['SgAndAssignOp'] ),
binaryExp('andE',['SgAndOp'] ),
binaryExp('arrowE',['SgArrowExp'] ),
binaryExp('arrowStarE',['SgArrowStarOp'] ),
binaryExp('assignE',['SgAssignOp'] ),
binaryExp('bitAndE',['SgBitAndOp'] ),
binaryExp('bitOrE',['SgBitOrOp'] ),
binaryExp('bitXorE',['SgBitXorOp'] ),
binaryExp('commaE',['SgCommaOpExp'] ),
binaryExp('divAssignE',['SgDivAssignOp'] ),
binaryExp('divE',['SgDivideOp'] ),
binaryExp('dotE',['SgDotExp'] ),
binaryExp('dotStarE',['SgDotStarOp'] ),
binaryExp('eqE',['SgEqualityOp'] ),
binaryExp('geqE',['SgGreaterOrEqualOp'] ),
binaryExp('gtE',['SgGreaterThanOp'] ),
binaryExp('iDivE',['SgIntegerDivideOp'] ),
binaryExp('orAssignE',['SgIorAssignOp'] ),
binaryExp('leqE',['SgLessOrEqualOp'] ),
binaryExp('ltE',['SgLessThanOp'] ),
binaryExp('lshiftAssignE',['SgLshiftAssignOp'] ),
binaryExp('lshiftE',['SgLshiftOp'] ),
binaryExp('minusAssignE',['SgMinusAssignOp'] ),
binaryExp('modAssignE',['SgModAssignOp'] ),
binaryExp('modE',['SgModOp'] ),
binaryExp('multiplyAssignE',['SgMultAssignOp'] ),
binaryExp('multiplyE',['SgMultiplyOp'] ),
binaryExp('neqE',['SgNotEqualOp'] ),
binaryExp('orE',['SgOrOp'] ),
binaryExp('addAssignE',['SgPlusAssignOp'] ),
binaryExp('arraySubscriptE',['SgPntrArrRefExp'] ),
binaryExp('rshiftAssignE',['SgRshiftAssignOp'] ),
binaryExp('rshiftE',['SgRshiftOp'] ),
binaryExp('scopeE',['SgScopeOp'] ),
binaryExp('subtractE',['SgSubtractOp'] ),
binaryExp('xorAssignE',['SgXorAssignOp'] ),
binaryExp(
        'anyAssignE',
        [ 'SgAndAssignOp','SgAssignOp','SgDivAssignOp',
          'SgIorAssignOp','SgLshiftAssignOp','SgMinusAssignOp',
          'SgModAssignOp','SgMultAssignOp','SgPlusAssignOp',
          'SgRshiftAssignOp','SgXorAssignOp'
        ]
    ),

relation(
        'condE',4,['SgConditionalExp'],
        setTupleE( [
            'node','node->get_conditional_exp()',
            'node->get_true_exp()','node->get_false_exp()'] ),
        setDoc( [ 'condE', 'conditional', 'true', 'false' ] )
    ),

relation(
        'varE', 2, ['SgVarRefExp'],
        setTupleE( ['node','node->get_symbol()'] ),
        setDoc( ['node','symbol'] )
    ),

unaryRelation( 'nullE', ['SgNullExpression'] ),

relation(
        'callE', 2, ['SgFunctionCallExp'],
        setTupleE( ['node', 'node->get_function()' ] ),
        setDoc( ['call', 'function' ] )
    ),

relation(
        'funcRefE', 2, ['SgFunctionRefExp'],
        setTupleE( ['node','node->get_symbol_i()'] ),
        setDoc( ['ref', 'function symbol' ] )
    ),

unaryExp( 'addressOfE', ['SgAddressOfOp'] ),
unaryExp( 'bitNegE', ['SgBitComplementOp'] ),
unaryExp( 'castE', ['SgCastExp'] ),
unaryExp( 'rootE', ['SgExpressionRoot'] ),
unaryExp( 'minusMinusE', ['SgMinusMinusOp'] ),
unaryExp( 'negE', ['SgMinusOp'] ),
unaryExp( 'notE', ['SgNotOp'] ),
unaryExp( 'plusPlusE', ['SgPlusPlusOp'] ),
unaryExp( 'derefE', ['SgPointerDerefExp'] ),
unaryExp( 'throwE', ['SgThrowOp'] ),
unaryExp( 'ptrDerefE', ['SgPointerDerefExp'] ),
#unaryExp('unaryAddE',['SgUnaryAddOp'] ),

relation(
        'prefix', 1, ['SgUnaryOp'],
        setTupleE( ['node'] ),setDoc( ['exp'] ),
        pred='node->get_mode()==SgUnaryOp::prefix'
    ),

relation(
        'postfix', 1, ['SgUnaryOp'],
        setTupleE( ['node'] ),setDoc( ['exp'] ),
        pred='node->get_mode()==SgUnaryOp::postfix'
    ),



unaryRelation( 'breakS', ['SgBreakStmt'] ),
unaryRelation( 'continueS', ['SgContinueStmt'] ),
unaryRelation( 'nullS', ['SgNullStatement'] ),

relation(
        'caseS', 3, ['SgCaseOptionStmt'],
        setTupleE( ['node','node->get_key()','node->get_body()'] ),
        setDoc( ['caseS','label','body'] )
    ),

#relation( 'catchS',

relation(
        'defaultS', 2, ['SgDefaultOptionStmt'],
        setTupleE( ['node','node->get_body()'] ),
        setDoc( ['defaultS','body'] )
    ),

relation(
        'exprS', 2, ['SgExprStatement'],
        setTupleE( ['node','node->get_expression()'] ),
        setDoc( ['exprS','expr'] )
    ),

#relation( forInitS

relation(
        'gotoS', 2, ['SgGotoStatement'],
        setTupleE( ['node','node->get_label()'] ),
        setDoc( ['gotoS','label'] )
    ),

relation(
        'labelS', 2, ['SgLabelStatement'],
        """
                tuple[0]= getElement(node);
                SgName name= node->get_label();
                tuple[1]= getElement(&name);
        """,
        setDoc( ['labelS','label'] )
    ),

relation(
        'returnS', 2, ['SgReturnStmt'],
        setTupleE( ['node','node->get_expression()'] ),
        setDoc( ['returnS', 'exp'] )
    ),

#the following two defs are for the same relation
relation(
        'loop', 3, ['SgDoWhileStmt','SgWhileStmt'],
        setTupleE( ['node', 'node->get_condition()','node->get_body()'] ),
        setDoc( [ 'loop', 'cond', 'body' ] )
    ),
relation(
        'loop', 3, ['SgForStatement'],
        setTupleE( ['node', 'node->get_test()','node->get_loop_body()'] ),
        setDoc( [ 'loop', 'cond', 'body' ] )
    ),

relation(
        'whileS', 3, ['SgWhileStmt'],
        setTupleE( ['node', 'node->get_condition()','node->get_body()'] ),
        setDoc( [ 'whileS', 'cond', 'body' ] )
    ),
relation(
        'doWhileS', 3, ['SgDoWhileStmt'],
        setTupleE( ['node', 'node->get_condition()','node->get_body()'] ),
        setDoc( [ 'doWhileS', 'cond', 'body' ] )
    ),

relation(
        'ifS', 4, ['SgIfStmt'],
        setTupleE( [
                'node', 'node->get_conditional()',
                'node->get_true_body()',
                'node->get_false_body()'
            ] ),
        setDoc( [ 'ifS', 'cond', 'true', 'false' ] )
    ),

relation(
        'switchS', 3, ['SgSwitchStatement'],
        setTupleE( ['node', 'node->get_item_selector()', 'node->get_body()'] ),
        setDoc( ['switchS','selector','body'] )
    ),

relation(
        'parent', 2, ['SgNode'],
        setTupleE( ['node->get_parent()', 'node'] ),
        setDoc( [ 'node', 'node' ] ),
        '( node->get_parent() != NULL )'
    ),

#relation(
#        'scope', 2, ['SgStatement'],
#        setTupleE( ['node->get_scope()','node'] ),
#        setDoc( [ 'scope', 'statement' ] ),
#        '( node && node->get_scope() )'
#    ),

unaryRelation( 'globalScope', ['SgGlobal'] ),


relation(
        'classD', 4, ['SgClassDeclaration'],
        """
                tuple[0]= getElement(node);
                SgName name= node->get_name();
                tuple[1]= getElement(&name);
                tuple[2]= getElement(node->get_type());
                tuple[3]= getElement(node->get_definition());
        """,
        setDoc( ['class','name','type','def'] ),
        'node->get_definingDeclaration() == node'
    ),

relation(
        'enumD', 3, ['SgEnumDeclaration'],
        """
                tuple[0]= getElement(node);
                SgName name= node->get_name();
                tuple[1]= getElement(&name);
                tuple[2]= getElement(node->get_type());
        """,
        setDoc( ['enum','name','type'] )
    ),

relation(
        'base', 2, ['SgBaseClass'],
        setTupleE( [
                'node',
                'isSgClassDeclaration(node->get_base_class()->get_definingDeclaration())'
            ] ),
        setDoc( ['base','class decl'] )
    ),


unaryRelation( 'varD', ['SgVariableDeclaration'] ),

relation(
        'iname', 4, ['SgInitializedName'],
        """
            tuple[0]= getElement(node);
            SgName name= node->get_name();
            tuple[1]= getElement(&name);
            tuple[2]= getElement(node->get_scope()->lookup_symbol(name));
            tuple[3]= getElement(node->get_typeptr());
        """,
        setDoc( ['initialized name', 'name', 'symbol', 'type' ] )
    ),

relation(
        'funcD', 4, ['SgFunctionDeclaration'], 
        """
                tuple[0]= getElement(node);
                SgName name= node->get_name();
                tuple[1]= getElement(&name);
                tuple[2]= getElement(node->get_type());
                tuple[3]= getElement(node->get_definition());
        """,
        setDoc( ['funcDecl','name','type','body'] ),
    ),


relation(
        'definingD', 1, ['SgDeclarationStatement'], 
	setTupleE(["node"]),
        setDoc( ['defining declaration'] ),
	'node->get_definingDeclaration()==node'
    ),

relation(
        'publicM', 1, ['SgDeclarationStatement'],
        setTupleE( ['node'] ), setDoc( ['public'] ),
        'node->get_declarationModifier().get_accessModifier().isPublic()'
    ),

relation(
        'privateM', 1, ['SgDeclarationStatement'],
        setTupleE( ['node'] ), setDoc( ['private'] ),
        'node->get_declarationModifier().get_accessModifier().isPrivate()'
    ),

relation(
        'protectedM', 1, ['SgDeclarationStatement'],
        setTupleE( ['node'] ), setDoc( ['protected'] ),
        'node->get_declarationModifier().get_accessModifier().isProtected()'
    ),

relation(
        'virtualM', 1, ['SgFunctionDeclaration'],
        setTupleE( ['node'] ), setDoc( ['virtual'] ),
        'node->get_functionModifier().isVirtual()'
    ),

relation(
        'pureVirtualM', 1, ['SgFunctionDeclaration'],
        setTupleE( ['node'] ), setDoc( ['pureVirtual'] ),
        'node->get_functionModifier().isPureVirtual()'
    ),

relation(
        'constructorM', 1, ['SgFunctionDeclaration'],
        setTupleE( ['node'] ), setDoc( ['constructor'] ),
        'node->get_specialFunctionModifier().isConstructor()'
    ),

relation(
        'destructorM', 1, ['SgFunctionDeclaration'],
        setTupleE( ['node'] ), setDoc( ['destructor'] ),
        'node->get_specialFunctionModifier().isDestructor()'
    ),

unaryRelation( 'varSym', ['SgVariableSymbol'] ),

unaryRelation( 'boolT',  ['SgTypeBool'] ),
unaryRelation( 'charT',  ['SgTypeChar'] ),
unaryRelation( 'defaultT',  ['SgTypeDefault'] ),
unaryRelation( 'doubltT',  ['SgTypeDouble'] ),
unaryRelation( 'ellipseT',  ['SgTypeEllipse'] ),
unaryRelation( 'floatT',  ['SgTypeFloat'] ),
unaryRelation( 'gvoidT',  ['SgTypeGlobalVoid'] ),
unaryRelation( 'intT',  ['SgTypeInt'] ),
unaryRelation( 'longT',  ['SgTypeLong'] ),
unaryRelation( 'ldoubleT',  ['SgTypeLongDouble'] ),
unaryRelation( 'llongT',  ['SgTypeLongLong'] ),
unaryRelation( 'shortT',  ['SgTypeShort'] ),
unaryRelation( 'scharT',  ['SgTypeSignedChar'] ),
unaryRelation( 'sintT',  ['SgTypeSignedInt'] ),
unaryRelation( 'slongT',  ['SgTypeSignedLong'] ),
unaryRelation( 'sshortT',  ['SgTypeSignedShort'] ),
unaryRelation( 'stringT',  ['SgTypeString'] ),
unaryRelation( 'unknownT',  ['SgTypeUnknown'] ),
unaryRelation( 'ucharT',  ['SgTypeUnsignedChar'] ),
unaryRelation( 'uintT',  ['SgTypeUnsignedInt'] ),
unaryRelation( 'ulongT',  ['SgTypeUnsignedLong'] ),
unaryRelation( 'ullongT',  ['SgTypeUnsignedLongLong'] ),
unaryRelation( 'ushortT',  ['SgTypeUnsignedShort'] ),
unaryRelation( 'voidT',  ['SgTypeVoid'] ),
unaryRelation( 'wcharT',  ['SgTypeWchar '] ),

relation(
        'arrayT', 3, ['SgArrayType'],
        setTupleE( ['node', 'node->get_base_type()', 'node->get_index()'] ),
        setDoc( ['array', 'base type', 'size expr'] )
    ),

relation(
        'refT', 2, ['SgReferenceType'],
        setTupleE( ['node', 'node->get_base_type()' ] ),
        setDoc( ['reference','base type'] )
    ),

relation(
        'ptrT', 2, ['SgPointerType'],
        setTupleE( ['node', 'node->get_base_type()' ] ),
        setDoc( ['pointer','base type'] )
    ),
]


def listR( type, ctype, first, next, getlist ):
    dict=   {
            'type':  type,
            'ctype': ctype,
            'first_relation': first,
            'next_relation': next,
            'list': getlist
            }
    return dict


#list like nodes in the IR have a different relation mechanism
ListRelations= [
listR(
        'SgBasicBlock', 'SgStatement',
        'first_inB', 'next_inB',
        'node->get_statements()'
    ),

listR(
        'SgFunctionParameterList', 'SgInitializedName',
        'first_inParam', 'next_inParam',
        'node->get_args()'
    ),

listR(
        'SgGlobal', 'SgDeclarationStatement',
        'first_inGlobal', 'next_inGlobal',
        'node->get_declarations()'
    ),

listR(
        'SgClassDefinition', 'SgDeclarationStatement',
        'first_declInClass', 'next_declInClass',
        'node->get_members()'
    ),

listR(
        'SgClassDefinition', 'SgBaseClass',
        'first_baseInClass', 'next_baseInClass',
        'node->get_inheritances()'
    ),

listR(
        'SgVariableDeclaration', 'SgInitializedName',
        'first_nameInV', 'next_nameInV',
        'node->get_variables()'
    ),

listR(
        'SgFunctionDeclaration', 'SgInitializedName',
        'first_paramInF', 'next_paramInF',
        'node->get_parameterList()->get_args()'
    ),

listR(
        'SgFunctionCallExp', 'SgExpression', 
        'first_argInCall', 'next_argInCall',
        'node->get_args()->get_expressions()'
    ),

]
