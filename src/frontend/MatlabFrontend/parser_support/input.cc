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

// Get command input interactively or from files.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <iostream>
#include <sstream>
#include <string>

#include <sys/types.h>
#include <unistd.h>
/*
#include "cmd-edit.h"
#include "file-ops.h"
#include "quit.h"
#include "str-vec.h"

#include "debug.h"
#include "defun.h"
#include "dirfns.h"
#include "error.h"
#include "gripes.h"
#include "help.h"
#include "input.h"
#include "lex.h"
#include "load-path.h"
#include "oct-map.h"
#include "oct-hist.h"
#include "toplev.h"
#include "oct-obj.h"
#include "pager.h"
#include "parse.h"
#include "pathlen.h"
#include "pt.h"
#include "pt-const.h"
#include "pt-eval.h"
#include "pt-stmt.h"
#include "sighandlers.h"
#include "sysdep.h"
#include "toplev.h"
#include "unwind-prot.h"
#include "utils.h"
#include "variables.h"*/

// Primary prompt string.
static std::string VPS1 = "\\s:\\#> ";

// Secondary prompt string.
static std::string VPS2 = "> ";

// String printed before echoed input (enabled by --echo-input).
std::string VPS4 = "+ ";

// Echo commands as they are executed?
//
//   1  ==>  echo commands read from script files
//   2  ==>  echo commands from functions
//   4  ==>  echo commands read from command line
//
// more than one state can be active at once.
//int Vecho_executing_commands = ECHO_OFF;

// The time we last printed a prompt.
//octave_time Vlast_prompt_time = 0.0;

// Character to append after successful command-line completion attempts.
//static char Vcompletion_append_char = ' ';

// Global pointer for eval().
std::string current_eval_string;

// TRUE means get input from current_eval_string.
bool get_input_from_eval_string = false;

// TRUE means we haven't been asked for the input from
// current_eval_string yet.
bool input_from_eval_string_pending = false;

// TRUE means that input is coming from a file that was named on
// the command line.
bool input_from_command_line_file = false;

// TRUE means that stdin is a terminal, not a pipe or redirected file.
bool stdin_is_tty = false;

// TRUE means we're parsing a function file.
bool reading_fcn_file = false;

// TRUE means we're parsing a classdef file.
bool reading_classdef_file = false;

// Simple name of function file we are reading.
std::string curr_fcn_file_name;

// Full name of file we are reading.
std::string curr_fcn_file_full_name;

// TRUE means we're parsing a script file.
bool reading_script_file = false;

// If we are reading from an M-file, this is it.
FILE *ff_instream = 0;

// TRUE means this is an interactive shell.
bool interactive = false;

// TRUE means the user forced this shell to be interactive (-i).
bool forced_interactive = false;

// Should we issue a prompt?
int promptflag = 1;

// The current line of input, from wherever.
std::string current_input_line;

// TRUE after a call to completion_matches.
bool octave_completion_matches_called = false;

/*// TRUE if the plotting system has requested a call to drawnow at
// the next user prompt.
bool Vdrawnow_requested = false;

// TRUE if we are in debugging mode.
bool Vdebugging = false;*/

// If we are in debugging mode, this is the last command entered, so
// that we can repeat the previous command if the user just types RET.
static std::string last_debugging_command;

// TRUE if we are running in the Emacs GUD mode.
//static bool Vgud_mode = false;

// The filemarker used to separate filenames from subfunction names
char Vfilemarker = '>';
/*
static void
do_input_echo (const std::string& input_string)
{
  int do_echo = reading_script_file ?
    (Vecho_executing_commands & ECHO_SCRIPTS)
      : (Vecho_executing_commands & ECHO_CMD_LINE) && ! forced_interactive;

  if (do_echo)
    {
      if (forced_interactive)
        {
          if (promptflag > 0)
            octave_stdout << command_editor::decode_prompt_string (VPS1);
          else
            octave_stdout << command_editor::decode_prompt_string (VPS2);
        }
      else
        octave_stdout << command_editor::decode_prompt_string (VPS4);

      if (! input_string.empty ())
        {
          octave_stdout << input_string;

          if (input_string[input_string.length () - 1] != '\n')
            octave_stdout << "\n";
        }
    }
}

std::string
gnu_readline (const std::string& s, bool force_readline)
{
  octave_quit ();

  std::string retval;

  if (line_editing || force_readline)
    {
      bool eof;

      retval = command_editor::readline (s, eof);

      if (! eof && retval.empty ())
        retval = "\n";
    }
  else
    {
      if (! s.empty () && (interactive || forced_interactive))
        {
          FILE *stream = command_editor::get_output_stream ();

          gnulib::fputs (s.c_str (), stream);
          fflush (stream);
        }

      FILE *curr_stream = command_editor::get_input_stream ();

      if (reading_fcn_file || reading_script_file || reading_classdef_file)
        curr_stream = ff_instream;

      retval = octave_fgets (curr_stream);
    }

  return retval;
}

static inline std::string
interactive_input (const std::string& s, bool force_readline = false)
{
  Vlast_prompt_time.stamp ();

  if (Vdrawnow_requested && (interactive || forced_interactive))
    {
      feval ("drawnow");

      flush_octave_stdout ();

      // We set Vdrawnow_requested to false even if there is an error
      // in drawnow so that the error doesn't reappear at every prompt.

      Vdrawnow_requested = false;

      if (error_state)
        return "\n";
    }

  return gnu_readline (s, force_readline);
}

static std::string
octave_gets (void)
{
  //octave_quit ();

  std::string retval;

  bool history_skip_auto_repeated_debugging_command = false;

  if ((interactive || forced_interactive)
      && (! (reading_fcn_file
             || reading_classdef_file
             || reading_script_file
             || get_input_from_eval_string
             || input_from_startup_file
             || input_from_command_line_file)))
    {
      std::string ps = (promptflag > 0) ? VPS1 : VPS2;

      std::string prompt = command_editor::decode_prompt_string (ps);

      pipe_handler_error_count = 0;

      flush_octave_stdout ();

      octave_diary << prompt;

      retval = interactive_input (prompt);

      // There is no need to update the load_path cache if there is no
      // user input.
      if (! retval.empty ()
          && retval.find_first_not_of (" \t\n\r") != std::string::npos)
        {
          load_path::update ();

          if (Vdebugging)
            last_debugging_command = retval;
          else
            last_debugging_command = std::string ();
        }
      else if (Vdebugging)
        {
          retval = last_debugging_command;
          history_skip_auto_repeated_debugging_command = true;
        }
    }
  else
    retval = gnu_readline ("");

  current_input_line = retval;

  if (! current_input_line.empty ())
    {
      if (! (input_from_startup_file || input_from_command_line_file
             || history_skip_auto_repeated_debugging_command))
        command_history::add (current_input_line);

      if (! (reading_fcn_file || reading_script_file || reading_classdef_file))
        {
          octave_diary << current_input_line;

          if (current_input_line[current_input_line.length () - 1] != '\n')
            octave_diary << "\n";
        }

      do_input_echo (current_input_line);
    }
  else if (! (reading_fcn_file || reading_script_file || reading_classdef_file))
    octave_diary << "\n";

  return retval;
}

// Read a line from the input stream.

static std::string
get_user_input (void)
{
  //octave_quit ();

  std::string retval;

  if (get_input_from_eval_string)
    {
      if (input_from_eval_string_pending)
        {
          input_from_eval_string_pending = false;

          retval = current_eval_string;

          size_t len = retval.length ();

          if (len > 0 && retval[len-1] != '\n')
            retval.append ("\n");
        }
    }
  else
    retval = octave_gets ();

  current_input_line = retval;

  return retval;
}
*/
/*
int
octave_read (char *buf, unsigned max_size)
{
  // FIXME -- is this a safe way to buffer the input?

  static const char * const eol = "\n";
  static std::string input_buf;
  static const char *pos = 0;
  static size_t chars_left = 0;

  int status = 0;
  if (chars_left == 0)
    {
      pos = 0;

      input_buf = get_user_input ();

      chars_left = input_buf.length ();

      pos = input_buf.c_str ();
    }

  if (chars_left > 0)
    {
      size_t len = max_size > chars_left ? chars_left : max_size;
      assert (len > 0);

      memcpy (buf, pos, len);

      chars_left -= len;
      pos += len;

      // Make sure input ends with a new line character.
      if (chars_left == 0 && buf[len-1] != '\n')
        {
          if (len < max_size)
            {
              // There is enough room to plug the newline character in
              // the buffer.
              buf[len++] = '\n';
            }
          else
            {
              // There isn't enough room to plug the newline character
              // in the buffer so make sure it is returned on the next
              // octave_read call.
              pos = eol;
              chars_left = 1;
            }
        }

      status = len;

    }
  else if (chars_left == 0)
    {
      status = 0;
    }
  else
    status = -1;

  return status;
}
*/
// Fix things up so that input can come from file `name', printing a
// warning if the file doesn't exist.
/*
FILE *
get_input_from_file (const std::string& name, int warn)
{
  FILE *instream = 0;

  if (name.length () > 0)
    instream = fopen (name.c_str (), "rb");

  if (! instream && warn)
    warning ("%s: no such file or directory", name.c_str ());

  if (reading_fcn_file || reading_script_file || reading_classdef_file)
    ff_instream = instream;
  else
    command_editor::set_input_stream (instream);

  return instream;
}

// Fix things up so that input can come from the standard input.  This
// may need to become much more complicated, which is why it's in a
// separate function.

FILE *
get_input_from_stdin (void)
{
  command_editor::set_input_stream (stdin);
  return command_editor::get_input_stream ();
}

// FIXME -- make this generate file names when appropriate.

static string_vector
generate_possible_completions (const std::string& text, std::string& prefix,
                               std::string& hint)
{
  string_vector names;

  prefix = "";

  if (looks_like_struct (text))
    names = generate_struct_completions (text, prefix, hint);
  else
    names = make_name_list ();

  // Sort and remove duplicates.

  names.sort (true);

  return names;
}

static bool
is_completing_dirfns (void)
{
  static std::string dirfns_commands[] = {"cd", "ls"};
  static const size_t dirfns_commands_length = 2;

  bool retval = false;

  std::string line = command_editor::get_line_buffer ();

  for (size_t i = 0; i < dirfns_commands_length; i++)
    {
      int index = line.find (dirfns_commands[i] + " ");

      if (index == 0)
        {
          retval = true;
          break;
        }
    }

  return retval;
}

static std::string
generate_completion (const std::string& text, int state)
{
  std::string retval;

  static std::string prefix;
  static std::string hint;

  static size_t hint_len = 0;

  static int list_index = 0;
  static int name_list_len = 0;
  static int name_list_total_len = 0;
  static string_vector name_list;
  static string_vector file_name_list;

  static int matches = 0;

  if (state == 0)
    {
      list_index = 0;

      prefix = "";

      hint = text;

      // No reason to display symbols while completing a
      // file/directory operation.

      if (is_completing_dirfns ())
        name_list = string_vector ();
      else
        name_list = generate_possible_completions (text, prefix, hint);

      name_list_len = name_list.length ();

      file_name_list = command_editor::generate_filename_completions (text);

      name_list.append (file_name_list);

      name_list_total_len = name_list.length ();

      hint_len = hint.length ();

      matches = 0;

      for (int i = 0; i < name_list_len; i++)
        if (hint == name_list[i].substr (0, hint_len))
          matches++;
    }

  if (name_list_total_len > 0 && matches > 0)
    {
      while (list_index < name_list_total_len)
        {
          std::string name = name_list[list_index];

          list_index++;

          if (hint == name.substr (0, hint_len))
            {
              if (list_index <= name_list_len && ! prefix.empty ())
                retval = prefix + "." + name;
              else
                retval = name;

              // FIXME -- looks_like_struct is broken for now,
              // so it always returns false.

              if (matches == 1 && looks_like_struct (retval))
                {
                  // Don't append anything, since we don't know
                  // whether it should be '(' or '.'.

                  command_editor::set_completion_append_character ('\0');
                }
              else
                command_editor::set_completion_append_character
                  (Vcompletion_append_char);

              break;
            }
        }
    }

  return retval;
}

static std::string
quoting_filename (const std::string &text, int, char quote)
{
  if (quote)
    return text;
  else
    return (std::string ("'") + text);
}

void
initialize_command_input (void)
{
  // If we are using readline, this allows conditional parsing of the
  // .inputrc file.

  command_editor::set_name ("Octave");

  // FIXME -- this needs to include a comma too, but that
  // causes trouble for the new struct element completion code.

  static const char *s = "\t\n !\"\'*+-/:;<=>(){}[\\]^`~";

  command_editor::set_basic_word_break_characters (s);

  command_editor::set_completer_word_break_characters (s);

  command_editor::set_basic_quote_characters ("\"");

  command_editor::set_filename_quote_characters (" \t\n\\\"'@<>=;|&()#$`?*[!:{");
  command_editor::set_completer_quote_characters ("'\"");

  command_editor::set_completion_function (generate_completion);

  command_editor::set_quoting_function (quoting_filename);
}

static void
get_debug_input (const std::string& prompt)
{
  octave_user_code *caller = octave_call_stack::caller_user_code ();
  std::string nm;

  int curr_debug_line = octave_call_stack::current_line ();

  bool have_file = false;

  if (caller)
    {
      nm = caller->fcn_file_name ();

      if (nm.empty ())
        nm = caller->name ();
      else
        have_file = true;
    }
  else
    curr_debug_line = -1;

  std::ostringstream buf;

  if (! nm.empty ())
    {
      if (Vgud_mode)
        {
          static char ctrl_z = 'Z' & 0x1f;

          buf << ctrl_z << ctrl_z << nm << ":" << curr_debug_line;
        }
      else
        {
          // FIXME -- we should come up with a clean way to detect
          // that we are stopped on the no-op command that marks the
          // end of a function or script.

          buf << "stopped in " << nm;

          if (curr_debug_line > 0)
            buf << " at line " << curr_debug_line;

          if (have_file)
            {
              std::string line_buf
                = get_file_line (nm, curr_debug_line);

              if (! line_buf.empty ())
                buf << "\n" << curr_debug_line << ": " << line_buf;
            }
        }
    }

  std::string msg = buf.str ();

  if (! msg.empty ())
    std::cerr << msg << std::endl;

  unwind_protect frame;

  frame.protect_var (VPS1);
  VPS1 = prompt;

  if (stdin_is_tty)
    {
      if (! (interactive || forced_interactive)
          || (reading_fcn_file
              || reading_classdef_file
              || reading_script_file
              || get_input_from_eval_string
              || input_from_startup_file
              || input_from_command_line_file))
        {
          frame.protect_var (forced_interactive);
          forced_interactive = true;

          frame.protect_var (reading_fcn_file);
          reading_fcn_file = false;

          frame.protect_var (reading_classdef_file);
          reading_classdef_file = false;

          frame.protect_var (reading_script_file);
          reading_script_file = false;

          frame.protect_var (input_from_startup_file);
          input_from_startup_file = false;

          frame.protect_var (input_from_command_line_file);
          input_from_command_line_file = false;

          frame.protect_var (get_input_from_eval_string);
          get_input_from_eval_string = false;

          YY_BUFFER_STATE old_buf = current_buffer ();
          YY_BUFFER_STATE new_buf = create_buffer (get_input_from_stdin ());

          // FIXME: are these safe?
          frame.add_fcn (switch_to_buffer, old_buf);
          frame.add_fcn (delete_buffer, new_buf);

          switch_to_buffer (new_buf);
        }

      while (Vdebugging)
        {
          reset_error_handler ();

          reset_parser ();

          // Save current value of global_command.
          frame.protect_var (global_command);

          global_command = 0;

          // Do this with an unwind-protect cleanup function so that the
          // forced variables will be unmarked in the event of an interrupt.
          symbol_table::scope_id scope = symbol_table::top_scope ();
          frame.add_fcn (symbol_table::unmark_forced_variables, scope);

          // This is the same as yyparse in parse.y.
          int retval = octave_parse ();

          if (retval == 0 && global_command)
            {
              global_command->accept (*current_evaluator);

              // FIXME -- To avoid a memory leak, global_command should be
              // deleted, I think.  But doing that here causes trouble if
              // an error occurs while executing a debugging command
              // (dbstep, for example). It's not clear to me why that
              // happens.
              //
              // delete global_command;
              //
              // global_command = 0;

              if (octave_completion_matches_called)
                octave_completion_matches_called = false;
            }

          // Unmark forced variables.
          // Restore previous value of global_command.
          frame.run_top (2);

          octave_quit ();
        }
    }
  else
    warning ("invalid attempt to debug script read from stdin");
}
*/
// If the user simply hits return, this will produce an empty matrix.

/*static octave_value_list
get_user_input (const octave_value_list& args, int nargout)
{
  octave_value_list retval;

  int nargin = args.length ();

  int read_as_string = 0;

  if (nargin == 2)
    read_as_string++;

  std::string prompt = args(0).string_value ();

  if (error_state)
    {
      error ("input: unrecognized argument");
      return retval;
    }

  flush_octave_stdout ();

  octave_diary << prompt;

  std::string input_buf = interactive_input (prompt.c_str (), true);

  if (! (error_state || input_buf.empty ()))
    {
      if (! input_from_startup_file)
        command_history::add (input_buf);

      size_t len = input_buf.length ();

      octave_diary << input_buf;

      if (input_buf[len - 1] != '\n')
        octave_diary << "\n";

      if (len < 1)
        return read_as_string ? octave_value ("") : octave_value (Matrix ());

      if (read_as_string)
        {
          // FIXME -- fix gnu_readline and octave_gets instead!
          if (input_buf.length () == 1 && input_buf[0] == '\n')
            retval(0) = "";
          else
            retval(0) = input_buf;
        }
      else
        {
          int parse_status = 0;

          retval = eval_string (input_buf, true, parse_status, nargout);

          if (! Vdebugging && retval.length () == 0)
            retval(0) = Matrix ();
        }
    }
  else
    error ("input: reading user-input failed!");

  return retval;
}*/
/*
DEFUN (input, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} input (@var{prompt})\n\
@deftypefnx {Built-in Function} {} input (@var{prompt}, \"s\")\n\
Print a prompt and wait for user input.  For example,\n\
\n\
@example\n\
input (\"Pick a number, any number! \")\n\
@end example\n\
\n\
@noindent\n\
prints the prompt\n\
\n\
@example\n\
Pick a number, any number!\n\
@end example\n\
\n\
@noindent\n\
and waits for the user to enter a value.  The string entered by the user\n\
is evaluated as an expression, so it may be a literal constant, a\n\
variable name, or any other valid expression.\n\
\n\
Currently, @code{input} only returns one value, regardless of the number\n\
of values produced by the evaluation of the expression.\n\
\n\
If you are only interested in getting a literal string value, you can\n\
call @code{input} with the character string @code{\"s\"} as the second\n\
argument.  This tells Octave to return the string entered by the user\n\
directly, without evaluating it first.\n\
\n\
Because there may be output waiting to be displayed by the pager, it is\n\
a good idea to always call @code{fflush (stdout)} before calling\n\
@code{input}.  This will ensure that all pending output is written to\n\
the screen before your prompt.  @xref{Input and Output}.\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin == 1 || nargin == 2)
    retval = get_user_input (args, nargout);
  else
    print_usage ();

  return retval;
}

bool
octave_yes_or_no (const std::string& prompt)
{
  std::string prompt_string = prompt + "(yes or no) ";

  while (1)
    {
      std::string input_buf = interactive_input (prompt_string, true);

      if (input_buf == "yes")
        return true;
      else if (input_buf == "no")
        return false;
      else
        message (0, "Please answer yes or no.");
    }
}

DEFUN (yes_or_no, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} yes_or_no (@var{prompt})\n\
Ask the user a yes-or-no question.  Return 1 if the answer is yes.\n\
Takes one argument, which is the string to display to ask the\n\
question.  It should end in a space; @samp{yes-or-no-p} adds\n\
@samp{(yes or no) } to it.  The user must confirm the answer with\n\
RET and can edit it until it has been confirmed.\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 0 || nargin == 1)
    {
      std::string prompt;

      if (nargin == 1)
        {
          prompt = args(0).string_value ();

          if (error_state)
            {
              error ("yes_or_no: PROMPT must be a character string");
              return retval;
            }
        }

      retval = octave_yes_or_no (prompt);
    }
  else
    print_usage ();

  return retval;
}

octave_value
do_keyboard (const octave_value_list& args)
{
  octave_value retval;

  int nargin = args.length ();

  assert (nargin == 0 || nargin == 1);

  unwind_protect frame;

  // FIXME -- we shouldn't need both the
  // command_history object and the
  // Vsaving_history variable...
  command_history::ignore_entries (false);

  frame.add_fcn (command_history::ignore_entries, ! Vsaving_history);

  frame.protect_var (Vsaving_history);
  frame.protect_var (Vdebugging);

  frame.add_fcn (octave_call_stack::restore_frame,
                 octave_call_stack::current_frame ());

  // FIXME -- probably we just want to print one line, not the
  // entire statement, which might span many lines...
  //
  // tree_print_code tpc (octave_stdout);
  // stmt.accept (tpc);

  Vsaving_history = true;
  Vdebugging = true;

  std::string prompt = "debug> ";
  if (nargin > 0)
    prompt = args(0).string_value ();

  if (! error_state)
    get_debug_input (prompt);

  return retval;
}

DEFUN (keyboard, args, ,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} keyboard ()\n\
@deftypefnx {Built-in Function} {} keyboard (@var{prompt})\n\
This function is normally used for simple debugging.  When the\n\
@code{keyboard} function is executed, Octave prints a prompt and waits\n\
for user input.  The input strings are then evaluated and the results\n\
are printed.  This makes it possible to examine the values of variables\n\
within a function, and to assign new values if necessary.  To leave the\n\
prompt and return to normal execution type @samp{return} or @samp{dbcont}.\n\
The @code{keyboard} function does not return an exit status.\n\
\n\
If @code{keyboard} is invoked without arguments, a default prompt of\n\
@samp{debug> } is used.\n\
@seealso{dbcont, dbquit}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin == 0 || nargin == 1)
    {
      unwind_protect frame;

      frame.add_fcn (octave_call_stack::restore_frame,
                     octave_call_stack::current_frame ());

      // Skip the frame assigned to the keyboard function.
      octave_call_stack::goto_frame_relative (0);

      tree_evaluator::debug_mode = true;

      tree_evaluator::current_frame = octave_call_stack::current_frame ();

      do_keyboard (args);
    }
  else
    print_usage ();

  return retval;
}

DEFUN (echo, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Command} {} echo options\n\
Control whether commands are displayed as they are executed.  Valid\n\
options are:\n\
\n\
@table @code\n\
@item on\n\
Enable echoing of commands as they are executed in script files.\n\
\n\
@item off\n\
Disable echoing of commands as they are executed in script files.\n\
\n\
@item on all\n\
Enable echoing of commands as they are executed in script files and\n\
functions.\n\
\n\
@item off all\n\
Disable echoing of commands as they are executed in script files and\n\
functions.\n\
@end table\n\
\n\
@noindent\n\
With no arguments, @code{echo} toggles the current echo state.\n\
@end deftypefn")
{
  octave_value_list retval;

  int argc = args.length () + 1;

  string_vector argv = args.make_argv ("echo");

  if (error_state)
    return retval;

  switch (argc)
    {
    case 1:
      {
        if ((Vecho_executing_commands & ECHO_SCRIPTS)
            || (Vecho_executing_commands & ECHO_FUNCTIONS))
          Vecho_executing_commands = ECHO_OFF;
        else
          Vecho_executing_commands = ECHO_SCRIPTS;
      }
      break;

    case 2:
      {
        std::string arg = argv[1];

        if (arg == "on")
          Vecho_executing_commands = ECHO_SCRIPTS;
        else if (arg == "off")
          Vecho_executing_commands = ECHO_OFF;
        else
          print_usage ();
      }
      break;

    case 3:
      {
        std::string arg = argv[1];

        if (arg == "on" && argv[2] == "all")
          {
            int tmp = (ECHO_SCRIPTS | ECHO_FUNCTIONS);
            Vecho_executing_commands = tmp;
          }
        else if (arg == "off" && argv[2] == "all")
          Vecho_executing_commands = ECHO_OFF;
        else
          print_usage ();
      }
      break;

    default:
      print_usage ();
      break;
    }

  return retval;
}

DEFUN (completion_matches, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} completion_matches (@var{hint})\n\
Generate possible completions given @var{hint}.\n\
\n\
This function is provided for the benefit of programs like Emacs which\n\
might be controlling Octave and handling user input.  The current\n\
command number is not incremented when this function is called.  This is\n\
a feature, not a bug.\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 1)
    {
      std::string hint = args(0).string_value ();

      if (! error_state)
        {
          int n = 32;

          string_vector list (n);

          int k = 0;

          for (;;)
            {
              std::string cmd = generate_completion (hint, k);

              if (! cmd.empty ())
                {
                  if (k == n)
                    {
                      n *= 2;
                      list.resize (n);
                    }

                  list[k++] = cmd;
                }
              else
                {
                  list.resize (k);
                  break;
                }
            }

          if (nargout > 0)
            {
              if (! list.empty ())
                retval = list;
              else
                retval = "";
            }
          else
            {
              // We don't use string_vector::list_in_columns here
              // because it will be easier for Emacs if the names
              // appear in a single column.

              int len = list.length ();

              for (int i = 0; i < len; i++)
                octave_stdout << list[i] << "\n";
            }

          octave_completion_matches_called = true;
        }
    }
  else
    print_usage ();

  return retval;
}

DEFUN (read_readline_init_file, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} read_readline_init_file (@var{file})\n\
Read the readline library initialization file @var{file}.  If\n\
@var{file} is omitted, read the default initialization file (normally\n\
@file{~/.inputrc}).\n\
\n\
@xref{Readline Init File, , , readline, GNU Readline Library},\n\
for details.\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin == 0)
    command_editor::read_init_file ();
  else if (nargin == 1)
    {
      std::string file = args(0).string_value ();

      if (! error_state)
        command_editor::read_init_file (file);
    }
  else
    print_usage ();

  return retval;
}

DEFUN (re_read_readline_init_file, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} re_read_readline_init_file ()\n\
Re-read the last readline library initialization file that was read.\n\
@xref{Readline Init File, , , readline, GNU Readline Library},\n\
for details.\n\
@end deftypefn")
{
  octave_value_list retval;

  if (args.length () == 0)
    command_editor::re_read_init_file ();
  else
    print_usage ();

  return retval;
}

typedef std::map<std::string, octave_value> hook_fcn_map_type;

static hook_fcn_map_type hook_fcn_map;

static int
input_event_hook (void)
{
  if (! lexer_flags.defining_func)
    {
      hook_fcn_map_type::iterator p = hook_fcn_map.begin ();

      while (p != hook_fcn_map.end ())
        {
          std::string hook_fcn = p->first;
          octave_value user_data = p->second;

          hook_fcn_map_type::iterator q = p++;

          if (is_valid_function (hook_fcn))
            {
              if (user_data.is_defined ())
                feval (hook_fcn, user_data, 0);
              else
                feval (hook_fcn, octave_value_list (), 0);
            }
          else
            hook_fcn_map.erase (q);
        }

      if (hook_fcn_map.empty ())
        command_editor::remove_event_hook (input_event_hook);
    }

  return 0;
}

DEFUN (add_input_event_hook, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} add_input_event_hook (@var{fcn}, @var{data})\n\
Add the named function @var{fcn} to the list of functions to call\n\
periodically when Octave is waiting for input.  The function should\n\
have the form\n\
\n\
@example\n\
@var{fcn} (@var{data})\n\
@end example\n\
\n\
If @var{data} is omitted, Octave calls the function without any\n\
arguments.\n\
@seealso{remove_input_event_hook}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin == 1 || nargin == 2)
    {
      octave_value user_data;

      if (nargin == 2)
        user_data = args(1);

      std::string hook_fcn = args(0).string_value ();

      if (! error_state)
        {
          if (hook_fcn_map.empty ())
            command_editor::add_event_hook (input_event_hook);

          hook_fcn_map[hook_fcn] = user_data;
        }
      else
        error ("add_input_event_hook: expecting string as first arg");
    }
  else
    print_usage ();

  return retval;
}

DEFUN (remove_input_event_hook, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} remove_input_event_hook (@var{fcn})\n\
Remove the named function @var{fcn} to the list of functions to call\n\
periodically when Octave is waiting for input.\n\
@seealso{add_input_event_hook}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin == 1)
    {
      std::string hook_fcn = args(0).string_value ();

      if (! error_state)
        {
          hook_fcn_map_type::iterator p = hook_fcn_map.find (hook_fcn);

          if (p != hook_fcn_map.end ())
            hook_fcn_map.erase (p);
          else
            error ("remove_input_event_hook: %s not found in list",
                   hook_fcn.c_str ());

          if (hook_fcn_map.empty ())
            command_editor::remove_event_hook (input_event_hook);
        }
      else
        error ("remove_input_event_hook: expecting string as first arg");
    }
  else
    print_usage ();

  return retval;
}

DEFUN (PS1, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} PS1 ()\n\
@deftypefnx {Built-in Function} {@var{old_val} =} PS1 (@var{new_val})\n\
Query or set the primary prompt string.  When executing interactively,\n\
Octave displays the primary prompt when it is ready to read a command.\n\
\n\
The default value of the primary prompt string is @code{\"\\s:\\#> \"}.\n\
To change it, use a command like\n\
\n\
@example\n\
PS1 (\"\\\\u@@\\\\H> \")\n\
@end example\n\
\n\
@noindent\n\
which will result in the prompt @samp{boris@@kremvax> } for the user\n\
@samp{boris} logged in on the host @samp{kremvax.kgb.su}.  Note that two\n\
backslashes are required to enter a backslash into a double-quoted\n\
character string.  @xref{Strings}.\n\
\n\
You can also use ANSI escape sequences if your terminal supports them.\n\
This can be useful for coloring the prompt.  For example,\n\
\n\
@example\n\
PS1 (\"\\\\[\\\\033[01;31m\\\\]\\\\s:\\\\#> \\\\[\\\\033[0m\\]\")\n\
@end example\n\
\n\
@noindent\n\
will give the default Octave prompt a red coloring.\n\
@seealso{PS2, PS4}\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (PS1);
}

DEFUN (PS2, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} PS2 ()\n\
@deftypefnx {Built-in Function} {@var{old_val} =} PS2 (@var{new_val})\n\
Query or set the secondary prompt string.  The secondary prompt is\n\
printed when Octave is expecting additional input to complete a\n\
command.  For example, if you are typing a @code{for} loop that spans several\n\
lines, Octave will print the secondary prompt at the beginning of\n\
each line after the first.  The default value of the secondary prompt\n\
string is @code{\"> \"}.\n\
@seealso{PS1, PS4}\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (PS2);
}

DEFUN (PS4, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} PS4 ()\n\
@deftypefnx {Built-in Function} {@var{old_val} =} PS4 (@var{new_val})\n\
Query or set the character string used to prefix output produced\n\
when echoing commands is enabled.\n\
The default value is @code{\"+ \"}.\n\
@xref{Diary and Echo Commands}, for a description of echoing commands.\n\
@seealso{echo, echo_executing_commands, PS1, PS2}\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (PS4);
}

DEFUN (completion_append_char, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} completion_append_char ()\n\
@deftypefnx {Built-in Function} {@var{old_val} =} completion_append_char (@var{new_val})\n\
Query or set the internal character variable that is appended to\n\
successful command-line completion attempts.  The default\n\
value is @code{\" \"} (a single space).\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (completion_append_char);
}

DEFUN (echo_executing_commands, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} echo_executing_commands ()\n\
@deftypefnx {Built-in Function} {@var{old_val} =} echo_executing_commands (@var{new_val})\n\
Query or set the internal variable that controls the echo state.\n\
It may be the sum of the following values:\n\
\n\
@table @asis\n\
@item 1\n\
Echo commands read from script files.\n\
\n\
@item 2\n\
Echo commands from functions.\n\
\n\
@item 4\n\
Echo commands read from command line.\n\
@end table\n\
\n\
More than one state can be active at once.  For example, a value of 3 is\n\
equivalent to the command @kbd{echo on all}.\n\
\n\
The value of @code{echo_executing_commands} may be set by the @kbd{echo}\n\
command or the command line option @option{--echo-commands}.\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (echo_executing_commands);
}

DEFUN (__request_drawnow__, args, ,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} __request_drawnow__ ()\n\
@deftypefnx {Built-in Function} {} __request_drawnow__ (@var{flag})\n\
Undocumented internal function.\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 0)
    Vdrawnow_requested = true;
  else if (nargin == 1)
    Vdrawnow_requested = args(0).bool_value ();
  else
    print_usage ();

  return retval;
}

DEFUN (__gud_mode__, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} __gud_mode__ ()\n\
Undocumented internal function.\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 0)
    retval = Vgud_mode;
  else if (nargin == 1)
    Vgud_mode = args(0).bool_value ();
  else
    print_usage ();

  return retval;
}

DEFUN (filemarker, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} filemarker ()\n\
@deftypefnx {Built-in Function} {} filemarker (@var{new_val})\n\
Query or set the character used to separate filename from the\n\
the subfunction names contained within the file.  This can be used in\n\
a generic manner to interact with subfunctions.  For example,\n\
\n\
@example\n\
help ([\"myfunc\", filemarker, \"mysubfunc\"])\n\
@end example\n\
\n\
@noindent\n\
returns the help string associated with the sub-function @code{mysubfunc}\n\
of the function @code{myfunc}.  Another use of @code{filemarker} is when\n\
debugging it allows easier placement of breakpoints within sub-functions.\n\
For example,\n\
\n\
@example\n\
dbstop ([\"myfunc\", filemarker, \"mysubfunc\"])\n\
@end example\n\
\n\
@noindent\n\
will set a breakpoint at the first line of the subfunction @code{mysubfunc}.\n\
@end deftypefn")
{
  char tmp = Vfilemarker;
  octave_value retval = SET_INTERNAL_VARIABLE (filemarker);

  // The character passed must not be a legal character for a function name
  if (! error_state && (::isalnum (Vfilemarker) || Vfilemarker == '_'))
    {
      Vfilemarker = tmp;
      error ("filemarker: character can not be a valid character for a function name");
    }

  return retval;
}
*/
