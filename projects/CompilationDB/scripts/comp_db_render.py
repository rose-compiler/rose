#!/usr/bin/python2.7

import os
import sys
import time
import json
import textwrap
import argparse
import subprocess

def prefix_path(path, prefixes):
	for (tag,prefix) in sorted(prefixes.iteritems(), key=lambda x: x[1], reverse=True):
		if path.startswith(prefix):
			return tag + path[len(prefix):]

def write_html_head(F, report, title, rscdir):
	F.write('<head>\n')

	F.write('<link rel="stylesheet" href="{}/css/bootstrap.min.css">\n\n'.format(rscdir))

	F.write('<link rel="stylesheet" href="{}/css/comp_db.css">\n\n'.format(rscdir))

	F.write('<meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">\n\n')

	F.write('<title>{}</title>\n\n'.format(title))

	F.write('</head>\n')

def write_navbar(F, report, title):
	prefixes = {
	    "@srcdir@" : report['directory']['source'],
	    "@builddir@" : report['directory']['build']
	}

	F.write('<nav class="navbar navbar-expand-lg fixed-top navbar-dark bg-primary" role="navigation">')
	F.write('  <div class="container">')
	F.write('  <a class="navbar-brand" href="#summary">Back to summary</a>\n')
	F.write('  <div class="collapse navbar-collapse">\n')
	F.write('    <ul class="navbar-nav mr-auto">\n')
	F.write('      <li class="nav-item dropdown">\n')
	F.write('        <a class="nav-link dropdown-toggle" href="#" id="navbarDropdown" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">\n')
	F.write('          Jump to a Compilation Unit\n')
	F.write('        </a>\n')
	F.write('        <div class="dropdown-menu" aria-labelledby="navbarDropdown">\n')

	for (cu_id, cu_report) in enumerate(report['comp-units']):
		filename = prefix_path(cu_report['file'], prefixes)
		style = 'btn-dark' if 'exception' in cu_report else ('btn-success' if cu_report['returncode'] == 0 else 'btn-danger')
		F.write('          <a class="btn {} page-scroll" href="#compilation_unit_{}" ><span>{}</span></a>\n'.format(style, cu_id,filename))
	F.write('        </div>\n')
	F.write('      </li>\n')
	F.write('    </ul>\n')
	F.write('  </div>\n')
	F.write('  </div>\n')
	F.write('</nav>\n')

def write_summary(F, report, title):
	tool = os.path.basename(report['tool']['command'])

	success_cnt = 0
	failure_cnt = 0
	except_cnt = 0
	for cu_report in report['comp-units']:
		if 'exception' in cu_report:
			except_cnt += 1
		elif cu_report['returncode'] == 0:
			success_cnt += 1
		else:
			failure_cnt += 1

	F.write('<section class="section" id="summary">\n')
	F.write('  <div class="container-fluid">\n')

	F.write('    <div class="card w-50 mx-auto bg-primary">\n')
	F.write('      <div class="card-body">\n')
	F.write('        <table border=0 align=center>\n')
	F.write('          <tr><td><h5>Project:</h5></td><td> </td><td><h1>{}</h1></td></tr>\n'.format(title))
	F.write('          <tr><td><h5>Tool:</h5>   </td><td> </td><td><h2>{}</h2></td></tr>\n'.format(tool))
	F.write('        </table>\n')
	F.write('      </div>\n')
	F.write('    </div>\n')

	F.write('    <div class="row">\n')

	F.write('      <div class="col-lg-12">\n')
	F.write('        <div class="card">\n')
	F.write('          <div class="card-header">\n')
	F.write('            <ul class="nav nav-pills card-header-pills">\n')
	F.write('              <li class="nav-item">')
	F.write('                <a class="nav-link active" data-toggle="collapse" href="#summary_results_body" aria-expanded="false" aria-controls="summary_results_body">Summary</a>\n')
	F.write('              </li>\n')
	F.write('            </ul>\n')
	F.write('          </div>\n')
	F.write('          <div class="collapse" id="summary_results_body">\n')
	F.write('            <div class="card-body">\n')
	F.write('              <p>\n')
	F.write('              Applied <code>{}</code> to <b>{} compilation units</b> in <b>{:.1f} seconds</b> using <b>{} processes</b>.\n'.format(tool, len(report['comp-units']), report['elapsed'], report['config']['nprocs']))
	if failure_cnt > 0:
		F.write('              It failed on <b>{} compilation units</b>, these compilation units are shown in red.\n'.format(failure_cnt))
	F.write('              </p>\n')
	if except_cnt > 0:
		F.write('              <p>For {} compilation units, the driver script encountered exceptions!!!</p>\n')
	F.write('            </div>\n')
	F.write('          </div>\n')
	F.write('        </div>\n')
	F.write('      </div>\n')

#	F.write('    </div>\n')
#	F.write('    <div class="row">\n')

	F.write('      <div class="col-md-6">\n')
	F.write('        <div class="card">\n')
	F.write('          <div class="card-header">\n')
	F.write('            <ul class="nav nav-pills card-header-pills">\n')
	F.write('              <li class="nav-item">')
	F.write('                <a class="nav-link active" data-toggle="collapse" href="#summary_tool_config_body" aria-expanded="false" aria-controls="summary_tool_config_body">Configuration</a>\n')
	F.write('              </li>\n')
	F.write('            </ul>\n')
	F.write('          </div>\n')
	F.write('          <div class="collapse" id="summary_tool_config_body">\n')
	F.write('            <div class="card-body">\n')
	F.write('              <b>Filters Arguments:</b> <pre>  {}</pre><br/>\n'.format('\n  '.join(report['arguments']['filter'])))
	F.write('              <b>Replace Arguments:</b> <pre>  {}</pre><br/>\n'.format('\n  '.join(map(lambda (u,v): '{} -> {}'.format(u,v), report['arguments']['replace'].iteritems()))))
	F.write('              <b>Extra Arguments:</b> <pre>  {}</pre><br/>\n'.format('\n  '.join(report['arguments']['tool'])))
	F.write('            </div>\n')
	F.write('          </div>\n')
	F.write('        </div>\n')
	F.write('      </div>\n')

	F.write('      <div class="col-md-6">\n')
	F.write('        <div class="card">\n')
	F.write('          <div class="card-header">\n')
	F.write('            <ul class="nav nav-pills card-header-pills">\n')
	F.write('              <li class="nav-item">')
	F.write('                <a class="nav-link active" data-toggle="collapse" href="#summary_tool_version_body" aria-expanded="false" aria-controls="summary_tool_version_body">Tool Version</a>\n')
	F.write('              </li>\n')
	F.write('              <li class="nav-item">\n')
	F.write('                <a class="nav-link js-tooltip js-copy" data-toggle="tooltip" data-placement="right" data-copy="summary_tool_samp" title="Copy to clipboard">\n')
	F.write('                  <svg class="icon" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" width="24" height="24" viewBox="0 0 24 24"><path d="M17,9H7V7H17M17,13H7V11H17M14,17H7V15H14M12,3A1,1 0 0,1 13,4A1,1 0 0,1 12,5A1,1 0 0,1 11,4A1,1 0 0,1 12,3M19,3H14.82C14.4,1.84 13.3,1 12,1C10.7,1 9.6,1.84 9.18,3H5A2,2 0 0,0 3,5V19A2,2 0 0,0 5,21H19A2,2 0 0,0 21,19V5A2,2 0 0,0 19,3Z" /></svg>\n')
	F.write('                </a>\n')
	F.write('              </li>\n')
	F.write('            </ul>\n')
	F.write('          </div>\n')
	F.write('          <div class="collapse" id="summary_tool_version_body">\n')
	F.write('            <div class="card-body">\n')
	F.write('              <pre id="summary_tool_samp">{}</pre>\n'.format(report['tool']['version']))
	F.write('            </div>\n')
	F.write('          </div>\n')
	F.write('        </div>\n')
	F.write('      </div>\n')

#	F.write('    </div>\n')
#	F.write('    <div class="row">\n')

	F.write('      <div class="col-lg-12">\n')
	F.write('        <div class="card">\n')
	F.write('          <div class="card-header">\n')
	F.write('            <ul class="nav nav-pills card-header-pills">\n')
	F.write('              <li class="nav-item">')
	F.write('                <a class="nav-link active" data-toggle="collapse" href="#summary_path_body" aria-expanded="false" aria-controls="summary_path_body">Paths</a>\n')
	F.write('              </li>\n')
	F.write('            </ul>\n')
	F.write('          </div>\n')
	F.write('          <div class="collapse" id="summary_path_body">\n')
	F.write('            <div class="card-body">\n')
	F.write('              <pre>Source directory: {}\nBuild directory:  {}\nTool command:     {}</pre>\n'.format(report['directory']['source'], report['directory']['build'], report['tool']['command']))
	F.write('            </div>\n')
	F.write('          </div>\n')
	F.write('        </div>\n')
	F.write('      </div>\n')

	F.write('    </div>\n')

	F.write('  </div>\n')
	F.write('</section>\n\n')

def write_global_report(F, report):
	pass # TODO

def write_compilation_unit_report(F, report, cu_id):
	cu_report = report['comp-units'][cu_id]

	style = 'dark' if 'exception' in cu_report else ('success' if cu_report['returncode'] == 0 else 'danger')

	prefixes = {
	    "@srcdir@" : report['directory']['source'],
	    "@builddir@" : report['directory']['build']
	}

	filename = prefix_path(cu_report['file'], prefixes)
	workdir = prefix_path(cu_report['directory'], prefixes)

	F.write('<section class="section" id="compilation_unit_{}">\n'.format(cu_id))
	F.write('  <div class="container-fluid">\n')
#	F.write('    <div class="card w-20"></div>\n')
	F.write('    <div class="card w-50 mx-auto bg-{}">\n'.format(style))
	F.write('      <div class="card-body">\n')
	F.write('        <table border=0 align=center>\n')
	F.write('          <tr><td><h5>File:</h5>          </td><td> </td><td><h3>{}</h3></td></tr>\n'.format(filename))
	F.write('          <tr><td><h5>Work Directory:</h5></td><td> </td><td><h3>{}</h3></td></tr>\n'.format(workdir))
	F.write('          <tr><td></td><td colspan=2><h5>Return code is <b>{}</b></h5></td></tr>\n'.format(cu_report['returncode']))
	F.write('          <tr><td></td><td colspan=2><h5>Processed in <b>{:.1f}</b> seconds</h5></td></tr>\n'.format(cu_report['elapsed']))
	F.write('        </table>\n')
	F.write('      </div>\n')
	F.write('    </div>\n')

	F.write('    <div class="row">\n')

	ocl = cu_report['arguments']['original']
	ocl = ' '.join(ocl)
	F.write('    <div class="col-md-6">\n')
	F.write('    <div class="card">\n')
	F.write('      <div class="card-header">\n')
	F.write('        <ul class="nav nav-pills card-header-pills">\n')
	F.write('          <li class="nav-item">')
	F.write('            <a class="nav-link active btn-{0}" data-toggle="collapse" href="#compilation_unit_{1}_ocl_body" aria-expanded="false" aria-controls="compilation_unit_{1}_ocl_body">Original Command Line</a>\n'.format(style, cu_id))
	F.write('          </li>\n')
	F.write('          <li class="nav-item">\n')
	F.write('            <a class="nav-link js-tooltip js-copy" data-toggle="tooltip" data-placement="right" data-copy="compilation_unit_{}_ocl_pre" title="Copy to clipboard">\n'.format(cu_id))
	F.write('              <svg class="icon" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" width="24" height="24" viewBox="0 0 24 24"><path d="M17,9H7V7H17M17,13H7V11H17M14,17H7V15H14M12,3A1,1 0 0,1 13,4A1,1 0 0,1 12,5A1,1 0 0,1 11,4A1,1 0 0,1 12,3M19,3H14.82C14.4,1.84 13.3,1 12,1C10.7,1 9.6,1.84 9.18,3H5A2,2 0 0,0 3,5V19A2,2 0 0,0 5,21H19A2,2 0 0,0 21,19V5A2,2 0 0,0 19,3Z" /></svg>\n')
	F.write('            </a>\n')
	F.write('          </li>\n')
	F.write('        </ul>\n')
	F.write('      </div>\n')
	F.write('      <div class="collapse" id="compilation_unit_{}_ocl_body">\n'.format(cu_id))
	F.write('        <div class="card-body">\n')
	F.write('          <pre id="compilation_unit_{}_ocl_pre">{}</pre>\n'.format(cu_id, ' \\\n         -'.join(ocl.split(' -'))))
	F.write('        </div>\n')
	F.write('      </div>\n')
	F.write('    </div>\n')
	F.write('    </div>\n')

	tcl = cu_report['arguments']['tool']
	tcl = ' '.join(tcl)
	F.write('    <div class="col-md-6">\n')
	F.write('    <div class="card">\n')
	F.write('      <div class="card-header">\n')
	F.write('        <ul class="nav nav-pills card-header-pills">\n')
	F.write('          <li class="nav-item">')
	F.write('            <a class="nav-link active btn-{0}" data-toggle="collapse" href="#compilation_unit_{1}_tcl_body" aria-expanded="false" aria-controls="compilation_unit_{1}_tcl_body">Tool Command Line</a>\n'.format(style, cu_id))
	F.write('          </li>\n')
	F.write('          <li class="nav-item">\n')
	F.write('            <a class="nav-link js-tooltip js-copy" data-toggle="tooltip" data-placement="right" data-copy="compilation_unit_{}_tcl_pre" title="Copy to clipboard">\n'.format(cu_id))
	F.write('              <svg class="icon" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" width="24" height="24" viewBox="0 0 24 24"><path d="M17,9H7V7H17M17,13H7V11H17M14,17H7V15H14M12,3A1,1 0 0,1 13,4A1,1 0 0,1 12,5A1,1 0 0,1 11,4A1,1 0 0,1 12,3M19,3H14.82C14.4,1.84 13.3,1 12,1C10.7,1 9.6,1.84 9.18,3H5A2,2 0 0,0 3,5V19A2,2 0 0,0 5,21H19A2,2 0 0,0 21,19V5A2,2 0 0,0 19,3Z" /></svg>\n')
	F.write('            </a>\n')
	F.write('          </li>\n')
	F.write('        </ul>\n')
	F.write('      </div>\n')
	F.write('      <div class="collapse" id="compilation_unit_{}_tcl_body">\n'.format(cu_id))
	F.write('        <div class="card-body">\n')
	F.write('          <pre id="compilation_unit_{}_tcl_pre">{}</pre>\n'.format(cu_id, ' \\\n         -'.join(tcl.split(' -'))))
	F.write('        </div>\n')
	F.write('      </div>\n')
	F.write('    </div>\n')
	F.write('    </div>\n')

	if 'out' in cu_report and len(cu_report['out']) > 0:
		F.write('    <div class="col-lg-12">\n')
		F.write('    <div class="card">\n')
		F.write('      <div class="card-header">\n')
		F.write('        <ul class="nav nav-pills card-header-pills">\n')
		F.write('          <li class="nav-item">')
		F.write('            <a class="nav-link active btn-{0}" data-toggle="collapse" href="#compilation_unit_{1}_out_body" aria-expanded="false" aria-controls="compilation_unit_{1}_out_body">Standard Output</a>\n'.format(style, cu_id))
		F.write('          </li>\n')
		F.write('        </ul>\n')
		F.write('      </div>\n')
		F.write('      <div class="collapse" id="compilation_unit_{}_out_body">\n'.format(cu_id))
		F.write('        <div class="card-body">\n')
		F.write('          <pre>{}</pre>\n'.format(cu_report['out'].encode('utf-8')))
		F.write('        </div>\n')
		F.write('      </div>\n')
		F.write('    </div>\n')
		F.write('    </div>\n')

	if 'err' in cu_report and len(cu_report['err']) > 0:
		F.write('    <div class="col-lg-12">\n')
		F.write('    <div class="card">\n')
		F.write('      <div class="card-header">\n')
		F.write('        <ul class="nav nav-pills card-header-pills">\n')
		F.write('          <li class="nav-item">')
		F.write('            <a class="nav-link active btn-{0}" data-toggle="collapse" href="#compilation_unit_{1}_err_body" aria-expanded="false" aria-controls="compilation_unit_{1}_err_body">Standard Error</a>\n'.format(style, cu_id))
		F.write('          </li>\n')
		F.write('        </ul>\n')
		F.write('      </div>\n')
		F.write('      <div class="collapse" id="compilation_unit_{}_err_body">\n'.format(cu_id))
		F.write('        <div class="card-body">\n')
		F.write('          <pre>{}</pre>\n'.format(cu_report['err'].encode('utf-8')))
		F.write('        </div>\n')
		F.write('      </div>\n')
		F.write('    </div>\n')
		F.write('    </div>\n')

	if 'exception' in cu_report:
		F.write('    <div class="col-lg-12">\n')
		F.write('    <div class="card">\n')
		F.write('      <div class="card-header">\n')
		F.write('        <ul class="nav nav-pills card-header-pills">\n')
		F.write('          <li class="nav-item">')
		F.write('            <a class="nav-link active btn-{0}" data-toggle="collapse" href="#compilation_unit_{1}_exception_body" aria-expanded="false" aria-controls="compilation_unit_{1}_exception_body">Exception Running the Tool!</a>\n'.format(style, cu_id))
		F.write('          </li>\n')
		F.write('        </ul>\n')
		F.write('      </div>\n')
		F.write('      <div class="collapse" id="compilation_unit_{}_exception_body">\n'.format(cu_id))
		F.write('        <div class="card-body">\n')
		F.write('          <pre>{}</pre>\n'.format(cu_report['exception'].encode('utf-8')))
		F.write('        </div>\n')
		F.write('      </div>\n')
		F.write('    </div>\n')
		F.write('    </div>\n')

	for addon in cu_report['addons']:
		F.write('    <div class="col-lg-12">\n')
		F.write('    <div class="card">\n')
		F.write('      <div class="card-header">\n')
		F.write('        <ul class="nav nav-pills card-header-pills">\n')
		F.write('          <li class="nav-item">')
		F.write('            <a class="nav-link active btn-{0}" data-toggle="collapse" href="#compilation_unit_{1}_{3}_body" aria-expanded="false" aria-controls="compilation_unit_{1}_{3}_body">{2}</a>\n'.format(style, cu_id, addon['title'], addon['tag']))
		F.write('          </li>\n')
		F.write('        </ul>\n')
		F.write('      </div>\n')
		F.write('      <div class="collapse {2}" id="compilation_unit_{0}_{1}_body">\n'.format(cu_id, addon['tag'], addon['class']))
		F.write('        <div class="card-body">{}</div>\n'.format(addon['html']))
		F.write('      </div>\n')
		F.write('    </div>\n')
		F.write('    </div>\n')

	F.write('    </div>\n')
	F.write('    </div>\n')
	F.write('  </div>\n')
	F.write('</section>\n\n')

def write_html_body(F, report, title):
	F.write('<body>\n')

	write_navbar(F, report, title)

	F.write('<main class="circuit">\n')

	write_summary(F, report, title)

	write_global_report(F, report)

	for cu_id in range(len(report['comp-units'])):
		write_compilation_unit_report(F, report, cu_id)

	F.write('</main>\n')
	F.write('</body>\n')

def write_scripts(F, report, rscdir):
	F.write('<script src="{}/js/jquery-3.3.1.slim.min.js"></script>\n'.format(rscdir))
	F.write('<script src="{}/js/popper.min.js"></script>\n'.format(rscdir))
	F.write('<script src="{}/js/bootstrap.min.js"></script>\n\n'.format(rscdir))

	F.write('<script src="{}/js/svg-pan-zoom.min.js"></script>\n'.format(rscdir))

	F.write('<script src="{}/js/comp_db.js"></script>\n'.format(rscdir))

def generate_html(F, report, title, rscdir):
	F.write('<!doctype html>\n')
	F.write('<html lang="en">\n')
	write_html_head(F, report, title, rscdir)
	write_html_body(F, report, title)
	write_scripts(F, report, rscdir)
	F.write('</html>\n')

def addons_apply_graphviz(report, addon_id, title, filename):
	if '%F' in filename or '%f' in filename or '%F%e' in filename or '%f%e' in filename:
		for cu_report in report['comp-units']:
			cu_filepath_ext = os.path.realpath(cu_report['file'])
			cu_filepath = '.'.join(cu_filepath_ext.split('.')[:-1])
			cu_filename_ext = os.path.basename(cu_filepath_ext)
			cu_filename = '.'.join(cu_filename_ext.split('.')[:-1])
			cu_wordir = cu_report['directory']

			cu_filename = filename.replace('%Fe',cu_filepath_ext).replace('%fe',cu_filename_ext).replace('%F',cu_filepath).replace('%f',cu_filename).replace('%d',cu_wordir)

			if '*' in cu_filename:
				assert False, "NIY!!!" # TODO
			else:
				cu_files = [ ( cu_filename , '' ) ]

			for (fid, ( fpath , fM ) ) in enumerate(cu_files):
				if not os.path.isfile(fpath):
					html = '<p>Cannot find the GraphViz file:</p><pre>{}</pre>'.format(fpath)
				elif os.stat(fpath).st_size > 512*1024:
					html = '<p>Provided GraphViz file is too large. You can try manually:</p><pre>{}</pre>'.format(fpath)
				else:
					dot_proc = subprocess.Popen([ 'dot' , '-Tsvg' , fpath ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
					out, err = dot_proc.communicate()
					html = out if len(err) == 0 else ( '{}<pre>{}</pre>'.format(out, err) if len(out) > 0 else '<pre>{}</pre>'.format(err) )

				cu_report['addons'].append({
				  'tag': 'addon_{}_file_{}'.format(addon_id, fid),
				  'class' : 'svg_zoom_pan',
				  'title' : title.replace('%M', fM),
				  'html' : html
				})
	else:
		pass

def generate_report(report, title, addons, rscdir):
	for cu_report in report['comp-units']:
		cu_report.update({ 'addons' : list() })

	for (addon_id, addon_desc) in enumerate(addons):
		if not isinstance(addon_desc, dict) or not 'function' in addon_desc or not 'kwargs' in addon_desc:
			print "Found invalid add-on descriptor #{}...".format(addon_id)
			exit(1)
		if addon_desc['function'] == 'graphviz':
			addons_apply_graphviz(report=report, addon_id=addon_id, **addon_desc['kwargs'])
		else:
			print "Unrecognized add-on function: {}".format(addon_desc['function'])

	filename = report['report']
	filename = '{}.html'.format('.'.join((filename.split('.')[:-1])))
	with open(filename, 'w') as F:
		generate_html(F=F, report=report, title=title, rscdir=rscdir)

def build_parser():
	parser = argparse.ArgumentParser(
				formatter_class=argparse.RawTextHelpFormatter, add_help=False,
				usage=textwrap.dedent('''
					(0) python comp_db_render.py [-h|--help]
					(1) python comp_db_render.py [ --report report.json ] [--title "Compilation Report" ] \
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

	optional.add_argument('--title',
		help=textwrap.dedent('''\
			Title to use instead of "Compilation Report". \
			'''))

	optional.add_argument('--addons',
		help=textwrap.dedent('''\
			Either a JSON file or inlined JSON describing additional information provided by the tool. \
			It must be a list of dictionary entries. Each entry represents on addon with the following fields:
			 - "function", one of: graphviz or user-defined,
			 - "kwargs":  of arguments for rendering.
                        The "graphviz" function expects "title" and "filename" in "kwargs".
			In filename, %F and %f represent the source file path and basename  *without* extension respectively. \
			While, %Fe and %fe represent the source file path and basename  *with* extension respectively. \
			Finally, %d is replaced by the work directory. \
			It also accepts one * at most. In this case, one graph will be generated for each of the matching file. \
			The title field is used to name the section(s) added to the compilation unit report.
			If using *, then the "title" can contain %M which will be replaced by the matched part of the filename.
			The "user-defined" function is not implemented yet! It should permit the user to post-process the tools results using his own tools. \
			One possible use could be to generate a filtered version of the standard output or formating some user-defined output file.
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

	if args.title is None:
		args.title = "Compilation Report"

	if not args.addons is None:
		if os.path.isfile(args.addons):
			try:
				with open(args.addons) as F:
					args.addons = json.load(F)
			except:
				print "The addons argument expects a JSON file or inlined JSON. We received a valid file path but could not parse JSON from it."
				exit(1)
		else:
			try:
				args.addons = json.loads(args.addons)
			except:
				print "The addons argument expects a JSON file or inlined JSON. We did not receive a valid file path and the string was not valid JSON."
				exit(1)

		if not isinstance(args.addons, list):
			print "The data associated with the addons argument should be a list."
			exit(1)
	else:
		args.addons = list()
		

	rscdir = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + '/../static')

	return { 'report' : args.report, 'title' : args.title, 'addons' : args.addons, 'rscdir' : rscdir }

if __name__ == "__main__":
	generate_report(**cli_parse_args(sys.argv[1:]))

