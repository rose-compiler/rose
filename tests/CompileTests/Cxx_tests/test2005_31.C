// Original Code:
//    getline(in_stream, line, '\n');

// Generated Code:
//    getline(in_stream,line,10);

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include<iostream.h>

void foo ( istream & in_stream )
   {
     int integerVariable;
     std::string line;

  // Cases to handle '\n', '\t', '\v', '\b', '\r', '\f', '\a', '\\', '\?', '\'', '\"', 
  // Null character: NUL  '\0', 
  // Start of heading character: NUL  '\0', 

  // Char Oct  Dec  Hex	Control-Key	Control Action
  // NUL	  0	 0	   0	^@	Null character
  // SOH	  1	 1	   1	^A	Start of heading, = console interrupt
  // STX	  2	 2	   2	^B	Start of text, maintenance mode on HP console
  // ETX	  3	 3	   3	^C	End of text
  // EOT	  4	 4	   4	^D	End of transmission, not the same as ETB
  // ENQ	  5	 5	   5	^E	Enquiry, goes with ACK; old HP flow control
  // ACK	  6	 6	   6	^F	Acknowledge, clears ENQ logon hand
  // BEL	  7	 7	   7	^G	Bell, rings the bell...
  // BS	 10	 8	   8	^H	Backspace, works on HP terminals/computers
  // HT	 11	 9	   9	^I	Horizontal tab, move to next tab stop
  // LF	 12	10	   a	^J	Line Feed
  // VT	 13	11	   b	^K	Vertical tab
  // FF	 14	12	   c	^L	Form Feed, page eject
  // CR	 15	13	   d	^M	Carriage Return
  // SO	 16	14	   e	^N	Shift Out, alternate character set
  // SI	 17	15	   f	^O	Shift In, resume defaultn character set
  // DLE	 20	16	  10	^P	Data link escape
  // DC1	 21	17	  11	^Q	XON, with XOFF to pause listings; &quot:okay to send".
  // DC2	 22	18	  12	^R	Device control 2, block-mode flow control
  // DC3	 23	19	  13	^S	XOFF, with XON is TERM=18 flow control
  // DC4	 24	20	  14	^T	Device control 4
  // NAK	 25	21	  15	^U	Negative acknowledge
  // SYN	 26	22	  16	^V	Synchronous idle
  // ETB	 27	23	  17	^W	End transmission block, not the same as EOT
  // CAN	 30	24	  18	^X	Cancel line, MPE echoes !!!
  // EM	 31	25	  19	^Y	End of medium, Control-Y interrupt
  // SUB	 32	26	  1a	^Z	Substitute
  // ESC	 33	27	  1b	^[	Escape, next character is not echoed
  // FS	 34	28	  1c	^\	File separator
  // GS	 35	29	  1d	^]	Group separator
  // RS	 36	30	  1e	^^	Record separator, block-mode terminator
  // US	 37	31	  1f	^_	Unit separator

  // DEL	177  127	  7f	Delete (rubout), cross-hatch box

  // When this is unparsed the '\n' is converted into an integer and then wrong function prototype is sought
     std::getline(in_stream, line, '\n');

     char c0  = '\0';
     char c1  = '\1';
     char c2  = '\2';
     char c3  = '\3';
     char c4  = '\4';
     char c5  = '\5';
     char c6  = '\6';
     char c7  = '\7';
     char c8  = '\b';
     char c9  = (char) 127; // ASCI DEL
     char c10 = '\n';
     char c11 = '\t';
     char c12 = '\v';
     char c13 = '\b';
     char c14 = '\r';
     char c15 = '\f';
     char c16 = '\a';
     char c17 = '\\';
     char c18 = '\?';
     char c19 = '\'';
     char c20 = '\"';
     char c21 = '_';
     char c22 = '~';
     char c23 = '"';
     char c24 = '?';
     char c25 = '\'';
     char c26 = '`';
     char c27 = '\\';
     char c28 = 'a';
     char c29 = 'b';

  // Check that '0' is not converted to '48'  (which is a multi-character character constant, not what we want!)
     char c30  = '0';
     char c31  = '1';
     char c32  = '2';
     char c33  = '3';
     char c34  = '4';
     char c35  = '5';
     char c36  = '6';
     char c37  = '7';
     char c38  = '8';
     char c39  = '9';

     char c40  = '-';
     char c41  = '+';
     char c42  = '*';
     char c43  = '/';
     char c44  = '#';
     char c45  = '!';
     char c46  = '@';
     char c47  = '$';
     char c48  = '%';
     char c49  = '^';
     char c50  = '&';
     char c51  = '(';
     char c52  = ')';
     char c53  = '=';
     char c54  = '{';
     char c55  = '}';
     char c56  = '[';
     char c57  = ']';
     char c58  = '|';
     char c59  = ':';
     char c60  = ';';
     char c61  = '<';
     char c62  = '>';
     char c63  = ',';
     char c64  = '.';

  // Note that octal and hexademimal char literals are normalized to there ASCI character or C++ name equivalents.
     char c65  = '\127';
     char c66  = '\x000';

  // Use the variables to avoid lots of compiler warnings!
     cout << integerVariable;
     cout << c0 << c1 << c2 << c3 << c4 << c5 << c6 << c7 << c8 << c9;
     cout << c10 << c11 << c12 << c13 << c14 << c15 << c16 << c17 << c18 << c19;
     cout << c20 << c21 << c22 << c23 << c24 << c25 << c26 << c27 << c28 << c29;
     cout << c30 << c31 << c32 << c33 << c34 << c35 << c36 << c37 << c38 << c39;
     cout << c40 << c41 << c42 << c43 << c44 << c45 << c46 << c47 << c48 << c49;
     cout << c50 << c51 << c52 << c53 << c54 << c55 << c56 << c57 << c58 << c59;
     cout << c60 << c61 << c62 << c63 << c64 << c65 << c66;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

