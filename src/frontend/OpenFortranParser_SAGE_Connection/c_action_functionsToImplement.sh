#! /bin/sh

# This script generate the list of functions required to be implemented 
# to support strict dynamic linking that requires even unexecuted functions
# to be present as defined symbols.  This is a difference between the java
# class loaders on our machines at LLNL and the stricter class loaders on
# the NMI machines.
grep c_action_ FortranParserAction.h | cut -d'(' -f1 | sort +1 > actions_to_implement.h
grep "void c_action_" FortranParserActionROSE.C | grep -v "//" | cut -d'(' -f1 | sort +1 > actions_implemented.h
diff -U 200 actions_implemented.h actions_to_implement.h | grep "+" | sed s/"+"//g


