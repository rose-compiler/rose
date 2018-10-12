
import os
import sys
import time
import json
import argparse
import subprocess
import multiprocessing

def apply_tool(comp_unit, tool, extra_args):
	start_time = time.time()

	if not 'arguments' in comp_unit:
		assert 'command' in comp_unit
		comp_unit.update({ 'arguments' : map(lambda s: s.strip() , filter(lambda s: len(s) > 0, comp_unit['command'].split(' '))) })

	workdir  = comp_unit['directory']
	filepath = comp_unit['file']
	filename = filepath.split('/')[-1]
	origtool = comp_unit['arguments'][0]

	extra_args = map(lambda arg: arg.replace('%F',filepath).replace('%f',filename).replace('%d',workdir).replace('%t',origtool), extra_args)

	cmdline = [ tool ] + comp_unit['arguments'][1:] + extra_args

	rose_proc = subprocess.Popen(cmdline, cwd=workdir, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

	out, err = rose_proc.communicate()

	result = { 'returncode' : rose_proc.returncode, 'out' : out , 'err' : err, 'time' : time.time() - start_time }
	result.update(comp_unit)
	result.update({ 'tool' : tool })
	result.update({ 'extra_args' : extra_args })

	return result
		
def apply_tool_helper(kwargs):
	try:
		return apply_tool(**kwargs)
	except Exception as e:
		return { 'exception' : e }

def map_tool(comp_db, tool, directory, nprocs, extra_args):
	start_time = time.time()
	pool = multiprocessing.Pool(nprocs)

	workload = map(lambda cu: { 'comp_unit' : cu, 'tool' : tool, 'extra_args' : extra_args }, comp_db)
	future_result = pool.map_async(func=apply_tool_helper, iterable=workload, chunksize=1)
	pool.close()

	while (not future_result.ready()):
		elapsed_time = time.time() - start_time
		number_done = len(workload) - future_result._number_left
		sys.stdout.write("\r                                       \r{}/{} in {:.1f} seconds".format(number_done, len(workload), elapsed_time ))
		sys.stdout.flush()
		time.sleep(.1)

	sys.stdout.write("\r                                       \r{}/{} in {:.1f} seconds\n".format(number_done, len(workload), elapsed_time ))
	sys.stdout.flush()

	return { 'tool' : tool , 'directory' : directory , 'nprocs' : nprocs , 'time' : time.time() - start_time , 'results' : future_result.get() }

def generate_html_report(report):
	with open('{}/report.html'.format(report['directory']), 'w') as F:
		F.write('<table width="100%" border=1>\n')
		F.write('  <tr><td width="1em">Directory</td><td>{}</td></tr>\n'.format(report['directory']))
		F.write('  <tr><td width="1em">Tool</td><td>{}</td></tr>\n'.format(report['tool']))
		F.write('  <tr><td width="1em">Number Processors</td><td>{}</td></tr>\n'.format(report['nprocs']))
		F.write('  <tr><td width="1em">Time (second)</td><td>{}</td></tr>\n'.format(report['time']))
		F.write('</table>\n\n')
		for result in report['results']:
			F.write('<br><hr><br>\n')
			F.write('<table width="100%" border=1>\n')
			F.write('  <td width="1em">Source File</td><td>{}</td></tr>\n'.format(result['file']))
			F.write('  <td width="1em">Directory</td><td>{}</td></tr>\n'.format(result['directory']))
			F.write('  <td width="1em">Original Command Line</td><td>{}</td></tr>\n'.format('<br/>'.join(result['arguments'])))
			F.write('  <td width="1em">Tool Specific Arguments</td><td>{}</td></tr>\n'.format('<br/>'.join(result['extra_args'])))
			F.write('  <td width="1em">Return Code</td><td>{}</td></tr>\n'.format(result['returncode']))
			F.write('  <td width="1em">Elapsed Time</td><td>{}</td></tr>\n'.format(result['time']))
			F.write('  <td width="1em">Standard Output</td><td><textarea rows="20" cols="240">{}</textarea></td></tr>\n'.format(result['out']))
			F.write('  <td width="1em">Standard Error</td><td><textarea rows="20" cols="240">{}</textarea></td></tr>\n'.format(result['err']))
			F.write('</table>\n\n')

def cli_parse_args():
	parser = argparse.ArgumentParser(description='Apply a tool to each compilation unit in a CLANG-style compilation database.')
	parser.add_argument('--srddir',              help='Source directory of the application being analyzed.')
	parser.add_argument('--builddir',            help='Directory where a specific build of the application can be found.')
	parser.add_argument('--database', nargs='?', help='Path for the compilation database (default is compile_commands.json in the build directory).')
	parser.add_argument('--tool',                help='Tool to apply on each compilation unit (default is ROSE\'s identityTranslator).')
	parser.add_argument('--filter',   nargs='+', help='Filter/replace command line arguments provided to the tool.')
	parser.add_argument('--nprocs',   type=int,  help='Number of proccesses to use to run the tool.')
	parser.add_argument('extra',      nargs='+', help='Command line arguments to be added for each compilation unit. Four place-holders are available: %F, %f, %d, %t corresponding, respectively, to source file path, source file name, working directory, and original tool/compiler.')

	return parser.parse_args()

if __name__ == "__main__":

	# args = cli_parse_args()

	assert len(sys.argv) > 2
	comp_db_path  = sys.argv[1]
	tool = sys.argv[2]
	nprocs = int(sys.argv[3])
	extra_args = sys.argv[4:]

	directory = '/'.join(comp_db_path.split('/')[:-1])

	with open(comp_db_path, 'r') as F:
		comp_db = json.load(F)

	report = map_tool(comp_db=comp_db, tool=tool, directory=directory, nprocs=nprocs, extra_args=extra_args)

	json_report_path = '{}/report.json'.format(directory)

	with open(json_report_path, 'w') as F:
		json.dump(report, F)

	generate_html_report(report)
