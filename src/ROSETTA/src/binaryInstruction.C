
#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "nonterminal.h"

void
Grammar::setUpBinaryInstructions ()
   {

// Support for binaries must be turned on via the configure command line.
#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT

  // DQ (3/14/2007): Added support in IR for binaries.
  // This function sets up the IR nodes to support the representation of a binary file (or machine instructions).
  // It can be either platform specific or non-platform specific.

  // tps (09/21/07): example code to handle ARM binaries

     // data transfer instructions
     NEW_TERMINAL_MACRO ( AsmArmMov ,    "AsmArmMov",     "AsmArmMovTag" );
     NEW_TERMINAL_MACRO ( AsmArmMvn ,    "AsmArmMvn",     "AsmArmMvnTag" );
     NEW_TERMINAL_MACRO ( AsmArmMrs ,    "AsmArmMrs",     "AsmArmMrsTag" );
     NEW_TERMINAL_MACRO ( AsmArmMsr ,    "AsmArmMsr",     "AsmArmMsrTag" );

     NEW_TERMINAL_MACRO ( AsmArmLdr ,    "AsmArmLdr",     "AsmArmLdrTag" );
     NEW_TERMINAL_MACRO ( AsmArmLdm ,    "AsmArmLdm",     "AsmArmLdmTag" );
     NEW_TERMINAL_MACRO ( AsmArmStr ,    "AsmArmStr",     "AsmArmStrTag" );
     NEW_TERMINAL_MACRO ( AsmArmStm ,    "AsmArmStm",     "AsmArmStmTag" );

     // Asm Arithmetic
     NEW_TERMINAL_MACRO ( AsmArmAdd ,    "AsmArmAdd",     "AsmArmAddTag" );
     NEW_TERMINAL_MACRO ( AsmArmAdc ,    "AsmArmAdc",     "AsmArmAdcTag" );
     NEW_TERMINAL_MACRO ( AsmArmSub ,    "AsmArmSub",     "AsmArmSubTag" );
     NEW_TERMINAL_MACRO ( AsmArmSbc ,    "AsmArmSbc",     "AsmArmSbcTag" );
     NEW_TERMINAL_MACRO ( AsmArmRsb ,    "AsmArmRsb",     "AsmArmRsbTag" );
     NEW_TERMINAL_MACRO ( AsmArmRsc ,    "AsmArmRsc",     "AsmArmRscTag" );
     NEW_TERMINAL_MACRO ( AsmArmMul ,    "AsmArmMul",     "AsmArmMulTag" );
     NEW_TERMINAL_MACRO ( AsmArmMla ,    "AsmArmMla",     "AsmArmMlaTag" );
     NEW_TERMINAL_MACRO ( AsmArmUmull ,    "AsmArmUmull",     "AsmArmUmullTag" );
     NEW_TERMINAL_MACRO ( AsmArmUmlal ,    "AsmArmUmlal",     "AsmArmUmlalTag" );
     NEW_TERMINAL_MACRO ( AsmArmSmull ,    "AsmArmSmull",     "AsmArmSmullTag" );
     NEW_TERMINAL_MACRO ( AsmArmSmlal ,    "AsmArmSmlal",     "AsmArmSmlalTag" );
     NEW_TERMINAL_MACRO ( AsmArmCmp ,    "AsmArmCmp",     "AsmArmCmpTag" );
     NEW_TERMINAL_MACRO ( AsmArmCmn ,    "AsmArmCmn",     "AsmArmCmnTag" );

     // Logical Instructions
     NEW_TERMINAL_MACRO ( AsmArmTst , "AsmArmTst", "AsmArmTstTag" );
     NEW_TERMINAL_MACRO ( AsmArmTeq , "AsmArmTeq", "AsmArmTeqTag" );
     NEW_TERMINAL_MACRO ( AsmArmAnd , "AsmArmAnd", "AsmArmAndTag" );
     NEW_TERMINAL_MACRO ( AsmArmEor , "AsmArmEor", "AsmArmEorTag" );
     NEW_TERMINAL_MACRO ( AsmArmOrr , "AsmArmOrr", "AsmArmOrrTag" );
     NEW_TERMINAL_MACRO ( AsmArmBic , "AsmArmBic", "AsmArmBicTag" );

     // Control Transfer Instructions
     NEW_TERMINAL_MACRO ( AsmArmB ,      "AsmArmB",      "AsmArmBTag" );
     NEW_TERMINAL_MACRO ( AsmArmBl ,     "AsmArmBl",    "AsmArmBlTag" );
     NEW_TERMINAL_MACRO ( AsmArmBx ,     "AsmArmBx",     "AsmArmBxTag" );

     // Misc Instructions
     NEW_TERMINAL_MACRO ( AsmArmSwp ,      "AsmArmSwp",     "AsmArmSwpTag" );
     NEW_TERMINAL_MACRO ( AsmArmSwi ,      "AsmArmSwi",     "AsmArmSwiTag" );

     NEW_NONTERMINAL_MACRO ( AsmArmArithmeticInstruction, 
                             AsmArmSub | AsmArmAdd | AsmArmAdc | AsmArmSbc | AsmArmRsb | AsmArmRsc |
			     AsmArmMul | AsmArmMla | AsmArmUmull | AsmArmUmlal | AsmArmSmull | AsmArmSmlal |
			     AsmArmCmp | AsmArmCmn
			     , "AsmArmArithmeticInstruction", "AsmArmArithmeticInstructionTag" );
     
     NEW_NONTERMINAL_MACRO ( AsmArmDataTransferInstruction, AsmArmMov | AsmArmMvn | AsmArmMrs | AsmArmMsr 
			      | AsmArmLdr | AsmArmLdm | AsmArmStr | AsmArmStm | AsmArmArithmeticInstruction
			     , "AsmArmDataTransferInstruction", "AsmArmDataTransferInstructionTag" );
     

     NEW_NONTERMINAL_MACRO ( AsmArmControlTransferInstruction, 
			     AsmArmB | AsmArmBl | AsmArmBx
			     , "AsmArmControlTransferInstruction", "AsmArmControlTransferInstructionTag" );

     NEW_NONTERMINAL_MACRO ( AsmArmMiscInstruction, 
			     AsmArmSwp | AsmArmSwi
			     , "AsmArmMiscInstruction", "AsmArmMiscInstructionTag" );

     NEW_NONTERMINAL_MACRO ( AsmArmLogicalInstruction, 
			     AsmArmTst | AsmArmTeq | AsmArmAnd | AsmArmEor | AsmArmOrr | AsmArmBic
			     , "AsmArmLogicalInstruction", "AsmArmLogicalInstructionTag" );


  // tps (08/08/07): added most of the 80086 instructions (except 80087)
  // the instructions are grouped by their purpose
     // data transfer instructions
     NEW_TERMINAL_MACRO ( Asmx86Mov ,    "Asmx86Mov",     "Asmx86MovTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMove ,  "Asmx86CMove",   "Asmx86CMoveTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovne , "Asmx86CMovne",  "Asmx86CMovneTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMova ,  "Asmx86CMova",   "Asmx86CMovaTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovae , "Asmx86CMovae",  "Asmx86CMovaeTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovb ,  "Asmx86CMovb",   "Asmx86CMovbTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovbe , "Asmx86CMovbe",  "Asmx86CMovbeTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovg ,  "Asmx86CMovg",   "Asmx86CMovgTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovge , "Asmx86CMovge",  "Asmx86CMovgeTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovl ,  "Asmx86CMovl",   "Asmx86CMovlTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovle , "Asmx86CMovle",  "Asmx86CMovleTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovc ,  "Asmx86CMovc",   "Asmx86CMovcTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovnc , "Asmx86CMovnc",  "Asmx86CMovncTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovo ,  "Asmx86CMovo",   "Asmx86CMovoTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovno , "Asmx86CMovno",  "Asmx86CMovnoTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovs ,  "Asmx86CMovs",   "Asmx86CMovsTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovns , "Asmx86CMovns",  "Asmx86CMovnsTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovp ,  "Asmx86CMovp",   "Asmx86CMovpTag" );
     NEW_TERMINAL_MACRO ( Asmx86CMovnp , "Asmx86CMovnp",  "Asmx86CMovnpTag" );

     NEW_TERMINAL_MACRO ( Asmx86Push ,  "Asmx86Push",  "Asmx86PushTag" );
     NEW_TERMINAL_MACRO ( Asmx86Pop ,   "Asmx86Pop",   "Asmx86PopTag" );
     NEW_TERMINAL_MACRO ( Asmx86Xchg ,  "Asmx86Xchg",  "Asmx86XchgTag" );
     NEW_TERMINAL_MACRO ( Asmx86Bswap , "Asmx86Bswap", "Asmx86BswapTag" );
     NEW_TERMINAL_MACRO ( Asmx86Xadd ,  "Asmx86Xadd",  "Asmx86XaddTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cmpxchg ,   "Asmx86Cmpxchg", "Asmx86CmpxchgTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cmpxchg8b , "Asmx86Cmpxchg8b", "Asmx86Cmpxchg8bTag" );
     NEW_TERMINAL_MACRO ( Asmx86Pusha , "Asmx86Pusha", "Asmx86PushaTag" );
     NEW_TERMINAL_MACRO ( Asmx86Popa ,  "Asmx86Popa",  "Asmx86PopaTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cwd ,   "Asmx86Cwd",   "Asmx86CwdTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cbw ,   "Asmx86Cbw",   "Asmx86CbwTag" );
     NEW_TERMINAL_MACRO ( Asmx86Movsx , "Asmx86Movsx", "Asmx86MovsxTag" );
     NEW_TERMINAL_MACRO ( Asmx86Movzx , "Asmx86Movzx", "Asmx86MovzxTag" );

     // Asmx86 Arithmetic
     NEW_TERMINAL_MACRO ( Asmx86Sub , "Asmx86Sub", "Asmx86SubTag" );
     NEW_TERMINAL_MACRO ( Asmx86Add , "Asmx86Add", "Asmx86AddTag" );
     NEW_TERMINAL_MACRO ( Asmx86Adc , "Asmx86Adc", "Asmx86AdcTag" );
     NEW_TERMINAL_MACRO ( Asmx86Sbb , "Asmx86Sbb", "Asmx86SbbTag" );
     NEW_TERMINAL_MACRO ( Asmx86Mul , "Asmx86Mul", "Asmx86MulTag" );
     NEW_TERMINAL_MACRO ( Asmx86IMul ,"Asmx86IMul","Asmx86IMulTag" );
     NEW_TERMINAL_MACRO ( Asmx86Div , "Asmx86Div", "Asmx86DivTag" );
     NEW_TERMINAL_MACRO ( Asmx86IDiv ,"Asmx86IDiv","Asmx86IDivTag" );
     NEW_TERMINAL_MACRO ( Asmx86Inc , "Asmx86Inc", "Asmx86IncTag" );
     NEW_TERMINAL_MACRO ( Asmx86Dec , "Asmx86Dec", "Asmx86DecTag" );
     NEW_TERMINAL_MACRO ( Asmx86Neg , "Asmx86Neg", "Asmx86NegTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cmp , "Asmx86Cmp", "Asmx86CmpTag" );

     // decimal arithmetic instructions
     NEW_TERMINAL_MACRO ( Asmx86Daa , "Asmx86Daa", "Asmx86DaaTag" );
     NEW_TERMINAL_MACRO ( Asmx86Das , "Asmx86Das", "Asmx86DasTag" );
     NEW_TERMINAL_MACRO ( Asmx86Aaa , "Asmx86Aaa", "Asmx86AaaTag" );
     NEW_TERMINAL_MACRO ( Asmx86Aas , "Asmx86Aas", "Asmx86AasTag" );
     NEW_TERMINAL_MACRO ( Asmx86Aam , "Asmx86Aam", "Asmx86AamTag" );
     NEW_TERMINAL_MACRO ( Asmx86Aad , "Asmx86Aad", "Asmx86AadTag" );

     // Logical Instructions
     NEW_TERMINAL_MACRO ( Asmx86And , "Asmx86And", "Asmx86AndTag" );
     NEW_TERMINAL_MACRO ( Asmx86Or ,  "Asmx86Or", "Asmx86OrTag" );
     NEW_TERMINAL_MACRO ( Asmx86Xor , "Asmx86Xor", "Asmx86XorTag" );
     NEW_TERMINAL_MACRO ( Asmx86Not , "Asmx86Not", "Asmx86NotTag" );

     // Shift and Rotate Instructions
     NEW_TERMINAL_MACRO ( Asmx86Shl , "Asmx86Shl", "Asmx86ShlTag" );
     NEW_TERMINAL_MACRO ( Asmx86Shr , "Asmx86Shr", "Asmx86ShrTag" );
     NEW_TERMINAL_MACRO ( Asmx86Sar , "Asmx86Sar", "Asmx86SarTag" );
     NEW_TERMINAL_MACRO ( Asmx86Shrd ,"Asmx86Shrd","Asmx86ShrdTag" );
     NEW_TERMINAL_MACRO ( Asmx86Shld ,"Asmx86Shld","Asmx86ShldTag" );
     NEW_TERMINAL_MACRO ( Asmx86Ror , "Asmx86Ror", "Asmx86RorTag" );
     NEW_TERMINAL_MACRO ( Asmx86Rol , "Asmx86Rol", "Asmx86RolTag" );
     NEW_TERMINAL_MACRO ( Asmx86Rcr , "Asmx86Rcr", "Asmx86RcrTag" );
     NEW_TERMINAL_MACRO ( Asmx86Rcl , "Asmx86Rcl", "Asmx86RclTag" );

     // Bit and Byte Instructions
     NEW_TERMINAL_MACRO ( Asmx86Bt ,   "Asmx86Bt",    "Asmx86BtTag" );
     NEW_TERMINAL_MACRO ( Asmx86Bts ,  "Asmx86Bts",   "Asmx86BtsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Btr ,  "Asmx86Btr",   "Asmx86BtrTag" );
     NEW_TERMINAL_MACRO ( Asmx86Btc ,  "Asmx86Btc",   "Asmx86BtcTag" );
     NEW_TERMINAL_MACRO ( Asmx86Bsf ,  "Asmx86Bsf",   "Asmx86BsfTag" );
     NEW_TERMINAL_MACRO ( Asmx86Bsr ,  "Asmx86Bsr",   "Asmx86BsrTag" );
     NEW_TERMINAL_MACRO ( Asmx86Sete , "Asmx86Sete",  "Asmx86SeteTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setne, "Asmx86Setne", "Asmx86SetneTag" );
     NEW_TERMINAL_MACRO ( Asmx86Seta , "Asmx86Seta",  "Asmx86SetaTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setae, "Asmx86Setae", "Asmx86SetaeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setb , "Asmx86Setb",  "Asmx86SetbTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setbe, "Asmx86Setbe", "Asmx86SetbeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setg , "Asmx86Setg",  "Asmx86SetgTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setge, "Asmx86Setge", "Asmx86SetgeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setl , "Asmx86Setl",  "Asmx86SetlTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setle, "Asmx86Setle", "Asmx86SetleTag" );
     NEW_TERMINAL_MACRO ( Asmx86Sets , "Asmx86Sets",  "Asmx86SetsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setns, "Asmx86Setns", "Asmx86SetnsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Seto , "Asmx86Seto",  "Asmx86SetoTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setno, "Asmx86Setno", "Asmx86SetnoTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setpe, "Asmx86Setpe", "Asmx86SetpeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Setpo, "Asmx86Setpo", "Asmx86SetpoTag" );
     NEW_TERMINAL_MACRO ( Asmx86Test , "Asmx86Test",  "Asmx86TestTag" );

     // Control Transfer Instructions
     NEW_TERMINAL_MACRO ( Asmx86Ret ,       "Asmx86Ret",      "Asmx86RetTag" );
     NEW_TERMINAL_MACRO ( Asmx86Leave ,     "Asmx86Leave",    "Asmx86LeaveTag" );
     NEW_TERMINAL_MACRO ( Asmx86Call ,      "Asmx86Call",     "Asmx86CallTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jmp ,       "Asmx86Jmp",      "Asmx86JmpTag" );
     NEW_TERMINAL_MACRO ( Asmx86Je ,        "Asmx86Je",       "Asmx86JeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jne ,       "Asmx86Jne",      "Asmx86JneTag" );
     NEW_TERMINAL_MACRO ( Asmx86Ja ,        "Asmx86Ja",       "Asmx86JaTag" );

     NEW_TERMINAL_MACRO ( Asmx86Jae ,       "Asmx86Jae",      "Asmx86JaeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jb ,        "Asmx86Jb",       "Asmx86JbTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jbe ,       "Asmx86Jbe",      "Asmx86JbeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jg ,        "Asmx86Jg",       "Asmx86JgTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jge ,       "Asmx86Jge",      "Asmx86JgeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jl ,        "Asmx86Jl",       "Asmx86JlTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jle ,       "Asmx86Jle",      "Asmx86JleTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jc ,        "Asmx86Jc",       "Asmx86JcTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jnc ,       "Asmx86Jnc",      "Asmx86JncTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jo ,        "Asmx86Jo",       "Asmx86JoTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jno ,       "Asmx86Jno",      "Asmx86JnoTag" );
     NEW_TERMINAL_MACRO ( Asmx86Js ,        "Asmx86Js",       "Asmx86JsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jns ,       "Asmx86Jns",      "Asmx86JnsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jpo ,       "Asmx86Jpo",      "Asmx86JpoTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jpe ,       "Asmx86Jpe",      "Asmx86JpeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Jcxz ,      "Asmx86Jcxz",     "Asmx86JcxzTag" );

     NEW_TERMINAL_MACRO ( Asmx86Loop ,      "Asmx86Loop",     "Asmx86LoopTag" );
     NEW_TERMINAL_MACRO ( Asmx86Loopz ,     "Asmx86Loopz",    "Asmx86LoopzTag" );
     NEW_TERMINAL_MACRO ( Asmx86Loopnz ,    "Asmx86Loopnz",   "Asmx86LoopnzTag" );
     NEW_TERMINAL_MACRO ( Asmx86IRet ,      "Asmx86IRet",     "Asmx86IRetTag" );
     NEW_TERMINAL_MACRO ( Asmx86Int ,       "Asmx86Int",      "Asmx86IntTag" );
     NEW_TERMINAL_MACRO ( Asmx86Into ,      "Asmx86Into",     "Asmx86IntoTag" );
     NEW_TERMINAL_MACRO ( Asmx86Bound ,     "Asmx86Bound",    "Asmx86BoundTag" );
     NEW_TERMINAL_MACRO ( Asmx86Enter ,     "Asmx86Enter",    "Asmx86EnterTag" );

     // Asmx86 String Instructions
     NEW_TERMINAL_MACRO ( Asmx86Movs ,     "Asmx86Movs",    "Asmx86MovsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cmps ,     "Asmx86Cmps",    "Asmx86CmpsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Scas ,     "Asmx86Scas",    "Asmx86ScasTag" );
     NEW_TERMINAL_MACRO ( Asmx86Lods ,     "Asmx86Lods",    "Asmx86LodsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Stos ,     "Asmx86Stos",    "Asmx86StosTag" );
     NEW_TERMINAL_MACRO ( Asmx86Rep ,      "Asmx86Rep",     "Asmx86RepTag" );
     NEW_TERMINAL_MACRO ( Asmx86Repe ,     "Asmx86Repe",    "Asmx86RepeTag" );
     NEW_TERMINAL_MACRO ( Asmx86Repne ,    "Asmx86Repne",   "Asmx86RepneTag" );

     // I/O Instructions
     NEW_TERMINAL_MACRO ( Asmx86In ,       "Asmx86In",      "Asmx86InTag" );
     NEW_TERMINAL_MACRO ( Asmx86Out ,      "Asmx86Out",     "Asmx86OutTag" );
     NEW_TERMINAL_MACRO ( Asmx86Ins ,      "Asmx86Ins",     "Asmx86InsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Outs ,     "Asmx86Outs",    "Asmx86OutsTag" );

     // Misc Instructions
     NEW_TERMINAL_MACRO ( Asmx86Lea ,      "Asmx86Lea",     "Asmx86LeaTag" );
     NEW_TERMINAL_MACRO ( Asmx86Nop ,      "Asmx86Nop",     "Asmx86NopTag" );
     NEW_TERMINAL_MACRO ( Asmx86Ud2 ,      "Asmx86Ud2",     "Asmx86Ud2Tag" );
     NEW_TERMINAL_MACRO ( Asmx86Xlat ,     "Asmx86Xlat",    "Asmx86XlatTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cpuid ,    "Asmx86Cpuid",   "Asmx86CpuidTag" );
     NEW_TERMINAL_MACRO ( Asmx86Rdtsc ,    "Asmx86Rdtsc",   "Asmx86RdtscTag" );

     // Flag Control Instructions
     NEW_TERMINAL_MACRO ( Asmx86Stc ,      "Asmx86Stc",     "Asmx86StcTag" );
     NEW_TERMINAL_MACRO ( Asmx86Clc ,      "Asmx86Clc",     "Asmx86ClcTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cmc ,      "Asmx86Cmc",     "Asmx86CmcTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cld ,      "Asmx86Cld",     "Asmx86CldTag" );
     NEW_TERMINAL_MACRO ( Asmx86Std ,      "Asmx86Std",     "Asmx86StdTag" );
     NEW_TERMINAL_MACRO ( Asmx86Lahf ,     "Asmx86Lahf",    "Asmx86LahfTag" );
     NEW_TERMINAL_MACRO ( Asmx86Sahf ,     "Asmx86Sahf",    "Asmx86SahfTag" );
     NEW_TERMINAL_MACRO ( Asmx86Pushf ,    "Asmx86Pushf",   "Asmx86PushfTag" );
     NEW_TERMINAL_MACRO ( Asmx86Popf ,     "Asmx86Popf",    "Asmx86PopfTag" );
     NEW_TERMINAL_MACRO ( Asmx86Sti ,      "Asmx86Sti",     "Asmx86StiTag" );
     NEW_TERMINAL_MACRO ( Asmx86Cli ,      "Asmx86Cli",     "Asmx86CliTag" );

     // Segment Register Instructions
     NEW_TERMINAL_MACRO ( Asmx86Lds ,      "Asmx86Lds",     "Asmx86LdsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Les ,      "Asmx86Les",     "Asmx86LesTag" );
     NEW_TERMINAL_MACRO ( Asmx86Lfs ,      "Asmx86Lfs",     "Asmx86LfsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Lgs  ,     "Asmx86Lgs",     "Asmx86LgsTag" );
     NEW_TERMINAL_MACRO ( Asmx86Lss ,      "Asmx86Lss",     "Asmx86LssTag" );

     NEW_TERMINAL_MACRO ( Asmx86Esc ,      "Asmx86Esc",     "Asmx86EscTag" );
     NEW_TERMINAL_MACRO ( Asmx86Lock ,     "Asmx86Lock",    "Asmx86LockTag" );
     NEW_TERMINAL_MACRO ( Asmx86Wait ,     "Asmx86Wait",    "Asmx86WaitTag" );
     NEW_TERMINAL_MACRO ( Asmx86Hlt ,      "Asmx86Hlt",     "Asmx86HltTag" );


     NEW_TERMINAL_MACRO ( AsmBlock        , "AsmBlock",        "AsmBlockTag" );
     NEW_TERMINAL_MACRO ( AsmOperandList  , "AsmOperandList",  "AsmOperandListTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86ConditionalFlagStringInstruction, 
			     Asmx86Repe | Asmx86Repne | Asmx86Rep
			     , "Asmx86ConditionalFlagStringInstruction", "Asmx86ConditionalFlagStringInstructionTag" );


     NEW_NONTERMINAL_MACRO ( Asmx86StringInstruction, Asmx86ConditionalFlagStringInstruction |
			     Asmx86Movs | Asmx86Cmps | Asmx86Scas | Asmx86Lods |
			     Asmx86Stos | Asmx86Repe | Asmx86Repne
			     , "Asmx86StringInstruction", "Asmx86StringInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86FlagControlInstruction, 
			     Asmx86Stc | Asmx86Clc | Asmx86Cmc | Asmx86Cld | Asmx86Std |
			     Asmx86Lahf | Asmx86Sahf | Asmx86Pushf | Asmx86Popf | 
			     Asmx86Sti | Asmx86Cli
			     , "Asmx86FlagControlInstruction", "Asmx86FlagControlInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86SegmentRegisterInstruction,
			     Asmx86Lds | Asmx86Les | Asmx86Lfs | Asmx86Lgs | Asmx86Lss
			     , "Asmx86SegmentRegisterInstruction", "Asmx86SegmentRegisterInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86IOInstruction, 
			     Asmx86In | Asmx86Out | Asmx86Ins | Asmx86Outs
			     , "Asmx86IOInstruction", "Asmx86IOInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86ConditionalFlagDataTransferInstruction, 
                             Asmx86CMovc | Asmx86CMovnc | Asmx86CMovo | Asmx86CMovno | 
                             Asmx86CMovs | Asmx86CMovns | Asmx86CMovp | Asmx86CMovnp |
                             Asmx86CMove | Asmx86CMovne | Asmx86CMova | Asmx86CMovae | Asmx86CMovb | Asmx86CMovbe | Asmx86CMovg | 
                             Asmx86CMovge | Asmx86CMovl | Asmx86CMovle 
			     , "Asmx86ConditionalFlagDataTransferInstruction", "Asmx86ConditionalFlagDataTransferInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86ConditionalDataTransferInstruction,  Asmx86Cmpxchg | Asmx86Cmpxchg8b 
			     , "Asmx86ConditionalDataTransferInstruction", "Asmx86ConditionalDataTransferInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86ArithmeticInstruction, 
                             Asmx86Sub | Asmx86Add | Asmx86Inc |
			     Asmx86Adc | Asmx86Sbb | Asmx86Mul | Asmx86IMul | Asmx86Div | Asmx86IDiv | 
			     Asmx86Dec | Asmx86Neg | Asmx86Cmp |
			     Asmx86Daa | Asmx86Das | Asmx86Aaa | 
			     Asmx86Aas | Asmx86Aam | Asmx86Aad  , "Asmx86ArithmeticInstruction", "Asmx86ArithmeticInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86DataTransferInstruction, Asmx86Push | Asmx86Mov | Asmx86Pop | 
			     Asmx86ConditionalDataTransferInstruction | Asmx86ConditionalFlagDataTransferInstruction | 
			     Asmx86ArithmeticInstruction | 
			     Asmx86Xchg | Asmx86Bswap | Asmx86Xadd | 
			     Asmx86Pusha | Asmx86Popa | Asmx86Cwd | Asmx86Cbw | Asmx86Movsx | Asmx86Movzx 
			     , "Asmx86DataTransferInstruction", "Asmx86DataTransferInstructionTag" );


     NEW_NONTERMINAL_MACRO ( Asmx86LogicalInstruction, 
			     Asmx86And | Asmx86Or | Asmx86Xor | Asmx86Not 
			     , "Asmx86LogicalInstruction", "Asmx86LogicalInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86MiscInstruction, 
			     Asmx86Lea | Asmx86Nop | Asmx86Ud2 | Asmx86Xlat | Asmx86Cpuid | Asmx86Rdtsc | Asmx86Esc | Asmx86Lock | Asmx86Wait | Asmx86Hlt 
			     , "Asmx86MiscInstruction", "Asmx86MiscInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86ShiftAndRotateInstruction, 
                             Asmx86Shl | Asmx86Shr |
			     Asmx86Sar | Asmx86Shrd | Asmx86Shld | Asmx86Ror | Asmx86Rol | Asmx86Rcr |
			     Asmx86Rcl , "Asmx86ShiftAndRotateInstruction", "Asmx86ShiftAndRotateInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86ConditionalFlagBitAndByteInstruction, 
			     Asmx86Sete | Asmx86Setne | Asmx86Seta | Asmx86Setae | Asmx86Setb | Asmx86Setbe |
			     Asmx86Setg | Asmx86Setge | Asmx86Setl | Asmx86Setle | Asmx86Sets | Asmx86Setns |
			     Asmx86Seto | Asmx86Setno | Asmx86Setpe | Asmx86Setpo | Asmx86Test , "Asmx86ConditionalFlagBitAndByteInstruction", "Asmx86ConditionalFlagBitAndByteInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86BitAndByteInstruction, Asmx86ConditionalFlagBitAndByteInstruction | 
			     Asmx86Bt | Asmx86Bts | Asmx86Btr | Asmx86Btc | Asmx86Bsf | Asmx86Bsr 
			     , "Asmx86BitAndByteInstruction", "Asmx86BitAndByteInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86ConditionalFlagControlTransferInstruction, 
			     Asmx86Je | Asmx86Jne | Asmx86Ja | Asmx86Jae |
			     Asmx86Jb | Asmx86Jbe | Asmx86Jg | Asmx86Jge | Asmx86Jl |
			     Asmx86Jle | Asmx86Jc | Asmx86Jnc | Asmx86Jo | Asmx86Jno |
			     Asmx86Js | Asmx86Jns | Asmx86Jpo | Asmx86Jpe | Asmx86Jcxz |
			     Asmx86Loopz | Asmx86Loopnz |  Asmx86Into | Asmx86Bound 
			     , "Asmx86ConditionalFlagControlTransferInstruction", "Asmx86ConditionalFlagControlTransferInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86ConditionalControlTransferInstruction, 
			     Asmx86Loop   , "Asmx86ConditionalControlTransferInstruction", "Asmx86ConditionalControlTransferInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86UnConditionalControlTransferInstruction, 
			     Asmx86Call | Asmx86Jmp | Asmx86Ret | Asmx86IRet | Asmx86Int | Asmx86Enter | Asmx86Leave 
			     , "Asmx86UnConditionalControlTransferInstruction", "Asmx86UnConditionalControlTransferInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86ControlTransferInstruction, 
			     Asmx86ConditionalFlagControlTransferInstruction | Asmx86ConditionalControlTransferInstruction | 
			     Asmx86UnConditionalControlTransferInstruction 
			      , "Asmx86ControlTransferInstruction", "Asmx86ControlTransferInstructionTag" );

     NEW_NONTERMINAL_MACRO ( AsmArmInstruction, AsmArmControlTransferInstruction //| AsmArmArithmeticInstruction  
			     | AsmArmDataTransferInstruction 
			     | AsmArmLogicalInstruction | AsmArmMiscInstruction
			     , "AsmArmInstruction", "AsmArmInstructionTag" );

     NEW_NONTERMINAL_MACRO ( Asmx86Instruction, Asmx86ControlTransferInstruction | //Asmx86ArithmeticInstruction | 
			     Asmx86DataTransferInstruction | 
			     Asmx86LogicalInstruction | Asmx86ShiftAndRotateInstruction | Asmx86BitAndByteInstruction |
			     Asmx86StringInstruction | Asmx86IOInstruction | Asmx86MiscInstruction |
			     Asmx86FlagControlInstruction | Asmx86SegmentRegisterInstruction  
			     , "Asmx86Instruction", "Asmx86InstructionTag" );

  // This is currently a AsmFunctionCall plus other unspecified uses of Asmx86Instruction, it may be refied later.
     NEW_NONTERMINAL_MACRO ( AsmInstruction, Asmx86Instruction | AsmArmInstruction
			     , "AsmInstruction", "AsmInstructionTag" );


     NEW_TERMINAL_MACRO ( AsmDataStructureDeclaration , "AsmDataStructureDeclaration", "AsmDataStructureDeclarationTag" );
     NEW_TERMINAL_MACRO ( AsmFunctionDeclaration      , "AsmFunctionDeclaration",      "AsmFunctionDeclarationTag" );
     NEW_TERMINAL_MACRO ( AsmFieldDeclaration         , "AsmFieldDeclaration",         "AsmFieldDeclarationTag" );

     NEW_NONTERMINAL_MACRO ( AsmDeclaration, AsmDataStructureDeclaration | AsmFunctionDeclaration | 
                             AsmFieldDeclaration, "AsmDeclaration", "AsmDeclarationTag" );

     NEW_NONTERMINAL_MACRO ( AsmStatement, AsmDeclaration | AsmBlock | AsmInstruction, "AsmStatement", "AsmStatementTag" );

  // DQ (3/30/2007): Fixed up the names where were "AsmUnaryAdd"
     NEW_TERMINAL_MACRO ( AsmBinaryAdd    ,  "AsmBinaryAdd",  "AsmBinaryAddTag" );
     NEW_TERMINAL_MACRO ( AsmBinarySubtract, "AsmBinarySubtract", "AsmBinarySubtractTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryMultiply, "AsmBinaryMultiply", "AsmBinaryMultiplyTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryDivide,   "AsmBinaryDivide", "AsmBinaryDivideTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryMod,      "AsmBinaryMod", "AsmBinaryModTag" );

     NEW_NONTERMINAL_MACRO ( AsmBinaryExpression, AsmBinaryAdd | AsmBinarySubtract | AsmBinaryMultiply | 
                             AsmBinaryDivide | AsmBinaryMod, "AsmBinaryExpression", "AsmBinaryExpressionTag" );

     NEW_TERMINAL_MACRO ( AsmUnaryPlus , "AsmUnaryPlus",  "AsmUnaryPlusTag" );
     NEW_TERMINAL_MACRO ( AsmUnaryMinus, "AsmUnaryMinus", "AsmUnaryMinusTag" );

     NEW_NONTERMINAL_MACRO ( AsmUnaryExpression, AsmUnaryPlus | AsmUnaryMinus, "AsmUnaryExpression", "AsmUnaryExpressionTag" );

     NEW_TERMINAL_MACRO ( AsmMemoryReferenceExpression   , "AsmMemoryReferenceExpression",   "AsmMemoryReferenceExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmControlFlagsExpression      , "AsmControlFlagsExpression",      "AsmControlFlagsExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmCommonSubExpression         , "AsmCommonSubExpression",         "AsmCommonSubExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmRegisterReferenceExpression , "AsmRegisterReferenceExpression", "AsmRegisterReferenceExpressionTag" );

     //     NEW_TERMINAL_MACRO ( AsmArmRegisterReferenceExpression , "AsmArmRegisterReferenceExpression", "AsmArmRegisterReferenceExpressionTag" );

     NEW_TERMINAL_MACRO ( AsmByteValueExpression        , "AsmByteValueExpression",        "AsmByteValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmWordValueExpression        , "AsmWordValueExpression",        "AsmWordValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmDoubleWordValueExpression  , "AsmDoubleWordValueExpression",  "AsmDoubleWordValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmQuadWordValueExpression    , "AsmQuadWordValueExpression",    "AsmQuadWordValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmSingleFloatValueExpression , "AsmSingleFloatValueExpression", "AsmSingleFloatValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmDoubleFloatValueExpression , "AsmDoubleFloatValueExpression", "AsmDoubleFloatValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmVectorValueExpression      , "AsmVectorValueExpression",      "AsmVectorValueExpressionTag" );

     NEW_NONTERMINAL_MACRO ( AsmValueExpression, AsmByteValueExpression | AsmWordValueExpression | 
                             AsmDoubleWordValueExpression | AsmQuadWordValueExpression | 
                             AsmSingleFloatValueExpression | AsmDoubleFloatValueExpression | 
                             AsmVectorValueExpression, "AsmValueExpression", "AsmValueExpressionTag" );

     NEW_NONTERMINAL_MACRO ( AsmExpression, AsmValueExpression | AsmBinaryExpression | AsmUnaryExpression | 
                             AsmMemoryReferenceExpression | AsmRegisterReferenceExpression | AsmControlFlagsExpression | 
                             AsmCommonSubExpression 
			     //| AsmArmRegisterReferenceExpression
			     , "AsmExpression", "AsmExpressionTag" );

  // These are attached to the SgAsmNode object (the root for the IR used for binaries)
     NEW_TERMINAL_MACRO ( AsmTypeByte        , "AsmTypeByte",        "AsmTypeByteTag" );
     NEW_TERMINAL_MACRO ( AsmTypeWord        , "AsmTypeWord",        "AsmTypeWordTag" );
     NEW_TERMINAL_MACRO ( AsmTypeDoubleWord  , "AsmTypeDoubleWord",  "AsmTypeDoubleWordTag" );
     NEW_TERMINAL_MACRO ( AsmTypeQuadWord    , "AsmTypeQuadWord",    "AsmTypeQuadWordTag" );
     NEW_TERMINAL_MACRO ( AsmTypeSingleFloat , "AsmTypeSingleFloat", "AsmTypeSingleFloatTag" );
     NEW_TERMINAL_MACRO ( AsmTypeDoubleFloat , "AsmTypeDoubleFloat", "AsmTypeDoubleFloatTag" );
     NEW_TERMINAL_MACRO ( AsmTypeVector      , "AsmTypeVector",      "AsmTypeVectorTag" );

     NEW_NONTERMINAL_MACRO ( AsmType, AsmTypeByte | AsmTypeWord | AsmTypeDoubleWord | AsmTypeQuadWord | 
                             AsmTypeSingleFloat | AsmTypeDoubleFloat | AsmTypeVector, "AsmType", "AsmTypeTag" );

     NEW_TERMINAL_MACRO ( AsmFile                     , "AsmFile",                     "AsmFileTag" );

  // DQ (1/6/2008): Added ELF program header and section header support to AST.
     NEW_TERMINAL_MACRO ( AsmProgramHeader, "AsmProgramHeader", "AsmProgramHeaderTag" );
     NEW_TERMINAL_MACRO ( AsmSectionHeader, "AsmSectionHeader", "AsmSectionHeaderTag" );

  // DQ (1/6/2008): These store the lists of AsmProgramHeader and AsmSectionHeader objects.
  // AsmFile can not have traversed data members and lists, so this give it two additional data members.
     NEW_TERMINAL_MACRO ( AsmProgramHeaderList, "AsmProgramHeaderList", "AsmProgramHeaderListTag" );
     NEW_TERMINAL_MACRO ( AsmSectionHeaderList, "AsmSectionHeaderList", "AsmSectionHeaderListTag" );

  // NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmOperandList | AsmType, "AsmNode","AsmNodeTag");
     NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmProgramHeader | AsmSectionHeader | AsmProgramHeaderList | AsmSectionHeaderList | AsmOperandList | AsmType, "AsmNode","AsmNodeTag");


  // DQ (3/15/2007): Added support forbinaries (along lines of suggestions by Thomas Dullien)
  // AsmInstructionBase.setFunctionPrototype        ( "HEADER", "../Grammar/Common.code");
     AsmNode.setFunctionPrototype        ( "HEADER_BINARY", "../Grammar/BinaryInstruction.code");

  // Asm instructions should be able to have attached attributes like other IR nodes in ROSE
     AsmNode.setDataPrototype     ( "AttachedPreprocessingInfoType*", "attachedPreprocessingInfoPtr", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, COPY_DATA);

     AsmStatement.setDataPrototype("unsigned int","address","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     //AsmStatement.setDataPrototype("SgAsmNode*","parent","= NULL",
     //                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);

  // Most platform and non-platform dependent machine instructions will map to this IR node
     AsmInstruction.setFunctionPrototype        ( "HEADER_BINARY_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     AsmInstruction.setDataPrototype("std::string","mnemonic","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
  // AsmInstruction.setDataPrototype("long","basic_block_id","= -1",
  //                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("std::string","raw_bytes","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("std::string","comment","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("SgAsmOperandList*","operandList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("SgAsmStatementPtrList","sources","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     Asmx86ControlTransferInstruction.setFunctionPrototype  ( "HEADER_BINARY_CONTROLTRANSFER_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     Asmx86ControlTransferInstruction.setDataPrototype("SgAsmInstruction*","destination","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);

     Asmx86Ret.setFunctionPrototype  ( "HEADER_BINARY_RET", "../Grammar/BinaryInstruction.code");
     Asmx86Ret.setDataPrototype("SgAsmStatementPtrList","dest","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Representation of function call
     Asmx86Call.setFunctionPrototype       ( "HEADER_BINARY_FUNCTION_CALL", "../Grammar/BinaryInstruction.code");
  // added by tps on 4Apr07
     Asmx86Call.setDataPrototype("unsigned int","destinationAddress","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);



  // Block of instructions (helps define depth to the AST)
     AsmBlock.setFunctionPrototype              ( "HEADER_BINARY_BLOCK", "../Grammar/BinaryInstruction.code");
     // added by tps, 05Apr07 ... need this for the control_flow_graph
     AsmBlock.setDataPrototype("unsigned int","next_block_true_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("unsigned int","next_block_false_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("unsigned int","id","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);

     AsmOperandList.setFunctionPrototype        ( "HEADER_BINARY_OPERAND_LIST", "../Grammar/BinaryInstruction.code");
     AsmOperandList.setDataPrototype("SgAsmExpressionPtrList","operands","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmBlock.setDataPrototype("SgAsmStatementPtrList","statementList","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmFile.setFunctionPrototype ( "HEADER_BINARY_FILE", "../Grammar/BinaryInstruction.code");
     AsmFile.setDataPrototype("std::string","name","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmBlock*","global_block","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // AsmFile.setDataPrototype("std::vector<unsigned int>", "magic_number_vector","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // AsmFile.setDataPrototype("SgAsmFile::magic_number_vector_type", "magic_number_vector","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("std::string", "magic_number_string","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmFile.setDataPrototype("SgAsmFile::elf_class_kind_enum", "binary_class_type","= SgAsmFile::e_class_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_data_encoding_enum", "data_encoding","= SgAsmFile::e_data_encoding_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_version_enum", "version","= SgAsmFile::e_version_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("int", "magicNumberPaddingStartIndex","= -1",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_OS_ABI_identification_enum", "operating_system","= SgAsmFile::e_OS_ABI_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_object_file_type_enum", "object_file_type","= SgAsmFile::e_file_type_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_machine_architecture_enum", "machine_architecture","= SgAsmFile::e_machine_architecture_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "associated_entry_point","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "program_header_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "section_header_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "processor_specific_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "elf_header_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "program_header_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "program_header_entry_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "number_of_program_headers","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "section_header_entry_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "number_of_section_headers","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "section_header_string_table_index","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmProgramHeaderList*", "programHeaderList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmSectionHeaderList*", "sectionHeaderList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmSectionHeaderList.setFunctionPrototype ( "HEADER_BINARY_FILE_SECTION_HEADER_LIST", "../Grammar/BinaryInstruction.code");
#if 1
     AsmSectionHeaderList.setDataPrototype("SgAsmSectionHeaderPtrList","section_headers","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
     AsmSectionHeaderList.setAutomaticGenerationOfConstructor(FALSE);
     AsmSectionHeaderList.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_DECLARATIONS", "../Grammar/Statement.code" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_DATA_TYPE", "SgAsmSectionHeaderPtrList" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_NAME", "section_headers" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_FUNCTION_RETURN_TYPE", "void" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_FUNCTION_NAME", "section_header" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
     AsmSectionHeaderList.setDataPrototype    ( "SgAsmSectionHeaderPtrList", "section_headers", "",
				      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

     AsmProgramHeaderList.setFunctionPrototype ( "HEADER_BINARY_FILE_PROGRAM_HEADER_LIST", "../Grammar/BinaryInstruction.code");
     AsmProgramHeaderList.setDataPrototype("SgAsmProgramHeaderPtrList","program_headers","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmProgramHeader.setFunctionPrototype ( "HEADER_BINARY_FILE_PROGRAM_HEADER", "../Grammar/BinaryInstruction.code");
  // AsmProgramHeader.setDataPrototype("std::string","name","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // AsmProgramHeader.setDataPrototype("unsigned long","name_string_index","= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","starting_file_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","starting_virtual_memory_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","starting_physical_memory_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","file_image_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","memory_image_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","segment_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","alignment","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmSectionHeader.setFunctionPrototype ( "HEADER_BINARY_FILE_SECTION_HEADER", "../Grammar/BinaryInstruction.code");
     AsmSectionHeader.setDataPrototype("std::string","name","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","name_string_index","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","starting_memory_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","starting_file_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","table_index_link","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","info","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","address_alignment","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","table_entry_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // There are several sorts of declarations within a binary
     AsmDeclaration.setFunctionPrototype        ( "HEADER_BINARY_DECLARATION", "../Grammar/BinaryInstruction.code");

     AsmDataStructureDeclaration.setFunctionPrototype ( "HEADER_BINARY_DATA_STRUCTURE", "../Grammar/BinaryInstruction.code");
  // DQ (3/15/2007): I can't seem to get this to compile so I will leave it out for now!
  // Binaries have some easily resolved data structures so we use this to represent these
  // AsmDataStructureDeclaration.setDataPrototype("std::list<SgAsmDeclaration*>","declarationList","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);

  // Binaries have many easily resolved functions so we use this to represent these
     AsmFunctionDeclaration.setFunctionPrototype        ( "HEADER_BINARY_FUNCTION_DECLARATION", "../Grammar/BinaryInstruction.code");
     AsmFunctionDeclaration.setDataPrototype("std::string","name","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmFunctionDeclaration::function_kind_enum","function_kind","= SgAsmFunctionDeclaration::e_unknown",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
  // AsmFunctionDeclaration.setDataPrototype("int","function_kind","= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  //   AsmFunctionDeclaration.setDataPrototype("SgAsmBlockPtrList","body","",
  //                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("std::string","name_md5","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmStatementPtrList","statementList","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmStatementPtrList","dest","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // These are used as data members in AsmDataStructureDeclaration
     AsmFieldDeclaration.setDataPrototype("std::string","name","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Not clear if we want to store the offset explicitly
     AsmFieldDeclaration.setDataPrototype("unsigned long","offset","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmByteValueExpression.setDataPrototype("unsigned short","value","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmWordValueExpression.setDataPrototype("unsigned int","value","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDoubleWordValueExpression.setDataPrototype("unsigned long","value","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // This should maybe be "unsigned long long"
     AsmQuadWordValueExpression.setDataPrototype("unsigned long","value","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSingleFloatValueExpression.setDataPrototype("float","value","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDoubleFloatValueExpression.setDataPrototype("double","value","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmVectorValueExpression.setDataPrototype("unsigned int","size","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // For now we will let this be a SgType, but we have to figure how what the binary types will go in the IR.
  // Also we don't traverse types (they are considered special attributes to the IR nodes that define the AST).
     AsmVectorValueExpression.setDataPrototype("SgAsmType*","type","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Store the expression tree from any constant folding (this can be ignored until later)
     AsmValueExpression.setDataPrototype("SgAsmValueExpression*","unfolded_expression_tree","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // added by tps on 5Apr07
     AsmExpression.setDataPrototype("std::string","replacement","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmBinaryExpression.setDataPrototype("SgAsmExpression*","lhs","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmBinaryExpression.setDataPrototype("SgAsmExpression*","rhs","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmUnaryExpression.setDataPrototype("SgAsmExpression*","operand","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*","address","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     // added by tps on 16Jan08
     AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*","segment","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmRegisterReferenceExpression.setFunctionPrototype ( "HEADER_BINARY_REGISTER_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmRegisterReferenceExpression.setDataPrototype("SgAsmRegisterReferenceExpression::x86_register_enum","x86_register_code","= SgAsmRegisterReferenceExpression::undefined_general_register",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     
     AsmRegisterReferenceExpression.setDataPrototype("SgAsmRegisterReferenceExpression::arm_register_enum","arm_register_code","= SgAsmRegisterReferenceExpression::undefined_arm_register",
                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     
     AsmRegisterReferenceExpression.setDataPrototype("SgAsmRegisterReferenceExpression::x86_position_in_register_enum","x86_position_in_register_code","= SgAsmRegisterReferenceExpression::undefined_position_in_register",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmRegisterReferenceExpression.setDataPrototype("SgAsmRegisterReferenceExpression::arm_position_in_register_enum","arm_position_in_register_code","= SgAsmRegisterReferenceExpression::undefined_arm_position_in_register",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // added by tps on 3Apr07 and removed on 16Jan08
     //     AsmRegisterReferenceExpression.setDataPrototype("SgAsmExpression*","offset","= NULL",
     //                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


  // This might better be an STL std::vector<bool> type.
     AsmControlFlagsExpression.setDataPrototype("unsigned long","bit_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Not sure what to do with this, but if it is in the database we should build them
     AsmCommonSubExpression.setDataPrototype("SgAsmExpression*","subexpression","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Attach source code to the IR nodes (taken from ../Grammar/BinaryInstruction.code)
     AsmType.setFunctionPrototype            ( "HEADER_BINARY_TYPE", "../Grammar/BinaryInstruction.code");
     AsmTypeByte.setFunctionPrototype        ( "HEADER_BINARY_TYPE_BYTE", "../Grammar/BinaryInstruction.code");
     AsmTypeWord.setFunctionPrototype        ( "HEADER_BINARY_TYPE_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeDoubleWord.setFunctionPrototype  ( "HEADER_BINARY_TYPE_DOUBLE_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeQuadWord.setFunctionPrototype    ( "HEADER_BINARY_TYPE_QUAD_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeSingleFloat.setFunctionPrototype ( "HEADER_BINARY_TYPE_SINGLE_FLOAT", "../Grammar/BinaryInstruction.code");
     AsmTypeDoubleFloat.setFunctionPrototype ( "HEADER_BINARY_TYPE_DOUBLE_FLOAT", "../Grammar/BinaryInstruction.code");
     AsmTypeVector.setFunctionPrototype      ( "HEADER_BINARY_TYPE_VECTOR", "../Grammar/BinaryInstruction.code");




     AsmNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmNode.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     AsmNode.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");





  // ***********************************************************************
  // ***********************************************************************
  //                       Source Code Definition
  // ***********************************************************************
  // ***********************************************************************

     AsmBlock.setFunctionSource                    ( "SOURCE_BINARY_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmOperandList.setFunctionSource              ( "SOURCE_BINARY_OPERAND_LIST", "../Grammar/BinaryInstruction.code");
     AsmDataStructureDeclaration.setFunctionSource ( "SOURCE_BINARY_DATA_STRUCTURE", "../Grammar/BinaryInstruction.code");
     AsmInstruction.setFunctionSource              ( "SOURCE_BINARY_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     AsmFunctionDeclaration.setFunctionSource      ( "SOURCE_BINARY_FUNCTION_DECLARATION", "../Grammar/BinaryInstruction.code");
     AsmNode.setFunctionSource                     ( "SOURCE_BINARY_NODE", "../Grammar/BinaryInstruction.code");
     Asmx86ControlTransferInstruction.setFunctionSource  ( "SOURCE_BINARY_CONTROLTRANSFER_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     Asmx86Ret.setFunctionSource                     ( "SOURCE_BINARY_RET", "../Grammar/BinaryInstruction.code");

     AsmSectionHeaderList.setFunctionSource ( "SOURCE_BINARY_FILE_SECTION_HEADER_LIST", "../Grammar/BinaryInstruction.code");
     AsmProgramHeaderList.setFunctionSource ( "SOURCE_BINARY_FILE_PROGRAM_HEADER_LIST", "../Grammar/BinaryInstruction.code");

// endif for USE_ROSE_BINARY_ANALYSIS_SUPPORT
#endif

   }
