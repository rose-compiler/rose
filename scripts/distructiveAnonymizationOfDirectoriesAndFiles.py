#!/usr/bin/python

# Program:	distructiveAnonymizationOfDirectoriesAndFiles
# Author:	Greg White, LLNL
# Version:	1.2
# Date:		Sept 7, 2008
#
# Tested under Python 2.5.1 on MacOS X 10.5
#
# Notes:
#  1. It is not yet clear how it handles symbolic links.
#  2. File and directory names with '.' are only changed up to the first '.'
#     so "README.interesting_subject.exe" will be "fxxx.interesting_subject.exe".
#     [GKW - can't reproduce this problem with Python 2.5.1]
#  3. Because of #2, all anonymized files should be reviewed to make sure
#     that they still don't carry identifying signatures in their file or
#     directory names.
#  4. Numbering for directory and file numbers is fixed at 6 digits.  We produce a fatal error
#     if we overflow either.  The number of digits should be made adjustable:
#     either by the user or automatically determined by the program.
#  5. Add the ability to change to another start directory from the command line
#  6. What should we do if a file or directory names starts with a period?  I think it should
#     be treated as a filename without an extension. This is not implemented yet.
#  7. Maybe we should split up the program into a dry-run phase, followed by a renaming phase.
#     Right now the fatal errors occur in the middle of the rename.  This is bad.
#  8. We should also think about implementing a fully renamed directory path to the destination
#     of the rename.  This would probably be easier if it happened after the previous item was
#     implemented.  When this is implemented, the printed "after" directory and file names
#     should be changed back to include "after" parent directory names.
#
# Version history:
#  1.0	- First working version
#
#  1.1	- Better comments
#
#  1.2  - Added options:  help, version force, dry-run, quiet, summarize, randomize,
#         and remove-extensions.
#       - Checks to make sure new file and directory names don't already exist before renaming.
#       - Changed "after" directory name in printout to by only the base directory name.  This
#         matches the way file names are displayed.
#       - Renamed to anonymizeDirectoriesAndFiles.py

import os, sys, getopt, random

_removeFileExtensions = False
_dryRun = False
_quiet = False
_summarize = False
_randomizeNumbering = False

def printVersion():
	print "anonymizeDirectoriesAndFiles, version 1.2, Sept 7, 2008"

def printUsage():
    print
    print "WARNING: This is a destructive program which renames directories and files to be a uniquely"
    print "numbered set of names (using numbers and prefixed with 'd' or 'f' for renamed directories or"
    print "files respectively).  Permissions are preserved.  The output of the this program is a list"
    print "of the mappings between the original directory and files names and their new names.  It starts"
    print "renaming in the current directory."
    print
    print "This program performs recursive renaming of directories and files, IN THE CURRENT DIRECTORY." 
    print "In general, it is DANGEROUS and IRREVERSIBLE.  It is HIGHLY recommended to use the --dry-run"
    print "option first."
    print    
    print "usage:     anonymizeDirectoriesAndFiles [options]"
    print "options:"
    print "   -h, --help, --usage         Show this help message and exit"
    print "   -V, --version               Print version number and exit"
    print
    print "   -f, --force                 Don't ask if the user is sure they want to do this"
    print "   -n, --dry-run               No-op: Don't rename directories or files"
    print "                               (sets --force, since nothing will be renamed)"
    print "   -q, --quiet                 Don't print old and new directory and file names"
    print "   -s, --summarize             Print summary of number of directories and files counted"
    print "   -r, --randomize             Randomize numbering for directories and files"
    print "                               (default is a depth first, sequential renumbering from 1."
    print "                                This option results in significantly slower execution times.)"
    print "   -x, --remove-extensions     Remove file extensions"
    print "                               (default is to leave extensions intact)"

def anonymize():
	global _dryRun, _quiet, _summarize, _randomizeNumbering, _removeFileExtensions
	
	numberOfDirectories = 0
	numberOfFiles = 0
	
	nCF = 0
	nCD = 0
	
	if _randomizeNumbering:
		random.seed()
		directoryNumberList = []
		fileNumberList = []
	 
	for root, dirs, files in os.walk(top='.', topdown=False):
		#print "In Dir: ",root
		for name in files:
			ext = os.path.splitext(name)			   # returns a tuple
			numberOfFiles += 1
			if numberOfFiles == 1000000:
				print "Fatal Error:  exceeded 999,999 files"
				sys.exit()
			
			if _randomizeNumbering:
				while True:
					r = random.randint(1,1000000)
					if not r in fileNumberList:
						fileNumberList.append(r)
						break
					# nCF += 1
				newFileName = 'f%(#)06d' % {"#":r}
			else:
				newFileName = 'f%(#)06d' % {"#":numberOfFiles}
			
			if not _removeFileExtensions:
				newFileName += ext[1]
			
			if not _quiet:
				#print "File:\t"+os.path.join(root,name)+"\t"+os.path.join(root,newFileName)
				#print "File:\t"+name+"\t"+newFileName
				print "File:\t"+os.path.join(root,name)+"\t"+newFileName

			if os.path.exists(os.path.join(root,newFileName)):
				print "Fatal Error:  New file name already exists: "+os.path.join(root,newFileName)
				sys.exit()

			if not _dryRun:
				os.rename(os.path.join(root,name), os.path.join(root,newFileName));

		for name in dirs:
			numberOfDirectories += 1
			if numberOfDirectories == 1000000:
				print "Fatal Error:  exceeded 999,999 directories"
				sys.exit()

			if _randomizeNumbering:
				while True:
					r = random.randint(1,1000000)
					if not r in directoryNumberList:
						directoryNumberList.append(r)
						break
					# nCD += 1
				newDirectoryName = 'd%(#)06d' % {"#":r}
			else:
				newDirectoryName = 'd%(#)06d' % {"#":numberOfDirectories}

			if not _quiet:
				print "Dir:\t"+os.path.join(root,name)+"\t"+newDirectoryName
				#print "Dir:\t"+name+"\t"+newDirectoryName

			if os.path.exists(os.path.join(root,newDirectoryName)):
				print "Fatal Error:  New directory name already exists: "+os.path.join(root,newDirectoryName)
				sys.exit()

			if not _dryRun:
				os.rename(os.path.join(root,name), os.path.join(root,newDirectoryName));
	 
	if _summarize:
		print
		print "Files Renamed:      ",numberOfFiles
		print "Directories Renamed:",numberOfDirectories
		# print "nCF ",nCF
		# print "nCD ",nCD



def main(argv):
	global _dryRun, _quiet, _summarize, _randomizeNumbering, _removeFileExtensions
	# _force isn't global
	_force = False

	try:
		opts, args = getopt.getopt(argv, "fhnqsrxV", ["force","help","usage","dry-run","quiet","summary","randomize","remove-extensions","version"])
	except getopt.GetoptError:
		printUsage()
		sys.exit(2)
	for opt, arg in opts:
		if opt in ["-h", "--help","--usage"]:
			 printUsage()
			 sys.exit()
		elif opt in ("-V", "--version"):
			 printVersion()
			 sys.exit()
		elif opt in ("-f", "--force"):
			 _force = True
		elif opt in ("-n", "--dry-run"):
			 _dryRun = True
			 _force = True					# we are not doing any renaming, so it's safe
		elif opt in ("-q", "--quiet"):
			 _quiet = True
		elif opt in ("-s", "--summarize"):
			 _summarize = True
		elif opt in ("-r", "--randomize"):
			 _randomizeNumbering = True
		elif opt in ("-x", "--remove-extensions"):
			 _removeFileExtensions = True

	if not _force:
		print
		print "This program performs recursive renaming of directories and files, IN THE CURRENT DIRECTORY." 
		print "In general, it is DANGEROUS and IRREVERSIBLE.  It is HIGHLY recommended to use the --dry-run"
		print "option first."
		print
		text = raw_input('Please type "yes" to continue: ')
		if text != "yes":
			print
			print "<<<aborting>>>"
			sys.exit()
       		
	anonymize()
 

if __name__ == "__main__":
	main(sys.argv[1:])
