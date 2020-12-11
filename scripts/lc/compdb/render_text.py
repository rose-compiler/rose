#!/usr/bin/env python2
"""Creates a textual summary file from the JSON output of comp_db_map

Glossary:
Job: The whole comp_db_map process/result, consisting of multiple Runs
     Data: source dir, build dir, tool command, tool name, tool version
Run: One execution of the tool.
     Data: in file, out file, command line arguments (tool command is first 
     one), elapsed time, stdout, stderr
     One "report"
"""

import argparse
import inspect
import json
import os
import re
import sys
# Add the parent directory to the search path:
sys.path.insert(1, os.path.join(sys.path[0], '..'))
from support.local_logging import Logger


class KeyValueMapSorted (object):
    """Key value map, sorted by Key
    """
    def __init__(self):
        self._key_value_map = dict()
        self._keys = list()
        self._values = list()
        self._sort_needed = False

    def count(self):
        return len(self._key_value_map)

    def add(self, name, value):
        if name in self._key_value_map:
            self._key_value_map[name] = value
        else:
            self._key_value_map[name] = value
            self._keys.append(name)
            self._sort_needed = True

    def _sort_if_needed(self):
        if self._sort_needed:
            self._keys.sort()
            self._values = list()
            for name in self._keys:
                self._values.append(self._key_value_map[name])
            self._sort_needed = False

    def to_value(self, name):
        return self._key_value_map[name]

    def values_by_key(self):
        self._sort_if_needed()
        return self._values

    def keys_sorted(self):
        self._sort_if_needed()
        return self._keys


class NameIdMapSorted (KeyValueMapSorted):
    """Name to id map, sorted by name
    """
    def to_id(self, name):
        return self.to_value(name)

    def ids_by_name(self):
        return self.values_by_key()

    def names_sorted(self):
        return self.keys_sorted()


class KeyCountMapSorted(object):
    """Keys and their counts, ordered by _counts_
    """

    def __init__(self):
        self._counts = list()
        self._sort_needed = False
        self._key_count_map = dict()
        self._keys = list()

    def count(self):
        return len(self._keys)

    def increment(self, key):
        if key in self._key_count_map:
            self._key_count_map[key] += 1
        else:
            self._key_count_map[key] = 1
            self._keys.append(key)
        self._sort_needed = True

    def _sort_if_needed(self):
        if self._sort_needed:
            self._keys.sort()
            # Remove duplicates and sort:
            self._counts = sorted(set(self._key_count_map.values()))
            self._counts.reverse()
            self._sort_needed = False

    def counts_and_keys_by_count(self):
        """Return a list of (count, string) tuples sorted by count, then key.
        """
        result = list()
        self._sort_if_needed()
        for count in self._counts:
            for key in self._keys:
                if self._key_count_map[key] == count:
                    result.append((count, key))
        return result

    def keys_sorted(self):
        return sorted(self._keys)

    def count_for_key(self, key):
        if key in self._key_count_map:
            return self._key_count_map[key]
        else:
            return 0

# Numbers below are for sort ordering:
SUCCESS = 'Success'
TOOL_ERROR = 'Tool Error'
BACKEND_ERROR = 'Backend Error'
EXCEPTION = 'Python Exception'
RESULT_KINDS=[SUCCESS, TOOL_ERROR, BACKEND_ERROR, EXCEPTION]

def make_id(file_path, arguments):
    """Returns the file name without the path, followed by a hash of the arguments mod 1000. Leaves out the first
    argument, which should be the tool command iteself.
    """
    return "{:s}_{:0>3d}".format(
        os.path.basename(file_path),
        hash(' '.join(arguments[1:])) % 1000)

def write_begin_end(message):
    """Decorator factory, returning a begin-end decorator with the desired message.
    Can only be used on functions of a class that has self.write_end and self.write_begin.
    Usage:
    @write_begin_end('Summary')
    def _write_summary(self):
    ...
    """
    def decorator(func_to_be_wrapped):
        """Decorator, returning a wrapped function
        """
        def wrapper(self, *args, **kwargs):
            """The wrapped function
            """
            self.write_begin(message)
            func_to_be_wrapped(self, *args, **kwargs)
            self.write_end(message)
        return wrapper
    return decorator


class Renderer (object):

    def __init__(self):
        # Standard init stuff
        # _args will be a Namespace (e.g. can refer to self._args.some_arg):
        self._args = None
        self._current_dir = ''
        self._logger = Logger(
            name='render_text.Renderer',
            show_date=False,
            show_time=False)
        self._parser = None
        self._script_dir = ''

        self._all_main_stderr_lines = list()
        self._all_stderr_texts = list()
        self._first_occurrences_per_error = KeyCountMapSorted()
        self._json_in = None
        self._run_id_to_report_index = dict()
        self._occurrences_per_error = KeyCountMapSorted()
        self._runs_per_result_kind = KeyCountMapSorted()
        self._runs_per_return_code = KeyCountMapSorted()

        self._define_args()

    def _define_args(self):
        """ Set up the command-line arg parser.
        """
        parser = argparse.ArgumentParser(
            formatter_class=argparse.RawTextHelpFormatter,
            description=__doc__,
            epilog='NOTE: The argument names may be shortened, as long as they are unique')

        parser.add_argument(
            '--in_file',
            type=argparse.FileType('r'),
            default='report.json',
            metavar='<path>',
            help='Report generated by comp_db_map (default: %(default)s)')

        parser.add_argument(
            '--out_file',
            type=argparse.FileType('w'),
            default='report.txt',
            metavar='<path>',
            help='Text file output location (default: %(default)s)')

        parser.add_argument(
            '--debug',
            action='store_true',
            help='Turns on debug output'
        )

        self._parser = parser

    def _process_args(self):
        """Parse the command-line args, store them, and finish initializing
        """
        self._args = self._parser.parse_args()
        if self._args.debug:
            self._logger.set_debug_on()
        # Contains NO softlinks (e.g. /usr/WS2/charles/code):
        # self._current_dir = os.getcwd()
        # Contains softlinks (e.g. /g/g17/charles/code/):
        self._current_dir = os.environ['PWD']
        # Contains softlinks (e.g. /g/g17/charles/code/):
        # self._script_dir = os.path.dirname(os.path.abspath(__file__))
        # Instead of above,robustly get this script's directory, even when started by exec or execfiles:
        script_rel_path = inspect.getframeinfo(inspect.currentframe()).filename
        self._script_dir = os.path.dirname(os.path.abspath(script_rel_path))

        self._logger.debug('self._args: ' + str(self._args))
        self._logger.debug('self._current_dir: ' + str(self._current_dir))
        self._logger.debug('self._script_dir: ' + str(self._script_dir))

    def total_units(self):
        return len(self.run_reports())

    def run_reports(self):
        return self._json_in['trans-units']

    def sorted_run_ids(self):
        return sorted(self._run_id_to_report_index.iterkeys())

    def report_from_run_id(self, run_id):
        run_reports = self.run_reports()
        index = self._run_id_to_report_index[run_id]
        return run_reports[index]

    def run(self):
        """ Create the report
        """
        self._process_args()
        try:
            self._json_in = json.load(self._args.in_file)
        except ValueError as e:
            self._logger.error('Exception {} raised doing json.load on "{}"'.
                               format('ValueError', self._args.in_file.name))
            self._logger.error('Message: "{}"'.format(e.message))
            self._logger.error('Exiting')
            exit(1)
        self._generate_report()

    def _generate_report(self):
        self._categorize_executions()
        self._write_summary()
        self._write_run_lists()
        self._write_run_reports()
        # self._write_stderr_texts()
        # self._write_not_backend_error_units()
        # self._write_main_error_lines()
        self._write_error_histogram()
        self._write_first_error_histogram()

    def _categorize_executions(self):
        """Adds status to each translation unit's JSON
        Adds counts by status
        Creates a key, sorted by file name, of unit ids
        Identifies backend errors.
        """
        def add_to_json(field_name, value):
            self._json_in['trans-units'][report_index][field_name] = value

        # Detect status and create a key sorted by filename:
        for (report_index, report) in enumerate(self.run_reports()):
            # Do run_id:
            run_id = make_id(report['file'], report['arguments']['tool'])
            # Be sure run_id is unique:
            assert (run_id not in self._run_id_to_report_index)
            self._run_id_to_report_index[run_id] = report_index
            add_to_json('run_id', run_id)

            if 'exception' in report:
                result_kind = EXCEPTION
                # Return code never gets set on comp_db_map.py:
                return_code=129
            else:
                return_code = report['returncode']

            self._runs_per_return_code.increment(return_code)

            if return_code == 0:
                result_kind = SUCCESS
            else:
                # Identify backend errors
                if 'err' in report and len(report['err']) > 0:
                    if 'rose_' in report['err']:
                        result_kind = BACKEND_ERROR
                    else:
                        result_kind = TOOL_ERROR
                else:
                    result_kind = TOOL_ERROR

            add_to_json('result_kind', result_kind)
            self._runs_per_result_kind.increment(result_kind)

    @write_begin_end('Summary')
    def _write_summary(self):

        def write_return_code_counts():
            self.writeln('Return codes:')
            for count, return_code in self._runs_per_return_code.counts_and_keys_by_count():
                percent = float(count) / float(self.total_units())
                self.writeln_and_debug('{:>4d} ({:>3.0%}): {:s}'.format(count, percent, str(return_code)))
            self.writeln('')

        def write_result_kind_counts():
            self.writeln('Result kinds:')
            for count, result_kind in self._runs_per_result_kind.counts_and_keys_by_count():
                percent = float(count) / float(self.total_units())
                self.writeln_and_debug('{:>4d} ({:>3.0%}): {:s}'.format(count, percent, str(result_kind)))
            self.writeln('')

        def write_has_stderr_and_return_code_0_count():

            def has_stderr_and_return_code_0():
                return 'returncode' in run_report and run_report['returncode'] == 0 and \
                       'err' in run_report and len(run_report['err']) > 0

            count = 0
            for run_id in self.sorted_run_ids():
                run_report = self.report_from_run_id(run_id)
                if has_stderr_and_return_code_0():
                    count += 1
            percent = float(count) / float(self.total_units())
            self.writeln_and_debug('{:>4d} ({:>3.0%}) had stderr output and return code 0'.
                         format(count, percent))
            self.writeln('')
            
        def write_has_error_in_stdout_count():

            def has_error_in_stdout():
                if 'out' in run_report and len(run_report['out']) > 0:
                    for line in run_report['out']:
                        if line[0:1] == 'No':
                            return True
                return False

            count = 0
            for run_id in self.sorted_run_ids():
                run_report = self.report_from_run_id(run_id)
                if has_error_in_stdout():
                    count += 1
            percent = float(count) / float(self.total_units())
            self.writeln_and_debug('{:>4d} ({:>3.0%}) had "ERROR:" in stdout'.
                         format(count, percent))
            self.writeln('')

        report = self._json_in
        tool = os.path.basename(report['tool']['command'])

        self.writeln_and_debug('This file        : {:s}'.format(self._args.out_file.name))
        self.writeln_and_debug('Input file       : {:s}'.format(self._args.in_file.name))
        self.writeln_and_debug('Tool             : {:s}'.format(tool))
        self.writeln_and_debug('Source directory : {:s}'.format(report['directory']['source']))
        self.writeln_and_debug('Build directory  : {:s}'.format(report['directory']['build']))
        self.writeln_and_debug('Processed {:d} runs in {:.1f} seconds'.format(self.total_units(), report['elapsed']))
        self.writeln('')
        write_return_code_counts()
        write_result_kind_counts()
        write_has_stderr_and_return_code_0_count()
        write_has_error_in_stdout_count()
        self.writeln('== Configuration ==')
        self.writeln_and_debug('Added Arguments:    {}'.format('\n  '.join(report['arguments']['tool'])))
        self.writeln_and_debug('Removed Arguments:  {}'.format('\n  '.join(report['arguments']['filter'])))
        self.writeln_and_debug('Replaced Arguments: {}'.
                     format('\n  '.join(map(lambda (u, v): '{} -> {}'.
                                            format(u, v), report['arguments']['replace'].iteritems()))))
        self.writeln('')

        self.writeln('Command: {}'.format(report['tool']['command']))
        self.writeln('Version: {}'.format(report['tool']['version']))
        # Above has an extra line feed:
        # self.writeln('')

    @write_begin_end('Run Lists')
    def _write_run_lists(self):

        def write_return_code_lists():
            for return_code in self._runs_per_return_code.keys_sorted():
                self.write_begin('Return code {} - {} runs'.format(
                    return_code,
                    self._runs_per_return_code.count_for_key(return_code)))
                for run_id in self.sorted_run_ids():
                    report = self.report_from_run_id(run_id)
                    if 'returncode' in report and report['returncode'] == return_code:
                        self.writeln(run_id)
                self.write_end('Return code {}'.format(return_code))

        def write_result_kind_lists():
            # for result_kind in self._runs_per_result_kind.keys_sorted():
            for result_kind in (SUCCESS, TOOL_ERROR, BACKEND_ERROR):
                self.write_begin('Result kind "{}" - {} runs'.format(
                    result_kind,
                    self._runs_per_result_kind.count_for_key(result_kind)))
                for run_id in self.sorted_run_ids():
                    if self.report_from_run_id(run_id)['result_kind'] == result_kind:
                        self.writeln(run_id)
                self.write_end('Result kind {}'.format(result_kind))

        write_return_code_lists()
        write_result_kind_lists()


    @write_begin_end('Run Reports')
    def _write_run_reports(self):
        for run_id in self.sorted_run_ids():
            self._write_run_report(run_id)

    @write_begin_end('Run')
    def _write_run_report(self, run_id):
        run_report = self.report_from_run_id(run_id)

        def report_length(report_name):
            return (len(run_report[report_name]) if report_name in run_report else 0)

        self.write_boundary('{}'.format(run_id))
        self.writeln('== Work Directory: {}'.format(run_report['directory']))
        self.writeln('== Source File:    {}'.format(run_report['file']))
        self.writeln('== Elapsed Time:   {:.1f} seconds'.format((run_report['elapsed'] if 'elapsed' in run_report else 0)))
        self.writeln('== Return code:    {}'.format((run_report['returncode'] if 'returncode' in run_report else 'none')))
        self.writeln('== stdout size:    {}'.format(report_length('out')))
        self.writeln('== stderr size:    {}'.format(report_length('err')))
        self.writeln('== Result kind:    {}'.format(run_report['result_kind']))
        self.writeln('== Tool Command Line:')
        tcl = ' '.join(run_report['arguments']['tool'])
        self.writeln('{}'.format(' \\\n-'.join(tcl.split(' -'))))
        self.writeln('')

        if 'exception' in run_report:
            self.writeln('== Exception Running the Tool!')
            self.writeln('{}'.format(run_report['exception'].encode('utf-8')))

        if 'out' in run_report and len(run_report['out']) > 0:
            self.writeln('== Standard Output:')
            self.writeln('{}'.format(run_report['out'].encode('utf-8')))

        if 'err' in run_report and len(run_report['err']) > 0:
            self.writeln('== Standard Error:')
            self.writeln('{}'.format(run_report['err'].encode('utf-8')))
            self._capture_stderr_text(run_report['err'])

    def _capture_stderr_text(self, stderr_text):
        """All the lines for an execution are concatenated together into one string with "\n" between lines.
        Capture each line that is not indented or blank as a separate line. 
        """
        self._all_stderr_texts.append(stderr_text)
        lines = stderr_text.splitlines()
        # self.writeln('[{} lines, listed elsewhere]'.format(len(lines)))
        is_first_line = True
        for line in lines:
            if len(line) > 0 and not line.startswith((' ', '\v', '\x0b')):
                self._capture_stderr_line(line, is_first_line)
                is_first_line = False

    def _capture_stderr_line(self, raw_line, is_first_line):
        """Non-backend error lines start with "identityTranslator:"
        Backend error lines start with "rose" and/or contain "error:"
        Some lines we don't want look like:
        /dir/file_name.h(94): note: this candidate was rejected because...
        """
        clean_line = ''
        if raw_line.startswith('identityTranslator:'):
            want_line = True
            clean_line = raw_line
        elif raw_line.startswith('rose_'):
            want_line = True
            # Take out leading "rose_[file name].[suffix]([line number]): "
            # e.g. "rose_FILE_NAME.cpp(448): "
            p = re.compile(r'^rose_.*\([0-9]+\): ')
            clean_line = p.sub('', raw_line)
        elif 'error:' in raw_line:
            want_line = True
            # Take out leading "[file name].[suffix]([line number]): "
            # e.g. "FILE_NAME.cpp(448): "
            p = re.compile(r'^.*\([0-9]+\): ')
            clean_line = p.sub('', raw_line)
        else:
            want_line = False
        if want_line:
            self._all_main_stderr_lines.append(raw_line)
            self._occurrences_per_error.increment(clean_line)
            if is_first_line:
                # Want to know how many runs/files failed for each error.
                # Assume the first error in stderr caused that file to fail.
                # Call this once per error run.  Increase the count for this error line."""
                self._first_occurrences_per_error.increment(clean_line)

    @write_begin_end('NOT Backend Error Units')
    def _write_not_backend_error_units(self):
        for error_kind in ERROR_KINDS:
            self.write_begin(error_kind)
            name_id_map = self._error_groups[error_kind]
            self.writeln('These {} units had error_kind "{}":'.format(name_id_map.count(), error_kind))
            for name in name_id_map.names_sorted():
                self._write_error_unit(name, error_kind)
            self.write_end(error_kind)

        # self.writeln('======= NOT backend errors stderr only ========')
        # for name in self._not_backend_error_names:
        #     self._write_not_backend_error_unit_stderr_only(name)
        #
    def _write_error_unit(self, name, error_kind):
        report = self._json_in
        unit_id = self._error_groups[error_kind].to_id(name)
        run_report = report['trans-units'][unit_id]
        self.write_begin('translation_unit_{} ({})'.format(unit_id, error_kind))
        self.writeln('== File:           {}'.format(name))
        self.writeln('== Return code:    {}'.format(run_report['returncode']))
        if 'err' in run_report and len(run_report['err']) > 0:
            self.writeln('== Standard Error')
            self.writeln('{}'.format(run_report['err'].encode('utf-8')))
        self.write_end('translation_unit_{} ({})'.format(unit_id, error_kind))

    # def _write_not_backend_error_unit_stderr_only(self, name):
    #     report = self._json_in
    #     group_name = ERROR
    #     unit_id = self._sorted_status_groups[group_name].to_id(name)
    #     run_report = report['trans-units'][unit_id]
    #     if 'err' in run_report and len(run_report['err']) > 0:
    #         self.writeln('{}'.format(run_report['err'].encode('utf-8')))

    @write_begin_end('Error Line Groups')
    def _write_stderr_texts(self):
        line_group_index = 0
        for line_group in self._all_stderr_texts:
            self.write_begin('Error Line Group {}'.format(line_group_index))
            self.write(line_group)
            self.write_end('Error Line Group {}'.format(line_group_index))
            line_group_index += 1

    @write_begin_end('Main Error Lines')
    def _write_main_error_lines(self):
        self._all_main_stderr_lines.sort()
        for line in self._all_main_stderr_lines:
            self.writeln(line)

    @write_begin_end('Error Histogram')
    def _write_error_histogram(self):
        self.write_boundary('{} unique errors, ordered by number of occurrences'.
                         format(self._occurrences_per_error.count()))
        for count, line in self._occurrences_per_error.counts_and_keys_by_count():
            self.writeln('({:3d} occurrences) {}'.format(count, line))

    @write_begin_end('First Error Histogram')
    def _write_first_error_histogram(self):
        self.write_boundary('{} unique errors, ordered by number of files they appear first in'.
                         format(self._first_occurrences_per_error.count()))
        for count, line in self._first_occurrences_per_error.counts_and_keys_by_count():
            self.writeln('({:3d} files) {}'.format(count, line))

    def write_begin(self, message):
        self.write_boundary('BEGIN {}'.format(message))

    def write_end(self, message):
        self.write_boundary('END {}'.format(message))
        self.writeln('')

    def write_boundary(self, message):
        bar = '=' * 20
        self.writeln((bar + ' {} ' + bar).format(message))

    def writeln(self, message):
        self.write(message + '\n')

    def write(self, message):
        self._args.out_file.write(message)

    def writeln_and_debug(self, message):
        self._logger.debug(message)
        self.writeln(message)


def main():
    Renderer().run()

if __name__ == '__main__':
    main()
