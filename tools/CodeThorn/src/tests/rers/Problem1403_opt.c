// .--------------------------------------------------------.
// | printf in functions replaced by global output variable |
// *--------------------------------------------------------*
int input;
#include <stdio.h> 
#include <assert.h>
#include <math.h>
#include <stdlib.h>
//edited by script: global output variable replacing printf in functions 
int output;
int a68 = 12;
int a118 = 1;
int cf = 1;
int a198 = - 3;
int a94 = 11;
int a71 = 7;
int a42 = 251;
int a171_0 = 53;
int a171_1 = 54;
int a171_2 = 55;
int a171_3 = 56;
int a171_5 = 58;
int a35 = 12;
int a74 = 68;
int a188 = 296;
int a173 = 10;
int a39_0 = 19;
int a39_1 = 20;
int a39_2 = 21;
int a39_3 = 22;
int a39_4 = 23;
int a39_5 = 24;
int a124_0 = 31;
int a124_1 = 32;
int a124_2 = 33;
int a124_3 = 34;
int a124_4 = 35;
int a124_5 = 36;
int a15_0 = 25;
int a15_1 = 26;
int a15_2 = 27;
int a15_3 = 28;
int a15_4 = 29;
int a15_5 = 30;
int a18_0 = 31;
int a18_1 = 32;
int a18_2 = 33;
int a18_3 = 34;
int a18_4 = 35;
int a18_5 = 36;
int a101_0 = 37;
int a101_1 = 38;
int a101_2 = 39;
int a101_3 = 40;
int a101_4 = 41;
int a101_5 = 42;
int a53 = 4;
int a107_0 = 9;
int a107_5 = 14;
int a107_7 = 16;
int a62 = 6;
int a187 = 4;
int a105 = - 193;
int a165_0 = 8;
int a165_2 = 10;
int a165_3 = 11;
int a165_4 = 12;
int a165_5 = 13;
int a165_6 = 14;
int a165_7 = 15;
int a80 = 216;
int a148_1 = 65;
int a148_2 = 66;
int a148_3 = 67;
int a148_4 = 68;
int a148_5 = 69;
int a127_0 = 104;
int a127_1 = 105;
int a127_4 = 108;
int a127_5 = 109;
int a49 = 379;
int a8 = 6;
int a167 = 8;
int a119 = 14;
int a182_0 = 100;
int a182_1 = 101;
int a182_2 = 102;
int a182_3 = 103;
int a182_4 = 104;
int a182_5 = 105;
int a125_0 = 88;
int a125_1 = 89;
int a125_2 = 90;
int a125_3 = 91;
int a125_4 = 92;
int a125_5 = 93;
int a193_6 = 11;
int a197_0 = 87;
int a197_1 = 88;
int a197_2 = 89;
int a197_3 = 90;
int a197_4 = 91;
int a197_5 = 92;
int a6_0 = 99;
int a6_1 = 100;
int a6_2 = 101;
int a6_3 = 102;
int a6_4 = 103;
int a6_5 = 104;
int a183_0 = 99;
int a183_1 = 100;
int a183_2 = 101;
int a183_3 = 102;
int a183_4 = 103;
int a183_5 = 104;
int a191_0 = 7;
int a191_4 = 11;
int a191_6 = 13;
int a191_7 = 14;
int a67 = 9;
int a109_2 = 3;
int a109_3 = 4;
int a109_5 = 6;
int a109_7 = 8;
int a37 = 266;
int a116 = 6;
int a73_0 = 6;
int a73_1 = 7;
int a73_3 = 9;
int a73_4 = 10;
int a73_6 = 12;
int a24_0 = 27;
int a24_1 = 28;
int a24_2 = 29;
int a24_3 = 30;
int a24_4 = 31;
int a24_5 = 32;
int a89 = 13;
int a69_0 = 0;
int a69_1 = 1;
int a69_2 = 2;
int a69_5 = 5;
int a81_3 = 9;
int a81_5 = 11;
int a31 = 7;
int a154_0 = 57;
int a154_1 = 58;
int a154_2 = 59;
int a154_3 = 60;
int a154_4 = 61;
int a154_5 = 62;
int a158 = 9;
int a45_5 = 12;
int a111 = 56;
int a104_1 = 7;
int a104_2 = 8;
int a104_3 = 9;
int a104_4 = 10;
int a104_5 = 11;
int a104_7 = 13;
int a122 = 6;
int a169_0 = 98;
int a169_1 = 99;
int a169_2 = 100;
int a169_3 = 101;
int a169_4 = 102;
int a169_5 = 103;
int a157_0 = 104;
int a157_1 = 105;
int a157_2 = 106;
int a157_3 = 107;
int a157_4 = 108;
int a157_5 = 109;
int a91_0 = 110;
int a91_1 = 111;
int a91_2 = 112;
int a91_3 = 113;
int a91_4 = 114;
int a91_5 = 115;
int a52_0 = 98;
int a52_1 = 99;
int a52_2 = 100;
int a52_3 = 101;
int a52_4 = 102;
int a52_5 = 103;
int a139_0 = 7;
int a139_2 = 9;
int a139_4 = 11;
int a139_5 = 12;
int a139_6 = 13;
int a139_7 = 14;
int a86 = 13;
int a1 = 8;
int a137_0 = 7;
int a137_1 = 8;
int a137_3 = 10;
int a137_5 = 12;
int a137_6 = 13;
int a137_7 = 14;
int a26 = 15;
int a194 = 9;
int a50 = 13;
int a176_1 = 8;
int a176_3 = 10;
int a176_6 = 13;

int main()
{
//srand((unsigned)time(NULL));
// main i/o-loop
  while(1){
//edited by script: maybe no output, reset output var 
    output = - 1;
// read input
    scanf("%d",&input);
{
      cf = 1;
      if (a37 <= 35 && cf == 1) {
        if (cf == 1 && a188 <= - 79) {{
            if (cf == 1 && (88 < a80 && 150 >= a80)) {
              if (a118 == 6 && cf == 1) {{
                  if (cf == 1 && input == 2) {
                    cf = 0;
                    a8 = 2;
                    a94 = 11;
                    a52_0 = 104;
                    a52_1 = 105;
                    a52_2 = 106;
                    a52_3 = 107;
                    a52_4 = 108;
                    a52_5 = 109;
                    a37 = (a37 * a80 % 14999 + - 12383 - 1204) % 24 + 222;
                    output = 21;
                  }
                }
              }
              if (a118 == 7 && cf == 1) {{
                  if (input == 4 && cf == 1) {
                    cf = 0;
                    a116 = 8;
                    a194 = 6;
                    a105 = (a105 * a188 % 14999 % 14900 - - 15098) * 1 - - 1;
                    a37 = a37 * a80 % 14999 % 81 - - 117 - - 1 - - 3600 + - 3600;
                    output = 20;
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && (- 79 < a188 && 111 >= a188)) {{
            if (cf == 1 && 33 == a101_2) {
              if (cf == 1 && a122 == 2) {{
                  if (input == 1 && cf == 1) {
                    cf = 0;
                    a15_0 = 31;
                    a15_1 = 32;
                    a15_2 = 33;
                    a15_3 = 34;
                    a15_4 = 35;
                    a15_5 = 36;
                    a71 = 11;
                    a37 = (a37 * a188 % 14999 - - 1833) % 14876 - - 15123 + 1;
                    output = 22;
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && (111 < a188 && 285 >= a188)) {{
            if (a53 == 5 && cf == 1) {
              if (a31 == 13 && cf == 1) {{
                  if (cf == 1 && input == 4) {
                    cf = 0;
                    output = 19;
                  }
                  if (cf == 1 && input == 5) {
                    cf = 0;
                    a194 = 10;
                    a167 = 11;
                    a105 = (a105 * a37 % 14999 + 10695 - 21488) * 1 % 14906 - 15093;
                    a37 = a37 * a188 % 14999 % 81 + 116 - 0 - 0;
                    output = 26;
                  }
                  if (cf == 1 && input == 1) {
                    cf = 0;
                    output = 19;
                  }
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    a8 = 2;
                    a94 = 11;
                    a52_0 = 104;
                    a52_1 = 105;
                    a52_2 = 106;
                    a52_3 = 107;
                    a52_4 = 108;
                    a52_5 = 109;
                    a37 = ((a37 * a37 % 14999 % 24 - - 223) * 5 - - 10855) % 24 + 209;
                    output = 19;
                  }
                  if (input == 3 && cf == 1) {
                    cf = 0;
                    a158 = 10;
                    a94 = 15;
                    a52_0 = 104;
                    a52_1 = 105;
                    a52_2 = 106;
                    a52_3 = 107;
                    a52_4 = 108;
                    a52_5 = 109;
                    a37 = (a37 * a37 % 14999 - - 1772) / 5 % 24 + 223;
                    output = 21;
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && 285 < a188) {{
            if (89 == a183_2 && cf == 1) {
              if (cf == 1 && a89 == 9) {{
                  if (cf == 1 && input == 4) {
                    cf = 0;
                    a8 = 4;
                    a42 = (a42 * a188 % 14999 - - 3608) % 14830 - - 15169 - - 1;
                    a71 = 7;
                    a37 = ((a37 * a188 % 14999 % 14876 - - 15123) / 5 - 16432) * - 1 / 10;
                    output = 23;
                  }
                }
              }
            }
            if (cf == 1 && 100 == a183_1) {
              if (a86 == 9 && cf == 1) {{
                  if (cf == 1 && input == 1) {
                    cf = 0;
                    a62 = 5;
                    a80 = (a80 * a37 % 14999 + 189 + 12853) % 40 - - 192;
                    a52_0 = 98;
                    a52_1 = 99;
                    a52_2 = 100;
                    a52_3 = 101;
                    a52_4 = 102;
                    a52_5 = 103;
                    a37 = (a37 * a37 % 14999 % 24 + 223 - 1) * 1;
                    output = 19;
                  }
                  if (cf == 1 && input == 5) {
                    cf = 0;
                    a173 = 14;
                    a74 = (a74 * a188 % 14999 % 80 - - 224) / 5 / 5 + 274;
                    a71 = 14;
                    a37 = (a37 * a37 % 14999 % 14876 - - 15123) / 5 / 5 + 23499;
                    output = 22;
                  }
                  if (input == 4 && cf == 1) {
                    cf = 0;
                    output = 23;
                  }
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    output = 23;
                  }
                }
              }
            }
          }
        }
      }
      if (cf == 1 && (35 < a37 && 198 >= a37)) {
        if (cf == 1 && a105 <= - 188) {{
            if (a167 == 6 && cf == 1) {
              if (cf == 1 && a35 == 8) {{
                  if (input == 1 && cf == 1) {
                    cf = 0;
                    a118 = 7;
                    a80 = ((a80 * a105 % 14999 % 30 - - 119) * 5 - 14866) % 30 - - 135;
                    a188 = a188 * a105 % 14999 % 14960 + - 15038 + - 3 - - 10502 + - 10499;
                    a37 = (a37 * a105 % 14999 + - 3591 + - 1641) * 1;
                    output = 26;
                  }
                  if (cf == 1 && input == 4) {
                    cf = 0;
                    a122 = 2;
                    a101_0 = 31;
                    a101_1 = 32;
                    a101_2 = 33;
                    a101_3 = 34;
                    a101_4 = 35;
                    a101_5 = 36;
                    a188 = (a188 * a105 % 14999 - 3816 + - 10670) % 94 + 15;
                    a37 = (a37 * a105 % 14999 - - 2989 - 6080) * 1;
                    output = 21;
                  }
                }
              }
            }
            if (cf == 1 && a167 == 7) {
              if (cf == 1 && 59 == 59) {{
                  if (input == 3 && cf == 1) {
                    cf = 0;
                    a116 = 5;
                    a194 = 6;
                    a105 = (a105 * a105 % 14999 - - 2899 - - 6116) * 1;
                    output = 23;
                  }
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    output = 24;
                  }
                  if (input == 4 && cf == 1) {
                    cf = 0;
                    output = 24;
                  }
                  if (cf == 1 && input == 5) {
                    cf = 0;
                    a67 = 8;
                    a167 = 8;
                    output = 26;
                  }
                  if (cf == 1 && input == 1) {
                    cf = 0;
                    output = 24;
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && (- 188 < a105 && 5 >= a105)) {{
            if (cf == 1 && a49 <= 153) {
              if (a42 <= 71 && cf == 1) {{
                  if (input == 4 && cf == 1) {
                    cf = 0;
                    a31 = 13;
                    a53 = 5;
                    a188 = a188 * a37 % 14999 * 2 % 86 - - 197 + - 17826 - - 17826;
                    a37 = a37 * a105 % 14999 * 2 % 15017 - 14981 + - 2;
                    output = 19;
                  }
                }
              }
            }
            if (cf == 1 && (372 < a49 && 445 >= a49)) {
              if (a118 == 4 && cf == 1) {{
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    a62 = 7;
                    a80 = (a80 * a105 % 14999 + - 8776) / 5 % 40 + 191;
                    a52_0 = 98;
                    a52_1 = 99;
                    a52_2 = 100;
                    a52_3 = 101;
                    a52_4 = 102;
                    a52_5 = 103;
                    a37 = (a37 * a37 % 14999 + 3000) % 24 - - 215 + - 872 - - 871;
                    output = 23;
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && (5 < a105 && 198 >= a105)) {{
            if (70 < a74 && 142 >= a74 && cf == 1) {
              if (cf == 1 && a187 == 8) {{
                  if (input == 3 && cf == 1) {
                    cf = 0;
                    a173 = 9;
                    a1 = 6;
                    a71 = 13;
                    a37 = (a37 * a74 + 1568) * 1 + 268;
                    output = 26;
                  }
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    a122 = 5;
                    a35 = 8;
                    a52_0 = 110;
                    a52_1 = 111;
                    a52_2 = 112;
                    a52_3 = 113;
                    a52_4 = 114;
                    a52_5 = 115;
                    a37 = (a37 * a105 % 14999 - - 6360) % 24 + 210 - - 29013 - 29022;
                    output = 26;
                  }
                  if (cf == 1 && input == 4) {
                    cf = 0;
                    a8 = 2;
                    a94 = 11;
                    a52_0 = 104;
                    a52_1 = 105;
                    a52_2 = 106;
                    a52_3 = 107;
                    a52_4 = 108;
                    a52_5 = 109;
                    a37 = (a37 * a74 + - 2932) * 1 % 24 - - 222;
                    output = 26;
                  }
                  if (cf == 1 && input == 1) {
                    cf = 0;
                    a111 = (a111 * a105 % 14999 % 14852 - - 15146) / 5 + 16346;
                    a167 = 13;
                    a105 = (a105 * a74 - 30059) * 1 - - 8433 + - 7675;
                    output = 19;
                  }
                  if (cf == 1 && input == 5) {
                    cf = 0;
                    a198 = (a198 * a105 % 14999 + - 7209 - 713) * 1 % 79 - - 8;
                    a15_0 = 19;
                    a15_1 = 20;
                    a15_2 = 21;
                    a15_3 = 22;
                    a15_4 = 23;
                    a15_5 = 24;
                    a71 = 11;
                    a37 = (a37 * a105 % 14999 + 7260) * 1 * 1;
                    output = 19;
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && 198 < a105) {{
            if (cf == 1 && a194 == 6) {
              if (a116 == 8 && cf == 1) {{
                  if (cf == 1 && input == 3) {
                    cf = 0;
                    a26 = 9;
                    a94 = 13;
                    a52_0 = 104;
                    a52_1 = 105;
                    a52_2 = 106;
                    a52_3 = 107;
                    a52_4 = 108;
                    a52_5 = 109;
                    a37 = (a37 * a37 % 14999 - - 4583 + - 16089 + 22684) % 24 + 212;
                    output = 23;
                  }
                  if (cf == 1 && input == 1) {
                    cf = 0;
                    a118 = 7;
                    a80 = (a80 * a37 % 14999 % 30 - - 118) * 1 + 0;
                    a188 = (a188 * a105 % 14999 % 14960 + - 15038) / 5 - 1714;
                    a37 = (a37 * a37 % 14999 + - 25480) * 1 - 234;
                    output = 26;
                  }
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    a35 = 8;
                    a167 = 6;
                    a105 = (a105 * a105 % 14999 + - 20624) * 1 * 10 / 9;
                    output = 22;
                  }
                  if (cf == 1 && input == 4) {
                    cf = 0;
                    a125_0 = 100;
                    a125_1 = 101;
                    a125_2 = 102;
                    a125_3 = 103;
                    a125_4 = 104;
                    a125_5 = 105;
                    a74 = (a74 * a105 % 14999 - 13058 + 5534) % 35 + 106;
                    a71 = 14;
                    a37 = (a37 * a37 % 14999 - - 13014 - - 893) / 5;
                    output = 20;
                  }
                  if (cf == 1 && input == 5) {
                    cf = 0;
                    a122 = 5;
                    a101_0 = 31;
                    a101_1 = 32;
                    a101_2 = 33;
                    a101_3 = 34;
                    a101_4 = 35;
                    a101_5 = 36;
                    a188 = (a188 * a105 % 14999 + - 13683) % 94 + 17 + 5592 + - 5593;
                    a37 = (a37 * a105 % 14999 / 5 - - 12593) / - 5;
                    output = 26;
                  }
                }
              }
            }
          }
        }
      }
      if (cf == 1 && (198 < a37 && 247 >= a37)) {
        if (cf == 1 && 98 == a52_0) {{
            if (cf == 1 && a80 <= 88) {
              if (a119 == 7 && cf == 1) {{
                  if (input == 4 && cf == 1) {
                    cf = 0;
                    a122 = 5;
                    a35 = 8;
                    a52_0 = 110;
                    a52_1 = 111;
                    a52_2 = 112;
                    a52_3 = 113;
                    a52_4 = 114;
                    a52_5 = 115;
                    output = 26;
                  }
                }
              }
            }
            if (150 < a80 && 232 >= a80 && cf == 1) {
              if (a62 == 7 && cf == 1) {{
                  if (cf == 1 && input == 2) {
                    cf = 0;
                    a86 = 9;
                    a183_0 = 99;
                    a183_1 = 100;
                    a183_2 = 101;
                    a183_3 = 102;
                    a183_4 = 103;
                    a183_5 = 104;
                    a188 = a188 * a80 % 14999 / 5 + 25434 - 27546 - - 6590;
                    a37 = (a37 * a80 % 14999 + - 2722 + 2736) * 1 + - 17395;
                    output = 23;
                  }
                }
              }
            }
          }
        }
        if (108 == a52_4 && cf == 1) {{
            if (a94 == 11 && cf == 1) {
              if (a8 == 2 && cf == 1) {{
                  if (input == 1 && cf == 1) {
                    cf = 0;
                    output = 21;
                  }
                  if (input == 5 && cf == 1) {
                    cf = 0;
                    a111 = a111 * a37 % 14999 % 14953 - 15046 - - 9546 + - 9546;
                    a167 = 13;
                    a105 = (a105 * a37 % 14999 * 2 + - 2 + 3) % 14906 + - 15093;
                    a37 = (a37 * a37 % 14999 % 81 + 66) * 9 / 10 * 5 % 81 + 78;
                    output = 24;
                  }
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    output = 21;
                  }
                  if (cf == 1 && input == 3) {
                    cf = 0;
                    a158 = 5;
                    a94 = 15;
                    output = 23;
                  }
                  if (cf == 1 && input == 4) {
                    cf = 0;
                    output = 21;
                  }
                }
              }
            }
          }
        }
        if (115 == a52_5 && cf == 1) {{
            if (a35 == 6 && cf == 1) {
              if (cf == 1 && a50 == 9) {{
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    a187 = 8;
                    a74 = (a74 * a37 % 14999 / 5 - 21879) % 35 + 116;
                    a105 = (a105 * a37 % 14999 + 620) % 96 + 101 + 0;
                    a37 = (a37 * a37 % 14999 % 81 - - 63) * 10 / 9 * 9 / 10;
                    output = 26;
                  }
                }
              }
            }
            if (a35 == 8 && cf == 1) {
              if (a122 == 5 && cf == 1) {{
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    a50 = 9;
                    a35 = 6;
                    output = 23;
                  }
                }
              }
            }
          }
        }
      }
      if (cf == 1 && 247 < a37) {
        if (a71 == 7 && cf == 1) {{
            if (169 < a42 && 339 >= a42 && cf == 1) {
              if (cf == 1 && a68 == 12) {{
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    a119 = 7;
                    a80 = a80 * a37 % 14999 - 14972 - 28 - 1;
                    a52_0 = 98;
                    a52_1 = 99;
                    a52_2 = 100;
                    a52_3 = 101;
                    a52_4 = 102;
                    a52_5 = 103;
                    a37 = (a37 * a42 % 14999 - - 6640) * 1 % 24 + 212;
                    output = 19;
                  }
                  if (input == 1 && cf == 1) {
                    cf = 0;
                    a89 = 9;
                    a183_0 = 87;
                    a183_1 = 88;
                    a183_2 = 89;
                    a183_3 = 90;
                    a183_4 = 91;
                    a183_5 = 92;
                    a188 = a188 * a42 % 14999 / 5 - - 15466 - - 10297;
                    a37 = (a37 * a42 % 14999 + - 1633) / 5 * 5 - 22904;
                    output = 21;
                  }
                  if (cf == 1 && input == 4) {
                    cf = 0;
                    a125_0 = 100;
                    a125_1 = 101;
                    a125_2 = 102;
                    a125_3 = 103;
                    a125_4 = 104;
                    a125_5 = 105;
                    a74 = (a74 * a37 % 14999 / 5 + 17394 + 6778) % 35 - - 74;
                    a71 = 14;
                    output = 22;
                  }
                }
              }
            }
            if (339 < a42 && cf == 1) {
              if (a8 == 4 && cf == 1) {{
                  if (cf == 1 && input == 2) {
                    cf = 0;
                    a42 = (a42 * a42 % 14999 + - 8520 + - 12295) * 1;
                    a49 = (a49 * a37 % 14999 + - 15001 + - 1) * 1;
                    a105 = a105 * a37 % 14999 % 96 + - 91 + 1 - 1;
                    a37 = (a37 * a37 % 14999 / 5 - - 14015) % 81 + 60;
                    output = 24;
                  }
                  if (cf == 1 && input == 1) {
                    cf = 0;
                    a119 = 11;
                    a68 = 13;
                    a71 = 10;
                    output = 25;
                  }
                  if (input == 3 && cf == 1) {
                    cf = 0;
                    a118 = 6;
                    a80 = a80 * a37 % 14999 % 30 + 119 + - 1 + 3;
                    a188 = (a188 * a37 % 14999 - - 488) / 5 / 5 - 24569;
                    a37 = a37 * a37 % 14999 - 19576 - 4282 + - 3073;
                    output = 26;
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a71 == 10) {{
            if (cf == 1 && a68 == 13) {
              if (a119 == 11 && cf == 1) {{
                  if (input == 1 && cf == 1) {
                    cf = 0;
                    a167 = 7;
                    a105 = (a105 * a37 % 14999 % 14906 - 15093) * 1 + - 1;
                    a37 = a37 * a37 % 14999 % 81 - - 91 - 24 + - 2685 + 2708;
                    output = 24;
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a71 == 11) {{
            if (cf == 1 && 32 == a15_1) {
              if (10 == 10 && cf == 1) {{
                  if (cf == 1 && input == 4) {
                    cf = 0;
                    a119 = 9;
                    a80 = (a80 * a37 % 14999 + - 14929) / 5 - 10615;
                    a52_0 = 98;
                    a52_1 = 99;
                    a52_2 = 100;
                    a52_3 = 101;
                    a52_4 = 102;
                    a52_5 = 103;
                    a37 = a37 * a37 % 14999 % 24 + 210 + 999 + 10185 + - 11183;
                    output = 26;
                  }
                  if (input == 3 && cf == 1) {
                    cf = 0;
                    a80 = (a80 * a37 % 14999 * 2 - 0) % 14883 + 15115;
                    a188 = (a188 * a37 % 14999 % 14960 - 15038 + - 1) * 1;
                    a37 = (a37 * a37 % 14999 / 5 - 9877) * 3;
                    output = 24;
                  }
                  if (input == 2 && cf == 1) {
                    cf = 0;
                    output = 22;
                  }
                  if (input == 1 && cf == 1) {
                    cf = 0;
                    output = 22;
                  }
                  if (cf == 1 && input == 5) {
                    cf = 0;
                    a86 = 10;
                    a183_0 = 99;
                    a183_1 = 100;
                    a183_2 = 101;
                    a183_3 = 102;
                    a183_4 = 103;
                    a183_5 = 104;
                    a188 = (a188 * a37 % 14999 + - 5931 - 6065) % 14857 + 15142;
                    a37 = (a37 * a37 % 14999 - - 2983 - 31207) * 1;
                    output = 24;
                  }
                }
              }
            }
          }
        }
        if (a71 == 14 && cf == 1) {{
            if (70 < a74 && 142 >= a74 && cf == 1) {
              if (102 == a125_2 && cf == 1) {{
                  if (input == 4 && cf == 1) {
                    cf = 0;
                    a35 = 8;
                    a167 = 6;
                    a105 = a105 * a74 % 14999 * 2 % 14906 - 15093 + - 2;
                    a37 = (a37 * a74 % 14999 / 5 + - 14205) % 81 + 167;
                    output = 22;
                  }
                  if (cf == 1 && input == 2) {
                    cf = 0;
                    a118 = 4;
                    a49 = a49 * a37 % 14999 * 2 / 5 % 36 + 409;
                    a105 = a105 * a37 % 14999 % 96 + - 91 - - 1 + - 1;
                    a37 = (a37 * a74 % 14999 / 5 / 5 - - 2265) % 81 + 110;
                    output = 26;
                  }
                }
              }
            }
          }
        }
      }
{
        if (8 == 6 && 304 < a74 && (5 < a105 && 198 >= a105) && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_0:
          !1?((void )0) : __assert_fail("!error_0","Problem1403_mod_global_in_out.c",230,__PRETTY_FUNCTION__);
        }
        if (8 == 3 && a194 == 10 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_1:
          !1?((void )0) : __assert_fail("!error_1","Problem1403_mod_global_in_out.c",234,__PRETTY_FUNCTION__);
        }
        if (a67 == 8 && a167 == 8 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_2:
          !1?((void )0) : __assert_fail("!error_2","Problem1403_mod_global_in_out.c",238,__PRETTY_FUNCTION__);
        }
        if (a62 == 1 && (150 < a80 && 232 >= a80) && 98 == a52_0 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_3:
          !1?((void )0) : __assert_fail("!error_3","Problem1403_mod_global_in_out.c",242,__PRETTY_FUNCTION__);
        }
        if (11 == 7 && 41 == a101_4 && (- 79 < a188 && 111 >= a188) && a37 <= 35) {
          cf = 0;
          error_4:
          !1?((void )0) : __assert_fail("!error_4","Problem1403_mod_global_in_out.c",246,__PRETTY_FUNCTION__);
        }
        if (a89 == 15 && 89 == a183_2 && 285 < a188 && a37 <= 35) {
          cf = 0;
          error_5:
          !1?((void )0) : __assert_fail("!error_5","Problem1403_mod_global_in_out.c",250,__PRETTY_FUNCTION__);
        }
        if (a187 == 4 && (70 < a74 && 142 >= a74) && (5 < a105 && 198 >= a105) && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_6:
          !1?((void )0) : __assert_fail("!error_6","Problem1403_mod_global_in_out.c",254,__PRETTY_FUNCTION__);
        }
        if (25 == a101_0 && a74 <= 70 && (5 < a105 && 198 >= a105) && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_7:
          !1?((void )0) : __assert_fail("!error_7","Problem1403_mod_global_in_out.c",258,__PRETTY_FUNCTION__);
        }
        if (a119 == 12 && a68 == 13 && a71 == 10 && 247 < a37) {
          cf = 0;
          error_8:
          !1?((void )0) : __assert_fail("!error_8","Problem1403_mod_global_in_out.c",262,__PRETTY_FUNCTION__);
        }
        if (a194 == 6 && a167 == 11 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_9:
          !1?((void )0) : __assert_fail("!error_9","Problem1403_mod_global_in_out.c",266,__PRETTY_FUNCTION__);
        }
        if (a94 == 9 && 9 == 7 && a71 == 12 && 247 < a37) {
          cf = 0;
          error_10:
          !1?((void )0) : __assert_fail("!error_10","Problem1403_mod_global_in_out.c",270,__PRETTY_FUNCTION__);
        }
        if (108 == a52_4 && a68 == 15 && a71 == 10 && 247 < a37) {
          cf = 0;
          error_11:
          !1?((void )0) : __assert_fail("!error_11","Problem1403_mod_global_in_out.c",274,__PRETTY_FUNCTION__);
        }
        if (a194 == 9 && a167 == 11 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_12:
          !1?((void )0) : __assert_fail("!error_12","Problem1403_mod_global_in_out.c",278,__PRETTY_FUNCTION__);
        }
        if (a68 == 10 && (169 < a42 && 339 >= a42) && a71 == 7 && 247 < a37) {
          cf = 0;
          error_13:
          !1?((void )0) : __assert_fail("!error_13","Problem1403_mod_global_in_out.c",282,__PRETTY_FUNCTION__);
        }
        if (11 == 13 && 41 == a101_4 && (- 79 < a188 && 111 >= a188) && a37 <= 35) {
          cf = 0;
          error_14:
          !1?((void )0) : __assert_fail("!error_14","Problem1403_mod_global_in_out.c",286,__PRETTY_FUNCTION__);
        }
        if (a68 == 13 && 9 == 11 && a71 == 12 && 247 < a37) {
          cf = 0;
          error_15:
          !1?((void )0) : __assert_fail("!error_15","Problem1403_mod_global_in_out.c",290,__PRETTY_FUNCTION__);
        }
        if (a158 == 11 && a94 == 15 && 108 == a52_4 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_16:
          !1?((void )0) : __assert_fail("!error_16","Problem1403_mod_global_in_out.c",294,__PRETTY_FUNCTION__);
        }
        if (a1 == 6 && (142 < a74 && 304 >= a74) && (5 < a105 && 198 >= a105) && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_17:
          !1?((void )0) : __assert_fail("!error_17","Problem1403_mod_global_in_out.c",298,__PRETTY_FUNCTION__);
        }
        if (2 == 2 && 10 == 9 && a71 == 9 && 247 < a37) {
          cf = 0;
          error_18:
          !1?((void )0) : __assert_fail("!error_18","Problem1403_mod_global_in_out.c",302,__PRETTY_FUNCTION__);
        }
        if (a86 == 12 && 100 == a183_1 && 285 < a188 && a37 <= 35) {
          cf = 0;
          error_19:
          !1?((void )0) : __assert_fail("!error_19","Problem1403_mod_global_in_out.c",306,__PRETTY_FUNCTION__);
        }
        if (a86 == 8 && 100 == a183_1 && 285 < a188 && a37 <= 35) {
          cf = 0;
          error_20:
          !1?((void )0) : __assert_fail("!error_20","Problem1403_mod_global_in_out.c",310,__PRETTY_FUNCTION__);
        }
        if (28 == 28 && 232 < a80 && a188 <= - 79 && a37 <= 35) {
          cf = 0;
          error_21:
          !1?((void )0) : __assert_fail("!error_21","Problem1403_mod_global_in_out.c",314,__PRETTY_FUNCTION__);
        }
        if (45 == 57 && a1 == 8 && a71 == 13 && 247 < a37) {
          cf = 0;
          error_22:
          !1?((void )0) : __assert_fail("!error_22","Problem1403_mod_global_in_out.c",318,__PRETTY_FUNCTION__);
        }
        if (22 == 28 && a194 == 8 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_23:
          !1?((void )0) : __assert_fail("!error_23","Problem1403_mod_global_in_out.c",322,__PRETTY_FUNCTION__);
        }
        if (9 == 7 && (88 < a80 && 150 >= a80) && 98 == a52_0 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_24:
          !1?((void )0) : __assert_fail("!error_24","Problem1403_mod_global_in_out.c",326,__PRETTY_FUNCTION__);
        }
        if (a74 <= 70 && (71 < a42 && 169 >= a42) && a71 == 7 && 247 < a37) {
          cf = 0;
          error_25:
          !1?((void )0) : __assert_fail("!error_25","Problem1403_mod_global_in_out.c",330,__PRETTY_FUNCTION__);
        }
        if (a122 == 5 && 33 == a101_2 && (- 79 < a188 && 111 >= a188) && a37 <= 35) {
          cf = 0;
          error_26:
          !1?((void )0) : __assert_fail("!error_26","Problem1403_mod_global_in_out.c",334,__PRETTY_FUNCTION__);
        }
        if (14 == 10 && 101 == 107 && a71 == 8 && 247 < a37) {
          cf = 0;
          error_27:
          !1?((void )0) : __assert_fail("!error_27","Problem1403_mod_global_in_out.c",338,__PRETTY_FUNCTION__);
        }
        if (- 9 < -5 && 62 >= -5 && a1 == 10 && a71 == 13 && 247 < a37) {
          cf = 0;
          error_28:
          !1?((void )0) : __assert_fail("!error_28","Problem1403_mod_global_in_out.c",342,__PRETTY_FUNCTION__);
        }
        if (9 == 13 && (88 < a80 && 150 >= a80) && 98 == a52_0 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_29:
          !1?((void )0) : __assert_fail("!error_29","Problem1403_mod_global_in_out.c",346,__PRETTY_FUNCTION__);
        }
        if (64 == 64 && a194 == 7 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_30:
          !1?((void )0) : __assert_fail("!error_30","Problem1403_mod_global_in_out.c",350,__PRETTY_FUNCTION__);
        }
        if (a8 == 2 && 339 < a42 && a71 == 7 && 247 < a37) {
          cf = 0;
          error_31:
          !1?((void )0) : __assert_fail("!error_31","Problem1403_mod_global_in_out.c",354,__PRETTY_FUNCTION__);
        }
        if (a68 == 15 && 9 == 11 && a71 == 12 && 247 < a37) {
          cf = 0;
          error_32:
          !1?((void )0) : __assert_fail("!error_32","Problem1403_mod_global_in_out.c",358,__PRETTY_FUNCTION__);
        }
        if (383 < 64 && a167 == 12 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_33:
          !1?((void )0) : __assert_fail("!error_33","Problem1403_mod_global_in_out.c",362,__PRETTY_FUNCTION__);
        }
        if (a94 == 8 && 9 == 7 && a71 == 12 && 247 < a37) {
          cf = 0;
          error_34:
          !1?((void )0) : __assert_fail("!error_34","Problem1403_mod_global_in_out.c",366,__PRETTY_FUNCTION__);
        }
        if (a194 == 10 && a167 == 11 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_35:
          !1?((void )0) : __assert_fail("!error_35","Problem1403_mod_global_in_out.c",370,__PRETTY_FUNCTION__);
        }
        if (89 < 98 && a94 == 10 && 108 == a52_4 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_36:
          !1?((void )0) : __assert_fail("!error_36","Problem1403_mod_global_in_out.c",374,__PRETTY_FUNCTION__);
        }
        if (a86 == 10 && 100 == a183_1 && 285 < a188 && a37 <= 35) {
          cf = 0;
          error_37:
          !1?((void )0) : __assert_fail("!error_37","Problem1403_mod_global_in_out.c",378,__PRETTY_FUNCTION__);
        }
        if (8 == 6 && a194 == 10 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_38:
          !1?((void )0) : __assert_fail("!error_38","Problem1403_mod_global_in_out.c",382,__PRETTY_FUNCTION__);
        }
        if (a173 == 12 && a68 == 11 && a71 == 10 && 247 < a37) {
          cf = 0;
          error_39:
          !1?((void )0) : __assert_fail("!error_39","Problem1403_mod_global_in_out.c",386,__PRETTY_FUNCTION__);
        }
        if (a50 == 11 && a35 == 12 && 115 == a52_5 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_40:
          !1?((void )0) : __assert_fail("!error_40","Problem1403_mod_global_in_out.c",390,__PRETTY_FUNCTION__);
        }
        if (13 == 11 && a194 == 12 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_41:
          !1?((void )0) : __assert_fail("!error_41","Problem1403_mod_global_in_out.c",394,__PRETTY_FUNCTION__);
        }
        if (a173 == 10 && a35 == 10 && 115 == a52_5 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_42:
          !1?((void )0) : __assert_fail("!error_42","Problem1403_mod_global_in_out.c",398,__PRETTY_FUNCTION__);
        }
        if (0 == 1 && a53 == 2 && (111 < a188 && 285 >= a188) && a37 <= 35) {
          cf = 0;
          error_43:
          !1?((void )0) : __assert_fail("!error_43","Problem1403_mod_global_in_out.c",402,__PRETTY_FUNCTION__);
        }
        if (294 < a111 && a167 == 13 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_44:
          !1?((void )0) : __assert_fail("!error_44","Problem1403_mod_global_in_out.c",406,__PRETTY_FUNCTION__);
        }
        if (8 == 9 && a1 == 12 && a71 == 13 && 247 < a37) {
          cf = 0;
          error_45:
          !1?((void )0) : __assert_fail("!error_45","Problem1403_mod_global_in_out.c",410,__PRETTY_FUNCTION__);
        }
        if (99 < 25 && a94 == 12 && 108 == a52_4 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_46:
          !1?((void )0) : __assert_fail("!error_46","Problem1403_mod_global_in_out.c",414,__PRETTY_FUNCTION__);
        }
        if (2 == 1 && 10 == 3 && a71 == 9 && 247 < a37) {
          cf = 0;
          error_47:
          !1?((void )0) : __assert_fail("!error_47","Problem1403_mod_global_in_out.c",418,__PRETTY_FUNCTION__);
        }
        if (10 < 64 && 163 >= 64 && a167 == 12 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_48:
          !1?((void )0) : __assert_fail("!error_48","Problem1403_mod_global_in_out.c",422,__PRETTY_FUNCTION__);
        }
        if (15 == 27 && 232 < a80 && a188 <= - 79 && a37 <= 35) {
          cf = 0;
          error_49:
          !1?((void )0) : __assert_fail("!error_49","Problem1403_mod_global_in_out.c",426,__PRETTY_FUNCTION__);
        }
        if (8 == 9 && a194 == 10 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_50:
          !1?((void )0) : __assert_fail("!error_50","Problem1403_mod_global_in_out.c",430,__PRETTY_FUNCTION__);
        }
        if (a119 == 9 && a80 <= 88 && 98 == a52_0 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_51:
          !1?((void )0) : __assert_fail("!error_51","Problem1403_mod_global_in_out.c",434,__PRETTY_FUNCTION__);
        }
        if (a119 == 14 && a68 == 13 && a71 == 10 && 247 < a37) {
          cf = 0;
          error_52:
          !1?((void )0) : __assert_fail("!error_52","Problem1403_mod_global_in_out.c",438,__PRETTY_FUNCTION__);
        }
        if (15 == 10 && a53 == 1 && (111 < a188 && 285 >= a188) && a37 <= 35) {
          cf = 0;
          error_53:
          !1?((void )0) : __assert_fail("!error_53","Problem1403_mod_global_in_out.c",442,__PRETTY_FUNCTION__);
        }
        if (a8 == 6 && 339 < a42 && a71 == 7 && 247 < a37) {
          cf = 0;
          error_54:
          !1?((void )0) : __assert_fail("!error_54","Problem1403_mod_global_in_out.c",446,__PRETTY_FUNCTION__);
        }
        if (a1 == 11 && (142 < a74 && 304 >= a74) && (5 < a105 && 198 >= a105) && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_55:
          !1?((void )0) : __assert_fail("!error_55","Problem1403_mod_global_in_out.c",450,__PRETTY_FUNCTION__);
        }
        if (a122 == 1 && a35 == 8 && 115 == a52_5 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_56:
          !1?((void )0) : __assert_fail("!error_56","Problem1403_mod_global_in_out.c",454,__PRETTY_FUNCTION__);
        }
        if (14 == 15 && 101 == 107 && a71 == 8 && 247 < a37) {
          cf = 0;
          error_57:
          !1?((void )0) : __assert_fail("!error_57","Problem1403_mod_global_in_out.c",458,__PRETTY_FUNCTION__);
        }
        if (a8 == 7 && 339 < a42 && a71 == 7 && 247 < a37) {
          cf = 0;
          error_58:
          !1?((void )0) : __assert_fail("!error_58","Problem1403_mod_global_in_out.c",462,__PRETTY_FUNCTION__);
        }
        if (13 == 13 && 9 == 14 && a71 == 12 && 247 < a37) {
          cf = 0;
          error_59:
          !1?((void )0) : __assert_fail("!error_59","Problem1403_mod_global_in_out.c",466,__PRETTY_FUNCTION__);
        }
        if (a122 == 7 && a35 == 8 && 115 == a52_5 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_60:
          !1?((void )0) : __assert_fail("!error_60","Problem1403_mod_global_in_out.c",470,__PRETTY_FUNCTION__);
        }
        if (a173 == 9 && (142 < a74 && 304 >= a74) && a71 == 14 && 247 < a37) {
          cf = 0;
          error_61:
          !1?((void )0) : __assert_fail("!error_61","Problem1403_mod_global_in_out.c",474,__PRETTY_FUNCTION__);
        }
        if (a35 == 13 && 106 == 106 && a71 == 8 && 247 < a37) {
          cf = 0;
          error_62:
          !1?((void )0) : __assert_fail("!error_62","Problem1403_mod_global_in_out.c",478,__PRETTY_FUNCTION__);
        }
        if (a89 == 11 && 89 == a183_2 && 285 < a188 && a37 <= 35) {
          cf = 0;
          error_63:
          !1?((void )0) : __assert_fail("!error_63","Problem1403_mod_global_in_out.c",482,__PRETTY_FUNCTION__);
        }
        if (- 71 < a198 && 88 >= a198 && 24 == a15_5 && a71 == 11 && 247 < a37) {
          cf = 0;
          error_64:
          !1?((void )0) : __assert_fail("!error_64","Problem1403_mod_global_in_out.c",486,__PRETTY_FUNCTION__);
        }
        if (a35 == 10 && a167 == 6 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_65:
          !1?((void )0) : __assert_fail("!error_65","Problem1403_mod_global_in_out.c",490,__PRETTY_FUNCTION__);
        }
        if (a173 == 10 && a167 == 9 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_66:
          !1?((void )0) : __assert_fail("!error_66","Problem1403_mod_global_in_out.c",494,__PRETTY_FUNCTION__);
        }
        if (a62 == 5 && (150 < a80 && 232 >= a80) && 98 == a52_0 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_67:
          !1?((void )0) : __assert_fail("!error_67","Problem1403_mod_global_in_out.c",498,__PRETTY_FUNCTION__);
        }
        if (a62 == 4 && a35 == 7 && 115 == a52_5 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_68:
          !1?((void )0) : __assert_fail("!error_68","Problem1403_mod_global_in_out.c",502,__PRETTY_FUNCTION__);
        }
        if (- 94 < a111 && 97 >= a111 && a167 == 13 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_69:
          !1?((void )0) : __assert_fail("!error_69","Problem1403_mod_global_in_out.c",506,__PRETTY_FUNCTION__);
        }
        if (8 == 12 && 304 < a74 && (5 < a105 && 198 >= a105) && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_70:
          !1?((void )0) : __assert_fail("!error_70","Problem1403_mod_global_in_out.c",510,__PRETTY_FUNCTION__);
        }
        if (383 < 64 && 27 == a15_2 && a71 == 11 && 247 < a37) {
          cf = 0;
          error_71:
          !1?((void )0) : __assert_fail("!error_71","Problem1403_mod_global_in_out.c",514,__PRETTY_FUNCTION__);
        }
        if (a173 == 15 && a68 == 11 && a71 == 10 && 247 < a37) {
          cf = 0;
          error_72:
          !1?((void )0) : __assert_fail("!error_72","Problem1403_mod_global_in_out.c",518,__PRETTY_FUNCTION__);
        }
        if (13 == 10 && a194 == 5 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_73:
          !1?((void )0) : __assert_fail("!error_73","Problem1403_mod_global_in_out.c",522,__PRETTY_FUNCTION__);
        }
        if (a173 == 9 && a1 == 6 && a71 == 13 && 247 < a37) {
          cf = 0;
          error_74:
          !1?((void )0) : __assert_fail("!error_74","Problem1403_mod_global_in_out.c",526,__PRETTY_FUNCTION__);
        }
        if (9 == 8 && (88 < a80 && 150 >= a80) && 98 == a52_0 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_75:
          !1?((void )0) : __assert_fail("!error_75","Problem1403_mod_global_in_out.c",530,__PRETTY_FUNCTION__);
        }
        if (62 < -5 && 159 >= -5 && a1 == 10 && a71 == 13 && 247 < a37) {
          cf = 0;
          error_76:
          !1?((void )0) : __assert_fail("!error_76","Problem1403_mod_global_in_out.c",534,__PRETTY_FUNCTION__);
        }
        if (13 == 8 && a194 == 5 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_77:
          !1?((void )0) : __assert_fail("!error_77","Problem1403_mod_global_in_out.c",538,__PRETTY_FUNCTION__);
        }
        if (45 == 57 && a42 <= 71 && a71 == 7 && 247 < a37) {
          cf = 0;
          error_78:
          !1?((void )0) : __assert_fail("!error_78","Problem1403_mod_global_in_out.c",542,__PRETTY_FUNCTION__);
        }
        if (a158 == 10 && a94 == 15 && 108 == a52_4 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_79:
          !1?((void )0) : __assert_fail("!error_79","Problem1403_mod_global_in_out.c",546,__PRETTY_FUNCTION__);
        }
        if (a94 == 11 && 9 == 7 && a71 == 12 && 247 < a37) {
          cf = 0;
          error_80:
          !1?((void )0) : __assert_fail("!error_80","Problem1403_mod_global_in_out.c",550,__PRETTY_FUNCTION__);
        }
        if (a116 == 7 && a194 == 6 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_81:
          !1?((void )0) : __assert_fail("!error_81","Problem1403_mod_global_in_out.c",554,__PRETTY_FUNCTION__);
        }
        if (a111 <= - 94 && a167 == 13 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_82:
          !1?((void )0) : __assert_fail("!error_82","Problem1403_mod_global_in_out.c",558,__PRETTY_FUNCTION__);
        }
        if (a118 == 3 && (88 < a80 && 150 >= a80) && a188 <= - 79 && a37 <= 35) {
          cf = 0;
          error_83:
          !1?((void )0) : __assert_fail("!error_83","Problem1403_mod_global_in_out.c",562,__PRETTY_FUNCTION__);
        }
        if (2 == 7 && 10 == 9 && a71 == 9 && 247 < a37) {
          cf = 0;
          error_84:
          !1?((void )0) : __assert_fail("!error_84","Problem1403_mod_global_in_out.c",566,__PRETTY_FUNCTION__);
        }
        if (a194 == 8 && a167 == 11 && a105 <= - 188 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_85:
          !1?((void )0) : __assert_fail("!error_85","Problem1403_mod_global_in_out.c",570,__PRETTY_FUNCTION__);
        }
        if (a26 == 9 && a94 == 13 && 108 == a52_4 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_86:
          !1?((void )0) : __assert_fail("!error_86","Problem1403_mod_global_in_out.c",574,__PRETTY_FUNCTION__);
        }
        if (- 94 < a111 && 97 >= a111 && 9 == 10 && a71 == 12 && 247 < a37) {
          cf = 0;
          error_87:
          !1?((void )0) : __assert_fail("!error_87","Problem1403_mod_global_in_out.c",578,__PRETTY_FUNCTION__);
        }
        if (2 == 5 && 9 == 9 && a71 == 12 && 247 < a37) {
          cf = 0;
          error_88:
          !1?((void )0) : __assert_fail("!error_88","Problem1403_mod_global_in_out.c",582,__PRETTY_FUNCTION__);
        }
        if (71 < a42 && 169 >= a42 && a49 <= 153 && (- 188 < a105 && 5 >= a105) && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_89:
          !1?((void )0) : __assert_fail("!error_89","Problem1403_mod_global_in_out.c",586,__PRETTY_FUNCTION__);
        }
        if (a173 == 14 && (142 < a74 && 304 >= a74) && a71 == 14 && 247 < a37) {
          cf = 0;
          error_90:
          !1?((void )0) : __assert_fail("!error_90","Problem1403_mod_global_in_out.c",590,__PRETTY_FUNCTION__);
        }
        if (a158 == 5 && a94 == 15 && 108 == a52_4 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_91:
          !1?((void )0) : __assert_fail("!error_91","Problem1403_mod_global_in_out.c",594,__PRETTY_FUNCTION__);
        }
        if (62 < -5 && 159 >= -5 && 10 == 10 && a71 == 9 && 247 < a37) {
          cf = 0;
          error_92:
          !1?((void )0) : __assert_fail("!error_92","Problem1403_mod_global_in_out.c",598,__PRETTY_FUNCTION__);
        }
        if (a53 == 6 && 232 < a80 && 98 == a52_0 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_93:
          !1?((void )0) : __assert_fail("!error_93","Problem1403_mod_global_in_out.c",602,__PRETTY_FUNCTION__);
        }
        if (a116 == 5 && a194 == 6 && 198 < a105 && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_94:
          !1?((void )0) : __assert_fail("!error_94","Problem1403_mod_global_in_out.c",606,__PRETTY_FUNCTION__);
        }
        if (a187 == 7 && (70 < a74 && 142 >= a74) && (5 < a105 && 198 >= a105) && (35 < a37 && 198 >= a37)) {
          cf = 0;
          error_95:
          !1?((void )0) : __assert_fail("!error_95","Problem1403_mod_global_in_out.c",610,__PRETTY_FUNCTION__);
        }
        if (14 == 11 && 101 == 107 && a71 == 8 && 247 < a37) {
          cf = 0;
          error_96:
          !1?((void )0) : __assert_fail("!error_96","Problem1403_mod_global_in_out.c",614,__PRETTY_FUNCTION__);
        }
        if (a62 == 2 && (150 < a80 && 232 >= a80) && 98 == a52_0 && (198 < a37 && 247 >= a37)) {
          cf = 0;
          error_97:
          !1?((void )0) : __assert_fail("!error_97","Problem1403_mod_global_in_out.c",618,__PRETTY_FUNCTION__);
        }
        if (0 == 7 && 10 == 5 && a71 == 9 && 247 < a37) {
          cf = 0;
          error_98:
          !1?((void )0) : __assert_fail("!error_98","Problem1403_mod_global_in_out.c",622,__PRETTY_FUNCTION__);
        }
        if (13 == 8 && 9 == 14 && a71 == 12 && 247 < a37) {
          cf = 0;
          error_99:
          !1?((void )0) : __assert_fail("!error_99","Problem1403_mod_global_in_out.c",626,__PRETTY_FUNCTION__);
        }
      }
      if (cf == 1) {
        output = - 2;
      }
    }
//edited by script: global output variable replacing printf in functions 
    if (output == - 2) {
      fprintf(stderr,"Invalid input: %d\n",input);
    }
    else {
      if (output != - 1) {
        printf("%d\n",output);
      }
    }
  }
}
