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

#include <cstdarg>
#include <cstring>

#include <iostream>
#include <sstream>
#include <string>

/*#include "defun.h"
#include "error.h"
#include "input.h"
#include "pager.h"
#include "oct-obj.h"
#include "oct-map.h"
#include "utils.h"
#include "ov.h"
#include "ov-usr-fcn.h"
#include "pt-pr-code.h"
#include "pt-stmt.h"
#include "toplev.h"
#include "unwind-prot.h"
#include "variables.h"
*/
// TRUE means that Octave will try to beep obnoxiously before printing
// error messages.
static bool Vbeep_on_error = false;

// TRUE means that Octave will try to enter the debugger when an error
// is encountered.  This will also inhibit printing of the normal
// traceback message (you will only see the top-level error message).
bool Vdebug_on_error = false;

// TRUE means that Octave will try to enter the debugger when a warning
// is encountered.
bool Vdebug_on_warning = false;

// TRUE means that Octave will try to display a stack trace when a
// warning is encountered.
static bool Vbacktrace_on_warning = false;

// TRUE means that Octave will print a verbose warning.  Currently unused.
static bool Vverbose_warning;

// TRUE means that Octave will print no warnings, but lastwarn will be
//updated
static bool Vquiet_warning = false;

// A structure containing (most of) the current state of warnings.
//static octave_map warning_options;

// The text of the last error message.
static std::string Vlast_error_message;

// The text of the last warning message.
static std::string Vlast_warning_message;

// The last warning message id.
static std::string Vlast_warning_id;

// The last error message id.
static std::string Vlast_error_id;

// The last file in which an error occured
//static octave_map Vlast_error_stack;

// Current error state.
//
// Valid values:
//
//   -2: an error has occurred, but don't print any messages.
//   -1: an error has occurred, we are printing a traceback
//    0: no error
//    1: an error has occurred
//
int error_state = 0;

// Current warning state.
//
//  Valid values:
//
//    0: no warning
//    1: a warning has occurred
//
int warning_state = 0;

// Tell the error handler whether to print messages, or just store
// them for later.  Used for handling errors in eval() and
// the `unwind_protect' statement.
int buffer_error_messages = 0;

// TRUE means error messages are turned off.
bool discard_error_messages = false;

// TRUE means warning messages are turned off.
bool discard_warning_messages = false;

// The message buffer.
static std::ostringstream *error_message_buffer = 0;

/*void
reset_error_handler (void)
{
  error_state = 0;
  warning_state = 0;
  buffer_error_messages = 0;
  discard_error_messages = false;
}

static void
initialize_warning_options (const std::string& state)
{
  octave_scalar_map initw;

  initw.setfield ("identifier", "all");
  initw.setfield ("state", state);

  warning_options = initw;
}

static octave_map
initialize_last_error_stack (void)
{
  return octave_call_stack::empty_backtrace ();
}

// Warning messages are never buffered.

static void
vwarning (const char *name, const char *id, const char *fmt, va_list args)
{
  if (discard_warning_messages)
    return;

  flush_octave_stdout ();

  std::ostringstream output_buf;

  if (name)
    output_buf << name << ": ";

  octave_vformat (output_buf, fmt, args);

  output_buf << std::endl;

  // FIXME -- we really want to capture the message before it
  // has all the formatting goop attached to it.  We probably also
  // want just the message, not the traceback information.

  std::string msg_string = output_buf.str ();

  if (! warning_state)
    {
      // This is the first warning in a possible series.

      Vlast_warning_id = id;
      Vlast_warning_message = msg_string;
    }

  if (! Vquiet_warning)
    {
      octave_diary << msg_string;

      std::cerr << msg_string;
    }
}

static void
verror (bool save_last_error, std::ostream& os,
        const char *name, const char *id, const char *fmt, va_list args,
        bool with_cfn = false)
{
  if (discard_error_messages)
    return;

  if (! buffer_error_messages)
    flush_octave_stdout ();

  // FIXME -- we really want to capture the message before it
  // has all the formatting goop attached to it.  We probably also
  // want just the message, not the traceback information.

  std::ostringstream output_buf;

  octave_vformat (output_buf, fmt, args);

  std::string base_msg = output_buf.str ();

  bool to_beep_or_not_to_beep_p = Vbeep_on_error && ! error_state;

  std::string msg_string;

  if (to_beep_or_not_to_beep_p)
    msg_string = "\a";

  if (name)
    msg_string += std::string (name) + ": ";

  // If with_fcn is specified, we'll attempt to prefix the message with the name
  // of the current executing function. But we'll do so only if:
  // 1. the name is not empty (anonymous function)
  // 2. it is not already there (including the following colon)
  if (with_cfn)
    {
      octave_function *curfcn = octave_call_stack::current ();
      if (curfcn)
        {
          std::string cfn = curfcn->name ();
          if (! cfn.empty ())
            {
              cfn += ':';
              if (cfn.length () > base_msg.length ()
                 || base_msg.compare (0, cfn.length (), cfn) != 0)
                {
                  msg_string += cfn + ' ';
                }
            }
        }
    }

  msg_string += base_msg + "\n";

  if (! error_state && save_last_error)
    {
      // This is the first error in a possible series.

      Vlast_error_id = id;
      Vlast_error_message = base_msg;

      octave_user_code *fcn = octave_call_stack::caller_user_code ();

      if (fcn)
        {
          octave_idx_type curr_frame = -1;

          Vlast_error_stack = octave_call_stack::backtrace (0, curr_frame);
        }
      else
        Vlast_error_stack = initialize_last_error_stack ();
    }

  if (buffer_error_messages)
    {
      if (error_message_buffer)
        msg_string = "error: " + msg_string;
      else
        error_message_buffer = new std::ostringstream ();

      *error_message_buffer << msg_string;
    }
  else
    {
      octave_diary << msg_string;
      os << msg_string;
    }
}

// Note that we don't actually print any message if the error string
// is just "" or "\n".  This allows error ("") and error ("\n") to
// just set the error state.

static void
error_1 (std::ostream& os, const char *name, const char *id,
         const char *fmt, va_list args, bool with_cfn = false)
{
  if (error_state != -2)
    {
      if (fmt)
        {
          if (*fmt)
            {
              size_t len = strlen (fmt);

              if (len > 0)
                {
                  if (fmt[len - 1] == '\n')
                    {
                      if (len > 1)
                        {
                          char *tmp_fmt = strdup (fmt);
                          tmp_fmt[len - 1] = '\0';
                          //verror (true, os, name, id, tmp_fmt, args, with_cfn);
                          error("ERROR");
                          delete [] tmp_fmt;
                        }

                      error_state = -2;
                    }
                  else
                    {
                      //verror (true, os, name, id, fmt, args, with_cfn);
                      error("ERROR");

                      if (! error_state)
                        error_state = 1;
                    }
                }
            }
        }
      else
        panic ("error_1: invalid format");
    }
}

void
vmessage (const char *name, const char *fmt, va_list args)
{
  verror (false, std::cerr, name, "", fmt, args);
}

void
message (const char *name, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vmessage (name, fmt, args);
  va_end (args);
}

void
vmessage_with_id (const char *name, const char *id, const char *fmt,
                  va_list args)
{
  verror (false, std::cerr, name, id, fmt, args);
}

void
message_with_id (const char *name, const char *id, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vmessage_with_id (name, id, fmt, args);
  va_end (args);
}

void
usage_1 (const char *id, const char *fmt, va_list args)
{
  verror (true, std::cerr, "usage", id, fmt, args);
  error_state = -1;
}

void
vusage (const char *fmt, va_list args)
{
  usage_1 ("", fmt, args);
}

void
usage (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vusage (fmt, args);
  va_end (args);
}

void
vusage_with_id (const char *id, const char *fmt, va_list args)
{
  usage_1 (id, fmt, args);
}

void
usage_with_id (const char *id, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vusage_with_id (id, fmt, args);
  va_end (args);
}

static void
pr_where_2 (const char *fmt, va_list args)
{
  if (fmt)
    {
      if (*fmt)
        {
          size_t len = strlen (fmt);

          if (len > 0)
            {
              if (fmt[len - 1] == '\n')
                {
                  if (len > 1)
                    {
                      char *tmp_fmt = strsave (fmt);
                      tmp_fmt[len - 1] = '\0';
                      verror (false, std::cerr, 0, "", tmp_fmt, args);
                      delete [] tmp_fmt;
                    }
                }
              else
                verror (false, std::cerr, 0, "", fmt, args);
            }
        }
    }
  else
    panic ("pr_where_2: invalid format");
}

static void
pr_where_1 (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  pr_where_2 (fmt, args);
  va_end (args);
}

static void
pr_where (const char *who)
{
  octave_idx_type curr_frame = -1;

  octave_map stk = octave_call_stack::backtrace (0, curr_frame);

  octave_idx_type nframes_to_display = stk.numel ();

  if (nframes_to_display > 0)
    {
      pr_where_1 ("%s: called from\n", who);

      Cell names = stk.contents ("name");
      Cell lines = stk.contents ("line");
      Cell columns = stk.contents ("column");

      for (octave_idx_type i = 0; i < nframes_to_display; i++)
        {
          octave_value name = names(i);
          octave_value line = lines(i);
          octave_value column = columns(i);

          std::string nm = name.string_value ();

          pr_where_1 ("    %s at line %d column %d\n", nm.c_str (),
                      line.int_value (), column.int_value ());
        }
    }
}
*/

void error (const char *fmt, ...);

static void
error_2 (const char *id, const char *fmt, va_list args, bool with_cfn = false)
{
  //int init_state = error_state;

  //error_1 (std::cerr, "error", id, fmt, args, with_cfn);

  //error("ERROR");
  // if ((interactive || forced_interactive)
  //     && Vdebug_on_error && init_state == 0
  //     && octave_call_stack::caller_user_code ())
  //   {
  //     unwind_protect frame;
  //     frame.protect_var (Vdebug_on_error);
  //     Vdebug_on_error = false;

  //     error_state = 0;

  //     pr_where ("error");

  //     do_keyboard (octave_value_list ());
  //   }
}

void
verror (const char *fmt, va_list args)
{
  error_2 ("", fmt, args);
}

void
error (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  verror (fmt, args);
  va_end (args);
}

/*void
verror_with_cfn (const char *fmt, va_list args)
{
  error_2 ("", fmt, args, true);
}

void
error_with_cfn (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  verror_with_cfn (fmt, args);
  va_end (args);
}

void
verror_with_id (const char *id, const char *fmt, va_list args)
{
  error_2 (id, fmt, args);
}

void
error_with_id (const char *id, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  verror_with_id (id, fmt, args);
  va_end (args);
}

void
verror_with_id_cfn (const char *id, const char *fmt, va_list args)
{
  error_2 (id, fmt, args, true);
}

void
error_with_id_cfn (const char *id, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  verror_with_id_cfn (id, fmt, args);
  va_end (args);
}

static int
check_state (const std::string& state)
{
  // -1: not found
  //  0: found, "off"
  //  1: found, "on"
  //  2: found, "error"

  if (state == "off")
    return 0;
  else if (state == "on")
    return 1;
  else if (state == "error")
    return 2;
  else
    return -1;
}

// For given warning ID, return 0 if warnings are disabled, 1 if
// enabled, and 2 if this ID should be an error instead of a warning.

int
warning_enabled (const std::string& id)
{
  int retval = 0;

  int all_state = -1;
  int id_state = -1;

  octave_idx_type nel = warning_options.numel ();

  if (nel > 0)
    {
      Cell identifier = warning_options.contents ("identifier");
      Cell state = warning_options.contents ("state");

      bool all_found = false;
      bool id_found = false;

      for (octave_idx_type i = 0; i < nel; i++)
        {
          octave_value ov = identifier(i);
          std::string ovs = ov.string_value ();

          if (! all_found && ovs == "all")
            {
              all_state = check_state (state(i).string_value ());

              if (all_state >= 0)
                all_found = true;
            }

          if (! id_found && ovs == id)
            {
              id_state = check_state (state(i).string_value ());

              if (id_state >= 0)
                id_found = true;
            }

          if (all_found && id_found)
            break;
        }
    }

  if (all_state == -1)
    panic_impossible ();

  if (all_state == 0)
    {
      if (id_state >= 0)
        retval = id_state;
    }
  else if (all_state == 1)
    {
      if (id_state == 0 || id_state == 2)
        retval = id_state;
      else
        retval = all_state;
    }
  else if (all_state == 2)
    {
      if (id_state == 0)
        retval= id_state;
      else
        retval = all_state;
    }

  return retval;
}
*/
static void
warning_1 (const char *id, const char *fmt, va_list args)
{
  // int warn_opt = warning_enabled (id);

  // if (warn_opt == 2)
  //   {
      // Handle this warning as an error.

	std::cerr << "Warning";
  //   }
  // else if (warn_opt == 1)
  //   {
  //     vwarning ("warning", id, fmt, args);

  //     if (! symbol_table::at_top_level ()
  //         && Vbacktrace_on_warning
  //         && ! warning_state
  //         && ! discard_warning_messages)
  //       pr_where ("warning");

  //     warning_state = 1;

  //     if ((interactive || forced_interactive)
  //         && Vdebug_on_warning
  //         && octave_call_stack::caller_user_code ())
  //       {
  //         unwind_protect frame;
  //         frame.protect_var (Vdebug_on_warning);
  //         Vdebug_on_warning = false;

  //         do_keyboard (octave_value_list ());
  //       }
  //   }
}

void
vwarning (const char *fmt, va_list args)
{
  warning_1 ("", fmt, args);
}

void
warning (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vwarning (fmt, args);
  va_end (args);
}

void
vwarning_with_id (const char *id, const char *fmt, va_list args)
{
  warning_1 (id, fmt, args);
}

void
warning_with_id (const char *id, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vwarning_with_id (id, fmt, args);
  va_end (args);
}
/*
void
vparse_error (const char *fmt, va_list args)
{
  error_1 (std::cerr, 0, "", fmt, args);
}

void
parse_error (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vparse_error (fmt, args);
  va_end (args);
}

void
vparse_error_with_id (const char *id, const char *fmt, va_list args)
{
  error_1 (std::cerr, 0, id, fmt, args);
}

void
parse_error_with_id (const char *id, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vparse_error_with_id (id, fmt, args);
  va_end (args);
}

void
rethrow_error (const char *id, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  error_1 (std::cerr, 0, id, fmt, args);
  va_end (args);
}
*/
void
panic (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  buffer_error_messages = 0;
  discard_error_messages = false;
  verror (fmt, args);
  va_end (args);
  //abort ();
}
/*
static void
defun_usage_message_1 (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  error_1 (octave_stdout, 0, "", fmt, args);
  va_end (args);
}

void
defun_usage_message (const std::string& msg)
{
  defun_usage_message_1 ("%s", msg.c_str ());
}

typedef void (*error_fun)(const char *, const char *, ...);

extern octave_value_list Fsprintf (const octave_value_list&, int);

static std::string
handle_message (error_fun f, const char *id, const char *msg,
                const octave_value_list& args)
{
  std::string retval;

  std::string tstr;

  int nargin = args.length ();

  if (nargin > 0)
    {
      octave_value arg;

      if (nargin > 1)
        {
          octave_value_list tmp = Fsprintf (args, 1);
          arg = tmp(0);
        }
      else
        arg = args(0);

      if (arg.is_defined ())
        {
          if (arg.is_string ())
            {
              tstr = arg.string_value ();
              msg = tstr.c_str ();

              if (! msg)
                return retval;
            }
          else if (arg.is_empty ())
            return retval;
        }
    }

// Ugh.

  size_t len = strlen (msg);

  if (len > 0)
    {
      if (msg[len - 1] == '\n')
        {
          if (len > 1)
            {
              char *tmp_msg = strsave (msg);
              tmp_msg[len - 1] = '\0';
              f (id, "%s\n", tmp_msg);
              retval = tmp_msg;
              delete [] tmp_msg;
            }
        }
      else
        {
          f (id, "%s", msg);
          retval = msg;
        }
    }

  return retval;
}

DEFUN (rethrow, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} rethrow (@var{err})\n\
Reissue a previous error as defined by @var{err}.  @var{err} is a structure\n\
that must contain at least the 'message' and 'identifier' fields.  @var{err}\n\
can also contain a field 'stack' that gives information on the assumed\n\
location of the error.  Typically @var{err} is returned from\n\
@code{lasterror}.\n\
@seealso{lasterror, lasterr, error}\n\
@end deftypefn")
{
  octave_value retval;
  int nargin = args.length();

  if (nargin != 1)
    print_usage ();
  else
    {
      const octave_scalar_map err = args(0).scalar_map_value ();

      if (! error_state)
        {
          if (err.contains ("message") && err.contains ("identifier"))
            {
              std::string msg = err.contents("message").string_value ();
              std::string id = err.contents("identifier").string_value ();
              int len = msg.length();

              std::string file;
              std::string nm;
              int l = -1;
              int c = -1;

              octave_map err_stack = initialize_last_error_stack ();

              if (err.contains ("stack"))
                {
                  err_stack = err.contents("stack").map_value ();

                  if (err_stack.numel () > 0)
                    {
                      if (err_stack.contains ("file"))
                        file = err_stack.contents("file")(0).string_value ();

                      if (err_stack.contains ("name"))
                        nm = err_stack.contents("name")(0).string_value ();

                      if (err_stack.contains ("line"))
                        l = err_stack.contents("line")(0).nint_value ();

                      if (err_stack.contains ("column"))
                        c = err_stack.contents("column")(0).nint_value ();
                    }
                }

              // Ugh.
              char *tmp_msg = strsave (msg.c_str ());
              if (tmp_msg[len-1] == '\n')
                {
                  if (len > 1)
                    {
                      tmp_msg[len - 1] = '\0';
                      rethrow_error (id.c_str (), "%s\n", tmp_msg);
                    }
                }
              else
                rethrow_error (id.c_str (), "%s", tmp_msg);
              delete [] tmp_msg;

              // FIXME -- is this the right thing to do for
              // Vlast_error_stack?  Should it be saved and restored
              // with unwind_protect?

              Vlast_error_stack = err_stack;

              if (err.contains ("stack"))
                {
                  if (file.empty ())
                    {
                      if (nm.empty ())
                        {
                          if (l > 0)
                            {
                              if (c > 0)
                                pr_where_1 ("error: near line %d, column %d",
                                            l, c);
                              else
                                pr_where_1 ("error: near line %d", l);
                            }
                        }
                      else
                        {
                          if (l > 0)
                            {
                              if (c > 0)
                                pr_where_1 ("error: called from `%s' near line %d, column %d",
                                            nm.c_str (), l, c);
                              else
                                pr_where_1 ("error: called from `%d' near line %d", nm.c_str (), l);
                            }
                        }
                    }
                  else
                    {
                      if (nm.empty ())
                        {
                          if (l > 0)
                            {
                              if (c > 0)
                                pr_where_1 ("error: in file %s near line %d, column %d",
                                            file.c_str (), l, c);
                              else
                                pr_where_1 ("error: in file %s near line %d", file.c_str (), l);
                            }
                        }
                      else
                        {
                          if (l > 0)
                            {
                              if (c > 0)
                                pr_where_1 ("error: called from `%s' in file %s near line %d, column %d",
                                            nm.c_str (), file.c_str (), l, c);
                              else
                                pr_where_1 ("error: called from `%d' in file %s near line %d", nm.c_str (), file.c_str (), l);
                            }
                        }
                    }
                }
            }
          else
            error ("rethrow: ERR structure must contain the fields 'message and 'identifier'");
        }
    }
  return retval;
}

DEFUN (error, args, ,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} error (@var{template}, @dots{})\n\
@deftypefnx {Built-in Function} {} error (@var{id}, @var{template}, @dots{})\n\
Format the optional arguments under the control of the template string\n\
@var{template} using the same rules as the @code{printf} family of\n\
functions (@pxref{Formatted Output}) and print the resulting message\n\
on the @code{stderr} stream.  The message is prefixed by the character\n\
string @samp{error: }.\n\
\n\
Calling @code{error} also sets Octave's internal error state such that\n\
control will return to the top level without evaluating any more\n\
commands.  This is useful for aborting from functions or scripts.\n\
\n\
If the error message does not end with a new line character, Octave will\n\
print a traceback of all the function calls leading to the error.  For\n\
example, given the following function definitions:\n\
\n\
@example\n\
@group\n\
function f () g (); end\n\
function g () h (); end\n\
function h () nargin == 1 || error (\"nargin != 1\"); end\n\
@end group\n\
@end example\n\
\n\
@noindent\n\
calling the function @code{f} will result in a list of messages that\n\
can help you to quickly locate the exact location of the error:\n\
\n\
@example\n\
@group\n\
f ()\n\
error: nargin != 1\n\
error: called from:\n\
error:   error at line -1, column -1\n\
error:   h at line 1, column 27\n\
error:   g at line 1, column 15\n\
error:   f at line 1, column 15\n\
@end group\n\
@end example\n\
\n\
If the error message ends in a new line character, Octave will print the\n\
message but will not display any traceback messages as it returns\n\
control to the top level.  For example, modifying the error message\n\
in the previous example to end in a new line causes Octave to only print\n\
a single message:\n\
\n\
@example\n\
@group\n\
function h () nargin == 1 || error (\"nargin != 1\\n\"); end\n\
f ()\n\
error: nargin != 1\n\
@end group\n\
@end example\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  octave_value_list nargs = args;

  std::string id;

  if (nargin == 0)
    print_usage ();
  else
    {
      if (nargin > 1)
        {
          std::string arg1 = args(0).string_value ();

          if (! error_state)
            {
              if (arg1.find ('%') == std::string::npos)
                {
                  id = arg1;

                  nargs.resize (nargin-1);

                  for (int i = 1; i < nargin; i++)
                    nargs(i-1) = args(i);
                }
            }
          else
            return retval;
        }
      else if (nargin == 1 && args(0).is_map ())
        {
          octave_value_list tmp;

          octave_scalar_map m = args(0).scalar_map_value ();

          if (m.contains ("message"))
            {
              octave_value c = m.getfield ("message");

              if (c.is_string ())
                 nargs(0) = c.string_value ();
            }

          if (m.contains ("identifier"))
            {
              octave_value c = m.getfield ("identifier");

              if (c.is_string ())
                 id = c.string_value ();
            }

          // FIXME -- also need to handle "stack" field in error
          // structure, but that will require some more significant
          // surgery on handle_message, error_with_id, etc.
        }

      handle_message (error_with_id, id.c_str (), "unspecified error", nargs);
    }

  return retval;
}

DEFUN (warning, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} warning (@var{template}, @dots{})\n\
@deftypefnx {Built-in Function} {} warning (@var{id}, @var{template}, @dots{})\n\
Format the optional arguments under the control of the template string\n\
@var{template} using the same rules as the @code{printf} family of\n\
functions (@pxref{Formatted Output}) and print the resulting message\n\
on the @code{stderr} stream.  The message is prefixed by the character\n\
string @samp{warning: }.\n\
You should use this function when you want to notify the user\n\
of an unusual condition, but only when it makes sense for your program\n\
to go on.\n\
\n\
The optional message identifier allows users to enable or disable\n\
warnings tagged by @var{id}.  The special identifier @samp{\"all\"} may\n\
be used to set the state of all warnings.\n\
\n\
@deftypefnx {Built-in Function} {} warning (\"on\", @var{id})\n\
@deftypefnx {Built-in Function} {} warning (\"off\", @var{id})\n\
@deftypefnx {Built-in Function} {} warning (\"error\", @var{id})\n\
@deftypefnx {Built-in Function} {} warning (\"query\", @var{id})\n\
Set or query the state of a particular warning using the identifier\n\
@var{id}.  If the identifier is omitted, a value of @samp{\"all\"} is\n\
assumed.  If you set the state of a warning to @samp{\"error\"}, the\n\
warning named by @var{id} is handled as if it were an error instead.\n\
@seealso{warning_ids}\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();
  int argc = nargin + 1;

  bool done = false;

  if (argc > 1 && args.all_strings_p ())
    {
      string_vector argv = args.make_argv ("warning");

      if (! error_state)
        {
          std::string arg1 = argv(1);
          std::string arg2 = "all";

          if (argc == 3)
            arg2 = argv(2);

          if (arg1 == "on" || arg1 == "off" || arg1 == "error")
            {
              octave_map old_warning_options = warning_options;

              if (arg2 == "all")
                {
                  octave_map tmp;

                  Cell id (1, 1);
                  Cell st (1, 1);

                  id(0) = arg2;
                  st(0) = arg1;

                  // Since internal Octave functions are not
                  // compatible, turning all warnings into errors
                  // should leave the state of
                  // Octave:matlab-incompatible alone.

                  if (arg1 == "error"
                      && warning_options.contains ("identifier"))
                    {
                      octave_idx_type n = 1;

                      Cell tid = warning_options.contents ("identifier");
                      Cell tst = warning_options.contents ("state");

                      for (octave_idx_type i = 0; i < tid.numel (); i++)
                        {
                          octave_value vid = tid(i);

                          if (vid.is_string ())
                            {
                              std::string key = vid.string_value ();

                              if (key == "Octave:matlab-incompatible"
                                  || key == "Octave:single-quote-string")
                                {
                                  id.resize (dim_vector (1, n+1));
                                  st.resize (dim_vector (1, n+1));

                                  id(n) = tid(i);
                                  st(n) = tst(i);

                                  n++;
                                }
                            }
                        }
                    }

                  tmp.assign ("identifier", id);
                  tmp.assign ("state", st);

                  warning_options = tmp;

                  done = true;
                }
              else if (arg2 == "backtrace")
                {
                  if (arg1 != "error")
                    {
                      Vbacktrace_on_warning = (arg1 == "on");
                      done = true;
                    }
                }
              else if (arg2 == "debug")
                {
                  if (arg1 != "error")
                    {
                      Vdebug_on_warning = (arg1 == "on");
                      done = true;
                    }
                }
              else if (arg2 == "verbose")
                {
                  if (arg1 != "error")
                    {
                      Vverbose_warning = (arg1 == "on");
                      done = true;
                    }
                }
              else if (arg2 == "quiet")
                {
                  if (arg1 != "error")
                    {
                      Vquiet_warning = (arg1 == "on");
                      done = true;
                    }
                }
              else
                {
                  if (arg2 == "last")
                    arg2 = Vlast_warning_id;

                  if (arg2 == "all")
                    initialize_warning_options (arg1);
                  else
                    {
                      Cell ident = warning_options.contents ("identifier");
                      Cell state = warning_options.contents ("state");

                      octave_idx_type nel = ident.numel ();

                      bool found = false;

                      for (octave_idx_type i = 0; i < nel; i++)
                        {
                          if (ident(i).string_value () == arg2)
                            {
                              // FIXME -- if state for "all" is
                              // same as arg1, we can simply remove the
                              // item from the list.

                              state(i) = arg1;
                              warning_options.assign ("state", state);
                              found = true;
                              break;
                            }
                        }

                      if (! found)
                        {
                          // FIXME -- if state for "all" is
                          // same as arg1, we don't need to do anything.

                          ident.resize (dim_vector (1, nel+1));
                          state.resize (dim_vector (1, nel+1));

                          ident(nel) = arg2;
                          state(nel) = arg1;

                          warning_options.clear ();

                          warning_options.assign ("identifier", ident);
                          warning_options.assign ("state", state);
                        }
                    }

                  done = true;
                }

              if (done && nargout > 0)
                retval = old_warning_options;
            }
          else if (arg1 == "query")
            {
              if (arg2 == "all")
                retval = warning_options;
              else if (arg2 == "backtrace" || arg2 == "debug"
                       || arg2 == "verbose" || arg2 == "quiet")
                {
                  octave_scalar_map tmp;
                  tmp.assign ("identifier", arg2);
                  if (arg2 == "backtrace")
                    tmp.assign ("state", Vbacktrace_on_warning ? "on" : "off");
                  else if (arg2 == "debug")
                    tmp.assign ("state", Vdebug_on_warning ? "on" : "off");
                  else if (arg2 == "verbose")
                    tmp.assign ("state", Vverbose_warning ? "on" : "off");
                  else
                    tmp.assign ("state", Vquiet_warning ? "on" : "off");

                  retval = tmp;
                }
              else
                {
                  if (arg2 == "last")
                    arg2 = Vlast_warning_id;

                  Cell ident = warning_options.contents ("identifier");
                  Cell state = warning_options.contents ("state");

                  octave_idx_type nel = ident.numel ();

                  bool found = false;

                  std::string val;

                  for (octave_idx_type i = 0; i < nel; i++)
                    {
                      if (ident(i).string_value () == arg2)
                        {
                          val = state(i).string_value ();
                          found = true;
                          break;
                        }
                    }

                  if (! found)
                    {
                      for (octave_idx_type i = 0; i < nel; i++)
                        {
                          if (ident(i).string_value () == "all")
                            {
                              val = state(i).string_value ();
                              found = true;
                              break;
                            }
                        }
                    }

                  if (found)
                    {
                      octave_scalar_map tmp;

                      tmp.assign ("identifier", arg2);
                      tmp.assign ("state", val);

                      retval = tmp;
                    }
                  else
                    error ("warning: unable to find default warning state!");
                }

              done = true;
            }
        }
    }
  else if (argc == 1)
    {
      retval = warning_options;

      done = true;
    }
  else if (argc == 2)
    {
      octave_value arg = args(0);

      octave_map old_warning_options = warning_options;

      if (arg.is_map ())
        {
          octave_map m = arg.map_value ();

          if (m.contains ("identifier") && m.contains ("state"))
            warning_options = m;
          else
            error ("warning: expecting structure with fields `identifier' and `state'");

          done = true;

          if (nargout > 0)
            retval = old_warning_options;
        }
    }

  if (! (error_state || done))
    {
      octave_value_list nargs = args;

      std::string id;

      if (nargin > 1)
        {
          std::string arg1 = args(0).string_value ();

          if (! error_state)
            {
              if (arg1.find ('%') == std::string::npos)
                {
                  id = arg1;

                  nargs.resize (nargin-1);

                  for (int i = 1; i < nargin; i++)
                    nargs(i-1) = args(i);
                }
            }
          else
            return retval;
        }

      std::string prev_msg = Vlast_warning_message;

      std::string curr_msg = handle_message (warning_with_id, id.c_str (),
                                             "unspecified warning", nargs);

      if (nargout > 0)
        retval = prev_msg;
    }

  return retval;
}

octave_value_list
set_warning_state (const std::string& id, const std::string& state)
{
  octave_value_list args;

  args(1) = id;
  args(0) = state;

  return Fwarning (args, 1);
}

octave_value_list
set_warning_state (const octave_value_list& args)
{
  return Fwarning (args, 1);
}

void
disable_warning (const std::string& id)
{
  set_warning_state (id, "off");
}

void
initialize_default_warning_state (void)
{
  initialize_warning_options ("on");

  // Most people will want to have the following disabled.

  disable_warning ("Octave:array-to-scalar");
  disable_warning ("Octave:array-to-vector");
  disable_warning ("Octave:empty-list-elements");
  disable_warning ("Octave:fortran-indexing");
  disable_warning ("Octave:imag-to-real");
  disable_warning ("Octave:matlab-incompatible");
  disable_warning ("Octave:missing-semicolon");
  disable_warning ("Octave:neg-dim-as-zero");
  disable_warning ("Octave:resize-on-range-error");
  disable_warning ("Octave:separator-insert");
  disable_warning ("Octave:single-quote-string");
  disable_warning ("Octave:str-to-num");
  disable_warning ("Octave:string-concat");
  disable_warning ("Octave:variable-switch-label");
  disable_warning ("Octave:complex-cmp-ops");

  // This should be an error unless we are in maximum braindamage mode.

  set_warning_state ("Octave:allow-noninteger-ranges-as-indices", "error");
}

DEFUN (lasterror, args, ,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{lasterr} =} lasterror ()\n\
@deftypefnx {Built-in Function} {} lasterror (@var{err})\n\
@deftypefnx {Built-in Function} {} lasterror ('reset')\n\
Query or set the last error message.  Called without any arguments\n\
returns a structure containing the last error message, as well as other\n\
information related to this error.  The elements of this structure are:\n\
\n\
@table @asis\n\
@item 'message'\n\
The text of the last error message\n\
\n\
@item 'identifier'\n\
The message identifier of this error message\n\
\n\
@item 'stack'\n\
A structure containing information on where the message occurred.  This might\n\
be an empty structure if this in the case where this information cannot\n\
be obtained.  The fields of this structure are:\n\
\n\
@table @asis\n\
@item 'file'\n\
The name of the file where the error occurred\n\
\n\
@item 'name'\n\
The name of function in which the error occurred\n\
\n\
@item 'line'\n\
The line number at which the error occurred\n\
\n\
@item 'column'\n\
An optional field with the column number at which the error occurred\n\
@end table\n\
@end table\n\
\n\
The @var{err} structure may also be passed to @code{lasterror} to set the\n\
information about the last error.  The only constraint on @var{err} in that\n\
case is that it is a scalar structure.  Any fields of @var{err} that match\n\
the above are set to the value passed in @var{err}, while other fields are\n\
set to their default values.\n\
\n\
If @code{lasterror} is called with the argument 'reset', all values take\n\
their default values.\n\
@end deftypefn")
{
  octave_value retval;
  int nargin = args.length();

  unwind_protect frame;

  frame.protect_var (error_state);
  error_state = 0;

  if (nargin < 2)
    {
      octave_scalar_map err;

      err.assign ("message", Vlast_error_message);
      err.assign ("identifier", Vlast_error_id);

      err.assign ("stack", octave_value (Vlast_error_stack));

      if (nargin == 1)
        {
          if (args(0).is_string())
            {
              if (args(0).string_value () == "reset")
                {
                  Vlast_error_message = std::string();
                  Vlast_error_id = std::string();

                  Vlast_error_stack = initialize_last_error_stack ();
                }
              else
                error("lasterror: unrecognized string argument");
            }
          else if (args(0).is_map ())
            {
              octave_scalar_map new_err = args(0).scalar_map_value ();
              std::string new_error_message;
              std::string new_error_id;
              std::string new_error_file;
              std::string new_error_name;
              int new_error_line = -1;
              int new_error_column = -1;

              if (! error_state && new_err.contains ("message"))
                {
                  const std::string tmp =
                    new_err.getfield("message").string_value ();
                  new_error_message = tmp;
                }

              if (! error_state && new_err.contains ("identifier"))
                {
                  const std::string tmp =
                    new_err.getfield("identifier").string_value ();
                  new_error_id = tmp;
                }

              if (! error_state && new_err.contains ("stack"))
                {
                  octave_scalar_map new_err_stack =
                    new_err.getfield("identifier").scalar_map_value ();

                  if (! error_state && new_err_stack.contains ("file"))
                    {
                      const std::string tmp =
                        new_err_stack.getfield("file").string_value ();
                      new_error_file = tmp;
                    }

                  if (! error_state && new_err_stack.contains ("name"))
                    {
                      const std::string tmp =
                        new_err_stack.getfield("name").string_value ();
                      new_error_name = tmp;
                    }

                  if (! error_state && new_err_stack.contains ("line"))
                    {
                      const int tmp =
                        new_err_stack.getfield("line").nint_value ();
                      new_error_line = tmp;
                    }

                  if (! error_state && new_err_stack.contains ("column"))
                    {
                      const int tmp =
                        new_err_stack.getfield("column").nint_value ();
                      new_error_column = tmp;
                    }
                }

              if (! error_state)
                {
                  Vlast_error_message = new_error_message;
                  Vlast_error_id = new_error_id;

                  octave_idx_type curr_frame = -1;

                  Vlast_error_stack
                    = octave_call_stack::backtrace (0, curr_frame);
                }
            }
          else
            error ("lasterror: argument must be a structure or a string");
        }

      if (! error_state)
        retval = err;
    }
  else
    print_usage ();

  return retval;
}

DEFUN (lasterr, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {[@var{msg}, @var{msgid}] =} lasterr (@var{msg}, @var{msgid})\n\
Without any arguments, return the last error message.  With one\n\
argument, set the last error message to @var{msg}.  With two arguments,\n\
also set the last message identifier.\n\
@end deftypefn")
{
  octave_value_list retval;

  unwind_protect frame;

  frame.protect_var (error_state);
  error_state = 0;

  int argc = args.length () + 1;

  if (argc < 4)
    {
      string_vector argv = args.make_argv ("lasterr");

      if (! error_state)
        {
          std::string prev_error_id = Vlast_error_id;
          std::string prev_error_message = Vlast_error_message;

          if (argc > 2)
            Vlast_error_id = argv(2);

          if (argc > 1)
            Vlast_error_message = argv(1);

          if (argc == 1 || nargout > 0)
            {
              retval(1) = prev_error_id;
              retval(0) = prev_error_message;
            }
        }
      else
        error ("lasterr: expecting arguments to be character strings");
    }
  else
    print_usage ();

  return retval;
}

// For backward compatibility.
DEFALIAS (error_text, lasterr);
DEFALIAS (__error_text__, lasterr);

DEFUN (lastwarn, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {[@var{msg}, @var{msgid}] =} lastwarn (@var{msg}, @var{msgid})\n\
Without any arguments, return the last warning message.  With one\n\
argument, set the last warning message to @var{msg}.  With two arguments,\n\
also set the last message identifier.\n\
@end deftypefn")
{
  octave_value_list retval;

  int argc = args.length () + 1;

  if (argc < 4)
    {
      string_vector argv = args.make_argv ("lastwarn");

      if (! error_state)
        {
          std::string prev_warning_id = Vlast_warning_id;
          std::string prev_warning_message = Vlast_warning_message;

          if (argc > 2)
            Vlast_warning_id = argv(2);

          if (argc > 1)
            Vlast_warning_message = argv(1);

          if (argc == 1 || nargout > 0)
            {
              warning_state = 0;
              retval(1) = prev_warning_id;
              retval(0) = prev_warning_message;
            }
        }
      else
        error ("lastwarn: expecting arguments to be character strings");
    }
  else
    print_usage ();

  return retval;
}

DEFUN (usage, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} usage (@var{msg})\n\
Print the message @var{msg}, prefixed by the string @samp{usage: }, and\n\
set Octave's internal error state such that control will return to the\n\
top level without evaluating any more commands.  This is useful for\n\
aborting from functions.\n\
\n\
After @code{usage} is evaluated, Octave will print a traceback of all\n\
the function calls leading to the usage message.\n\
\n\
You should use this function for reporting problems errors that result\n\
from an improper call to a function, such as calling a function with an\n\
incorrect number of arguments, or with arguments of the wrong type.  For\n\
example, most functions distributed with Octave begin with code like\n\
this\n\
\n\
@example\n\
@group\n\
if (nargin != 2)\n\
  usage (\"foo (a, b)\");\n\
endif\n\
@end group\n\
@end example\n\
\n\
@noindent\n\
to check for the proper number of arguments.\n\
@end deftypefn")
{
  octave_value_list retval;
  handle_message (usage_with_id, "", "unknown", args);
  return retval;
}

DEFUN (beep_on_error, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} beep_on_error ()\n\
@deftypefnx {Built-in Function} {@var{old_val} =} beep_on_error (@var{new_val})\n\
Query or set the internal variable that controls whether Octave will try\n\
to ring the terminal bell before printing an error message.\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (beep_on_error);
}

DEFUN (debug_on_error, args, nargout,
    "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} debug_on_error ()\n\
@deftypefnx {Built-in Function} {@var{old_val} =} debug_on_error (@var{new_val})\n\
Query or set the internal variable that controls whether Octave will try\n\
to enter the debugger when an error is encountered.  This will also\n\
inhibit printing of the normal traceback message (you will only see\n\
the top-level error message).\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (debug_on_error);
}

DEFUN (debug_on_warning, args, nargout,
    "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} debug_on_warning ()\n\
@deftypefnx {Built-in Function} {@var{old_val} =} debug_on_warning (@var{new_val})\n\
Query or set the internal variable that controls whether Octave will try\n\
to enter the debugger when a warning is encountered.\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (debug_on_warning);
}

std::string
last_error_message (void)
{
  return Vlast_error_message;
}

std::string
last_error_id (void)
{
  return Vlast_error_id;
}

std::string
last_warning_message (void)
{
  return Vlast_warning_message;
}

std::string
last_warning_id (void)
{
  return Vlast_warning_id;
}

void
interpreter_try (unwind_protect& frame)
{
  frame.protect_var (error_state);
  frame.protect_var (buffer_error_messages);
  frame.protect_var (Vdebug_on_error);
  frame.protect_var (Vdebug_on_warning);

  buffer_error_messages++;
  Vdebug_on_error = false;
  Vdebug_on_warning = false;
}


*/
