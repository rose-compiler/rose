#
# This file is part of Canviz. See http://www.canviz.org/
# $Id: layoutlist.awk 246 2008-12-27 08:36:24Z ryandesign.com $
#

{
	print "/*"
	print " * This file is part of Canviz. See http://www.canviz.org/"
	print " */"
	print ""
	print "var layouts = ["
	for (i = 1; i <= NF; ++i) {
		line = "'" $i "'"
		if (i != NF) {
			line = line ","
		}
		print line
	}
	print "];"
}
