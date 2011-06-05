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

class SageTranslator(ast.NodeVisitor):

  def __init__(self, file):
    self.file = file

  def __call__(self, syntax_tree):
    return self.visit(syntax_tree)

  def generic_visit(self, node):
    print "No visit() method define for class: ", node.__class__.__name__
    return map(self.visit, ast.iter_child_nodes(node))

  def visit_BinOp(self, node):
    lhs = self.visit(node.left)
    rhs = self.visit(node.right)
    build_fxn = OPERATOR_BUILDFXN_MAP[node.op.__class__]
    return build_fxn(lhs, rhs)

  def visit_Module(self, node):
    subforest = self.generic_visit(node)
    return sage.buildGlobal(subforest)

  def visit_Num(self, node):
    return sage.buildLongIntVal(node.n)

  def visit_Print(self, node):
    subforest = self.generic_visit(node)
    return sage.buildPrintStmt(subforest)

  def visit_Str(self, node):
    return sage.buildStringVal(node.s, FileInfo(self.file, node))


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
