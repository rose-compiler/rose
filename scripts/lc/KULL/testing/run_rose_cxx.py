#!/usr/bin/env python
"""
Runs a ROSE tool.  Impersonates CXX.  For use by /kull/systemconf/compilers/
Linux_run_rose_gnu_4_9_3_mvapich2_2_2_compiler.py and similar.

If the tool returns status 0, logs "PASSED" to stdout and copies the command 
and its arguments to kull_testing/passed.txt.  Otherwise, logs "FAILED" to 
stdout, copies the command and its arguments to kull_testing/failed.txt, and 
runs the native compiler.

Returns status 0 whether the tool succeeds or fails.
"""

import argparse
import inspect
import os
from support.local_logging import Logger
from support.runner import Runner

class ROSECXXRunner (object):
    """ Just exports run()
    """

    _SEPARATOR = "================================================================================"

    def __init__(self):
        # Will be a Namespace (e.g. can refer to self._args_defined.command_args):
        self._args_defined = None
        self._args_remaining = []
        self._current_dir = ""
        self._failed_file = None
        self._failed_file_path = ""
        self._logger = Logger( os.path.basename(__file__) + '.' + type(self).__name__ )
        self._parser = None
        self._passed_file = None
        self._passed_file_path = ""
        self._primary_args = []
        self._primary_command = ""
        self._runner = Runner()
        self._script_dir = ""
        self._secondary_args = []
        self._secondary_command = ""

        self._define_args()

    def _define_args(self):
        """ This script passes all its arguments on to the called
        programs, so there are no args defined.
        """
        self._parser = argparse.ArgumentParser(
          prog='run_rose_cxx.py',
          description=__doc__,
          formatter_class=argparse.RawDescriptionHelpFormatter)
        # This matches the first positional and all remaining/following args.
        # We want ALL the arguments, so, we are using parse_known_arguments
        # below instead and commenting this out for now:
        #self._parser.add_argument('command_args', nargs=argparse.REMAINDER)

    def _process_args(self):
        self._args_defined, self._args_remaining = self._parser.parse_known_args()
        self._logger.debug("defined args\n" + str(self._args_defined))
        self._logger.debug("remaining args\n" + str(self._args_remaining))
        self._current_dir = os.getcwd()
        #self._script_dir = os.path.dirname(os.path.abspath(__file__))
        # Robustly get this script's directory, even when started by exec or execfiles:
        script_rel_path = inspect.getframeinfo(inspect.currentframe()).filename
        self._script_dir = os.path.dirname(os.path.abspath(script_rel_path))

# Could execute /bin/bash -c parms, but only if parms are one array element!
#        self._primary_command = '/g/g17/charles/code/ROSE/rose-vanderbrugge1-rose-1398-test-2018-10-30-gcc-4.9.3/bin/identityTranslator'
        self._primary_command = '/g/g17/charles/code/ROSE/rose-0.9.10.91-gcc-4.9.3/bin/identityTranslator'
        self._secondary_command = '/usr/tce/packages/gcc/gcc-4.9.3/bin/g++'
        # self._primary_command = self._secondary_command

        self._primary_args.append(self._primary_command)
        # Helpful rose parameter added by ROSE-1424:
        self._primary_args.append('-rose:no_optimize_flag_for_frontend')
        # # Helpful rose parameter added by ROSE-1392:
        self._primary_args.append('-rose:unparse_edg_normalized_method_ROSE_1392')
        self._primary_args.extend(self._args_remaining)

        self._secondary_args.append(self._secondary_command)
        self._secondary_args.extend(self._args_remaining)

        self._passed_file_path = os.path.join (self._current_dir, "passed.txt")
        self._failed_file_path = os.path.join (self._current_dir, "failed.txt")

    def _log_success(self, args):
        self._logger.success("\n" + self._SEPARATOR + "\nPASSED")
        self._logger.debug("Will log to " + self._passed_file_path + ":")
        self._logger.debug(args)
        self._passed_file.write(str(args) + '\n')

    def _log_failure(self, args):
        self._logger.problem("\n" + self._SEPARATOR + "\nFAILED")
        self._logger.debug("Will log to " + self._failed_file_path + ":")
        self._logger.debug(args)
        self._failed_file.write(str(args) + '\n')

    def _run_command (self, args, dir):
        self._logger.info("\n" + self._SEPARATOR)
        self._runner.callOrLog(args, dir)

    def run(self):
        """ Run the primary command.  If it fails, run the secondary command.  If
        that fails, let the exception (Runner.Failed) propagate.
        """
        self._logger.set_debug_off()
        self._runner.setEffortOnly(False)
        self._process_args()
        self._passed_file = open(self._passed_file_path, 'a')
        self._failed_file = open(self._failed_file_path, 'a')
        try:
            self._run_command(self._primary_args, self._current_dir)
            self._log_success(self._primary_args)
        except Runner.Failed, e:
            self._log_failure(self._primary_args)
            # 2018-10-05: Only run the tool (doing do_sbatch_in_subdir.sh):
            self._run_command(self._secondary_args, self._current_dir)

def main():
    ROSECXXRunner().run()

if __name__ == '__main__':
    main()

