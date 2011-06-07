import sys, ast

import sage

OPERATOR_BUILDFXN_MAP = {
    ast.Add: sage.buildAddOp,
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

  def visit_BinOp(self, node):
    lhs = self.visit(node.left)
    rhs = self.visit(node.right)
    build_fxn = OPERATOR_BUILDFXN_MAP[node.op.__class__]
    return build_fxn(lhs, rhs, self.file_info(node))

  def visit_Expr(self, node):
    value = self.visit(node.value)
    return sage.buildExpr(value)

  def visit_FunctionDef(self, node):
    scope = self.scopeStack.peek()
    defaults = map(self.visit, node.args.defaults)
    capsule = sage.buildFunctionDef(node, defaults, self.file_info(node), scope)
    self.scopeStack.push(capsule)
    body_forest = map(self.visit, node.body)
    sage.addChildrenToNode(capsule, body_forest)
    self.scopeStack.pop(capsule)
    return capsule

  def visit_Module(self, node):
    scope_capsule = sage.buildGlobal(self.filename)

    self.scopeStack.push(scope_capsule)
    subforest = self.generic_visit(node)
    self.scopeStack.pop(scope_capsule)

    sage.addChildrenToNode(scope_capsule, subforest)
    return scope_capsule

  def visit_Name(self, node):
    print "building name:", node.id
    scope = self.scopeStack.peek()
    return sage.buildName(node.id, scope)

  def visit_Num(self, node):
    return sage.buildLongIntVal(node.n, self.file_info(node))

  def visit_Print(self, node):
    subforest = self.generic_visit(node)
    return sage.buildPrintStmt(subforest, self.file_info(node))

  def visit_Return(self, node):
    value = self.visit(node.value)
    return sage.buildReturnStmt(value)

  def visit_Str(self, node):
    return sage.buildStringVal(node.s, self.file_info(node))


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
