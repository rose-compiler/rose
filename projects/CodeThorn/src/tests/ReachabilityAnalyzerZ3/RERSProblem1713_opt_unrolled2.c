//**ALL COMMENTS ABOUT MANUAL ADJUSTMENTS AND REACHABILITY ARE MARKED WITH TWO STARS
//**ALL ERROR NUMBERS UNREACHABLE


// .--------------------------------------------------------.
// | printf in functions replaced by global output variable |
// *--------------------------------------------------------*
#include <stdio.h> 
#include <assert.h>
//#include <math.h> //**REMOVED INCLUSION OF UNNECESSARY PACKAGE
//#include <stdlib.h> //**REMOVED INCLUSION OF UNNECESSARY PACKAGE
extern void __VERIFIER_error(int );
//edited by script: global output variable replacing printf in functions 

//int output;   //**DELARED output AND input LOCALLY INSTEAD AND INITIALIZED THEM WITH 0
//int input;


//int inputs[] = {(5), (6), (1), (7), (2), (8), (3), (9), (10), (4)}; //**REMOVED DECLARATION OF VALID INPUTS

/*				//**REMOVED ALL FUNCTION DECLARATIONS EXCEPT __VERIFIER_error
void errorCheck();
void calculate_output();
void calculate_outputm1();
void calculate_outputm2();
void calculate_outputm3();
void calculate_outputm4();
void calculate_outputm5();
void calculate_outputm6();
void calculate_outputm7();
void calculate_outputm8();
void calculate_outputm9();
void calculate_outputm10();
void calculate_outputm11();
void calculate_outputm12();
void calculate_outputm13();
void calculate_outputm14();
void calculate_outputm15();
void calculate_outputm16();
void calculate_outputm17();
void calculate_outputm18();
void calculate_outputm19();
void calculate_outputm20();
void calculate_outputm21();
void calculate_outputm22();
void calculate_outputm23();
void calculate_outputm24();
void calculate_outputm25();
void calculate_outputm26();
void calculate_outputm27();
void calculate_outputm28();
void calculate_outputm29();
void calculate_outputm30();
void calculate_outputm31();
void calculate_outputm32();
void calculate_outputm33();
void calculate_outputm34();
void calculate_outputm35();
void calculate_outputm36();
void calculate_outputm37();
void calculate_outputm38();
void calculate_outputm39();
void calculate_outputm40();
void calculate_outputm41();
void calculate_outputm42();
void calculate_outputm43();
void calculate_outputm44();
void calculate_outputm45();
void calculate_outputm46();
void calculate_outputm47();
void calculate_outputm48();
void calculate_outputm49();
void calculate_outputm50();
void calculate_outputm51();
void calculate_outputm52();
void calculate_outputm53();
void calculate_outputm54();
void calculate_outputm55();
void calculate_outputm56();
void calculate_outputm57();
void calculate_outputm58();
void calculate_outputm59();
void calculate_outputm60();
void calculate_outputm61();
void calculate_outputm62();
void calculate_outputm63();
void calculate_outputm64();
void calculate_outputm65();
void calculate_outputm66();
void calculate_outputm67();
void calculate_outputm68();
void calculate_outputm69();
void calculate_outputm70();
void calculate_outputm71();
void calculate_outputm72();
void calculate_outputm73();
void calculate_outputm74();
void calculate_outputm75();
void calculate_outputm76();
void calculate_outputm77();
void calculate_outputm78();
void calculate_outputm79();
void calculate_outputm80();
void calculate_outputm81();
void calculate_outputm82();
void calculate_outputm83();
void calculate_outputm84();
void calculate_outputm85();
void calculate_outputm86();
void calculate_outputm87();
void calculate_outputm88();
void calculate_outputm89();
void calculate_outputm90();
void calculate_outputm91();
void calculate_outputm92();
void calculate_outputm93();
void calculate_outputm94();
void calculate_outputm95();
void calculate_outputm96();
void calculate_outputm97();
void calculate_outputm98();
void calculate_outputm99();
void calculate_outputm100();
void calculate_outputm101();
void calculate_outputm102();
void calculate_outputm103();
void calculate_outputm104();
void calculate_outputm105();
void calculate_outputm106();
void calculate_outputm107();
void calculate_outputm108();
void calculate_outputm109();
void calculate_outputm110();
void calculate_outputm111();
void calculate_outputm112();
void calculate_outputm113();
void calculate_outputm114();
void calculate_outputm115();
void calculate_outputm116();
void calculate_outputm117();
void calculate_outputm118();
void calculate_outputm119();
void calculate_outputm120();
void calculate_outputm121();
void calculate_outputm122();
void calculate_outputm123();
void calculate_outputm124();
void calculate_outputm125();
void calculate_outputm126();
void calculate_outputm127();
void calculate_outputm128();
void calculate_outputm129();
void calculate_outputm130();
void calculate_outputm131();
void calculate_outputm132();
void calculate_outputm133();
void calculate_outputm134();
void calculate_outputm135();
void calculate_outputm136();
void calculate_outputm137();
void calculate_outputm138();
void calculate_outputm139();
void calculate_outputm140();
void calculate_outputm141();
void calculate_outputm142();
void calculate_outputm143();
void calculate_outputm144();
void calculate_outputm145();
void calculate_outputm146();
void calculate_outputm147();
void calculate_outputm148();
void calculate_outputm149();
void calculate_outputm150();
void calculate_outputm151();
void calculate_outputm152();
void calculate_outputm153();
void calculate_outputm154();
void calculate_outputm155();
void calculate_outputm156();
void calculate_outputm157();
void calculate_outputm158();
void calculate_outputm159();
void calculate_outputm160();
void calculate_outputm161();
void calculate_outputm162();
void calculate_outputm163();
void calculate_outputm164();
void calculate_outputm165();
void calculate_outputm166();
void calculate_outputm167();
void calculate_outputm168();
void calculate_outputm169();
void calculate_outputm170();
void calculate_outputm171();
void calculate_outputm172();
void calculate_outputm173();
void calculate_outputm174();
void calculate_outputm175();
void calculate_outputm176();
void calculate_outputm177();
void calculate_outputm178();
void calculate_outputm179();
void calculate_outputm180();
void calculate_outputm181();
void calculate_outputm182();
void calculate_outputm183();
void calculate_outputm184();
void calculate_outputm185();
void calculate_outputm186();
void calculate_outputm187();
void calculate_outputm188();
void calculate_outputm189();
void calculate_outputm190();
void calculate_outputm191();
void calculate_outputm192();
void calculate_outputm193();
void calculate_outputm194();
void calculate_outputm195();
void calculate_outputm196();
void calculate_outputm197();
void calculate_outputm198();
void calculate_outputm199();
void calculate_outputm200();
void calculate_outputm201();
void calculate_outputm202();
void calculate_outputm203();
void calculate_outputm204();
void calculate_outputm205();
void calculate_outputm206();
void calculate_outputm207();
void calculate_outputm208();
void calculate_outputm209();
void calculate_outputm210();
void calculate_outputm211();
void calculate_outputm212();
void calculate_outputm213();
void calculate_outputm214();
void calculate_outputm215();
void calculate_outputm216();
void calculate_outputm217();
void calculate_outputm218();
void calculate_outputm219();
void calculate_outputm220();
void calculate_outputm221();
void calculate_outputm222();
void calculate_outputm223();
void calculate_outputm224();
void calculate_outputm225();
void calculate_outputm226();
void calculate_outputm227();
void calculate_outputm228();
void calculate_outputm229();
void calculate_outputm230();
void calculate_outputm231();
void calculate_outputm232();
void calculate_outputm233();
void calculate_outputm234();
void calculate_outputm235();
void calculate_outputm236();
void calculate_outputm237();
void calculate_outputm238();
void calculate_outputm239();
void calculate_outputm240();
void calculate_outputm241();
void calculate_outputm242();
void calculate_outputm243();
void calculate_outputm244();
void calculate_outputm245();
void calculate_outputm246();
void calculate_outputm247();
void calculate_outputm248();
void calculate_outputm249();
void calculate_outputm250();
void calculate_outputm251();
void calculate_outputm252();
void calculate_outputm253();
void calculate_outputm254();
void calculate_outputm255();
void calculate_outputm256();
void calculate_outputm257();
void calculate_outputm258();
void calculate_outputm259();
void calculate_outputm260();
void calculate_outputm261();
void calculate_outputm262();
void calculate_outputm263();
void calculate_outputm264();
void calculate_outputm265();
void calculate_outputm266();
void calculate_outputm267();
void calculate_outputm268();
void calculate_outputm269();
void calculate_outputm270();
void calculate_outputm271();
void calculate_outputm272();
void calculate_outputm273();
void calculate_outputm274();
void calculate_outputm275();
void calculate_outputm276();
void calculate_outputm277();
void calculate_outputm278();
void calculate_outputm279();
void calculate_outputm280();
void calculate_outputm281();
void calculate_outputm282();
void calculate_outputm283();
void calculate_outputm284();
void calculate_outputm285();
void calculate_outputm286();
void calculate_outputm287();
void calculate_outputm288();
void calculate_outputm289();
void calculate_outputm290();
void calculate_outputm291();
void calculate_outputm292();
void calculate_outputm293();
void calculate_outputm294();
void calculate_outputm295();
void calculate_outputm296();
void calculate_outputm297();
void calculate_outputm298();
void calculate_outputm299();
void calculate_outputm300();
void calculate_outputm301();
void calculate_outputm302();
void calculate_outputm303();
void calculate_outputm304();
void calculate_outputm305();
void calculate_outputm306();
void calculate_outputm307();
void calculate_outputm308();
void calculate_outputm309();
void calculate_outputm310();
void calculate_outputm311();
void calculate_outputm312();
void calculate_outputm313();
void calculate_outputm314();
void calculate_outputm315();
void calculate_outputm316();
void calculate_outputm317();
void calculate_outputm318();
void calculate_outputm319();
void calculate_outputm320();
void calculate_outputm321();
void calculate_outputm322();
void calculate_outputm323();
void calculate_outputm324();
void calculate_outputm325();
void calculate_outputm326();
void calculate_outputm327();
void calculate_outputm328();
void calculate_outputm329();
void calculate_outputm330();
void calculate_outputm331();
void calculate_outputm332();
void calculate_outputm333();
void calculate_outputm334();
void calculate_outputm335();
void calculate_outputm336();
void calculate_outputm337();
void calculate_outputm338();
void calculate_outputm339();
void calculate_outputm340();
void calculate_outputm341();
void calculate_outputm342();
void calculate_outputm343();
void calculate_outputm344();
void calculate_outputm345();
void calculate_outputm346();
void calculate_outputm347();
void calculate_outputm348();
void calculate_outputm349();
void calculate_outputm350();
void calculate_outputm351();
void calculate_outputm352();
void calculate_outputm353();
void calculate_outputm354();
void calculate_outputm355();
void calculate_outputm356();
void calculate_outputm357();
void calculate_outputm358();
void calculate_outputm359();
void calculate_outputm360();
void calculate_outputm361();
void calculate_outputm362();
void calculate_outputm363();
void calculate_outputm364();
void calculate_outputm365();
void calculate_outputm366();
void calculate_outputm367();
void calculate_outputm368();
void calculate_outputm369();
void calculate_outputm370();
void calculate_outputm371();
void calculate_outputm372();
void calculate_outputm373();
void calculate_outputm374();
void calculate_outputm375();
void calculate_outputm376();
void calculate_outputm377();
void calculate_outputm378();
void calculate_outputm379();
void calculate_outputm380();
void calculate_outputm381();
void calculate_outputm382();
void calculate_outputm383();
void calculate_outputm384();
void calculate_outputm385();
void calculate_outputm386();
void calculate_outputm387();
void calculate_outputm388();
void calculate_outputm389();
void calculate_outputm390();
void calculate_outputm391();
void calculate_outputm392();
void calculate_outputm393();
void calculate_outputm394();
void calculate_outputm395();
void calculate_outputm396();
void calculate_outputm397();
void calculate_outputm398();
void calculate_outputm399();
void calculate_outputm400();
void calculate_outputm401();
void calculate_outputm402();
void calculate_outputm403();
void calculate_outputm404();
void calculate_outputm405();
void calculate_outputm406();
void calculate_outputm407();
void calculate_outputm408();
void calculate_outputm409();
void calculate_outputm410();
void calculate_outputm411();
void calculate_outputm412();
void calculate_outputm413();
void calculate_outputm414();
void calculate_outputm415();
void calculate_outputm416();
void calculate_outputm417();
void calculate_outputm418();
void calculate_outputm419();
void calculate_outputm420();
void calculate_outputm421();
void calculate_outputm422();
void calculate_outputm423();
void calculate_outputm424();
void calculate_outputm425();
void calculate_outputm426();
void calculate_outputm427();
void calculate_outputm428();
void calculate_outputm429();
void calculate_outputm430();
*/

/*			//**DECLARED ALL VARIABLE LOCALLY INSTEAD
int a344 = 8;
int a139 = 32;
int a386 = 9;
int a47 = 11;
int a323 = 4;
int a171 = 8;
int a185 = 7;
int a178 = 9;
int a31 = 14;
int a42 = 14;
int a310 = 13;
int a7 = 6;
int a289 = 35;
int a8 = 36;
int a190 = 34;
int a314 = 33;
int a24 = 35;
int a128 = 34;
int a0 = 6;
int a32 = 9;
int a191 = 33;
int a170 = 13;
int a296 = 32;
int a20 = 1;
int a271 = 7;
int a153 = 8;
int a86 = 5;
int a104 = 32;
int a292 = 3;
int a96 = 9;
int a74 = 33;
int a377 = 9;
int a232 = 36;
int a44 = 8;
int a184 = 11;
int a116 = 5;
int a16 = 34;
int a172 = 32;
int a100 = 32;
int a129 = 35;
int a66 = 34;
int a63 = 12;
int a119 = 34;
int a83 = 33;
int a79 = 32;
int a12 = 35;
int a37 = 32;
int a5 = 35;
int a98 = 5;
int a78 = 35;
int a156 = 34;
int a196 = 35;
int a51 = 4;
int a244 = 32;
int a162 = 8;
int a87 = 15;
int a80 = 33;
int a183 = 5;
int a341 = 33;
int a9 = 36;
int a49 = 3;
int a154 = 14;
int a56 = 13;
int a332 = 33;
int a58 = 36;
int a21 = 4;
int a138 = 8;
int a112 = 34;
int a36 = 13;
int a175 = 12;
int a186 = 34;
int a385 = 36;
int a103 = 35;
int a52 = 35;
int a91 = 34;
int a113 = 33;
int a168 = 6;
int a194 = 11;
int a28 = 4;
int a144 = 32;
int a161 = 15;
int a57 = 32;
int a115 = 36;
int a248 = 7;
int a117 = 8;
int a141 = 3;
int a164 = 35;
int a249 = 34;
int a165 = 10;
int a120 = 32;
int a145 = 33;
int a195 = 9;
int a22 = 35;
int a151 = 8;
int a99 = 4;
int a182 = 13;
int a163 = 36;
int a146 = 32;
int a166 = 35;
int a60 = 14;
int a270 = 36;
int a364 = 6;
int a125 = 14;
int a111 = 4;
int a127 = 11;
int a3 = 13;
int a19 = 35;
int a142 = 12;
int a148 = 33;
int a123 = 33;
int a159 = 6;
int a135 = 35;
int a176 = 35;
int a73 = 35;
int a15 = 10;
int a17 = 5;
int a102 = 9;
int a140 = 13;
int a93 = 6;
int a130 = 5;
int a106 = 32;
int a179 = 32;
int cf = 1;
int a59 = 35;
int a40 = 32;
int a77 = 9;
int a13 = 32;
int a71 = 6;
int a55 = 6;
int a242 = 9;
int a4 = 12;
int a25 = 33;
int a75 = 35;
int a54 = 13;
int a198 = 34;
int a89 = 32;
int a81 = 36;
int a121 = 35;
int a192 = 3;
int a390 = 8;
int a197 = 33;
int a306 = 4;
int a2 = 7;
int a72 = 33;
int a69 = 12;
int a200 = 5;
int a70 = 34;
int a173 = 8;
int a126 = 36;
int a143 = 10;
int a122 = 36;
int a67 = 33;
int a30 = 4;
int a23 = 12;
int a157 = 35;
int a365 = 11;
int a222 = 33;
int a62 = 3;
int a35 = 35;
int a265 = 4;
int a335 = 12;
*/

int main()
{

int output = 0;
int input = 0;


int a344 = 8;
int a139 = 32;
int a386 = 9;
int a47 = 11;
int a323 = 4;
int a171 = 8;
int a185 = 7;
int a178 = 9;
int a31 = 14;
int a42 = 14;
int a310 = 13;
int a7 = 6;
int a289 = 35;
int a8 = 36;
int a190 = 34;
int a314 = 33;
int a24 = 35;
int a128 = 34;
int a0 = 6;
int a32 = 9;
int a191 = 33;
int a170 = 13;
int a296 = 32;
int a20 = 1;
int a271 = 7;
int a153 = 8;
int a86 = 5;
int a104 = 32;
int a292 = 3;
int a96 = 9;
int a74 = 33;
int a377 = 9;
int a232 = 36;
int a44 = 8;
int a184 = 11;
int a116 = 5;
int a16 = 34;
int a172 = 32;
int a100 = 32;
int a129 = 35;
int a66 = 34;
int a63 = 12;
int a119 = 34;
int a83 = 33;
int a79 = 32;
int a12 = 35;
int a37 = 32;
int a5 = 35;
int a98 = 5;
int a78 = 35;
int a156 = 34;
int a196 = 35;
int a51 = 4;
int a244 = 32;
int a162 = 8;
int a87 = 15;
int a80 = 33;
int a183 = 5;
int a341 = 33;
int a9 = 36;
int a49 = 3;
int a154 = 14;
int a56 = 13;
int a332 = 33;
int a58 = 36;
int a21 = 4;
int a138 = 8;
int a112 = 34;
int a36 = 13;
int a175 = 12;
int a186 = 34;
int a385 = 36;
int a103 = 35;
int a52 = 35;
int a91 = 34;
int a113 = 33;
int a168 = 6;
int a194 = 11;
int a28 = 4;
int a144 = 32;
int a161 = 15;
int a57 = 32;
int a115 = 36;
int a248 = 7;
int a117 = 8;
int a141 = 3;
int a164 = 35;
int a249 = 34;
int a165 = 10;
int a120 = 32;
int a145 = 33;
int a195 = 9;
int a22 = 35;
int a151 = 8;
int a99 = 4;
int a182 = 13;
int a163 = 36;
int a146 = 32;
int a166 = 35;
int a60 = 14;
int a270 = 36;
int a364 = 6;
int a125 = 14;
int a111 = 4;
int a127 = 11;
int a3 = 13;
int a19 = 35;
int a142 = 12;
int a148 = 33;
int a123 = 33;
int a159 = 6;
int a135 = 35;
int a176 = 35;
int a73 = 35;
int a15 = 10;
int a17 = 5;
int a102 = 9;
int a140 = 13;
int a93 = 6;
int a130 = 5;
int a106 = 32;
int a179 = 32;
int cf = 1;
int a59 = 35;
int a40 = 32;
int a77 = 9;
int a13 = 32;
int a71 = 6;
int a55 = 6;
int a242 = 9;
int a4 = 12;
int a25 = 33;
int a75 = 35;
int a54 = 13;
int a198 = 34;
int a89 = 32;
int a81 = 36;
int a121 = 35;
int a192 = 3;
int a390 = 8;
int a197 = 33;
int a306 = 4;
int a2 = 7;
int a72 = 33;
int a69 = 12;
int a200 = 5;
int a70 = 34;
int a173 = 8;
int a126 = 36;
int a143 = 10;
int a122 = 36;
int a67 = 33;
int a30 = 4;
int a23 = 12;
int a157 = 35;
int a365 = 11;
int a222 = 33;
int a62 = 3;
int a35 = 35;
int a265 = 4;
int a335 = 12;



// main i/o-loop


//  while(1){ //**UNROLLED MAIN LOOP 


//edited by script: maybe no output, reset output var 
    output = - 1;
// read input
    scanf("%d",&input);
// operate eca engine
    //if (input != 5 && input != 6 && input != 1 && input != 7 && input != 2 && input != 8 && input != 3 && input != 9 && input != 10 && input != 4) //**REMOVED RETURN STATEMENT AND GENERATE FORMULA (1 <= input <= 10) WHENEVER AN INPUT IS READ
    //return - 2;
{
      cf = 1;
      if (cf == 1 && a102 == 7) {
        if (a130 == 3 && cf == 1) {{
            if (cf == 1 && a168 == 1) {
              if (cf == 1 && a162 == 5) {{
                  if (input == 10 && (a162 == 5 && (a102 == 7 && cf == 1 && a130 == 3)) && a168 == 1) {
                    cf = 0;
                    a102 = 12;
                    a22 = 36;
                    a116 = 6;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 3 && (a102 == 7 && (a130 == 3 && cf == 1 && a162 == 5) && a168 == 1)) {
                    cf = 0;
                    a168 = 8;
                    a30 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a162 == 5 && (input == 1 && (a168 == 1 && (cf == 1 && a130 == 3)) && a102 == 7)) {
                    cf = 0;
                    a102 = 12;
                    a22 = 36;
                    a116 = 6;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a162 == 6 && cf == 1) {{
                  if (a168 == 1 && (a102 == 7 && cf == 1 && input == 4 && a162 == 6) && a130 == 3) {
                    cf = 0;
                    a112 = 34;
                    a130 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a130 == 3 && (cf == 1 && a162 == 6) && a168 == 1) && input == 3) {
                    cf = 0;
                    a270 = 35;
                    a102 = 13;
                    a157 = 36;
                    a170 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a162 == 9) {{
                  if (a162 == 9 && (cf == 1 && a102 == 7) && input == 1 && a130 == 3 && a168 == 1) {
                    cf = 0;
                    a116 = 5;
                    a5 = 35;
                    a102 = 12;
                    a173 = 7;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a168 == 2) {
              if (a86 == 7 && cf == 1) {{
                  if (a102 == 7 && (a86 == 7 && cf == 1) && a130 == 3 && a168 == 2 && input == 7) {
                    cf = 0;
                    a168 = 8;
                    a30 = 8;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a86 == 11) {{
                  if (a130 == 3 && (cf == 1 && a86 == 11 && a168 == 2 && a102 == 7 && input == 2)) {
                    cf = 0;
                    a130 = 10;
                    a143 = 5;
                    a49 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a168 == 2 && (a86 == 11 && (cf == 1 && a130 == 3)) && input == 4)) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a168 == 3) {
              if (cf == 1 && a15 == 8) {{
                  if (a168 == 3 && (input == 3 && (cf == 1 && a102 == 7 && a15 == 8 && a130 == 3))) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a15 == 8 && (a102 == 7 && cf == 1 && a168 == 3) && a130 == 3 && input == 7) {
                    cf = 0;
                    a102 = 9;
                    a12 = 34;
                    a20 = 2;
                    a63 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 10 && (a102 == 7 && (a130 == 3 && (a15 == 8 && cf == 1 && a168 == 3)))) {
                    cf = 0;
                    a20 = 2;
                    a102 = 9;
                    a12 = 34;
                    a63 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a130 == 3 && (input == 8 && (a168 == 3 && (cf == 1 && a15 == 8))) && a102 == 7) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a168 == 6 && cf == 1) {
              if (a242 == 10 && cf == 1) {{
                  if (a168 == 6 && (a242 == 10 && cf == 1 && a130 == 3 && a102 == 7 && input == 4)) {
                    cf = 0;
                    a102 = 12;
                    a116 = 4;
                    a67 = 32;
                    a98 = 5;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a242 == 10 && (input == 10 && (cf == 1 && a168 == 6)) && a130 == 3 && a102 == 7) {
                    cf = 0;
                    a116 = 6;
                    a102 = 12;
                    a56 = 14;
                    a49 = 3;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a168 == 6 && (a130 == 3 && (cf == 1 && input == 6) && a242 == 10) && a102 == 7) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 11;
                    a69 = 13;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a168 == 7) {
              if (cf == 1 && a100 == 36) {{
                  if (a130 == 3 && (a102 == 7 && (cf == 1 && input == 10 && a168 == 7) && a100 == 36)) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a100 == 36 && (a130 == 3 && (cf == 1 && a168 == 7) && a102 == 7 && input == 7)) {
                    cf = 0;
                    a102 = 12;
                    a67 = 34;
                    a116 = 4;
                    a142 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 6 && (a130 == 3 && (cf == 1 && a168 == 7 && a102 == 7)) && a100 == 36) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a100 == 36 && cf == 1 && a102 == 7 && input == 4 && a168 == 7 && a130 == 3) {
                    cf = 0;
                    a102 = 14;
                    a91 = 32;
                    a296 = 35;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a168 == 8 && cf == 1) {
              if (cf == 1 && a30 == 7) {{
                  if (a102 == 7 && (cf == 1 && a30 == 7 && a130 == 3 && input == 10 && a168 == 8)) {
                    cf = 0;
                    a106 = 35;
                    a314 = 34;
                    a102 = 11;
                    a4 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a130 == 3 && (input == 7 && (a168 == 8 && cf == 1) && a30 == 7 && a102 == 7)) {
                    cf = 0;
                    a143 = 5;
                    a130 = 10;
                    a49 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a30 == 7 && (a168 == 8 && (a102 == 7 && cf == 1))) && a130 == 3) {
                    cf = 0;
                    a5 = 32;
                    a116 = 5;
                    a102 = 12;
                    a117 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a130 == 3 && cf == 1 && a168 == 8 && a30 == 7 && input == 8 && a102 == 7) {
                    cf = 0;
                    a102 = 9;
                    a183 = 7;
                    a20 = 1;
                    a165 = 10;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a30 == 8 && cf == 1) {{
                  if (a168 == 8 && (a30 == 8 && cf == 1 && a130 == 3 && input == 10) && a102 == 7) {
                    cf = 0;
                    a310 = 10;
                    a190 = 36;
                    a102 = 14;
                    a28 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a168 == 8 && (a30 == 8 && (a130 == 3 && (input == 2 && cf == 1) && a102 == 7))) {
                    cf = 0;
                    a314 = 33;
                    a16 = 36;
                    a8 = 36;
                    a102 = 11;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a130 == 4) {{
            if (a17 == 4 && cf == 1) {
              if (cf == 1 && a21 == 9) {{
                  if (input == 3 && (a21 == 9 && (a17 == 4 && (cf == 1 && a102 == 7) && a130 == 4))) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a17 == 4 && cf == 1 && input == 2 && a130 == 4 && a102 == 7 && a21 == 9) {
                    cf = 0;
                    a310 = 9;
                    a102 = 14;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a17 == 6) {
              if (a143 == 5 && cf == 1) {{
                  if (a17 == 6 && (a102 == 7 && (a130 == 4 && cf == 1 && input == 4)) && a143 == 5) {
                    cf = 0;
                    a270 = 36;
                    a222 = 34;
                    a102 = 13;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a17 == 6 && cf == 1 && a130 == 4 && input == 10 && a143 == 5 && a102 == 7) {
                    cf = 0;
                    a270 = 36;
                    a102 = 13;
                    a222 = 34;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a17 == 6 && (a130 == 4 && cf == 1 && a143 == 5 && a102 == 7) && input == 1) {
                    cf = 0;
                    a102 = 12;
                    a270 = 33;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a17 == 7 && cf == 1) {
              if (cf == 1 && a151 == 4) {{
                  if (a130 == 4 && (a102 == 7 && cf == 1) && a151 == 4 && input == 4 && a17 == 7) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a151 == 4 && (cf == 1 && a17 == 7) && a102 == 7 && input == 2 && a130 == 4) {
                    cf = 0;
                    a20 = 6;
                    a120 = 33;
                    a102 = 9;
                    a127 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a151 == 4 && (a130 == 4 && (a102 == 7 && (a17 == 7 && (input == 7 && cf == 1))))) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a17 == 8) {
              if (a115 == 32 && cf == 1) {{
                  if (input == 4 && (a130 == 4 && (a17 == 8 && cf == 1) && a102 == 7) && a115 == 32) {
                    cf = 0;
                    a102 = 9;
                    a120 = 36;
                    a145 = 35;
                    a20 = 6;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a130 == 4 && (a102 == 7 && (a17 == 8 && cf == 1) && input == 6 && a115 == 32)) {
                    cf = 0;
                    a112 = 33;
                    a244 = 36;
                    a130 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a17 == 8 && (a115 == 32 && (cf == 1 && a130 == 4 && input == 2) && a102 == 7)) {
                    cf = 0;
                    a102 = 10;
                    a59 = 33;
                    a40 = 35;
                    a98 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (a115 == 35 && cf == 1) {{
                  if (a17 == 8 && (a102 == 7 && (a115 == 35 && cf == 1 && input == 7)) && a130 == 4) {
                    cf = 0;
                    a289 = 32;
                    a102 = 8;
                    a144 = 35;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a115 == 35 && (cf == 1 && a102 == 7 && a130 == 4 && input == 5) && a17 == 8) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a17 == 8 && (cf == 1 && input == 1 && a115 == 35) && a102 == 7 && a130 == 4) {
                    cf = 0;
                    a125 = 15;
                    a102 = 14;
                    a310 = 9;
                    a182 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a17 == 8 && (a102 == 7 && cf == 1)) && a130 == 4 && a115 == 35) {
                    cf = 0;
                    a144 = 35;
                    a289 = 32;
                    a102 = 8;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a17 == 9) {
              if (cf == 1 && a195 == 11) {{
                  if (a195 == 11 && (a130 == 4 && (input == 10 && (a102 == 7 && (a17 == 9 && cf == 1))))) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a9 = 33;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a17 == 9 && (a195 == 11 && (input == 3 && (a102 == 7 && cf == 1) && a130 == 4))) {
                    cf = 0;
                    a129 = 33;
                    a20 = 3;
                    a102 = 9;
                    a344 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a195 == 12) {{
                  if (a130 == 4 && (cf == 1 && a195 == 12 && input == 2) && a17 == 9 && a102 == 7) {
                    cf = 0;
                    a37 = 35;
                    a130 = 6;
                    a248 = 12;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a17 == 10 && cf == 1) {
              if (cf == 1 && 6 == 6) {{
                  if (a17 == 10 && (input == 6 && (a102 == 7 && (cf == 1 && a130 == 4 && 6 == 6)))) {
                    cf = 0;
                    a100 = 36;
                    a130 = 3;
                    a168 = 7;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a130 == 5) {{
            if (cf == 1 && a103 == 34) {
              if (a80 == 32 && cf == 1) {{
                  if (a130 == 5 && (a103 == 34 && (input == 8 && cf == 1 && a102 == 7)) && a80 == 32) {
                    cf = 0;
                    a130 = 8;
                    a182 = 9;
                    a386 = 12;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a103 == 35 && cf == 1) {
              if (a116 == 7 && cf == 1) {{
                  if (a103 == 35 && (cf == 1 && input == 3 && a116 == 7 && a130 == 5) && a102 == 7) {
                    cf = 0;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a116 == 7 && (a102 == 7 && (input == 2 && (a130 == 5 && (cf == 1 && a103 == 35))))) {
                    cf = 0;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a116 == 7 && (a102 == 7 && (input == 1 && (a103 == 35 && cf == 1) && a130 == 5))) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 13;
                    a87 = 12;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 4 && cf == 1 && a102 == 7 && a130 == 5 && a103 == 35 && a116 == 7) {
                    cf = 0;
                    a148 = 36;
                    a130 = 8;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a130 == 6 && cf == 1) {{
            if (a248 == 7 && cf == 1) {
              if (cf == 1 && 35 == 35) {{
                  if (35 == 35 && (a102 == 7 && cf == 1 && input == 6 && a248 == 7) && a130 == 6) {
                    cf = 0;
                    a130 = 4;
                    a17 = 6;
                    a143 = 5;
                    output = 15;
                    fflush(stdout);
                  }
                  if (35 == 35 && (cf == 1 && input == 7 && a248 == 7) && a102 == 7 && a130 == 6) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a248 == 8 && cf == 1) {
              if (a335 == 7 && cf == 1) {{
                  if (a248 == 8 && (cf == 1 && input == 1 && a102 == 7 && a335 == 7) && a130 == 6) {
                    cf = 0;
                    a102 = 14;
                    a310 = 15;
                    a22 = 33;
                    a71 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a335 == 8) {{
                  if (input == 3 && (a130 == 6 && cf == 1 && a248 == 8 && a102 == 7 && a335 == 8)) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a248 == 9) {
              if (cf == 1 && a178 == 5) {{
                  if (a102 == 7 && (input == 4 && (a178 == 5 && cf == 1 && a248 == 9)) && a130 == 6) {
                    cf = 0;
                    a310 = 11;
                    a102 = 14;
                    a0 = 3;
                    a154 = 10;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a248 == 12 && cf == 1) {
              if (cf == 1 && a37 == 32) {{
                  if (input == 2 && (a37 == 32 && (a248 == 12 && (cf == 1 && a130 == 6) && a102 == 7))) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 10 && (a248 == 12 && (cf == 1 && a130 == 6 && a102 == 7) && a37 == 32)) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a37 == 32 && (cf == 1 && a248 == 12) && a130 == 6 && a102 == 7 && input == 4) {
                    cf = 0;
                    a130 = 3;
                    a168 = 6;
                    a242 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a248 == 12 && (a37 == 32 && cf == 1 && input == 7) && a130 == 6 && a102 == 7) {
                    cf = 0;
                    a56 = 14;
                    a102 = 12;
                    a116 = 6;
                    a49 = 3;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a37 == 34) {{
                  if (a248 == 12 && (input == 1 && (cf == 1 && a102 == 7)) && a130 == 6 && a37 == 34) {
                    cf = 0;
                    a17 = 6;
                    a130 = 4;
                    a143 = 5;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a37 == 35) {{
                  if (a102 == 7 && (a130 == 6 && (a37 == 35 && cf == 1 && input == 1) && a248 == 12)) {
                    cf = 0;
                    a57 = 33;
                    a289 = 34;
                    a270 = 32;
                    a102 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a248 == 14) {
              if (cf == 1 && a365 == 12) {{
                  if (a248 == 14 && (a365 == 12 && (input == 10 && cf == 1 && a130 == 6)) && a102 == 7) {
                    cf = 0;
                    a248 = 7;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a248 == 14 && (a130 == 6 && cf == 1) && a365 == 12) && input == 3) {
                    cf = 0;
                    a168 = 1;
                    a130 = 3;
                    a162 = 6;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a130 == 7 && cf == 1) {{
            if (a162 == 5 && cf == 1) {
              if (a175 == 9 && cf == 1) {{
                  if (a175 == 9 && (input == 2 && (a102 == 7 && cf == 1) && a162 == 5 && a130 == 7)) {
                    cf = 0;
                    a314 = 35;
                    a83 = 34;
                    a102 = 11;
                    a117 = 14;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a175 == 9 && (input == 3 && (a162 == 5 && (cf == 1 && a130 == 7 && a102 == 7)))) {
                    cf = 0;
                    a102 = 12;
                    a37 = 36;
                    a74 = 35;
                    a116 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a175 == 9 && (a162 == 5 && (a130 == 7 && (cf == 1 && input == 4)) && a102 == 7)) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 13;
                    a87 = 12;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a175 == 9 && (input == 8 && (a130 == 7 && (a162 == 5 && cf == 1))) && a102 == 7) {
                    cf = 0;
                    a182 = 14;
                    a289 = 32;
                    a102 = 8;
                    a184 = 5;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a130 == 7 && (a102 == 7 && (input == 1 && (cf == 1 && a162 == 5)) && a175 == 9)) {
                    cf = 0;
                    a16 = 32;
                    a102 = 11;
                    a314 = 33;
                    a192 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (a175 == 11 && cf == 1) {{
                  if (input == 4 && (a130 == 7 && (a102 == 7 && cf == 1) && a162 == 5) && a175 == 11) {
                    cf = 0;
                    a296 = 35;
                    a91 = 36;
                    a102 = 14;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a162 == 5 && (a130 == 7 && (cf == 1 && input == 8 && a175 == 11)))) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a162 == 5 && (a102 == 7 && (a130 == 7 && (input == 6 && cf == 1) && a175 == 11))) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a162 == 5 && (cf == 1 && a175 == 11 && a102 == 7 && input == 10 && a130 == 7)) {
                    cf = 0;
                    a130 = 8;
                    a148 = 36;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 9 && (a102 == 7 && (a175 == 11 && (a162 == 5 && (a130 == 7 && cf == 1))))) {
                    cf = 0;
                    a130 = 3;
                    a168 = 6;
                    a242 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a162 == 7 && cf == 1) {
              if (cf == 1 && a28 == 3) {{
                  if (a102 == 7 && (a162 == 7 && (cf == 1 && a130 == 7 && a28 == 3 && input == 8))) {
                    cf = 0;
                    a102 = 8;
                    a289 = 33;
                    a75 = 35;
                    a20 = 2;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a162 == 8 && cf == 1) {
              if (a93 == 6 && cf == 1) {{
                  if (a162 == 8 && (a130 == 7 && (input == 1 && (a93 == 6 && cf == 1))) && a102 == 7) {
                    cf = 0;
                    a102 = 8;
                    a146 = 33;
                    a289 = 36;
                    a195 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a130 == 7 && (a93 == 6 && cf == 1) && a102 == 7 && a162 == 8 && input == 7) {
                    cf = 0;
                    a102 = 13;
                    a270 = 35;
                    a73 = 36;
                    a170 = 12;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a162 == 10 && cf == 1) {
              if (a128 == 33 && cf == 1) {{
                  if (a130 == 7 && (a128 == 33 && (cf == 1 && a162 == 10 && a102 == 7) && input == 1)) {
                    cf = 0;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a128 == 33 && (a162 == 10 && (input == 3 && cf == 1 && a102 == 7) && a130 == 7)) {
                    cf = 0;
                    a296 = 35;
                    a102 = 14;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a130 == 8) {{
            if (cf == 1 && a182 == 9) {
              if (cf == 1 && a386 == 9) {{
                  if (a102 == 7 && (a182 == 9 && (a386 == 9 && (input == 6 && cf == 1)) && a130 == 8)) {
                    cf = 0;
                    a170 = 10;
                    a102 = 13;
                    a270 = 35;
                    a55 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a386 == 9 && (a102 == 7 && (cf == 1 && a182 == 9 && a130 == 8)) && input == 2) {
                    cf = 0;
                    a102 = 14;
                    a25 = 32;
                    a310 = 15;
                    a71 = 13;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (a386 == 12 && cf == 1) {{
                  if (a386 == 12 && (a102 == 7 && cf == 1) && a130 == 8 && input == 10 && a182 == 9) {
                    cf = 0;
                    a306 = 11;
                    a102 = 9;
                    a20 = 4;
                    a77 = 5;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 11) {
              if (cf == 1 && a148 == 36) {{
                  if (cf == 1 && a130 == 8 && input == 2 && a148 == 36 && a182 == 11 && a102 == 7) {
                    cf = 0;
                    a176 = 34;
                    a102 = 14;
                    a310 = 11;
                    a0 = 4;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a148 == 36 && (a102 == 7 && (input == 6 && (a130 == 8 && cf == 1 && a182 == 11)))) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a148 == 36 && (cf == 1 && a182 == 11) && a102 == 7 && a130 == 8 && input == 10) {
                    cf = 0;
                    a314 = 32;
                    a102 = 11;
                    a197 = 34;
                    a154 = 14;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 14) {
              if (a52 == 32 && cf == 1) {{
                  if (a102 == 7 && (a130 == 8 && cf == 1) && a182 == 14 && input == 7 && a52 == 32) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a130 == 8 && (cf == 1 && a52 == 32 && input == 4 && a102 == 7 && a182 == 14)) {
                    cf = 0;
                    a40 = 35;
                    a9 = 35;
                    a102 = 10;
                    a98 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a52 == 35) {{
                  if (a182 == 14 && (a52 == 35 && (cf == 1 && a102 == 7) && input == 6 && a130 == 8)) {
                    cf = 0;
                    a130 = 3;
                    a168 = 1;
                    a162 = 9;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a52 == 35 && (a130 == 8 && (input == 2 && cf == 1 && a102 == 7)) && a182 == 14) {
                    cf = 0;
                    a102 = 12;
                    a191 = 35;
                    a232 = 35;
                    a116 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a130 == 9 && cf == 1) {{
            if (cf == 1 && a112 == 33) {
              if (a244 == 34 && cf == 1) {{
                  if (a130 == 9 && (a244 == 34 && (a102 == 7 && cf == 1 && input == 6)) && a112 == 33) {
                    cf = 0;
                    a130 = 10;
                    a143 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (input == 10 && (a130 == 9 && cf == 1)) && a112 == 33 && a244 == 34) {
                    cf = 0;
                    a116 = 8;
                    a87 = 14;
                    a102 = 12;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a244 == 36) {{
                  if (a130 == 9 && (a102 == 7 && (a112 == 33 && (cf == 1 && a244 == 36 && input == 2)))) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a112 == 32) {
              if (a32 == 10 && cf == 1) {{
                  if (input == 1 && (a130 == 9 && cf == 1 && a102 == 7 && a112 == 32 && a32 == 10)) {
                    cf = 0;
                    a116 = 2;
                    a57 = 36;
                    a102 = 12;
                    a7 = 5;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a112 == 34 && cf == 1) {
              if (a168 == 1 && cf == 1) {{
                  if (a168 == 1 && (a112 == 34 && (input == 2 && (a130 == 9 && cf == 1)) && a102 == 7)) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (input == 3 && (a130 == 9 && (cf == 1 && a112 == 34) && a102 == 7 && a168 == 1)) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (cf == 1 && a130 == 9 && a168 == 1 && a112 == 34 && input == 4 && a102 == 7) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
              if (a168 == 2 && cf == 1) {{
                  if (input == 7 && (a130 == 9 && (a112 == 34 && (a102 == 7 && (cf == 1 && a168 == 2))))) {
                    cf = 0;
                    a102 = 14;
                    a125 = 11;
                    a310 = 9;
                    a69 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a168 == 2 && (a112 == 34 && (a130 == 9 && (a102 == 7 && (input == 6 && cf == 1))))) {
                    cf = 0;
                    a102 = 9;
                    a62 = 3;
                    a20 = 7;
                    a171 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a112 == 34 && (a130 == 9 && (a102 == 7 && cf == 1 && input == 8 && a168 == 2))) {
                    cf = 0;
                    a102 = 10;
                    a40 = 33;
                    a121 = 34;
                    a178 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a168 == 7) {{
                  if (a130 == 9 && (a102 == 7 && (a168 == 7 && (input == 4 && cf == 1)) && a112 == 34)) {
                    cf = 0;
                    a9 = 34;
                    a102 = 10;
                    a40 = 35;
                    a98 = 8;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (input == 8 && (a130 == 9 && (cf == 1 && a112 == 34) && a168 == 7))) {
                    cf = 0;
                    a112 = 32;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (input == 1 && (a168 == 7 && cf == 1 && a130 == 9 && a112 == 34))) {
                    cf = 0;
                    a102 = 11;
                    a314 = 34;
                    a106 = 35;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a168 == 7 && (a130 == 9 && (a112 == 34 && (input == 10 && cf == 1))) && a102 == 7) {
                    cf = 0;
                    a102 = 8;
                    a289 = 34;
                    a57 = 36;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a130 == 9 && (a168 == 7 && (input == 6 && cf == 1) && a112 == 34 && a102 == 7)) {
                    cf = 0;
                    a130 = 10;
                    a89 = 32;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a112 == 35 && cf == 1) {
              if (a116 == 4 && cf == 1) {{
                  if (a102 == 7 && (a112 == 35 && cf == 1) && a130 == 9 && a116 == 4 && input == 6) {
                    cf = 0;
                    a102 = 13;
                    a270 = 34;
                    a9 = 32;
                    a183 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a116 == 5) {{
                  if (cf == 1 && a116 == 5 && a112 == 35 && a102 == 7 && input == 3 && a130 == 9) {
                    cf = 0;
                    a116 = 8;
                    a385 = 33;
                    a102 = 12;
                    a87 = 17;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a130 == 9 && (cf == 1 && a116 == 5 && a102 == 7 && input == 7 && a112 == 35)) {
                    cf = 0;
                    a116 = 6;
                    a22 = 36;
                    a102 = 12;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a116 == 5 && (a130 == 9 && (cf == 1 && a102 == 7 && a112 == 35) && input == 10)) {
                    cf = 0;
                    a116 = 6;
                    a22 = 36;
                    a102 = 12;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a112 == 36) {
              if (cf == 1 && a121 == 33) {{
                  if (input == 7 && (cf == 1 && a121 == 33) && a102 == 7 && a112 == 36 && a130 == 9) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (a121 == 34 && cf == 1) {{
                  if (a121 == 34 && (a112 == 36 && (a102 == 7 && (cf == 1 && a130 == 9 && input == 7)))) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a130 == 9 && (a121 == 34 && (cf == 1 && input == 10) && a102 == 7 && a112 == 36)) {
                    cf = 0;
                    a20 = 5;
                    a249 = 34;
                    a102 = 9;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a130 == 9 && (a112 == 36 && (a102 == 7 && cf == 1)) && input == 1 && a121 == 34) {
                    cf = 0;
                    a102 = 12;
                    a22 = 36;
                    a116 = 6;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a130 == 10) {{
            if (cf == 1 && a143 == 5) {
              if (cf == 1 && a49 == 6) {{
                  if (a49 == 6 && (input == 3 && (a102 == 7 && cf == 1 && a143 == 5) && a130 == 10)) {
                    cf = 0;
                    a102 = 12;
                    a57 = 36;
                    a116 = 2;
                    a7 = 4;
                    output = 19;
                    fflush(stdout);
                  }
                  if (cf == 1 && a102 == 7 && input == 8 && a130 == 10 && a143 == 5 && a49 == 6) {
                    cf = 0;
                    a130 = 3;
                    a168 = 2;
                    a86 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (a49 == 7 && cf == 1) {{
                  if (a102 == 7 && (a49 == 7 && cf == 1 && a143 == 5 && a130 == 10) && input == 7) {
                    cf = 0;
                    a49 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a49 == 7 && (cf == 1 && input == 5 && a102 == 7 && a130 == 10) && a143 == 5) {
                    cf = 0;
                    a102 = 14;
                    a310 = 15;
                    a71 = 12;
                    a165 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a49 == 8 && cf == 1) {{
                  if (a49 == 8 && (a143 == 5 && (cf == 1 && a102 == 7)) && a130 == 10 && input == 2) {
                    cf = 0;
                    a270 = 35;
                    a102 = 13;
                    a157 = 36;
                    a170 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a49 == 8 && (a130 == 10 && cf == 1 && a143 == 5) && a102 == 7 && input == 1) {
                    cf = 0;
                    a83 = 34;
                    a314 = 35;
                    a102 = 11;
                    a117 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a49 == 8 && (input == 4 && (a102 == 7 && (cf == 1 && a143 == 5)) && a130 == 10)) {
                    cf = 0;
                    a78 = 36;
                    a314 = 36;
                    a102 = 11;
                    a265 = 3;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (input == 8 && (a130 == 10 && cf == 1 && a49 == 8)) && a143 == 5) {
                    cf = 0;
                    a289 = 32;
                    a182 = 10;
                    a102 = 8;
                    a195 = 10;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a143 == 6) {
              if (cf == 1 && a123 == 32) {{
                  if (a123 == 32 && (cf == 1 && a130 == 10 && a143 == 6) && a102 == 7 && input == 6) {
                    cf = 0;
                    a102 = 12;
                    a74 = 32;
                    a116 = 7;
                    a28 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (cf == 1 && a123 == 32 && input == 3 && a130 == 10 && a102 == 7 && a143 == 6) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 7 && (a123 == 32 && (a143 == 6 && (a102 == 7 && (a130 == 10 && cf == 1))))) {
                    cf = 0;
                    a102 = 12;
                    a87 = 14;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a143 == 7 && cf == 1) {
              if (32 == 32 && cf == 1) {{
                  if (a143 == 7 && (32 == 32 && cf == 1) && a130 == 10 && input == 2 && a102 == 7) {
                    cf = 0;
                    a112 = 33;
                    a244 = 34;
                    a130 = 9;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a143 == 8) {
              if (cf == 1 && a99 == 6) {{
                  if (a102 == 7 && (a99 == 6 && (input == 1 && cf == 1) && a130 == 10 && a143 == 8)) {
                    cf = 0;
                    a102 = 8;
                    a70 = 32;
                    a289 = 32;
                    a182 = 16;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a99 == 7) {{
                  if (input == 6 && cf == 1 && a130 == 10 && a99 == 7 && a102 == 7 && a143 == 8) {
                    cf = 0;
                    a183 = 4;
                    a102 = 9;
                    a20 = 1;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a143 == 8 && (a99 == 7 && cf == 1) && input == 1 && a130 == 10 && a102 == 7) {
                    cf = 0;
                    a102 = 9;
                    a183 = 4;
                    a20 = 1;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a130 == 10 && (input == 8 && (a99 == 7 && (a143 == 8 && (cf == 1 && a102 == 7))))) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a143 == 8 && (input == 4 && (a99 == 7 && cf == 1) && a130 == 10))) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a143 == 9) {
              if (a89 == 32 && cf == 1) {{
                  if (a143 == 9 && (cf == 1 && a130 == 10 && a102 == 7 && input == 6) && a89 == 32) {
                    cf = 0;
                    a102 = 9;
                    a306 = 4;
                    a20 = 4;
                    a28 = 9;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
              if (a89 == 36 && cf == 1) {{
                  if (a130 == 10 && (a102 == 7 && (input == 4 && (cf == 1 && a143 == 9)) && a89 == 36)) {
                    cf = 0;
                    a112 = 34;
                    a130 = 9;
                    a168 = 2;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 1 && (a143 == 9 && (cf == 1 && a130 == 10) && a89 == 36 && a102 == 7)) {
                    cf = 0;
                    a122 = 34;
                    a102 = 14;
                    a310 = 13;
                    a242 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a130 == 10 && (cf == 1 && a143 == 9 && input == 5 && a89 == 36))) {
                    cf = 0;
                    a102 = 12;
                    a116 = 4;
                    a67 = 36;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a143 == 10 && cf == 1) {
              if (a49 == 4 && cf == 1) {{
                  if (a143 == 10 && (a102 == 7 && (input == 7 && (cf == 1 && a49 == 4)) && a130 == 10)) {
                    cf = 0;
                    a67 = 35;
                    a385 = 36;
                    a40 = 32;
                    a102 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a49 == 9) {{
                  if (a143 == 10 && cf == 1 && a130 == 10 && a102 == 7 && input == 4 && a49 == 9) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a130 == 10 && (input == 3 && cf == 1) && a102 == 7 && a49 == 9 && a143 == 10) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 6 && (a130 == 10 && (a143 == 10 && cf == 1 && a102 == 7) && a49 == 9)) {
                    cf = 0;
                    a91 = 32;
                    a296 = 35;
                    a102 = 14;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (cf == 1 && a102 == 8) {
        if (cf == 1 && a289 == 33) {{
            if (cf == 1 && a75 == 33) {
              if (a67 == 35 && cf == 1) {{
                  if (a67 == 35 && (a289 == 33 && (input == 5 && cf == 1 && a75 == 33)) && a102 == 8) {
                    cf = 0;
                    a270 = 32;
                    a35 = 36;
                    a102 = 13;
                    a69 = 8;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a75 == 33 && cf == 1 && a67 == 35 && a102 == 8 && a289 == 33 && input == 1) {
                    cf = 0;
                    a130 = 10;
                    a89 = 36;
                    a102 = 7;
                    a143 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 8 && cf == 1 && a67 == 35 && a289 == 33 && a75 == 33 && input == 10) {
                    cf = 0;
                    a129 = 32;
                    a78 = 35;
                    a102 = 9;
                    a20 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a75 == 34) {
              if (a141 == 7 && cf == 1) {{
                  if (input == 6 && (a289 == 33 && (a141 == 7 && (a75 == 34 && (a102 == 8 && cf == 1))))) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a75 == 34 && (input == 3 && cf == 1 && a102 == 8 && a141 == 7) && a289 == 33) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a75 == 35 && cf == 1) {
              if (cf == 1 && a20 == 2) {{
                  if (a289 == 33 && (a20 == 2 && (a102 == 8 && cf == 1) && a75 == 35) && input == 1) {
                    cf = 0;
                    a270 = 32;
                    a81 = 32;
                    a102 = 13;
                    a69 = 9;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a289 == 33 && (input == 7 && (a102 == 8 && (cf == 1 && a75 == 35) && a20 == 2))) {
                    cf = 0;
                    a112 = 34;
                    a102 = 7;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 8 && (a289 == 33 && (cf == 1 && a75 == 35) && a20 == 2 && a102 == 8)) {
                    cf = 0;
                    a314 = 34;
                    a106 = 35;
                    a102 = 11;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (a20 == 7 && cf == 1) {{
                  if (a289 == 33 && (a75 == 35 && (a102 == 8 && (cf == 1 && input == 2 && a20 == 7)))) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 10 && (cf == 1 && a75 == 35 && a289 == 33) && a102 == 8 && a20 == 7) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a75 == 36 && cf == 1) {
              if (cf == 1 && a52 == 34) {{
                  if (a52 == 34 && (a102 == 8 && (cf == 1 && a289 == 33)) && a75 == 36 && input == 3) {
                    cf = 0;
                    a20 = 7;
                    a102 = 9;
                    a67 = 36;
                    a62 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a289 == 32) {{
            if (a182 == 9 && cf == 1) {
              if (a36 == 10 && cf == 1) {{
                  if (a102 == 8 && (a182 == 9 && (input == 2 && cf == 1 && a36 == 10) && a289 == 32)) {
                    cf = 0;
                    a289 = 36;
                    a146 = 33;
                    a195 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a182 == 9 && (input == 6 && (a36 == 10 && (a102 == 8 && cf == 1 && a289 == 32)))) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a182 == 10 && cf == 1) {
              if (a195 == 10 && cf == 1) {{
                  if (a289 == 32 && (a182 == 10 && (a102 == 8 && cf == 1 && input == 6)) && a195 == 10) {
                    cf = 0;
                    a232 = 36;
                    a270 = 33;
                    a102 = 12;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a195 == 10 && (input == 4 && (a102 == 8 && (cf == 1 && a182 == 10 && a289 == 32)))) {
                    cf = 0;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a102 == 8 && (input == 8 && (a195 == 10 && (a182 == 10 && (a289 == 32 && cf == 1))))) {
                    cf = 0;
                    a270 = 33;
                    a102 = 12;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 11) {
              if (cf == 1 && a200 == 6) {{
                  if (a289 == 32 && (a102 == 8 && (a182 == 11 && (a200 == 6 && (input == 10 && cf == 1))))) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a200 == 7) {{
                  if (a182 == 11 && (a200 == 7 && (a102 == 8 && (cf == 1 && a289 == 32 && input == 6)))) {
                    cf = 0;
                    a57 = 36;
                    a289 = 34;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a182 == 13 && cf == 1) {
              if (cf == 1 && a3 == 8) {{
                  if (a102 == 8 && (a3 == 8 && (a182 == 13 && cf == 1 && a289 == 32) && input == 8)) {
                    cf = 0;
                    a289 = 36;
                    a146 = 35;
                    a17 = 5;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a3 == 8 && (a289 == 32 && (cf == 1 && a182 == 13 && a102 == 8 && input == 3))) {
                    cf = 0;
                    a270 = 34;
                    a102 = 13;
                    a9 = 34;
                    a47 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (input == 4 && (a289 == 32 && (a182 == 13 && cf == 1) && a3 == 8 && a102 == 8)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 8;
                    a99 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a289 == 32 && (a3 == 8 && (a102 == 8 && (a182 == 13 && cf == 1 && input == 7)))) {
                    cf = 0;
                    a20 = 5;
                    a102 = 9;
                    a249 = 33;
                    a194 = 10;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a3 == 13) {{
                  if (a182 == 13 && (a289 == 32 && (cf == 1 && a3 == 13 && a102 == 8)) && input == 1) {
                    cf = 0;
                    a37 = 35;
                    a130 = 6;
                    a102 = 7;
                    a248 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a102 == 8 && (a289 == 32 && (a182 == 13 && cf == 1) && a3 == 13) && input == 10) {
                    cf = 0;
                    a102 = 12;
                    a116 = 4;
                    a67 = 35;
                    a62 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 14) {
              if (cf == 1 && a184 == 5) {{
                  if (a289 == 32 && (a182 == 14 && (cf == 1 && a184 == 5 && input == 8 && a102 == 8))) {
                    cf = 0;
                    a121 = 33;
                    a102 = 7;
                    a112 = 36;
                    a130 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a182 == 14 && (a184 == 5 && (input == 2 && (cf == 1 && a289 == 32) && a102 == 8))) {
                    cf = 0;
                    a164 = 33;
                    a102 = 12;
                    a74 = 36;
                    a116 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a184 == 10) {{
                  if (a289 == 32 && (a182 == 14 && (a184 == 10 && (cf == 1 && a102 == 8)) && input == 2)) {
                    cf = 0;
                    a314 = 34;
                    a106 = 36;
                    a102 = 11;
                    a60 = 12;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a184 == 10 && cf == 1 && a182 == 14 && input == 3 && a289 == 32 && a102 == 8) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a179 = 33;
                    a125 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 15) {
              if (cf == 1 && a144 == 35) {{
                  if (a102 == 8 && cf == 1 && input == 10 && a144 == 35 && a289 == 32 && a182 == 15) {
                    cf = 0;
                    a314 = 35;
                    a83 = 34;
                    a102 = 11;
                    a117 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a289 == 32 && (a144 == 35 && cf == 1 && input == 5) && a182 == 15 && a102 == 8) {
                    cf = 0;
                    a115 = 35;
                    a102 = 7;
                    a130 = 4;
                    a17 = 8;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a182 == 15 && (a102 == 8 && (a144 == 35 && cf == 1)) && input == 4 && a289 == 32) {
                    cf = 0;
                    a102 = 12;
                    a116 = 4;
                    a67 = 36;
                    a310 = 15;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a182 == 16 && cf == 1) {
              if (a70 == 32 && cf == 1) {{
                  if (input == 6 && (a289 == 32 && (a182 == 16 && cf == 1 && a102 == 8)) && a70 == 32) {
                    cf = 0;
                    a196 = 32;
                    a16 = 34;
                    a314 = 33;
                    a102 = 11;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a70 == 32 && (a102 == 8 && (cf == 1 && input == 8)) && a182 == 16 && a289 == 32) {
                    cf = 0;
                    a102 = 9;
                    a20 = 7;
                    a67 = 36;
                    a62 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (cf == 1 && a182 == 16 && a289 == 32 && input == 10 && a70 == 32 && a102 == 8) {
                    cf = 0;
                    a102 = 7;
                    a143 = 8;
                    a130 = 10;
                    a99 = 6;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a289 == 34) {{
            if (a57 == 33 && cf == 1) {
              if (cf == 1 && a270 == 32) {{
                  if (a102 == 8 && (a270 == 32 && (a57 == 33 && (a289 == 34 && cf == 1))) && input == 3) {
                    cf = 0;
                    a146 = 35;
                    a289 = 36;
                    a17 = 9;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a289 == 34 && (input == 6 && (a102 == 8 && cf == 1 && a57 == 33 && a270 == 32))) {
                    cf = 0;
                    a87 = 12;
                    a102 = 12;
                    a116 = 8;
                    a185 = 8;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a57 == 33 && (a289 == 34 && (a270 == 32 && (input == 7 && cf == 1) && a102 == 8))) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a183 = 4;
                    a17 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 8 && (a289 == 34 && (a57 == 33 && (cf == 1 && input == 8) && a270 == 32))) {
                    cf = 0;
                    a130 = 4;
                    a102 = 7;
                    a115 = 32;
                    a17 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a57 == 32) {
              if (cf == 1 && a335 == 11) {{
                  if (a289 == 34 && (a335 == 11 && (cf == 1 && a57 == 32 && input == 2)) && a102 == 8) {
                    cf = 0;
                    a248 = 14;
                    a130 = 6;
                    a102 = 7;
                    a365 = 12;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a289 == 34 && (cf == 1 && a335 == 11 && a57 == 32 && input == 7) && a102 == 8) {
                    cf = 0;
                    a197 = 32;
                    a102 = 11;
                    a314 = 32;
                    a170 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a57 == 36) {
              if (cf == 1 && a138 == 13) {{
                  if (a138 == 13 && (cf == 1 && a57 == 36 && a289 == 34 && a102 == 8) && input == 7) {
                    cf = 0;
                    a100 = 36;
                    a102 = 7;
                    a130 = 3;
                    a168 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a138 == 15) {{
                  if (a138 == 15 && (a102 == 8 && (cf == 1 && a289 == 34) && input == 6) && a57 == 36) {
                    cf = 0;
                    a17 = 6;
                    a102 = 7;
                    a130 = 4;
                    a143 = 5;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a289 == 34 && (a102 == 8 && (a138 == 15 && (input == 7 && cf == 1 && a57 == 36)))) {
                    cf = 0;
                    a310 = 9;
                    a125 = 12;
                    a102 = 14;
                    a23 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a289 == 34 && (a57 == 36 && (input == 10 && (cf == 1 && a102 == 8)) && a138 == 15)) {
                    cf = 0;
                    a182 = 11;
                    a289 = 32;
                    a200 = 7;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a289 == 35) {{
            if (cf == 1 && a249 == 34) {
              if (cf == 1 && a111 == 8) {{
                  if (a102 == 8 && (input == 3 && (a111 == 8 && (a289 == 35 && (a249 == 34 && cf == 1))))) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a111 == 8 && (a289 == 35 && (a249 == 34 && cf == 1 && input == 6) && a102 == 8)) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a111 == 8 && (input == 8 && (a102 == 8 && cf == 1 && a249 == 34 && a289 == 35))) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a289 == 35 && (a102 == 8 && (cf == 1 && input == 4) && a249 == 34) && a111 == 8) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a249 == 36) {
              if (cf == 1 && a19 == 32) {{
                  if (a102 == 8 && (a289 == 35 && (a249 == 36 && cf == 1 && a19 == 32)) && input == 8) {
                    cf = 0;
                    a270 = 36;
                    a344 = 8;
                    a102 = 13;
                    a153 = 4;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a289 == 35 && (a102 == 8 && (cf == 1 && input == 7) && a249 == 36 && a19 == 32)) {
                    cf = 0;
                    a12 = 34;
                    a20 = 2;
                    a102 = 9;
                    a63 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a249 == 36 && (a19 == 32 && (cf == 1 && a102 == 8 && input == 2) && a289 == 35)) {
                    cf = 0;
                    a385 = 34;
                    a72 = 33;
                    a40 = 32;
                    a102 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a289 == 36) {{
            if (cf == 1 && a146 == 33) {
              if (a195 == 9 && cf == 1) {{
                  if (a289 == 36 && (a146 == 33 && (a102 == 8 && (cf == 1 && input == 3)) && a195 == 9)) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a195 == 9 && (a289 == 36 && (a146 == 33 && (cf == 1 && a102 == 8))) && input == 7) {
                    cf = 0;
                    a102 = 10;
                    a40 = 33;
                    a178 = 7;
                    a271 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a289 == 36 && (cf == 1 && input == 2 && a146 == 33 && a195 == 9 && a102 == 8)) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a195 == 12) {{
                  if (a195 == 12 && (a289 == 36 && (input == 4 && cf == 1 && a146 == 33) && a102 == 8)) {
                    cf = 0;
                    a9 = 35;
                    a102 = 10;
                    a40 = 35;
                    a98 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 6 && (a102 == 8 && (a195 == 12 && (cf == 1 && a146 == 33)) && a289 == 36)) {
                    cf = 0;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a146 == 33 && (a102 == 8 && (a289 == 36 && (cf == 1 && input == 10 && a195 == 12)))) {
                    cf = 0;
                    a296 = 35;
                    a102 = 14;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a289 == 36 && (a195 == 12 && (input == 8 && cf == 1 && a146 == 33) && a102 == 8)) {
                    cf = 0;
                    a40 = 35;
                    a102 = 10;
                    a9 = 35;
                    a98 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a146 == 32) {
              if (cf == 1 && a93 == 7) {{
                  if (input == 3 && (a93 == 7 && (a102 == 8 && (a146 == 32 && (a289 == 36 && cf == 1))))) {
                    cf = 0;
                    a102 = 12;
                    a67 = 32;
                    a116 = 4;
                    a98 = 5;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a146 == 32 && (a93 == 7 && (cf == 1 && input == 4)) && a102 == 8 && a289 == 36) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a146 == 32 && (input == 7 && (a289 == 36 && cf == 1) && a93 == 7) && a102 == 8) {
                    cf = 0;
                    a67 = 32;
                    a116 = 4;
                    a102 = 12;
                    a98 = 5;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 6 && (cf == 1 && a93 == 7 && a146 == 32 && a289 == 36) && a102 == 8) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a146 == 35 && cf == 1) {
              if (cf == 1 && a17 == 5) {{
                  if (a146 == 35 && (a17 == 5 && cf == 1) && input == 10 && a289 == 36 && a102 == 8) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a146 == 35 && (a17 == 5 && cf == 1 && a102 == 8 && input == 7 && a289 == 36)) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a17 == 9 && cf == 1) {{
                  if (a102 == 8 && (cf == 1 && input == 6 && a289 == 36) && a146 == 35 && a17 == 9) {
                    cf = 0;
                    a102 = 12;
                    a5 = 34;
                    a19 = 35;
                    a116 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 9 && cf == 1) {
        if (a20 == 1 && cf == 1) {{
            if (cf == 1 && a183 == 4) {
              if (cf == 1 && a17 == 7) {{
                  if (input == 6 && (a20 == 1 && (cf == 1 && a183 == 4) && a17 == 7) && a102 == 9) {
                    cf = 0;
                    a102 = 14;
                    a126 = 33;
                    a190 = 34;
                    a310 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a20 == 1 && (input == 7 && (cf == 1 && a17 == 7))) && a183 == 4) {
                    cf = 0;
                    a289 = 32;
                    a182 = 13;
                    a102 = 8;
                    a3 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a17 == 7 && (input == 10 && (a102 == 9 && (a20 == 1 && (cf == 1 && a183 == 4))))) {
                    cf = 0;
                    a102 = 10;
                    a385 = 32;
                    a40 = 32;
                    a323 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a17 == 7 && (a102 == 9 && (input == 4 && (a20 == 1 && (cf == 1 && a183 == 4))))) {
                    cf = 0;
                    a20 = 7;
                    a62 = 3;
                    a171 = 13;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
              if (a17 == 10 && cf == 1) {{
                  if (a20 == 1 && (a17 == 10 && (a102 == 9 && cf == 1 && a183 == 4)) && input == 10) {
                    cf = 0;
                    a270 = 34;
                    a102 = 13;
                    a9 = 32;
                    a183 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 1 && (a17 == 10 && (cf == 1 && a20 == 1) && a183 == 4) && a102 == 9) {
                    cf = 0;
                    a130 = 6;
                    a37 = 35;
                    a102 = 7;
                    a248 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a183 == 5 && cf == 1) {
              if (33 == 33 && cf == 1) {{
                  if (a102 == 9 && (a183 == 5 && (input == 8 && (cf == 1 && a20 == 1 && 33 == 33)))) {
                    cf = 0;
                    a12 = 35;
                    a20 = 2;
                    a138 = 9;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a183 == 5 && (33 == 33 && (input == 5 && cf == 1) && a20 == 1) && a102 == 9) {
                    cf = 0;
                    a116 = 8;
                    a87 = 11;
                    a102 = 12;
                    a364 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a20 == 1 && cf == 1 && input == 2 && 33 == 33 && a183 == 5 && a102 == 9) {
                    cf = 0;
                    a102 = 7;
                    a168 = 2;
                    a130 = 3;
                    a86 = 7;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a183 == 6) {
              if (cf == 1 && a42 == 14) {{
                  if (a20 == 1 && (a102 == 9 && (input == 1 && (a183 == 6 && cf == 1))) && a42 == 14) {
                    cf = 0;
                    a40 = 36;
                    a24 = 32;
                    a102 = 10;
                    output = 19;
                    fflush(stdout);
                  }
                  if (cf == 1 && input == 4 && a42 == 14 && a183 == 6 && a20 == 1 && a102 == 9) {
                    cf = 0;
                    a271 = 12;
                    a20 = 8;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a20 == 1 && (a183 == 6 && (cf == 1 && input == 2))) && a42 == 14) {
                    cf = 0;
                    a143 = 5;
                    a130 = 10;
                    a102 = 7;
                    a49 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a183 == 7 && cf == 1) {
              if (a165 == 10 && cf == 1) {{
                  if (a165 == 10 && (input == 2 && (a183 == 7 && (a102 == 9 && cf == 1)) && a20 == 1)) {
                    cf = 0;
                    a83 = 33;
                    a102 = 11;
                    a314 = 35;
                    a125 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a165 == 10 && (a183 == 7 && cf == 1) && a102 == 9 && input == 10 && a20 == 1) {
                    cf = 0;
                    a20 = 2;
                    a12 = 32;
                    a87 = 17;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a183 == 7 && (input == 6 && cf == 1) && a165 == 10 && a102 == 9 && a20 == 1) {
                    cf = 0;
                    a385 = 33;
                    a116 = 8;
                    a102 = 12;
                    a87 = 17;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a183 == 8) {
              if (a144 == 35 && cf == 1) {{
                  if (a102 == 9 && (a20 == 1 && (cf == 1 && a144 == 35 && input == 1)) && a183 == 8) {
                    cf = 0;
                    a52 = 34;
                    a289 = 33;
                    a75 = 36;
                    a102 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a20 == 1 && (a102 == 9 && (a183 == 8 && (input == 2 && (a144 == 35 && cf == 1))))) {
                    cf = 0;
                    a310 = 11;
                    a102 = 14;
                    a120 = 32;
                    a0 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 2) {{
            if (cf == 1 && a12 == 33) {
              if (a36 == 13 && cf == 1) {{
                  if (input == 8 && (a102 == 9 && (a20 == 2 && (cf == 1 && a36 == 13) && a12 == 33))) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a56 = 16;
                    a159 = 10;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a20 == 2 && (a36 == 13 && cf == 1 && a12 == 33 && input == 4) && a102 == 9) {
                    cf = 0;
                    a385 = 34;
                    a72 = 35;
                    a40 = 32;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a12 == 32) {
              if (a87 == 15 && cf == 1) {{
                  if (input == 7 && (a87 == 15 && (a12 == 32 && (cf == 1 && a102 == 9)) && a20 == 2)) {
                    cf = 0;
                    a91 = 32;
                    a296 = 35;
                    a102 = 14;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a20 == 2 && (a102 == 9 && (a12 == 32 && (cf == 1 && input == 3) && a87 == 15))) {
                    cf = 0;
                    a102 = 12;
                    a56 = 14;
                    a116 = 6;
                    a49 = 3;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a12 == 32 && (a20 == 2 && (input == 10 && (cf == 1 && a102 == 9) && a87 == 15))) {
                    cf = 0;
                    a296 = 35;
                    a102 = 14;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a12 == 32 && cf == 1) && a20 == 2 && a87 == 15 && input == 6) {
                    cf = 0;
                    a296 = 35;
                    a102 = 14;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a12 == 34 && cf == 1) {
              if (cf == 1 && a63 == 11) {{
                  if (a12 == 34 && (a102 == 9 && (cf == 1 && a20 == 2)) && input == 7 && a63 == 11) {
                    cf = 0;
                    a102 = 7;
                    a130 = 3;
                    a168 = 3;
                    a15 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a63 == 12) {{
                  if (a63 == 12 && (a20 == 2 && (a12 == 34 && cf == 1 && input == 6 && a102 == 9))) {
                    cf = 0;
                    a129 = 36;
                    a20 = 3;
                    a96 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a63 == 12 && (a20 == 2 && (input == 2 && (a102 == 9 && cf == 1)) && a12 == 34)) {
                    cf = 0;
                    a102 = 8;
                    a182 = 9;
                    a289 = 32;
                    a36 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a12 == 35) {
              if (a138 == 9 && cf == 1) {{
                  if (a12 == 35 && (cf == 1 && a20 == 2) && a102 == 9 && input == 6 && a138 == 9) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a12 == 35 && (a138 == 9 && (cf == 1 && input == 4) && a20 == 2) && a102 == 9) {
                    cf = 0;
                    a102 = 12;
                    a270 = 33;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 3) {{
            if (cf == 1 && a129 == 33) {
              if (a344 == 10 && cf == 1) {{
                  if (a129 == 33 && (input == 8 && (cf == 1 && a102 == 9 && a344 == 10)) && a20 == 3) {
                    cf = 0;
                    a190 = 34;
                    a126 = 33;
                    a102 = 14;
                    a310 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a344 == 10 && (a129 == 33 && (input == 1 && (a20 == 3 && cf == 1))))) {
                    cf = 0;
                    a190 = 34;
                    a102 = 14;
                    a126 = 33;
                    a310 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a129 == 33 && (a20 == 3 && cf == 1 && a344 == 10)) && input == 4) {
                    cf = 0;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a344 == 10 && (a102 == 9 && (a129 == 33 && cf == 1 && a20 == 3)) && input == 7) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (a344 == 11 && cf == 1) {{
                  if (a102 == 9 && (a20 == 3 && (cf == 1 && input == 5 && a344 == 11) && a129 == 33)) {
                    cf = 0;
                    a67 = 36;
                    a102 = 12;
                    a116 = 4;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (cf == 1 && a20 == 3 && a129 == 33) && input == 1 && a344 == 11) {
                    cf = 0;
                    a122 = 34;
                    a102 = 14;
                    a310 = 13;
                    a242 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 4 && cf == 1 && a129 == 33 && a20 == 3 && a102 == 9 && a344 == 11) {
                    cf = 0;
                    a176 = 34;
                    a102 = 14;
                    a310 = 16;
                    a364 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a344 == 13) {{
                  if (a102 == 9 && (a344 == 13 && (a20 == 3 && (a129 == 33 && cf == 1))) && input == 10) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a20 == 3 && (a102 == 9 && (cf == 1 && a344 == 13 && a129 == 33 && input == 7))) {
                    cf = 0;
                    a130 = 4;
                    a102 = 7;
                    a17 = 9;
                    a195 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a129 == 32 && cf == 1) {
              if (cf == 1 && a78 == 35) {{
                  if (a78 == 35 && (a129 == 32 && (a102 == 9 && (a20 == 3 && cf == 1))) && input == 5) {
                    cf = 0;
                    a310 = 9;
                    a102 = 14;
                    a125 = 15;
                    a182 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a129 == 32 && (input == 2 && (a102 == 9 && (a78 == 35 && cf == 1))) && a20 == 3) {
                    cf = 0;
                    a310 = 9;
                    a125 = 15;
                    a102 = 14;
                    a182 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a129 == 34 && cf == 1) {
              if (a62 == 10 && cf == 1) {{
                  if (a20 == 3 && (a102 == 9 && (a62 == 10 && (cf == 1 && input == 5 && a129 == 34)))) {
                    cf = 0;
                    a106 = 32;
                    a102 = 11;
                    a314 = 34;
                    a54 = 16;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a20 == 3 && (cf == 1 && a62 == 10 && a102 == 9 && a129 == 34 && input == 10)) {
                    cf = 0;
                    a178 = 5;
                    a102 = 10;
                    a40 = 33;
                    a170 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a129 == 35 && cf == 1) {
              if (cf == 1 && a23 == 15) {{
                  if (a20 == 3 && (a23 == 15 && (a129 == 35 && (cf == 1 && a102 == 9 && input == 1)))) {
                    cf = 0;
                    a314 = 34;
                    a78 = 36;
                    a106 = 33;
                    a102 = 11;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a23 == 15 && (a20 == 3 && (input == 10 && (a129 == 35 && cf == 1)) && a102 == 9)) {
                    cf = 0;
                    a56 = 10;
                    a116 = 6;
                    a102 = 12;
                    a377 = 8;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a129 == 36 && cf == 1) {
              if (cf == 1 && a96 == 12) {{
                  if (a102 == 9 && (a96 == 12 && (input == 8 && cf == 1) && a20 == 3) && a129 == 36) {
                    cf = 0;
                    a102 = 13;
                    a9 = 32;
                    a270 = 34;
                    a183 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 4) {{
            if (cf == 1 && a306 == 4) {
              if (cf == 1 && a28 == 2) {{
                  if (a20 == 4 && (input == 1 && cf == 1 && a306 == 4 && a28 == 2) && a102 == 9) {
                    cf = 0;
                    a289 = 33;
                    a102 = 8;
                    a75 = 34;
                    a141 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (a28 == 9 && cf == 1) {{
                  if (a102 == 9 && (input == 1 && (a306 == 4 && cf == 1)) && a20 == 4 && a28 == 9) {
                    cf = 0;
                    a89 = 32;
                    a102 = 7;
                    a130 = 10;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a20 == 4 && (cf == 1 && a306 == 4) && a28 == 9) && input == 8) {
                    cf = 0;
                    a232 = 36;
                    a102 = 12;
                    a270 = 33;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 3 && (a28 == 9 && (a102 == 9 && (a306 == 4 && cf == 1) && a20 == 4))) {
                    cf = 0;
                    a89 = 32;
                    a102 = 7;
                    a130 = 10;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 10 && (a20 == 4 && (a306 == 4 && (a102 == 9 && cf == 1) && a28 == 9))) {
                    cf = 0;
                    a222 = 34;
                    a102 = 13;
                    a270 = 36;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a306 == 6 && cf == 1) {
              if (a117 == 12 && cf == 1) {{
                  if (a102 == 9 && (a306 == 6 && (a20 == 4 && (a117 == 12 && cf == 1) && input == 2))) {
                    cf = 0;
                    a249 = 33;
                    a20 = 5;
                    a194 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a20 == 4 && (cf == 1 && input == 10) && a102 == 9 && a306 == 6 && a117 == 12) {
                    cf = 0;
                    a74 = 36;
                    a102 = 12;
                    a164 = 33;
                    a116 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a306 == 8) {
              if (32 == 32 && cf == 1) {{
                  if (a102 == 9 && (a20 == 4 && (a306 == 8 && (cf == 1 && 32 == 32) && input == 1))) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (32 == 32 && (input == 3 && (cf == 1 && a102 == 9 && a20 == 4)) && a306 == 8) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a20 == 4 && (a102 == 9 && (input == 7 && (cf == 1 && a306 == 8))) && 32 == 32) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a306 == 9 && cf == 1) {
              if (cf == 1 && a2 == 6) {{
                  if (a306 == 9 && (a102 == 9 && (a20 == 4 && cf == 1) && a2 == 6) && input == 3) {
                    cf = 0;
                    a170 = 14;
                    a102 = 13;
                    a270 = 35;
                    a96 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (a2 == 11 && cf == 1) {{
                  if (a102 == 9 && (a20 == 4 && (a306 == 9 && cf == 1) && input == 5 && a2 == 11)) {
                    cf = 0;
                    a102 = 14;
                    a310 = 15;
                    a296 = 33;
                    a71 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a306 == 11 && cf == 1) {
              if (a77 == 5 && cf == 1) {{
                  if (a102 == 9 && (input == 2 && cf == 1 && a20 == 4) && a306 == 11 && a77 == 5) {
                    cf = 0;
                    a157 = 36;
                    a270 = 35;
                    a102 = 13;
                    a170 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 8 && (a102 == 9 && (cf == 1 && a306 == 11) && a20 == 4 && a77 == 5)) {
                    cf = 0;
                    a289 = 32;
                    a182 = 10;
                    a102 = 8;
                    a195 = 10;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 1 && cf == 1 && a20 == 4 && a77 == 5 && a306 == 11 && a102 == 9) {
                    cf = 0;
                    a183 = 6;
                    a20 = 1;
                    a42 = 14;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a77 == 5 && (a102 == 9 && (input == 4 && cf == 1 && a20 == 4 && a306 == 11))) {
                    cf = 0;
                    a78 = 36;
                    a102 = 11;
                    a314 = 36;
                    a265 = 3;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (a77 == 6 && cf == 1) {{
                  if (a306 == 11 && (input == 7 && (a77 == 6 && (a20 == 4 && cf == 1 && a102 == 9)))) {
                    cf = 0;
                    a24 = 36;
                    a102 = 10;
                    a40 = 36;
                    a184 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a77 == 6 && (a20 == 4 && (cf == 1 && a102 == 9 && input == 6)) && a306 == 11) {
                    cf = 0;
                    a116 = 5;
                    a5 = 32;
                    a102 = 12;
                    a117 = 8;
                    output = 21;
                    fflush(stdout);
                  }
                  if (input == 10 && (a20 == 4 && (a102 == 9 && cf == 1) && a77 == 6 && a306 == 11)) {
                    cf = 0;
                    a129 = 34;
                    a20 = 3;
                    a62 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a306 == 11 && (input == 1 && (a20 == 4 && (a102 == 9 && cf == 1) && a77 == 6))) {
                    cf = 0;
                    a106 = 33;
                    a116 = 6;
                    a102 = 12;
                    a56 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a77 == 7) {{
                  if (input == 6 && (a102 == 9 && (a306 == 11 && cf == 1) && a20 == 4) && a77 == 7) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a306 == 11 && (a20 == 4 && (a77 == 7 && cf == 1 && input == 1)))) {
                    cf = 0;
                    a91 = 36;
                    a296 = 35;
                    a102 = 14;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a77 == 7 && cf == 1 && a20 == 4 && a102 == 9 && a306 == 11 && input == 10) {
                    cf = 0;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a77 == 7 && (a20 == 4 && (input == 8 && cf == 1) && a306 == 11))) {
                    cf = 0;
                    a20 = 1;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 5) {{
            if (a249 == 33 && cf == 1) {
              if (a194 == 6 && cf == 1) {{
                  if (a102 == 9 && (input == 4 && cf == 1) && a20 == 5 && a194 == 6 && a249 == 33) {
                    cf = 0;
                    a102 = 7;
                    a168 = 6;
                    a130 = 3;
                    a242 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                  if (cf == 1 && input == 2 && a20 == 5 && a102 == 9 && a249 == 33 && a194 == 6) {
                    cf = 0;
                    a168 = 6;
                    a102 = 7;
                    a130 = 3;
                    a242 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a194 == 10) {{
                  if (a249 == 33 && (cf == 1 && a194 == 10 && a20 == 5 && a102 == 9) && input == 1) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 36;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a249 == 32 && cf == 1) {
              if (a128 == 33 && cf == 1) {{
                  if (a128 == 33 && (a249 == 32 && cf == 1 && a102 == 9 && input == 10) && a20 == 5) {
                    cf = 0;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 4 && (a128 == 33 && (a102 == 9 && cf == 1 && a20 == 5) && a249 == 32)) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (input == 8 && (cf == 1 && a128 == 33)) && a20 == 5 && a249 == 32) {
                    cf = 0;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a249 == 34 && cf == 1) {
              if (a77 == 11 && cf == 1) {{
                  if (input == 6 && (a77 == 11 && (a249 == 34 && (cf == 1 && a102 == 9)) && a20 == 5)) {
                    cf = 0;
                    a244 = 33;
                    a40 = 34;
                    a119 = 36;
                    a102 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (input == 2 && (a20 == 5 && (a249 == 34 && cf == 1) && a77 == 11))) {
                    cf = 0;
                    a306 = 11;
                    a20 = 4;
                    a77 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 6) {{
            if (a120 == 33 && cf == 1) {
              if (a127 == 10 && cf == 1) {{
                  if (a127 == 10 && (a102 == 9 && (a120 == 33 && cf == 1 && a20 == 6) && input == 6)) {
                    cf = 0;
                    a130 = 4;
                    a102 = 7;
                    a17 = 7;
                    a151 = 4;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a127 == 11) {{
                  if (a20 == 6 && (a120 == 33 && cf == 1) && a127 == 11 && input == 6 && a102 == 9) {
                    cf = 0;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a127 == 11 && (a20 == 6 && (input == 7 && cf == 1) && a102 == 9 && a120 == 33)) {
                    cf = 0;
                    a183 = 4;
                    a20 = 1;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 1 && (cf == 1 && a120 == 33 && a127 == 11 && a102 == 9) && a20 == 6) {
                    cf = 0;
                    a20 = 1;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a120 == 36) {
              if (cf == 1 && a145 == 35) {{
                  if (input == 8 && (a145 == 35 && cf == 1) && a120 == 36 && a102 == 9 && a20 == 6) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a20 == 7 && cf == 1) {{
            if (cf == 1 && a62 == 3) {
              if (cf == 1 && a171 == 12) {{
                  if (cf == 1 && a171 == 12 && input == 10 && a20 == 7 && a102 == 9 && a62 == 3) {
                    cf = 0;
                    a129 = 32;
                    a78 = 35;
                    a20 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (input == 5 && (a62 == 3 && cf == 1)) && a171 == 12 && a20 == 7) {
                    cf = 0;
                    a176 = 34;
                    a310 = 16;
                    a102 = 14;
                    a364 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a20 == 7 && (a62 == 3 && (input == 1 && (a171 == 12 && cf == 1 && a102 == 9)))) {
                    cf = 0;
                    a89 = 36;
                    a130 = 10;
                    a102 = 7;
                    a143 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (a171 == 13 && cf == 1) {{
                  if (a171 == 13 && (a20 == 7 && cf == 1 && a102 == 9) && input == 7 && a62 == 3) {
                    cf = 0;
                    a314 = 34;
                    a102 = 11;
                    a106 = 32;
                    a54 = 17;
                    output = 21;
                    fflush(stdout);
                  }
                  if (input == 4 && (a171 == 13 && (cf == 1 && a102 == 9 && a62 == 3)) && a20 == 7) {
                    cf = 0;
                    a232 = 32;
                    a102 = 12;
                    a198 = 34;
                    a116 = 3;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a62 == 4) {
              if (cf == 1 && a289 == 36) {{
                  if (input == 8 && (a102 == 9 && (a20 == 7 && cf == 1 && a62 == 4 && a289 == 36))) {
                    cf = 0;
                    a314 = 32;
                    a139 = 34;
                    a197 = 36;
                    a102 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a62 == 6 && cf == 1) {
              if (a98 == 11 && cf == 1) {{
                  if (a102 == 9 && (a20 == 7 && (input == 1 && cf == 1 && a98 == 11) && a62 == 6)) {
                    cf = 0;
                    a20 = 8;
                    a341 = 33;
                    a271 = 11;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a62 == 9) {
              if (a332 == 34 && cf == 1) {{
                  if (a102 == 9 && (a20 == 7 && (cf == 1 && a62 == 9 && input == 7) && a332 == 34)) {
                    cf = 0;
                    a130 = 10;
                    a102 = 7;
                    a123 = 32;
                    a143 = 6;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a62 == 10 && cf == 1) {
              if (cf == 1 && a67 == 36) {{
                  if (a20 == 7 && (a62 == 10 && cf == 1 && input == 7 && a102 == 9 && a67 == 36)) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a62 == 10 && (input == 3 && (a20 == 7 && cf == 1 && a102 == 9)) && a67 == 36) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 8) {{
            if (a271 == 7 && cf == 1) {
              if (a126 == 32 && cf == 1) {{
                  if (a20 == 8 && (input == 3 && (a102 == 9 && cf == 1 && a271 == 7) && a126 == 32)) {
                    cf = 0;
                    a146 = 35;
                    a289 = 36;
                    a102 = 8;
                    a17 = 9;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a126 == 32 && (a20 == 8 && (input == 7 && (a271 == 7 && (a102 == 9 && cf == 1))))) {
                    cf = 0;
                    a182 = 13;
                    a102 = 8;
                    a289 = 32;
                    a3 = 13;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 6 && (a102 == 9 && (a126 == 32 && (cf == 1 && a271 == 7 && a20 == 8)))) {
                    cf = 0;
                    a102 = 12;
                    a87 = 12;
                    a116 = 8;
                    a185 = 8;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a271 == 7 && (a20 == 8 && (a126 == 32 && (cf == 1 && input == 8))) && a102 == 9) {
                    cf = 0;
                    a130 = 4;
                    a102 = 7;
                    a115 = 32;
                    a17 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a271 == 11 && cf == 1) {
              if (a341 == 33 && cf == 1) {{
                  if (cf == 1 && a20 == 8 && a102 == 9 && a341 == 33 && a271 == 11 && input == 3) {
                    cf = 0;
                    a20 = 7;
                    a62 = 6;
                    a98 = 11;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a271 == 12) {
              if (a31 == 11 && cf == 1) {{
                  if (a271 == 12 && (a31 == 11 && (a102 == 9 && (cf == 1 && input == 8 && a20 == 8)))) {
                    cf = 0;
                    a102 = 12;
                    a5 = 33;
                    a116 = 5;
                    a28 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a271 == 12 && (a20 == 8 && (input == 6 && (a102 == 9 && cf == 1) && a31 == 11))) {
                    cf = 0;
                    a20 = 6;
                    a120 = 32;
                    a44 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 10 && cf == 1) {
        if (a40 == 33 && cf == 1) {{
            if (a178 == 5 && cf == 1) {
              if (cf == 1 && a170 == 8) {{
                  if (a170 == 8 && (cf == 1 && a178 == 5 && input == 2 && a102 == 10 && a40 == 33)) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a22 = 36;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 4 && (a178 == 5 && (cf == 1 && a170 == 8 && a102 == 10)) && a40 == 33) {
                    cf = 0;
                    a102 = 9;
                    a20 = 3;
                    a129 = 34;
                    a62 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (input == 3 && (a102 == 10 && (a170 == 8 && cf == 1)) && a178 == 5 && a40 == 33) {
                    cf = 0;
                    a249 = 34;
                    a102 = 9;
                    a20 = 5;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 7 && (a170 == 8 && cf == 1 && a178 == 5 && a102 == 10) && a40 == 33) {
                    cf = 0;
                    a20 = 4;
                    a102 = 9;
                    a306 = 11;
                    a77 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a178 == 7 && cf == 1) {
              if (cf == 1 && a271 == 9) {{
                  if (a40 == 33 && (a102 == 10 && (a271 == 9 && (cf == 1 && input == 6 && a178 == 7)))) {
                    cf = 0;
                    a102 = 14;
                    a310 = 11;
                    a0 = 7;
                    a175 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (a40 == 33 && (a271 == 9 && (cf == 1 && input == 8) && a178 == 7))) {
                    cf = 0;
                    a102 = 7;
                    a17 = 9;
                    a130 = 4;
                    a195 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a178 == 7 && (a271 == 9 && (a40 == 33 && cf == 1 && input == 1)) && a102 == 10) {
                    cf = 0;
                    a385 = 36;
                    a40 = 32;
                    a67 = 34;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 7 && (a178 == 7 && (cf == 1 && a40 == 33 && a271 == 9 && a102 == 10))) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 8;
                    a93 = 6;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a178 == 10 && cf == 1) {
              if (a66 == 33 && cf == 1) {{
                  if (input == 6 && (a178 == 10 && (a66 == 33 && cf == 1 && a102 == 10) && a40 == 33)) {
                    cf = 0;
                    a183 = 7;
                    a20 = 1;
                    a102 = 9;
                    a165 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a40 == 32) {{
            if (cf == 1 && a385 == 32) {
              if (cf == 1 && a323 == 5) {{
                  if (a385 == 32 && (a40 == 32 && (input == 4 && cf == 1) && a323 == 5) && a102 == 10) {
                    cf = 0;
                    a102 = 9;
                    a20 = 6;
                    a120 = 33;
                    a127 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a385 == 32 && (a323 == 5 && (a102 == 10 && cf == 1 && input == 3)) && a40 == 32) {
                    cf = 0;
                    a75 = 35;
                    a102 = 8;
                    a289 = 33;
                    a20 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a323 == 5 && (input == 1 && (a385 == 32 && (a40 == 32 && cf == 1 && a102 == 10)))) {
                    cf = 0;
                    a123 = 32;
                    a83 = 35;
                    a314 = 35;
                    a102 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a323 == 5 && (a385 == 32 && (a102 == 10 && cf == 1)) && input == 10 && a40 == 32) {
                    cf = 0;
                    a102 = 9;
                    a20 = 4;
                    a306 = 11;
                    a77 = 7;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a385 == 34) {
              if (cf == 1 && a72 == 33) {{
                  if (a385 == 34 && (cf == 1 && input == 3 && a40 == 32 && a102 == 10) && a72 == 33) {
                    cf = 0;
                    a17 = 9;
                    a130 = 4;
                    a102 = 7;
                    a195 = 12;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a72 == 35) {{
                  if (a102 == 10 && (cf == 1 && a72 == 35 && a385 == 34 && input == 4) && a40 == 32) {
                    cf = 0;
                    a102 = 14;
                    a310 = 15;
                    a166 = 33;
                    a71 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (a72 == 36 && cf == 1) {{
                  if (input == 7 && (a102 == 10 && (cf == 1 && a40 == 32 && a72 == 36) && a385 == 34)) {
                    cf = 0;
                    a130 = 9;
                    a102 = 7;
                    a112 = 34;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a40 == 32 && (input == 8 && (cf == 1 && a102 == 10 && a72 == 36) && a385 == 34)) {
                    cf = 0;
                    a102 = 11;
                    a106 = 35;
                    a314 = 34;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (cf == 1 && a72 == 36 && a40 == 32 && a102 == 10 && a385 == 34 && input == 1) {
                    cf = 0;
                    a270 = 36;
                    a102 = 13;
                    a222 = 34;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a385 == 35) {
              if (cf == 1 && a140 == 9) {{
                  if (a40 == 32 && (a385 == 35 && (a140 == 9 && cf == 1) && a102 == 10 && input == 6)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a144 = 35;
                    a183 = 8;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a385 == 35 && (a40 == 32 && (a102 == 10 && (input == 4 && (a140 == 9 && cf == 1))))) {
                    cf = 0;
                    a9 = 35;
                    a270 = 34;
                    a40 = 33;
                    a102 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a140 == 9 && (a385 == 35 && (input == 8 && (a40 == 32 && cf == 1) && a102 == 10))) {
                    cf = 0;
                    a310 = 9;
                    a190 = 34;
                    a102 = 14;
                    a125 = 14;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a40 == 32 && (a102 == 10 && (a140 == 9 && (a385 == 35 && cf == 1)) && input == 1)) {
                    cf = 0;
                    a20 = 3;
                    a102 = 9;
                    a129 = 35;
                    a23 = 15;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a385 == 36) {
              if (cf == 1 && a67 == 34) {{
                  if (input == 10 && (a67 == 34 && (a102 == 10 && (cf == 1 && a385 == 36)) && a40 == 32)) {
                    cf = 0;
                    a102 = 11;
                    a314 = 34;
                    a106 = 32;
                    a54 = 10;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
              if (a67 == 35 && cf == 1) {{
                  if (a67 == 35 && (a40 == 32 && (cf == 1 && input == 5 && a102 == 10) && a385 == 36)) {
                    cf = 0;
                    a102 = 9;
                    a62 = 6;
                    a20 = 7;
                    a98 = 6;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a385 == 36 && (input == 2 && (a40 == 32 && cf == 1)) && a102 == 10 && a67 == 35) {
                    cf = 0;
                    a197 = 33;
                    a102 = 11;
                    a314 = 32;
                    a86 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (a40 == 32 && (input == 10 && (a385 == 36 && (a67 == 35 && cf == 1))))) {
                    cf = 0;
                    a130 = 8;
                    a102 = 7;
                    a52 = 35;
                    a182 = 14;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a40 == 34 && cf == 1) {{
            if (a244 == 33 && cf == 1) {
              if (a119 == 35 && cf == 1) {{
                  if (a40 == 34 && (a244 == 33 && cf == 1 && input == 6 && a102 == 10 && a119 == 35)) {
                    cf = 0;
                    a310 = 11;
                    a120 = 33;
                    a102 = 14;
                    a0 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a244 == 34) {
              if (34 == 34 && cf == 1) {{
                  if (34 == 34 && (a244 == 34 && cf == 1 && a102 == 10 && input == 5 && a40 == 34)) {
                    cf = 0;
                    a102 = 9;
                    a13 = 35;
                    a20 = 1;
                    a183 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 10 && (a40 == 34 && (34 == 34 && cf == 1 && a244 == 34)) && a102 == 10) {
                    cf = 0;
                    a270 = 32;
                    a289 = 34;
                    a57 = 33;
                    a102 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 8 && (34 == 34 && (a40 == 34 && (a102 == 10 && cf == 1 && a244 == 34)))) {
                    cf = 0;
                    a40 = 33;
                    a178 = 7;
                    a271 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (a40 == 34 && (a244 == 34 && (cf == 1 && 34 == 34 && input == 2)))) {
                    cf = 0;
                    a248 = 9;
                    a102 = 7;
                    a130 = 6;
                    a178 = 5;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (cf == 1 && a40 == 34 && a244 == 34 && 34 == 34 && input == 7)) {
                    cf = 0;
                    a57 = 32;
                    a102 = 8;
                    a289 = 34;
                    a335 = 11;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a244 == 35 && cf == 1) {
              if (cf == 1 && a58 == 34) {{
                  if (input == 4 && (a58 == 34 && (a40 == 34 && cf == 1 && a102 == 10 && a244 == 35))) {
                    cf = 0;
                    a244 = 34;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a40 == 35) {{
            if (a98 == 5 && cf == 1) {
              if (cf == 1 && 36 == 36) {{
                  if (a40 == 35 && (a98 == 5 && (input == 5 && cf == 1)) && a102 == 10 && 36 == 36) {
                    cf = 0;
                    a102 = 14;
                    a310 = 16;
                    a176 = 34;
                    a364 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a98 == 7 && cf == 1) {
              if (a59 == 33 && cf == 1) {{
                  if (a59 == 33 && (a98 == 7 && (cf == 1 && a40 == 35 && input == 1)) && a102 == 10) {
                    cf = 0;
                    a270 = 32;
                    a57 = 33;
                    a289 = 34;
                    a102 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a40 == 35 && (a102 == 10 && (a98 == 7 && (input == 6 && cf == 1)) && a59 == 33)) {
                    cf = 0;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a59 == 33 && (a40 == 35 && cf == 1) && a102 == 10 && input == 4 && a98 == 7) {
                    cf = 0;
                    a115 = 32;
                    a102 = 7;
                    a130 = 4;
                    a17 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a98 == 8) {
              if (cf == 1 && a9 == 35) {{
                  if (a40 == 35 && (cf == 1 && input == 7 && a9 == 35 && a102 == 10) && a98 == 8) {
                    cf = 0;
                    a146 = 33;
                    a102 = 8;
                    a289 = 36;
                    a195 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 2 && (a40 == 35 && (cf == 1 && a98 == 8 && a102 == 10) && a9 == 35)) {
                    cf = 0;
                    a102 = 7;
                    a52 = 32;
                    a130 = 8;
                    a182 = 14;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a98 == 9) {
              if (a51 == 5 && cf == 1) {{
                  if (a98 == 9 && (a102 == 10 && (input == 1 && cf == 1) && a40 == 35) && a51 == 5) {
                    cf = 0;
                    a249 = 34;
                    a20 = 5;
                    a102 = 9;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a40 == 36) {{
            if (cf == 1 && a24 == 33) {
              if (cf == 1 && a135 == 34) {{
                  if (a24 == 33 && (input == 3 && (a40 == 36 && (a135 == 34 && (cf == 1 && a102 == 10))))) {
                    cf = 0;
                    a143 = 8;
                    a102 = 7;
                    a130 = 10;
                    a99 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a24 == 32 && cf == 1) {
              if (cf == 1 && 32 == 32) {{
                  if (a24 == 32 && (cf == 1 && input == 10) && 32 == 32 && a102 == 10 && a40 == 36) {
                    cf = 0;
                    a385 = 33;
                    a40 = 32;
                    a182 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                  if (32 == 32 && (input == 8 && cf == 1 && a40 == 36 && a102 == 10) && a24 == 32) {
                    cf = 0;
                    a5 = 33;
                    a116 = 5;
                    a102 = 12;
                    a28 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a24 == 34) {
              if (a8 == 35 && cf == 1) {{
                  if (a8 == 35 && (cf == 1 && a24 == 34 && a40 == 36 && a102 == 10) && input == 3) {
                    cf = 0;
                    a102 = 11;
                    a78 = 36;
                    a314 = 36;
                    a265 = 6;
                    output = 22;
                    fflush(stdout);
                  }
                  if (input == 1 && (a40 == 36 && (a8 == 35 && (a24 == 34 && cf == 1)) && a102 == 10)) {
                    cf = 0;
                    a130 = 10;
                    a143 = 10;
                    a102 = 7;
                    a49 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a24 == 35) {
              if (a156 == 32 && cf == 1) {{
                  if (a102 == 10 && (a156 == 32 && (a24 == 35 && (cf == 1 && input == 8))) && a40 == 36) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a102 = 7;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a24 == 36) {
              if (cf == 1 && a184 == 9) {{
                  if (a40 == 36 && (a102 == 10 && (input == 1 && (cf == 1 && a184 == 9 && a24 == 36)))) {
                    cf = 0;
                    a102 = 7;
                    a121 = 34;
                    a112 = 36;
                    a130 = 9;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a184 == 9 && (a102 == 10 && (input == 3 && (a24 == 36 && cf == 1))) && a40 == 36) {
                    cf = 0;
                    a40 = 35;
                    a98 = 9;
                    a51 = 5;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (cf == 1 && input == 4 && a40 == 36) && a184 == 9 && a24 == 36) {
                    cf = 0;
                    a314 = 34;
                    a106 = 35;
                    a102 = 11;
                    a4 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 11 && cf == 1) {
        if (cf == 1 && a314 == 33) {{
            if (cf == 1 && a16 == 33) {
              if (a113 == 33 && cf == 1) {{
                  if (a16 == 33 && (a102 == 11 && cf == 1 && a314 == 33 && a113 == 33 && input == 6)) {
                    cf = 0;
                    a102 = 12;
                    a116 = 8;
                    a87 = 14;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a113 == 33 && (a314 == 33 && (cf == 1 && a16 == 33 && input == 4 && a102 == 11))) {
                    cf = 0;
                    a57 = 33;
                    a83 = 33;
                    a102 = 12;
                    a116 = 2;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a16 == 32) {
              if (cf == 1 && a192 == 6) {{
                  if (a192 == 6 && (a16 == 32 && (a314 == 33 && cf == 1 && a102 == 11 && input == 3))) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 15;
                    a182 = 14;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a192 == 7) {{
                  if (a314 == 33 && (a102 == 11 && (a192 == 7 && cf == 1 && a16 == 32 && input == 10))) {
                    cf = 0;
                    a129 = 32;
                    a102 = 9;
                    a78 = 35;
                    a20 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                  if (input == 5 && (a192 == 7 && (a16 == 32 && (a102 == 11 && cf == 1 && a314 == 33)))) {
                    cf = 0;
                    a102 = 14;
                    a176 = 34;
                    a310 = 16;
                    a364 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a16 == 32 && (a102 == 11 && (a314 == 33 && cf == 1 && input == 1 && a192 == 7))) {
                    cf = 0;
                    a20 = 3;
                    a102 = 9;
                    a129 = 33;
                    a344 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a16 == 34) {
              if (a196 == 32 && cf == 1) {{
                  if (a196 == 32 && (a314 == 33 && (a16 == 34 && (cf == 1 && input == 4 && a102 == 11)))) {
                    cf = 0;
                    a126 = 34;
                    a190 = 34;
                    a102 = 14;
                    a310 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a16 == 34 && (a196 == 32 && (a102 == 11 && (cf == 1 && input == 6) && a314 == 33))) {
                    cf = 0;
                    a168 = 3;
                    a102 = 7;
                    a130 = 3;
                    a15 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a16 == 36 && cf == 1) {
              if (cf == 1 && a8 == 35) {{
                  if (input == 1 && (a102 == 11 && cf == 1 && a8 == 35 && a314 == 33 && a16 == 36)) {
                    cf = 0;
                    a12 = 33;
                    a20 = 2;
                    a102 = 9;
                    a36 = 13;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a8 == 36) {{
                  if (input == 4 && (a8 == 36 && (cf == 1 && a314 == 33)) && a16 == 36 && a102 == 11) {
                    cf = 0;
                    a102 = 14;
                    a310 = 10;
                    a190 = 33;
                    a71 = 11;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a16 == 36 && (input == 3 && (a8 == 36 && (a102 == 11 && cf == 1)) && a314 == 33)) {
                    cf = 0;
                    a40 = 32;
                    a102 = 10;
                    a385 = 35;
                    a140 = 9;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a314 == 32 && cf == 1) {{
            if (a197 == 33 && cf == 1) {
              if (a86 == 6 && cf == 1) {{
                  if (a102 == 11 && (a197 == 33 && (cf == 1 && a86 == 6 && a314 == 32 && input == 4))) {
                    cf = 0;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (a86 == 6 && (input == 1 && (a197 == 33 && cf == 1))) && a314 == 32) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a86 == 6 && (a314 == 32 && (cf == 1 && input == 2 && a102 == 11 && a197 == 33))) {
                    cf = 0;
                    a310 = 11;
                    a102 = 14;
                    a176 = 34;
                    a0 = 4;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a314 == 32 && (a102 == 11 && (a86 == 6 && (cf == 1 && a197 == 33)) && input == 8)) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a86 == 10) {{
                  if (a197 == 33 && (a86 == 10 && (cf == 1 && input == 3) && a102 == 11 && a314 == 32)) {
                    cf = 0;
                    a40 = 32;
                    a102 = 10;
                    a385 = 35;
                    a140 = 9;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 4 && (cf == 1 && a102 == 11 && a314 == 32) && a197 == 33 && a86 == 10) {
                    cf = 0;
                    a249 = 36;
                    a289 = 35;
                    a19 = 32;
                    a102 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a86 == 10 && (input == 5 && (a197 == 33 && cf == 1)) && a102 == 11 && a314 == 32) {
                    cf = 0;
                    a102 = 13;
                    a70 = 36;
                    a270 = 35;
                    a170 = 9;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a197 == 32) {
              if (cf == 1 && a170 == 12) {{
                  if (input == 3 && (a314 == 32 && (a197 == 32 && cf == 1 && a170 == 12)) && a102 == 11) {
                    cf = 0;
                    a102 = 7;
                    a130 = 6;
                    a248 = 8;
                    a335 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a314 == 32 && (a102 == 11 && (cf == 1 && a170 == 12) && input == 7 && a197 == 32)) {
                    cf = 0;
                    a190 = 35;
                    a270 = 34;
                    a9 = 36;
                    a102 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a170 == 12 && (a102 == 11 && (a197 == 32 && cf == 1) && a314 == 32) && input == 10) {
                    cf = 0;
                    a57 = 35;
                    a102 = 12;
                    a78 = 35;
                    a116 = 2;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 4 && (a314 == 32 && (cf == 1 && a102 == 11 && a170 == 12 && a197 == 32))) {
                    cf = 0;
                    a249 = 32;
                    a128 = 33;
                    a102 = 9;
                    a20 = 5;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a197 == 35 && cf == 1) {
              if (cf == 1 && a140 == 13) {{
                  if (a314 == 32 && (a197 == 35 && (a140 == 13 && (cf == 1 && a102 == 11) && input == 10))) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a35 = 34;
                    a69 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a314 == 32 && (a102 == 11 && (a140 == 13 && cf == 1 && a197 == 35 && input == 3))) {
                    cf = 0;
                    a197 = 33;
                    a86 = 6;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a197 == 36 && cf == 1) {
              if (a139 == 34 && cf == 1) {{
                  if (a139 == 34 && (a197 == 36 && (a314 == 32 && (cf == 1 && a102 == 11 && input == 1)))) {
                    cf = 0;
                    a78 = 35;
                    a314 = 36;
                    a126 = 35;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a197 == 36 && (input == 8 && cf == 1) && a102 == 11 && a314 == 32 && a139 == 34) {
                    cf = 0;
                    a310 = 15;
                    a197 = 35;
                    a102 = 14;
                    a71 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a314 == 32 && (a102 == 11 && (input == 10 && (cf == 1 && a197 == 36))) && a139 == 34) {
                    cf = 0;
                    a102 = 8;
                    a289 = 34;
                    a57 = 36;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (cf == 1 && a139 == 34 && a314 == 32 && input == 6) && a197 == 36) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a89 = 32;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a314 == 34 && cf == 1) {{
            if (cf == 1 && a106 == 33) {
              if (cf == 1 && a78 == 36) {{
                  if (a314 == 34 && (a78 == 36 && (cf == 1 && a106 == 33) && a102 == 11 && input == 4)) {
                    cf = 0;
                    a232 = 34;
                    a102 = 12;
                    a116 = 3;
                    a344 = 14;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (a106 == 33 && (input == 1 && (a314 == 34 && cf == 1)) && a78 == 36)) {
                    cf = 0;
                    a83 = 35;
                    a314 = 35;
                    a123 = 32;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a106 == 32 && cf == 1) {
              if (a54 == 10 && cf == 1) {{
                  if (a314 == 34 && (input == 8 && cf == 1) && a54 == 10 && a102 == 11 && a106 == 32) {
                    cf = 0;
                    a244 = 34;
                    a40 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a314 == 34 && (a102 == 11 && (cf == 1 && a106 == 32) && a54 == 10) && input == 3) {
                    cf = 0;
                    a67 = 34;
                    a385 = 36;
                    a40 = 32;
                    a102 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a54 == 16) {{
                  if (a106 == 32 && cf == 1 && a102 == 11 && input == 5 && a314 == 34 && a54 == 16) {
                    cf = 0;
                    a20 = 5;
                    a249 = 34;
                    a102 = 9;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a314 == 34 && (a102 == 11 && (a106 == 32 && (cf == 1 && input == 2 && a54 == 16)))) {
                    cf = 0;
                    a20 = 5;
                    a249 = 34;
                    a102 = 9;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a54 == 17) {{
                  if (a102 == 11 && (a54 == 17 && (input == 10 && (a106 == 32 && (a314 == 34 && cf == 1))))) {
                    cf = 0;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a54 == 17 && (a102 == 11 && (a106 == 32 && (a314 == 34 && cf == 1)) && input == 8)) {
                    cf = 0;
                    a102 = 14;
                    a91 = 36;
                    a296 = 35;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a54 == 17 && (input == 1 && (a314 == 34 && cf == 1 && a102 == 11) && a106 == 32)) {
                    cf = 0;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a54 == 17 && (cf == 1 && a314 == 34) && a106 == 32 && input == 2 && a102 == 11) {
                    cf = 0;
                    a102 = 9;
                    a20 = 7;
                    a62 = 3;
                    a171 = 13;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a106 == 35 && cf == 1) {
              if (a4 == 7 && cf == 1) {{
                  if (a314 == 34 && (cf == 1 && a102 == 11 && a4 == 7 && a106 == 35) && input == 8) {
                    cf = 0;
                    a385 = 34;
                    a72 = 36;
                    a40 = 32;
                    a102 = 10;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a4 == 10) {{
                  if (a106 == 35 && (a102 == 11 && cf == 1 && input == 1) && a314 == 34 && a4 == 10) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (a314 == 34 && (cf == 1 && input == 6) && a106 == 35 && a4 == 10)) {
                    cf = 0;
                    a24 = 36;
                    a102 = 10;
                    a40 = 36;
                    a184 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a4 == 10 && (a314 == 34 && cf == 1 && input == 8 && a106 == 35) && a102 == 11) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a106 == 36) {
              if (a60 == 12 && cf == 1) {{
                  if (a102 == 11 && (input == 5 && cf == 1 && a60 == 12 && a106 == 36) && a314 == 34) {
                    cf = 0;
                    a102 = 13;
                    a270 = 32;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 3 && (cf == 1 && a314 == 34 && a102 == 11) && a106 == 36 && a60 == 12) {
                    cf = 0;
                    a102 = 12;
                    a87 = 14;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a106 == 36 && (a314 == 34 && (a60 == 12 && (input == 6 && (cf == 1 && a102 == 11))))) {
                    cf = 0;
                    a289 = 32;
                    a102 = 8;
                    a182 = 14;
                    a184 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a106 == 36 && (a102 == 11 && (cf == 1 && input == 2) && a314 == 34) && a60 == 12) {
                    cf = 0;
                    a102 = 8;
                    a182 = 14;
                    a289 = 32;
                    a184 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a314 == 35) {{
            if (a83 == 33 && cf == 1) {
              if (cf == 1 && a125 == 10) {{
                  if (a102 == 11 && (input == 4 && (cf == 1 && a83 == 33 && a314 == 35 && a125 == 10))) {
                    cf = 0;
                    a40 = 33;
                    a102 = 10;
                    a66 = 33;
                    a178 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a83 == 34) {
              if (a117 == 10 && cf == 1) {{
                  if (a83 == 34 && (a117 == 10 && (a102 == 11 && (cf == 1 && a314 == 35)) && input == 2)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 5;
                    a49 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 1 && (a314 == 35 && (a102 == 11 && (a117 == 10 && cf == 1)) && a83 == 34)) {
                    cf = 0;
                    a271 = 12;
                    a102 = 9;
                    a20 = 8;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 4 && (a83 == 34 && (cf == 1 && a102 == 11)) && a117 == 10 && a314 == 35) {
                    cf = 0;
                    a102 = 9;
                    a20 = 8;
                    a271 = 12;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a117 == 11) {{
                  if (a83 == 34 && (a117 == 11 && (a102 == 11 && cf == 1 && input == 7) && a314 == 35)) {
                    cf = 0;
                    a116 = 8;
                    a102 = 12;
                    a87 = 13;
                    a159 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 4 && (a83 == 34 && cf == 1 && a117 == 11 && a314 == 35) && a102 == 11) {
                    cf = 0;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a314 == 35 && (a117 == 11 && (cf == 1 && a102 == 11) && input == 2) && a83 == 34) {
                    cf = 0;
                    a289 = 32;
                    a144 = 35;
                    a102 = 8;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 3 && (a314 == 35 && (a117 == 11 && cf == 1 && a83 == 34)) && a102 == 11) {
                    cf = 0;
                    a87 = 13;
                    a102 = 12;
                    a116 = 8;
                    a159 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a117 == 13 && cf == 1) {{
                  if (a83 == 34 && (a117 == 13 && (cf == 1 && a102 == 11)) && input == 7 && a314 == 35) {
                    cf = 0;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a117 == 14 && cf == 1) {{
                  if (a83 == 34 && (a102 == 11 && cf == 1) && a117 == 14 && input == 8 && a314 == 35) {
                    cf = 0;
                    a102 = 13;
                    a344 = 8;
                    a270 = 36;
                    a153 = 8;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 10 && (a83 == 34 && cf == 1 && a102 == 11 && a314 == 35) && a117 == 14) {
                    cf = 0;
                    a24 = 35;
                    a40 = 36;
                    a156 = 32;
                    a102 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a314 == 35 && (input == 3 && (a102 == 11 && cf == 1 && a83 == 34) && a117 == 14)) {
                    cf = 0;
                    a310 = 15;
                    a102 = 14;
                    a166 = 33;
                    a71 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 1 && (a117 == 14 && (a83 == 34 && (a314 == 35 && cf == 1 && a102 == 11)))) {
                    cf = 0;
                    a40 = 36;
                    a135 = 34;
                    a24 = 33;
                    a102 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a83 == 35 && cf == 1) {
              if (a123 == 32 && cf == 1) {{
                  if (a123 == 32 && (input == 3 && (a314 == 35 && (a102 == 11 && (a83 == 35 && cf == 1))))) {
                    cf = 0;
                    a102 = 10;
                    a385 = 32;
                    a40 = 32;
                    a323 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (a123 == 32 && (input == 1 && (a83 == 35 && cf == 1) && a314 == 35))) {
                    cf = 0;
                    a102 = 14;
                    a91 = 36;
                    a296 = 35;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a83 == 35 && (a314 == 35 && (cf == 1 && a102 == 11) && input == 2 && a123 == 32)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a123 == 32 && (a102 == 11 && (a83 == 35 && (cf == 1 && a314 == 35))) && input == 4) {
                    cf = 0;
                    a40 = 32;
                    a385 = 32;
                    a102 = 10;
                    a323 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a314 == 36 && cf == 1) {{
            if (a78 == 32 && cf == 1) {
              if (32 == 32 && cf == 1) {{
                  if (input == 4 && (a78 == 32 && (a102 == 11 && (cf == 1 && a314 == 36))) && 32 == 32) {
                    cf = 0;
                    a102 = 8;
                    a289 = 32;
                    a70 = 32;
                    a182 = 16;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a78 == 34 && cf == 1) {
              if (a104 == 33 && cf == 1) {{
                  if (a104 == 33 && (cf == 1 && a102 == 11) && a78 == 34 && a314 == 36 && input == 8) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (cf == 1 && input == 6 && a104 == 33) && a314 == 36 && a78 == 34) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a78 == 35 && cf == 1) {
              if (a126 == 35 && cf == 1) {{
                  if (a102 == 11 && (a314 == 36 && (input == 8 && (a78 == 35 && cf == 1)) && a126 == 35)) {
                    cf = 0;
                    a102 = 13;
                    a344 = 14;
                    a270 = 36;
                    a194 = 9;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a78 == 36) {
              if (cf == 1 && a265 == 3) {{
                  if (a314 == 36 && (a102 == 11 && (a78 == 36 && (a265 == 3 && cf == 1))) && input == 10) {
                    cf = 0;
                    a5 = 33;
                    a116 = 5;
                    a102 = 12;
                    a28 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a265 == 3 && (cf == 1 && a78 == 36 && a102 == 11) && a314 == 36 && input == 1) {
                    cf = 0;
                    a116 = 5;
                    a102 = 12;
                    a5 = 33;
                    a28 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a314 == 36 && (a265 == 3 && (a102 == 11 && (input == 4 && cf == 1 && a78 == 36)))) {
                    cf = 0;
                    a102 = 12;
                    a270 = 33;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a265 == 6) {{
                  if (input == 7 && cf == 1 && a314 == 36 && a265 == 6 && a78 == 36 && a102 == 11) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a265 == 6 && (a314 == 36 && (cf == 1 && input == 6 && a78 == 36) && a102 == 11)) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a265 == 6 && (a102 == 11 && cf == 1) && a78 == 36 && input == 3 && a314 == 36) {
                    cf = 0;
                    a102 = 14;
                    a91 = 32;
                    a296 = 35;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a265 == 6 && (a102 == 11 && cf == 1 && a78 == 36) && input == 10 && a314 == 36) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (cf == 1 && a102 == 12) {
        if (cf == 1 && a116 == 2) {{
            if (cf == 1 && a57 == 33) {
              if (a83 == 33 && cf == 1) {{
                  if (cf == 1 && a116 == 2 && a83 == 33 && a57 == 33 && input == 2 && a102 == 12) {
                    cf = 0;
                    a130 = 4;
                    a17 = 4;
                    a102 = 7;
                    a21 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a83 == 33 && (a102 == 12 && (a116 == 2 && cf == 1 && a57 == 33) && input == 8)) {
                    cf = 0;
                    a198 = 34;
                    a232 = 33;
                    a116 = 3;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a83 == 33 && (a116 == 2 && (a102 == 12 && cf == 1 && input == 4 && a57 == 33))) {
                    cf = 0;
                    a16 = 33;
                    a314 = 33;
                    a113 = 33;
                    a102 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 10 && (a102 == 12 && (a57 == 33 && cf == 1 && a83 == 33) && a116 == 2)) {
                    cf = 0;
                    a232 = 33;
                    a198 = 36;
                    a116 = 3;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a83 == 34) {{
                  if (a102 == 12 && (a57 == 33 && cf == 1 && input == 6 && a83 == 34) && a116 == 2) {
                    cf = 0;
                    a125 = 11;
                    a102 = 14;
                    a310 = 9;
                    a69 = 13;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a83 == 34 && (a57 == 33 && (input == 7 && (cf == 1 && a102 == 12)) && a116 == 2)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a83 == 34 && (input == 4 && (cf == 1 && a102 == 12 && a57 == 33)) && a116 == 2) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 11;
                    a69 = 13;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a57 == 35 && cf == 1) {
              if (cf == 1 && a78 == 35) {{
                  if (a78 == 35 && (a102 == 12 && (cf == 1 && a116 == 2 && a57 == 35 && input == 6))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (cf == 1 && a78 == 35 && a102 == 12 && input == 7 && a57 == 35 && a116 == 2) {
                    cf = 0;
                    a102 = 8;
                    a57 = 32;
                    a289 = 34;
                    a335 = 11;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a116 == 2 && (cf == 1 && a102 == 12) && input == 4 && a78 == 35 && a57 == 35) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 1 && (a57 == 35 && cf == 1 && a102 == 12 && a116 == 2 && a78 == 35)) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a57 == 36) {
              if (a7 == 4 && cf == 1) {{
                  if (a102 == 12 && cf == 1 && a7 == 4 && input == 7 && a57 == 36 && a116 == 2) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a7 == 5 && cf == 1) {{
                  if (a7 == 5 && (a57 == 36 && (cf == 1 && input == 4 && a102 == 12) && a116 == 2)) {
                    cf = 0;
                    a130 = 9;
                    a112 = 32;
                    a102 = 7;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a116 == 2 && (a57 == 36 && (cf == 1 && a102 == 12 && a7 == 5) && input == 3)) {
                    cf = 0;
                    a102 = 7;
                    a112 = 34;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a7 == 7 && cf == 1) {{
                  if (a57 == 36 && (a7 == 7 && (a116 == 2 && (input == 1 && cf == 1)) && a102 == 12)) {
                    cf = 0;
                    a102 = 13;
                    a270 = 32;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a116 == 2 && (a57 == 36 && (a102 == 12 && cf == 1)) && input == 3 && a7 == 7) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a116 == 3) {{
            if (cf == 1 && a232 == 33) {
              if (a198 == 34 && cf == 1) {{
                  if (a116 == 3 && (a232 == 33 && (a198 == 34 && cf == 1 && input == 5 && a102 == 12))) {
                    cf = 0;
                    a83 = 33;
                    a57 = 33;
                    a116 = 2;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a232 == 33 && (a198 == 34 && (a102 == 12 && cf == 1) && a116 == 3) && input == 10) {
                    cf = 0;
                    a310 = 9;
                    a125 = 15;
                    a102 = 14;
                    a182 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a198 == 34 && (cf == 1 && a232 == 33 && input == 8 && a102 == 12) && a116 == 3) {
                    cf = 0;
                    a87 = 14;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 7 && (a198 == 34 && (a116 == 3 && (cf == 1 && a232 == 33 && a102 == 12)))) {
                    cf = 0;
                    a116 = 8;
                    a87 = 14;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (a198 == 36 && cf == 1) {{
                  if (a116 == 3 && (a102 == 12 && (cf == 1 && input == 8 && a198 == 36 && a232 == 33))) {
                    cf = 0;
                    a57 = 33;
                    a83 = 33;
                    a116 = 2;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a232 == 32) {
              if (a198 == 34 && cf == 1) {{
                  if (input == 3 && (cf == 1 && a116 == 3 && a198 == 34) && a102 == 12 && a232 == 32) {
                    cf = 0;
                    a20 = 1;
                    a102 = 9;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a232 == 32 && (a116 == 3 && (input == 6 && (a198 == 34 && (cf == 1 && a102 == 12))))) {
                    cf = 0;
                    a102 = 9;
                    a62 = 3;
                    a20 = 7;
                    a171 = 13;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a232 == 34) {
              if (a344 == 14 && cf == 1) {{
                  if (a116 == 3 && (input == 6 && cf == 1) && a344 == 14 && a102 == 12 && a232 == 34) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (input == 3 && (a116 == 3 && cf == 1 && a344 == 14)) && a232 == 34) {
                    cf = 0;
                    a102 = 14;
                    a310 = 11;
                    a0 = 7;
                    a175 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (input == 1 && (a116 == 3 && cf == 1) && a232 == 34 && a344 == 14)) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 4 && (a232 == 34 && (cf == 1 && a102 == 12)) && a344 == 14 && a116 == 3) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a232 == 36) {
              if (a270 == 33 && cf == 1) {{
                  if (a116 == 3 && (a270 == 33 && cf == 1 && input == 3) && a102 == 12 && a232 == 36) {
                    cf = 0;
                    a20 = 8;
                    a102 = 9;
                    a271 = 12;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a232 == 36 && (a116 == 3 && (a270 == 33 && (a102 == 12 && cf == 1)) && input == 5)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 3;
                    a186 = 33;
                    a168 = 5;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a116 == 3 && (input == 7 && (a270 == 33 && cf == 1 && a232 == 36)))) {
                    cf = 0;
                    a102 = 13;
                    a270 = 36;
                    a222 = 34;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a116 == 4) {{
            if (cf == 1 && a67 == 32) {
              if (cf == 1 && a98 == 5) {{
                  if (a116 == 4 && (a67 == 32 && (cf == 1 && a102 == 12) && input == 2) && a98 == 5) {
                    cf = 0;
                    a78 = 34;
                    a314 = 36;
                    a104 = 33;
                    a102 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a98 == 5 && (cf == 1 && a67 == 32)) && a116 == 4 && input == 4) {
                    cf = 0;
                    a102 = 9;
                    a249 = 33;
                    a20 = 5;
                    a194 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (cf == 1 && a98 == 5 && a67 == 32 && input == 8) && a116 == 4) {
                    cf = 0;
                    a289 = 36;
                    a146 = 32;
                    a102 = 8;
                    a93 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a67 == 34 && cf == 1) {
              if (a142 == 8 && cf == 1) {{
                  if (a116 == 4 && (a67 == 34 && (cf == 1 && a102 == 12 && a142 == 8) && input == 3)) {
                    cf = 0;
                    a17 = 10;
                    a102 = 7;
                    a130 = 4;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a116 == 4 && (input == 7 && (a102 == 12 && (a142 == 8 && cf == 1) && a67 == 34))) {
                    cf = 0;
                    a9 = 35;
                    a102 = 10;
                    a40 = 35;
                    a98 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a67 == 34 && (input == 10 && (a102 == 12 && cf == 1 && a116 == 4 && a142 == 8))) {
                    cf = 0;
                    a8 = 35;
                    a40 = 36;
                    a24 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a67 == 35) {
              if (cf == 1 && a62 == 8) {{
                  if (a62 == 8 && (a67 == 35 && (a102 == 12 && (a116 == 4 && cf == 1)) && input == 1)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a183 = 4;
                    a17 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a62 == 8 && (a67 == 35 && cf == 1 && input == 10)) && a116 == 4) {
                    cf = 0;
                    a102 = 14;
                    a190 = 36;
                    a310 = 10;
                    a28 = 8;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a116 == 4 && (a102 == 12 && cf == 1 && input == 8 && a62 == 8 && a67 == 35)) {
                    cf = 0;
                    a9 = 32;
                    a270 = 34;
                    a102 = 13;
                    a183 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a67 == 36 && cf == 1) {
              if (a310 == 12 && cf == 1) {{
                  if (cf == 1 && a67 == 36 && input == 3 && a116 == 4 && a102 == 12 && a310 == 12) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 15;
                    a182 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a67 == 36 && (cf == 1 && input == 1) && a310 == 12 && a116 == 4)) {
                    cf = 0;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a310 == 12 && (a116 == 4 && (a102 == 12 && cf == 1)) && a67 == 36)) {
                    cf = 0;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a310 == 12 && (a102 == 12 && (input == 7 && (cf == 1 && a67 == 36 && a116 == 4)))) {
                    cf = 0;
                    a130 = 10;
                    a123 = 32;
                    a102 = 7;
                    a143 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a310 == 15) {{
                  if (a67 == 36 && (a116 == 4 && (cf == 1 && a102 == 12)) && input == 1 && a310 == 15) {
                    cf = 0;
                    a87 = 14;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a116 == 5 && cf == 1) {{
            if (cf == 1 && a5 == 33) {
              if (a28 == 8 && cf == 1) {{
                  if (a116 == 5 && (input == 5 && (cf == 1 && a102 == 12 && a28 == 8)) && a5 == 33) {
                    cf = 0;
                    a40 = 32;
                    a102 = 10;
                    a385 = 32;
                    a323 = 1;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 10 && (a116 == 5 && (a102 == 12 && (cf == 1 && a28 == 8 && a5 == 33)))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 5;
                    a49 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a5 == 32) {
              if (a117 == 12 && cf == 1) {{
                  if (a5 == 32 && (input == 3 && (a117 == 12 && cf == 1) && a102 == 12 && a116 == 5)) {
                    cf = 0;
                    a102 = 13;
                    a270 = 32;
                    a69 = 14;
                    a390 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a116 == 5 && (cf == 1 && a5 == 32 && a102 == 12 && a117 == 12))) {
                    cf = 0;
                    a289 = 32;
                    a144 = 36;
                    a102 = 8;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a5 == 34) {
              if (a19 == 35 && cf == 1) {{
                  if (a102 == 12 && (cf == 1 && a5 == 34 && input == 2) && a116 == 5 && a19 == 35) {
                    cf = 0;
                    a244 = 34;
                    a40 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a5 == 34 && (a102 == 12 && cf == 1 && a116 == 5 && a19 == 35) && input == 7) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a5 == 35) {
              if (cf == 1 && a173 == 7) {{
                  if (a102 == 12 && (input == 4 && cf == 1 && a5 == 35 && a116 == 5) && a173 == 7) {
                    cf = 0;
                    a40 = 32;
                    a72 = 35;
                    a385 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a116 == 5 && (a102 == 12 && (a173 == 7 && (input == 8 && (cf == 1 && a5 == 35))))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 6;
                    a37 = 34;
                    a248 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a116 == 6) {{
            if (a56 == 10 && cf == 1) {
              if (a377 == 8 && cf == 1) {{
                  if (input == 6 && (a102 == 12 && cf == 1 && a56 == 10 && a377 == 8) && a116 == 6) {
                    cf = 0;
                    a130 = 10;
                    a102 = 7;
                    a143 = 10;
                    a49 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a377 == 8 && (a102 == 12 && (cf == 1 && a56 == 10 && input == 2 && a116 == 6))) {
                    cf = 0;
                    a130 = 10;
                    a143 = 8;
                    a102 = 7;
                    a99 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a56 == 11 && cf == 1) {
              if (cf == 1 && a106 == 33) {{
                  if (a56 == 11 && (a102 == 12 && cf == 1) && input == 5 && a116 == 6 && a106 == 33) {
                    cf = 0;
                    a163 = 34;
                    a310 = 14;
                    a102 = 14;
                    a15 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a116 == 6 && (a106 == 33 && (a102 == 12 && (cf == 1 && input == 3)) && a56 == 11)) {
                    cf = 0;
                    a310 = 13;
                    a102 = 14;
                    a122 = 34;
                    a242 = 6;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a56 == 13 && cf == 1) {
              if (cf == 1 && a22 == 36) {{
                  if (input == 1 && (a56 == 13 && cf == 1) && a102 == 12 && a22 == 36 && a116 == 6) {
                    cf = 0;
                    a168 = 8;
                    a102 = 7;
                    a130 = 3;
                    a30 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a22 == 36 && (input == 9 && cf == 1 && a56 == 13) && a102 == 12 && a116 == 6) {
                    cf = 0;
                    a74 = 36;
                    a164 = 35;
                    a116 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a116 == 6 && (a56 == 13 && (a102 == 12 && (input == 3 && cf == 1))) && a22 == 36) {
                    cf = 0;
                    a249 = 34;
                    a102 = 9;
                    a20 = 5;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a56 == 14) {
              if (cf == 1 && a49 == 3) {{
                  if (a56 == 14 && (a102 == 12 && (a49 == 3 && (input == 8 && cf == 1))) && a116 == 6) {
                    cf = 0;
                    a20 = 2;
                    a102 = 9;
                    a12 = 32;
                    a87 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a116 == 6 && (input == 3 && (a56 == 14 && cf == 1 && a49 == 3) && a102 == 12)) {
                    cf = 0;
                    a130 = 6;
                    a37 = 32;
                    a102 = 7;
                    a248 = 12;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 10 && (a56 == 14 && cf == 1 && a49 == 3 && a116 == 6 && a102 == 12)) {
                    cf = 0;
                    a128 = 33;
                    a130 = 7;
                    a102 = 7;
                    a162 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a56 == 15) {
              if (cf == 1 && a222 == 34) {{
                  if (a102 == 12 && (a116 == 6 && (a222 == 34 && (cf == 1 && a56 == 15 && input == 8)))) {
                    cf = 0;
                    a172 = 32;
                    a70 = 33;
                    a270 = 33;
                    a102 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a56 == 16) {
              if (a159 == 10 && cf == 1) {{
                  if (a159 == 10 && (a56 == 16 && (a102 == 12 && (a116 == 6 && (cf == 1 && input == 1))))) {
                    cf = 0;
                    a87 = 16;
                    a116 = 8;
                    a44 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a116 == 7 && cf == 1) {{
            if (cf == 1 && a74 == 32) {
              if (cf == 1 && a28 == 3) {{
                  if (a28 == 3 && (input == 6 && (a74 == 32 && (a102 == 12 && cf == 1)) && a116 == 7)) {
                    cf = 0;
                    a67 = 35;
                    a289 = 33;
                    a75 = 33;
                    a102 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 7 && (a74 == 32 && (a102 == 12 && cf == 1) && a28 == 3) && a116 == 7) {
                    cf = 0;
                    a310 = 9;
                    a125 = 11;
                    a102 = 14;
                    a69 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a28 == 6) {{
                  if (a74 == 32 && (a116 == 7 && (cf == 1 && a28 == 6) && input == 3) && a102 == 12) {
                    cf = 0;
                    a81 = 35;
                    a102 = 13;
                    a270 = 32;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a116 == 7 && (input == 7 && (a74 == 32 && (a28 == 6 && cf == 1)) && a102 == 12)) {
                    cf = 0;
                    a22 = 36;
                    a116 = 6;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a74 == 36 && cf == 1) {
              if (a164 == 33 && cf == 1) {{
                  if (a74 == 36 && (input == 1 && (a102 == 12 && cf == 1 && a116 == 7 && a164 == 33))) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a164 == 33 && (cf == 1 && a102 == 12) && input == 6 && a116 == 7 && a74 == 36) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a116 == 7 && (a164 == 33 && (a74 == 36 && cf == 1))) && input == 7) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a164 == 33 && (input == 3 && cf == 1 && a116 == 7 && a102 == 12) && a74 == 36) {
                    cf = 0;
                    a182 = 14;
                    a289 = 32;
                    a102 = 8;
                    a184 = 5;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a116 == 8) {{
            if (cf == 1 && a87 == 11) {
              if (a364 == 11 && cf == 1) {{
                  if (cf == 1 && a102 == 12 && a364 == 11 && a116 == 8 && input == 8 && a87 == 11) {
                    cf = 0;
                    a116 = 6;
                    a22 = 36;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 5 && (a364 == 11 && (cf == 1 && a102 == 12) && a87 == 11) && a116 == 8) {
                    cf = 0;
                    a306 = 9;
                    a102 = 9;
                    a20 = 4;
                    a2 = 11;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a87 == 12 && cf == 1) {
              if (a185 == 8 && cf == 1) {{
                  if (a102 == 12 && (input == 4 && (cf == 1 && a185 == 8 && a87 == 12)) && a116 == 8) {
                    cf = 0;
                    a79 = 33;
                    a116 = 9;
                    a154 = 16;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (cf == 1 && a185 == 8 && input == 2 && a116 == 8) && a87 == 12) {
                    cf = 0;
                    a102 = 8;
                    a289 = 32;
                    a182 = 11;
                    a200 = 6;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a185 == 8 && (cf == 1 && a102 == 12 && a87 == 12 && a116 == 8) && input == 10) {
                    cf = 0;
                    a58 = 34;
                    a40 = 34;
                    a244 = 35;
                    a102 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a87 == 13) {
              if (cf == 1 && a159 == 6) {{
                  if (a159 == 6 && (cf == 1 && a87 == 13 && input == 1) && a116 == 8 && a102 == 12) {
                    cf = 0;
                    a332 = 34;
                    a102 = 9;
                    a20 = 7;
                    a62 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a159 == 6 && (a116 == 8 && (a87 == 13 && (cf == 1 && a102 == 12))) && input == 8) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a159 == 6 && (input == 7 && (a87 == 13 && cf == 1) && a102 == 12) && a116 == 8) {
                    cf = 0;
                    a144 = 35;
                    a102 = 8;
                    a289 = 32;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 9 && (a116 == 8 && cf == 1 && a87 == 13 && a102 == 12) && a159 == 6) {
                    cf = 0;
                    a40 = 32;
                    a67 = 36;
                    a385 = 36;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a116 == 8 && (a87 == 13 && cf == 1 && input == 3 && a159 == 6 && a102 == 12)) {
                    cf = 0;
                    a102 = 8;
                    a182 = 14;
                    a289 = 32;
                    a184 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a87 == 14 && cf == 1) {
              if (cf == 1 && a62 == 5) {{
                  if (a116 == 8 && (a102 == 12 && (input == 6 && (a62 == 5 && cf == 1) && a87 == 14))) {
                    cf = 0;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a62 == 5 && (input == 10 && (a116 == 8 && cf == 1) && a102 == 12) && a87 == 14) {
                    cf = 0;
                    a5 = 32;
                    a116 = 5;
                    a117 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a62 == 8) {{
                  if (a102 == 12 && (a87 == 14 && (a116 == 8 && (cf == 1 && a62 == 8)) && input == 2)) {
                    cf = 0;
                    a341 = 34;
                    a20 = 4;
                    a102 = 9;
                    a306 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 5 && (a87 == 14 && (a102 == 12 && (a62 == 8 && cf == 1))) && a116 == 8) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 15;
                    a182 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a87 == 16) {
              if (cf == 1 && a44 == 11) {{
                  if (a116 == 8 && (cf == 1 && a87 == 16 && input == 10 && a102 == 12) && a44 == 11) {
                    cf = 0;
                    a20 = 7;
                    a289 = 36;
                    a102 = 9;
                    a62 = 4;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a44 == 11 && cf == 1 && a87 == 16 && input == 4 && a102 == 12 && a116 == 8) {
                    cf = 0;
                    a222 = 34;
                    a102 = 13;
                    a270 = 36;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 1 && (a87 == 16 && (a102 == 12 && (a44 == 11 && cf == 1) && a116 == 8))) {
                    cf = 0;
                    a102 = 13;
                    a344 = 9;
                    a270 = 36;
                    a161 = 14;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a87 == 17) {
              if (cf == 1 && a385 == 33) {{
                  if (a102 == 12 && (a116 == 8 && (input == 6 && cf == 1) && a385 == 33 && a87 == 17)) {
                    cf = 0;
                    a102 = 11;
                    a83 = 34;
                    a314 = 35;
                    a117 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a116 == 8 && (input == 2 && (a87 == 17 && (cf == 1 && a385 == 33)) && a102 == 12)) {
                    cf = 0;
                    a74 = 32;
                    a116 = 7;
                    a28 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a87 == 17 && (input == 10 && cf == 1) && a385 == 33 && a116 == 8)) {
                    cf = 0;
                    a79 = 35;
                    a146 = 34;
                    a116 = 9;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a87 == 17 && (a385 == 33 && (a102 == 12 && (cf == 1 && a116 == 8)) && input == 3)) {
                    cf = 0;
                    a112 = 35;
                    a130 = 9;
                    a102 = 7;
                    a116 = 5;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a116 == 9 && cf == 1) {{
            if (a79 == 33 && cf == 1) {
              if (a154 == 12 && cf == 1) {{
                  if (a154 == 12 && (a116 == 9 && cf == 1) && a102 == 12 && input == 1 && a79 == 33) {
                    cf = 0;
                    a102 = 9;
                    a126 = 32;
                    a20 = 8;
                    a271 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a154 == 16) {{
                  if (a79 == 33 && (input == 4 && (a116 == 9 && (a154 == 16 && (a102 == 12 && cf == 1))))) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a79 == 34) {
              if (cf == 1 && a106 == 34) {{
                  if (a116 == 9 && (a79 == 34 && (cf == 1 && a102 == 12 && a106 == 34 && input == 10))) {
                    cf = 0;
                    a20 = 2;
                    a102 = 9;
                    a12 = 34;
                    a63 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a79 == 35 && cf == 1) {
              if (cf == 1 && a146 == 34) {{
                  if (a102 == 12 && (a79 == 35 && (a146 == 34 && cf == 1) && a116 == 9 && input == 4)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 3;
                    a168 = 8;
                    a30 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a116 == 9 && (input == 5 && (a79 == 35 && cf == 1) && a102 == 12 && a146 == 34)) {
                    cf = 0;
                    a168 = 8;
                    a130 = 3;
                    a102 = 7;
                    a30 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a116 == 9 && (cf == 1 && a79 == 35 && a102 == 12 && input == 6) && a146 == 34) {
                    cf = 0;
                    a40 = 33;
                    a102 = 10;
                    a178 = 5;
                    a170 = 8;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 13 && cf == 1) {
        if (a270 == 33 && cf == 1) {{
            if (a172 == 32 && cf == 1) {
              if (a70 == 33 && cf == 1) {{
                  if (a172 == 32 && (a270 == 33 && (cf == 1 && a102 == 13) && input == 6) && a70 == 33) {
                    cf = 0;
                    a130 = 10;
                    a89 = 32;
                    a102 = 7;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a270 == 33 && (a172 == 32 && (cf == 1 && input == 1) && a102 == 13) && a70 == 33) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 7;
                    a28 = 3;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 10 && (a172 == 32 && cf == 1 && a70 == 33) && a102 == 13 && a270 == 33) {
                    cf = 0;
                    a102 = 8;
                    a289 = 34;
                    a57 = 36;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a172 == 32 && (a270 == 33 && (a102 == 13 && (input == 8 && (a70 == 33 && cf == 1))))) {
                    cf = 0;
                    a112 = 32;
                    a130 = 9;
                    a102 = 7;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a172 == 35) {
              if (cf == 1 && a164 == 33) {{
                  if (input == 2 && (a102 == 13 && (a172 == 35 && (a270 == 33 && cf == 1) && a164 == 33))) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a164 == 33 && (cf == 1 && a270 == 33 && a102 == 13) && a172 == 35 && input == 10) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a270 == 32) {{
            if (a69 == 8 && cf == 1) {
              if (a35 == 34 && cf == 1) {{
                  if (a102 == 13 && (a69 == 8 && (a270 == 32 && (a35 == 34 && (cf == 1 && input == 1))))) {
                    cf = 0;
                    a130 = 8;
                    a102 = 7;
                    a148 = 36;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a270 == 32 && (a69 == 8 && (a35 == 34 && (input == 3 && (a102 == 13 && cf == 1))))) {
                    cf = 0;
                    a197 = 35;
                    a102 = 11;
                    a314 = 32;
                    a140 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a35 == 36) {{
                  if (a102 == 13 && (a270 == 32 && (a69 == 8 && cf == 1 && a35 == 36 && input == 7))) {
                    cf = 0;
                    a16 = 32;
                    a102 = 11;
                    a314 = 33;
                    a192 = 7;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a69 == 9 && cf == 1) {
              if (a81 == 32 && cf == 1) {{
                  if (a270 == 32 && (a69 == 9 && (a102 == 13 && cf == 1 && a81 == 32) && input == 8)) {
                    cf = 0;
                    a102 = 14;
                    a163 = 32;
                    a113 = 35;
                    a310 = 14;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a81 == 35 && cf == 1) {{
                  if (input == 8 && (a102 == 13 && (a81 == 35 && (a270 == 32 && cf == 1))) && a69 == 9) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a22 = 36;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a270 == 32 && cf == 1 && a69 == 9 && a81 == 35 && input == 5 && a102 == 13) {
                    cf = 0;
                    a87 = 14;
                    a116 = 8;
                    a102 = 12;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a69 == 10 && cf == 1) {
              if (cf == 1 && a9 == 33) {{
                  if (input == 6 && (cf == 1 && a9 == 33 && a102 == 13) && a69 == 10 && a270 == 32) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a69 == 14 && cf == 1) {
              if (cf == 1 && a390 == 6) {{
                  if (a270 == 32 && cf == 1 && input == 10 && a69 == 14 && a102 == 13 && a390 == 6) {
                    cf = 0;
                    a78 = 35;
                    a129 = 32;
                    a102 = 9;
                    a20 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                  if (cf == 1 && a390 == 6 && a270 == 32 && input == 1 && a102 == 13 && a69 == 14) {
                    cf = 0;
                    a344 = 12;
                    a270 = 36;
                    a36 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 13 && (a69 == 14 && (a270 == 32 && cf == 1) && a390 == 6) && input == 5) {
                    cf = 0;
                    a310 = 16;
                    a102 = 14;
                    a176 = 34;
                    a364 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a390 == 9 && cf == 1) {{
                  if (input == 6 && (a102 == 13 && (a69 == 14 && cf == 1 && a390 == 9)) && a270 == 32) {
                    cf = 0;
                    a87 = 14;
                    a116 = 8;
                    a102 = 12;
                    a62 = 5;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a69 == 14 && (a102 == 13 && cf == 1) && a390 == 9 && a270 == 32 && input == 3) {
                    cf = 0;
                    a168 = 1;
                    a130 = 3;
                    a102 = 7;
                    a162 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a390 == 9 && (a270 == 32 && (a69 == 14 && cf == 1)) && a102 == 13 && input == 4) {
                    cf = 0;
                    a57 = 36;
                    a102 = 12;
                    a116 = 2;
                    a7 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a270 == 34) {{
            if (a9 == 32 && cf == 1) {
              if (cf == 1 && a183 == 6) {{
                  if (input == 1 && (a9 == 32 && (a183 == 6 && (a102 == 13 && cf == 1))) && a270 == 34) {
                    cf = 0;
                    a183 = 4;
                    a20 = 1;
                    a102 = 9;
                    a17 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 8 && (a102 == 13 && cf == 1 && a183 == 6 && a9 == 32) && a270 == 34) {
                    cf = 0;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a9 == 34 && cf == 1) {
              if (a47 == 8 && cf == 1) {{
                  if (a47 == 8 && (a9 == 34 && (a270 == 34 && (input == 4 && (cf == 1 && a102 == 13))))) {
                    cf = 0;
                    a270 = 35;
                    a73 = 36;
                    a170 = 12;
                    output = 15;
                    fflush(stdout);
                  }
                  if (cf == 1 && a9 == 34 && input == 8 && a102 == 13 && a47 == 8 && a270 == 34) {
                    cf = 0;
                    a178 = 7;
                    a40 = 33;
                    a102 = 10;
                    a271 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a47 == 13) {{
                  if (a270 == 34 && (a102 == 13 && (a9 == 34 && (cf == 1 && a47 == 13 && input == 2)))) {
                    cf = 0;
                    a121 = 33;
                    a112 = 36;
                    a102 = 7;
                    a130 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a47 == 13 && (input == 8 && (cf == 1 && a102 == 13)) && a9 == 34 && a270 == 34) {
                    cf = 0;
                    a310 = 9;
                    a125 = 15;
                    a102 = 14;
                    a182 = 14;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a9 == 35) {
              if (a40 == 33 && cf == 1) {{
                  if (cf == 1 && a40 == 33 && a9 == 35 && a102 == 13 && input == 8 && a270 == 34) {
                    cf = 0;
                    a289 = 34;
                    a57 = 36;
                    a102 = 8;
                    a138 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a9 == 35 && (a40 == 33 && (a270 == 34 && (input == 10 && cf == 1)) && a102 == 13)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 4;
                    a306 = 6;
                    a117 = 12;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a9 == 36 && cf == 1) {
              if (a190 == 35 && cf == 1) {{
                  if (a102 == 13 && (a190 == 35 && (a270 == 34 && (cf == 1 && a9 == 36)) && input == 8)) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 13 && (a190 == 35 && (a9 == 36 && cf == 1) && a270 == 34 && input == 4)) {
                    cf = 0;
                    a102 = 8;
                    a289 = 34;
                    a57 = 32;
                    a335 = 11;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a270 == 35 && cf == 1) {{
            if (a170 == 7 && cf == 1) {
              if (a157 == 36 && cf == 1) {{
                  if (a170 == 7 && (input == 6 && (a102 == 13 && (a157 == 36 && (cf == 1 && a270 == 35))))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 5;
                    a49 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a170 == 7 && cf == 1 && a270 == 35 && a157 == 36 && a102 == 13 && input == 1) {
                    cf = 0;
                    a20 = 8;
                    a271 = 12;
                    a102 = 9;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a170 == 10) {
              if (a55 == 5 && cf == 1) {{
                  if (a102 == 13 && (a55 == 5 && (a170 == 10 && (cf == 1 && input == 2)) && a270 == 35)) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a170 == 12) {
              if (a73 == 36 && cf == 1) {{
                  if (cf == 1 && a73 == 36 && a102 == 13 && a270 == 35 && input == 4 && a170 == 12) {
                    cf = 0;
                    a178 = 7;
                    a102 = 10;
                    a40 = 33;
                    a271 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (cf == 1 && a170 == 12 && input == 8 && a102 == 13 && a270 == 35 && a73 == 36) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a170 == 14 && cf == 1) {
              if (cf == 1 && a96 == 8) {{
                  if (a270 == 35 && (a96 == 8 && (cf == 1 && a170 == 14) && input == 2) && a102 == 13) {
                    cf = 0;
                    a79 = 33;
                    a102 = 12;
                    a116 = 9;
                    a154 = 12;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a270 == 36) {{
            if (a344 == 8 && cf == 1) {
              if (cf == 1 && a153 == 4) {{
                  if (a153 == 4 && (input == 4 && (a270 == 36 && (cf == 1 && a344 == 8)) && a102 == 13)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 9;
                    a112 = 34;
                    a168 = 1;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a344 == 8 && (a102 == 13 && (a153 == 4 && cf == 1) && input == 2 && a270 == 36)) {
                    cf = 0;
                    a75 = 34;
                    a289 = 33;
                    a102 = 8;
                    a141 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 6 && (a270 == 36 && (a153 == 4 && cf == 1 && a102 == 13)) && a344 == 8) {
                    cf = 0;
                    a190 = 34;
                    a102 = 14;
                    a126 = 34;
                    a310 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a344 == 8 && (cf == 1 && a102 == 13 && a270 == 36 && input == 8 && a153 == 4)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 4;
                    a306 = 8;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a153 == 8) {{
                  if (input == 8 && (a102 == 13 && (cf == 1 && a344 == 8) && a270 == 36 && a153 == 8)) {
                    cf = 0;
                    a102 = 7;
                    a143 = 10;
                    a130 = 10;
                    a49 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a344 == 9 && cf == 1) {
              if (a161 == 14 && cf == 1) {{
                  if (a102 == 13 && (input == 7 && (a344 == 9 && (a270 == 36 && (cf == 1 && a161 == 14))))) {
                    cf = 0;
                    a222 = 34;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 3 && (a270 == 36 && (a102 == 13 && (a344 == 9 && cf == 1) && a161 == 14))) {
                    cf = 0;
                    a80 = 32;
                    a102 = 7;
                    a103 = 34;
                    a130 = 5;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a344 == 12) {
              if (a36 == 9 && cf == 1) {{
                  if (a344 == 12 && (cf == 1 && a270 == 36 && input == 1 && a36 == 9 && a102 == 13)) {
                    cf = 0;
                    a310 = 13;
                    a122 = 34;
                    a102 = 14;
                    a242 = 6;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a344 == 12 && (a36 == 9 && (a102 == 13 && (input == 3 && cf == 1))) && a270 == 36) {
                    cf = 0;
                    a310 = 16;
                    a102 = 14;
                    a176 = 34;
                    a364 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a36 == 10) {{
                  if (a36 == 10 && (a344 == 12 && (cf == 1 && a102 == 13)) && input == 1 && a270 == 36) {
                    cf = 0;
                    a102 = 14;
                    a122 = 34;
                    a310 = 13;
                    a242 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a344 == 12 && (a270 == 36 && (cf == 1 && a36 == 10)) && a102 == 13 && input == 5) {
                    cf = 0;
                    a102 = 12;
                    a67 = 36;
                    a116 = 4;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a344 == 12 && (a102 == 13 && cf == 1) && a270 == 36 && a36 == 10)) {
                    cf = 0;
                    a102 = 14;
                    a310 = 14;
                    a163 = 34;
                    a15 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a344 == 14 && cf == 1) {
              if (a194 == 9 && cf == 1) {{
                  if (a194 == 9 && cf == 1 && a270 == 36 && input == 7 && a102 == 13 && a344 == 14) {
                    cf = 0;
                    a112 = 34;
                    a102 = 7;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a270 == 36 && (a344 == 14 && (a194 == 9 && cf == 1) && a102 == 13) && input == 8) {
                    cf = 0;
                    a314 = 34;
                    a102 = 11;
                    a106 = 35;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a344 == 14 && (cf == 1 && a194 == 9 && input == 1 && a102 == 13) && a270 == 36) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a222 = 34;
                    a56 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a344 == 15 && cf == 1) {
              if (cf == 1 && a222 == 34) {{
                  if (a222 == 34 && (a344 == 15 && (a270 == 36 && (a102 == 13 && cf == 1)) && input == 5)) {
                    cf = 0;
                    a102 = 10;
                    a178 = 5;
                    a40 = 33;
                    a170 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a270 == 36 && (a222 == 34 && (cf == 1 && a102 == 13)) && a344 == 15 && input == 8) {
                    cf = 0;
                    a112 = 34;
                    a102 = 7;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 14 && cf == 1) {
        if (a310 == 9 && cf == 1) {{
            if (a125 == 10 && cf == 1) {
              if (a179 == 33 && cf == 1) {{
                  if (input == 2 && (a125 == 10 && (a102 == 14 && (a179 == 33 && cf == 1)) && a310 == 9)) {
                    cf = 0;
                    a87 = 14;
                    a102 = 12;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a125 == 11 && cf == 1) {
              if (cf == 1 && a69 == 9) {{
                  if (a125 == 11 && (input == 6 && (cf == 1 && a102 == 14) && a310 == 9) && a69 == 9) {
                    cf = 0;
                    a102 = 13;
                    a344 = 12;
                    a270 = 36;
                    a36 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
              if (a69 == 10 && cf == 1) {{
                  if (a102 == 14 && (a69 == 10 && (cf == 1 && a310 == 9 && a125 == 11)) && input == 8) {
                    cf = 0;
                    a310 = 16;
                    a176 = 35;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a125 == 11 && (input == 3 && (cf == 1 && a310 == 9)) && a69 == 10)) {
                    cf = 0;
                    a62 = 6;
                    a102 = 9;
                    a20 = 7;
                    a98 = 11;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 1 && (a102 == 14 && (cf == 1 && a69 == 10) && a310 == 9 && a125 == 11)) {
                    cf = 0;
                    a40 = 35;
                    a102 = 10;
                    a98 = 5;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a69 == 10 && (a310 == 9 && cf == 1) && a125 == 11) && input == 7) {
                    cf = 0;
                    a83 = 33;
                    a102 = 12;
                    a57 = 33;
                    a116 = 2;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a69 == 13) {{
                  if (a125 == 11 && (a310 == 9 && cf == 1 && a102 == 14) && input == 3 && a69 == 13) {
                    cf = 0;
                    a83 = 34;
                    a102 = 12;
                    a57 = 33;
                    a116 = 2;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a125 == 12) {
              if (cf == 1 && a23 == 9) {{
                  if (input == 1 && (a102 == 14 && (a23 == 9 && (a310 == 9 && (cf == 1 && a125 == 12))))) {
                    cf = 0;
                    a102 = 7;
                    a112 = 34;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a125 == 12 && (a23 == 9 && (a102 == 14 && (cf == 1 && input == 4) && a310 == 9))) {
                    cf = 0;
                    a102 = 12;
                    a270 = 33;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 10 && (cf == 1 && a310 == 9 && a125 == 12 && a102 == 14) && a23 == 9) {
                    cf = 0;
                    a270 = 33;
                    a102 = 12;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a125 == 13 && cf == 1) {
              if (a87 == 12 && cf == 1) {{
                  if (input == 1 && (cf == 1 && a310 == 9 && a102 == 14) && a87 == 12 && a125 == 13) {
                    cf = 0;
                    a130 = 5;
                    a103 = 35;
                    a102 = 7;
                    a116 = 7;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a87 == 12 && (cf == 1 && a125 == 13 && a310 == 9 && a102 == 14) && input == 2) {
                    cf = 0;
                    a102 = 8;
                    a289 = 35;
                    a249 = 34;
                    a111 = 8;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a125 == 14 && cf == 1) {
              if (cf == 1 && a190 == 34) {{
                  if (input == 7 && (a190 == 34 && cf == 1 && a102 == 14 && a125 == 14 && a310 == 9)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 9;
                    a112 = 35;
                    a116 = 4;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a190 == 34 && (cf == 1 && a102 == 14) && a125 == 14 && a310 == 9 && input == 1) {
                    cf = 0;
                    a306 = 4;
                    a102 = 9;
                    a20 = 4;
                    a28 = 2;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a125 == 15) {
              if (a182 == 13 && cf == 1) {{
                  if (a182 == 13 && (a102 == 14 && (a125 == 15 && (a310 == 9 && (cf == 1 && input == 8))))) {
                    cf = 0;
                    a116 = 8;
                    a102 = 12;
                    a87 = 13;
                    a159 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a125 == 15 && (a102 == 14 && (input == 4 && (cf == 1 && a310 == 9))) && a182 == 13) {
                    cf = 0;
                    a102 = 12;
                    a116 = 9;
                    a79 = 36;
                    a292 = 6;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (cf == 1 && a310 == 9) && input == 7 && a125 == 15 && a182 == 13) {
                    cf = 0;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a182 == 13 && cf == 1 && a125 == 15 && input == 1 && a310 == 9 && a102 == 14) {
                    cf = 0;
                    a112 = 34;
                    a130 = 9;
                    a102 = 7;
                    a168 = 2;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a182 == 14) {{
                  if (a125 == 15 && (cf == 1 && a310 == 9) && input == 6 && a102 == 14 && a182 == 14) {
                    cf = 0;
                    a16 = 32;
                    a102 = 11;
                    a314 = 33;
                    a192 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a125 == 15 && (cf == 1 && a182 == 14 && input == 3) && a310 == 9 && a102 == 14) {
                    cf = 0;
                    a314 = 33;
                    a16 = 32;
                    a102 = 11;
                    a192 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a182 == 14 && (input == 1 && cf == 1 && a125 == 15) && a102 == 14 && a310 == 9) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 10 && cf == 1) {{
            if (a190 == 33 && cf == 1) {
              if (a71 == 11 && cf == 1) {{
                  if (a102 == 14 && (input == 2 && (cf == 1 && a190 == 33) && a71 == 11) && a310 == 10) {
                    cf = 0;
                    a306 = 9;
                    a102 = 9;
                    a20 = 4;
                    a2 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 7 && (a190 == 33 && (cf == 1 && a310 == 10 && a102 == 14)) && a71 == 11) {
                    cf = 0;
                    a12 = 34;
                    a20 = 2;
                    a102 = 9;
                    a63 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a71 == 11 && (a102 == 14 && (cf == 1 && a310 == 10 && input == 8 && a190 == 33))) {
                    cf = 0;
                    a102 = 13;
                    a344 = 8;
                    a270 = 36;
                    a153 = 4;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a71 == 13) {{
                  if (a190 == 33 && (a71 == 13 && (a310 == 10 && (cf == 1 && a102 == 14))) && input == 1) {
                    cf = 0;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a190 == 33 && (a310 == 10 && (input == 10 && cf == 1) && a71 == 13) && a102 == 14) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a71 == 13 && (input == 7 && cf == 1 && a310 == 10 && a102 == 14 && a190 == 33)) {
                    cf = 0;
                    a296 = 35;
                    a91 = 36;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a190 == 34) {
              if (cf == 1 && a126 == 33) {{
                  if (a190 == 34 && (input == 2 && (a126 == 33 && (a310 == 10 && cf == 1))) && a102 == 14) {
                    cf = 0;
                    a190 = 33;
                    a71 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a126 == 33 && (a310 == 10 && cf == 1) && a190 == 34 && input == 6)) {
                    cf = 0;
                    a20 = 3;
                    a129 = 33;
                    a102 = 9;
                    a344 = 10;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a126 == 34) {{
                  if (a102 == 14 && (a310 == 10 && (a190 == 34 && (cf == 1 && a126 == 34) && input == 10))) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a190 == 34 && (a310 == 10 && (a126 == 34 && cf == 1 && a102 == 14) && input == 7)) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a190 == 36 && cf == 1) {
              if (a28 == 7 && cf == 1) {{
                  if (a310 == 10 && (a190 == 36 && (cf == 1 && a102 == 14 && a28 == 7) && input == 6)) {
                    cf = 0;
                    a314 = 33;
                    a8 = 35;
                    a16 = 36;
                    a102 = 11;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 11 && cf == 1) {{
            if (a0 == 3 && cf == 1) {
              if (a154 == 10 && cf == 1) {{
                  if (a310 == 11 && (input == 7 && (a154 == 10 && cf == 1)) && a102 == 14 && a0 == 3) {
                    cf = 0;
                    a270 = 33;
                    a172 = 35;
                    a164 = 33;
                    a102 = 13;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a0 == 4 && cf == 1) {
              if (cf == 1 && a176 == 34) {{
                  if (a102 == 14 && (cf == 1 && input == 6 && a0 == 4) && a310 == 11 && a176 == 34) {
                    cf = 0;
                    a102 = 9;
                    a120 = 33;
                    a20 = 6;
                    a127 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a0 == 4 && (a102 == 14 && (a176 == 34 && cf == 1 && a310 == 11)) && input == 8) {
                    cf = 0;
                    a102 = 11;
                    a314 = 32;
                    a197 = 35;
                    a140 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a176 == 34 && (a0 == 4 && (a102 == 14 && (cf == 1 && a310 == 11 && input == 2)))) {
                    cf = 0;
                    a130 = 8;
                    a182 = 9;
                    a102 = 7;
                    a386 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 3 && (a0 == 4 && (a310 == 11 && (a102 == 14 && cf == 1))) && a176 == 34) {
                    cf = 0;
                    a183 = 6;
                    a102 = 9;
                    a20 = 1;
                    a42 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a310 == 11 && (a0 == 4 && (cf == 1 && a176 == 34) && input == 4) && a102 == 14) {
                    cf = 0;
                    a119 = 35;
                    a40 = 34;
                    a244 = 33;
                    a102 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a0 == 7) {
              if (a175 == 13 && cf == 1) {{
                  if (a175 == 13 && (a0 == 7 && cf == 1 && a102 == 14 && input == 10 && a310 == 11)) {
                    cf = 0;
                    a102 = 12;
                    a116 = 3;
                    a232 = 34;
                    a344 = 14;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a310 == 11 && (input == 1 && (a175 == 13 && cf == 1 && a0 == 7) && a102 == 14)) {
                    cf = 0;
                    a9 = 34;
                    a270 = 34;
                    a102 = 13;
                    a47 = 8;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a0 == 10) {
              if (a120 == 33 && cf == 1) {{
                  if (a120 == 33 && (a102 == 14 && (a0 == 10 && (a310 == 11 && cf == 1 && input == 4)))) {
                    cf = 0;
                    a176 = 34;
                    a0 = 4;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a120 == 32) {{
                  if (a102 == 14 && (a120 == 32 && (a0 == 10 && cf == 1 && input == 8) && a310 == 11)) {
                    cf = 0;
                    a102 = 9;
                    a67 = 36;
                    a20 = 7;
                    a62 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a310 == 12) {{
            if (a296 == 35 && cf == 1) {
              if (a91 == 32 && cf == 1) {{
                  if (input == 4 && (cf == 1 && a91 == 32 && a296 == 35 && a102 == 14 && a310 == 12)) {
                    cf = 0;
                    a116 = 4;
                    a67 = 34;
                    a102 = 12;
                    a142 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a296 == 35 && (a91 == 32 && (a102 == 14 && cf == 1)) && a310 == 12 && input == 10) {
                    cf = 0;
                    a130 = 3;
                    a168 = 6;
                    a102 = 7;
                    a242 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a296 == 35 && (cf == 1 && a91 == 32 && a102 == 14 && input == 3 && a310 == 12)) {
                    cf = 0;
                    a106 = 34;
                    a102 = 12;
                    a79 = 34;
                    a116 = 9;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a296 == 35 && (cf == 1 && input == 5)) && a310 == 12 && a91 == 32) {
                    cf = 0;
                    a8 = 34;
                    a314 = 33;
                    a16 = 36;
                    a102 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a310 == 12 && (a296 == 35 && (a91 == 32 && cf == 1 && input == 2)))) {
                    cf = 0;
                    a314 = 36;
                    a78 = 32;
                    a102 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a91 == 36) {{
                  if (a310 == 12 && (a296 == 35 && (a91 == 36 && (input == 5 && (cf == 1 && a102 == 14))))) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a222 = 32;
                    a56 = 15;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 4 && (a91 == 36 && (a296 == 35 && (a310 == 12 && cf == 1)) && a102 == 14)) {
                    cf = 0;
                    a183 = 4;
                    a102 = 9;
                    a20 = 1;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 13 && cf == 1) {{
            if (a122 == 34 && cf == 1) {
              if (a242 == 6 && cf == 1) {{
                  if (a242 == 6 && (cf == 1 && input == 8 && a122 == 34) && a102 == 14 && a310 == 13) {
                    cf = 0;
                    a102 = 12;
                    a106 = 33;
                    a116 = 6;
                    a56 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a310 == 14) {{
            if (a163 == 32 && cf == 1) {
              if (cf == 1 && a113 == 35) {{
                  if (a102 == 14 && (a163 == 32 && (input == 10 && (a310 == 14 && cf == 1 && a113 == 35)))) {
                    cf = 0;
                    a57 = 36;
                    a289 = 34;
                    a102 = 8;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a163 == 32 && (cf == 1 && a310 == 14 && a102 == 14) && a113 == 35 && input == 8) {
                    cf = 0;
                    a248 = 8;
                    a102 = 7;
                    a130 = 6;
                    a335 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a310 == 14 && (input == 1 && cf == 1) && a163 == 32 && a102 == 14 && a113 == 35) {
                    cf = 0;
                    a106 = 35;
                    a314 = 34;
                    a102 = 11;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a113 == 35 && (a102 == 14 && (input == 6 && (a163 == 32 && (a310 == 14 && cf == 1))))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a89 = 32;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a163 == 34) {
              if (cf == 1 && a15 == 7) {{
                  if (a102 == 14 && (a163 == 34 && (input == 2 && cf == 1) && a15 == 7) && a310 == 14) {
                    cf = 0;
                    a289 = 34;
                    a57 = 35;
                    a102 = 8;
                    a130 = 8;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (input == 7 && (a15 == 7 && (a163 == 34 && (a310 == 14 && cf == 1))))) {
                    cf = 0;
                    a125 = 11;
                    a310 = 9;
                    a69 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a15 == 7 && cf == 1 && a102 == 14 && a310 == 14 && input == 6 && a163 == 34) {
                    cf = 0;
                    a20 = 7;
                    a62 = 3;
                    a102 = 9;
                    a171 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (a15 == 10 && cf == 1) {{
                  if (input == 10 && (a15 == 10 && (a310 == 14 && cf == 1 && a102 == 14)) && a163 == 34) {
                    cf = 0;
                    a67 = 36;
                    a116 = 4;
                    a102 = 12;
                    a310 = 12;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a163 == 35 && cf == 1) {
              if (cf == 1 && a58 == 32) {{
                  if (a310 == 14 && (a163 == 35 && (a58 == 32 && (a102 == 14 && (input == 2 && cf == 1))))) {
                    cf = 0;
                    a178 = 5;
                    a40 = 33;
                    a102 = 10;
                    a170 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a310 == 14 && cf == 1 && a102 == 14 && a163 == 35 && a58 == 32 && input == 4) {
                    cf = 0;
                    a112 = 32;
                    a102 = 7;
                    a130 = 9;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 3 && cf == 1 && a102 == 14 && a58 == 32 && a310 == 14 && a163 == 35) {
                    cf = 0;
                    a112 = 34;
                    a130 = 9;
                    a102 = 7;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 15 && cf == 1) {{
            if (a71 == 8 && cf == 1) {
              if (a197 == 35 && cf == 1) {{
                  if (a197 == 35 && (input == 1 && (a71 == 8 && cf == 1)) && a102 == 14 && a310 == 15) {
                    cf = 0;
                    a163 = 35;
                    a58 = 32;
                    a310 = 14;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a71 == 9) {
              if (cf == 1 && a166 == 33) {{
                  if (a310 == 15 && (a102 == 14 && (a71 == 9 && (input == 1 && cf == 1) && a166 == 33))) {
                    cf = 0;
                    a83 = 34;
                    a102 = 11;
                    a314 = 35;
                    a117 = 14;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a310 == 15 && (input == 7 && (a102 == 14 && (a166 == 33 && cf == 1) && a71 == 9))) {
                    cf = 0;
                    a130 = 8;
                    a148 = 36;
                    a102 = 7;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a71 == 9 && cf == 1 && a166 == 33 && a310 == 15) && input == 4) {
                    cf = 0;
                    a130 = 8;
                    a102 = 7;
                    a148 = 36;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a71 == 10) {
              if (cf == 1 && a22 == 33) {{
                  if (a22 == 33 && (a310 == 15 && (input == 4 && cf == 1)) && a71 == 10 && a102 == 14) {
                    cf = 0;
                    a130 = 9;
                    a102 = 7;
                    a112 = 32;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a71 == 10 && cf == 1 && a102 == 14 && a22 == 33 && input == 3 && a310 == 15) {
                    cf = 0;
                    a112 = 34;
                    a102 = 7;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a22 == 33 && cf == 1) && a310 == 15 && input == 1 && a71 == 10) {
                    cf = 0;
                    a116 = 8;
                    a102 = 12;
                    a385 = 32;
                    a87 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a71 == 11 && cf == 1) {
              if (cf == 1 && a296 == 33) {{
                  if (a296 == 33 && (input == 7 && (cf == 1 && a102 == 14 && a310 == 15) && a71 == 11)) {
                    cf = 0;
                    a310 = 9;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 8 && (a102 == 14 && (cf == 1 && a310 == 15 && a296 == 33 && a71 == 11))) {
                    cf = 0;
                    a116 = 8;
                    a87 = 13;
                    a102 = 12;
                    a159 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a296 == 33 && (a71 == 11 && (input == 1 && (a102 == 14 && cf == 1 && a310 == 15)))) {
                    cf = 0;
                    a116 = 7;
                    a74 = 32;
                    a102 = 12;
                    a28 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a71 == 13) {
              if (cf == 1 && a25 == 32) {{
                  if (a71 == 13 && (input == 7 && (a25 == 32 && cf == 1 && a102 == 14) && a310 == 15)) {
                    cf = 0;
                    a102 = 7;
                    a148 = 36;
                    a130 = 8;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a25 == 32 && (a71 == 13 && (input == 1 && cf == 1 && a310 == 15) && a102 == 14)) {
                    cf = 0;
                    a102 = 7;
                    a182 = 9;
                    a130 = 8;
                    a386 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a25 == 32 && cf == 1 && a102 == 14 && input == 10 && a71 == 13 && a310 == 15) {
                    cf = 0;
                    a310 = 11;
                    a176 = 34;
                    a0 = 4;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 16 && cf == 1) {{
            if (a176 == 34 && cf == 1) {
              if (a364 == 8 && cf == 1) {{
                  if (a364 == 8 && (cf == 1 && a310 == 16 && a102 == 14 && input == 8 && a176 == 34)) {
                    cf = 0;
                    a102 = 13;
                    a270 = 32;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 1 && (a364 == 8 && (cf == 1 && a176 == 34) && a310 == 16) && a102 == 14) {
                    cf = 0;
                    a102 = 10;
                    a40 = 35;
                    a98 = 5;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a176 == 34 && (a310 == 16 && (cf == 1 && input == 5 && a102 == 14 && a364 == 8))) {
                    cf = 0;
                    a102 = 12;
                    a116 = 8;
                    a87 = 14;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a176 == 34 && (a310 == 16 && (cf == 1 && a102 == 14 && a364 == 8)) && input == 4) {
                    cf = 0;
                    a310 = 9;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a364 == 9) {{
                  if (a102 == 14 && (a176 == 34 && (input == 7 && cf == 1 && a310 == 16)) && a364 == 9) {
                    cf = 0;
                    a125 = 11;
                    a310 = 9;
                    a69 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a176 == 34 && (a102 == 14 && (input == 6 && (cf == 1 && a364 == 9))) && a310 == 16) {
                    cf = 0;
                    a69 = 14;
                    a102 = 13;
                    a270 = 32;
                    a390 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a364 == 11) {{
                  if (a364 == 11 && (a176 == 34 && (cf == 1 && a310 == 16) && a102 == 14) && input == 7) {
                    cf = 0;
                    a102 = 9;
                    a62 = 3;
                    a20 = 7;
                    a171 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
{
        if (a121 == 33 && a178 == 11 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(0);
        }
        if (a197 == 33 && a344 == 13 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(1);
        }
        if (a145 == 32 && a120 == 36 && a20 == 6 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(2);
        }
        if (36 == 36 && a106 == 34 && a314 == 34 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(3);
        }
        if (a9 == 34 && a98 == 8 && a40 == 35 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(4);
        }
        if (a164 == 35 && a74 == 36 && a116 == 7 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(5);
        }
        if (a157 == 32 && a170 == 7 && a270 == 35 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(6);
        }
        if (a111 == 10 && a249 == 34 && a289 == 35 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(7);
        }
        if (a106 == 36 && a56 == 11 && a116 == 6 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(8);
        }
        if (a170 == 12 && a178 == 5 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(9);
        }
        if (a135 == 32 && a24 == 33 && a40 == 36 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(10);
        }
        if (a130 == 8 && a57 == 35 && a289 == 34 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(11);
        }
        if (a70 == 36 && a170 == 9 && a270 == 35 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(12);
        }
        if (2 == 6 && a176 == 35 && a310 == 16 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(13);
        }
        if (a127 == 6 && a120 == 33 && a20 == 6 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(14);
        }
        if (a170 == 14 && a178 == 5 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(15);
        }
        if (a123 == 36 && a83 == 35 && a314 == 35 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(16);
        }
        if (a222 == 32 && a56 == 15 && a116 == 6 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(17);
        }
        if (a159 == 12 && a87 == 13 && a116 == 8 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(18);
        }
        if (a121 == 34 && a178 == 11 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(19);
        }
        if (2 == 2 && a176 == 35 && a310 == 16 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(20);
        }
        if (a2 == 8 && a306 == 9 && a20 == 4 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(21);
        }
        if (a142 == 7 && a67 == 34 && a116 == 4 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(22);
        }
        if (a98 == 6 && a62 == 6 && a20 == 7 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(23);
        }
        if (a87 == 11 && a12 == 32 && a20 == 2 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(24);
        }
        if (32 == 34 && a143 == 7 && a130 == 10 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(25);
        }
        if (a341 == 34 && a306 == 10 && a20 == 4 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(26);
        }
        if (a44 == 12 && a120 == 32 && a20 == 6 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(27);
        }
        if (a98 == 10 && a71 == 6 && a310 == 15 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(28);
        }
        if (a13 == 35 && a183 == 9 && a20 == 1 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(29);
        }
        if (a195 == 10 && a17 == 9 && a130 == 4 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(30);
        }
        if (a154 == 14 && a197 == 34 && a314 == 32 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(31);
        }
        if (a194 == 8 && a249 == 33 && a20 == 5 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(32);
        }
        if (a44 == 14 && a170 == 13 && a270 == 35 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(33);
        }
        if (a129 == 35 && a120 == 35 && a20 == 6 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(34);
        }
        if (a8 == 34 && a16 == 36 && a314 == 33 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(35);
        }
        if (a115 == 35 && a146 == 34 && a289 == 36 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(36);
        }
        if (34 == 35 && a162 == 11 && a130 == 7 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(37);
        }
        if (a335 == 10 && a57 == 32 && a289 == 34 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(38);
        }
        if (a242 == 6 && a168 == 6 && a130 == 3 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(39);
        }
        if (a47 == 11 && a9 == 34 && a270 == 34 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(40);
        }
        if (a385 == 32 && a87 == 10 && a116 == 8 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(41);
        }
        if (a36 == 11 && a344 == 12 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(42);
        }
        if (a42 == 13 && a183 == 6 && a20 == 1 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(43);
        }
        if (10 == 12 && a57 == 32 && a116 == 2 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(44);
        }
        if (a178 == 9 && a248 == 9 && a130 == 6 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(45);
        }
        if (a87 == 17 && a12 == 32 && a20 == 2 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(46);
        }
        if (a66 == 34 && a178 == 10 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(47);
        }
        if (a194 == 11 && a344 == 14 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(48);
        }
        if (12 == 12 && a74 == 34 && a116 == 7 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(49);
        }
        if (a156 == 32 && a176 == 32 && a310 == 16 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(50);
        }
        if (a117 == 8 && a5 == 32 && a116 == 5 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(51);
        }
        if (a332 == 35 && a62 == 9 && a20 == 7 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(52);
        }
        if (a49 == 5 && a143 == 5 && a130 == 10 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(53);
        }
        if (a292 == 10 && a79 == 36 && a116 == 9 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(54);
        }
        if (a161 == 13 && a344 == 11 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(55);
        }
        if (a306 == 6 && a183 == 10 && a20 == 1 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(56);
        }
        if (8 == 7 && a249 == 35 && a20 == 5 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(57);
        }
        if (a9 == 36 && a178 == 8 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(58);
        }
        if (a292 == 6 && a79 == 36 && a116 == 9 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(59);
        }
        if (a67 == 36 && a385 == 36 && a40 == 32 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(60);
        }
        if (a25 == 33 && a71 == 13 && a310 == 15 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(61);
        }
        if (a171 == 11 && a62 == 3 && a20 == 7 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(62);
        }
        if (a185 == 5 && a87 == 12 && a116 == 8 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(63);
        }
        if (a28 == 6 && a162 == 7 && a130 == 7 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(64);
        }
        if (a28 == 8 && a190 == 36 && a310 == 10 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(65);
        }
        if (a36 == 13 && a182 == 9 && a289 == 32 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(66);
        }
        if (a123 == 35 && a83 == 35 && a314 == 35 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(67);
        }
        if (a70 == 32 && a172 == 32 && a270 == 33 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(68);
        }
        if (a98 == 10 && a249 == 35 && a289 == 35 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(69);
        }
        if (a23 == 8 && a125 == 12 && a310 == 9 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(70);
        }
        if (a344 == 8 && a129 == 33 && a20 == 3 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(71);
        }
        if (a54 == 14 && a106 == 32 && a314 == 34 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(72);
        }
        if (a165 == 12 && a71 == 12 && a310 == 15 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(73);
        }
        if (a116 == 8 && a112 == 35 && a130 == 9 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(74);
        }
        if (a186 == 33 && a168 == 5 && a130 == 3 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(75);
        }
        if (a91 == 35 && a125 == 8 && a310 == 9 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(76);
        }
        if (11 == 15 && a244 == 32 && a40 == 34 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(77);
        }
        if (a323 == 1 && a385 == 32 && a40 == 32 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(78);
        }
        if (a197 == 32 && a344 == 13 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(79);
        }
        if (a4 == 11 && a106 == 35 && a314 == 34 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(80);
        }
        if (a244 == 33 && a112 == 33 && a130 == 9 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(81);
        }
        if (a195 == 14 && a17 == 9 && a130 == 4 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(82);
        }
        if (36 == 35 && a182 == 16 && a130 == 8 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(83);
        }
        if (a182 == 11 && a385 == 33 && a40 == 32 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(84);
        }
        if (10 == 13 && a344 == 10 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(85);
        }
        if (a3 == 9 && a182 == 13 && a289 == 32 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(86);
        }
        if (a78 == 36 && a0 == 5 && a310 == 11 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(87);
        }
        if (a119 == 36 && a244 == 33 && a40 == 34 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(88);
        }
        if (a143 == 10 && a248 == 10 && a130 == 6 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(89);
        }
        if (a144 == 36 && a182 == 15 && a289 == 32 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(90);
        }
        if (a37 == 36 && a74 == 35 && a116 == 7 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(91);
        }
        if (33 == 34 && a271 == 6 && a20 == 8 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(92);
        }
        if (a191 == 35 && a232 == 35 && a116 == 3 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(93);
        }
        if (34 == 33 && a296 == 33 && a310 == 12 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(94);
        }
        if (a3 == 10 && a182 == 13 && a289 == 32 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(95);
        }
        if (a170 == 10 && a178 == 5 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(96);
        }
        if (a36 == 14 && a344 == 12 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(97);
        }
        if (a86 == 7 && a197 == 33 && a314 == 32 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(98);
        }
        if (a166 == 36 && a71 == 9 && a310 == 15 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(99);
        }
      }
      if (cf == 1) 
        output = - 2;
    }
//edited by script: global output variable replacing printf in functions 
    if (output == - 2) {
      //fprintf(stderr,"Invalid input: %d\n",input);
    }
     else if (output != - 1) {
      printf("%d\n",output);
    }
 // }






// main i/o-loop


//  while(1){ //**UNROLLED MAIN LOOP


//edited by script: maybe no output, reset output var 
    output = - 1;
// read input
    scanf("%d",&input);
// operate eca engine
    //if (input != 5 && input != 6 && input != 1 && input != 7 && input != 2 && input != 8 && input != 3 && input != 9 && input != 10 && input != 4) //**REMOVED RETURN STATEMENT AND GENERATE FORMULA (1 <= input <= 10) WHENEVER AN INPUT IS READ
    //return - 2;
{
      cf = 1;
      if (cf == 1 && a102 == 7) {
        if (a130 == 3 && cf == 1) {{
            if (cf == 1 && a168 == 1) {
              if (cf == 1 && a162 == 5) {{
                  if (input == 10 && (a162 == 5 && (a102 == 7 && cf == 1 && a130 == 3)) && a168 == 1) {
                    cf = 0;
                    a102 = 12;
                    a22 = 36;
                    a116 = 6;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 3 && (a102 == 7 && (a130 == 3 && cf == 1 && a162 == 5) && a168 == 1)) {
                    cf = 0;
                    a168 = 8;
                    a30 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a162 == 5 && (input == 1 && (a168 == 1 && (cf == 1 && a130 == 3)) && a102 == 7)) {
                    cf = 0;
                    a102 = 12;
                    a22 = 36;
                    a116 = 6;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a162 == 6 && cf == 1) {{
                  if (a168 == 1 && (a102 == 7 && cf == 1 && input == 4 && a162 == 6) && a130 == 3) {
                    cf = 0;
                    a112 = 34;
                    a130 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a130 == 3 && (cf == 1 && a162 == 6) && a168 == 1) && input == 3) {
                    cf = 0;
                    a270 = 35;
                    a102 = 13;
                    a157 = 36;
                    a170 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a162 == 9) {{
                  if (a162 == 9 && (cf == 1 && a102 == 7) && input == 1 && a130 == 3 && a168 == 1) {
                    cf = 0;
                    a116 = 5;
                    a5 = 35;
                    a102 = 12;
                    a173 = 7;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a168 == 2) {
              if (a86 == 7 && cf == 1) {{
                  if (a102 == 7 && (a86 == 7 && cf == 1) && a130 == 3 && a168 == 2 && input == 7) {
                    cf = 0;
                    a168 = 8;
                    a30 = 8;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a86 == 11) {{
                  if (a130 == 3 && (cf == 1 && a86 == 11 && a168 == 2 && a102 == 7 && input == 2)) {
                    cf = 0;
                    a130 = 10;
                    a143 = 5;
                    a49 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a168 == 2 && (a86 == 11 && (cf == 1 && a130 == 3)) && input == 4)) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a168 == 3) {
              if (cf == 1 && a15 == 8) {{
                  if (a168 == 3 && (input == 3 && (cf == 1 && a102 == 7 && a15 == 8 && a130 == 3))) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a15 == 8 && (a102 == 7 && cf == 1 && a168 == 3) && a130 == 3 && input == 7) {
                    cf = 0;
                    a102 = 9;
                    a12 = 34;
                    a20 = 2;
                    a63 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 10 && (a102 == 7 && (a130 == 3 && (a15 == 8 && cf == 1 && a168 == 3)))) {
                    cf = 0;
                    a20 = 2;
                    a102 = 9;
                    a12 = 34;
                    a63 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a130 == 3 && (input == 8 && (a168 == 3 && (cf == 1 && a15 == 8))) && a102 == 7) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a168 == 6 && cf == 1) {
              if (a242 == 10 && cf == 1) {{
                  if (a168 == 6 && (a242 == 10 && cf == 1 && a130 == 3 && a102 == 7 && input == 4)) {
                    cf = 0;
                    a102 = 12;
                    a116 = 4;
                    a67 = 32;
                    a98 = 5;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a242 == 10 && (input == 10 && (cf == 1 && a168 == 6)) && a130 == 3 && a102 == 7) {
                    cf = 0;
                    a116 = 6;
                    a102 = 12;
                    a56 = 14;
                    a49 = 3;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a168 == 6 && (a130 == 3 && (cf == 1 && input == 6) && a242 == 10) && a102 == 7) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 11;
                    a69 = 13;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a168 == 7) {
              if (cf == 1 && a100 == 36) {{
                  if (a130 == 3 && (a102 == 7 && (cf == 1 && input == 10 && a168 == 7) && a100 == 36)) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a100 == 36 && (a130 == 3 && (cf == 1 && a168 == 7) && a102 == 7 && input == 7)) {
                    cf = 0;
                    a102 = 12;
                    a67 = 34;
                    a116 = 4;
                    a142 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 6 && (a130 == 3 && (cf == 1 && a168 == 7 && a102 == 7)) && a100 == 36) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a100 == 36 && cf == 1 && a102 == 7 && input == 4 && a168 == 7 && a130 == 3) {
                    cf = 0;
                    a102 = 14;
                    a91 = 32;
                    a296 = 35;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a168 == 8 && cf == 1) {
              if (cf == 1 && a30 == 7) {{
                  if (a102 == 7 && (cf == 1 && a30 == 7 && a130 == 3 && input == 10 && a168 == 8)) {
                    cf = 0;
                    a106 = 35;
                    a314 = 34;
                    a102 = 11;
                    a4 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a130 == 3 && (input == 7 && (a168 == 8 && cf == 1) && a30 == 7 && a102 == 7)) {
                    cf = 0;
                    a143 = 5;
                    a130 = 10;
                    a49 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a30 == 7 && (a168 == 8 && (a102 == 7 && cf == 1))) && a130 == 3) {
                    cf = 0;
                    a5 = 32;
                    a116 = 5;
                    a102 = 12;
                    a117 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a130 == 3 && cf == 1 && a168 == 8 && a30 == 7 && input == 8 && a102 == 7) {
                    cf = 0;
                    a102 = 9;
                    a183 = 7;
                    a20 = 1;
                    a165 = 10;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a30 == 8 && cf == 1) {{
                  if (a168 == 8 && (a30 == 8 && cf == 1 && a130 == 3 && input == 10) && a102 == 7) {
                    cf = 0;
                    a310 = 10;
                    a190 = 36;
                    a102 = 14;
                    a28 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a168 == 8 && (a30 == 8 && (a130 == 3 && (input == 2 && cf == 1) && a102 == 7))) {
                    cf = 0;
                    a314 = 33;
                    a16 = 36;
                    a8 = 36;
                    a102 = 11;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a130 == 4) {{
            if (a17 == 4 && cf == 1) {
              if (cf == 1 && a21 == 9) {{
                  if (input == 3 && (a21 == 9 && (a17 == 4 && (cf == 1 && a102 == 7) && a130 == 4))) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a17 == 4 && cf == 1 && input == 2 && a130 == 4 && a102 == 7 && a21 == 9) {
                    cf = 0;
                    a310 = 9;
                    a102 = 14;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a17 == 6) {
              if (a143 == 5 && cf == 1) {{
                  if (a17 == 6 && (a102 == 7 && (a130 == 4 && cf == 1 && input == 4)) && a143 == 5) {
                    cf = 0;
                    a270 = 36;
                    a222 = 34;
                    a102 = 13;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a17 == 6 && cf == 1 && a130 == 4 && input == 10 && a143 == 5 && a102 == 7) {
                    cf = 0;
                    a270 = 36;
                    a102 = 13;
                    a222 = 34;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a17 == 6 && (a130 == 4 && cf == 1 && a143 == 5 && a102 == 7) && input == 1) {
                    cf = 0;
                    a102 = 12;
                    a270 = 33;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a17 == 7 && cf == 1) {
              if (cf == 1 && a151 == 4) {{
                  if (a130 == 4 && (a102 == 7 && cf == 1) && a151 == 4 && input == 4 && a17 == 7) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a151 == 4 && (cf == 1 && a17 == 7) && a102 == 7 && input == 2 && a130 == 4) {
                    cf = 0;
                    a20 = 6;
                    a120 = 33;
                    a102 = 9;
                    a127 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a151 == 4 && (a130 == 4 && (a102 == 7 && (a17 == 7 && (input == 7 && cf == 1))))) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a17 == 8) {
              if (a115 == 32 && cf == 1) {{
                  if (input == 4 && (a130 == 4 && (a17 == 8 && cf == 1) && a102 == 7) && a115 == 32) {
                    cf = 0;
                    a102 = 9;
                    a120 = 36;
                    a145 = 35;
                    a20 = 6;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a130 == 4 && (a102 == 7 && (a17 == 8 && cf == 1) && input == 6 && a115 == 32)) {
                    cf = 0;
                    a112 = 33;
                    a244 = 36;
                    a130 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a17 == 8 && (a115 == 32 && (cf == 1 && a130 == 4 && input == 2) && a102 == 7)) {
                    cf = 0;
                    a102 = 10;
                    a59 = 33;
                    a40 = 35;
                    a98 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (a115 == 35 && cf == 1) {{
                  if (a17 == 8 && (a102 == 7 && (a115 == 35 && cf == 1 && input == 7)) && a130 == 4) {
                    cf = 0;
                    a289 = 32;
                    a102 = 8;
                    a144 = 35;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a115 == 35 && (cf == 1 && a102 == 7 && a130 == 4 && input == 5) && a17 == 8) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a17 == 8 && (cf == 1 && input == 1 && a115 == 35) && a102 == 7 && a130 == 4) {
                    cf = 0;
                    a125 = 15;
                    a102 = 14;
                    a310 = 9;
                    a182 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a17 == 8 && (a102 == 7 && cf == 1)) && a130 == 4 && a115 == 35) {
                    cf = 0;
                    a144 = 35;
                    a289 = 32;
                    a102 = 8;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a17 == 9) {
              if (cf == 1 && a195 == 11) {{
                  if (a195 == 11 && (a130 == 4 && (input == 10 && (a102 == 7 && (a17 == 9 && cf == 1))))) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a9 = 33;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a17 == 9 && (a195 == 11 && (input == 3 && (a102 == 7 && cf == 1) && a130 == 4))) {
                    cf = 0;
                    a129 = 33;
                    a20 = 3;
                    a102 = 9;
                    a344 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a195 == 12) {{
                  if (a130 == 4 && (cf == 1 && a195 == 12 && input == 2) && a17 == 9 && a102 == 7) {
                    cf = 0;
                    a37 = 35;
                    a130 = 6;
                    a248 = 12;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a17 == 10 && cf == 1) {
              if (cf == 1 && 6 == 6) {{
                  if (a17 == 10 && (input == 6 && (a102 == 7 && (cf == 1 && a130 == 4 && 6 == 6)))) {
                    cf = 0;
                    a100 = 36;
                    a130 = 3;
                    a168 = 7;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a130 == 5) {{
            if (cf == 1 && a103 == 34) {
              if (a80 == 32 && cf == 1) {{
                  if (a130 == 5 && (a103 == 34 && (input == 8 && cf == 1 && a102 == 7)) && a80 == 32) {
                    cf = 0;
                    a130 = 8;
                    a182 = 9;
                    a386 = 12;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a103 == 35 && cf == 1) {
              if (a116 == 7 && cf == 1) {{
                  if (a103 == 35 && (cf == 1 && input == 3 && a116 == 7 && a130 == 5) && a102 == 7) {
                    cf = 0;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a116 == 7 && (a102 == 7 && (input == 2 && (a130 == 5 && (cf == 1 && a103 == 35))))) {
                    cf = 0;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a116 == 7 && (a102 == 7 && (input == 1 && (a103 == 35 && cf == 1) && a130 == 5))) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 13;
                    a87 = 12;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 4 && cf == 1 && a102 == 7 && a130 == 5 && a103 == 35 && a116 == 7) {
                    cf = 0;
                    a148 = 36;
                    a130 = 8;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a130 == 6 && cf == 1) {{
            if (a248 == 7 && cf == 1) {
              if (cf == 1 && 35 == 35) {{
                  if (35 == 35 && (a102 == 7 && cf == 1 && input == 6 && a248 == 7) && a130 == 6) {
                    cf = 0;
                    a130 = 4;
                    a17 = 6;
                    a143 = 5;
                    output = 15;
                    fflush(stdout);
                  }
                  if (35 == 35 && (cf == 1 && input == 7 && a248 == 7) && a102 == 7 && a130 == 6) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a248 == 8 && cf == 1) {
              if (a335 == 7 && cf == 1) {{
                  if (a248 == 8 && (cf == 1 && input == 1 && a102 == 7 && a335 == 7) && a130 == 6) {
                    cf = 0;
                    a102 = 14;
                    a310 = 15;
                    a22 = 33;
                    a71 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a335 == 8) {{
                  if (input == 3 && (a130 == 6 && cf == 1 && a248 == 8 && a102 == 7 && a335 == 8)) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a248 == 9) {
              if (cf == 1 && a178 == 5) {{
                  if (a102 == 7 && (input == 4 && (a178 == 5 && cf == 1 && a248 == 9)) && a130 == 6) {
                    cf = 0;
                    a310 = 11;
                    a102 = 14;
                    a0 = 3;
                    a154 = 10;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a248 == 12 && cf == 1) {
              if (cf == 1 && a37 == 32) {{
                  if (input == 2 && (a37 == 32 && (a248 == 12 && (cf == 1 && a130 == 6) && a102 == 7))) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 10 && (a248 == 12 && (cf == 1 && a130 == 6 && a102 == 7) && a37 == 32)) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a37 == 32 && (cf == 1 && a248 == 12) && a130 == 6 && a102 == 7 && input == 4) {
                    cf = 0;
                    a130 = 3;
                    a168 = 6;
                    a242 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a248 == 12 && (a37 == 32 && cf == 1 && input == 7) && a130 == 6 && a102 == 7) {
                    cf = 0;
                    a56 = 14;
                    a102 = 12;
                    a116 = 6;
                    a49 = 3;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a37 == 34) {{
                  if (a248 == 12 && (input == 1 && (cf == 1 && a102 == 7)) && a130 == 6 && a37 == 34) {
                    cf = 0;
                    a17 = 6;
                    a130 = 4;
                    a143 = 5;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a37 == 35) {{
                  if (a102 == 7 && (a130 == 6 && (a37 == 35 && cf == 1 && input == 1) && a248 == 12)) {
                    cf = 0;
                    a57 = 33;
                    a289 = 34;
                    a270 = 32;
                    a102 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a248 == 14) {
              if (cf == 1 && a365 == 12) {{
                  if (a248 == 14 && (a365 == 12 && (input == 10 && cf == 1 && a130 == 6)) && a102 == 7) {
                    cf = 0;
                    a248 = 7;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a248 == 14 && (a130 == 6 && cf == 1) && a365 == 12) && input == 3) {
                    cf = 0;
                    a168 = 1;
                    a130 = 3;
                    a162 = 6;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a130 == 7 && cf == 1) {{
            if (a162 == 5 && cf == 1) {
              if (a175 == 9 && cf == 1) {{
                  if (a175 == 9 && (input == 2 && (a102 == 7 && cf == 1) && a162 == 5 && a130 == 7)) {
                    cf = 0;
                    a314 = 35;
                    a83 = 34;
                    a102 = 11;
                    a117 = 14;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a175 == 9 && (input == 3 && (a162 == 5 && (cf == 1 && a130 == 7 && a102 == 7)))) {
                    cf = 0;
                    a102 = 12;
                    a37 = 36;
                    a74 = 35;
                    a116 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a175 == 9 && (a162 == 5 && (a130 == 7 && (cf == 1 && input == 4)) && a102 == 7)) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 13;
                    a87 = 12;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a175 == 9 && (input == 8 && (a130 == 7 && (a162 == 5 && cf == 1))) && a102 == 7) {
                    cf = 0;
                    a182 = 14;
                    a289 = 32;
                    a102 = 8;
                    a184 = 5;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a130 == 7 && (a102 == 7 && (input == 1 && (cf == 1 && a162 == 5)) && a175 == 9)) {
                    cf = 0;
                    a16 = 32;
                    a102 = 11;
                    a314 = 33;
                    a192 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (a175 == 11 && cf == 1) {{
                  if (input == 4 && (a130 == 7 && (a102 == 7 && cf == 1) && a162 == 5) && a175 == 11) {
                    cf = 0;
                    a296 = 35;
                    a91 = 36;
                    a102 = 14;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a162 == 5 && (a130 == 7 && (cf == 1 && input == 8 && a175 == 11)))) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a162 == 5 && (a102 == 7 && (a130 == 7 && (input == 6 && cf == 1) && a175 == 11))) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a162 == 5 && (cf == 1 && a175 == 11 && a102 == 7 && input == 10 && a130 == 7)) {
                    cf = 0;
                    a130 = 8;
                    a148 = 36;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 9 && (a102 == 7 && (a175 == 11 && (a162 == 5 && (a130 == 7 && cf == 1))))) {
                    cf = 0;
                    a130 = 3;
                    a168 = 6;
                    a242 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a162 == 7 && cf == 1) {
              if (cf == 1 && a28 == 3) {{
                  if (a102 == 7 && (a162 == 7 && (cf == 1 && a130 == 7 && a28 == 3 && input == 8))) {
                    cf = 0;
                    a102 = 8;
                    a289 = 33;
                    a75 = 35;
                    a20 = 2;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a162 == 8 && cf == 1) {
              if (a93 == 6 && cf == 1) {{
                  if (a162 == 8 && (a130 == 7 && (input == 1 && (a93 == 6 && cf == 1))) && a102 == 7) {
                    cf = 0;
                    a102 = 8;
                    a146 = 33;
                    a289 = 36;
                    a195 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a130 == 7 && (a93 == 6 && cf == 1) && a102 == 7 && a162 == 8 && input == 7) {
                    cf = 0;
                    a102 = 13;
                    a270 = 35;
                    a73 = 36;
                    a170 = 12;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a162 == 10 && cf == 1) {
              if (a128 == 33 && cf == 1) {{
                  if (a130 == 7 && (a128 == 33 && (cf == 1 && a162 == 10 && a102 == 7) && input == 1)) {
                    cf = 0;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a128 == 33 && (a162 == 10 && (input == 3 && cf == 1 && a102 == 7) && a130 == 7)) {
                    cf = 0;
                    a296 = 35;
                    a102 = 14;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a130 == 8) {{
            if (cf == 1 && a182 == 9) {
              if (cf == 1 && a386 == 9) {{
                  if (a102 == 7 && (a182 == 9 && (a386 == 9 && (input == 6 && cf == 1)) && a130 == 8)) {
                    cf = 0;
                    a170 = 10;
                    a102 = 13;
                    a270 = 35;
                    a55 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a386 == 9 && (a102 == 7 && (cf == 1 && a182 == 9 && a130 == 8)) && input == 2) {
                    cf = 0;
                    a102 = 14;
                    a25 = 32;
                    a310 = 15;
                    a71 = 13;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (a386 == 12 && cf == 1) {{
                  if (a386 == 12 && (a102 == 7 && cf == 1) && a130 == 8 && input == 10 && a182 == 9) {
                    cf = 0;
                    a306 = 11;
                    a102 = 9;
                    a20 = 4;
                    a77 = 5;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 11) {
              if (cf == 1 && a148 == 36) {{
                  if (cf == 1 && a130 == 8 && input == 2 && a148 == 36 && a182 == 11 && a102 == 7) {
                    cf = 0;
                    a176 = 34;
                    a102 = 14;
                    a310 = 11;
                    a0 = 4;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a148 == 36 && (a102 == 7 && (input == 6 && (a130 == 8 && cf == 1 && a182 == 11)))) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a148 == 36 && (cf == 1 && a182 == 11) && a102 == 7 && a130 == 8 && input == 10) {
                    cf = 0;
                    a314 = 32;
                    a102 = 11;
                    a197 = 34;
                    a154 = 14;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 14) {
              if (a52 == 32 && cf == 1) {{
                  if (a102 == 7 && (a130 == 8 && cf == 1) && a182 == 14 && input == 7 && a52 == 32) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a130 == 8 && (cf == 1 && a52 == 32 && input == 4 && a102 == 7 && a182 == 14)) {
                    cf = 0;
                    a40 = 35;
                    a9 = 35;
                    a102 = 10;
                    a98 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a52 == 35) {{
                  if (a182 == 14 && (a52 == 35 && (cf == 1 && a102 == 7) && input == 6 && a130 == 8)) {
                    cf = 0;
                    a130 = 3;
                    a168 = 1;
                    a162 = 9;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a52 == 35 && (a130 == 8 && (input == 2 && cf == 1 && a102 == 7)) && a182 == 14) {
                    cf = 0;
                    a102 = 12;
                    a191 = 35;
                    a232 = 35;
                    a116 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a130 == 9 && cf == 1) {{
            if (cf == 1 && a112 == 33) {
              if (a244 == 34 && cf == 1) {{
                  if (a130 == 9 && (a244 == 34 && (a102 == 7 && cf == 1 && input == 6)) && a112 == 33) {
                    cf = 0;
                    a130 = 10;
                    a143 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (input == 10 && (a130 == 9 && cf == 1)) && a112 == 33 && a244 == 34) {
                    cf = 0;
                    a116 = 8;
                    a87 = 14;
                    a102 = 12;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a244 == 36) {{
                  if (a130 == 9 && (a102 == 7 && (a112 == 33 && (cf == 1 && a244 == 36 && input == 2)))) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a112 == 32) {
              if (a32 == 10 && cf == 1) {{
                  if (input == 1 && (a130 == 9 && cf == 1 && a102 == 7 && a112 == 32 && a32 == 10)) {
                    cf = 0;
                    a116 = 2;
                    a57 = 36;
                    a102 = 12;
                    a7 = 5;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a112 == 34 && cf == 1) {
              if (a168 == 1 && cf == 1) {{
                  if (a168 == 1 && (a112 == 34 && (input == 2 && (a130 == 9 && cf == 1)) && a102 == 7)) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (input == 3 && (a130 == 9 && (cf == 1 && a112 == 34) && a102 == 7 && a168 == 1)) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (cf == 1 && a130 == 9 && a168 == 1 && a112 == 34 && input == 4 && a102 == 7) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
              if (a168 == 2 && cf == 1) {{
                  if (input == 7 && (a130 == 9 && (a112 == 34 && (a102 == 7 && (cf == 1 && a168 == 2))))) {
                    cf = 0;
                    a102 = 14;
                    a125 = 11;
                    a310 = 9;
                    a69 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a168 == 2 && (a112 == 34 && (a130 == 9 && (a102 == 7 && (input == 6 && cf == 1))))) {
                    cf = 0;
                    a102 = 9;
                    a62 = 3;
                    a20 = 7;
                    a171 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a112 == 34 && (a130 == 9 && (a102 == 7 && cf == 1 && input == 8 && a168 == 2))) {
                    cf = 0;
                    a102 = 10;
                    a40 = 33;
                    a121 = 34;
                    a178 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a168 == 7) {{
                  if (a130 == 9 && (a102 == 7 && (a168 == 7 && (input == 4 && cf == 1)) && a112 == 34)) {
                    cf = 0;
                    a9 = 34;
                    a102 = 10;
                    a40 = 35;
                    a98 = 8;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (input == 8 && (a130 == 9 && (cf == 1 && a112 == 34) && a168 == 7))) {
                    cf = 0;
                    a112 = 32;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (input == 1 && (a168 == 7 && cf == 1 && a130 == 9 && a112 == 34))) {
                    cf = 0;
                    a102 = 11;
                    a314 = 34;
                    a106 = 35;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a168 == 7 && (a130 == 9 && (a112 == 34 && (input == 10 && cf == 1))) && a102 == 7) {
                    cf = 0;
                    a102 = 8;
                    a289 = 34;
                    a57 = 36;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a130 == 9 && (a168 == 7 && (input == 6 && cf == 1) && a112 == 34 && a102 == 7)) {
                    cf = 0;
                    a130 = 10;
                    a89 = 32;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a112 == 35 && cf == 1) {
              if (a116 == 4 && cf == 1) {{
                  if (a102 == 7 && (a112 == 35 && cf == 1) && a130 == 9 && a116 == 4 && input == 6) {
                    cf = 0;
                    a102 = 13;
                    a270 = 34;
                    a9 = 32;
                    a183 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a116 == 5) {{
                  if (cf == 1 && a116 == 5 && a112 == 35 && a102 == 7 && input == 3 && a130 == 9) {
                    cf = 0;
                    a116 = 8;
                    a385 = 33;
                    a102 = 12;
                    a87 = 17;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a130 == 9 && (cf == 1 && a116 == 5 && a102 == 7 && input == 7 && a112 == 35)) {
                    cf = 0;
                    a116 = 6;
                    a22 = 36;
                    a102 = 12;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a116 == 5 && (a130 == 9 && (cf == 1 && a102 == 7 && a112 == 35) && input == 10)) {
                    cf = 0;
                    a116 = 6;
                    a22 = 36;
                    a102 = 12;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a112 == 36) {
              if (cf == 1 && a121 == 33) {{
                  if (input == 7 && (cf == 1 && a121 == 33) && a102 == 7 && a112 == 36 && a130 == 9) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (a121 == 34 && cf == 1) {{
                  if (a121 == 34 && (a112 == 36 && (a102 == 7 && (cf == 1 && a130 == 9 && input == 7)))) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a130 == 9 && (a121 == 34 && (cf == 1 && input == 10) && a102 == 7 && a112 == 36)) {
                    cf = 0;
                    a20 = 5;
                    a249 = 34;
                    a102 = 9;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a130 == 9 && (a112 == 36 && (a102 == 7 && cf == 1)) && input == 1 && a121 == 34) {
                    cf = 0;
                    a102 = 12;
                    a22 = 36;
                    a116 = 6;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a130 == 10) {{
            if (cf == 1 && a143 == 5) {
              if (cf == 1 && a49 == 6) {{
                  if (a49 == 6 && (input == 3 && (a102 == 7 && cf == 1 && a143 == 5) && a130 == 10)) {
                    cf = 0;
                    a102 = 12;
                    a57 = 36;
                    a116 = 2;
                    a7 = 4;
                    output = 19;
                    fflush(stdout);
                  }
                  if (cf == 1 && a102 == 7 && input == 8 && a130 == 10 && a143 == 5 && a49 == 6) {
                    cf = 0;
                    a130 = 3;
                    a168 = 2;
                    a86 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (a49 == 7 && cf == 1) {{
                  if (a102 == 7 && (a49 == 7 && cf == 1 && a143 == 5 && a130 == 10) && input == 7) {
                    cf = 0;
                    a49 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a49 == 7 && (cf == 1 && input == 5 && a102 == 7 && a130 == 10) && a143 == 5) {
                    cf = 0;
                    a102 = 14;
                    a310 = 15;
                    a71 = 12;
                    a165 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a49 == 8 && cf == 1) {{
                  if (a49 == 8 && (a143 == 5 && (cf == 1 && a102 == 7)) && a130 == 10 && input == 2) {
                    cf = 0;
                    a270 = 35;
                    a102 = 13;
                    a157 = 36;
                    a170 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a49 == 8 && (a130 == 10 && cf == 1 && a143 == 5) && a102 == 7 && input == 1) {
                    cf = 0;
                    a83 = 34;
                    a314 = 35;
                    a102 = 11;
                    a117 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a49 == 8 && (input == 4 && (a102 == 7 && (cf == 1 && a143 == 5)) && a130 == 10)) {
                    cf = 0;
                    a78 = 36;
                    a314 = 36;
                    a102 = 11;
                    a265 = 3;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (input == 8 && (a130 == 10 && cf == 1 && a49 == 8)) && a143 == 5) {
                    cf = 0;
                    a289 = 32;
                    a182 = 10;
                    a102 = 8;
                    a195 = 10;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a143 == 6) {
              if (cf == 1 && a123 == 32) {{
                  if (a123 == 32 && (cf == 1 && a130 == 10 && a143 == 6) && a102 == 7 && input == 6) {
                    cf = 0;
                    a102 = 12;
                    a74 = 32;
                    a116 = 7;
                    a28 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (cf == 1 && a123 == 32 && input == 3 && a130 == 10 && a102 == 7 && a143 == 6) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 7 && (a123 == 32 && (a143 == 6 && (a102 == 7 && (a130 == 10 && cf == 1))))) {
                    cf = 0;
                    a102 = 12;
                    a87 = 14;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a143 == 7 && cf == 1) {
              if (32 == 32 && cf == 1) {{
                  if (a143 == 7 && (32 == 32 && cf == 1) && a130 == 10 && input == 2 && a102 == 7) {
                    cf = 0;
                    a112 = 33;
                    a244 = 34;
                    a130 = 9;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a143 == 8) {
              if (cf == 1 && a99 == 6) {{
                  if (a102 == 7 && (a99 == 6 && (input == 1 && cf == 1) && a130 == 10 && a143 == 8)) {
                    cf = 0;
                    a102 = 8;
                    a70 = 32;
                    a289 = 32;
                    a182 = 16;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a99 == 7) {{
                  if (input == 6 && cf == 1 && a130 == 10 && a99 == 7 && a102 == 7 && a143 == 8) {
                    cf = 0;
                    a183 = 4;
                    a102 = 9;
                    a20 = 1;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a143 == 8 && (a99 == 7 && cf == 1) && input == 1 && a130 == 10 && a102 == 7) {
                    cf = 0;
                    a102 = 9;
                    a183 = 4;
                    a20 = 1;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a130 == 10 && (input == 8 && (a99 == 7 && (a143 == 8 && (cf == 1 && a102 == 7))))) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a143 == 8 && (input == 4 && (a99 == 7 && cf == 1) && a130 == 10))) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a143 == 9) {
              if (a89 == 32 && cf == 1) {{
                  if (a143 == 9 && (cf == 1 && a130 == 10 && a102 == 7 && input == 6) && a89 == 32) {
                    cf = 0;
                    a102 = 9;
                    a306 = 4;
                    a20 = 4;
                    a28 = 9;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
              if (a89 == 36 && cf == 1) {{
                  if (a130 == 10 && (a102 == 7 && (input == 4 && (cf == 1 && a143 == 9)) && a89 == 36)) {
                    cf = 0;
                    a112 = 34;
                    a130 = 9;
                    a168 = 2;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 1 && (a143 == 9 && (cf == 1 && a130 == 10) && a89 == 36 && a102 == 7)) {
                    cf = 0;
                    a122 = 34;
                    a102 = 14;
                    a310 = 13;
                    a242 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a102 == 7 && (a130 == 10 && (cf == 1 && a143 == 9 && input == 5 && a89 == 36))) {
                    cf = 0;
                    a102 = 12;
                    a116 = 4;
                    a67 = 36;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a143 == 10 && cf == 1) {
              if (a49 == 4 && cf == 1) {{
                  if (a143 == 10 && (a102 == 7 && (input == 7 && (cf == 1 && a49 == 4)) && a130 == 10)) {
                    cf = 0;
                    a67 = 35;
                    a385 = 36;
                    a40 = 32;
                    a102 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a49 == 9) {{
                  if (a143 == 10 && cf == 1 && a130 == 10 && a102 == 7 && input == 4 && a49 == 9) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a130 == 10 && (input == 3 && cf == 1) && a102 == 7 && a49 == 9 && a143 == 10) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 6 && (a130 == 10 && (a143 == 10 && cf == 1 && a102 == 7) && a49 == 9)) {
                    cf = 0;
                    a91 = 32;
                    a296 = 35;
                    a102 = 14;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (cf == 1 && a102 == 8) {
        if (cf == 1 && a289 == 33) {{
            if (cf == 1 && a75 == 33) {
              if (a67 == 35 && cf == 1) {{
                  if (a67 == 35 && (a289 == 33 && (input == 5 && cf == 1 && a75 == 33)) && a102 == 8) {
                    cf = 0;
                    a270 = 32;
                    a35 = 36;
                    a102 = 13;
                    a69 = 8;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a75 == 33 && cf == 1 && a67 == 35 && a102 == 8 && a289 == 33 && input == 1) {
                    cf = 0;
                    a130 = 10;
                    a89 = 36;
                    a102 = 7;
                    a143 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 8 && cf == 1 && a67 == 35 && a289 == 33 && a75 == 33 && input == 10) {
                    cf = 0;
                    a129 = 32;
                    a78 = 35;
                    a102 = 9;
                    a20 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a75 == 34) {
              if (a141 == 7 && cf == 1) {{
                  if (input == 6 && (a289 == 33 && (a141 == 7 && (a75 == 34 && (a102 == 8 && cf == 1))))) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a75 == 34 && (input == 3 && cf == 1 && a102 == 8 && a141 == 7) && a289 == 33) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a75 == 35 && cf == 1) {
              if (cf == 1 && a20 == 2) {{
                  if (a289 == 33 && (a20 == 2 && (a102 == 8 && cf == 1) && a75 == 35) && input == 1) {
                    cf = 0;
                    a270 = 32;
                    a81 = 32;
                    a102 = 13;
                    a69 = 9;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a289 == 33 && (input == 7 && (a102 == 8 && (cf == 1 && a75 == 35) && a20 == 2))) {
                    cf = 0;
                    a112 = 34;
                    a102 = 7;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 8 && (a289 == 33 && (cf == 1 && a75 == 35) && a20 == 2 && a102 == 8)) {
                    cf = 0;
                    a314 = 34;
                    a106 = 35;
                    a102 = 11;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (a20 == 7 && cf == 1) {{
                  if (a289 == 33 && (a75 == 35 && (a102 == 8 && (cf == 1 && input == 2 && a20 == 7)))) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 10 && (cf == 1 && a75 == 35 && a289 == 33) && a102 == 8 && a20 == 7) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a75 == 36 && cf == 1) {
              if (cf == 1 && a52 == 34) {{
                  if (a52 == 34 && (a102 == 8 && (cf == 1 && a289 == 33)) && a75 == 36 && input == 3) {
                    cf = 0;
                    a20 = 7;
                    a102 = 9;
                    a67 = 36;
                    a62 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a289 == 32) {{
            if (a182 == 9 && cf == 1) {
              if (a36 == 10 && cf == 1) {{
                  if (a102 == 8 && (a182 == 9 && (input == 2 && cf == 1 && a36 == 10) && a289 == 32)) {
                    cf = 0;
                    a289 = 36;
                    a146 = 33;
                    a195 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a182 == 9 && (input == 6 && (a36 == 10 && (a102 == 8 && cf == 1 && a289 == 32)))) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a182 == 10 && cf == 1) {
              if (a195 == 10 && cf == 1) {{
                  if (a289 == 32 && (a182 == 10 && (a102 == 8 && cf == 1 && input == 6)) && a195 == 10) {
                    cf = 0;
                    a232 = 36;
                    a270 = 33;
                    a102 = 12;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a195 == 10 && (input == 4 && (a102 == 8 && (cf == 1 && a182 == 10 && a289 == 32)))) {
                    cf = 0;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a102 == 8 && (input == 8 && (a195 == 10 && (a182 == 10 && (a289 == 32 && cf == 1))))) {
                    cf = 0;
                    a270 = 33;
                    a102 = 12;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 11) {
              if (cf == 1 && a200 == 6) {{
                  if (a289 == 32 && (a102 == 8 && (a182 == 11 && (a200 == 6 && (input == 10 && cf == 1))))) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a200 == 7) {{
                  if (a182 == 11 && (a200 == 7 && (a102 == 8 && (cf == 1 && a289 == 32 && input == 6)))) {
                    cf = 0;
                    a57 = 36;
                    a289 = 34;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a182 == 13 && cf == 1) {
              if (cf == 1 && a3 == 8) {{
                  if (a102 == 8 && (a3 == 8 && (a182 == 13 && cf == 1 && a289 == 32) && input == 8)) {
                    cf = 0;
                    a289 = 36;
                    a146 = 35;
                    a17 = 5;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a3 == 8 && (a289 == 32 && (cf == 1 && a182 == 13 && a102 == 8 && input == 3))) {
                    cf = 0;
                    a270 = 34;
                    a102 = 13;
                    a9 = 34;
                    a47 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (input == 4 && (a289 == 32 && (a182 == 13 && cf == 1) && a3 == 8 && a102 == 8)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 8;
                    a99 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a289 == 32 && (a3 == 8 && (a102 == 8 && (a182 == 13 && cf == 1 && input == 7)))) {
                    cf = 0;
                    a20 = 5;
                    a102 = 9;
                    a249 = 33;
                    a194 = 10;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a3 == 13) {{
                  if (a182 == 13 && (a289 == 32 && (cf == 1 && a3 == 13 && a102 == 8)) && input == 1) {
                    cf = 0;
                    a37 = 35;
                    a130 = 6;
                    a102 = 7;
                    a248 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a102 == 8 && (a289 == 32 && (a182 == 13 && cf == 1) && a3 == 13) && input == 10) {
                    cf = 0;
                    a102 = 12;
                    a116 = 4;
                    a67 = 35;
                    a62 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 14) {
              if (cf == 1 && a184 == 5) {{
                  if (a289 == 32 && (a182 == 14 && (cf == 1 && a184 == 5 && input == 8 && a102 == 8))) {
                    cf = 0;
                    a121 = 33;
                    a102 = 7;
                    a112 = 36;
                    a130 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a182 == 14 && (a184 == 5 && (input == 2 && (cf == 1 && a289 == 32) && a102 == 8))) {
                    cf = 0;
                    a164 = 33;
                    a102 = 12;
                    a74 = 36;
                    a116 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a184 == 10) {{
                  if (a289 == 32 && (a182 == 14 && (a184 == 10 && (cf == 1 && a102 == 8)) && input == 2)) {
                    cf = 0;
                    a314 = 34;
                    a106 = 36;
                    a102 = 11;
                    a60 = 12;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a184 == 10 && cf == 1 && a182 == 14 && input == 3 && a289 == 32 && a102 == 8) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a179 = 33;
                    a125 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a182 == 15) {
              if (cf == 1 && a144 == 35) {{
                  if (a102 == 8 && cf == 1 && input == 10 && a144 == 35 && a289 == 32 && a182 == 15) {
                    cf = 0;
                    a314 = 35;
                    a83 = 34;
                    a102 = 11;
                    a117 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a289 == 32 && (a144 == 35 && cf == 1 && input == 5) && a182 == 15 && a102 == 8) {
                    cf = 0;
                    a115 = 35;
                    a102 = 7;
                    a130 = 4;
                    a17 = 8;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a182 == 15 && (a102 == 8 && (a144 == 35 && cf == 1)) && input == 4 && a289 == 32) {
                    cf = 0;
                    a102 = 12;
                    a116 = 4;
                    a67 = 36;
                    a310 = 15;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a182 == 16 && cf == 1) {
              if (a70 == 32 && cf == 1) {{
                  if (input == 6 && (a289 == 32 && (a182 == 16 && cf == 1 && a102 == 8)) && a70 == 32) {
                    cf = 0;
                    a196 = 32;
                    a16 = 34;
                    a314 = 33;
                    a102 = 11;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a70 == 32 && (a102 == 8 && (cf == 1 && input == 8)) && a182 == 16 && a289 == 32) {
                    cf = 0;
                    a102 = 9;
                    a20 = 7;
                    a67 = 36;
                    a62 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (cf == 1 && a182 == 16 && a289 == 32 && input == 10 && a70 == 32 && a102 == 8) {
                    cf = 0;
                    a102 = 7;
                    a143 = 8;
                    a130 = 10;
                    a99 = 6;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a289 == 34) {{
            if (a57 == 33 && cf == 1) {
              if (cf == 1 && a270 == 32) {{
                  if (a102 == 8 && (a270 == 32 && (a57 == 33 && (a289 == 34 && cf == 1))) && input == 3) {
                    cf = 0;
                    a146 = 35;
                    a289 = 36;
                    a17 = 9;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a289 == 34 && (input == 6 && (a102 == 8 && cf == 1 && a57 == 33 && a270 == 32))) {
                    cf = 0;
                    a87 = 12;
                    a102 = 12;
                    a116 = 8;
                    a185 = 8;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a57 == 33 && (a289 == 34 && (a270 == 32 && (input == 7 && cf == 1) && a102 == 8))) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a183 = 4;
                    a17 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 8 && (a289 == 34 && (a57 == 33 && (cf == 1 && input == 8) && a270 == 32))) {
                    cf = 0;
                    a130 = 4;
                    a102 = 7;
                    a115 = 32;
                    a17 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a57 == 32) {
              if (cf == 1 && a335 == 11) {{
                  if (a289 == 34 && (a335 == 11 && (cf == 1 && a57 == 32 && input == 2)) && a102 == 8) {
                    cf = 0;
                    a248 = 14;
                    a130 = 6;
                    a102 = 7;
                    a365 = 12;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a289 == 34 && (cf == 1 && a335 == 11 && a57 == 32 && input == 7) && a102 == 8) {
                    cf = 0;
                    a197 = 32;
                    a102 = 11;
                    a314 = 32;
                    a170 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a57 == 36) {
              if (cf == 1 && a138 == 13) {{
                  if (a138 == 13 && (cf == 1 && a57 == 36 && a289 == 34 && a102 == 8) && input == 7) {
                    cf = 0;
                    a100 = 36;
                    a102 = 7;
                    a130 = 3;
                    a168 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a138 == 15) {{
                  if (a138 == 15 && (a102 == 8 && (cf == 1 && a289 == 34) && input == 6) && a57 == 36) {
                    cf = 0;
                    a17 = 6;
                    a102 = 7;
                    a130 = 4;
                    a143 = 5;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a289 == 34 && (a102 == 8 && (a138 == 15 && (input == 7 && cf == 1 && a57 == 36)))) {
                    cf = 0;
                    a310 = 9;
                    a125 = 12;
                    a102 = 14;
                    a23 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a289 == 34 && (a57 == 36 && (input == 10 && (cf == 1 && a102 == 8)) && a138 == 15)) {
                    cf = 0;
                    a182 = 11;
                    a289 = 32;
                    a200 = 7;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a289 == 35) {{
            if (cf == 1 && a249 == 34) {
              if (cf == 1 && a111 == 8) {{
                  if (a102 == 8 && (input == 3 && (a111 == 8 && (a289 == 35 && (a249 == 34 && cf == 1))))) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a111 == 8 && (a289 == 35 && (a249 == 34 && cf == 1 && input == 6) && a102 == 8)) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a111 == 8 && (input == 8 && (a102 == 8 && cf == 1 && a249 == 34 && a289 == 35))) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a289 == 35 && (a102 == 8 && (cf == 1 && input == 4) && a249 == 34) && a111 == 8) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a249 == 36) {
              if (cf == 1 && a19 == 32) {{
                  if (a102 == 8 && (a289 == 35 && (a249 == 36 && cf == 1 && a19 == 32)) && input == 8) {
                    cf = 0;
                    a270 = 36;
                    a344 = 8;
                    a102 = 13;
                    a153 = 4;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a289 == 35 && (a102 == 8 && (cf == 1 && input == 7) && a249 == 36 && a19 == 32)) {
                    cf = 0;
                    a12 = 34;
                    a20 = 2;
                    a102 = 9;
                    a63 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a249 == 36 && (a19 == 32 && (cf == 1 && a102 == 8 && input == 2) && a289 == 35)) {
                    cf = 0;
                    a385 = 34;
                    a72 = 33;
                    a40 = 32;
                    a102 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a289 == 36) {{
            if (cf == 1 && a146 == 33) {
              if (a195 == 9 && cf == 1) {{
                  if (a289 == 36 && (a146 == 33 && (a102 == 8 && (cf == 1 && input == 3)) && a195 == 9)) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a195 == 9 && (a289 == 36 && (a146 == 33 && (cf == 1 && a102 == 8))) && input == 7) {
                    cf = 0;
                    a102 = 10;
                    a40 = 33;
                    a178 = 7;
                    a271 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a289 == 36 && (cf == 1 && input == 2 && a146 == 33 && a195 == 9 && a102 == 8)) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a195 == 12) {{
                  if (a195 == 12 && (a289 == 36 && (input == 4 && cf == 1 && a146 == 33) && a102 == 8)) {
                    cf = 0;
                    a9 = 35;
                    a102 = 10;
                    a40 = 35;
                    a98 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 6 && (a102 == 8 && (a195 == 12 && (cf == 1 && a146 == 33)) && a289 == 36)) {
                    cf = 0;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a146 == 33 && (a102 == 8 && (a289 == 36 && (cf == 1 && input == 10 && a195 == 12)))) {
                    cf = 0;
                    a296 = 35;
                    a102 = 14;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a289 == 36 && (a195 == 12 && (input == 8 && cf == 1 && a146 == 33) && a102 == 8)) {
                    cf = 0;
                    a40 = 35;
                    a102 = 10;
                    a9 = 35;
                    a98 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a146 == 32) {
              if (cf == 1 && a93 == 7) {{
                  if (input == 3 && (a93 == 7 && (a102 == 8 && (a146 == 32 && (a289 == 36 && cf == 1))))) {
                    cf = 0;
                    a102 = 12;
                    a67 = 32;
                    a116 = 4;
                    a98 = 5;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a146 == 32 && (a93 == 7 && (cf == 1 && input == 4)) && a102 == 8 && a289 == 36) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a146 == 32 && (input == 7 && (a289 == 36 && cf == 1) && a93 == 7) && a102 == 8) {
                    cf = 0;
                    a67 = 32;
                    a116 = 4;
                    a102 = 12;
                    a98 = 5;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 6 && (cf == 1 && a93 == 7 && a146 == 32 && a289 == 36) && a102 == 8) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a146 == 35 && cf == 1) {
              if (cf == 1 && a17 == 5) {{
                  if (a146 == 35 && (a17 == 5 && cf == 1) && input == 10 && a289 == 36 && a102 == 8) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a146 == 35 && (a17 == 5 && cf == 1 && a102 == 8 && input == 7 && a289 == 36)) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a17 == 9 && cf == 1) {{
                  if (a102 == 8 && (cf == 1 && input == 6 && a289 == 36) && a146 == 35 && a17 == 9) {
                    cf = 0;
                    a102 = 12;
                    a5 = 34;
                    a19 = 35;
                    a116 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 9 && cf == 1) {
        if (a20 == 1 && cf == 1) {{
            if (cf == 1 && a183 == 4) {
              if (cf == 1 && a17 == 7) {{
                  if (input == 6 && (a20 == 1 && (cf == 1 && a183 == 4) && a17 == 7) && a102 == 9) {
                    cf = 0;
                    a102 = 14;
                    a126 = 33;
                    a190 = 34;
                    a310 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a20 == 1 && (input == 7 && (cf == 1 && a17 == 7))) && a183 == 4) {
                    cf = 0;
                    a289 = 32;
                    a182 = 13;
                    a102 = 8;
                    a3 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a17 == 7 && (input == 10 && (a102 == 9 && (a20 == 1 && (cf == 1 && a183 == 4))))) {
                    cf = 0;
                    a102 = 10;
                    a385 = 32;
                    a40 = 32;
                    a323 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a17 == 7 && (a102 == 9 && (input == 4 && (a20 == 1 && (cf == 1 && a183 == 4))))) {
                    cf = 0;
                    a20 = 7;
                    a62 = 3;
                    a171 = 13;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
              if (a17 == 10 && cf == 1) {{
                  if (a20 == 1 && (a17 == 10 && (a102 == 9 && cf == 1 && a183 == 4)) && input == 10) {
                    cf = 0;
                    a270 = 34;
                    a102 = 13;
                    a9 = 32;
                    a183 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 1 && (a17 == 10 && (cf == 1 && a20 == 1) && a183 == 4) && a102 == 9) {
                    cf = 0;
                    a130 = 6;
                    a37 = 35;
                    a102 = 7;
                    a248 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a183 == 5 && cf == 1) {
              if (33 == 33 && cf == 1) {{
                  if (a102 == 9 && (a183 == 5 && (input == 8 && (cf == 1 && a20 == 1 && 33 == 33)))) {
                    cf = 0;
                    a12 = 35;
                    a20 = 2;
                    a138 = 9;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a183 == 5 && (33 == 33 && (input == 5 && cf == 1) && a20 == 1) && a102 == 9) {
                    cf = 0;
                    a116 = 8;
                    a87 = 11;
                    a102 = 12;
                    a364 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a20 == 1 && cf == 1 && input == 2 && 33 == 33 && a183 == 5 && a102 == 9) {
                    cf = 0;
                    a102 = 7;
                    a168 = 2;
                    a130 = 3;
                    a86 = 7;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a183 == 6) {
              if (cf == 1 && a42 == 14) {{
                  if (a20 == 1 && (a102 == 9 && (input == 1 && (a183 == 6 && cf == 1))) && a42 == 14) {
                    cf = 0;
                    a40 = 36;
                    a24 = 32;
                    a102 = 10;
                    output = 19;
                    fflush(stdout);
                  }
                  if (cf == 1 && input == 4 && a42 == 14 && a183 == 6 && a20 == 1 && a102 == 9) {
                    cf = 0;
                    a271 = 12;
                    a20 = 8;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a20 == 1 && (a183 == 6 && (cf == 1 && input == 2))) && a42 == 14) {
                    cf = 0;
                    a143 = 5;
                    a130 = 10;
                    a102 = 7;
                    a49 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a183 == 7 && cf == 1) {
              if (a165 == 10 && cf == 1) {{
                  if (a165 == 10 && (input == 2 && (a183 == 7 && (a102 == 9 && cf == 1)) && a20 == 1)) {
                    cf = 0;
                    a83 = 33;
                    a102 = 11;
                    a314 = 35;
                    a125 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a165 == 10 && (a183 == 7 && cf == 1) && a102 == 9 && input == 10 && a20 == 1) {
                    cf = 0;
                    a20 = 2;
                    a12 = 32;
                    a87 = 17;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a183 == 7 && (input == 6 && cf == 1) && a165 == 10 && a102 == 9 && a20 == 1) {
                    cf = 0;
                    a385 = 33;
                    a116 = 8;
                    a102 = 12;
                    a87 = 17;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a183 == 8) {
              if (a144 == 35 && cf == 1) {{
                  if (a102 == 9 && (a20 == 1 && (cf == 1 && a144 == 35 && input == 1)) && a183 == 8) {
                    cf = 0;
                    a52 = 34;
                    a289 = 33;
                    a75 = 36;
                    a102 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a20 == 1 && (a102 == 9 && (a183 == 8 && (input == 2 && (a144 == 35 && cf == 1))))) {
                    cf = 0;
                    a310 = 11;
                    a102 = 14;
                    a120 = 32;
                    a0 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 2) {{
            if (cf == 1 && a12 == 33) {
              if (a36 == 13 && cf == 1) {{
                  if (input == 8 && (a102 == 9 && (a20 == 2 && (cf == 1 && a36 == 13) && a12 == 33))) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a56 = 16;
                    a159 = 10;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a20 == 2 && (a36 == 13 && cf == 1 && a12 == 33 && input == 4) && a102 == 9) {
                    cf = 0;
                    a385 = 34;
                    a72 = 35;
                    a40 = 32;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a12 == 32) {
              if (a87 == 15 && cf == 1) {{
                  if (input == 7 && (a87 == 15 && (a12 == 32 && (cf == 1 && a102 == 9)) && a20 == 2)) {
                    cf = 0;
                    a91 = 32;
                    a296 = 35;
                    a102 = 14;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a20 == 2 && (a102 == 9 && (a12 == 32 && (cf == 1 && input == 3) && a87 == 15))) {
                    cf = 0;
                    a102 = 12;
                    a56 = 14;
                    a116 = 6;
                    a49 = 3;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a12 == 32 && (a20 == 2 && (input == 10 && (cf == 1 && a102 == 9) && a87 == 15))) {
                    cf = 0;
                    a296 = 35;
                    a102 = 14;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a12 == 32 && cf == 1) && a20 == 2 && a87 == 15 && input == 6) {
                    cf = 0;
                    a296 = 35;
                    a102 = 14;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a12 == 34 && cf == 1) {
              if (cf == 1 && a63 == 11) {{
                  if (a12 == 34 && (a102 == 9 && (cf == 1 && a20 == 2)) && input == 7 && a63 == 11) {
                    cf = 0;
                    a102 = 7;
                    a130 = 3;
                    a168 = 3;
                    a15 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a63 == 12) {{
                  if (a63 == 12 && (a20 == 2 && (a12 == 34 && cf == 1 && input == 6 && a102 == 9))) {
                    cf = 0;
                    a129 = 36;
                    a20 = 3;
                    a96 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a63 == 12 && (a20 == 2 && (input == 2 && (a102 == 9 && cf == 1)) && a12 == 34)) {
                    cf = 0;
                    a102 = 8;
                    a182 = 9;
                    a289 = 32;
                    a36 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a12 == 35) {
              if (a138 == 9 && cf == 1) {{
                  if (a12 == 35 && (cf == 1 && a20 == 2) && a102 == 9 && input == 6 && a138 == 9) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a12 == 35 && (a138 == 9 && (cf == 1 && input == 4) && a20 == 2) && a102 == 9) {
                    cf = 0;
                    a102 = 12;
                    a270 = 33;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 3) {{
            if (cf == 1 && a129 == 33) {
              if (a344 == 10 && cf == 1) {{
                  if (a129 == 33 && (input == 8 && (cf == 1 && a102 == 9 && a344 == 10)) && a20 == 3) {
                    cf = 0;
                    a190 = 34;
                    a126 = 33;
                    a102 = 14;
                    a310 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a344 == 10 && (a129 == 33 && (input == 1 && (a20 == 3 && cf == 1))))) {
                    cf = 0;
                    a190 = 34;
                    a102 = 14;
                    a126 = 33;
                    a310 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a129 == 33 && (a20 == 3 && cf == 1 && a344 == 10)) && input == 4) {
                    cf = 0;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a344 == 10 && (a102 == 9 && (a129 == 33 && cf == 1 && a20 == 3)) && input == 7) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (a344 == 11 && cf == 1) {{
                  if (a102 == 9 && (a20 == 3 && (cf == 1 && input == 5 && a344 == 11) && a129 == 33)) {
                    cf = 0;
                    a67 = 36;
                    a102 = 12;
                    a116 = 4;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (cf == 1 && a20 == 3 && a129 == 33) && input == 1 && a344 == 11) {
                    cf = 0;
                    a122 = 34;
                    a102 = 14;
                    a310 = 13;
                    a242 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 4 && cf == 1 && a129 == 33 && a20 == 3 && a102 == 9 && a344 == 11) {
                    cf = 0;
                    a176 = 34;
                    a102 = 14;
                    a310 = 16;
                    a364 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a344 == 13) {{
                  if (a102 == 9 && (a344 == 13 && (a20 == 3 && (a129 == 33 && cf == 1))) && input == 10) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a20 == 3 && (a102 == 9 && (cf == 1 && a344 == 13 && a129 == 33 && input == 7))) {
                    cf = 0;
                    a130 = 4;
                    a102 = 7;
                    a17 = 9;
                    a195 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a129 == 32 && cf == 1) {
              if (cf == 1 && a78 == 35) {{
                  if (a78 == 35 && (a129 == 32 && (a102 == 9 && (a20 == 3 && cf == 1))) && input == 5) {
                    cf = 0;
                    a310 = 9;
                    a102 = 14;
                    a125 = 15;
                    a182 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a129 == 32 && (input == 2 && (a102 == 9 && (a78 == 35 && cf == 1))) && a20 == 3) {
                    cf = 0;
                    a310 = 9;
                    a125 = 15;
                    a102 = 14;
                    a182 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a129 == 34 && cf == 1) {
              if (a62 == 10 && cf == 1) {{
                  if (a20 == 3 && (a102 == 9 && (a62 == 10 && (cf == 1 && input == 5 && a129 == 34)))) {
                    cf = 0;
                    a106 = 32;
                    a102 = 11;
                    a314 = 34;
                    a54 = 16;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a20 == 3 && (cf == 1 && a62 == 10 && a102 == 9 && a129 == 34 && input == 10)) {
                    cf = 0;
                    a178 = 5;
                    a102 = 10;
                    a40 = 33;
                    a170 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a129 == 35 && cf == 1) {
              if (cf == 1 && a23 == 15) {{
                  if (a20 == 3 && (a23 == 15 && (a129 == 35 && (cf == 1 && a102 == 9 && input == 1)))) {
                    cf = 0;
                    a314 = 34;
                    a78 = 36;
                    a106 = 33;
                    a102 = 11;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a23 == 15 && (a20 == 3 && (input == 10 && (a129 == 35 && cf == 1)) && a102 == 9)) {
                    cf = 0;
                    a56 = 10;
                    a116 = 6;
                    a102 = 12;
                    a377 = 8;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a129 == 36 && cf == 1) {
              if (cf == 1 && a96 == 12) {{
                  if (a102 == 9 && (a96 == 12 && (input == 8 && cf == 1) && a20 == 3) && a129 == 36) {
                    cf = 0;
                    a102 = 13;
                    a9 = 32;
                    a270 = 34;
                    a183 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 4) {{
            if (cf == 1 && a306 == 4) {
              if (cf == 1 && a28 == 2) {{
                  if (a20 == 4 && (input == 1 && cf == 1 && a306 == 4 && a28 == 2) && a102 == 9) {
                    cf = 0;
                    a289 = 33;
                    a102 = 8;
                    a75 = 34;
                    a141 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (a28 == 9 && cf == 1) {{
                  if (a102 == 9 && (input == 1 && (a306 == 4 && cf == 1)) && a20 == 4 && a28 == 9) {
                    cf = 0;
                    a89 = 32;
                    a102 = 7;
                    a130 = 10;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a20 == 4 && (cf == 1 && a306 == 4) && a28 == 9) && input == 8) {
                    cf = 0;
                    a232 = 36;
                    a102 = 12;
                    a270 = 33;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 3 && (a28 == 9 && (a102 == 9 && (a306 == 4 && cf == 1) && a20 == 4))) {
                    cf = 0;
                    a89 = 32;
                    a102 = 7;
                    a130 = 10;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 10 && (a20 == 4 && (a306 == 4 && (a102 == 9 && cf == 1) && a28 == 9))) {
                    cf = 0;
                    a222 = 34;
                    a102 = 13;
                    a270 = 36;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a306 == 6 && cf == 1) {
              if (a117 == 12 && cf == 1) {{
                  if (a102 == 9 && (a306 == 6 && (a20 == 4 && (a117 == 12 && cf == 1) && input == 2))) {
                    cf = 0;
                    a249 = 33;
                    a20 = 5;
                    a194 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a20 == 4 && (cf == 1 && input == 10) && a102 == 9 && a306 == 6 && a117 == 12) {
                    cf = 0;
                    a74 = 36;
                    a102 = 12;
                    a164 = 33;
                    a116 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a306 == 8) {
              if (32 == 32 && cf == 1) {{
                  if (a102 == 9 && (a20 == 4 && (a306 == 8 && (cf == 1 && 32 == 32) && input == 1))) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (32 == 32 && (input == 3 && (cf == 1 && a102 == 9 && a20 == 4)) && a306 == 8) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a20 == 4 && (a102 == 9 && (input == 7 && (cf == 1 && a306 == 8))) && 32 == 32) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a306 == 9 && cf == 1) {
              if (cf == 1 && a2 == 6) {{
                  if (a306 == 9 && (a102 == 9 && (a20 == 4 && cf == 1) && a2 == 6) && input == 3) {
                    cf = 0;
                    a170 = 14;
                    a102 = 13;
                    a270 = 35;
                    a96 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (a2 == 11 && cf == 1) {{
                  if (a102 == 9 && (a20 == 4 && (a306 == 9 && cf == 1) && input == 5 && a2 == 11)) {
                    cf = 0;
                    a102 = 14;
                    a310 = 15;
                    a296 = 33;
                    a71 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a306 == 11 && cf == 1) {
              if (a77 == 5 && cf == 1) {{
                  if (a102 == 9 && (input == 2 && cf == 1 && a20 == 4) && a306 == 11 && a77 == 5) {
                    cf = 0;
                    a157 = 36;
                    a270 = 35;
                    a102 = 13;
                    a170 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 8 && (a102 == 9 && (cf == 1 && a306 == 11) && a20 == 4 && a77 == 5)) {
                    cf = 0;
                    a289 = 32;
                    a182 = 10;
                    a102 = 8;
                    a195 = 10;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 1 && cf == 1 && a20 == 4 && a77 == 5 && a306 == 11 && a102 == 9) {
                    cf = 0;
                    a183 = 6;
                    a20 = 1;
                    a42 = 14;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a77 == 5 && (a102 == 9 && (input == 4 && cf == 1 && a20 == 4 && a306 == 11))) {
                    cf = 0;
                    a78 = 36;
                    a102 = 11;
                    a314 = 36;
                    a265 = 3;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (a77 == 6 && cf == 1) {{
                  if (a306 == 11 && (input == 7 && (a77 == 6 && (a20 == 4 && cf == 1 && a102 == 9)))) {
                    cf = 0;
                    a24 = 36;
                    a102 = 10;
                    a40 = 36;
                    a184 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a77 == 6 && (a20 == 4 && (cf == 1 && a102 == 9 && input == 6)) && a306 == 11) {
                    cf = 0;
                    a116 = 5;
                    a5 = 32;
                    a102 = 12;
                    a117 = 8;
                    output = 21;
                    fflush(stdout);
                  }
                  if (input == 10 && (a20 == 4 && (a102 == 9 && cf == 1) && a77 == 6 && a306 == 11)) {
                    cf = 0;
                    a129 = 34;
                    a20 = 3;
                    a62 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a306 == 11 && (input == 1 && (a20 == 4 && (a102 == 9 && cf == 1) && a77 == 6))) {
                    cf = 0;
                    a106 = 33;
                    a116 = 6;
                    a102 = 12;
                    a56 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a77 == 7) {{
                  if (input == 6 && (a102 == 9 && (a306 == 11 && cf == 1) && a20 == 4) && a77 == 7) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a306 == 11 && (a20 == 4 && (a77 == 7 && cf == 1 && input == 1)))) {
                    cf = 0;
                    a91 = 36;
                    a296 = 35;
                    a102 = 14;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a77 == 7 && cf == 1 && a20 == 4 && a102 == 9 && a306 == 11 && input == 10) {
                    cf = 0;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (a77 == 7 && (a20 == 4 && (input == 8 && cf == 1) && a306 == 11))) {
                    cf = 0;
                    a20 = 1;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 5) {{
            if (a249 == 33 && cf == 1) {
              if (a194 == 6 && cf == 1) {{
                  if (a102 == 9 && (input == 4 && cf == 1) && a20 == 5 && a194 == 6 && a249 == 33) {
                    cf = 0;
                    a102 = 7;
                    a168 = 6;
                    a130 = 3;
                    a242 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                  if (cf == 1 && input == 2 && a20 == 5 && a102 == 9 && a249 == 33 && a194 == 6) {
                    cf = 0;
                    a168 = 6;
                    a102 = 7;
                    a130 = 3;
                    a242 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a194 == 10) {{
                  if (a249 == 33 && (cf == 1 && a194 == 10 && a20 == 5 && a102 == 9) && input == 1) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 36;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a249 == 32 && cf == 1) {
              if (a128 == 33 && cf == 1) {{
                  if (a128 == 33 && (a249 == 32 && cf == 1 && a102 == 9 && input == 10) && a20 == 5) {
                    cf = 0;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 4 && (a128 == 33 && (a102 == 9 && cf == 1 && a20 == 5) && a249 == 32)) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (input == 8 && (cf == 1 && a128 == 33)) && a20 == 5 && a249 == 32) {
                    cf = 0;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a249 == 34 && cf == 1) {
              if (a77 == 11 && cf == 1) {{
                  if (input == 6 && (a77 == 11 && (a249 == 34 && (cf == 1 && a102 == 9)) && a20 == 5)) {
                    cf = 0;
                    a244 = 33;
                    a40 = 34;
                    a119 = 36;
                    a102 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (input == 2 && (a20 == 5 && (a249 == 34 && cf == 1) && a77 == 11))) {
                    cf = 0;
                    a306 = 11;
                    a20 = 4;
                    a77 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 6) {{
            if (a120 == 33 && cf == 1) {
              if (a127 == 10 && cf == 1) {{
                  if (a127 == 10 && (a102 == 9 && (a120 == 33 && cf == 1 && a20 == 6) && input == 6)) {
                    cf = 0;
                    a130 = 4;
                    a102 = 7;
                    a17 = 7;
                    a151 = 4;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a127 == 11) {{
                  if (a20 == 6 && (a120 == 33 && cf == 1) && a127 == 11 && input == 6 && a102 == 9) {
                    cf = 0;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a127 == 11 && (a20 == 6 && (input == 7 && cf == 1) && a102 == 9 && a120 == 33)) {
                    cf = 0;
                    a183 = 4;
                    a20 = 1;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 1 && (cf == 1 && a120 == 33 && a127 == 11 && a102 == 9) && a20 == 6) {
                    cf = 0;
                    a20 = 1;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a120 == 36) {
              if (cf == 1 && a145 == 35) {{
                  if (input == 8 && (a145 == 35 && cf == 1) && a120 == 36 && a102 == 9 && a20 == 6) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a20 == 7 && cf == 1) {{
            if (cf == 1 && a62 == 3) {
              if (cf == 1 && a171 == 12) {{
                  if (cf == 1 && a171 == 12 && input == 10 && a20 == 7 && a102 == 9 && a62 == 3) {
                    cf = 0;
                    a129 = 32;
                    a78 = 35;
                    a20 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 9 && (input == 5 && (a62 == 3 && cf == 1)) && a171 == 12 && a20 == 7) {
                    cf = 0;
                    a176 = 34;
                    a310 = 16;
                    a102 = 14;
                    a364 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a20 == 7 && (a62 == 3 && (input == 1 && (a171 == 12 && cf == 1 && a102 == 9)))) {
                    cf = 0;
                    a89 = 36;
                    a130 = 10;
                    a102 = 7;
                    a143 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (a171 == 13 && cf == 1) {{
                  if (a171 == 13 && (a20 == 7 && cf == 1 && a102 == 9) && input == 7 && a62 == 3) {
                    cf = 0;
                    a314 = 34;
                    a102 = 11;
                    a106 = 32;
                    a54 = 17;
                    output = 21;
                    fflush(stdout);
                  }
                  if (input == 4 && (a171 == 13 && (cf == 1 && a102 == 9 && a62 == 3)) && a20 == 7) {
                    cf = 0;
                    a232 = 32;
                    a102 = 12;
                    a198 = 34;
                    a116 = 3;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a62 == 4) {
              if (cf == 1 && a289 == 36) {{
                  if (input == 8 && (a102 == 9 && (a20 == 7 && cf == 1 && a62 == 4 && a289 == 36))) {
                    cf = 0;
                    a314 = 32;
                    a139 = 34;
                    a197 = 36;
                    a102 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a62 == 6 && cf == 1) {
              if (a98 == 11 && cf == 1) {{
                  if (a102 == 9 && (a20 == 7 && (input == 1 && cf == 1 && a98 == 11) && a62 == 6)) {
                    cf = 0;
                    a20 = 8;
                    a341 = 33;
                    a271 = 11;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a62 == 9) {
              if (a332 == 34 && cf == 1) {{
                  if (a102 == 9 && (a20 == 7 && (cf == 1 && a62 == 9 && input == 7) && a332 == 34)) {
                    cf = 0;
                    a130 = 10;
                    a102 = 7;
                    a123 = 32;
                    a143 = 6;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a62 == 10 && cf == 1) {
              if (cf == 1 && a67 == 36) {{
                  if (a20 == 7 && (a62 == 10 && cf == 1 && input == 7 && a102 == 9 && a67 == 36)) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a62 == 10 && (input == 3 && (a20 == 7 && cf == 1 && a102 == 9)) && a67 == 36) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a20 == 8) {{
            if (a271 == 7 && cf == 1) {
              if (a126 == 32 && cf == 1) {{
                  if (a20 == 8 && (input == 3 && (a102 == 9 && cf == 1 && a271 == 7) && a126 == 32)) {
                    cf = 0;
                    a146 = 35;
                    a289 = 36;
                    a102 = 8;
                    a17 = 9;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a126 == 32 && (a20 == 8 && (input == 7 && (a271 == 7 && (a102 == 9 && cf == 1))))) {
                    cf = 0;
                    a182 = 13;
                    a102 = 8;
                    a289 = 32;
                    a3 = 13;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 6 && (a102 == 9 && (a126 == 32 && (cf == 1 && a271 == 7 && a20 == 8)))) {
                    cf = 0;
                    a102 = 12;
                    a87 = 12;
                    a116 = 8;
                    a185 = 8;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a271 == 7 && (a20 == 8 && (a126 == 32 && (cf == 1 && input == 8))) && a102 == 9) {
                    cf = 0;
                    a130 = 4;
                    a102 = 7;
                    a115 = 32;
                    a17 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a271 == 11 && cf == 1) {
              if (a341 == 33 && cf == 1) {{
                  if (cf == 1 && a20 == 8 && a102 == 9 && a341 == 33 && a271 == 11 && input == 3) {
                    cf = 0;
                    a20 = 7;
                    a62 = 6;
                    a98 = 11;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a271 == 12) {
              if (a31 == 11 && cf == 1) {{
                  if (a271 == 12 && (a31 == 11 && (a102 == 9 && (cf == 1 && input == 8 && a20 == 8)))) {
                    cf = 0;
                    a102 = 12;
                    a5 = 33;
                    a116 = 5;
                    a28 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a271 == 12 && (a20 == 8 && (input == 6 && (a102 == 9 && cf == 1) && a31 == 11))) {
                    cf = 0;
                    a20 = 6;
                    a120 = 32;
                    a44 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 10 && cf == 1) {
        if (a40 == 33 && cf == 1) {{
            if (a178 == 5 && cf == 1) {
              if (cf == 1 && a170 == 8) {{
                  if (a170 == 8 && (cf == 1 && a178 == 5 && input == 2 && a102 == 10 && a40 == 33)) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a22 = 36;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 4 && (a178 == 5 && (cf == 1 && a170 == 8 && a102 == 10)) && a40 == 33) {
                    cf = 0;
                    a102 = 9;
                    a20 = 3;
                    a129 = 34;
                    a62 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (input == 3 && (a102 == 10 && (a170 == 8 && cf == 1)) && a178 == 5 && a40 == 33) {
                    cf = 0;
                    a249 = 34;
                    a102 = 9;
                    a20 = 5;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 7 && (a170 == 8 && cf == 1 && a178 == 5 && a102 == 10) && a40 == 33) {
                    cf = 0;
                    a20 = 4;
                    a102 = 9;
                    a306 = 11;
                    a77 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a178 == 7 && cf == 1) {
              if (cf == 1 && a271 == 9) {{
                  if (a40 == 33 && (a102 == 10 && (a271 == 9 && (cf == 1 && input == 6 && a178 == 7)))) {
                    cf = 0;
                    a102 = 14;
                    a310 = 11;
                    a0 = 7;
                    a175 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (a40 == 33 && (a271 == 9 && (cf == 1 && input == 8) && a178 == 7))) {
                    cf = 0;
                    a102 = 7;
                    a17 = 9;
                    a130 = 4;
                    a195 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a178 == 7 && (a271 == 9 && (a40 == 33 && cf == 1 && input == 1)) && a102 == 10) {
                    cf = 0;
                    a385 = 36;
                    a40 = 32;
                    a67 = 34;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 7 && (a178 == 7 && (cf == 1 && a40 == 33 && a271 == 9 && a102 == 10))) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 8;
                    a93 = 6;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a178 == 10 && cf == 1) {
              if (a66 == 33 && cf == 1) {{
                  if (input == 6 && (a178 == 10 && (a66 == 33 && cf == 1 && a102 == 10) && a40 == 33)) {
                    cf = 0;
                    a183 = 7;
                    a20 = 1;
                    a102 = 9;
                    a165 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a40 == 32) {{
            if (cf == 1 && a385 == 32) {
              if (cf == 1 && a323 == 5) {{
                  if (a385 == 32 && (a40 == 32 && (input == 4 && cf == 1) && a323 == 5) && a102 == 10) {
                    cf = 0;
                    a102 = 9;
                    a20 = 6;
                    a120 = 33;
                    a127 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a385 == 32 && (a323 == 5 && (a102 == 10 && cf == 1 && input == 3)) && a40 == 32) {
                    cf = 0;
                    a75 = 35;
                    a102 = 8;
                    a289 = 33;
                    a20 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a323 == 5 && (input == 1 && (a385 == 32 && (a40 == 32 && cf == 1 && a102 == 10)))) {
                    cf = 0;
                    a123 = 32;
                    a83 = 35;
                    a314 = 35;
                    a102 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a323 == 5 && (a385 == 32 && (a102 == 10 && cf == 1)) && input == 10 && a40 == 32) {
                    cf = 0;
                    a102 = 9;
                    a20 = 4;
                    a306 = 11;
                    a77 = 7;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a385 == 34) {
              if (cf == 1 && a72 == 33) {{
                  if (a385 == 34 && (cf == 1 && input == 3 && a40 == 32 && a102 == 10) && a72 == 33) {
                    cf = 0;
                    a17 = 9;
                    a130 = 4;
                    a102 = 7;
                    a195 = 12;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a72 == 35) {{
                  if (a102 == 10 && (cf == 1 && a72 == 35 && a385 == 34 && input == 4) && a40 == 32) {
                    cf = 0;
                    a102 = 14;
                    a310 = 15;
                    a166 = 33;
                    a71 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (a72 == 36 && cf == 1) {{
                  if (input == 7 && (a102 == 10 && (cf == 1 && a40 == 32 && a72 == 36) && a385 == 34)) {
                    cf = 0;
                    a130 = 9;
                    a102 = 7;
                    a112 = 34;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a40 == 32 && (input == 8 && (cf == 1 && a102 == 10 && a72 == 36) && a385 == 34)) {
                    cf = 0;
                    a102 = 11;
                    a106 = 35;
                    a314 = 34;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (cf == 1 && a72 == 36 && a40 == 32 && a102 == 10 && a385 == 34 && input == 1) {
                    cf = 0;
                    a270 = 36;
                    a102 = 13;
                    a222 = 34;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a385 == 35) {
              if (cf == 1 && a140 == 9) {{
                  if (a40 == 32 && (a385 == 35 && (a140 == 9 && cf == 1) && a102 == 10 && input == 6)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a144 = 35;
                    a183 = 8;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a385 == 35 && (a40 == 32 && (a102 == 10 && (input == 4 && (a140 == 9 && cf == 1))))) {
                    cf = 0;
                    a9 = 35;
                    a270 = 34;
                    a40 = 33;
                    a102 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a140 == 9 && (a385 == 35 && (input == 8 && (a40 == 32 && cf == 1) && a102 == 10))) {
                    cf = 0;
                    a310 = 9;
                    a190 = 34;
                    a102 = 14;
                    a125 = 14;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a40 == 32 && (a102 == 10 && (a140 == 9 && (a385 == 35 && cf == 1)) && input == 1)) {
                    cf = 0;
                    a20 = 3;
                    a102 = 9;
                    a129 = 35;
                    a23 = 15;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a385 == 36) {
              if (cf == 1 && a67 == 34) {{
                  if (input == 10 && (a67 == 34 && (a102 == 10 && (cf == 1 && a385 == 36)) && a40 == 32)) {
                    cf = 0;
                    a102 = 11;
                    a314 = 34;
                    a106 = 32;
                    a54 = 10;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
              if (a67 == 35 && cf == 1) {{
                  if (a67 == 35 && (a40 == 32 && (cf == 1 && input == 5 && a102 == 10) && a385 == 36)) {
                    cf = 0;
                    a102 = 9;
                    a62 = 6;
                    a20 = 7;
                    a98 = 6;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a385 == 36 && (input == 2 && (a40 == 32 && cf == 1)) && a102 == 10 && a67 == 35) {
                    cf = 0;
                    a197 = 33;
                    a102 = 11;
                    a314 = 32;
                    a86 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (a40 == 32 && (input == 10 && (a385 == 36 && (a67 == 35 && cf == 1))))) {
                    cf = 0;
                    a130 = 8;
                    a102 = 7;
                    a52 = 35;
                    a182 = 14;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a40 == 34 && cf == 1) {{
            if (a244 == 33 && cf == 1) {
              if (a119 == 35 && cf == 1) {{
                  if (a40 == 34 && (a244 == 33 && cf == 1 && input == 6 && a102 == 10 && a119 == 35)) {
                    cf = 0;
                    a310 = 11;
                    a120 = 33;
                    a102 = 14;
                    a0 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a244 == 34) {
              if (34 == 34 && cf == 1) {{
                  if (34 == 34 && (a244 == 34 && cf == 1 && a102 == 10 && input == 5 && a40 == 34)) {
                    cf = 0;
                    a102 = 9;
                    a13 = 35;
                    a20 = 1;
                    a183 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 10 && (a40 == 34 && (34 == 34 && cf == 1 && a244 == 34)) && a102 == 10) {
                    cf = 0;
                    a270 = 32;
                    a289 = 34;
                    a57 = 33;
                    a102 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 8 && (34 == 34 && (a40 == 34 && (a102 == 10 && cf == 1 && a244 == 34)))) {
                    cf = 0;
                    a40 = 33;
                    a178 = 7;
                    a271 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (a40 == 34 && (a244 == 34 && (cf == 1 && 34 == 34 && input == 2)))) {
                    cf = 0;
                    a248 = 9;
                    a102 = 7;
                    a130 = 6;
                    a178 = 5;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (cf == 1 && a40 == 34 && a244 == 34 && 34 == 34 && input == 7)) {
                    cf = 0;
                    a57 = 32;
                    a102 = 8;
                    a289 = 34;
                    a335 = 11;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a244 == 35 && cf == 1) {
              if (cf == 1 && a58 == 34) {{
                  if (input == 4 && (a58 == 34 && (a40 == 34 && cf == 1 && a102 == 10 && a244 == 35))) {
                    cf = 0;
                    a244 = 34;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a40 == 35) {{
            if (a98 == 5 && cf == 1) {
              if (cf == 1 && 36 == 36) {{
                  if (a40 == 35 && (a98 == 5 && (input == 5 && cf == 1)) && a102 == 10 && 36 == 36) {
                    cf = 0;
                    a102 = 14;
                    a310 = 16;
                    a176 = 34;
                    a364 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a98 == 7 && cf == 1) {
              if (a59 == 33 && cf == 1) {{
                  if (a59 == 33 && (a98 == 7 && (cf == 1 && a40 == 35 && input == 1)) && a102 == 10) {
                    cf = 0;
                    a270 = 32;
                    a57 = 33;
                    a289 = 34;
                    a102 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a40 == 35 && (a102 == 10 && (a98 == 7 && (input == 6 && cf == 1)) && a59 == 33)) {
                    cf = 0;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a59 == 33 && (a40 == 35 && cf == 1) && a102 == 10 && input == 4 && a98 == 7) {
                    cf = 0;
                    a115 = 32;
                    a102 = 7;
                    a130 = 4;
                    a17 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a98 == 8) {
              if (cf == 1 && a9 == 35) {{
                  if (a40 == 35 && (cf == 1 && input == 7 && a9 == 35 && a102 == 10) && a98 == 8) {
                    cf = 0;
                    a146 = 33;
                    a102 = 8;
                    a289 = 36;
                    a195 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 2 && (a40 == 35 && (cf == 1 && a98 == 8 && a102 == 10) && a9 == 35)) {
                    cf = 0;
                    a102 = 7;
                    a52 = 32;
                    a130 = 8;
                    a182 = 14;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a98 == 9) {
              if (a51 == 5 && cf == 1) {{
                  if (a98 == 9 && (a102 == 10 && (input == 1 && cf == 1) && a40 == 35) && a51 == 5) {
                    cf = 0;
                    a249 = 34;
                    a20 = 5;
                    a102 = 9;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a40 == 36) {{
            if (cf == 1 && a24 == 33) {
              if (cf == 1 && a135 == 34) {{
                  if (a24 == 33 && (input == 3 && (a40 == 36 && (a135 == 34 && (cf == 1 && a102 == 10))))) {
                    cf = 0;
                    a143 = 8;
                    a102 = 7;
                    a130 = 10;
                    a99 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a24 == 32 && cf == 1) {
              if (cf == 1 && 32 == 32) {{
                  if (a24 == 32 && (cf == 1 && input == 10) && 32 == 32 && a102 == 10 && a40 == 36) {
                    cf = 0;
                    a385 = 33;
                    a40 = 32;
                    a182 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                  if (32 == 32 && (input == 8 && cf == 1 && a40 == 36 && a102 == 10) && a24 == 32) {
                    cf = 0;
                    a5 = 33;
                    a116 = 5;
                    a102 = 12;
                    a28 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a24 == 34) {
              if (a8 == 35 && cf == 1) {{
                  if (a8 == 35 && (cf == 1 && a24 == 34 && a40 == 36 && a102 == 10) && input == 3) {
                    cf = 0;
                    a102 = 11;
                    a78 = 36;
                    a314 = 36;
                    a265 = 6;
                    output = 22;
                    fflush(stdout);
                  }
                  if (input == 1 && (a40 == 36 && (a8 == 35 && (a24 == 34 && cf == 1)) && a102 == 10)) {
                    cf = 0;
                    a130 = 10;
                    a143 = 10;
                    a102 = 7;
                    a49 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a24 == 35) {
              if (a156 == 32 && cf == 1) {{
                  if (a102 == 10 && (a156 == 32 && (a24 == 35 && (cf == 1 && input == 8))) && a40 == 36) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a102 = 7;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a24 == 36) {
              if (cf == 1 && a184 == 9) {{
                  if (a40 == 36 && (a102 == 10 && (input == 1 && (cf == 1 && a184 == 9 && a24 == 36)))) {
                    cf = 0;
                    a102 = 7;
                    a121 = 34;
                    a112 = 36;
                    a130 = 9;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a184 == 9 && (a102 == 10 && (input == 3 && (a24 == 36 && cf == 1))) && a40 == 36) {
                    cf = 0;
                    a40 = 35;
                    a98 = 9;
                    a51 = 5;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a102 == 10 && (cf == 1 && input == 4 && a40 == 36) && a184 == 9 && a24 == 36) {
                    cf = 0;
                    a314 = 34;
                    a106 = 35;
                    a102 = 11;
                    a4 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 11 && cf == 1) {
        if (cf == 1 && a314 == 33) {{
            if (cf == 1 && a16 == 33) {
              if (a113 == 33 && cf == 1) {{
                  if (a16 == 33 && (a102 == 11 && cf == 1 && a314 == 33 && a113 == 33 && input == 6)) {
                    cf = 0;
                    a102 = 12;
                    a116 = 8;
                    a87 = 14;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a113 == 33 && (a314 == 33 && (cf == 1 && a16 == 33 && input == 4 && a102 == 11))) {
                    cf = 0;
                    a57 = 33;
                    a83 = 33;
                    a102 = 12;
                    a116 = 2;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a16 == 32) {
              if (cf == 1 && a192 == 6) {{
                  if (a192 == 6 && (a16 == 32 && (a314 == 33 && cf == 1 && a102 == 11 && input == 3))) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 15;
                    a182 = 14;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a192 == 7) {{
                  if (a314 == 33 && (a102 == 11 && (a192 == 7 && cf == 1 && a16 == 32 && input == 10))) {
                    cf = 0;
                    a129 = 32;
                    a102 = 9;
                    a78 = 35;
                    a20 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                  if (input == 5 && (a192 == 7 && (a16 == 32 && (a102 == 11 && cf == 1 && a314 == 33)))) {
                    cf = 0;
                    a102 = 14;
                    a176 = 34;
                    a310 = 16;
                    a364 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a16 == 32 && (a102 == 11 && (a314 == 33 && cf == 1 && input == 1 && a192 == 7))) {
                    cf = 0;
                    a20 = 3;
                    a102 = 9;
                    a129 = 33;
                    a344 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a16 == 34) {
              if (a196 == 32 && cf == 1) {{
                  if (a196 == 32 && (a314 == 33 && (a16 == 34 && (cf == 1 && input == 4 && a102 == 11)))) {
                    cf = 0;
                    a126 = 34;
                    a190 = 34;
                    a102 = 14;
                    a310 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a16 == 34 && (a196 == 32 && (a102 == 11 && (cf == 1 && input == 6) && a314 == 33))) {
                    cf = 0;
                    a168 = 3;
                    a102 = 7;
                    a130 = 3;
                    a15 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a16 == 36 && cf == 1) {
              if (cf == 1 && a8 == 35) {{
                  if (input == 1 && (a102 == 11 && cf == 1 && a8 == 35 && a314 == 33 && a16 == 36)) {
                    cf = 0;
                    a12 = 33;
                    a20 = 2;
                    a102 = 9;
                    a36 = 13;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a8 == 36) {{
                  if (input == 4 && (a8 == 36 && (cf == 1 && a314 == 33)) && a16 == 36 && a102 == 11) {
                    cf = 0;
                    a102 = 14;
                    a310 = 10;
                    a190 = 33;
                    a71 = 11;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a16 == 36 && (input == 3 && (a8 == 36 && (a102 == 11 && cf == 1)) && a314 == 33)) {
                    cf = 0;
                    a40 = 32;
                    a102 = 10;
                    a385 = 35;
                    a140 = 9;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a314 == 32 && cf == 1) {{
            if (a197 == 33 && cf == 1) {
              if (a86 == 6 && cf == 1) {{
                  if (a102 == 11 && (a197 == 33 && (cf == 1 && a86 == 6 && a314 == 32 && input == 4))) {
                    cf = 0;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (a86 == 6 && (input == 1 && (a197 == 33 && cf == 1))) && a314 == 32) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a86 == 6 && (a314 == 32 && (cf == 1 && input == 2 && a102 == 11 && a197 == 33))) {
                    cf = 0;
                    a310 = 11;
                    a102 = 14;
                    a176 = 34;
                    a0 = 4;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a314 == 32 && (a102 == 11 && (a86 == 6 && (cf == 1 && a197 == 33)) && input == 8)) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a86 == 10) {{
                  if (a197 == 33 && (a86 == 10 && (cf == 1 && input == 3) && a102 == 11 && a314 == 32)) {
                    cf = 0;
                    a40 = 32;
                    a102 = 10;
                    a385 = 35;
                    a140 = 9;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 4 && (cf == 1 && a102 == 11 && a314 == 32) && a197 == 33 && a86 == 10) {
                    cf = 0;
                    a249 = 36;
                    a289 = 35;
                    a19 = 32;
                    a102 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a86 == 10 && (input == 5 && (a197 == 33 && cf == 1)) && a102 == 11 && a314 == 32) {
                    cf = 0;
                    a102 = 13;
                    a70 = 36;
                    a270 = 35;
                    a170 = 9;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a197 == 32) {
              if (cf == 1 && a170 == 12) {{
                  if (input == 3 && (a314 == 32 && (a197 == 32 && cf == 1 && a170 == 12)) && a102 == 11) {
                    cf = 0;
                    a102 = 7;
                    a130 = 6;
                    a248 = 8;
                    a335 = 8;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a314 == 32 && (a102 == 11 && (cf == 1 && a170 == 12) && input == 7 && a197 == 32)) {
                    cf = 0;
                    a190 = 35;
                    a270 = 34;
                    a9 = 36;
                    a102 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a170 == 12 && (a102 == 11 && (a197 == 32 && cf == 1) && a314 == 32) && input == 10) {
                    cf = 0;
                    a57 = 35;
                    a102 = 12;
                    a78 = 35;
                    a116 = 2;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 4 && (a314 == 32 && (cf == 1 && a102 == 11 && a170 == 12 && a197 == 32))) {
                    cf = 0;
                    a249 = 32;
                    a128 = 33;
                    a102 = 9;
                    a20 = 5;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a197 == 35 && cf == 1) {
              if (cf == 1 && a140 == 13) {{
                  if (a314 == 32 && (a197 == 35 && (a140 == 13 && (cf == 1 && a102 == 11) && input == 10))) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a35 = 34;
                    a69 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a314 == 32 && (a102 == 11 && (a140 == 13 && cf == 1 && a197 == 35 && input == 3))) {
                    cf = 0;
                    a197 = 33;
                    a86 = 6;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a197 == 36 && cf == 1) {
              if (a139 == 34 && cf == 1) {{
                  if (a139 == 34 && (a197 == 36 && (a314 == 32 && (cf == 1 && a102 == 11 && input == 1)))) {
                    cf = 0;
                    a78 = 35;
                    a314 = 36;
                    a126 = 35;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a197 == 36 && (input == 8 && cf == 1) && a102 == 11 && a314 == 32 && a139 == 34) {
                    cf = 0;
                    a310 = 15;
                    a197 = 35;
                    a102 = 14;
                    a71 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a314 == 32 && (a102 == 11 && (input == 10 && (cf == 1 && a197 == 36))) && a139 == 34) {
                    cf = 0;
                    a102 = 8;
                    a289 = 34;
                    a57 = 36;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (cf == 1 && a139 == 34 && a314 == 32 && input == 6) && a197 == 36) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a89 = 32;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a314 == 34 && cf == 1) {{
            if (cf == 1 && a106 == 33) {
              if (cf == 1 && a78 == 36) {{
                  if (a314 == 34 && (a78 == 36 && (cf == 1 && a106 == 33) && a102 == 11 && input == 4)) {
                    cf = 0;
                    a232 = 34;
                    a102 = 12;
                    a116 = 3;
                    a344 = 14;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (a106 == 33 && (input == 1 && (a314 == 34 && cf == 1)) && a78 == 36)) {
                    cf = 0;
                    a83 = 35;
                    a314 = 35;
                    a123 = 32;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a106 == 32 && cf == 1) {
              if (a54 == 10 && cf == 1) {{
                  if (a314 == 34 && (input == 8 && cf == 1) && a54 == 10 && a102 == 11 && a106 == 32) {
                    cf = 0;
                    a244 = 34;
                    a40 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a314 == 34 && (a102 == 11 && (cf == 1 && a106 == 32) && a54 == 10) && input == 3) {
                    cf = 0;
                    a67 = 34;
                    a385 = 36;
                    a40 = 32;
                    a102 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a54 == 16) {{
                  if (a106 == 32 && cf == 1 && a102 == 11 && input == 5 && a314 == 34 && a54 == 16) {
                    cf = 0;
                    a20 = 5;
                    a249 = 34;
                    a102 = 9;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a314 == 34 && (a102 == 11 && (a106 == 32 && (cf == 1 && input == 2 && a54 == 16)))) {
                    cf = 0;
                    a20 = 5;
                    a249 = 34;
                    a102 = 9;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a54 == 17) {{
                  if (a102 == 11 && (a54 == 17 && (input == 10 && (a106 == 32 && (a314 == 34 && cf == 1))))) {
                    cf = 0;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a54 == 17 && (a102 == 11 && (a106 == 32 && (a314 == 34 && cf == 1)) && input == 8)) {
                    cf = 0;
                    a102 = 14;
                    a91 = 36;
                    a296 = 35;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a54 == 17 && (input == 1 && (a314 == 34 && cf == 1 && a102 == 11) && a106 == 32)) {
                    cf = 0;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a54 == 17 && (cf == 1 && a314 == 34) && a106 == 32 && input == 2 && a102 == 11) {
                    cf = 0;
                    a102 = 9;
                    a20 = 7;
                    a62 = 3;
                    a171 = 13;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a106 == 35 && cf == 1) {
              if (a4 == 7 && cf == 1) {{
                  if (a314 == 34 && (cf == 1 && a102 == 11 && a4 == 7 && a106 == 35) && input == 8) {
                    cf = 0;
                    a385 = 34;
                    a72 = 36;
                    a40 = 32;
                    a102 = 10;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a4 == 10) {{
                  if (a106 == 35 && (a102 == 11 && cf == 1 && input == 1) && a314 == 34 && a4 == 10) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (a314 == 34 && (cf == 1 && input == 6) && a106 == 35 && a4 == 10)) {
                    cf = 0;
                    a24 = 36;
                    a102 = 10;
                    a40 = 36;
                    a184 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a4 == 10 && (a314 == 34 && cf == 1 && input == 8 && a106 == 35) && a102 == 11) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a106 == 36) {
              if (a60 == 12 && cf == 1) {{
                  if (a102 == 11 && (input == 5 && cf == 1 && a60 == 12 && a106 == 36) && a314 == 34) {
                    cf = 0;
                    a102 = 13;
                    a270 = 32;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 3 && (cf == 1 && a314 == 34 && a102 == 11) && a106 == 36 && a60 == 12) {
                    cf = 0;
                    a102 = 12;
                    a87 = 14;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a106 == 36 && (a314 == 34 && (a60 == 12 && (input == 6 && (cf == 1 && a102 == 11))))) {
                    cf = 0;
                    a289 = 32;
                    a102 = 8;
                    a182 = 14;
                    a184 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a106 == 36 && (a102 == 11 && (cf == 1 && input == 2) && a314 == 34) && a60 == 12) {
                    cf = 0;
                    a102 = 8;
                    a182 = 14;
                    a289 = 32;
                    a184 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a314 == 35) {{
            if (a83 == 33 && cf == 1) {
              if (cf == 1 && a125 == 10) {{
                  if (a102 == 11 && (input == 4 && (cf == 1 && a83 == 33 && a314 == 35 && a125 == 10))) {
                    cf = 0;
                    a40 = 33;
                    a102 = 10;
                    a66 = 33;
                    a178 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a83 == 34) {
              if (a117 == 10 && cf == 1) {{
                  if (a83 == 34 && (a117 == 10 && (a102 == 11 && (cf == 1 && a314 == 35)) && input == 2)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 5;
                    a49 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 1 && (a314 == 35 && (a102 == 11 && (a117 == 10 && cf == 1)) && a83 == 34)) {
                    cf = 0;
                    a271 = 12;
                    a102 = 9;
                    a20 = 8;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 4 && (a83 == 34 && (cf == 1 && a102 == 11)) && a117 == 10 && a314 == 35) {
                    cf = 0;
                    a102 = 9;
                    a20 = 8;
                    a271 = 12;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a117 == 11) {{
                  if (a83 == 34 && (a117 == 11 && (a102 == 11 && cf == 1 && input == 7) && a314 == 35)) {
                    cf = 0;
                    a116 = 8;
                    a102 = 12;
                    a87 = 13;
                    a159 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 4 && (a83 == 34 && cf == 1 && a117 == 11 && a314 == 35) && a102 == 11) {
                    cf = 0;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a314 == 35 && (a117 == 11 && (cf == 1 && a102 == 11) && input == 2) && a83 == 34) {
                    cf = 0;
                    a289 = 32;
                    a144 = 35;
                    a102 = 8;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 3 && (a314 == 35 && (a117 == 11 && cf == 1 && a83 == 34)) && a102 == 11) {
                    cf = 0;
                    a87 = 13;
                    a102 = 12;
                    a116 = 8;
                    a159 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a117 == 13 && cf == 1) {{
                  if (a83 == 34 && (a117 == 13 && (cf == 1 && a102 == 11)) && input == 7 && a314 == 35) {
                    cf = 0;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a117 == 14 && cf == 1) {{
                  if (a83 == 34 && (a102 == 11 && cf == 1) && a117 == 14 && input == 8 && a314 == 35) {
                    cf = 0;
                    a102 = 13;
                    a344 = 8;
                    a270 = 36;
                    a153 = 8;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 10 && (a83 == 34 && cf == 1 && a102 == 11 && a314 == 35) && a117 == 14) {
                    cf = 0;
                    a24 = 35;
                    a40 = 36;
                    a156 = 32;
                    a102 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a314 == 35 && (input == 3 && (a102 == 11 && cf == 1 && a83 == 34) && a117 == 14)) {
                    cf = 0;
                    a310 = 15;
                    a102 = 14;
                    a166 = 33;
                    a71 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 1 && (a117 == 14 && (a83 == 34 && (a314 == 35 && cf == 1 && a102 == 11)))) {
                    cf = 0;
                    a40 = 36;
                    a135 = 34;
                    a24 = 33;
                    a102 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a83 == 35 && cf == 1) {
              if (a123 == 32 && cf == 1) {{
                  if (a123 == 32 && (input == 3 && (a314 == 35 && (a102 == 11 && (a83 == 35 && cf == 1))))) {
                    cf = 0;
                    a102 = 10;
                    a385 = 32;
                    a40 = 32;
                    a323 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (a123 == 32 && (input == 1 && (a83 == 35 && cf == 1) && a314 == 35))) {
                    cf = 0;
                    a102 = 14;
                    a91 = 36;
                    a296 = 35;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a83 == 35 && (a314 == 35 && (cf == 1 && a102 == 11) && input == 2 && a123 == 32)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a123 == 32 && (a102 == 11 && (a83 == 35 && (cf == 1 && a314 == 35))) && input == 4) {
                    cf = 0;
                    a40 = 32;
                    a385 = 32;
                    a102 = 10;
                    a323 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a314 == 36 && cf == 1) {{
            if (a78 == 32 && cf == 1) {
              if (32 == 32 && cf == 1) {{
                  if (input == 4 && (a78 == 32 && (a102 == 11 && (cf == 1 && a314 == 36))) && 32 == 32) {
                    cf = 0;
                    a102 = 8;
                    a289 = 32;
                    a70 = 32;
                    a182 = 16;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a78 == 34 && cf == 1) {
              if (a104 == 33 && cf == 1) {{
                  if (a104 == 33 && (cf == 1 && a102 == 11) && a78 == 34 && a314 == 36 && input == 8) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 11 && (cf == 1 && input == 6 && a104 == 33) && a314 == 36 && a78 == 34) {
                    cf = 0;
                    a130 = 7;
                    a162 = 5;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a78 == 35 && cf == 1) {
              if (a126 == 35 && cf == 1) {{
                  if (a102 == 11 && (a314 == 36 && (input == 8 && (a78 == 35 && cf == 1)) && a126 == 35)) {
                    cf = 0;
                    a102 = 13;
                    a344 = 14;
                    a270 = 36;
                    a194 = 9;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a78 == 36) {
              if (cf == 1 && a265 == 3) {{
                  if (a314 == 36 && (a102 == 11 && (a78 == 36 && (a265 == 3 && cf == 1))) && input == 10) {
                    cf = 0;
                    a5 = 33;
                    a116 = 5;
                    a102 = 12;
                    a28 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a265 == 3 && (cf == 1 && a78 == 36 && a102 == 11) && a314 == 36 && input == 1) {
                    cf = 0;
                    a116 = 5;
                    a102 = 12;
                    a5 = 33;
                    a28 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a314 == 36 && (a265 == 3 && (a102 == 11 && (input == 4 && cf == 1 && a78 == 36)))) {
                    cf = 0;
                    a102 = 12;
                    a270 = 33;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a265 == 6) {{
                  if (input == 7 && cf == 1 && a314 == 36 && a265 == 6 && a78 == 36 && a102 == 11) {
                    cf = 0;
                    a102 = 14;
                    a296 = 35;
                    a91 = 32;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a265 == 6 && (a314 == 36 && (cf == 1 && input == 6 && a78 == 36) && a102 == 11)) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a265 == 6 && (a102 == 11 && cf == 1) && a78 == 36 && input == 3 && a314 == 36) {
                    cf = 0;
                    a102 = 14;
                    a91 = 32;
                    a296 = 35;
                    a310 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a265 == 6 && (a102 == 11 && cf == 1 && a78 == 36) && input == 10 && a314 == 36) {
                    cf = 0;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (cf == 1 && a102 == 12) {
        if (cf == 1 && a116 == 2) {{
            if (cf == 1 && a57 == 33) {
              if (a83 == 33 && cf == 1) {{
                  if (cf == 1 && a116 == 2 && a83 == 33 && a57 == 33 && input == 2 && a102 == 12) {
                    cf = 0;
                    a130 = 4;
                    a17 = 4;
                    a102 = 7;
                    a21 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a83 == 33 && (a102 == 12 && (a116 == 2 && cf == 1 && a57 == 33) && input == 8)) {
                    cf = 0;
                    a198 = 34;
                    a232 = 33;
                    a116 = 3;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a83 == 33 && (a116 == 2 && (a102 == 12 && cf == 1 && input == 4 && a57 == 33))) {
                    cf = 0;
                    a16 = 33;
                    a314 = 33;
                    a113 = 33;
                    a102 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 10 && (a102 == 12 && (a57 == 33 && cf == 1 && a83 == 33) && a116 == 2)) {
                    cf = 0;
                    a232 = 33;
                    a198 = 36;
                    a116 = 3;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a83 == 34) {{
                  if (a102 == 12 && (a57 == 33 && cf == 1 && input == 6 && a83 == 34) && a116 == 2) {
                    cf = 0;
                    a125 = 11;
                    a102 = 14;
                    a310 = 9;
                    a69 = 13;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a83 == 34 && (a57 == 33 && (input == 7 && (cf == 1 && a102 == 12)) && a116 == 2)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a83 == 34 && (input == 4 && (cf == 1 && a102 == 12 && a57 == 33)) && a116 == 2) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 11;
                    a69 = 13;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a57 == 35 && cf == 1) {
              if (cf == 1 && a78 == 35) {{
                  if (a78 == 35 && (a102 == 12 && (cf == 1 && a116 == 2 && a57 == 35 && input == 6))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (cf == 1 && a78 == 35 && a102 == 12 && input == 7 && a57 == 35 && a116 == 2) {
                    cf = 0;
                    a102 = 8;
                    a57 = 32;
                    a289 = 34;
                    a335 = 11;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a116 == 2 && (cf == 1 && a102 == 12) && input == 4 && a78 == 35 && a57 == 35) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 1 && (a57 == 35 && cf == 1 && a102 == 12 && a116 == 2 && a78 == 35)) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a57 == 36) {
              if (a7 == 4 && cf == 1) {{
                  if (a102 == 12 && cf == 1 && a7 == 4 && input == 7 && a57 == 36 && a116 == 2) {
                    cf = 0;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a7 == 5 && cf == 1) {{
                  if (a7 == 5 && (a57 == 36 && (cf == 1 && input == 4 && a102 == 12) && a116 == 2)) {
                    cf = 0;
                    a130 = 9;
                    a112 = 32;
                    a102 = 7;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a116 == 2 && (a57 == 36 && (cf == 1 && a102 == 12 && a7 == 5) && input == 3)) {
                    cf = 0;
                    a102 = 7;
                    a112 = 34;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a7 == 7 && cf == 1) {{
                  if (a57 == 36 && (a7 == 7 && (a116 == 2 && (input == 1 && cf == 1)) && a102 == 12)) {
                    cf = 0;
                    a102 = 13;
                    a270 = 32;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a116 == 2 && (a57 == 36 && (a102 == 12 && cf == 1)) && input == 3 && a7 == 7) {
                    cf = 0;
                    a270 = 32;
                    a102 = 13;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a116 == 3) {{
            if (cf == 1 && a232 == 33) {
              if (a198 == 34 && cf == 1) {{
                  if (a116 == 3 && (a232 == 33 && (a198 == 34 && cf == 1 && input == 5 && a102 == 12))) {
                    cf = 0;
                    a83 = 33;
                    a57 = 33;
                    a116 = 2;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a232 == 33 && (a198 == 34 && (a102 == 12 && cf == 1) && a116 == 3) && input == 10) {
                    cf = 0;
                    a310 = 9;
                    a125 = 15;
                    a102 = 14;
                    a182 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a198 == 34 && (cf == 1 && a232 == 33 && input == 8 && a102 == 12) && a116 == 3) {
                    cf = 0;
                    a87 = 14;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 7 && (a198 == 34 && (a116 == 3 && (cf == 1 && a232 == 33 && a102 == 12)))) {
                    cf = 0;
                    a116 = 8;
                    a87 = 14;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
              if (a198 == 36 && cf == 1) {{
                  if (a116 == 3 && (a102 == 12 && (cf == 1 && input == 8 && a198 == 36 && a232 == 33))) {
                    cf = 0;
                    a57 = 33;
                    a83 = 33;
                    a116 = 2;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a232 == 32) {
              if (a198 == 34 && cf == 1) {{
                  if (input == 3 && (cf == 1 && a116 == 3 && a198 == 34) && a102 == 12 && a232 == 32) {
                    cf = 0;
                    a20 = 1;
                    a102 = 9;
                    a183 = 4;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a232 == 32 && (a116 == 3 && (input == 6 && (a198 == 34 && (cf == 1 && a102 == 12))))) {
                    cf = 0;
                    a102 = 9;
                    a62 = 3;
                    a20 = 7;
                    a171 = 13;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a232 == 34) {
              if (a344 == 14 && cf == 1) {{
                  if (a116 == 3 && (input == 6 && cf == 1) && a344 == 14 && a102 == 12 && a232 == 34) {
                    cf = 0;
                    a40 = 34;
                    a244 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (input == 3 && (a116 == 3 && cf == 1 && a344 == 14)) && a232 == 34) {
                    cf = 0;
                    a102 = 14;
                    a310 = 11;
                    a0 = 7;
                    a175 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (input == 1 && (a116 == 3 && cf == 1) && a232 == 34 && a344 == 14)) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (input == 4 && (a232 == 34 && (cf == 1 && a102 == 12)) && a344 == 14 && a116 == 3) {
                    cf = 0;
                    a102 = 7;
                    a162 = 5;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a232 == 36) {
              if (a270 == 33 && cf == 1) {{
                  if (a116 == 3 && (a270 == 33 && cf == 1 && input == 3) && a102 == 12 && a232 == 36) {
                    cf = 0;
                    a20 = 8;
                    a102 = 9;
                    a271 = 12;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a232 == 36 && (a116 == 3 && (a270 == 33 && (a102 == 12 && cf == 1)) && input == 5)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 3;
                    a186 = 33;
                    a168 = 5;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a116 == 3 && (input == 7 && (a270 == 33 && cf == 1 && a232 == 36)))) {
                    cf = 0;
                    a102 = 13;
                    a270 = 36;
                    a222 = 34;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a116 == 4) {{
            if (cf == 1 && a67 == 32) {
              if (cf == 1 && a98 == 5) {{
                  if (a116 == 4 && (a67 == 32 && (cf == 1 && a102 == 12) && input == 2) && a98 == 5) {
                    cf = 0;
                    a78 = 34;
                    a314 = 36;
                    a104 = 33;
                    a102 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a98 == 5 && (cf == 1 && a67 == 32)) && a116 == 4 && input == 4) {
                    cf = 0;
                    a102 = 9;
                    a249 = 33;
                    a20 = 5;
                    a194 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (cf == 1 && a98 == 5 && a67 == 32 && input == 8) && a116 == 4) {
                    cf = 0;
                    a289 = 36;
                    a146 = 32;
                    a102 = 8;
                    a93 = 7;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a67 == 34 && cf == 1) {
              if (a142 == 8 && cf == 1) {{
                  if (a116 == 4 && (a67 == 34 && (cf == 1 && a102 == 12 && a142 == 8) && input == 3)) {
                    cf = 0;
                    a17 = 10;
                    a102 = 7;
                    a130 = 4;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a116 == 4 && (input == 7 && (a102 == 12 && (a142 == 8 && cf == 1) && a67 == 34))) {
                    cf = 0;
                    a9 = 35;
                    a102 = 10;
                    a40 = 35;
                    a98 = 8;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a67 == 34 && (input == 10 && (a102 == 12 && cf == 1 && a116 == 4 && a142 == 8))) {
                    cf = 0;
                    a8 = 35;
                    a40 = 36;
                    a24 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a67 == 35) {
              if (cf == 1 && a62 == 8) {{
                  if (a62 == 8 && (a67 == 35 && (a102 == 12 && (a116 == 4 && cf == 1)) && input == 1)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 1;
                    a183 = 4;
                    a17 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a62 == 8 && (a67 == 35 && cf == 1 && input == 10)) && a116 == 4) {
                    cf = 0;
                    a102 = 14;
                    a190 = 36;
                    a310 = 10;
                    a28 = 8;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a116 == 4 && (a102 == 12 && cf == 1 && input == 8 && a62 == 8 && a67 == 35)) {
                    cf = 0;
                    a9 = 32;
                    a270 = 34;
                    a102 = 13;
                    a183 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a67 == 36 && cf == 1) {
              if (a310 == 12 && cf == 1) {{
                  if (cf == 1 && a67 == 36 && input == 3 && a116 == 4 && a102 == 12 && a310 == 12) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 15;
                    a182 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a67 == 36 && (cf == 1 && input == 1) && a310 == 12 && a116 == 4)) {
                    cf = 0;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a310 == 12 && (a116 == 4 && (a102 == 12 && cf == 1)) && a67 == 36)) {
                    cf = 0;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a310 == 12 && (a102 == 12 && (input == 7 && (cf == 1 && a67 == 36 && a116 == 4)))) {
                    cf = 0;
                    a130 = 10;
                    a123 = 32;
                    a102 = 7;
                    a143 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a310 == 15) {{
                  if (a67 == 36 && (a116 == 4 && (cf == 1 && a102 == 12)) && input == 1 && a310 == 15) {
                    cf = 0;
                    a87 = 14;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a116 == 5 && cf == 1) {{
            if (cf == 1 && a5 == 33) {
              if (a28 == 8 && cf == 1) {{
                  if (a116 == 5 && (input == 5 && (cf == 1 && a102 == 12 && a28 == 8)) && a5 == 33) {
                    cf = 0;
                    a40 = 32;
                    a102 = 10;
                    a385 = 32;
                    a323 = 1;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 10 && (a116 == 5 && (a102 == 12 && (cf == 1 && a28 == 8 && a5 == 33)))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 5;
                    a49 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a5 == 32) {
              if (a117 == 12 && cf == 1) {{
                  if (a5 == 32 && (input == 3 && (a117 == 12 && cf == 1) && a102 == 12 && a116 == 5)) {
                    cf = 0;
                    a102 = 13;
                    a270 = 32;
                    a69 = 14;
                    a390 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a116 == 5 && (cf == 1 && a5 == 32 && a102 == 12 && a117 == 12))) {
                    cf = 0;
                    a289 = 32;
                    a144 = 36;
                    a102 = 8;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a5 == 34) {
              if (a19 == 35 && cf == 1) {{
                  if (a102 == 12 && (cf == 1 && a5 == 34 && input == 2) && a116 == 5 && a19 == 35) {
                    cf = 0;
                    a244 = 34;
                    a40 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a5 == 34 && (a102 == 12 && cf == 1 && a116 == 5 && a19 == 35) && input == 7) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a5 == 35) {
              if (cf == 1 && a173 == 7) {{
                  if (a102 == 12 && (input == 4 && cf == 1 && a5 == 35 && a116 == 5) && a173 == 7) {
                    cf = 0;
                    a40 = 32;
                    a72 = 35;
                    a385 = 34;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a116 == 5 && (a102 == 12 && (a173 == 7 && (input == 8 && (cf == 1 && a5 == 35))))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 6;
                    a37 = 34;
                    a248 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a116 == 6) {{
            if (a56 == 10 && cf == 1) {
              if (a377 == 8 && cf == 1) {{
                  if (input == 6 && (a102 == 12 && cf == 1 && a56 == 10 && a377 == 8) && a116 == 6) {
                    cf = 0;
                    a130 = 10;
                    a102 = 7;
                    a143 = 10;
                    a49 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a377 == 8 && (a102 == 12 && (cf == 1 && a56 == 10 && input == 2 && a116 == 6))) {
                    cf = 0;
                    a130 = 10;
                    a143 = 8;
                    a102 = 7;
                    a99 = 6;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a56 == 11 && cf == 1) {
              if (cf == 1 && a106 == 33) {{
                  if (a56 == 11 && (a102 == 12 && cf == 1) && input == 5 && a116 == 6 && a106 == 33) {
                    cf = 0;
                    a163 = 34;
                    a310 = 14;
                    a102 = 14;
                    a15 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a116 == 6 && (a106 == 33 && (a102 == 12 && (cf == 1 && input == 3)) && a56 == 11)) {
                    cf = 0;
                    a310 = 13;
                    a102 = 14;
                    a122 = 34;
                    a242 = 6;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a56 == 13 && cf == 1) {
              if (cf == 1 && a22 == 36) {{
                  if (input == 1 && (a56 == 13 && cf == 1) && a102 == 12 && a22 == 36 && a116 == 6) {
                    cf = 0;
                    a168 = 8;
                    a102 = 7;
                    a130 = 3;
                    a30 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a22 == 36 && (input == 9 && cf == 1 && a56 == 13) && a102 == 12 && a116 == 6) {
                    cf = 0;
                    a74 = 36;
                    a164 = 35;
                    a116 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a116 == 6 && (a56 == 13 && (a102 == 12 && (input == 3 && cf == 1))) && a22 == 36) {
                    cf = 0;
                    a249 = 34;
                    a102 = 9;
                    a20 = 5;
                    a77 = 11;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a56 == 14) {
              if (cf == 1 && a49 == 3) {{
                  if (a56 == 14 && (a102 == 12 && (a49 == 3 && (input == 8 && cf == 1))) && a116 == 6) {
                    cf = 0;
                    a20 = 2;
                    a102 = 9;
                    a12 = 32;
                    a87 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a116 == 6 && (input == 3 && (a56 == 14 && cf == 1 && a49 == 3) && a102 == 12)) {
                    cf = 0;
                    a130 = 6;
                    a37 = 32;
                    a102 = 7;
                    a248 = 12;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 10 && (a56 == 14 && cf == 1 && a49 == 3 && a116 == 6 && a102 == 12)) {
                    cf = 0;
                    a128 = 33;
                    a130 = 7;
                    a102 = 7;
                    a162 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a56 == 15) {
              if (cf == 1 && a222 == 34) {{
                  if (a102 == 12 && (a116 == 6 && (a222 == 34 && (cf == 1 && a56 == 15 && input == 8)))) {
                    cf = 0;
                    a172 = 32;
                    a70 = 33;
                    a270 = 33;
                    a102 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a56 == 16) {
              if (a159 == 10 && cf == 1) {{
                  if (a159 == 10 && (a56 == 16 && (a102 == 12 && (a116 == 6 && (cf == 1 && input == 1))))) {
                    cf = 0;
                    a87 = 16;
                    a116 = 8;
                    a44 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a116 == 7 && cf == 1) {{
            if (cf == 1 && a74 == 32) {
              if (cf == 1 && a28 == 3) {{
                  if (a28 == 3 && (input == 6 && (a74 == 32 && (a102 == 12 && cf == 1)) && a116 == 7)) {
                    cf = 0;
                    a67 = 35;
                    a289 = 33;
                    a75 = 33;
                    a102 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 7 && (a74 == 32 && (a102 == 12 && cf == 1) && a28 == 3) && a116 == 7) {
                    cf = 0;
                    a310 = 9;
                    a125 = 11;
                    a102 = 14;
                    a69 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a28 == 6) {{
                  if (a74 == 32 && (a116 == 7 && (cf == 1 && a28 == 6) && input == 3) && a102 == 12) {
                    cf = 0;
                    a81 = 35;
                    a102 = 13;
                    a270 = 32;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a116 == 7 && (input == 7 && (a74 == 32 && (a28 == 6 && cf == 1)) && a102 == 12)) {
                    cf = 0;
                    a22 = 36;
                    a116 = 6;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a74 == 36 && cf == 1) {
              if (a164 == 33 && cf == 1) {{
                  if (a74 == 36 && (input == 1 && (a102 == 12 && cf == 1 && a116 == 7 && a164 == 33))) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a164 == 33 && (cf == 1 && a102 == 12) && input == 6 && a116 == 7 && a74 == 36) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a116 == 7 && (a164 == 33 && (a74 == 36 && cf == 1))) && input == 7) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 9;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a164 == 33 && (input == 3 && cf == 1 && a116 == 7 && a102 == 12) && a74 == 36) {
                    cf = 0;
                    a182 = 14;
                    a289 = 32;
                    a102 = 8;
                    a184 = 5;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a116 == 8) {{
            if (cf == 1 && a87 == 11) {
              if (a364 == 11 && cf == 1) {{
                  if (cf == 1 && a102 == 12 && a364 == 11 && a116 == 8 && input == 8 && a87 == 11) {
                    cf = 0;
                    a116 = 6;
                    a22 = 36;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 5 && (a364 == 11 && (cf == 1 && a102 == 12) && a87 == 11) && a116 == 8) {
                    cf = 0;
                    a306 = 9;
                    a102 = 9;
                    a20 = 4;
                    a2 = 11;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a87 == 12 && cf == 1) {
              if (a185 == 8 && cf == 1) {{
                  if (a102 == 12 && (input == 4 && (cf == 1 && a185 == 8 && a87 == 12)) && a116 == 8) {
                    cf = 0;
                    a79 = 33;
                    a116 = 9;
                    a154 = 16;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (cf == 1 && a185 == 8 && input == 2 && a116 == 8) && a87 == 12) {
                    cf = 0;
                    a102 = 8;
                    a289 = 32;
                    a182 = 11;
                    a200 = 6;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a185 == 8 && (cf == 1 && a102 == 12 && a87 == 12 && a116 == 8) && input == 10) {
                    cf = 0;
                    a58 = 34;
                    a40 = 34;
                    a244 = 35;
                    a102 = 10;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a87 == 13) {
              if (cf == 1 && a159 == 6) {{
                  if (a159 == 6 && (cf == 1 && a87 == 13 && input == 1) && a116 == 8 && a102 == 12) {
                    cf = 0;
                    a332 = 34;
                    a102 = 9;
                    a20 = 7;
                    a62 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a159 == 6 && (a116 == 8 && (a87 == 13 && (cf == 1 && a102 == 12))) && input == 8) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a159 == 6 && (input == 7 && (a87 == 13 && cf == 1) && a102 == 12) && a116 == 8) {
                    cf = 0;
                    a144 = 35;
                    a102 = 8;
                    a289 = 32;
                    a182 = 15;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 9 && (a116 == 8 && cf == 1 && a87 == 13 && a102 == 12) && a159 == 6) {
                    cf = 0;
                    a40 = 32;
                    a67 = 36;
                    a385 = 36;
                    a102 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a116 == 8 && (a87 == 13 && cf == 1 && input == 3 && a159 == 6 && a102 == 12)) {
                    cf = 0;
                    a102 = 8;
                    a182 = 14;
                    a289 = 32;
                    a184 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a87 == 14 && cf == 1) {
              if (cf == 1 && a62 == 5) {{
                  if (a116 == 8 && (a102 == 12 && (input == 6 && (a62 == 5 && cf == 1) && a87 == 14))) {
                    cf = 0;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a62 == 5 && (input == 10 && (a116 == 8 && cf == 1) && a102 == 12) && a87 == 14) {
                    cf = 0;
                    a5 = 32;
                    a116 = 5;
                    a117 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a62 == 8) {{
                  if (a102 == 12 && (a87 == 14 && (a116 == 8 && (cf == 1 && a62 == 8)) && input == 2)) {
                    cf = 0;
                    a341 = 34;
                    a20 = 4;
                    a102 = 9;
                    a306 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 5 && (a87 == 14 && (a102 == 12 && (a62 == 8 && cf == 1))) && a116 == 8) {
                    cf = 0;
                    a102 = 14;
                    a310 = 9;
                    a125 = 15;
                    a182 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a87 == 16) {
              if (cf == 1 && a44 == 11) {{
                  if (a116 == 8 && (cf == 1 && a87 == 16 && input == 10 && a102 == 12) && a44 == 11) {
                    cf = 0;
                    a20 = 7;
                    a289 = 36;
                    a102 = 9;
                    a62 = 4;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a44 == 11 && cf == 1 && a87 == 16 && input == 4 && a102 == 12 && a116 == 8) {
                    cf = 0;
                    a222 = 34;
                    a102 = 13;
                    a270 = 36;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 1 && (a87 == 16 && (a102 == 12 && (a44 == 11 && cf == 1) && a116 == 8))) {
                    cf = 0;
                    a102 = 13;
                    a344 = 9;
                    a270 = 36;
                    a161 = 14;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a87 == 17) {
              if (cf == 1 && a385 == 33) {{
                  if (a102 == 12 && (a116 == 8 && (input == 6 && cf == 1) && a385 == 33 && a87 == 17)) {
                    cf = 0;
                    a102 = 11;
                    a83 = 34;
                    a314 = 35;
                    a117 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a116 == 8 && (input == 2 && (a87 == 17 && (cf == 1 && a385 == 33)) && a102 == 12)) {
                    cf = 0;
                    a74 = 32;
                    a116 = 7;
                    a28 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 12 && (a87 == 17 && (input == 10 && cf == 1) && a385 == 33 && a116 == 8)) {
                    cf = 0;
                    a79 = 35;
                    a146 = 34;
                    a116 = 9;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a87 == 17 && (a385 == 33 && (a102 == 12 && (cf == 1 && a116 == 8)) && input == 3)) {
                    cf = 0;
                    a112 = 35;
                    a130 = 9;
                    a102 = 7;
                    a116 = 5;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a116 == 9 && cf == 1) {{
            if (a79 == 33 && cf == 1) {
              if (a154 == 12 && cf == 1) {{
                  if (a154 == 12 && (a116 == 9 && cf == 1) && a102 == 12 && input == 1 && a79 == 33) {
                    cf = 0;
                    a102 = 9;
                    a126 = 32;
                    a20 = 8;
                    a271 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a154 == 16) {{
                  if (a79 == 33 && (input == 4 && (a116 == 9 && (a154 == 16 && (a102 == 12 && cf == 1))))) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a79 == 34) {
              if (cf == 1 && a106 == 34) {{
                  if (a116 == 9 && (a79 == 34 && (cf == 1 && a102 == 12 && a106 == 34 && input == 10))) {
                    cf = 0;
                    a20 = 2;
                    a102 = 9;
                    a12 = 34;
                    a63 = 11;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a79 == 35 && cf == 1) {
              if (cf == 1 && a146 == 34) {{
                  if (a102 == 12 && (a79 == 35 && (a146 == 34 && cf == 1) && a116 == 9 && input == 4)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 3;
                    a168 = 8;
                    a30 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a116 == 9 && (input == 5 && (a79 == 35 && cf == 1) && a102 == 12 && a146 == 34)) {
                    cf = 0;
                    a168 = 8;
                    a130 = 3;
                    a102 = 7;
                    a30 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a116 == 9 && (cf == 1 && a79 == 35 && a102 == 12 && input == 6) && a146 == 34) {
                    cf = 0;
                    a40 = 33;
                    a102 = 10;
                    a178 = 5;
                    a170 = 8;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 13 && cf == 1) {
        if (a270 == 33 && cf == 1) {{
            if (a172 == 32 && cf == 1) {
              if (a70 == 33 && cf == 1) {{
                  if (a172 == 32 && (a270 == 33 && (cf == 1 && a102 == 13) && input == 6) && a70 == 33) {
                    cf = 0;
                    a130 = 10;
                    a89 = 32;
                    a102 = 7;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a270 == 33 && (a172 == 32 && (cf == 1 && input == 1) && a102 == 13) && a70 == 33) {
                    cf = 0;
                    a102 = 7;
                    a130 = 7;
                    a162 = 7;
                    a28 = 3;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 10 && (a172 == 32 && cf == 1 && a70 == 33) && a102 == 13 && a270 == 33) {
                    cf = 0;
                    a102 = 8;
                    a289 = 34;
                    a57 = 36;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a172 == 32 && (a270 == 33 && (a102 == 13 && (input == 8 && (a70 == 33 && cf == 1))))) {
                    cf = 0;
                    a112 = 32;
                    a130 = 9;
                    a102 = 7;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a172 == 35) {
              if (cf == 1 && a164 == 33) {{
                  if (input == 2 && (a102 == 13 && (a172 == 35 && (a270 == 33 && cf == 1) && a164 == 33))) {
                    cf = 0;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a164 == 33 && (cf == 1 && a270 == 33 && a102 == 13) && a172 == 35 && input == 10) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a270 == 32) {{
            if (a69 == 8 && cf == 1) {
              if (a35 == 34 && cf == 1) {{
                  if (a102 == 13 && (a69 == 8 && (a270 == 32 && (a35 == 34 && (cf == 1 && input == 1))))) {
                    cf = 0;
                    a130 = 8;
                    a102 = 7;
                    a148 = 36;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a270 == 32 && (a69 == 8 && (a35 == 34 && (input == 3 && (a102 == 13 && cf == 1))))) {
                    cf = 0;
                    a197 = 35;
                    a102 = 11;
                    a314 = 32;
                    a140 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a35 == 36) {{
                  if (a102 == 13 && (a270 == 32 && (a69 == 8 && cf == 1 && a35 == 36 && input == 7))) {
                    cf = 0;
                    a16 = 32;
                    a102 = 11;
                    a314 = 33;
                    a192 = 7;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a69 == 9 && cf == 1) {
              if (a81 == 32 && cf == 1) {{
                  if (a270 == 32 && (a69 == 9 && (a102 == 13 && cf == 1 && a81 == 32) && input == 8)) {
                    cf = 0;
                    a102 = 14;
                    a163 = 32;
                    a113 = 35;
                    a310 = 14;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (a81 == 35 && cf == 1) {{
                  if (input == 8 && (a102 == 13 && (a81 == 35 && (a270 == 32 && cf == 1))) && a69 == 9) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a22 = 36;
                    a56 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a270 == 32 && cf == 1 && a69 == 9 && a81 == 35 && input == 5 && a102 == 13) {
                    cf = 0;
                    a87 = 14;
                    a116 = 8;
                    a102 = 12;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a69 == 10 && cf == 1) {
              if (cf == 1 && a9 == 33) {{
                  if (input == 6 && (cf == 1 && a9 == 33 && a102 == 13) && a69 == 10 && a270 == 32) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a69 == 14 && cf == 1) {
              if (cf == 1 && a390 == 6) {{
                  if (a270 == 32 && cf == 1 && input == 10 && a69 == 14 && a102 == 13 && a390 == 6) {
                    cf = 0;
                    a78 = 35;
                    a129 = 32;
                    a102 = 9;
                    a20 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                  if (cf == 1 && a390 == 6 && a270 == 32 && input == 1 && a102 == 13 && a69 == 14) {
                    cf = 0;
                    a344 = 12;
                    a270 = 36;
                    a36 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 13 && (a69 == 14 && (a270 == 32 && cf == 1) && a390 == 6) && input == 5) {
                    cf = 0;
                    a310 = 16;
                    a102 = 14;
                    a176 = 34;
                    a364 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
              if (a390 == 9 && cf == 1) {{
                  if (input == 6 && (a102 == 13 && (a69 == 14 && cf == 1 && a390 == 9)) && a270 == 32) {
                    cf = 0;
                    a87 = 14;
                    a116 = 8;
                    a102 = 12;
                    a62 = 5;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a69 == 14 && (a102 == 13 && cf == 1) && a390 == 9 && a270 == 32 && input == 3) {
                    cf = 0;
                    a168 = 1;
                    a130 = 3;
                    a102 = 7;
                    a162 = 5;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a390 == 9 && (a270 == 32 && (a69 == 14 && cf == 1)) && a102 == 13 && input == 4) {
                    cf = 0;
                    a57 = 36;
                    a102 = 12;
                    a116 = 2;
                    a7 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a270 == 34) {{
            if (a9 == 32 && cf == 1) {
              if (cf == 1 && a183 == 6) {{
                  if (input == 1 && (a9 == 32 && (a183 == 6 && (a102 == 13 && cf == 1))) && a270 == 34) {
                    cf = 0;
                    a183 = 4;
                    a20 = 1;
                    a102 = 9;
                    a17 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                  if (input == 8 && (a102 == 13 && cf == 1 && a183 == 6 && a9 == 32) && a270 == 34) {
                    cf = 0;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a9 == 34 && cf == 1) {
              if (a47 == 8 && cf == 1) {{
                  if (a47 == 8 && (a9 == 34 && (a270 == 34 && (input == 4 && (cf == 1 && a102 == 13))))) {
                    cf = 0;
                    a270 = 35;
                    a73 = 36;
                    a170 = 12;
                    output = 15;
                    fflush(stdout);
                  }
                  if (cf == 1 && a9 == 34 && input == 8 && a102 == 13 && a47 == 8 && a270 == 34) {
                    cf = 0;
                    a178 = 7;
                    a40 = 33;
                    a102 = 10;
                    a271 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a47 == 13) {{
                  if (a270 == 34 && (a102 == 13 && (a9 == 34 && (cf == 1 && a47 == 13 && input == 2)))) {
                    cf = 0;
                    a121 = 33;
                    a112 = 36;
                    a102 = 7;
                    a130 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a47 == 13 && (input == 8 && (cf == 1 && a102 == 13)) && a9 == 34 && a270 == 34) {
                    cf = 0;
                    a310 = 9;
                    a125 = 15;
                    a102 = 14;
                    a182 = 14;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a9 == 35) {
              if (a40 == 33 && cf == 1) {{
                  if (cf == 1 && a40 == 33 && a9 == 35 && a102 == 13 && input == 8 && a270 == 34) {
                    cf = 0;
                    a289 = 34;
                    a57 = 36;
                    a102 = 8;
                    a138 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a9 == 35 && (a40 == 33 && (a270 == 34 && (input == 10 && cf == 1)) && a102 == 13)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 4;
                    a306 = 6;
                    a117 = 12;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a9 == 36 && cf == 1) {
              if (a190 == 35 && cf == 1) {{
                  if (a102 == 13 && (a190 == 35 && (a270 == 34 && (cf == 1 && a9 == 36)) && input == 8)) {
                    cf = 0;
                    a162 = 5;
                    a130 = 7;
                    a102 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 13 && (a190 == 35 && (a9 == 36 && cf == 1) && a270 == 34 && input == 4)) {
                    cf = 0;
                    a102 = 8;
                    a289 = 34;
                    a57 = 32;
                    a335 = 11;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a270 == 35 && cf == 1) {{
            if (a170 == 7 && cf == 1) {
              if (a157 == 36 && cf == 1) {{
                  if (a170 == 7 && (input == 6 && (a102 == 13 && (a157 == 36 && (cf == 1 && a270 == 35))))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a143 = 5;
                    a49 = 8;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a170 == 7 && cf == 1 && a270 == 35 && a157 == 36 && a102 == 13 && input == 1) {
                    cf = 0;
                    a20 = 8;
                    a271 = 12;
                    a102 = 9;
                    a31 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a170 == 10) {
              if (a55 == 5 && cf == 1) {{
                  if (a102 == 13 && (a55 == 5 && (a170 == 10 && (cf == 1 && input == 2)) && a270 == 35)) {
                    cf = 0;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a170 == 12) {
              if (a73 == 36 && cf == 1) {{
                  if (cf == 1 && a73 == 36 && a102 == 13 && a270 == 35 && input == 4 && a170 == 12) {
                    cf = 0;
                    a178 = 7;
                    a102 = 10;
                    a40 = 33;
                    a271 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                  if (cf == 1 && a170 == 12 && input == 8 && a102 == 13 && a270 == 35 && a73 == 36) {
                    cf = 0;
                    a162 = 5;
                    a102 = 7;
                    a130 = 7;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a170 == 14 && cf == 1) {
              if (cf == 1 && a96 == 8) {{
                  if (a270 == 35 && (a96 == 8 && (cf == 1 && a170 == 14) && input == 2) && a102 == 13) {
                    cf = 0;
                    a79 = 33;
                    a102 = 12;
                    a116 = 9;
                    a154 = 12;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a270 == 36) {{
            if (a344 == 8 && cf == 1) {
              if (cf == 1 && a153 == 4) {{
                  if (a153 == 4 && (input == 4 && (a270 == 36 && (cf == 1 && a344 == 8)) && a102 == 13)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 9;
                    a112 = 34;
                    a168 = 1;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a344 == 8 && (a102 == 13 && (a153 == 4 && cf == 1) && input == 2 && a270 == 36)) {
                    cf = 0;
                    a75 = 34;
                    a289 = 33;
                    a102 = 8;
                    a141 = 7;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 6 && (a270 == 36 && (a153 == 4 && cf == 1 && a102 == 13)) && a344 == 8) {
                    cf = 0;
                    a190 = 34;
                    a102 = 14;
                    a126 = 34;
                    a310 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a344 == 8 && (cf == 1 && a102 == 13 && a270 == 36 && input == 8 && a153 == 4)) {
                    cf = 0;
                    a102 = 9;
                    a20 = 4;
                    a306 = 8;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a153 == 8) {{
                  if (input == 8 && (a102 == 13 && (cf == 1 && a344 == 8) && a270 == 36 && a153 == 8)) {
                    cf = 0;
                    a102 = 7;
                    a143 = 10;
                    a130 = 10;
                    a49 = 9;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a344 == 9 && cf == 1) {
              if (a161 == 14 && cf == 1) {{
                  if (a102 == 13 && (input == 7 && (a344 == 9 && (a270 == 36 && (cf == 1 && a161 == 14))))) {
                    cf = 0;
                    a222 = 34;
                    a344 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                  if (input == 3 && (a270 == 36 && (a102 == 13 && (a344 == 9 && cf == 1) && a161 == 14))) {
                    cf = 0;
                    a80 = 32;
                    a102 = 7;
                    a103 = 34;
                    a130 = 5;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a344 == 12) {
              if (a36 == 9 && cf == 1) {{
                  if (a344 == 12 && (cf == 1 && a270 == 36 && input == 1 && a36 == 9 && a102 == 13)) {
                    cf = 0;
                    a310 = 13;
                    a122 = 34;
                    a102 = 14;
                    a242 = 6;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a344 == 12 && (a36 == 9 && (a102 == 13 && (input == 3 && cf == 1))) && a270 == 36) {
                    cf = 0;
                    a310 = 16;
                    a102 = 14;
                    a176 = 34;
                    a364 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a36 == 10) {{
                  if (a36 == 10 && (a344 == 12 && (cf == 1 && a102 == 13)) && input == 1 && a270 == 36) {
                    cf = 0;
                    a102 = 14;
                    a122 = 34;
                    a310 = 13;
                    a242 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a344 == 12 && (a270 == 36 && (cf == 1 && a36 == 10)) && a102 == 13 && input == 5) {
                    cf = 0;
                    a102 = 12;
                    a67 = 36;
                    a116 = 4;
                    a310 = 15;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 4 && (a344 == 12 && (a102 == 13 && cf == 1) && a270 == 36 && a36 == 10)) {
                    cf = 0;
                    a102 = 14;
                    a310 = 14;
                    a163 = 34;
                    a15 = 7;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a344 == 14 && cf == 1) {
              if (a194 == 9 && cf == 1) {{
                  if (a194 == 9 && cf == 1 && a270 == 36 && input == 7 && a102 == 13 && a344 == 14) {
                    cf = 0;
                    a112 = 34;
                    a102 = 7;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a270 == 36 && (a344 == 14 && (a194 == 9 && cf == 1) && a102 == 13) && input == 8) {
                    cf = 0;
                    a314 = 34;
                    a102 = 11;
                    a106 = 35;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a344 == 14 && (cf == 1 && a194 == 9 && input == 1 && a102 == 13) && a270 == 36) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a222 = 34;
                    a56 = 15;
                    output = 24;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a344 == 15 && cf == 1) {
              if (cf == 1 && a222 == 34) {{
                  if (a222 == 34 && (a344 == 15 && (a270 == 36 && (a102 == 13 && cf == 1)) && input == 5)) {
                    cf = 0;
                    a102 = 10;
                    a178 = 5;
                    a40 = 33;
                    a170 = 12;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a270 == 36 && (a222 == 34 && (cf == 1 && a102 == 13)) && a344 == 15 && input == 8) {
                    cf = 0;
                    a112 = 34;
                    a102 = 7;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
      if (a102 == 14 && cf == 1) {
        if (a310 == 9 && cf == 1) {{
            if (a125 == 10 && cf == 1) {
              if (a179 == 33 && cf == 1) {{
                  if (input == 2 && (a125 == 10 && (a102 == 14 && (a179 == 33 && cf == 1)) && a310 == 9)) {
                    cf = 0;
                    a87 = 14;
                    a102 = 12;
                    a116 = 8;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a125 == 11 && cf == 1) {
              if (cf == 1 && a69 == 9) {{
                  if (a125 == 11 && (input == 6 && (cf == 1 && a102 == 14) && a310 == 9) && a69 == 9) {
                    cf = 0;
                    a102 = 13;
                    a344 = 12;
                    a270 = 36;
                    a36 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
              if (a69 == 10 && cf == 1) {{
                  if (a102 == 14 && (a69 == 10 && (cf == 1 && a310 == 9 && a125 == 11)) && input == 8) {
                    cf = 0;
                    a310 = 16;
                    a176 = 35;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a125 == 11 && (input == 3 && (cf == 1 && a310 == 9)) && a69 == 10)) {
                    cf = 0;
                    a62 = 6;
                    a102 = 9;
                    a20 = 7;
                    a98 = 11;
                    output = 16;
                    fflush(stdout);
                  }
                  if (input == 1 && (a102 == 14 && (cf == 1 && a69 == 10) && a310 == 9 && a125 == 11)) {
                    cf = 0;
                    a40 = 35;
                    a102 = 10;
                    a98 = 5;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a69 == 10 && (a310 == 9 && cf == 1) && a125 == 11) && input == 7) {
                    cf = 0;
                    a83 = 33;
                    a102 = 12;
                    a57 = 33;
                    a116 = 2;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a69 == 13) {{
                  if (a125 == 11 && (a310 == 9 && cf == 1 && a102 == 14) && input == 3 && a69 == 13) {
                    cf = 0;
                    a83 = 34;
                    a102 = 12;
                    a57 = 33;
                    a116 = 2;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a125 == 12) {
              if (cf == 1 && a23 == 9) {{
                  if (input == 1 && (a102 == 14 && (a23 == 9 && (a310 == 9 && (cf == 1 && a125 == 12))))) {
                    cf = 0;
                    a102 = 7;
                    a112 = 34;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a125 == 12 && (a23 == 9 && (a102 == 14 && (cf == 1 && input == 4) && a310 == 9))) {
                    cf = 0;
                    a102 = 12;
                    a270 = 33;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                  if (input == 10 && (cf == 1 && a310 == 9 && a125 == 12 && a102 == 14) && a23 == 9) {
                    cf = 0;
                    a270 = 33;
                    a102 = 12;
                    a232 = 36;
                    a116 = 3;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a125 == 13 && cf == 1) {
              if (a87 == 12 && cf == 1) {{
                  if (input == 1 && (cf == 1 && a310 == 9 && a102 == 14) && a87 == 12 && a125 == 13) {
                    cf = 0;
                    a130 = 5;
                    a103 = 35;
                    a102 = 7;
                    a116 = 7;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a87 == 12 && (cf == 1 && a125 == 13 && a310 == 9 && a102 == 14) && input == 2) {
                    cf = 0;
                    a102 = 8;
                    a289 = 35;
                    a249 = 34;
                    a111 = 8;
                    output = 22;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a125 == 14 && cf == 1) {
              if (cf == 1 && a190 == 34) {{
                  if (input == 7 && (a190 == 34 && cf == 1 && a102 == 14 && a125 == 14 && a310 == 9)) {
                    cf = 0;
                    a102 = 7;
                    a130 = 9;
                    a112 = 35;
                    a116 = 4;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a190 == 34 && (cf == 1 && a102 == 14) && a125 == 14 && a310 == 9 && input == 1) {
                    cf = 0;
                    a306 = 4;
                    a102 = 9;
                    a20 = 4;
                    a28 = 2;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a125 == 15) {
              if (a182 == 13 && cf == 1) {{
                  if (a182 == 13 && (a102 == 14 && (a125 == 15 && (a310 == 9 && (cf == 1 && input == 8))))) {
                    cf = 0;
                    a116 = 8;
                    a102 = 12;
                    a87 = 13;
                    a159 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a125 == 15 && (a102 == 14 && (input == 4 && (cf == 1 && a310 == 9))) && a182 == 13) {
                    cf = 0;
                    a102 = 12;
                    a116 = 9;
                    a79 = 36;
                    a292 = 6;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (cf == 1 && a310 == 9) && input == 7 && a125 == 15 && a182 == 13) {
                    cf = 0;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a182 == 13 && cf == 1 && a125 == 15 && input == 1 && a310 == 9 && a102 == 14) {
                    cf = 0;
                    a112 = 34;
                    a130 = 9;
                    a102 = 7;
                    a168 = 2;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a182 == 14) {{
                  if (a125 == 15 && (cf == 1 && a310 == 9) && input == 6 && a102 == 14 && a182 == 14) {
                    cf = 0;
                    a16 = 32;
                    a102 = 11;
                    a314 = 33;
                    a192 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a125 == 15 && (cf == 1 && a182 == 14 && input == 3) && a310 == 9 && a102 == 14) {
                    cf = 0;
                    a314 = 33;
                    a16 = 32;
                    a102 = 11;
                    a192 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a182 == 14 && (input == 1 && cf == 1 && a125 == 15) && a102 == 14 && a310 == 9) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 10 && cf == 1) {{
            if (a190 == 33 && cf == 1) {
              if (a71 == 11 && cf == 1) {{
                  if (a102 == 14 && (input == 2 && (cf == 1 && a190 == 33) && a71 == 11) && a310 == 10) {
                    cf = 0;
                    a306 = 9;
                    a102 = 9;
                    a20 = 4;
                    a2 = 6;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 7 && (a190 == 33 && (cf == 1 && a310 == 10 && a102 == 14)) && a71 == 11) {
                    cf = 0;
                    a12 = 34;
                    a20 = 2;
                    a102 = 9;
                    a63 = 12;
                    output = 24;
                    fflush(stdout);
                  }
                  if (a71 == 11 && (a102 == 14 && (cf == 1 && a310 == 10 && input == 8 && a190 == 33))) {
                    cf = 0;
                    a102 = 13;
                    a344 = 8;
                    a270 = 36;
                    a153 = 4;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a71 == 13) {{
                  if (a190 == 33 && (a71 == 13 && (a310 == 10 && (cf == 1 && a102 == 14))) && input == 1) {
                    cf = 0;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a190 == 33 && (a310 == 10 && (input == 10 && cf == 1) && a71 == 13) && a102 == 14) {
                    cf = 0;
                    a130 = 7;
                    a102 = 7;
                    a162 = 5;
                    a175 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a71 == 13 && (input == 7 && cf == 1 && a310 == 10 && a102 == 14 && a190 == 33)) {
                    cf = 0;
                    a296 = 35;
                    a91 = 36;
                    a310 = 12;
                    output = 21;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a190 == 34) {
              if (cf == 1 && a126 == 33) {{
                  if (a190 == 34 && (input == 2 && (a126 == 33 && (a310 == 10 && cf == 1))) && a102 == 14) {
                    cf = 0;
                    a190 = 33;
                    a71 = 13;
                    output = 17;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a126 == 33 && (a310 == 10 && cf == 1) && a190 == 34 && input == 6)) {
                    cf = 0;
                    a20 = 3;
                    a129 = 33;
                    a102 = 9;
                    a344 = 10;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a126 == 34) {{
                  if (a102 == 14 && (a310 == 10 && (a190 == 34 && (cf == 1 && a126 == 34) && input == 10))) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a190 == 34 && (a310 == 10 && (a126 == 34 && cf == 1 && a102 == 14) && input == 7)) {
                    cf = 0;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a190 == 36 && cf == 1) {
              if (a28 == 7 && cf == 1) {{
                  if (a310 == 10 && (a190 == 36 && (cf == 1 && a102 == 14 && a28 == 7) && input == 6)) {
                    cf = 0;
                    a314 = 33;
                    a8 = 35;
                    a16 = 36;
                    a102 = 11;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 11 && cf == 1) {{
            if (a0 == 3 && cf == 1) {
              if (a154 == 10 && cf == 1) {{
                  if (a310 == 11 && (input == 7 && (a154 == 10 && cf == 1)) && a102 == 14 && a0 == 3) {
                    cf = 0;
                    a270 = 33;
                    a172 = 35;
                    a164 = 33;
                    a102 = 13;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a0 == 4 && cf == 1) {
              if (cf == 1 && a176 == 34) {{
                  if (a102 == 14 && (cf == 1 && input == 6 && a0 == 4) && a310 == 11 && a176 == 34) {
                    cf = 0;
                    a102 = 9;
                    a120 = 33;
                    a20 = 6;
                    a127 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a0 == 4 && (a102 == 14 && (a176 == 34 && cf == 1 && a310 == 11)) && input == 8) {
                    cf = 0;
                    a102 = 11;
                    a314 = 32;
                    a197 = 35;
                    a140 = 13;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a176 == 34 && (a0 == 4 && (a102 == 14 && (cf == 1 && a310 == 11 && input == 2)))) {
                    cf = 0;
                    a130 = 8;
                    a182 = 9;
                    a102 = 7;
                    a386 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (input == 3 && (a0 == 4 && (a310 == 11 && (a102 == 14 && cf == 1))) && a176 == 34) {
                    cf = 0;
                    a183 = 6;
                    a102 = 9;
                    a20 = 1;
                    a42 = 13;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a310 == 11 && (a0 == 4 && (cf == 1 && a176 == 34) && input == 4) && a102 == 14) {
                    cf = 0;
                    a119 = 35;
                    a40 = 34;
                    a244 = 33;
                    a102 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a0 == 7) {
              if (a175 == 13 && cf == 1) {{
                  if (a175 == 13 && (a0 == 7 && cf == 1 && a102 == 14 && input == 10 && a310 == 11)) {
                    cf = 0;
                    a102 = 12;
                    a116 = 3;
                    a232 = 34;
                    a344 = 14;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a310 == 11 && (input == 1 && (a175 == 13 && cf == 1 && a0 == 7) && a102 == 14)) {
                    cf = 0;
                    a9 = 34;
                    a270 = 34;
                    a102 = 13;
                    a47 = 8;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a0 == 10) {
              if (a120 == 33 && cf == 1) {{
                  if (a120 == 33 && (a102 == 14 && (a0 == 10 && (a310 == 11 && cf == 1 && input == 4)))) {
                    cf = 0;
                    a176 = 34;
                    a0 = 4;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a120 == 32) {{
                  if (a102 == 14 && (a120 == 32 && (a0 == 10 && cf == 1 && input == 8) && a310 == 11)) {
                    cf = 0;
                    a102 = 9;
                    a67 = 36;
                    a20 = 7;
                    a62 = 10;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a310 == 12) {{
            if (a296 == 35 && cf == 1) {
              if (a91 == 32 && cf == 1) {{
                  if (input == 4 && (cf == 1 && a91 == 32 && a296 == 35 && a102 == 14 && a310 == 12)) {
                    cf = 0;
                    a116 = 4;
                    a67 = 34;
                    a102 = 12;
                    a142 = 8;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a296 == 35 && (a91 == 32 && (a102 == 14 && cf == 1)) && a310 == 12 && input == 10) {
                    cf = 0;
                    a130 = 3;
                    a168 = 6;
                    a102 = 7;
                    a242 = 10;
                    output = 16;
                    fflush(stdout);
                  }
                  if (a296 == 35 && (cf == 1 && a91 == 32 && a102 == 14 && input == 3 && a310 == 12)) {
                    cf = 0;
                    a106 = 34;
                    a102 = 12;
                    a79 = 34;
                    a116 = 9;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a296 == 35 && (cf == 1 && input == 5)) && a310 == 12 && a91 == 32) {
                    cf = 0;
                    a8 = 34;
                    a314 = 33;
                    a16 = 36;
                    a102 = 11;
                    output = 15;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a310 == 12 && (a296 == 35 && (a91 == 32 && cf == 1 && input == 2)))) {
                    cf = 0;
                    a314 = 36;
                    a78 = 32;
                    a102 = 11;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a91 == 36) {{
                  if (a310 == 12 && (a296 == 35 && (a91 == 36 && (input == 5 && (cf == 1 && a102 == 14))))) {
                    cf = 0;
                    a102 = 12;
                    a116 = 6;
                    a222 = 32;
                    a56 = 15;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 4 && (a91 == 36 && (a296 == 35 && (a310 == 12 && cf == 1)) && a102 == 14)) {
                    cf = 0;
                    a183 = 4;
                    a102 = 9;
                    a20 = 1;
                    a17 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 13 && cf == 1) {{
            if (a122 == 34 && cf == 1) {
              if (a242 == 6 && cf == 1) {{
                  if (a242 == 6 && (cf == 1 && input == 8 && a122 == 34) && a102 == 14 && a310 == 13) {
                    cf = 0;
                    a102 = 12;
                    a106 = 33;
                    a116 = 6;
                    a56 = 11;
                    output = 19;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (cf == 1 && a310 == 14) {{
            if (a163 == 32 && cf == 1) {
              if (cf == 1 && a113 == 35) {{
                  if (a102 == 14 && (a163 == 32 && (input == 10 && (a310 == 14 && cf == 1 && a113 == 35)))) {
                    cf = 0;
                    a57 = 36;
                    a289 = 34;
                    a102 = 8;
                    a138 = 15;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a163 == 32 && (cf == 1 && a310 == 14 && a102 == 14) && a113 == 35 && input == 8) {
                    cf = 0;
                    a248 = 8;
                    a102 = 7;
                    a130 = 6;
                    a335 = 7;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a310 == 14 && (input == 1 && cf == 1) && a163 == 32 && a102 == 14 && a113 == 35) {
                    cf = 0;
                    a106 = 35;
                    a314 = 34;
                    a102 = 11;
                    a4 = 7;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a113 == 35 && (a102 == 14 && (input == 6 && (a163 == 32 && (a310 == 14 && cf == 1))))) {
                    cf = 0;
                    a102 = 7;
                    a130 = 10;
                    a89 = 32;
                    a143 = 9;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a163 == 34) {
              if (cf == 1 && a15 == 7) {{
                  if (a102 == 14 && (a163 == 34 && (input == 2 && cf == 1) && a15 == 7) && a310 == 14) {
                    cf = 0;
                    a289 = 34;
                    a57 = 35;
                    a102 = 8;
                    a130 = 8;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (input == 7 && (a15 == 7 && (a163 == 34 && (a310 == 14 && cf == 1))))) {
                    cf = 0;
                    a125 = 11;
                    a310 = 9;
                    a69 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a15 == 7 && cf == 1 && a102 == 14 && a310 == 14 && input == 6 && a163 == 34) {
                    cf = 0;
                    a20 = 7;
                    a62 = 3;
                    a102 = 9;
                    a171 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (a15 == 10 && cf == 1) {{
                  if (input == 10 && (a15 == 10 && (a310 == 14 && cf == 1 && a102 == 14)) && a163 == 34) {
                    cf = 0;
                    a67 = 36;
                    a116 = 4;
                    a102 = 12;
                    a310 = 12;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a163 == 35 && cf == 1) {
              if (cf == 1 && a58 == 32) {{
                  if (a310 == 14 && (a163 == 35 && (a58 == 32 && (a102 == 14 && (input == 2 && cf == 1))))) {
                    cf = 0;
                    a178 = 5;
                    a40 = 33;
                    a102 = 10;
                    a170 = 10;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a310 == 14 && cf == 1 && a102 == 14 && a163 == 35 && a58 == 32 && input == 4) {
                    cf = 0;
                    a112 = 32;
                    a102 = 7;
                    a130 = 9;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 3 && cf == 1 && a102 == 14 && a58 == 32 && a310 == 14 && a163 == 35) {
                    cf = 0;
                    a112 = 34;
                    a130 = 9;
                    a102 = 7;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 15 && cf == 1) {{
            if (a71 == 8 && cf == 1) {
              if (a197 == 35 && cf == 1) {{
                  if (a197 == 35 && (input == 1 && (a71 == 8 && cf == 1)) && a102 == 14 && a310 == 15) {
                    cf = 0;
                    a163 = 35;
                    a58 = 32;
                    a310 = 14;
                    output = 15;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a71 == 9) {
              if (cf == 1 && a166 == 33) {{
                  if (a310 == 15 && (a102 == 14 && (a71 == 9 && (input == 1 && cf == 1) && a166 == 33))) {
                    cf = 0;
                    a83 = 34;
                    a102 = 11;
                    a314 = 35;
                    a117 = 14;
                    output = 21;
                    fflush(stdout);
                  }
                  if (a310 == 15 && (input == 7 && (a102 == 14 && (a166 == 33 && cf == 1) && a71 == 9))) {
                    cf = 0;
                    a130 = 8;
                    a148 = 36;
                    a102 = 7;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a71 == 9 && cf == 1 && a166 == 33 && a310 == 15) && input == 4) {
                    cf = 0;
                    a130 = 8;
                    a102 = 7;
                    a148 = 36;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a71 == 10) {
              if (cf == 1 && a22 == 33) {{
                  if (a22 == 33 && (a310 == 15 && (input == 4 && cf == 1)) && a71 == 10 && a102 == 14) {
                    cf = 0;
                    a130 = 9;
                    a102 = 7;
                    a112 = 32;
                    a32 = 10;
                    output = 23;
                    fflush(stdout);
                  }
                  if (a71 == 10 && cf == 1 && a102 == 14 && a22 == 33 && input == 3 && a310 == 15) {
                    cf = 0;
                    a112 = 34;
                    a102 = 7;
                    a130 = 9;
                    a168 = 7;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a102 == 14 && (a22 == 33 && cf == 1) && a310 == 15 && input == 1 && a71 == 10) {
                    cf = 0;
                    a116 = 8;
                    a102 = 12;
                    a385 = 32;
                    a87 = 10;
                    output = 18;
                    fflush(stdout);
                  }
                }
              }
            }
            if (a71 == 11 && cf == 1) {
              if (cf == 1 && a296 == 33) {{
                  if (a296 == 33 && (input == 7 && (cf == 1 && a102 == 14 && a310 == 15) && a71 == 11)) {
                    cf = 0;
                    a310 = 9;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                  if (input == 8 && (a102 == 14 && (cf == 1 && a310 == 15 && a296 == 33 && a71 == 11))) {
                    cf = 0;
                    a116 = 8;
                    a87 = 13;
                    a102 = 12;
                    a159 = 6;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a296 == 33 && (a71 == 11 && (input == 1 && (a102 == 14 && cf == 1 && a310 == 15)))) {
                    cf = 0;
                    a116 = 7;
                    a74 = 32;
                    a102 = 12;
                    a28 = 3;
                    output = 17;
                    fflush(stdout);
                  }
                }
              }
            }
            if (cf == 1 && a71 == 13) {
              if (cf == 1 && a25 == 32) {{
                  if (a71 == 13 && (input == 7 && (a25 == 32 && cf == 1 && a102 == 14) && a310 == 15)) {
                    cf = 0;
                    a102 = 7;
                    a148 = 36;
                    a130 = 8;
                    a182 = 11;
                    output = 25;
                    fflush(stdout);
                  }
                  if (a25 == 32 && (a71 == 13 && (input == 1 && cf == 1 && a310 == 15) && a102 == 14)) {
                    cf = 0;
                    a102 = 7;
                    a182 = 9;
                    a130 = 8;
                    a386 = 9;
                    output = 19;
                    fflush(stdout);
                  }
                  if (a25 == 32 && cf == 1 && a102 == 14 && input == 10 && a71 == 13 && a310 == 15) {
                    cf = 0;
                    a310 = 11;
                    a176 = 34;
                    a0 = 4;
                    output = 23;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
        if (a310 == 16 && cf == 1) {{
            if (a176 == 34 && cf == 1) {
              if (a364 == 8 && cf == 1) {{
                  if (a364 == 8 && (cf == 1 && a310 == 16 && a102 == 14 && input == 8 && a176 == 34)) {
                    cf = 0;
                    a102 = 13;
                    a270 = 32;
                    a81 = 35;
                    a69 = 9;
                    output = 23;
                    fflush(stdout);
                  }
                  if (input == 1 && (a364 == 8 && (cf == 1 && a176 == 34) && a310 == 16) && a102 == 14) {
                    cf = 0;
                    a102 = 10;
                    a40 = 35;
                    a98 = 5;
                    output = 20;
                    fflush(stdout);
                  }
                  if (a176 == 34 && (a310 == 16 && (cf == 1 && input == 5 && a102 == 14 && a364 == 8))) {
                    cf = 0;
                    a102 = 12;
                    a116 = 8;
                    a87 = 14;
                    a62 = 8;
                    output = 18;
                    fflush(stdout);
                  }
                  if (a176 == 34 && (a310 == 16 && (cf == 1 && a102 == 14 && a364 == 8)) && input == 4) {
                    cf = 0;
                    a310 = 9;
                    a125 = 11;
                    a69 = 10;
                    output = 25;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a364 == 9) {{
                  if (a102 == 14 && (a176 == 34 && (input == 7 && cf == 1 && a310 == 16)) && a364 == 9) {
                    cf = 0;
                    a125 = 11;
                    a310 = 9;
                    a69 = 9;
                    output = 22;
                    fflush(stdout);
                  }
                  if (a176 == 34 && (a102 == 14 && (input == 6 && (cf == 1 && a364 == 9))) && a310 == 16) {
                    cf = 0;
                    a69 = 14;
                    a102 = 13;
                    a270 = 32;
                    a390 = 6;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
              if (cf == 1 && a364 == 11) {{
                  if (a364 == 11 && (a176 == 34 && (cf == 1 && a310 == 16) && a102 == 14) && input == 7) {
                    cf = 0;
                    a102 = 9;
                    a62 = 3;
                    a20 = 7;
                    a171 = 12;
                    output = 16;
                    fflush(stdout);
                  }
                }
              }
            }
          }
        }
      }
{
        if (a121 == 33 && a178 == 11 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(0);
        }
        if (a197 == 33 && a344 == 13 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(1);
        }
        if (a145 == 32 && a120 == 36 && a20 == 6 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(2);
        }
        if (36 == 36 && a106 == 34 && a314 == 34 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(3);
        }
        if (a9 == 34 && a98 == 8 && a40 == 35 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(4);
        }
        if (a164 == 35 && a74 == 36 && a116 == 7 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(5);
        }
        if (a157 == 32 && a170 == 7 && a270 == 35 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(6);
        }
        if (a111 == 10 && a249 == 34 && a289 == 35 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(7);
        }
        if (a106 == 36 && a56 == 11 && a116 == 6 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(8);
        }
        if (a170 == 12 && a178 == 5 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(9);
        }
        if (a135 == 32 && a24 == 33 && a40 == 36 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(10);
        }
        if (a130 == 8 && a57 == 35 && a289 == 34 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(11);
        }
        if (a70 == 36 && a170 == 9 && a270 == 35 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(12);
        }
        if (2 == 6 && a176 == 35 && a310 == 16 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(13);
        }
        if (a127 == 6 && a120 == 33 && a20 == 6 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(14);
        }
        if (a170 == 14 && a178 == 5 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(15);
        }
        if (a123 == 36 && a83 == 35 && a314 == 35 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(16);
        }
        if (a222 == 32 && a56 == 15 && a116 == 6 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(17);
        }
        if (a159 == 12 && a87 == 13 && a116 == 8 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(18);
        }
        if (a121 == 34 && a178 == 11 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(19);
        }
        if (2 == 2 && a176 == 35 && a310 == 16 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(20);
        }
        if (a2 == 8 && a306 == 9 && a20 == 4 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(21);
        }
        if (a142 == 7 && a67 == 34 && a116 == 4 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(22);
        }
        if (a98 == 6 && a62 == 6 && a20 == 7 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(23);
        }
        if (a87 == 11 && a12 == 32 && a20 == 2 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(24);
        }
        if (32 == 34 && a143 == 7 && a130 == 10 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(25);
        }
        if (a341 == 34 && a306 == 10 && a20 == 4 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(26);
        }
        if (a44 == 12 && a120 == 32 && a20 == 6 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(27);
        }
        if (a98 == 10 && a71 == 6 && a310 == 15 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(28);
        }
        if (a13 == 35 && a183 == 9 && a20 == 1 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(29);
        }
        if (a195 == 10 && a17 == 9 && a130 == 4 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(30);
        }
        if (a154 == 14 && a197 == 34 && a314 == 32 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(31);
        }
        if (a194 == 8 && a249 == 33 && a20 == 5 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(32);
        }
        if (a44 == 14 && a170 == 13 && a270 == 35 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(33);
        }
        if (a129 == 35 && a120 == 35 && a20 == 6 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(34);
        }
        if (a8 == 34 && a16 == 36 && a314 == 33 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(35);
        }
        if (a115 == 35 && a146 == 34 && a289 == 36 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(36);
        }
        if (34 == 35 && a162 == 11 && a130 == 7 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(37);
        }
        if (a335 == 10 && a57 == 32 && a289 == 34 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(38);
        }
        if (a242 == 6 && a168 == 6 && a130 == 3 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(39);
        }
        if (a47 == 11 && a9 == 34 && a270 == 34 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(40);
        }
        if (a385 == 32 && a87 == 10 && a116 == 8 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(41);
        }
        if (a36 == 11 && a344 == 12 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(42);
        }
        if (a42 == 13 && a183 == 6 && a20 == 1 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(43);
        }
        if (10 == 12 && a57 == 32 && a116 == 2 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(44);
        }
        if (a178 == 9 && a248 == 9 && a130 == 6 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(45);
        }
        if (a87 == 17 && a12 == 32 && a20 == 2 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(46);
        }
        if (a66 == 34 && a178 == 10 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(47);
        }
        if (a194 == 11 && a344 == 14 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(48);
        }
        if (12 == 12 && a74 == 34 && a116 == 7 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(49);
        }
        if (a156 == 32 && a176 == 32 && a310 == 16 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(50);
        }
        if (a117 == 8 && a5 == 32 && a116 == 5 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(51);
        }
        if (a332 == 35 && a62 == 9 && a20 == 7 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(52);
        }
        if (a49 == 5 && a143 == 5 && a130 == 10 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(53);
        }
        if (a292 == 10 && a79 == 36 && a116 == 9 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(54);
        }
        if (a161 == 13 && a344 == 11 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(55);
        }
        if (a306 == 6 && a183 == 10 && a20 == 1 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(56);
        }
        if (8 == 7 && a249 == 35 && a20 == 5 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(57);
        }
        if (a9 == 36 && a178 == 8 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(58);
        }
        if (a292 == 6 && a79 == 36 && a116 == 9 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(59);
        }
        if (a67 == 36 && a385 == 36 && a40 == 32 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(60);
        }
        if (a25 == 33 && a71 == 13 && a310 == 15 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(61);
        }
        if (a171 == 11 && a62 == 3 && a20 == 7 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(62);
        }
        if (a185 == 5 && a87 == 12 && a116 == 8 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(63);
        }
        if (a28 == 6 && a162 == 7 && a130 == 7 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(64);
        }
        if (a28 == 8 && a190 == 36 && a310 == 10 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(65);
        }
        if (a36 == 13 && a182 == 9 && a289 == 32 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(66);
        }
        if (a123 == 35 && a83 == 35 && a314 == 35 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(67);
        }
        if (a70 == 32 && a172 == 32 && a270 == 33 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(68);
        }
        if (a98 == 10 && a249 == 35 && a289 == 35 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(69);
        }
        if (a23 == 8 && a125 == 12 && a310 == 9 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(70);
        }
        if (a344 == 8 && a129 == 33 && a20 == 3 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(71);
        }
        if (a54 == 14 && a106 == 32 && a314 == 34 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(72);
        }
        if (a165 == 12 && a71 == 12 && a310 == 15 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(73);
        }
        if (a116 == 8 && a112 == 35 && a130 == 9 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(74);
        }
        if (a186 == 33 && a168 == 5 && a130 == 3 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(75);
        }
        if (a91 == 35 && a125 == 8 && a310 == 9 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(76);
        }
        if (11 == 15 && a244 == 32 && a40 == 34 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(77);
        }
        if (a323 == 1 && a385 == 32 && a40 == 32 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(78);
        }
        if (a197 == 32 && a344 == 13 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(79);
        }
        if (a4 == 11 && a106 == 35 && a314 == 34 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(80);
        }
        if (a244 == 33 && a112 == 33 && a130 == 9 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(81);
        }
        if (a195 == 14 && a17 == 9 && a130 == 4 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(82);
        }
        if (36 == 35 && a182 == 16 && a130 == 8 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(83);
        }
        if (a182 == 11 && a385 == 33 && a40 == 32 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(84);
        }
        if (10 == 13 && a344 == 10 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(85);
        }
        if (a3 == 9 && a182 == 13 && a289 == 32 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(86);
        }
        if (a78 == 36 && a0 == 5 && a310 == 11 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(87);
        }
        if (a119 == 36 && a244 == 33 && a40 == 34 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(88);
        }
        if (a143 == 10 && a248 == 10 && a130 == 6 && a102 == 7) {
          cf = 0;
          __VERIFIER_error(89);
        }
        if (a144 == 36 && a182 == 15 && a289 == 32 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(90);
        }
        if (a37 == 36 && a74 == 35 && a116 == 7 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(91);
        }
        if (33 == 34 && a271 == 6 && a20 == 8 && a102 == 9) {
          cf = 0;
          __VERIFIER_error(92);
        }
        if (a191 == 35 && a232 == 35 && a116 == 3 && a102 == 12) {
          cf = 0;
          __VERIFIER_error(93);
        }
        if (34 == 33 && a296 == 33 && a310 == 12 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(94);
        }
        if (a3 == 10 && a182 == 13 && a289 == 32 && a102 == 8) {
          cf = 0;
          __VERIFIER_error(95);
        }
        if (a170 == 10 && a178 == 5 && a40 == 33 && a102 == 10) {
          cf = 0;
          __VERIFIER_error(96);
        }
        if (a36 == 14 && a344 == 12 && a270 == 36 && a102 == 13) {
          cf = 0;
          __VERIFIER_error(97);
        }
        if (a86 == 7 && a197 == 33 && a314 == 32 && a102 == 11) {
          cf = 0;
          __VERIFIER_error(98);
        }
        if (a166 == 36 && a71 == 9 && a310 == 15 && a102 == 14) {
          cf = 0;
          __VERIFIER_error(99);
        }
      }
      if (cf == 1) 
        output = - 2;
    }
//edited by script: global output variable replacing printf in functions 
    if (output == - 2) {
      //fprintf(stderr,"Invalid input: %d\n",input);
    }
     else if (output != - 1) {
      printf("%d\n",output);
    }
 // }








}
