# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

import sys,os,argparse
import subprocess
import re
#credit: https://stackoverflow.com/questions/377017/test-if-executable-exists-in-python



parser = argparse.ArgumentParser(description="Runs clang-query against a set of files")
parser.add_argument("-i",action="store_true",help="Run in interactive mode (don't error on matches, don't run all matchers)",dest="interactive")
parser.add_argument("--die-on-match",action="store_true",help="Return an error code if matches are found",dest="die_on_match")
parser.add_argument("--checkers",action="store",help="List of checkers to always run",dest="checkers")
parser.add_argument("--clang-query",action="store",help="Location of clang-query executable",dest="query_executable")
parser.add_argument("--compilation-database-path",action="store",help="Location of compilation database",dest="compilation_database_directory")
parser.add_argument("--checker-directories",action="store",help="Directories in which checkers can be found",dest="checker_directories")
parser.add_argument("files",nargs="+",help="Files to query")
args = parser.parse_args()
checker_directories = args.checker_directories

if len(checker_directories)==0:
  print("No directories with checkers found")
  sys.exit(1)

def get_all_checkers():
  checker_files = {}
  for directory in checker_directories.split(":"):
    for file_name in os.listdir(directory):
      checker_key = file_name.split("/")[-1]
      checker_files[checker_key] = directory+"/"+file_name
  return checker_files

def get_all_checker_files(checker_dict):
  return [checker_dict[key] for key in checker_dict]

invocation = args.query_executable+" -p="+args.compilation_database_directory+" "
interactive = args.interactive
interpreter = False
arg_checkers = args.checkers
die_on_match = args.die_on_match
available_checkers = get_all_checkers()
checker_files = []
if interactive:
  checker_string = os.getenv("checker")
  if checker_string is None:
    checker_files = get_all_checker_files(available_checkers)
  elif checker_string != "interpreter":
    environment_desired_checkers = checker_string.split(":")
    arg_desired_checkers = []
    if arg_checkers is not None:
      arg_desired_checkers = arg_checkers.split(":")
    all_desired_checkers = arg_desired_checkers + environment_desired_checkers
    desired_checkers = [checker for checker in all_desired_checkers if len(checker)>0] # filter out the empty entries from unspecified sources
    unavailable_checkers = [checker for checker in desired_checkers if checker not in available_checkers]
    if len(unavailable_checkers)>0:
      print("Error, request for checker(s) "+",".join(unavailable_checkers)+" which could not be found")
      sys.exit(1)
    checker_files = [available_checkers[checker] for checker in desired_checkers]
  else:
    interpreter=True
else:
  checker_files = get_all_checker_files(available_checkers)

if len(checker_files)==0 and not interactive:
  print("No static analysis files found in static analysis directories, exiting.")
  sys.exit(1)

for file_name in checker_files:
  invocation+="-f "+file_name+" "

invocation+=" ".join(args.files)
if not interpreter:
  output = subprocess.check_output(invocation, shell=True)
  print(output)
  if die_on_match:
    if re.search("[1-9][0-9]* matche?s?.",output) is not None:
      print("Code fails static analysis CI, exiting")
      sys.exit(1)
else:
  subprocess.call(invocation, shell=True, stdout=sys.stdout)
sys.exit(0)
