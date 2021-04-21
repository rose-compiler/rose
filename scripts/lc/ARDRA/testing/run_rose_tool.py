#!/usr/bin/env python
"""Runs a ROSE tool.  If the tool does not return status 0, then runs the 
corresponding non-ROSE compiler.  Records whether the tool succeeded, in 
passed.txt and failed.txt, but always returns status 0.
"""
import argparse
import inspect
import os
from support.local_logging import Logger
from support.runner import Runner

_SEPARATOR = "================================================================================"

class ROSERunner (object):

    def __init__(self):
        # Will be a Namespace (e.g. can refer to self._args_defined.command_args):
        self._args_defined = None
        # Will be a list:
        self._args_remaining = None
        self._current_dir = ""
        self._failed_file = None
        self._failed_file_path = ""
        self._logger = Logger("run_rose_tool.ROSERunner")
        self._parser = None
        self._passed_file = None
        self._passed_file_path = ""
        self._primary_command = ""
        self._runner = Runner()
        self._script_dir = ""
        self._secondary_command = ""

        self._define_args()

    def _define_args(self):
        """ This script passes all its arguments on to the called
        programs, so there are no argus defined.
        """
        parser = argparse.ArgumentParser(
            description="""Runs a ROSE tool.  If the tool does not return status 0, then runs the 
corresponding non-ROSE compiler.  Records whether the tool succeeded, in 
passed.txt and failed.txt, but always returns status 0.
""")
        # We want ALL the arguments, so, we are using parse_known_arguments
        # below instead and commenting this out for now:
        ## This matches the first positional and all remaining/following args:
        #parser.add_argument('command_args', nargs=argparse.REMAINDER)
        self._parser = parser

    def _process_args(self):
        self._args_defined, self._args_remaining = self._parser.parse_known_args()
        self._logger.debug("defined args\n" + str(self._args_defined))
        self._logger.debug("remaining args\n" + str(self._args_remaining))
        self._current_dir = os.getcwd()
        #self._script_dir = os.path.dirname(os.path.abspath(__file__))
        # Robustly get this script's directory, even when started by exec or execfiles:
        script_rel_path = inspect.getframeinfo(inspect.currentframe()).filename
        self._script_dir = os.path.dirname(os.path.abspath(script_rel_path))

        self._primary_command = "/g/g17/charles/code/ROSE/rose-0.9.10.64-intel-18.0.1.mpi/tutorial/identityTranslator"
        self._secondary_command = "/usr/tce/packages/mvapich2/mvapich2-2.2-intel-18.0.1/bin/mpicxx"
        self._passed_file_path = os.path.join (self._script_dir, "passed.txt")
        self._failed_file_path = os.path.join (self._script_dir, "failed.txt")

    def _log_success(self, args):
        self._logger.success("\n" + _SEPARATOR + "\nPASSED")
        self._logger.debug("Will log to passed file:")
        self._logger.debug(args)
        self._passed_file.write(str(args) + '\n')

    def _log_failure(self, args):
        self._logger.problem("\n" + _SEPARATOR + "\nFAILED")
        self._logger.debug("Will log to failed file:")
        self._logger.debug(args)
        self._failed_file.write(str(args) + '\n')

    def _run_command (self, args, dir):
        self._logger.info("\n" + _SEPARATOR)
        self._runner.callOrLog(args, dir)

    def run(self):
        """ Run the primary command.  If it fails, run the secondary command.  If
        that fails, let the exception (Runner.Failed) propagate.
        """
        self._logger.set_debug_off()
        #self._logger._logger.setLevel(Logger.ERROR)
        self._process_args()
        self._passed_file = open(self._passed_file_path, 'a')
        self._failed_file = open(self._failed_file_path, 'a')
        try:
            primary_args = [self._primary_command] + self._args_remaining
            self._run_command(primary_args, self._current_dir)
            self._log_success(primary_args)
        except Runner.Failed, e:
            self._log_failure(primary_args)
            secondary_args = [self._secondary_command] + self._args_remaining
            self._run_command(secondary_args, self._current_dir)

def main():
    ROSERunner().run()

if __name__ == '__main__':
    main()

