#!/usr/bin/env python
""" Runs run_rose_cxx.py with --rose:c11_only.
"""

import os
from support.local_logging import Logger
from run_rose_cxx import ROSECXXRunner

class ROSECCRunner (ROSECXXRunner):

    def __init__(self):
        """ Name the logger after this script and class, not the parent.
        """
        super( ROSECCRunner, self ).__init__()
        self._logger = Logger( os.path.basename(__file__) + '.' + type(self).__name__ )

    def _process_args(self):
        """ Call the parent and then add a parameter.
        Use gcc, not g++
        """
        super( ROSECCRunner, self )._process_args()
        self._secondary_command = "/usr/tce/packages/gcc/gcc-4.9.3/bin/gcc"
	self._secondary_args = [self._secondary_command] + self._args_remaining

def main():
    ROSECCRunner().run()

if __name__ == '__main__':
    main()


