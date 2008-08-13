#################################
import os

print "generating generate_checkers.C from CHECKER_LIST"
filename = "CHECKER_LIST"
f = file(filename,'r')
if (f):
    print " found file : ",filename
out = file("generate_checkers.C",'w')
for line in f:
    t = line
    text = t.split('\n')
    text = "generate_checker("+text[0]+");"
    print "   parsing line: ",text
    out.write(text+"\n")
out.close()
f.close()
