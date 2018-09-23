
import os
import sys
import json

if __name__ == "__main__":
	assert len(sys.argv) > 2
	filename_in  = sys.argv[1]
	filename_out = sys.argv[2]

	with open(filename_in, 'r') as F:
		comp_db = json.load(F)

        for comp_unit in comp_db:
                if not 'arguments' in comp_unit:
                        comp_unit.update({ 'arguments' : map(lambda s: s.strip() , filter(lambda s: len(s) > 0, comp_unit['command'].split(' '))) })

#	print 'Number of compilation units: {}'.format(len(comp_db))
#	source_files = sorted(list(set(map(lambda cu: cu['file'], comp_db))))
#	print 'Number of source files: {}'.format(len(source_files))
#	work_dirs = sorted(list(set(map(lambda cu: cu['directory'], comp_db))))
#	print 'Number of work directories: {}'.format(len(work_dirs))
#	cmd_tools = sorted(list(set(map(lambda cu: cu['arguments'][0], comp_db))))
#	print 'Tools: {}'.format(' '.join(cmd_tools))

	with open(filename_out, 'w') as F:
                json.dump(comp_db, F)
