#!/usr/bin/env python2

import os
import sys
import time
import json
import textwrap
import argparse
import traceback
import subprocess
import multiprocessing

def enforce_abspath(path, bdir):
	if os.path.isabs(path):
		return path
	else:
		return os.path.realpath(bdir + '/' + path)

def normalize_translation_unit(trans_unit):
	assert os.path.isabs(trans_unit['directory']), 'Found a "command object" where the workdir ("directory" field) is a relative path.'

	# if argument field is not provided build it from commend field
	if not 'arguments' in trans_unit:
		assert 'command' in trans_unit, 'Found a compile command with neither "command" nor "arguments" filed.'
		trans_unit.update({ 'arguments' : map(lambda s: s.strip() , filter(lambda s: len(s) > 0, trans_unit['command'].split(' '))) })

	# delete command field
	if 'command' in trans_unit:
		del trans_unit['command']

	# find all occurence of the original (potentially relative) path to the input file
	infile_index = [ i for (i, arg) in enumerate(trans_unit['arguments']) if arg == trans_unit['file'] ]

	# get absolute pass for input file
	trans_unit.update({ 'file' : enforce_abspath(trans_unit['file'], trans_unit['directory']) })

	# update references to input file's path in arguments field
	for i in infile_index:
		trans_unit['arguments'][i] = trans_unit['file']

	# make all include path absolute)
	trans_unit.update({ 'arguments' : map(lambda arg: '-I{}'.format(enforce_abspath(arg[2:], trans_unit['directory'])) if arg.startswith('-I') else arg, trans_unit['arguments']) })
	trans_unit.update({ 'incpath' : map(lambda arg: arg[2:], filter(lambda s: s.startswith('-I'), trans_unit['arguments'])) })

	# figure the output file (AND make it absolute)
	output_opt_index = [ i for (i, arg) in enumerate(trans_unit['arguments']) if arg == '-o' ]
	assert len(output_opt_index) < 2, 'Found a compile command with more than one "-o" arguments.'
	if len(output_opt_index) == 1:
		assert len(trans_unit['arguments']) > output_opt_index[0] + 1, 'Found a compile command where the last argument is the "-o" option'
		outfile = enforce_abspath(trans_unit['arguments'][output_opt_index[0]+1], trans_unit['directory'])
		trans_unit['arguments'][output_opt_index[0]+1] = outfile
		trans_unit.update({ 'output' : outfile })

	return trans_unit

def remove_duplicate_translation_units(comp_db):
	unit_list_map = dict()
	for unit in comp_db:
		key = ( unit['file'] , unit['output'] if 'output' in unit else '' , unit['directory'] )
		if key in unit_list_map:
			unit_list_map[key].append(unit)
		else:
			unit_list_map[key] = [unit,]
	result_comp_db = list()
	for unit_list in unit_list_map.itervalues():
		assert len(unit_list) > 0
		result_comp_db.append(unit_list[0])
	return result_comp_db

def normalize_compilation_database(comp_db):
        return remove_duplicate_translation_units(map(normalize_translation_unit, comp_db))

def transform_original_args(args, filter_args, replace_args):
	args = filter(lambda arg: not arg in filter_args, args)
	args = map(lambda arg: replace_args[arg] if arg in replace_args else arg, args)
	return args

def substitute_args_placeholders(args, filepath, filename, workdir, origtool):
	return map(lambda arg: arg.replace('%F',filepath).replace('%f',filename).replace('%d',workdir).replace('%t',origtool), args)

def apply_tool(tool, trans_unit, args):
	start_time = time.time()
	time_limit = '600s'

	arguments = trans_unit['arguments']

	trans_unit.update({ 'arguments' : { 'original' : arguments } })

	origtool = arguments[0]
	arguments = arguments[1:]

	workdir  = trans_unit['directory']
	filepath = trans_unit['file']
	filename = filepath.split('/')[-1]

	arguments = transform_original_args(arguments, args['filter'], args['replace'])
	arguments = args['tool'] + arguments
	arguments = substitute_args_placeholders(arguments, filepath, filename, workdir, origtool)

	# Timeout sends SIGTERM and has return code 124 if it is triggered. If that 
	# doesn't work, and --kill-after is present, sends SIGKILL (return code -9):
	arguments = [ 'timeout','--kill-after=5s', time_limit,  tool ] + arguments

	trans_unit['arguments'].update({ 'tool' : arguments })

	tool_proc = subprocess.Popen(arguments, cwd=workdir, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

	out, err = tool_proc.communicate()

	result = { 'returncode' : tool_proc.returncode, 'out' : out , 'err' : err, 'elapsed' : time.time() - start_time }
	result.update(trans_unit)

	return result
		
def apply_tool_helper(kwargs):
	try:
		return apply_tool(**kwargs)
	except Exception as e:
		traceback.print_exc(file=sys.stdout)
		trans_unit = kwargs['trans_unit']
		trans_unit.update({ 'exception' : str(e) })
		return trans_unit

def log_progress(number_done, total, elapsed_time):
    sys.stdout.write(
        "\r                                       \r{}/{} in {:.1f} seconds".
        format(number_done, total, elapsed_time ))
    sys.stdout.flush()

def map_tool(job):
	start_time = time.time()

	workload = map(lambda tu: { 'trans_unit' : tu, 'tool' : job['tool']['command'], 'args' : job['arguments'] }, job['database'])

	log_progress(0, len(workload), 0)

	if job['config']['nprocs'] == 0:
		results = list()
		for kwargs in workload:
			results.append(apply_tool(**kwargs))
			elapsed_time = time.time() - start_time	
			log_progress(len(results), len(workload), elapsed_time)
	else:
		pool = multiprocessing.Pool(job['config']['nprocs'])
		future_result = pool.map_async(func=apply_tool_helper, iterable=workload, chunksize=1)
		pool.close()

		while (not future_result.ready()):
			elapsed_time = time.time() - start_time
			number_done = len(workload) - future_result._number_left
			log_progress(number_done, len(workload), elapsed_time)
			time.sleep(5)

		results = future_result.get()
		elapsed_time = time.time() - start_time

	log_progress(len(results), len(workload), elapsed_time)
	job.update({ 'elapsed' : elapsed_time, 'trans-units' : results })

	return job

def split_if_needed(args, database):
	if  args.split > 1:
		parts = dict()
		# Make the fragments:
		frag_numbers = range(1, args.split + 1)
		for index in frag_numbers:
			parts[index] = list()
		# Fill the fragments:
		which_frag=1
		for unit in database:
			parts[which_frag].append(unit)
			which_frag += 1
			if which_frag > args.split:
				which_frag = 1
		# Write the fragments to files:
		db_path, db_filename=os.path.split(args.database_path)
		db_basename, db_ext = db_filename.split('.')
		print ('Splitting {} ({} entries) into {} files'.format(args.database_path, len(database), args.split))
		for index in frag_numbers:
			part_filename = db_basename + '_' + str(index) + '.' + db_ext
			part_path = os.path.join(db_path, part_filename)
			print ('Creating {} with {} entries'.format(part_path, len(parts[index])))
			with open(part_path, 'w') as part_file:
				json.dump(parts[index], part_file)
		print('Exiting')
		exit(0)

def truncate_as_needed(args, database):
	"""Remove units fom the database not between start_at and end_at.
	"""
	first = args.start_at
	last = args.end_at
	if first > 1 or last < len(database):
		truncated_size = last - first + 1
		print ('Truncating {} database entries down to {} entries, starting at {}, ending at {}'.
			   format(len(database), truncated_size, first, last))
	return database[first - 1 : last]

def build_parser():
	parser = argparse.ArgumentParser(
				formatter_class=argparse.RawTextHelpFormatter, add_help=False,
				usage=textwrap.dedent('''
					(0) python comp_db_map.py [-h|--help]
					(1) python comp_db_map.py srcdir builddir tool [--database DATABASE] [--filter FILTER] [--nprocs NPROCS] [ -- toolarg [toolarg ...] ]\
				'''),
				description=textwrap.dedent('''\
					This utility applies a given tool to each translation unit in a CLANG-style compilation database.
					It prevents ROSE's users from having to "hack" into a target applications build system.\
					'''),
				epilog=textwrap.dedent('''\
					Following the -- argument, users can provide additional argument to be forwarded to the tool.
					Four place-holders are available to specialize these arguments for each translation unit:
						- %F is replaced by the source file path
						- %f is replaced by the source file name
						- %d is replaced by the working directory
						- %t is replaced by the original tool/compiler.\
					''')
			)

	mandatory = parser.add_argument_group('Mandatory arguments')

	mandatory.add_argument('srcdir',
								 help=textwrap.dedent('''\
									Source directory of the application being analyzed. Used to display shorter paths.\
									'''))
	mandatory.add_argument('builddir',
								 help=textwrap.dedent('''\
									Directory where a specific build of the application is located. Used to display shorter paths and find the database.\
									'''))
	mandatory.add_argument('tool',
								 help=textwrap.dedent('''\
									Tool to apply on each translation unit, such as ROSE\'s identityTranslator.\
									'''))

	optional = parser.add_argument_group('Optional arguments')

	optional.add_argument('--database_path',
								help=textwrap.dedent('''\
									Path to the compilation database relative to the utility's working directory (or absolute).
									(default: $builddir/compile_commands.json).\
									'''))

	optional.add_argument('--report',
								help=textwrap.dedent('''\
									Name of the JSON file where the report get stored.
									(default: $builddir/$tool_basename.json).\
									'''))

	optional.add_argument('--filter', action='append', dest='filters',
								help=textwrap.dedent('''\
									Filter/replace command line argument provided to the tool.
									Multiple filter/replace command can be provided.
									  - "f:-x" means that "-x" argument will not be presented to the tool
									  - "r:-x:-y" means the "-x" argument must be replaced by "-y"
									The separator in these command do not have to be a semi-colon (:) but can be any character.
									The separator do not have to be the same accross filter/replace commands.\
									'''))

	optional.add_argument('--nprocs', type=int, default=multiprocessing.cpu_count(),
								help=textwrap.dedent('''\
									Number of proccesses to use to run the tool.
									(default: number of CPUs)\
									'''))

	optional.add_argument(
		'--split',
		type=int,
		default=0,
		metavar='<number>',
		help='Split the compilation database into this many files and exit.  ' +
		'Eliminates duplicate units first.  Default: %(default)s (do not split)'
	)

	optional.add_argument(
		'--start_at',
		type=int,
		default=1,
		metavar='<index>',
		help='First unit to process.  Limits the number of units in one job.  ' +
		'Eliminates duplicate units first.  Default: %(default)s (first unit in database)'
	)

	optional.add_argument(
		'--end_at',
		type=int,
		default=-1,
		metavar='<index>',
		help='Last unit to process.  Limits the number of units in one job.  ' +
			 'Eliminates duplicate units first.  Default: %(default)s (last unit in database)'
	)

	optional.add_argument('-h', '--help', action='help', help='show this help message and exit')

	return parser
	

def cli_parse_args(argv):
	parser = build_parser()

	if '--' in argv:
		i = argv.index('--')
		toolargs = argv[i+1:]
		argv = argv[:i]
	else:
		toolargs = list()

	args = parser.parse_args(argv)

	if not os.path.isdir(args.srcdir):
		print "Provided source directory is not a directory!"
		exit(1)
	args.srcdir = os.path.abspath(args.srcdir)

	if not os.path.isdir(args.builddir):
		print "Provided build directory is not a directory!"
		exit(1)
	args.builddir = os.path.abspath(args.builddir)

	if args.database_path is None:
		args.database_path = '{}/compile_commands.json'.format(args.builddir)
		if not os.path.isfile(args.database_path):
			print "Cannot find the compilation database in the build directory!"
			exit(1)
	else:
		if not os.path.isfile(args.database_path):
			print "Provided database path is not a file!"
			exit(1)

	try:
		with open(args.database_path, 'r') as F:
			json_database = json.load(F)
	except:
		print "The database file is not a valid JSON file!"
		exit(1)

	if args.end_at == -1:
		args.end_at = len(json_database)

	json_database = normalize_compilation_database(json_database)
	# May not return:
	split_if_needed(args, json_database)
	json_database = truncate_as_needed(args, json_database)

	if args.report is None:
		args.report = '{}/{}.json'.format(args.builddir, os.path.basename(args.tool))

	filter_args = list()
	replace_args = dict()
	if not args.filters is None:
		for f in args.filters:
			if f[0] == 'f':
				filter_args.append(f[2:])
			elif f[0] == 'r':
				F = f[2:].split(f[1])
				assert len(F) == 2, 'Invalid replace command: "{}"'.format(f)
				replace_args.update({ F[0] : F[1] })
			else:
				print 'Invalid filter/replace command: first character must be one of "f" or "r". Probematic command is "{}"'.format(f)

	print ("Using ROSE Tool at {}".format (args.tool))
	tool_proc = subprocess.Popen([ args.tool , '--version' ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

	tool_version = tool_proc.communicate()[0]

	return {
		'directory' : {
			'source' : args.srcdir,
			'build' : args.builddir
		},
		'database' : json_database,
		'tool' : {
			'command' : args.tool,
			'version' : tool_version
		},
		'arguments' : {
			'filter' : filter_args,
			'replace' : replace_args,
			'tool' : toolargs
		},
		'config' : {
			'nprocs' : args.nprocs
		},
		'report' : args.report
	}
        
def main():
	job = cli_parse_args(sys.argv[1:])
	job = map_tool(job)
	with open(job['report'], 'w') as F:
		json.dump(job, F)


if __name__ == "__main__":
    main()