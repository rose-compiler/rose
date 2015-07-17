/*

Copyright (C) 1993-2011 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

// Use the GNU readline library for command line editing and hisory.

#if !defined (octave_input_h)
#define octave_input_h 1

#include <cstdio>

#include <string>

/*#include "oct-time.h"
#include "oct-obj.h"
#include "pager.h"*/

//class octave_value;

//extern int octave_read (char *buf, unsigned max_size);
/*extern OCTINTERP_API FILE *get_input_from_file (const std::string& name, int warn = 1);
extern OCTINTERP_API FILE *get_input_from_stdin (void);*/

// Global pointer for eval().
extern std::string current_eval_string;

// TRUE means get input from current_eval_string.
extern bool get_input_from_eval_string;

// TRUE means we haven't been asked for the input from
// current_eval_string yet.
extern bool input_from_eval_string_pending;

// TRUE means that input is coming from a file that was named on
// the command line.
extern bool input_from_command_line_file;

// TRUE means that stdin is a terminal, not a pipe or redirected file.
extern bool stdin_is_tty;

// TRUE means we're parsing a function file.
extern bool reading_fcn_file;

// Simple name of function file we are reading.
extern std::string curr_fcn_file_name;

// Full name of file we are reading.
extern std::string curr_fcn_file_full_name;

// TRUE means we're parsing a script file.
extern bool reading_script_file;

// TRUE means we're parsing a classdef file.
extern bool reading_classdef_file;

// If we are reading from an M-file, this is it.
extern FILE *ff_instream;

// TRUE means this is an interactive shell.
extern bool interactive;

// TRUE means the user forced this shell to be interactive (-i).
extern bool forced_interactive;

// Should we issue a prompt?
extern int promptflag;

// A line of input.
extern std::string current_input_line;

// TRUE after a call to completion_matches.
extern bool octave_completion_matches_called;

// TRUE if the plotting system has requested a call to drawnow at
// the next user prompt.
/*extern OCTINTERP_API bool Vdrawnow_requested;

// TRUE if we are in debugging mode.
extern OCTINTERP_API bool Vdebugging;

extern std::string gnu_readline (const std::string& s, bool force_readline = false);

extern void initialize_command_input (void);

extern bool octave_yes_or_no (const std::string& prompt);

extern octave_value do_keyboard (const octave_value_list& args = octave_value_list ());*/

extern std::string VPS4;

extern char Vfilemarker;

enum echo_state
{
  ECHO_OFF = 0,
  ECHO_SCRIPTS = 1,
  ECHO_FUNCTIONS = 2,
  ECHO_CMD_LINE = 4
};

extern int Vecho_executing_commands;

//extern octave_time Vlast_prompt_time;

#endif
