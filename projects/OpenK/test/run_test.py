#!/usr/bin/env python

import shlex, subprocess
import os, os.path, sys

CURRENTDIR = os.path.dirname(os.path.realpath(__file__))
#ROOT = os.path.realpath(CURRENTDIR + "/../..")
ROOT = os.path.realpath(CURRENTDIR + "/..")

#REL = "../npb2.3-omp-c/"
REL = "npb2.3-omp-c/"
TESTDIRS_ = ["BT/bt_single.c", "CG/cg_single.c", "EP/ep_single.c", "FT/ft_single.c", "IS/is_single.c", "LU/lu_single.c", "MG/mg_single.c", "SP/sp_single.c"]
TESTDIRS = [REL + d for d in TESTDIRS_]


def run(cmd):
	print(cmd)
	args = shlex.split(cmd)
	p = subprocess.Popen(args)
	p.wait()

CPARSER = ROOT + "rosePrgKnowledgeBuilder/rosePrgKnowledgeBuilder" 
#FLAG = " -c -w -emit-owl "
FLAG = " -c -w -emit-ttl "
def build_kb():
	for mainc in TESTDIRS:
		F = os.path.basename(mainc)
		#INCL = " -I{L}/common" 
		cmd = CPARSER + FLAG + "{}.ttl ".format(F) + mainc
#		cmd = CPARSER + FLAG + "{}.owl ".format(F) + mainc
		run(cmd)

RoseNativeCl = ROOT + "/projects/rose_canonicalloop/roseNativeCl.exe -c -w "
def rose_cl():
	for mainc in TESTDIRS:
		F = os.path.basename(mainc)
		#INCL = " -I{L}/common" # for use the non single version
		cmd = RoseNativeCl + " -report-file {F}_rose_cl_report.txt ".format(F=F) + mainc
		run(cmd)

#--------
def openk_run(runpl, of):
	for mainc in TESTDIRS:
		main_in = os.path.basename(mainc) + ".ttl"
		main_out = main_in + "_openk_{}_report.txt".format(of)
		cmd = runpl + " " + main_in + " " + main_out
		run(cmd)


swipl = "time swipl --nosignal --quiet "  
def openk_cl():
	runpl = swipl + ROOT + "/projects/canonicalloop/run.pl"
	openk_run(runpl, 'of')

#----------
def openk_cfg():
	runpl = swipl + ROOT + "/projects/cfg_test/run.pl"
	openk_run(runpl, 'cfg')


roseCfg = ROOT + "/projects/staticCFG_tests/testStaticCFG.exe "
def rose_cfg():
	for mainc in TESTDIRS:
		cmd = roseCfg + mainc
		run(cmd)


clangCfg = "time clang -cc1 -analyze -analyzer-checker=debug.DumpCFG "
clangNone = "time clang -cc1 -analyze -analyzer-disable-checker=debug.DumpCFG "
def clang_cfg():
	for mainc in TESTDIRS:
		cmd = clangCfg + mainc 
		run(cmd)
		run(clangNone + mainc)

clangLA = "clang -Xclang -analyze -Xclang -analyzer-checker=debug.DumpLiveVars "
def clang_la():
	print "-" * 80
	for mainc in TESTDIRS:
		cmd = clangLA + mainc
		run(cmd)

def clean():
	subprocess.call("rm -f *_report.txt *.ttl", shell=True)

if __name__ == '__main__':

	if len(sys.argv) < 2:
		print sys.argv[0] + ' <option>' 
		print 'clean: clean the result files.'
		print 'build-kb: build knowledge base from the input codes'
		print 'openk-cl: run the openk canonical loop analysis. Must after the build-kb step'
		print 'rose-cl: run the rose native canonical loop analysis'
		print 'openk-cfg: openk cfg'
		sys.exit('exiting')

	invokes = {"clean":clean, "build-kb":build_kb, 
		"rose-cl":rose_cl, "openk-cl":openk_cl, 
		"rose-cfg":rose_cfg, "openk-cfg":openk_cfg, "clang-cfg":clang_cfg, 
		"clang-la": clang_la}

	if sys.argv[1] in invokes:
		invokes[sys.argv[1]]()
	else:
		print "unknown cmd"



