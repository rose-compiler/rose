import sys, ast

import sage

class SageTranslator(ast.NodeVisitor):

  def __init__(self, syntax_tree):
    self.visit(syntax_tree)

  def generic_visit(self, node):
    print "Visited %s node" % node.__class__.__name__
    map(self.visit, ast.iter_child_nodes(node))


def translate(infilename):
  try:
    infile = open(infilename)
    contents = infile.read()
    infile.close()
  except IOError:
    print >>sys.stderr, "IO error when reading file: %s" % infilename
    exit(1)
  syntax_tree = ast.parse(contents)
  SageTranslator(syntax_tree)

def main(argv):
  map(translate, argv[1:])

if __name__ == "__main__":
  main(sys.argv)
