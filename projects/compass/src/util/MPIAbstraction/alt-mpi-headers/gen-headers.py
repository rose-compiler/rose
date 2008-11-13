#!/usr/bin/env python
"""
usage: python gen-headers.py [options] <mpi-install-dir>

Valid [options]:

  --out-incs=<dir-name>
    Default: <dir-name> = ./include
    
  --out-srcs=<dir-name>
    Default: <dir-name> = ./lib
"""

import sys
import string
import re
import os

#**************************************************************************

def abort (msg = None, code = 1):
    if code != 0:
        sys.stderr.write ("*** ERROR ***\n")
    if msg:
        sys.stderr.write (msg + "\n")
    sys.exit (code)

def usage (note = None):
    sys.stderr.write (__doc__ + "\n")
    if note:
        abort (note)
    else:
        abort (note, 0)

def mkdir_p (newdir):
    """ok = mkdir_p (newdir)

    Functionally equivalent to the shell command, 'mkdir -p newdir'.
    Returns False on error."""
   
    if os.path.isdir (newdir):
        return True
    elif os.path.isfile (newdir):
        return False
    else:
        head, tail = os.path.split (newdir)
        if head and not os.path.isdir (head):
            if not mkdir_p (head):
                return False
        if tail:
            os.mkdir (newdir)
    return True

def write_line (fp, s):
    """write_line (fp, s)

    Writes the text 's' to the output file, 'fp', appending
    a new-line. If 's' is None, just writes the new-line."""
    
    assert (fp)
    if s:
        fp.write (s)
    fp.write ('\n')

def open_file (fn, mode = 'r'):
    """fp = open_file (fn, mode = 'r')

    Wrapper around 'open()' that aborts the program (by calling
    'abort()') on error."""
    
    fp = None
    if fn == '-':
        if mode == 'r':
            fp = sys.stdin
        elif mode == 'w':
            fp = sys.stdout
    else:
        fp = open (fn, mode) or abort ("Can't open file, '%s'." % fn)
    return fp

def close_file (fp):
    """close_file (fp)

    Calls fp.close() if 'fp' is not None, and just returns
    otherwise."""
    
    if fp:
        fp.close ()

#**************************************************************************
# Regular expression substrings for various MPI things.

mpi_id_restr = "MPI_[A-Z0-9_]+"  # MPI constant identifier
mpi_typename_restr = "MPI_[A-Z][A-Za-z0-9_]*"  # MPI type name

# MPI typedef: "typedef <base-type> <type-id>;"
mpi_typedef_restr = "typedef (.*) (" + mpi_typename_restr + ");"
mpi_typedef_matcher = re.compile (mpi_typedef_restr)
def match_mpi_typedef (line):
    global mpi_typedef_matcher
    assert (mpi_typedef_matcher)
    m = mpi_typedef_matcher.match (line)
    if m:
        return {'base-type' : m.groups (1),
                'type-id' : m.groups (2)}
    return None

# MPI basic define: "#define <id> <val>"
mpi_def1_restr = "# *define +(" + mpi_id_restr + ")\s+\(? *(-?[0-9]+) *\)?( +.*)?"
mpi_def1_matcher = re.compile (mpi_def1_restr)
def match_mpi_def (line):
    global mpi_def1_matcher
    assert (mpi_def1_matcher)
    m = mpi_def1_matcher.match (line)
    if m:
        return {'id' : m.group (1), 'val' : m.group (2)}
    return None

# MPI typed define: "#define <id> ((<type-id>)<val>)"
mpi_typed_def1_restr = "# *define +(" + mpi_id_restr + ") +\( *\( *(" + mpi_typename_restr + ") *\) *([0-9]+) *\)"
mpi_typed_def1_matcher = re.compile (mpi_typed_def1_restr)
mpi_typed_def2_restr = "# *define +(" + mpi_id_restr + ") +\( *(" + mpi_typename_restr + ") *\) *\( *([0-9]+) *\)"
mpi_typed_def2_matcher = re.compile (mpi_typed_def2_restr)
mpi_typed_def3_restr = "# *define +(" + mpi_id_restr + ")\s+\(\s*(.*)\s*\)\s*([0-9]+)"
mpi_typed_def3_matcher = re.compile (mpi_typed_def3_restr)
def match_mpi_typed_def (line):
    global mpi_typed_def1_matcher
    assert (mpi_typed_def1_matcher)
    global mpi_typed_def2_matcher
    assert (mpi_typed_def2_matcher)
    m = mpi_typed_def1_matcher.match (line)
    if not m:
        m = mpi_typed_def2_matcher.match (line)
    if not m:
        m = mpi_typed_def3_matcher.match (line)
    if m:
        return {'id' : m.group (1),
                'type-id' : m.group (2),
                'val' : m.group (3)}
    return None

#**************************************************************************

def rewrite_mpi_header (fp_in, fp_out, new_defs):
    """rewrite_mpi_header (fp_in, fp_out, fp_defs)

    Rewrites the '#define' statements in an MPI header file to use
    declared external constants. 'fp_in' is a file pointer to the
    original header file, 'fp_out' to the desired output file,
    and 'fp_defs' to store the defining declarations."""

    # Stores new declarations.
    # Use a hash to prevent duplicate declarations.
    new_externs = {}
    for line in fp_in.xreadlines ():
        line = string.rstrip (line)

        m = match_mpi_def (line)
        if m:
            new_externs[m['id']] = "extern const int %s; /* == %s @MOD@ */\n" \
                                   % (m['id'], m['val'])
            write_line (fp_out, "/* Replacing #define %s */" % m['id'])
            new_defs[m['id']] = "const int %s = %s;\n" % (m['id'], m['val'])
            continue

        m = match_mpi_typed_def (line)
        if m:
            new_externs[m['id']] = "extern const %s %s; /* == %s @MOD@ */\n" \
                                   % (m['type-id'], m['id'], m['val'])
            write_line (fp_out, "/* Replacing #define %s */" % m['id'])
            new_defs[m['id']] = "const %s %s = %s;\n" \
                                % (m['type-id'], m['id'], m['val'])
            continue

        # No match; just dump the declaration as-is.
        write_line (fp_out, line);

    # Dump all new 'extern' declarations.
    fp_out.writelines (new_externs.values ())
        
#**************************************************************************
# MAIN
#**************************************************************************

# === Default option values. ===
opts = {
    'out-incs' : './include',
    'out-srcs' : './lib'
    }

# === Process optional arguments. ===
opts_re_str = "[a-zA-Z0-9_-]+"
opts_flag_str = "--(" + opts_re_str + ")"
opts_flag_matcher = re.compile (opts_flag_str + "$")
opts_matcher = re.compile (opts_flag_str + "=(.*)$")
i = 1
while i < len (sys.argv):
    arg = sys.argv[i]
    if arg in ['-h', '-H', '--help']:
        usage ()

    m = opts_matcher.match (arg)
    if m:
        if m.group (1) in ['out-incs', 'out-srcs']:
            opts[m.group (1)] = m.group (2)
        else:
            usage ()
        i = i + 1
        continue

    m = opts_flag_matcher.match (arg)
    if m:
        usage ("Unrecognized option, '%s'." % m.group (0))
        
    # No matches.
    break

# === Process required arguments. ===
i+1 == len (sys.argv) or usage ("Wrong number of required arguments.")

# === Define input/output file paths. ===
filenames = {
    'mpi-install-dir' : sys.argv[i],
    'out-incs' : opts['out-incs'],
    'out-srcs' : opts['out-srcs'],
    'mpi-header-in' : sys.argv[i] + '/include/mpi.h',
    'mpi-header-out' : opts['out-incs'] + '/mpi.h',
    'mpi-header-defs-in' : sys.argv[i] + '/include/mpidefs.h',
    'mpi-header-defs-out' : opts['out-incs'] + '/mpidefs.h',
    'mpi-header-fortdefs-in' : sys.argv[i] + '/include/mpi_fortdefs.h',
    'mpi-header-fortdefs-out' : opts['out-incs'] + '/mpi_fortdefs.h',
    'mpi-header-io-in' : sys.argv[i] + '/include/mpio.h',
    'mpi-header-io-out' : opts['out-incs'] + '/mpio.h',
    'mpi-header-errno-in' : sys.argv[i] + '/include/mpi_errno.h',
    'mpi-header-errno-out' : opts['out-incs'] + '/mpi_errno.h',
    'mpi-defs-out' : opts['out-srcs'] + '/def.c'
    }

# === Create output directory. ===

for dn in ['out-incs', 'out-srcs']:
    mkdir_p (filenames[dn]) \
            or abort ("Can't create output directory, '%s'." \
                      % filenames[dn])

# === Convert the MPI header. ===

rewrite_file_pairs = [('mpi-header-in', 'mpi-header-out'),
                      ('mpi-header-defs-in', 'mpi-header-defs-out'),
                      ('mpi-header-fortdefs-in', 'mpi-header-fortdefs-out'),
                      ('mpi-header-io-in', 'mpi-header-io-out'),
                      ('mpi-header-errno-in', 'mpi-header-errno-out')]
new_defs = {}
for (fn_in, fn_out) in rewrite_file_pairs:
    
    sys.stderr.write ("... Rewriting '%s' ==> '%s ...\n" \
                      % (filenames[fn_in], filenames[fn_out]))
    
    fp_in = open_file (filenames[fn_in])
    fp_out = open_file (filenames[fn_out], 'w')

    # Insert a warning message just to verify that we got the right header.
    # JJW (11/13/2008): #warning messages are not useful -- use a comment
    # instead
    write_line (fp_out, '// "%s, derived from %s."' \
                % (filenames[fn_out], filenames[fn_in]))

    rewrite_mpi_header (fp_in, fp_out, new_defs)

    close_file (fp_out)
    close_file (fp_in)

# Write definitions.
fp_defs = open_file (filenames['mpi-defs-out'], 'w')
write_line (fp_defs, '#include <mpi.h>')
write_line (fp_defs, '')
fp_defs.writelines (new_defs.values ())
fp_defs.writelines (['\n', '/* eof */\n'])

close_file (fp_defs)

# eof
