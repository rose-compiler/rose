/* C++ code produced by gperf version 3.0.4 */
/* Command-line: gperf -t -C -D -G -L C++ -Z octave_kw_hash octave.gperf  */
/* Computed positions: -k'1,5' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "octave.gperf"

/*

Copyright (C) 1995-2011 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

NOTE: gperf 2.7.2 will silently generate bad code if there are blank
lines following the "%{" marker above.  This comment block seems to be
handled correctly.

*/
enum octave_kw_id
{
  break_kw,
  case_kw,
  catch_kw,
  classdef_kw,
  continue_kw,
  do_kw,
  else_kw,
  elseif_kw,
  end_kw,
  end_try_catch_kw,
  end_unwind_protect_kw,
  endclassdef_kw,
  endevents_kw,
  endfor_kw,
  endfunction_kw,
  endif_kw,
  endmethods_kw,
  endproperties_kw,
  endswitch_kw,
  endwhile_kw,
  events_kw,
  for_kw,
  function_kw,
  get_kw,
  global_kw,
  if_kw,
  magic_file_kw,
  magic_line_kw,
  methods_kw,
  otherwise_kw,
  properties_kw,
  return_kw,
  set_kw,
  static_kw,
  switch_kw,
  try_kw,
  until_kw,
  unwind_protect_kw,
  unwind_protect_cleanup_kw,
  while_kw
};
#line 71 "octave.gperf"
struct octave_kw { const char *name; int tok; octave_kw_id kw_id; };

#define TOTAL_KEYWORDS 41
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 22
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 68
/* maximum key range = 67, duplicates = 0 */

class octave_kw_hash
{
private:
  static inline unsigned int hash (const char *str, unsigned int len);
public:
  static const struct octave_kw *in_word_set (const char *str, unsigned int len);
};

inline unsigned int
octave_kw_hash::hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 35, 69, 30, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 25, 69,  0, 15, 10,
       0,  0, 15, 55,  0,  5, 69, 15,  5,  5,
       0,  0, 15, 69, 25, 10, 30, 30, 10,  0,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69, 69, 69, 69, 69,
      69, 69, 69, 69, 69, 69
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

static const struct octave_kw wordlist[] =
  {
#line 78 "octave.gperf"
    {"do", DO, do_kw},
#line 81 "octave.gperf"
    {"end", END, end_kw},
#line 79 "octave.gperf"
    {"else", ELSE, else_kw},
#line 111 "octave.gperf"
    {"while", WHILE, while_kw},
#line 86 "octave.gperf"
    {"endfor", END, endfor_kw},
#line 98 "octave.gperf"
    {"if", IF, if_kw},
#line 92 "octave.gperf"
    {"endwhile", END, endwhile_kw},
#line 91 "octave.gperf"
    {"endswitch", END, endswitch_kw},
#line 89 "octave.gperf"
    {"endmethods", END, endmethods_kw},
#line 80 "octave.gperf"
    {"elseif", ELSEIF, elseif_kw},
#line 99 "octave.gperf"
    {"methods", METHODS, methods_kw},
#line 104 "octave.gperf"
    {"set", SET, set_kw},
#line 74 "octave.gperf"
    {"case", CASE, case_kw},
#line 75 "octave.gperf"
    {"catch", CATCH, catch_kw},
#line 84 "octave.gperf"
    {"endclassdef", END, endclassdef_kw },
#line 94 "octave.gperf"
    {"for", FOR, for_kw},
#line 85 "octave.gperf"
    {"endevents", END, endevents_kw},
#line 88 "octave.gperf"
    {"endif", END, endif_kw},
#line 105 "octave.gperf"
    {"static", STATIC, static_kw},
#line 77 "octave.gperf"
    {"continue", CONTINUE, continue_kw},
#line 102 "octave.gperf"
    {"properties", PROPERTIES, properties_kw},
#line 106 "octave.gperf"
    {"switch", SWITCH, switch_kw},
#line 76 "octave.gperf"
    {"classdef", CLASSDEF, classdef_kw},
#line 101 "octave.gperf"
    {"persistent", STATIC, static_kw},
#line 107 "octave.gperf"
    {"try", TRY, try_kw},
#line 100 "octave.gperf"
    {"otherwise", OTHERWISE, otherwise_kw},
#line 73 "octave.gperf"
    {"break", BREAK, break_kw},
#line 93 "octave.gperf"
    {"events", EVENTS, events_kw},
#line 90 "octave.gperf"
    {"endproperties", END, endproperties_kw},
#line 108 "octave.gperf"
    {"until", UNTIL, until_kw},
#line 87 "octave.gperf"
    {"endfunction", END, endfunction_kw},
#line 82 "octave.gperf"
    {"end_try_catch", END, end_try_catch_kw},
#line 109 "octave.gperf"
    {"unwind_protect", UNWIND, unwind_protect_kw},
#line 83 "octave.gperf"
    {"end_unwind_protect", END, end_unwind_protect_kw},
#line 110 "octave.gperf"
    {"unwind_protect_cleanup", CLEANUP, unwind_protect_cleanup_kw},
#line 95 "octave.gperf"
    {"function", FCN, function_kw},
#line 103 "octave.gperf"
    {"return", FUNC_RET, return_kw},
#line 96 "octave.gperf"
    {"get", GET, get_kw},
#line 97 "octave.gperf"
    {"global", GLOBAL, global_kw},
#line 113 "octave.gperf"
    {"__LINE__", NUM, magic_line_kw},
#line 112 "octave.gperf"
    {"__FILE__", DQ_STRING, magic_file_kw}
  };

static const signed char lookup[] =
  {
    -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
    12, 13, 14, -1, 15, 16, 17, 18, -1, 19, -1, 20, 21, -1,
    22, -1, 23, -1, -1, 24, 25, 26, 27, -1, 28, -1, 29, 30,
    -1, 31, 32, -1, -1, -1, 33, -1, -1, -1, 34, 35, -1, -1,
    36, -1, 37, -1, -1, 38, -1, 39, -1, -1, -1, -1, 40
  };

const struct octave_kw *
octave_kw_hash::in_word_set (register const char *str, register unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
        }
    }
  return 0;
}
