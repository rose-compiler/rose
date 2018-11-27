#!/usr/bin/python2.7

import os
import sys
import time
import json
import textwrap
import argparse
import subprocess
import traceback
import multiprocessing

def aggregate_json_list(report, filename, mode, output, nprocs):
	result = list()
	for tu_report in report['trans-units']:
		tu_filepath_ext = os.path.realpath(tu_report['file'])
		tu_filepath = '.'.join(tu_filepath_ext.split('.')[:-1])
		tu_filename_ext = os.path.basename(tu_filepath_ext)
		tu_filename = '.'.join(tu_filename_ext.split('.')[:-1])
		tu_wordir = tu_report['directory']

		tu_file = filename.replace('%Fe',tu_filepath_ext).replace('%fe',tu_filename_ext).replace('%F',tu_filepath).replace('%f',tu_filename).replace('%d',tu_wordir)

		if os.path.isfile(tu_file):
			with open(tu_file, 'r') as F:
				try:
					data = json.load(F)
				except:
					print "File {} is not valid JSON.".format(tu_file)
		else:
			print "File {} does not exist.".format(tu_file)

		if not isinstance(data, list):
			print "Aggregation mode json:list expect the root of {} to be a list.".format(tu_file)
		else:
			result += data

	try:
		with open(output, 'w') as F:
			json.dump(result, F, indent=4) 
	except:
		print "Error while writing {}. Check that directory {} exist and you have permission to write there".format(output, os.path.dirname(output))

def aggregate(report, filename, mode, output, nprocs):
	if mode == 'json:list':
		aggregate_json_list(report, filename, mode, output, nprocs)
	else:
		assert False

def build_parser():
	parser = argparse.ArgumentParser(
				formatter_class=argparse.RawTextHelpFormatter, add_help=False,
				usage=textwrap.dedent('''
					(0) python comp_db_render.py [-h|--help]
					(1) python comp_db_render.py [ --report report.json ] \
				'''),
				description=textwrap.dedent('''\
					This renders a HTML document from the JSON report generated for a Compilation DB. \
					''')
			)

	optional = parser.add_argument_group('Optional arguments')

	optional.add_argument('--report',
		help=textwrap.dedent('''\
			Path of the JSON report.
			(default: report.json) \
			'''))

	optional.add_argument('--filename',
		help=textwrap.dedent('''\
			A filename where: %F and %f represent the source file path and basename  *without* extension respectively. \
			While, %Fe and %fe represent the source file path and basename  *with* extension respectively. \
			Finally, %d is replaced by the work directory.
			(default: "%d/%fe.json")
			'''))

	optional.add_argument('--mode',
		help=textwrap.dedent('''\
			Aggregation mode, one of:
			 - json:list
			 - txt:cat (NIY)
			 - txt:paste (NIY)
			 - csv:cat (NIY)
			 - csv:paste (NIY)
			(default:
			    'json:list' if '--filename' has 'json' extension
			    'txt:cat' if '--filename' has 'txt' extension
			    'csv:cat' if '--filename' has 'csv' extension
			)
			'''))

	optional.add_argument('--output',
		help=textwrap.dedent('''\
			Name of the output file.
			(default: the tool's basename with the extension associated with the aggregation mode ("identityTranslator.json") ) \
			'''))

	optional.add_argument('--nprocs', type=int, default=multiprocessing.cpu_count(),
								 	help=textwrap.dedent('''\
										Number of proccesses to use to run the tool.
										(default: number of CPUs)\
										'''))

	optional.add_argument('-h', '--help', action='help', help='show this help message and exit')

	return parser
	

def cli_parse_args(argv):
	parser = build_parser()

	args = parser.parse_args(argv)

	if args.report is None:
		args.report = 'report.json'
		if not os.path.isfile(args.report):
			print "No report.json found in the current directory."
			exit(1)
	else:
		if not os.path.isfile(args.report):
			print "Path provided for the report does not point to a file!"
			exit(1)

	try:
		with open(args.report, 'r') as F:
			args.report = json.load(F)
	except:
		print "The report file is not a valid JSON file!"
		exit(1)

	if args.filename is None:
		args.filename = "%d/%fe.json"

	if args.mode is None:
		if args.filename.endswith('.json'):
			args.mode = 'json:list'
		elif args.filename.endswith('.txt'):
			args.mode = 'txt:cat'
		elif args.filename.endswith('.csv'):
			args.mode = 'csv:cat'
		else:
			print "Unrecognized file extension, cannot determine default aggregation mode."
			exit(1)
	elif not args.mode in [ 'json:list' , 'txt:cat' , 'txt:paste' , 'csv:cat' , 'csv:paste' ]:
		print "Unrecognized aggregation mode."
		exit(1)
	elif args.mode in [ 'txt:cat' , 'txt:paste' , 'csv:cat' , 'csv:paste' ]:
		print "Aggregation mode {} not implemented yet.".format(args.mode)
		exit(1)

	if args.output is None:
		ext = args.mode.split(':')[0]
		tool = os.path.basename(args.report['tool']['command'])
		args.output = "{}.{}".format(tool, ext)

	return { 'report' : args.report, 'filename' : args.filename, 'mode' : args.mode, 'output' : args.output, 'nprocs' : args.nprocs }

if __name__ == "__main__":
	aggregate(**cli_parse_args(sys.argv[1:]))

