#
# This file is part of Canviz. See http://www.canviz.org/
# $Id: insert_file.awk 256 2009-01-08 11:14:07Z ryandesign.com $
#

BEGIN {
	replacement = ""
}

NR==FNR {
	gsub("\&", "\\\\\&", $0)
	replacement = replacement $0 "\n";
	next;
}

{
	gsub(placeholder, replacement, $0)
	print
}
