#!/bin/env python
"""
Permissions on installed LC ROSE tools must allow users to access, view, and
execute ROSE installation directories and files, as needed. This tool checks
access from the users' point of view.
"""
from __future__ import print_function

import argparse
import os
import stat
import sys

VERSION_STRING = '2017-02-27 13:26'
TEST_LIMIT = 100


class Checker(object):

    # Init support =============================================================

    def __init__(self):
        self._args = None
        self._define_args()
        self.dir_count = 0
        self.exception_count = 0
        self.failed = False
        self.file_count = 0
        self.non_matching_count = 0
        self.path_count = 0
        self.unwanted_write_count = 0

    def _define_args(self):
        parser = argparse.ArgumentParser(
            description='ROSE installation permission checker.  Checks that '
            'Group and Others permissions match User permissions.  Optionally '
            'checks that only the owner should have write permission. Returns'
            '0 on success, 1 if any permission errors or exceptions are detected.',
            epilog='You only need to provide enough of each argument name to '
                   'make it unique. e.g. --i and --input_file are equivalent.  '
        )
        parser.add_argument('-v', '--version', action='version',
                            version='%(prog)s ' + VERSION_STRING)
        parser.add_argument('-c', '--count_only', action='store_true',
                            help='Just count what would be checked, but don\'t do checks')
        parser.add_argument('-d', '--debug', action='store_true',
                            help='For script testing - just processes first 100 paths, emits debug')
        parser.add_argument('-w', '--check_write', action='store_true',
                            help='Check for unwanted group and others write permission')
        # noinspection PyProtectedMember
        parser.add_argument('installation_path',
                            help='Path to ROSE installation to be tested')
        self._parser = parser

    # END Init support =========================================================

    # Run support: =============================================================

    def run(self):
        self._process_args()
        self._do_check()
        if self.failed:
            return 1
        else:
            return 0

    def _process_args(self):
        self._args = self._parser.parse_args()

    def _do_check(self):
        """ Walks all subdirectories and files, checking each"""
        if self._args.count_only:
            print('Would check permissions for "%s" (counting only)' % self._args.installation_path)
        else:
            print('Checking permissions for "%s"' % self._args.installation_path)
        if self._args.debug:
            print ('(Debug is on)')
        for root, dirs, files in os.walk(self._args.installation_path):
            if self._args.debug:
                if self.path_count >= TEST_LIMIT:
                    return
                else:
                    self.path_count += 1
            self.dir_count += len(dirs)
            self.file_count += len(files)
            if not self._args.count_only:
                self._check_paths(root, dirs)
                self._check_paths(root, files)
        if self._args.count_only:
            print('Would check %i files in %i directories' % (self.file_count, self.dir_count))
        else:
            print('Checked %i files in %i directories' % (self.file_count, self.dir_count))
            if self.non_matching_count > 0:
                self.failed = True
            print('Found %i match errors' % self.non_matching_count)
            if self._args.check_write:
                if self.unwanted_write_count > 0:
                    self.failed = True
                print('Found %i write errors' % self.unwanted_write_count)
            if self.exception_count > 0:
                self.failed = True
            print('Got %i exceptions' % self.exception_count)

    def _check_paths(self, root, names):
        for name in names:
            if self._args.debug:
                if self.path_count >= TEST_LIMIT:
                    return
                else:
                    self.path_count += 1
            path = os.path.join(root, name)
            try:
                st = os.stat(path)
                int_mode = st.st_mode
                if self._args.debug:
                    if self._is_dir(st):
                        prefix = 'd'
                    else:
                        prefix = ' '
                    print('%s: %s %s' % (prefix, int_mode, path))
                self._check_write_is_off(path, int_mode)
                self._check_others_match_owner(path, int_mode)
            # Log any OS (hopefully file system) exceptions and return normally:
            except OSError:
                self.exception_count += 1
                e = sys.exc_info()
                print('EXCEPTION: %s; %s' % (e[0], e[1]))

    def _check_write_is_off(self, path, mode):
        if self._args.check_write:
            if self._write_enabled(mode):
                self.unwanted_write_count += 1
                print('ERROR - has non-owner write permission: "%s"' % path)

    def _check_others_match_owner(self, path, mode):
        if not self._others_match_owner(mode):
            self.non_matching_count += 1
            print('ERROR - others permissions do not match owner permission: "%s"' % path)

    @staticmethod
    def _write_enabled(mode):
        return bool((mode & stat.S_IWGRP) | (mode & stat.S_IWOTH))

    @staticmethod
    def _others_match_owner(mode):
        ur = bool(mode & stat.S_IRUSR)
        orr = bool(mode & stat.S_IROTH)
        ux = bool(mode & stat.S_IXUSR)
        ox = bool(mode & stat.S_IXOTH)
        if False:
            ur_eq_orr = ur == orr
            ux_eq_ox = ux == ox
            both_match = ur_eq_orr & ux_eq_ox
            print ('ur: %s, or: %s, ur_eq_orr: %s; ux: %s, ox: %s, ux_eq_ox: %s; both_match: %s' %
                   (ur, orr, ur_eq_orr, ux, ox, ux_eq_ox, both_match))
        return bool((ur == orr) & (ux == ox))

    @staticmethod
    def _is_dir(st):
        return bool(st.st_mode & stat.S_IFDIR)

if __name__ == '__main__':
    exit(Checker().run())
    
