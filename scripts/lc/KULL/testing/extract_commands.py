#!/usr/bin/env python
"""Extracts commands from a Kull make log to be used by do_sbatch_in_subdir.sh.

Takes a log file from a make that used run_rose_cxx.py, Looks for the args=[ 
and the dir=[ lines, and outputs a file with one equivalent runnable command 
per line.
"""

import argparse
import os
import re
import sys
from support.local_logging import Logger

class Command_Extractor (object):
    def __init__(self):
        # Name the logger after this file and this class (works for child class too):
        self._logger = Logger(os.path.basename(__file__) + '.' + type(self).__name__)
        self._parser = None
        self._args = []

        self._command_file = None
        self._command_file_path = ""
        self._log_file = None
        self._log_file_path = ""

        self._args_line_count = 0
        self._args_line_has_tool = False
        self._args_pattern = re.compile (r"^args=")
        self._dir_line = ""
        self._dir_line_count = 0
        self._dir_pattern = re.compile (r"^dir=")
        self._tool_args_line = ""
        self._tool_line_count = 0
        self._tool_pattern = re.compile (r"identityTranslator")

        self._define_args()

    def _define_args(self):
        """ Just need input file and output file.  If there is no output file, use stdout.
        """
        self._parser = argparse.ArgumentParser(
          prog='extract_commands.py',
          description=__doc__,
          formatter_class=argparse.RawDescriptionHelpFormatter)
        self._parser.add_argument( 'log_file_path', nargs='?', default='stdin',
                                   metavar='PATH1', help="log file path (input - default stdin)")
        self._parser.add_argument( 'command_file_path', nargs='?', default='stdout',
                                   metavar='PATH2', help="command file path (output - default stdout)")

    def _process_args(self):
        args=self._parser.parse_args()
        self._log_file_path=args.log_file_path
        self._command_file_path=args.command_file_path

    def run(self):
        """ Open files as needed, process each line in _log_file_path,
        print stats.
        """
        self._logger.set_debug_off()
        self._process_args()

        if self._command_file_path == 'stdout':
            self._command_file = sys.stdout
        else:
            self._command_file = open(self._command_file_path, 'w')

        if self._log_file_path == 'stdin':
            for line in sys.stdin:
                self._process_line(line)
        else:
            with open (self._log_file_path, 'r') as log_file:
                for line in log_file:
                    self._process_line(line)

        print ("args lines: %i" % self._args_line_count)
        print ("tool lines: %i" % self._tool_line_count)
        print ("dir lines: %i" % self._dir_line_count)

    def _process_line(self, line):
        """ If the log line has our tool and a run dir, make a command line.
        """
        if self._args_pattern.search(line):
            self._args_line_count += 1
            if self._tool_pattern.search(line):
                self._tool_line_count += 1
                self._tool_args_line=line
                self._args_line_has_tool=True
            else:
                self._args_line_has_tool=False
        if self._dir_pattern.search(line) and self._args_line_has_tool:
            self._dir_line_count += 1
            self._dir_line = line
            self._command_file.write (self._make_command(self._tool_args_line, self._dir_line) + '\n')

    def _make_command(self, args_line, dir_line):
        """ Make a command line like: "(cd [some dir]; ...run_rose_cxx.py [parms])".
        """
        result = ''
        # arg_line is: "args=[ ... ],"
        args=eval(args_line [5:-2])
        # dir_line is: "dir= ... )"
        dir = dir_line [4:-2]
        result = result + \
                 'cd ' + dir + '; '
        # Prepend the call to mpi:
        result = result + \
                 '/usr/dnta/kull/developers/tools/compilers/mvapich2-2.2/gcc-4.9.3p/mpicxx ' + \
                 '-cxx=/g/g17/charles/code/KULL/kull_testing/run_rose_cxx.py'
        # Skip the first arg, which is the original tool/compiler call:
        for arg in args [1:-1]:
            result = result + ' ' + arg
        # Enclose the commands in ( ... ) because eval likes that:
        result = "(" + result + ")"
        return result

def main():
    Command_Extractor().run()

if __name__ == '__main__':
    main()

