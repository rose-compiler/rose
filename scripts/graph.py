#!/usr/bin/env python
#
###############################################################################
#
# Author: Gary Yuan
# Date: 8/24/2006
# File: graph.py
# Purpose: Plots ROSE performance data 
#
###############################################################################

import sys
import os
import string
import getopt
import csv
import math
from gclass import *
from optparse import *

###############################################################################

def getHash(rawData):
  """
  """

  hash = dict([ (rawData[i],rawData[i+1]) for i in range(0,len(rawData)-1,2) ])

  return hash

##############################################################################

def generateGraph(reader,fout,xKey,yExclude,yInclude,separator):
  """
  """

  if yInclude != []:
    keys = yInclude
  
  for row in reader:
    hash = getHash(row)
    data = ""

    if yInclude == [] and yExclude != []:
      keys = hash.keys()
      keys.remove(xKey)

      for y in yExclude:
        keys.remove(y)

    for key in keys:
      data = data + separator + hash[key]

    fout.write(hash[xKey] + data + '\n')

  return keys

###############################################################################

def csv2gnuplot(inputs,output,xKey,yExclude,yInclude,xlabel,ylabel,
		Format,height,width,pointsize,with,
		yMin,yMax,xMin,xMax):
  """
  """

  fout = open(output + ".dat",'a')
  
  for i in inputs:
    reader = csv.reader(open(i,'r'),doublequote=True,skipinitialspace=True)
    keys = generateGraph(reader,fout,xKey,yExclude,yInclude,"\t")

  # gnuplot formatting
  index = 0
  fscriptout = open(output + ".plt",'a')

  while index < len(keys):
    if index > 0:
      fscriptout.write("re")

    fscriptout.write("plot \"" + output + ".dat" +"\" using 1:" + 
	str(index + 2) + ' ') 

    if with != "":
      fscriptout.write("smooth csplines ")
      fscriptout.write("title \"" + keys[index] + "\"" + ' ') 
      fscriptout.write("with " + with + '\n')
    else:
      fscriptout.write("title \"" + keys[index] + "\"" + '\n')

    index += 1
  # while

  if Format != "":
    fscriptout.write("set terminal " + Format + '\n')
    fscriptout.write("set output \"" + output + '.' + Format + "\"\n")

  if xMin != "" or xMax != "":
    fscriptout.write("set xrange [" + xMin + ':' + xMax + "]\n")

  if yMin != "" or yMax != "":
    fscriptout.write("set yrange [" + yMin + ':' + yMax + "]\n") 
  
  if xlabel != "":
    fscriptout.write("set xlabel \"" + xlabel + "\"\n")
  else:
    fscriptout.write("set xlabel \"" + xKey + "\"\n")

  if ylabel != "":
    fscriptout.write("set ylabel \"" + ylabel + "\"\n")
  # if
 
  fscriptout.write("set key below\nset key box\n") 
  fscriptout.write("set size " + width + ',' + height + '\n')
  fscriptout.write("set pointsize " + pointsize + '\n')
  fscriptout.write("replot\n")

  # end gnuplot formatting

  return
  # cvs2gnuplot()

###############################################################################

def csv2excel(inputs,output,xKey,yExclude):
  """
  """
  fout = open(output + ".csv",'a')

  for i in inputs:
    reader = csv.reader(open(i,'r'),doublequote=True,skipinitialspace=True)
    generateGraph(reader,fout,xKey,yExclude,',')

  return

###############################################################################

def csv2matlab(inputs,output,xKey,yExclude,xlabel,ylabel,height,width,Format):
  """
  """

  fout = open(output + ".dat",'a')
  
  # Matlab data
  for i in inputs:
    reader = csv.reader(open(i,'r'),doublequote=True,skipinitialspace=True)
    keys = generateGraph(reader,fout,xKey,yExclude,"  ")

  # Matlab script
  fscriptout = open(output + ".m",'a')
  index = 2
  ceilSqrt = int(math.ceil(math.sqrt(len(keys))))

  if xlabel == "":
    xlabel = xKey

  fscriptout.write("load " + output + ".dat" + '\n')
  fscriptout.write("set(gcf,'position',[0 0 " + str(width) + ' ' + 
			str(height) + "])\n")
  fscriptout.write("x = " + output + "(:,1)\n")

  while index < len(keys) + 2:
    fscriptout.write("y" + str(index) + " = " + output + "(:," 
			+ str(index) + ")\n")
    fscriptout.write("xlabel('" + xlabel + "')\n")
    fscriptout.write("ylabel('" + ylabel + "')\n")
    #fscriptout.write("ylabel('" + keys[index - 2] + "')\n")
    fscriptout.write("subplot(" + str(ceilSqrt) + ',' + str(ceilSqrt) + 
			',' + str(index - 1) + ") ; ")
    fscriptout.write("plot(x,y" + str(index) + ",'o')\n")
    fscriptout.write("legend('" + keys[index - 2] + "')\n")
    index += 1

  if Format != "":
    fscriptout.write("set(gcf,'PaperPositionMode','auto')\n")
    fscriptout.write("print(gcf,'-d" + Format + "'," + '\'' + 
			output + '.' + Format + "')\n")
    fscriptout.write("quit\n")
  # Matlab script

  return

###############################################################################

def cmdOptionParse(parser):
  """
    cmdOptionParse():
	Parses command-line arguments and redirects to appropriate functions.

	arguments:
	parser -- a optparse object that stores command-line arguments
  """

  # parse out options and input file arguments
  (options,inputs) = parser.parse_args()

  if inputs == []:
    sys.stderr.write("Error: No input file(s) specified\n")
    sys.exit(1)

  if options.output != "":
    output = options.output.split('.')[0]
  else:
    sys.stderr.write("Error: No output file name specified\n")
    sys.exit(1)

  if options.list:
    print "Supported formats:"
    print "1. Gnuplot (.dat .plt) -fgnuplot"
    print "2. MS Excel (.csv) -fexcel"
    print "3. Matlab (.dat) -fmatlab"
    sys.exit(0)

  if options.x == "":
    sys.stderr.write("Error: X-Axis data not specified, please specify with -x\n")
    sys.exit(1)
  # if, error checking

  if options.format == "gnuplot":
    if options.e != [] and options.y != []:
      sys.stderr.write("Error: Options -e and -y may not be used concurrently\n")
      sys.exit(1)

    csv2gnuplot(inputs,output,options.x,options.e,options.y,
		options.xlabel,options.ylabel,options.Format,
		options.Height,options.Width,options.pointsize,
		options.with,options.ymin,options.ymax,
		options.xmin,options.xmax)

    if options.rehash != "" and options.granularity != "":
      rehash(output + ".dat",string.atoi(options.granularity),'\t')
    elif options.rehash != "" and options.granularity == "":
      sys.stderr.write("Error: You must specifiy -g or --granularity with --rehash\n")
      sys.exit(1)

    if options.run:
      args = []
      args.append("")
      args.append(output + ".plt")
      os.execvp("gnuplot",args)
    # if
  # if
  elif options.format == "excel":
    csv2excel(inputs,options.output,options.x,options.e)
  elif options.format == "matlab":
    csv2matlab(inputs,options.output,options.x,options.e,
		options.xlabel,options.ylabel,
		options.Height,options.Width,
		options.Format)

    if options.run:
      args = []
      args.append("")
      args.append("-nodesktop")
      args.append("-r")
      args.append(output)
      os.execvp("matlab",args)
  else:
    sys.stderr.write("Error: Unrecognized output format\n")

  return

###############################################################################

def cmdOptionInit(arguments):
  """
    cmdOptionInit():
	Initializes command-line parser optparse object. Specifies which option
	flags behave in what way according to optparse.

	arguments:
	arguments -- sys.argv list of command-line arguments

	variables:
	parser -- optparse, OptionParser()
  """
  parser = OptionParser()

  parser.set_usage("graph.py <input file> [options]")

  parser.add_option("-f","--format",help="Output file format",
			metavar="%FORMAT%")
  parser.add_option("-F","--Format",help="Secondard output format",
			metavar="%FORMAT%",default="")
  parser.add_option("-l","--list", help="List supported output formats",
			action="store_true")
  parser.add_option("-o","--output",help="Output file name",metavar="%FILE%",
			default="")
  parser.add_option("-r","--run",help="Run plotting tool",action="store_true")
  parser.add_option("-x",help="X Axis Key Data",metavar="<XKEY>",default="")
  parser.add_option("-y",help="Include Y Axis Data",metavar="<KEY>",
			action="append",default=[])
  parser.add_option("-e",help="Exclude Y Axis Data",metavar="<KEY>",
			action="append",default=[])
  parser.add_option("-g","--granularity",
			help="granularity range for data manipulation",
			metavar="<#>",default="")
  parser.add_option("-w","--with",help="With lines,points,etc.",
			metavar="%WITH%",default="")
  parser.add_option("-H","--Height",help="Output Height default=1",
			metavar="<H#>",default="1")
  parser.add_option("-W","--Width",help="Output Width default=1",
			metavar="<W#>",default="1")
  parser.add_option("-P","--pointsize",help="Set pointsize default=1",
			metavar="<P#>",default="1")
  parser.add_option("--rehash",help="Rehash Data",metavar="%MODE%",
			default="")
  parser.add_option("--xlabel",help="X-Axis Label",metavar="%LABEL%",
			default="")
  parser.add_option("--xmin",help="Minimum X range value",metavar="<#>",
			default="")
  parser.add_option("--xmax",help="Maximum X range value",metavar="<#>",
			default="")
  parser.add_option("--ylabel",help="Y-Axis Label",metavar="%LABEL%",
			default="")
  parser.add_option("--ymin",help="Minimum Y range value",metavar="<#>",
			default="")
  parser.add_option("--ymax",help="Maximum Y range value",metavar="<#>",
			default="")

  return parser

###############################################################################
###############################################################################

parser = cmdOptionInit(sys.argv)
cmdOptionParse(parser)

# control flow: 
# main->cmdOptionInit->main->cmdOptionParse->csv2{}->generateGraph<->getHash()
