#!/usr/bin/python
#
"""
Filter a GraphViz dot graph to contain only nodes that are at most N
hops away from one or more specified central node(s).

Uses pydot: http://code.google.com/p/pydot/

"""

import pydot, dot_parser, argparse, sys

__version__ = '1.0'
__license__ = 'BSD'
__author__  = 'Adrian Prantl'

def neighbours(n, dist, edges):
    if dist == 0: return [n];

    if n not in edges.keys():
        return [n]

    ns = [n]
    for m in edges[n]:
        ns.extend(neighbours(m, dist-1, edges))
    return ns

if __name__ == '__main__':
    # Command line argument handling
    cmdline = argparse.ArgumentParser(
        description=__doc__,
        epilog='Please report bugs to <adrian@llnl.gov>.')

    cmdline.add_argument('-i','--input',    metavar='<in.dot>',      type=file, help='input graph')
    cmdline.add_argument('-o','--output',   metavar='<out.dot>',                help='output graph')
    cmdline.add_argument('-c','--center',   metavar='<label>', nargs='+',       help='center node')
    cmdline.add_argument('-d','--distance', metavar='<N>', default=3, type=int, help='distance N')
    cmdline.add_argument('-v','--verbose', action='store_true', help='verbose mode')

    args = cmdline.parse_args()

    if not args.input:  print 'no input specified!'; exit(1)
    if not args.output: print 'no output specified!'; exit(1)

    print "reading file", args.input.name
    g = dot_parser.parse_dot_data(args.input.read())

    centers = []
    for c in args.center:
        centers.extend(g.get_node(c))
    if len(centers) == 0: print "center node <%s> not found"%args.center; exit(1)

    edges = dict()
    for e in g.get_edges():
        src = e.get_source()
        dest = e.get_destination()
        edges[src]  = [dest] + (edges[src] if src  in edges.keys() else [])
        edges[dest] = [src] + (edges[dest] if dest in edges.keys() else [])

    num=0
    marked = []
    dist_from_center = dict()
    for center in centers:
        marked.extend(neighbours( center.get_name(), args.distance, edges))

    #print "leaving", marked
    for n in g.get_nodes():
        if n.get_name() not in marked:
            g.del_node(n)
            num += 1
    print 'deleted', num, 'nodes'

    num=0
    for e in g.get_edges():
        src = e.get_source()
        dest = e.get_destination()
        if (src not in marked) or (dest not in marked):
            g.del_edge(src, dest)
            num += 1
    print 'deleted', num, 'edges'

    print "writing file", args.output
    g.write(args.output)
