#################################
import os

path = "out/"
files = os.listdir(path)

sheet = {}

rowslength =0


def findType(type,nr):
    global rowslength
    global col
#    print "Looking for ",type,"  rowlength:",rowslength
    found =-1
    for row in range(rowslength):
        if (sheet[(0,row)] == type):
           found =row
#           print ">>>>>>>>>> Column is already contained: ", type
    if (found==-1):
#        print "Adding new column : ", type
        found=rowslength
        sheet[(0,rowslength)] = type
        rowslength=rowslength+1
    sheet[(col, found)] = nr
#    print sheet[(0,found)]," -- ",sheet[col,found]

    return 0


col=0
for inFile in files:
     columnData = []
     print "File: ", inFile," ----------------------------"
     if (inFile.find('PrintAsmInstruction')!=-1):
         col=col+1
         f = file(path + inFile,'r')
         for line in f:
             b = line.split(':')
#             print "  line = ",b
             if len(b)>=2:             
                 row=findType(b[0],b[1])
         f.close()

print "Number of columns: ",col

out = file('output.xls','w')
for item in range(rowslength):
    output=""
    average=0
    count=0
    max=0
    for i in range(col+1):
        value = '0'
        try:
           value = sheet[(i,item)]
        except:
            j=0
        if (i<1):
            output = output + value + "\t"
            max=0        
        if (i>0):
#            print "col ",i,"   value ",value,"  max ",max
            count = count+1
#            print "value = ",value,"  average=",average,"  count=",count
            average = average+int(value)
            if (int(value)>int(max)):
                max = value;
#                print "New max: ", max,"val=",value

    div = str((average/count))
#    print "sum: ",average, " div:",div
    output = div+"\t"+max+"\t"+output
    print output
    out.write(output+"\n")
out.close()
###########################
