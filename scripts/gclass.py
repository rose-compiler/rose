import sys
import string
import os


class valueAccumulator:

  def __init__(self,list):
    self.values= []
    self.nValues= 1
    for i in range(0,len(list)):
      self.values.append(float(list[i]))
    return

  def addValues(self,list):
    self.nValues += 1
    for i in range(0,len(list)):
      self.values[i] += float(list[i])
    return

  def averageValues(self):
    for i in range(0,len(self.values)):
      self.values[i] /= self.nValues
    return

  def getValue(self,index):
    return self.values[index]

  def getValues(self):
    return self.values

  def getnValue(self):
    return self.nValues



def getRange(num,gran):
  return str(int(round(num/gran)))

  range = num / gran
  wRange = int(str(range).split('.')[0])
  dRange = round(float(str(range).split('.')[1]))

  if dRange > wRange:
    return str(wRange + 1)
  else:
    return str(wRange)



def rehash(filename,gran,separator):

  valueHash = {}
  rawData = []
  fin = open(filename,"r")
  line = fin.readline()

  while line != "":
    rawData = string.strip(line).split('\t')
    r = getRange(float(rawData[0]),gran)

    if valueHash.has_key(r):
      valueHash[r].addValues(rawData)
    else:
      valueHash[r] = valueAccumulator(rawData)

    line = fin.readline()
  
  os.rename(filename,filename + ".old")
  fout = open(filename,'a')

  for key in valueHash.keys():
    valueHash[key].averageValues()

    for value in valueHash[key].getValues():
      fout.write(str(value) + separator)

    fout.write('\n')

  return
