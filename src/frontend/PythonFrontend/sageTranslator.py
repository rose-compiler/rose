import sys, ast

import sage

UNARY_OPERATOR_MAP = {
    ast.UAdd:     "+",
    ast.USub:     "-",
    ast.Invert:   "~"
}

BINARY_OPERATOR_MAP = {
    ast.Add:      "+",
    ast.BitAnd:   "&",
    ast.BitOr:    "|",
    ast.BitXor:   "^",
    ast.Div:      "/",
    ast.FloorDiv: "//",
    ast.LShift:   "<<",
    ast.Mod:      "%",
    ast.Mult:     "*",
    ast.Pow:      "**",
    ast.RShift:   ">>",
    ast.Sub:      "-",
}

AUG_OPERATOR_MAP = {
    ast.Add:      "+=",
    ast.BitAnd:   "&=",
    ast.BitOr:    "|=",
    ast.BitXor:   "^=",
    ast.Div:      "/=",
    ast.FloorDiv: "//=",
    ast.LShift:   "<<=",
    ast.Mod:      "%=",
    ast.Mult:     "*=",
    ast.Pow:      "**=",
    ast.RShift:   ">>=",
    ast.Sub:      "-=",
}

COMP_OPERATOR_MAP = {
    ast.Eq:       "==",
    ast.NotEq:    "!=",
    ast.LtE:      "<=",
    ast.GtE:      ">=",
    ast.Lt:       "<",
    ast.Gt:       ">",
    ast.Is:       "is",
    ast.IsNot:    "is not",
    ast.In:       "in",
    ast.NotIn:    "not in"
}

class FileInfo():

  def __init__(self, filename, node):
    self.filename = filename
    self.lineno = node.lineno
    self.col_offset = node.col_offset


class stack():

  def __init__(self):
    self._stack = []

  def push(self, obj):
    self._stack.append(obj)

  def pop(self, expected=None):
    if expected != None and expected != self.peek():
        raise Exception("Popped unexpected value from stack.")
    self._stack.pop()

  def peek(self):
    return self._stack[-1]


class SageTranslator(ast.NodeVisitor):

  def __init__(self, filename):
    self.filename = filename
    self.scopeStack = stack()

  def __call__(self, syntax_tree):
    return self.visit(syntax_tree)

  def file_info(self, node):
    return FileInfo(self.filename, node)

  def generic_visit(self, node):
    #print "generic_visit for class: ", node.__class__.__name__
    return map(self.visit, ast.iter_child_nodes(node))

  def visit_arguments(self, node):
    args = map(lambda arg: sage.buildInitializedName(arg.id), node.args)
    kwargs = map(self.visit, node.defaults)
    return sage.buildFunctionParameterList(args, kwargs)

  def visit_Assign(self, node):
    targets = map(self.visit, node.targets)
    value = self.visit(node.value)
    assert len(targets) == 1, "target assignment lists are yet to be supported"
    return sage.buildAssign(targets[0], value)

  def visit_AugAssign(self, node):
    target = self.visit(node.target)
    value  = self.visit(node.value)
    op_str = AUG_OPERATOR_MAP[node.op.__class__]
    return sage.buildAugAssign(target, value, op_str)

  def visit_BinOp(self, node):
    lhs = self.visit(node.left)
    rhs = self.visit(node.right)
    op_str = BINARY_OPERATOR_MAP[node.op.__class__]
    return sage.buildBinOp(lhs, rhs, op_str)

  def visit_Call(self, node):
    name = node.func.id
    args = map(self.visit, node.args)
    kwargs = map(self.visit, node.keywords)
    scope = self.scopeStack.peek()
    return sage.buildCall(name, args, kwargs, scope)

  def visit_Compare(self, node):
    lhs = self.visit(node.left)
    comparators = map(self.visit, node.comparators)
    ops = map(lambda op: COMP_OPERATOR_MAP[op.__class__], node.ops)
    return sage.buildCompare(lhs, ops, comparators)

  def visit_ExceptHandler(self, node):
    e_name = node.name
    e_type = node.type
    e_body = map(self.visit, node.body)
    return sage.buildExceptHandler(e_name, e_type, e_body)

  def visit_Expr(self, node):
    value = self.visit(node.value)
    return sage.buildExpr(value)

  def visit_FunctionDef(self, node):
    scope = self.scopeStack.peek()
    decorators = map(self.visit, node.decorator_list)
    params = self.visit(node.args)
    (capsule, scope) = \
        sage.buildFunctionDef(node.name, params, decorators, scope)
    self.scopeStack.push(scope)
    body_forest = map(self.visit, node.body)
    sage.appendStatements(capsule, body_forest)
    self.scopeStack.pop(scope)
    return capsule

  def visit_If(self, node):
    test = self.visit(node.test)
    body = map(self.visit, node.body)
    orelse = map(self.visit, node.orelse)
    return sage.buildIf(test, body, orelse)

  def visit_keyword(self, node):
    arg = self.visit(node.arg)
    value = self.visit(node.value)
    return sage.buildKeyword(arg, value)

  def visit_Lambda(self, node):
    scope = self.scopeStack.peek()
    params = self.visit(node.args)
    (lambda_capsule, lambda_scope) = sage.buildLambda(params, scope)

    self.scopeStack.push(scope)
    expr = self.visit(node.body)
    self.scopeStack.pop(scope)

    sage.appendStatements(lambda_capsule, [expr])
    return lambda_capsule

  def visit_List(self, node):
    return sage.buildListExp(map(self.visit, node.elts))

  def visit_Module(self, node):
    (scope, wrapper_func) = sage.buildGlobal(self.filename)

    self.scopeStack.push(scope)
    subforest = self.generic_visit(node)
    self.scopeStack.pop(scope)

    sage.appendStatements(wrapper_func, subforest)
    return scope

  def visit_Name(self, node):
    scope = self.scopeStack.peek()
    return sage.buildName(node.id, scope)

  def visit_NoneType(self, node):
    scope = self.scopeStack.peek()
    return sage.buildName("None", scope)

  def visit_Num(self, node):
    return sage.buildLongIntVal(node.n)

  def visit_Print(self, node):
    dest = self.visit(node.dest)
    values = sage.buildExprListExp(map(self.visit, node.values))
    return sage.buildPrintStmt(dest, values)

  def visit_Return(self, node):
    value = self.visit(node.value)
    return sage.buildReturnStmt(value)

  def visit_Str(self, node):
    return sage.buildStringVal(node.s)

  def visit_str(self, str):
    return sage.buildStringVal(str)

  def visit_TryExcept(self, node):
    body = map(self.visit, node.body)
    handlers = map(self.visit, node.handlers)
    orelse = map(self.visit, node.orelse)
    return sage.buildTryExcept(body, handlers, orelse)

  def visit_TryFinally(self, node):
    body = map(self.visit, node.body)
    finalbody = map(self.visit, node.finalbody)
    return sage.buildTryFinally(body, finalbody)

  def visit_Tuple(self, node):
    return sage.buildTuple(map(self.visit, node.elts))

  def visit_UnaryOp(self, node):
    operand = self.visit(node.operand)
    op_str = UNARY_OPERATOR_MAP[node.op.__class__]
    return sage.buildUnaryOp(op_str, operand)

  def visit_While(self, node):
    test = self.visit(node.test)
    body = sage.buildSuite(map(self.visit, node.body))
    #orelse = sage.buildSuite(map(self.visit, node.orelse))
    #return sage.buildWhile(test, body, orelse)
    return sage.buildWhile(test, body)

  def visit_With(self, node):
    expr = self.visit(node.test)
    vars = map(self.visit, node.optional_vars)
    body = sage.buildSuite(map(self.visit, node.body))
    return sage.buildWith(expr, vars, body)

def translate(infilename):
  try:
    infile = open(infilename)
    contents = infile.read()
    infile.close()
  except IOError:
    print >>sys.stderr, "IO error when reading file: %s" % infilename
    exit(1)
  syntax_tree = ast.parse(contents)
  return SageTranslator(infilename).visit(syntax_tree)

def main(argv):
  map(translate, argv[1:])

if __name__ == "__main__":
  main(sys.argv)
