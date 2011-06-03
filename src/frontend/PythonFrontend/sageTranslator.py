import sys, ast

import sage

class SageTranslator(ast.NodeVisitor):

  def __call__(self, syntax_tree):
    return self.visit(syntax_tree)

  def generic_visit(self, node):
    return map(self.visit, ast.iter_child_nodes(node))

  def visit_Module(self, node):
     subforest = self.generic_visit(node)
     return sage.buildGlobal(subforest)

  def visit_Print(self, node):
     subforest = self.generic_visit(node)
     return sage.buildPrintStmt(subforest)

  def visit_Str(self, node):
     return sage.buildStringVal(node.s)


def translate(infilename):
  try:
    infile = open(infilename)
    contents = infile.read()
    infile.close()
  except IOError:
    print >>sys.stderr, "IO error when reading file: %s" % infilename
    exit(1)
  syntax_tree = ast.parse(contents)
  sageTree = SageTranslator()(syntax_tree)
  print sageTree
  return sageTree

def main(argv):
  map(translate, argv[1:])

if __name__ == "__main__":
  main(sys.argv)
