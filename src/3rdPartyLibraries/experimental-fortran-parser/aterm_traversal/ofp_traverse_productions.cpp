#define COMPILED_WITH_ROSE 1

#if COMPILED_WITH_ROSE
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"
#endif

#include <iostream>

#include "traversal.hpp"
#include "OFPExpr.hpp"
#include "ASTBuilder.hpp"
#include <vector>

#define DEBUG_PRINT

#ifdef __cplusplus
//extern "C" {
#endif

OFP::ASTBuilder * ast = NULL;
void OFP::setASTBuilder(OFP::ASTBuilder * a) {ast = a;}

OFP::Unparser * unparser = NULL;
void OFP::setUnparser(OFP::Unparser * u) {unparser = u;}


/**                                                                                             
 * Section/Clause 2: Fortran concepts
 */

//========================================================================================
// R201 program
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Program(ATerm term, OFP::Program* Program)
{
#ifdef DEBUG_PRINT
   printf("Program: %s\n", ATwriteToString(term));
#endif

 OFP::StartCommentBlock StartCommentBlock;
 OFP::ProgramUnit ProgramUnit;
 if (ATmatch(term, "Program(<term>,<term>)", &StartCommentBlock.term, &ProgramUnit.term)) {

    if (ofp_traverse_StartCommentBlock(StartCommentBlock.term, &StartCommentBlock)) {
       // MATCHED StartCommentBlock
       Program->setStartCommentBlock(StartCommentBlock.newStartCommentBlock());
    }

   ATermList ProgramUnit_tail = (ATermList) ATmake("<term>", ProgramUnit.term);
   while (! ATisEmpty(ProgramUnit_tail)) {
      ProgramUnit.term = ATgetFirst(ProgramUnit_tail);
      ProgramUnit_tail = ATgetNext (ProgramUnit_tail);
      if (ofp_traverse_ProgramUnit(ProgramUnit.term, &ProgramUnit)) {
         // MATCHED ProgramUnit
         Program->appendProgramUnit(ProgramUnit.newProgramUnit());
      } else return ATfalse;
   }

   ast->build_Program(Program);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R202 program-unit
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProgramUnit(ATerm term, OFP::ProgramUnit* ProgramUnit)
{
#ifdef DEBUG_PRINT
   printf("ProgramUnit: %s\n", ATwriteToString(term));
#endif

 OFP::BlockData BlockData;
 if (ATmatch(term, "ProgramUnit_BD(<term>)", &BlockData.term)) {

      if (ofp_traverse_BlockData(BlockData.term, &BlockData)) {
         // MATCHED BlockData
         ProgramUnit->setBlockData(BlockData.newBlockData());
         ProgramUnit->inheritPayload(ProgramUnit->getBlockData());
      } else return ATfalse;

   // MATCHED ProgramUnit_BD
   ProgramUnit->setOptionType(OFP::ProgramUnit::ProgramUnit_BD);

   return ATtrue;
 }

 OFP::Submodule Submodule;
 if (ATmatch(term, "ProgramUnit_S(<term>)", &Submodule.term)) {

      if (ofp_traverse_Submodule(Submodule.term, &Submodule)) {
         // MATCHED Submodule
         ProgramUnit->setSubmodule(Submodule.newSubmodule());
         ProgramUnit->inheritPayload(ProgramUnit->getSubmodule());
      } else return ATfalse;

   // MATCHED ProgramUnit_S
   ProgramUnit->setOptionType(OFP::ProgramUnit::ProgramUnit_S);

   return ATtrue;
 }

 OFP::Module Module;
 if (ATmatch(term, "ProgramUnit_M(<term>)", &Module.term)) {

      if (ofp_traverse_Module(Module.term, &Module)) {
         // MATCHED Module
         ProgramUnit->setModule(Module.newModule());
         ProgramUnit->inheritPayload(ProgramUnit->getModule());
      } else return ATfalse;

   // MATCHED ProgramUnit_M
   ProgramUnit->setOptionType(OFP::ProgramUnit::ProgramUnit_M);

   return ATtrue;
 }

 OFP::ExternalSubprogram ExternalSubprogram;
 if (ATmatch(term, "ProgramUnit_ES(<term>)", &ExternalSubprogram.term)) {

      if (ofp_traverse_ExternalSubprogram(ExternalSubprogram.term, &ExternalSubprogram)) {
         // MATCHED ExternalSubprogram
         ProgramUnit->setExternalSubprogram(ExternalSubprogram.newExternalSubprogram());
         ProgramUnit->inheritPayload(ProgramUnit->getExternalSubprogram());
      } else return ATfalse;

   // MATCHED ProgramUnit_ES
   ProgramUnit->setOptionType(OFP::ProgramUnit::ProgramUnit_ES);

   return ATtrue;
 }

   OFP::MainProgram MainProgram;
   if (ofp_traverse_MainProgram(term, &MainProgram)) {
      // MATCHED MainProgram
      ProgramUnit->setMainProgram(MainProgram.newMainProgram());
      ProgramUnit->inheritPayload(ProgramUnit->getMainProgram());
      return ATtrue;
   }

   return ATfalse;
}

//========================================================================================
// R203 external-subprogram
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ExternalSubprogram(ATerm term, OFP::ExternalSubprogram* ExternalSubprogram)
{
#ifdef DEBUG_PRINT
   printf("ExternalSubprogram: %s\n", ATwriteToString(term));
#endif

 OFP::SubroutineSubprogram SubroutineSubprogram;
 if (ATmatch(term, "ExternalSubprogram_SS(<term>)", &SubroutineSubprogram.term)) {

      if (ofp_traverse_SubroutineSubprogram(SubroutineSubprogram.term, &SubroutineSubprogram)) {
         // MATCHED SubroutineSubprogram
         ExternalSubprogram->setSubroutineSubprogram(SubroutineSubprogram.newSubroutineSubprogram());
         ExternalSubprogram->inheritPayload(ExternalSubprogram->getSubroutineSubprogram());
      } else return ATfalse;

   // MATCHED ExternalSubprogram_SS
   ExternalSubprogram->setOptionType(OFP::ExternalSubprogram::ExternalSubprogram_SS);

   return ATtrue;
 }

 OFP::FunctionSubprogram FunctionSubprogram;
 if (ATmatch(term, "ExternalSubprogram_FS(<term>)", &FunctionSubprogram.term)) {

      if (ofp_traverse_FunctionSubprogram(FunctionSubprogram.term, &FunctionSubprogram)) {
         // MATCHED FunctionSubprogram
         ExternalSubprogram->setFunctionSubprogram(FunctionSubprogram.newFunctionSubprogram());
         ExternalSubprogram->inheritPayload(ExternalSubprogram->getFunctionSubprogram());
      } else return ATfalse;

   // MATCHED ExternalSubprogram_FS
   ExternalSubprogram->setOptionType(OFP::ExternalSubprogram::ExternalSubprogram_FS);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R204 specification-part
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InitialSpecPart(ATerm term, OFP::InitialSpecPart* InitialSpecPart)
{
#ifdef DEBUG_PRINT
   printf("InitialSpecPart(F): %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "InitialSpecPart(<term>)", &InitialSpecPart->term)) {

      ATermList list_tail = (ATermList) ATmake("<term>", InitialSpecPart->term);
      while (! ATisEmpty(list_tail)) {
         ATerm list_term;
         OFP::UseStmt UseStmt;
         OFP::ImportStmt ImportStmt;

         list_term = ATgetFirst(list_tail);
         list_tail = ATgetNext (list_tail);

         if (ofp_traverse_UseStmt(list_term, &UseStmt)) {
            // MATCHED UseStmt
            InitialSpecPart->appendStatement(UseStmt.newUseStmt());
         }
         else if (ofp_traverse_ImportStmt(list_term, &ImportStmt)) {
            // MATCHED ImportStmt
            InitialSpecPart->appendStatement(ImportStmt.newImportStmt());
         } else return ATfalse;
      }

      ast->build_InitialSpecPart(InitialSpecPart);

      return ATtrue;
   }

   return ATfalse;
}

ATbool ofp_traverse_SpecificationPart(ATerm term, OFP::SpecificationPart* SpecificationPart)
{
#ifdef DEBUG_PRINT
   printf("SpecificationPart(F): %s\n", ATwriteToString(term));
#endif

 OFP::UseStmt UseStmt;
 OFP::ImportStmt ImportStmt;
 OFP::ImplicitPart ImplicitPart;
 OFP::DeclarationConstruct DeclarationConstruct;
 if (ATmatch(term, "SpecificationPart(<term>,<term>,<term>,<term>)", &UseStmt.term, &ImportStmt.term, &ImplicitPart.term, &DeclarationConstruct.term)) {

   ATermList UseStmt_tail = (ATermList) ATmake("<term>", UseStmt.term);
   while (! ATisEmpty(UseStmt_tail)) {
      UseStmt.term = ATgetFirst(UseStmt_tail);
      UseStmt_tail = ATgetNext (UseStmt_tail);
      if (ofp_traverse_UseStmt(UseStmt.term, &UseStmt)) {
         // MATCHED UseStmt
         SpecificationPart->appendUseStmt(UseStmt.newUseStmt());
      } else return ATfalse;
   }

   ATermList ImportStmt_tail = (ATermList) ATmake("<term>", ImportStmt.term);
   while (! ATisEmpty(ImportStmt_tail)) {
      ImportStmt.term = ATgetFirst(ImportStmt_tail);
      ImportStmt_tail = ATgetNext (ImportStmt_tail);
      if (ofp_traverse_ImportStmt(ImportStmt.term, &ImportStmt)) {
         // MATCHED ImportStmt
         SpecificationPart->appendImportStmt(ImportStmt.newImportStmt());
      } else return ATfalse;
   }

   if (ATmatch(ImplicitPart.term, "Some(<term>)", &ImplicitPart.term)) {
      if (ofp_traverse_ImplicitPart(ImplicitPart.term, &ImplicitPart)) {
         // MATCHED ImplicitPart
         SpecificationPart->setImplicitPart(ImplicitPart.newImplicitPart());
      } else return ATfalse;
   }

   ATermList DeclarationConstruct_tail = (ATermList) ATmake("<term>", DeclarationConstruct.term);
   while (! ATisEmpty(DeclarationConstruct_tail)) {
      DeclarationConstruct.term = ATgetFirst(DeclarationConstruct_tail);
      DeclarationConstruct_tail = ATgetNext (DeclarationConstruct_tail);
      if (ofp_traverse_DeclarationConstruct(DeclarationConstruct.term, &DeclarationConstruct)) {
         // MATCHED DeclarationConstruct
         SpecificationPart->appendDeclarationConstruct(DeclarationConstruct.newDeclarationConstruct());
      } else return ATfalse;
   }

   ast->build_SpecificationPart(SpecificationPart);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R205 implicit-part
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ImplicitPart(ATerm term, OFP::ImplicitPart* ImplicitPart)
{
#ifdef DEBUG_PRINT
   printf("ImplicitPart(F): %s\n", ATwriteToString(term));
#endif

 OFP::ImplicitPartStmt ImplicitPartStmt;
 OFP::ImplicitStmt ImplicitStmt;
 if (ATmatch(term, "ImplicitPart(<term>,<term>)", &ImplicitPartStmt.term, &ImplicitStmt.term)) {

   ATermList ImplicitPartStmt_tail = (ATermList) ATmake("<term>", ImplicitPartStmt.term);
   while (! ATisEmpty(ImplicitPartStmt_tail)) {
      ImplicitPartStmt.term = ATgetFirst(ImplicitPartStmt_tail);
      ImplicitPartStmt_tail = ATgetNext (ImplicitPartStmt_tail);
      if (ofp_traverse_ImplicitPartStmt(ImplicitPartStmt.term, &ImplicitPartStmt)) {
         // MATCHED ImplicitPartStmt
         ImplicitPart->appendImplicitPartStmt(ImplicitPartStmt.newImplicitPartStmt());
      } else return ATfalse;
   }

      if (ofp_traverse_ImplicitStmt(ImplicitStmt.term, &ImplicitStmt)) {
         // MATCHED ImplicitStmt
         ImplicitPart->setImplicitStmt(ImplicitStmt.newImplicitStmt());
      } else return ATfalse;

   ast->build_ImplicitPart(ImplicitPart);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R206 implicit-part-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ImplicitPartStmt(ATerm term, OFP::ImplicitPartStmt* ImplicitPartStmt)
{
#ifdef DEBUG_PRINT
   printf("ImplicitPartStmt(F): %s\n", ATwriteToString(term));
#endif

 OFP::EntryStmt EntryStmt;
 if (ATmatch(term, "ImplicitPartStmt_ES(<term>)", &EntryStmt.term)) {

      if (ofp_traverse_EntryStmt(EntryStmt.term, &EntryStmt)) {
         // MATCHED EntryStmt
         ImplicitPartStmt->setEntryStmt(EntryStmt.newEntryStmt());
         ImplicitPartStmt->inheritPayload(ImplicitPartStmt->getEntryStmt());
      } else return ATfalse;

   // MATCHED ImplicitPartStmt_ES
   ImplicitPartStmt->setOptionType(OFP::ImplicitPartStmt::ImplicitPartStmt_ES);

   return ATtrue;
 }

 OFP::FormatStmt FormatStmt;
 if (ATmatch(term, "ImplicitPartStmt_FS(<term>)", &FormatStmt.term)) {

      if (ofp_traverse_FormatStmt(FormatStmt.term, &FormatStmt)) {
         // MATCHED FormatStmt
         ImplicitPartStmt->setFormatStmt(FormatStmt.newFormatStmt());
         ImplicitPartStmt->inheritPayload(ImplicitPartStmt->getFormatStmt());
      } else return ATfalse;

   // MATCHED ImplicitPartStmt_FS
   ImplicitPartStmt->setOptionType(OFP::ImplicitPartStmt::ImplicitPartStmt_FS);

   return ATtrue;
 }

 OFP::ParameterStmt ParameterStmt;
 if (ATmatch(term, "ImplicitPartStmt_PS(<term>)", &ParameterStmt.term)) {

      if (ofp_traverse_ParameterStmt(ParameterStmt.term, &ParameterStmt)) {
         // MATCHED ParameterStmt
         ImplicitPartStmt->setParameterStmt(ParameterStmt.newParameterStmt());
         ImplicitPartStmt->inheritPayload(ImplicitPartStmt->getParameterStmt());
      } else return ATfalse;

   // MATCHED ImplicitPartStmt_PS
   ImplicitPartStmt->setOptionType(OFP::ImplicitPartStmt::ImplicitPartStmt_PS);

   return ATtrue;
 }

 OFP::ImplicitStmt ImplicitStmt;
 if (ATmatch(term, "ImplicitPartStmt_IS(<term>)", &ImplicitStmt.term)) {

      if (ofp_traverse_ImplicitStmt(ImplicitStmt.term, &ImplicitStmt)) {
         // MATCHED ImplicitStmt
         ImplicitPartStmt->setImplicitStmt(ImplicitStmt.newImplicitStmt());
         ImplicitPartStmt->inheritPayload(ImplicitPartStmt->getImplicitStmt());
      } else return ATfalse;

   // MATCHED ImplicitPartStmt_IS
   ImplicitPartStmt->setOptionType(OFP::ImplicitPartStmt::ImplicitPartStmt_IS);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R207 declaration-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DeclarationConstruct(ATerm term, OFP::DeclarationConstruct* DeclarationConstruct)
{
#ifdef DEBUG_PRINT
   printf("DeclarationConstruct(F): %s\n", ATwriteToString(term));
#endif

 OFP::StmtFunctionStmt StmtFunctionStmt;
 if (ATmatch(term, "DeclarationConstruct_SFS(<term>)", &StmtFunctionStmt.term)) {

      if (ofp_traverse_StmtFunctionStmt(StmtFunctionStmt.term, &StmtFunctionStmt)) {
         // MATCHED StmtFunctionStmt
         DeclarationConstruct->setStmtFunctionStmt(StmtFunctionStmt.newStmtFunctionStmt());
         DeclarationConstruct->inheritPayload(DeclarationConstruct->getStmtFunctionStmt());
      } else return ATfalse;

   // MATCHED DeclarationConstruct_SFS
   DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::DeclarationConstruct_SFS);

   return ATtrue;
 }

   OFP::TypeDeclarationStmt TypeDeclarationStmt;
   if (ofp_traverse_TypeDeclarationStmt(term, &TypeDeclarationStmt)) {
      // MATCHED TypeDeclarationStmt
      DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::TypeDeclarationStmt_ot);
      DeclarationConstruct->setStatement(TypeDeclarationStmt.newTypeDeclarationStmt());
      DeclarationConstruct->setPayload(DeclarationConstruct->getStatement()->getPayload());
      return ATtrue;
   }

 OFP::OtherSpecificationStmt OtherSpecificationStmt;
 if (ATmatch(term, "DeclarationConstruct_OSS(<term>)", &OtherSpecificationStmt.term)) {

      if (ofp_traverse_OtherSpecificationStmt(OtherSpecificationStmt.term, &OtherSpecificationStmt)) {
         // MATCHED OtherSpecificationStmt
         DeclarationConstruct->setOtherSpecificationStmt(OtherSpecificationStmt.newOtherSpecificationStmt());
         DeclarationConstruct->inheritPayload(DeclarationConstruct->getOtherSpecificationStmt());
      } else return ATfalse;

   // MATCHED DeclarationConstruct_OSS
   DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::DeclarationConstruct_OSS);

   return ATtrue;
 }

 OFP::ProcedureDeclarationStmt ProcedureDeclarationStmt;
 if (ATmatch(term, "DeclarationConstruct_PDS(<term>)", &ProcedureDeclarationStmt.term)) {

      if (ofp_traverse_ProcedureDeclarationStmt(ProcedureDeclarationStmt.term, &ProcedureDeclarationStmt)) {
         // MATCHED ProcedureDeclarationStmt
         DeclarationConstruct->setProcedureDeclarationStmt(ProcedureDeclarationStmt.newProcedureDeclarationStmt());
         DeclarationConstruct->inheritPayload(DeclarationConstruct->getProcedureDeclarationStmt());
      } else return ATfalse;

   // MATCHED DeclarationConstruct_PDS
   DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::DeclarationConstruct_PDS);

   return ATtrue;
 }

 OFP::ParameterStmt ParameterStmt;
 if (ATmatch(term, "DeclarationConstruct_PS(<term>)", &ParameterStmt.term)) {

      if (ofp_traverse_ParameterStmt(ParameterStmt.term, &ParameterStmt)) {
         // MATCHED ParameterStmt
         DeclarationConstruct->setParameterStmt(ParameterStmt.newParameterStmt());
         DeclarationConstruct->inheritPayload(DeclarationConstruct->getParameterStmt());
      } else return ATfalse;

   // MATCHED DeclarationConstruct_PS
   DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::DeclarationConstruct_PS);

   return ATtrue;
 }

 OFP::InterfaceBlock InterfaceBlock;
 if (ATmatch(term, "DeclarationConstruct_IB(<term>)", &InterfaceBlock.term)) {

      if (ofp_traverse_InterfaceBlock(InterfaceBlock.term, &InterfaceBlock)) {
         // MATCHED InterfaceBlock
         DeclarationConstruct->setInterfaceBlock(InterfaceBlock.newInterfaceBlock());
         DeclarationConstruct->inheritPayload(DeclarationConstruct->getInterfaceBlock());
      } else return ATfalse;

   // MATCHED DeclarationConstruct_IB
   DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::DeclarationConstruct_IB);

   return ATtrue;
 }

 OFP::FormatStmt FormatStmt;
 if (ATmatch(term, "DeclarationConstruct_FS(<term>)", &FormatStmt.term)) {

      if (ofp_traverse_FormatStmt(FormatStmt.term, &FormatStmt)) {
         // MATCHED FormatStmt
         DeclarationConstruct->setFormatStmt(FormatStmt.newFormatStmt());
         DeclarationConstruct->inheritPayload(DeclarationConstruct->getFormatStmt());
      } else return ATfalse;

   // MATCHED DeclarationConstruct_FS
   DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::DeclarationConstruct_FS);

   return ATtrue;
 }

 OFP::EnumDef EnumDef;
 if (ATmatch(term, "DeclarationConstruct_ED(<term>)", &EnumDef.term)) {

      if (ofp_traverse_EnumDef(EnumDef.term, &EnumDef)) {
         // MATCHED EnumDef
         DeclarationConstruct->setEnumDef(EnumDef.newEnumDef());
         DeclarationConstruct->inheritPayload(DeclarationConstruct->getEnumDef());
      } else return ATfalse;

   // MATCHED DeclarationConstruct_ED
   DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::DeclarationConstruct_ED);

   return ATtrue;
 }

 OFP::EntryStmt EntryStmt;
 if (ATmatch(term, "DeclarationConstruct_ES(<term>)", &EntryStmt.term)) {

      if (ofp_traverse_EntryStmt(EntryStmt.term, &EntryStmt)) {
         // MATCHED EntryStmt
         DeclarationConstruct->setEntryStmt(EntryStmt.newEntryStmt());
         DeclarationConstruct->inheritPayload(DeclarationConstruct->getEntryStmt());
      } else return ATfalse;

   // MATCHED DeclarationConstruct_ES
   DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::DeclarationConstruct_ES);

   return ATtrue;
 }

 OFP::DerivedTypeDef DerivedTypeDef;
 if (ATmatch(term, "DeclarationConstruct_DTD(<term>)", &DerivedTypeDef.term)) {

      if (ofp_traverse_DerivedTypeDef(DerivedTypeDef.term, &DerivedTypeDef)) {
         // MATCHED DerivedTypeDef
         DeclarationConstruct->setDerivedTypeDef(DerivedTypeDef.newDerivedTypeDef());
         DeclarationConstruct->inheritPayload(DeclarationConstruct->getDerivedTypeDef());
      } else return ATfalse;

   // MATCHED DeclarationConstruct_DTD
   DeclarationConstruct->setOptionType(OFP::DeclarationConstruct::DeclarationConstruct_DTD);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R208 execution-part
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SpecAndExecPart(ATerm term, OFP::SpecAndExecPart* SpecAndExecPart)
{
#ifdef DEBUG_PRINT
   printf("SpecAndExecPart(F): %s\n", ATwriteToString(term));
#endif

   // ImplicitStmt          -> SpecAndExecConstruct
   // DeclarationConstruct  -> SpecAndExecConstruct
   // ExecutableConstruct   -> SpecAndExecConstruct

   OFP::ImplicitStmt ImplicitStmt;
   OFP::DeclarationConstruct DeclarationConstruct;
   OFP::ExecutableConstruct ExecutableConstruct;

   if (ATmatch(term, "SpecAndExecPart(<term>)", &SpecAndExecPart->term)) {

      ATermList list_tail = (ATermList) ATmake("<term>", SpecAndExecPart->term);
      while (! ATisEmpty(list_tail)) {
         ATerm list_term;

         list_term = ATgetFirst(list_tail);
         list_tail = ATgetNext (list_tail);

         if (ofp_traverse_ImplicitStmt(list_term, &ImplicitStmt)) {
            // MATCHED ImplicitStmt
            SpecAndExecPart->appendStatement(ImplicitStmt.newImplicitStmt());
         }
         else if (ofp_traverse_DeclarationConstruct(list_term, &DeclarationConstruct)) {
            SpecAndExecPart->appendStatement(DeclarationConstruct.getStatement());
         }
         else if (ofp_traverse_ExecutableConstruct(list_term, &ExecutableConstruct)) {
            SpecAndExecPart->appendStatement(ExecutableConstruct.getStatement());
         }
         else return ATfalse;
      }

      ast->build_SpecAndExecPart(SpecAndExecPart);

      return ATtrue;
   }

   return ATfalse;
}

ATbool ofp_traverse_ExecutionPart(ATerm term, OFP::ExecutionPart* ExecutionPart)
{
#ifdef DEBUG_PRINT
   printf("ExecutionPart(F): %s\n", ATwriteToString(term));
#endif

 OFP::ExecutionPartConstruct ExecutionPartConstruct;
 if (ATmatch(term, "ExecutionPart(<term>)", &ExecutionPartConstruct.term)) {

   ATermList ExecutionPartConstruct_tail = (ATermList) ATmake("<term>", ExecutionPartConstruct.term);
   while (! ATisEmpty(ExecutionPartConstruct_tail)) {
      ExecutionPartConstruct.term = ATgetFirst(ExecutionPartConstruct_tail);
      ExecutionPartConstruct_tail = ATgetNext (ExecutionPartConstruct_tail);
      if (ofp_traverse_ExecutionPartConstruct(ExecutionPartConstruct.term, &ExecutionPartConstruct)) {
         // MATCHED ExecutionPartConstruct
         ExecutionPart->appendExecutionPartConstruct(ExecutionPartConstruct.newExecutionPartConstruct());
      } else return ATfalse;
   }

   ast->build_ExecutionPart(ExecutionPart);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R209 execution-part-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ExecutionPartConstruct(ATerm term, OFP::ExecutionPartConstruct* ExecutionPartConstruct)
{
#ifdef DEBUG_PRINT
   printf("ExecutionPartConstruct(F): %s\n", ATwriteToString(term));
#endif

 OFP::DataStmt DataStmt;
 if (ATmatch(term, "ExecutionPartConstruct_DS(<term>)", &DataStmt.term)) {

      if (ofp_traverse_DataStmt(DataStmt.term, &DataStmt)) {
         // MATCHED DataStmt
         ExecutionPartConstruct->setDataStmt(DataStmt.newDataStmt());
         ExecutionPartConstruct->inheritPayload(ExecutionPartConstruct->getDataStmt());
      } else return ATfalse;

   // MATCHED ExecutionPartConstruct_DS
   ExecutionPartConstruct->setOptionType(OFP::ExecutionPartConstruct::ExecutionPartConstruct_DS);

   return ATtrue;
 }

 OFP::EntryStmt EntryStmt;
 if (ATmatch(term, "ExecutionPartConstruct_ES(<term>)", &EntryStmt.term)) {

      if (ofp_traverse_EntryStmt(EntryStmt.term, &EntryStmt)) {
         // MATCHED EntryStmt
         ExecutionPartConstruct->setEntryStmt(EntryStmt.newEntryStmt());
         ExecutionPartConstruct->inheritPayload(ExecutionPartConstruct->getEntryStmt());
      } else return ATfalse;

   // MATCHED ExecutionPartConstruct_ES
   ExecutionPartConstruct->setOptionType(OFP::ExecutionPartConstruct::ExecutionPartConstruct_ES);

   return ATtrue;
 }

 OFP::FormatStmt FormatStmt;
 if (ATmatch(term, "ExecutionPartConstruct_FS(<term>)", &FormatStmt.term)) {

      if (ofp_traverse_FormatStmt(FormatStmt.term, &FormatStmt)) {
         // MATCHED FormatStmt
         ExecutionPartConstruct->setFormatStmt(FormatStmt.newFormatStmt());
         ExecutionPartConstruct->inheritPayload(ExecutionPartConstruct->getFormatStmt());
      } else return ATfalse;

   // MATCHED ExecutionPartConstruct_FS
   ExecutionPartConstruct->setOptionType(OFP::ExecutionPartConstruct::ExecutionPartConstruct_FS);

   return ATtrue;
 }

 OFP::ExecutableConstruct ExecutableConstruct;
 if (ATmatch(term, "ExecutionPartConstruct_EC(<term>)", &ExecutableConstruct.term)) {

      if (ofp_traverse_ExecutableConstruct(ExecutableConstruct.term, &ExecutableConstruct)) {
         // MATCHED ExecutableConstruct
         ExecutionPartConstruct->setExecutableConstruct(ExecutableConstruct.newExecutableConstruct());
         ExecutionPartConstruct->inheritPayload(ExecutionPartConstruct->getExecutableConstruct());
      } else return ATfalse;

   // MATCHED ExecutionPartConstruct_EC
   ExecutionPartConstruct->setOptionType(OFP::ExecutionPartConstruct::ExecutionPartConstruct_EC);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R210 internal-subprogram-part
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InternalSubprogramPart(ATerm term, OFP::InternalSubprogramPart* InternalSubprogramPart)
{
#ifdef DEBUG_PRINT
   printf("SubprogramPart: %s\n", ATwriteToString(term));
#endif

 // ContainsStmt InternalSubprogram*  -> InternalSubprogramPart  {cons("SubprogramPart")}

 OFP::ContainsStmt ContainsStmt;
 OFP::InternalSubprogram InternalSubprogram;
 if (ATmatch(term, "SubprogramPart(<term>,<term>)", &ContainsStmt.term, &InternalSubprogram.term)) {

      if (ofp_traverse_ContainsStmt(ContainsStmt.term, &ContainsStmt)) {
         // MATCHED ContainsStmt
         InternalSubprogramPart->setContainsStmt(ContainsStmt.newContainsStmt());
      } else return ATfalse;

   ATermList InternalSubprogram_tail = (ATermList) ATmake("<term>", InternalSubprogram.term);
   while (! ATisEmpty(InternalSubprogram_tail)) {
      InternalSubprogram.term = ATgetFirst(InternalSubprogram_tail);
      InternalSubprogram_tail = ATgetNext (InternalSubprogram_tail);
      if (ofp_traverse_InternalSubprogram(InternalSubprogram.term, &InternalSubprogram)) {
         // MATCHED InternalSubprogram
         InternalSubprogramPart->appendInternalSubprogram(InternalSubprogram.newInternalSubprogram());
      } else return ATfalse;
   }

   ast->build_InternalSubprogramPart(InternalSubprogramPart);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R211 internal-subprogram
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InternalSubprogram(ATerm term, OFP::InternalSubprogram* InternalSubprogram)
{
#ifdef DEBUG_PRINT
   printf("InternalSubprogram: %s\n", ATwriteToString(term));
#endif

   //  FunctionSubprogram              -> InternalSubprogram
   //  SubroutineSubprogram            -> InternalSubprogram

   OFP::SubroutineSubprogram SubroutineSubprogram;
   OFP::FunctionSubprogram FunctionSubprogram;

   if (ofp_traverse_SubroutineSubprogram(term, &SubroutineSubprogram)) {
      // MATCHED SubroutineSubprogram
      InternalSubprogram->setOptionType(OFP::InternalSubprogram::SubroutineSubprogram_ot);
      InternalSubprogram->setSubroutineSubprogram(SubroutineSubprogram.newSubroutineSubprogram());
      InternalSubprogram->inheritPayload(InternalSubprogram->getSubroutineSubprogram());
      return ATtrue;
   }

   else if (ofp_traverse_FunctionSubprogram(term, &FunctionSubprogram)) {
      // MATCHED FunctionSubprogram
      InternalSubprogram->setOptionType(OFP::InternalSubprogram::FunctionSubprogram_ot);
      InternalSubprogram->setFunctionSubprogram(FunctionSubprogram.newFunctionSubprogram());
      InternalSubprogram->inheritPayload(InternalSubprogram->getFunctionSubprogram());
      return ATtrue;
   }

   return ATfalse;
}

//========================================================================================
// R212 other-specification-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_OtherSpecificationStmt(ATerm term, OFP::OtherSpecificationStmt* OtherSpecificationStmt)
{
#ifdef DEBUG_PRINT
   printf("OtherSpecificationStmt: %s\n", ATwriteToString(term));
#endif

 OFP::ValueStmt ValueStmt;
 if (ATmatch(term, "OtherSpecificationStmt_VS2(<term>)", &ValueStmt.term)) {

      if (ofp_traverse_ValueStmt(ValueStmt.term, &ValueStmt)) {
         // MATCHED ValueStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_VS2

   return ATtrue;
 }

 OFP::VolatileStmt VolatileStmt;
 if (ATmatch(term, "OtherSpecificationStmt_VS1(<term>)", &VolatileStmt.term)) {

      if (ofp_traverse_VolatileStmt(VolatileStmt.term, &VolatileStmt)) {
         // MATCHED VolatileStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_VS1

   return ATtrue;
 }

 OFP::TargetStmt TargetStmt;
 if (ATmatch(term, "OtherSpecificationStmt_TS(<term>)", &TargetStmt.term)) {

      if (ofp_traverse_TargetStmt(TargetStmt.term, &TargetStmt)) {
         // MATCHED TargetStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_TS

   return ATtrue;
 }

 OFP::SaveStmt SaveStmt;
 if (ATmatch(term, "OtherSpecificationStmt_SS(<term>)", &SaveStmt.term)) {

      if (ofp_traverse_SaveStmt(SaveStmt.term, &SaveStmt)) {
         // MATCHED SaveStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_SS

   return ATtrue;
 }

 OFP::ProtectedStmt ProtectedStmt;
 if (ATmatch(term, "OtherSpecificationStmt_PS2(<term>)", &ProtectedStmt.term)) {

      if (ofp_traverse_ProtectedStmt(ProtectedStmt.term, &ProtectedStmt)) {
         // MATCHED ProtectedStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_PS2

   return ATtrue;
 }

 OFP::PointerStmt PointerStmt;
 if (ATmatch(term, "OtherSpecificationStmt_PS1(<term>)", &PointerStmt.term)) {

      if (ofp_traverse_PointerStmt(PointerStmt.term, &PointerStmt)) {
         // MATCHED PointerStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_PS1

   return ATtrue;
 }

 OFP::OptionalStmt OptionalStmt;
 if (ATmatch(term, "OtherSpecificationStmt_OS(<term>)", &OptionalStmt.term)) {

      if (ofp_traverse_OptionalStmt(OptionalStmt.term, &OptionalStmt)) {
         // MATCHED OptionalStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_OS

   return ATtrue;
 }

 OFP::NamelistStmt NamelistStmt;
 if (ATmatch(term, "OtherSpecificationStmt_NS(<term>)", &NamelistStmt.term)) {

      if (ofp_traverse_NamelistStmt(NamelistStmt.term, &NamelistStmt)) {
         // MATCHED NamelistStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_NS

   return ATtrue;
 }

 OFP::IntrinsicStmt IntrinsicStmt;
 if (ATmatch(term, "OtherSpecificationStmt_IS2(<term>)", &IntrinsicStmt.term)) {

      if (ofp_traverse_IntrinsicStmt(IntrinsicStmt.term, &IntrinsicStmt)) {
         // MATCHED IntrinsicStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_IS2

   return ATtrue;
 }

 OFP::IntentStmt IntentStmt;
 if (ATmatch(term, "OtherSpecificationStmt_IS1(<term>)", &IntentStmt.term)) {

      if (ofp_traverse_IntentStmt(IntentStmt.term, &IntentStmt)) {
         // MATCHED IntentStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_IS1

   return ATtrue;
 }

 OFP::ExternalStmt ExternalStmt;
 if (ATmatch(term, "OtherSpecificationStmt_ES2(<term>)", &ExternalStmt.term)) {

      if (ofp_traverse_ExternalStmt(ExternalStmt.term, &ExternalStmt)) {
         // MATCHED ExternalStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_ES2

   return ATtrue;
 }

 OFP::EquivalenceStmt EquivalenceStmt;
 if (ATmatch(term, "OtherSpecificationStmt_ES1(<term>)", &EquivalenceStmt.term)) {

      if (ofp_traverse_EquivalenceStmt(EquivalenceStmt.term, &EquivalenceStmt)) {
         // MATCHED EquivalenceStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_ES1

   return ATtrue;
 }

 OFP::DimensionStmt DimensionStmt;
 if (ATmatch(term, "OtherSpecificationStmt_DS2(<term>)", &DimensionStmt.term)) {

      if (ofp_traverse_DimensionStmt(DimensionStmt.term, &DimensionStmt)) {
         // MATCHED DimensionStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_DS2

   return ATtrue;
 }

 OFP::DataStmt DataStmt;
 if (ATmatch(term, "OtherSpecificationStmt_DS1(<term>)", &DataStmt.term)) {

      if (ofp_traverse_DataStmt(DataStmt.term, &DataStmt)) {
         // MATCHED DataStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_DS1

   return ATtrue;
 }

 OFP::CommonStmt CommonStmt;
 if (ATmatch(term, "OtherSpecificationStmt_CS3(<term>)", &CommonStmt.term)) {

      if (ofp_traverse_CommonStmt(CommonStmt.term, &CommonStmt)) {
         // MATCHED CommonStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_CS3

   return ATtrue;
 }

 OFP::ContiguousStmt ContiguousStmt;
 if (ATmatch(term, "OtherSpecificationStmt_CS2(<term>)", &ContiguousStmt.term)) {

      if (ofp_traverse_ContiguousStmt(ContiguousStmt.term, &ContiguousStmt)) {
         // MATCHED ContiguousStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_CS2

   return ATtrue;
 }

 OFP::CodimensionStmt CodimensionStmt;
 if (ATmatch(term, "OtherSpecificationStmt_CS1(<term>)", &CodimensionStmt.term)) {

      if (ofp_traverse_CodimensionStmt(CodimensionStmt.term, &CodimensionStmt)) {
         // MATCHED CodimensionStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_CS1

   return ATtrue;
 }

 OFP::BindStmt BindStmt;
 if (ATmatch(term, "OtherSpecificationStmt_BS(<term>)", &BindStmt.term)) {

      if (ofp_traverse_BindStmt(BindStmt.term, &BindStmt)) {
         // MATCHED BindStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_BS

   return ATtrue;
 }

 OFP::AsynchronousStmt AsynchronousStmt;
 if (ATmatch(term, "OtherSpecificationStmt_AS3(<term>)", &AsynchronousStmt.term)) {

      if (ofp_traverse_AsynchronousStmt(AsynchronousStmt.term, &AsynchronousStmt)) {
         // MATCHED AsynchronousStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_AS3

   return ATtrue;
 }

 OFP::AllocatableStmt AllocatableStmt;
 if (ATmatch(term, "OtherSpecificationStmt_AS2(<term>)", &AllocatableStmt.term)) {

      if (ofp_traverse_AllocatableStmt(AllocatableStmt.term, &AllocatableStmt)) {
         // MATCHED AllocatableStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_AS2

   return ATtrue;
 }

 OFP::AccessStmt AccessStmt;
 if (ATmatch(term, "OtherSpecificationStmt_AS1(<term>)", &AccessStmt.term)) {

      if (ofp_traverse_AccessStmt(AccessStmt.term, &AccessStmt)) {
         // MATCHED AccessStmt
      } else return ATfalse;

   // MATCHED OtherSpecificationStmt_AS1

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R213 executable-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ExecutableConstruct(ATerm term, OFP::ExecutableConstruct* ExecutableConstruct)
{
#ifdef DEBUG_PRINT
   printf("ExecutableConstruct(F): %s\n", ATwriteToString(term));
#endif

 OFP::WhereConstruct WhereConstruct;
 if (ATmatch(term, "ExecutableConstruct_WC(<term>)", &WhereConstruct.term)) {

      if (ofp_traverse_WhereConstruct(WhereConstruct.term, &WhereConstruct)) {
         // MATCHED WhereConstruct
         ExecutableConstruct->setWhereConstruct(WhereConstruct.newWhereConstruct());
         ExecutableConstruct->inheritPayload(ExecutableConstruct->getWhereConstruct());
      } else return ATfalse;

   // MATCHED ExecutableConstruct_WC
   ExecutableConstruct->setOptionType(OFP::ExecutableConstruct::ExecutableConstruct_WC);

   return ATtrue;
 }

 OFP::SelectTypeConstruct SelectTypeConstruct;
 if (ATmatch(term, "ExecutableConstruct_STC(<term>)", &SelectTypeConstruct.term)) {

      if (ofp_traverse_SelectTypeConstruct(SelectTypeConstruct.term, &SelectTypeConstruct)) {
         // MATCHED SelectTypeConstruct
         ExecutableConstruct->setSelectTypeConstruct(SelectTypeConstruct.newSelectTypeConstruct());
         ExecutableConstruct->inheritPayload(ExecutableConstruct->getSelectTypeConstruct());
      } else return ATfalse;

   // MATCHED ExecutableConstruct_STC
   ExecutableConstruct->setOptionType(OFP::ExecutableConstruct::ExecutableConstruct_STC);

   return ATtrue;
 }

 OFP::IfConstruct IfConstruct;
 if (ATmatch(term, "ExecutableConstruct_IC(<term>)", &IfConstruct.term)) {

      if (ofp_traverse_IfConstruct(IfConstruct.term, &IfConstruct)) {
         // MATCHED IfConstruct
         ExecutableConstruct->setIfConstruct(IfConstruct.newIfConstruct());
         ExecutableConstruct->inheritPayload(ExecutableConstruct->getIfConstruct());
      } else return ATfalse;

   // MATCHED ExecutableConstruct_IC
   ExecutableConstruct->setOptionType(OFP::ExecutableConstruct::ExecutableConstruct_IC);

   return ATtrue;
 }

 OFP::ForallConstruct ForallConstruct;
 if (ATmatch(term, "ExecutableConstruct_FC(<term>)", &ForallConstruct.term)) {

      if (ofp_traverse_ForallConstruct(ForallConstruct.term, &ForallConstruct)) {
         // MATCHED ForallConstruct
         ExecutableConstruct->setForallConstruct(ForallConstruct.newForallConstruct());
         ExecutableConstruct->inheritPayload(ExecutableConstruct->getForallConstruct());
      } else return ATfalse;

   // MATCHED ExecutableConstruct_FC
   ExecutableConstruct->setOptionType(OFP::ExecutableConstruct::ExecutableConstruct_FC);

   return ATtrue;
 }

 OFP::CriticalConstruct CriticalConstruct;
 if (ATmatch(term, "ExecutableConstruct_CC2(<term>)", &CriticalConstruct.term)) {

      if (ofp_traverse_CriticalConstruct(CriticalConstruct.term, &CriticalConstruct)) {
         // MATCHED CriticalConstruct
         ExecutableConstruct->setCriticalConstruct(CriticalConstruct.newCriticalConstruct());
         ExecutableConstruct->inheritPayload(ExecutableConstruct->getCriticalConstruct());
      } else return ATfalse;

   // MATCHED ExecutableConstruct_CC2
   ExecutableConstruct->setOptionType(OFP::ExecutableConstruct::ExecutableConstruct_CC2);

   return ATtrue;
 }

 OFP::CaseConstruct CaseConstruct;
 if (ATmatch(term, "ExecutableConstruct_CC1(<term>)", &CaseConstruct.term)) {

      if (ofp_traverse_CaseConstruct(CaseConstruct.term, &CaseConstruct)) {
         // MATCHED CaseConstruct
         ExecutableConstruct->setCaseConstruct(CaseConstruct.newCaseConstruct());
         ExecutableConstruct->inheritPayload(ExecutableConstruct->getCaseConstruct());
      } else return ATfalse;

   // MATCHED ExecutableConstruct_CC1
   ExecutableConstruct->setOptionType(OFP::ExecutableConstruct::ExecutableConstruct_CC1);

   return ATtrue;
 }

 OFP::BlockConstruct BlockConstruct;
 if (ATmatch(term, "ExecutableConstruct_BC(<term>)", &BlockConstruct.term)) {

      if (ofp_traverse_BlockConstruct(BlockConstruct.term, &BlockConstruct)) {
         // MATCHED BlockConstruct
         ExecutableConstruct->setBlockConstruct(BlockConstruct.newBlockConstruct());
         ExecutableConstruct->inheritPayload(ExecutableConstruct->getBlockConstruct());
      } else return ATfalse;

   // MATCHED ExecutableConstruct_BC
   ExecutableConstruct->setOptionType(OFP::ExecutableConstruct::ExecutableConstruct_BC);

   return ATtrue;
 }

 OFP::AssociateConstruct AssociateConstruct;
 if (ATmatch(term, "ExecutableConstruct_AC(<term>)", &AssociateConstruct.term)) {

      if (ofp_traverse_AssociateConstruct(AssociateConstruct.term, &AssociateConstruct)) {
         // MATCHED AssociateConstruct
         ExecutableConstruct->setAssociateConstruct(AssociateConstruct.newAssociateConstruct());
         ExecutableConstruct->inheritPayload(ExecutableConstruct->getAssociateConstruct());
      } else return ATfalse;

   // MATCHED ExecutableConstruct_AC
   ExecutableConstruct->setOptionType(OFP::ExecutableConstruct::ExecutableConstruct_AC);

   return ATtrue;
 }

   OFP::ActionStmt ActionStmt;
   if (ofp_traverse_ActionStmt(term, &ActionStmt)) {
      // MATCHED ActionStmt
      ExecutableConstruct->setOptionType(ActionStmt.getOptionType());
      ExecutableConstruct->setStatement(ActionStmt.getStatement());
      ExecutableConstruct->setPayload(ExecutableConstruct->getStatement()->getPayload());
      return ATtrue;
   }

 return ATfalse;
}

//========================================================================================
// R214 action-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ActionStmt(ATerm term, OFP::ActionStmt* ActionStmt)
{
#ifdef DEBUG_PRINT
   printf("ActionStmt(F): %s\n", ATwriteToString(term));
#endif

 OFP::EndDoStmt EndDoStmt;
 if (ATmatch(term, "ActionStmt_EDS(<term>)", &EndDoStmt.term)) {

      if (ofp_traverse_EndDoStmt(EndDoStmt.term, &EndDoStmt)) {
         // MATCHED EndDoStmt
         ActionStmt->setEndDoStmt(EndDoStmt.newEndDoStmt());
         ActionStmt->inheritPayload(ActionStmt->getEndDoStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_EDS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_EDS);

   return ATtrue;
 }

 OFP::NonlabelDoStmt NonlabelDoStmt;
 if (ATmatch(term, "ActionStmt_NDS(<term>)", &NonlabelDoStmt.term)) {

      if (ofp_traverse_NonlabelDoStmt(NonlabelDoStmt.term, &NonlabelDoStmt)) {
         // MATCHED NonlabelDoStmt
         ActionStmt->setNonlabelDoStmt(NonlabelDoStmt.newNonlabelDoStmt());
         ActionStmt->inheritPayload(ActionStmt->getNonlabelDoStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_NDS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_NDS);

   return ATtrue;
 }

 OFP::LabelDoStmt LabelDoStmt;
 if (ATmatch(term, "ActionStmt_LDS(<term>)", &LabelDoStmt.term)) {

      if (ofp_traverse_LabelDoStmt(LabelDoStmt.term, &LabelDoStmt)) {
         // MATCHED LabelDoStmt
         ActionStmt->setLabelDoStmt(LabelDoStmt.newLabelDoStmt());
         ActionStmt->inheritPayload(ActionStmt->getLabelDoStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_LDS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_LDS);

   return ATtrue;
 }

 OFP::ComputedGotoStmt ComputedGotoStmt;
 if (ATmatch(term, "ActionStmt_CGS(<term>)", &ComputedGotoStmt.term)) {

      if (ofp_traverse_ComputedGotoStmt(ComputedGotoStmt.term, &ComputedGotoStmt)) {
         // MATCHED ComputedGotoStmt
         ActionStmt->setComputedGotoStmt(ComputedGotoStmt.newComputedGotoStmt());
         ActionStmt->inheritPayload(ActionStmt->getComputedGotoStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_CGS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_CGS);

   return ATtrue;
 }

 OFP::ArithmeticIfStmt ArithmeticIfStmt;
 if (ATmatch(term, "ActionStmt_AIS(<term>)", &ArithmeticIfStmt.term)) {

      if (ofp_traverse_ArithmeticIfStmt(ArithmeticIfStmt.term, &ArithmeticIfStmt)) {
         // MATCHED ArithmeticIfStmt
         ActionStmt->setArithmeticIfStmt(ArithmeticIfStmt.newArithmeticIfStmt());
         ActionStmt->inheritPayload(ActionStmt->getArithmeticIfStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_AIS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_AIS);

   return ATtrue;
 }

 OFP::WriteStmt WriteStmt;
 if (ATmatch(term, "ActionStmt_WS3(<term>)", &WriteStmt.term)) {

      if (ofp_traverse_WriteStmt(WriteStmt.term, &WriteStmt)) {
         // MATCHED WriteStmt
         ActionStmt->setWriteStmt(WriteStmt.newWriteStmt());
         ActionStmt->inheritPayload(ActionStmt->getWriteStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_WS3
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_WS3);

   return ATtrue;
 }

 OFP::WhereStmt WhereStmt;
 if (ATmatch(term, "ActionStmt_WS2(<term>)", &WhereStmt.term)) {

      if (ofp_traverse_WhereStmt(WhereStmt.term, &WhereStmt)) {
         // MATCHED WhereStmt
         ActionStmt->setWhereStmt(WhereStmt.newWhereStmt());
         ActionStmt->inheritPayload(ActionStmt->getWhereStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_WS2
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_WS2);

   return ATtrue;
 }

 OFP::WaitStmt WaitStmt;
 if (ATmatch(term, "ActionStmt_WS1(<term>)", &WaitStmt.term)) {

      if (ofp_traverse_WaitStmt(WaitStmt.term, &WaitStmt)) {
         // MATCHED WaitStmt
         ActionStmt->setWaitStmt(WaitStmt.newWaitStmt());
         ActionStmt->inheritPayload(ActionStmt->getWaitStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_WS1
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_WS1);

   return ATtrue;
 }

 OFP::UnlockStmt UnlockStmt;
 if (ATmatch(term, "ActionStmt_US(<term>)", &UnlockStmt.term)) {

      if (ofp_traverse_UnlockStmt(UnlockStmt.term, &UnlockStmt)) {
         // MATCHED UnlockStmt
         ActionStmt->setUnlockStmt(UnlockStmt.newUnlockStmt());
         ActionStmt->inheritPayload(ActionStmt->getUnlockStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_US
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_US);

   return ATtrue;
 }

 OFP::SyncMemoryStmt SyncMemoryStmt;
 if (ATmatch(term, "ActionStmt_SMS(<term>)", &SyncMemoryStmt.term)) {

      if (ofp_traverse_SyncMemoryStmt(SyncMemoryStmt.term, &SyncMemoryStmt)) {
         // MATCHED SyncMemoryStmt
         ActionStmt->setSyncMemoryStmt(SyncMemoryStmt.newSyncMemoryStmt());
         ActionStmt->inheritPayload(ActionStmt->getSyncMemoryStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_SMS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_SMS);

   return ATtrue;
 }

 OFP::SyncImagesStmt SyncImagesStmt;
 if (ATmatch(term, "ActionStmt_SIS(<term>)", &SyncImagesStmt.term)) {

      if (ofp_traverse_SyncImagesStmt(SyncImagesStmt.term, &SyncImagesStmt)) {
         // MATCHED SyncImagesStmt
         ActionStmt->setSyncImagesStmt(SyncImagesStmt.newSyncImagesStmt());
         ActionStmt->inheritPayload(ActionStmt->getSyncImagesStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_SIS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_SIS);

   return ATtrue;
 }

 OFP::SyncAllStmt SyncAllStmt;
 if (ATmatch(term, "ActionStmt_SAS(<term>)", &SyncAllStmt.term)) {

      if (ofp_traverse_SyncAllStmt(SyncAllStmt.term, &SyncAllStmt)) {
         // MATCHED SyncAllStmt
         ActionStmt->setSyncAllStmt(SyncAllStmt.newSyncAllStmt());
         ActionStmt->inheritPayload(ActionStmt->getSyncAllStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_SAS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_SAS);

   return ATtrue;
 }

 OFP::StopStmt StopStmt;
 if (ATmatch(term, "ActionStmt_SS(<term>)", &StopStmt.term)) {

      if (ofp_traverse_StopStmt(StopStmt.term, &StopStmt)) {
         // MATCHED StopStmt
         ActionStmt->setStopStmt(StopStmt.newStopStmt());
         ActionStmt->inheritPayload(ActionStmt->getStopStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_SS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_SS);

   return ATtrue;
 }

 OFP::RewindStmt RewindStmt;
 if (ATmatch(term, "ActionStmt_RS3(<term>)", &RewindStmt.term)) {

      if (ofp_traverse_RewindStmt(RewindStmt.term, &RewindStmt)) {
         // MATCHED RewindStmt
         ActionStmt->setRewindStmt(RewindStmt.newRewindStmt());
         ActionStmt->inheritPayload(ActionStmt->getRewindStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_RS3
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_RS3);

   return ATtrue;
 }

 OFP::ReturnStmt ReturnStmt;
 if (ATmatch(term, "ActionStmt_RS2(<term>)", &ReturnStmt.term)) {

      if (ofp_traverse_ReturnStmt(ReturnStmt.term, &ReturnStmt)) {
         // MATCHED ReturnStmt
         ActionStmt->setReturnStmt(ReturnStmt.newReturnStmt());
         ActionStmt->inheritPayload(ActionStmt->getReturnStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_RS2
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_RS2);

   return ATtrue;
 }

 OFP::ReadStmt ReadStmt;
 if (ATmatch(term, "ActionStmt_RS1(<term>)", &ReadStmt.term)) {

      if (ofp_traverse_ReadStmt(ReadStmt.term, &ReadStmt)) {
         // MATCHED ReadStmt
         ActionStmt->setReadStmt(ReadStmt.newReadStmt());
         ActionStmt->inheritPayload(ActionStmt->getReadStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_RS1
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_RS1);

   return ATtrue;
 }

 OFP::PrintStmt PrintStmt;
 if (ATmatch(term, "ActionStmt_PS1(<term>)", &PrintStmt.term)) {

      if (ofp_traverse_PrintStmt(PrintStmt.term, &PrintStmt)) {
         // MATCHED PrintStmt
         ActionStmt->setPrintStmt(PrintStmt.newPrintStmt());
         ActionStmt->inheritPayload(ActionStmt->getPrintStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_PS1
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_PS1);

   return ATtrue;
 }

 OFP::PointerAssignmentStmt PointerAssignmentStmt;
 if (ATmatch(term, "ActionStmt_PAS(<term>)", &PointerAssignmentStmt.term)) {

      if (ofp_traverse_PointerAssignmentStmt(PointerAssignmentStmt.term, &PointerAssignmentStmt)) {
         // MATCHED PointerAssignmentStmt
         ActionStmt->setPointerAssignmentStmt(PointerAssignmentStmt.newPointerAssignmentStmt());
         ActionStmt->inheritPayload(ActionStmt->getPointerAssignmentStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_PAS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_PAS);

   return ATtrue;
 }

 OFP::OpenStmt OpenStmt;
 if (ATmatch(term, "ActionStmt_OS(<term>)", &OpenStmt.term)) {

      if (ofp_traverse_OpenStmt(OpenStmt.term, &OpenStmt)) {
         // MATCHED OpenStmt
         ActionStmt->setOpenStmt(OpenStmt.newOpenStmt());
         ActionStmt->inheritPayload(ActionStmt->getOpenStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_OS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_OS);

   return ATtrue;
 }

 OFP::NullifyStmt NullifyStmt;
 if (ATmatch(term, "ActionStmt_NS(<term>)", &NullifyStmt.term)) {

      if (ofp_traverse_NullifyStmt(NullifyStmt.term, &NullifyStmt)) {
         // MATCHED NullifyStmt
         ActionStmt->setNullifyStmt(NullifyStmt.newNullifyStmt());
         ActionStmt->inheritPayload(ActionStmt->getNullifyStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_NS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_NS);

   return ATtrue;
 }

 OFP::LockStmt LockStmt;
 if (ATmatch(term, "ActionStmt_LS(<term>)", &LockStmt.term)) {

      if (ofp_traverse_LockStmt(LockStmt.term, &LockStmt)) {
         // MATCHED LockStmt
         ActionStmt->setLockStmt(LockStmt.newLockStmt());
         ActionStmt->inheritPayload(ActionStmt->getLockStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_LS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_LS);

   return ATtrue;
 }

 OFP::InquireStmt InquireStmt;
 if (ATmatch(term, "ActionStmt_IS2(<term>)", &InquireStmt.term)) {

      if (ofp_traverse_InquireStmt(InquireStmt.term, &InquireStmt)) {
         // MATCHED InquireStmt
         ActionStmt->setInquireStmt(InquireStmt.newInquireStmt());
         ActionStmt->inheritPayload(ActionStmt->getInquireStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_IS2
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_IS2);

   return ATtrue;
 }

 OFP::IfStmt IfStmt;
 if (ATmatch(term, "ActionStmt_IS1(<term>)", &IfStmt.term)) {

      if (ofp_traverse_IfStmt(IfStmt.term, &IfStmt)) {
         // MATCHED IfStmt
         ActionStmt->setIfStmt(IfStmt.newIfStmt());
         ActionStmt->inheritPayload(ActionStmt->getIfStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_IS1
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_IS1);

   return ATtrue;
 }

 OFP::GotoStmt GotoStmt;
 if (ATmatch(term, "ActionStmt_GS(<term>)", &GotoStmt.term)) {

      if (ofp_traverse_GotoStmt(GotoStmt.term, &GotoStmt)) {
         // MATCHED GotoStmt
         ActionStmt->setGotoStmt(GotoStmt.newGotoStmt());
         ActionStmt->inheritPayload(ActionStmt->getGotoStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_GS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_GS);

   return ATtrue;
 }

 OFP::ForallStmt ForallStmt;
 if (ATmatch(term, "ActionStmt_FS2(<term>)", &ForallStmt.term)) {

      if (ofp_traverse_ForallStmt(ForallStmt.term, &ForallStmt)) {
         // MATCHED ForallStmt
         ActionStmt->setForallStmt(ForallStmt.newForallStmt());
         ActionStmt->inheritPayload(ActionStmt->getForallStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_FS2
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_FS2);

   return ATtrue;
 }

 OFP::FlushStmt FlushStmt;
 if (ATmatch(term, "ActionStmt_FS1(<term>)", &FlushStmt.term)) {

      if (ofp_traverse_FlushStmt(FlushStmt.term, &FlushStmt)) {
         // MATCHED FlushStmt
         ActionStmt->setFlushStmt(FlushStmt.newFlushStmt());
         ActionStmt->inheritPayload(ActionStmt->getFlushStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_FS1
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_FS1);

   return ATtrue;
 }

 OFP::ExitStmt ExitStmt;
 if (ATmatch(term, "ActionStmt_ES2(<term>)", &ExitStmt.term)) {

      if (ofp_traverse_ExitStmt(ExitStmt.term, &ExitStmt)) {
         // MATCHED ExitStmt
         ActionStmt->setExitStmt(ExitStmt.newExitStmt());
         ActionStmt->inheritPayload(ActionStmt->getExitStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_ES2
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_ES2);

   return ATtrue;
 }

 OFP::ErrorStopStmt ErrorStopStmt;
 if (ATmatch(term, "ActionStmt_ESS2(<term>)", &ErrorStopStmt.term)) {

      if (ofp_traverse_ErrorStopStmt(ErrorStopStmt.term, &ErrorStopStmt)) {
         // MATCHED ErrorStopStmt
         ActionStmt->setErrorStopStmt(ErrorStopStmt.newErrorStopStmt());
         ActionStmt->inheritPayload(ActionStmt->getErrorStopStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_ESS2
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_ESS2);

   return ATtrue;
 }

 OFP::EndfileStmt EndfileStmt;
 if (ATmatch(term, "ActionStmt_ES1(<term>)", &EndfileStmt.term)) {

      if (ofp_traverse_EndfileStmt(EndfileStmt.term, &EndfileStmt)) {
         // MATCHED EndfileStmt
         ActionStmt->setEndfileStmt(EndfileStmt.newEndfileStmt());
         ActionStmt->inheritPayload(ActionStmt->getEndfileStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_ES1
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_ES1);

   return ATtrue;
 }

 OFP::EndSubroutineStmt EndSubroutineStmt;
 if (ATmatch(term, "ActionStmt_ESS1(<term>)", &EndSubroutineStmt.term)) {

      if (ofp_traverse_EndSubroutineStmt(EndSubroutineStmt.term, &EndSubroutineStmt)) {
         // MATCHED EndSubroutineStmt
         ActionStmt->setEndSubroutineStmt(EndSubroutineStmt.newEndSubroutineStmt());
         ActionStmt->inheritPayload(ActionStmt->getEndSubroutineStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_ESS1
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_ESS1);

   return ATtrue;
 }

 OFP::EndProgramStmt EndProgramStmt;
 if (ATmatch(term, "ActionStmt_EPS(<term>)", &EndProgramStmt.term)) {

      if (ofp_traverse_EndProgramStmt(EndProgramStmt.term, &EndProgramStmt)) {
         // MATCHED EndProgramStmt
         ActionStmt->setEndProgramStmt(EndProgramStmt.newEndProgramStmt());
         ActionStmt->inheritPayload(ActionStmt->getEndProgramStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_EPS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_EPS);

   return ATtrue;
 }

 OFP::EndMpSubprogramStmt EndMpSubprogramStmt;
 if (ATmatch(term, "ActionStmt_EMSS(<term>)", &EndMpSubprogramStmt.term)) {

      if (ofp_traverse_EndMpSubprogramStmt(EndMpSubprogramStmt.term, &EndMpSubprogramStmt)) {
         // MATCHED EndMpSubprogramStmt
         ActionStmt->setEndMpSubprogramStmt(EndMpSubprogramStmt.newEndMpSubprogramStmt());
         ActionStmt->inheritPayload(ActionStmt->getEndMpSubprogramStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_EMSS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_EMSS);

   return ATtrue;
 }

 OFP::EndFunctionStmt EndFunctionStmt;
 if (ATmatch(term, "ActionStmt_EFS(<term>)", &EndFunctionStmt.term)) {

      if (ofp_traverse_EndFunctionStmt(EndFunctionStmt.term, &EndFunctionStmt)) {
         // MATCHED EndFunctionStmt
         ActionStmt->setEndFunctionStmt(EndFunctionStmt.newEndFunctionStmt());
         ActionStmt->inheritPayload(ActionStmt->getEndFunctionStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_EFS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_EFS);

   return ATtrue;
 }

 OFP::DeallocateStmt DeallocateStmt;
 if (ATmatch(term, "ActionStmt_DS(<term>)", &DeallocateStmt.term)) {

      if (ofp_traverse_DeallocateStmt(DeallocateStmt.term, &DeallocateStmt)) {
         // MATCHED DeallocateStmt
         ActionStmt->setDeallocateStmt(DeallocateStmt.newDeallocateStmt());
         ActionStmt->inheritPayload(ActionStmt->getDeallocateStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_DS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_DS);

   return ATtrue;
 }

 OFP::CycleStmt CycleStmt;
 if (ATmatch(term, "ActionStmt_CS4(<term>)", &CycleStmt.term)) {

      if (ofp_traverse_CycleStmt(CycleStmt.term, &CycleStmt)) {
         // MATCHED CycleStmt
         ActionStmt->setCycleStmt(CycleStmt.newCycleStmt());
         ActionStmt->inheritPayload(ActionStmt->getCycleStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_CS4
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_CS4);

   return ATtrue;
 }

 OFP::ContinueStmt ContinueStmt;
 if (ATmatch(term, "ActionStmt_CS3(<term>)", &ContinueStmt.term)) {

      if (ofp_traverse_ContinueStmt(ContinueStmt.term, &ContinueStmt)) {
         // MATCHED ContinueStmt
         ActionStmt->setContinueStmt(ContinueStmt.newContinueStmt());
         ActionStmt->inheritPayload(ActionStmt->getContinueStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_CS3
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_CS3);

   return ATtrue;
 }

 OFP::CloseStmt CloseStmt;
 if (ATmatch(term, "ActionStmt_CS2(<term>)", &CloseStmt.term)) {

      if (ofp_traverse_CloseStmt(CloseStmt.term, &CloseStmt)) {
         // MATCHED CloseStmt
         ActionStmt->setCloseStmt(CloseStmt.newCloseStmt());
         ActionStmt->inheritPayload(ActionStmt->getCloseStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_CS2
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_CS2);

   return ATtrue;
 }

 OFP::CallStmt CallStmt;
 if (ATmatch(term, "ActionStmt_CS1(<term>)", &CallStmt.term)) {

      if (ofp_traverse_CallStmt(CallStmt.term, &CallStmt)) {
         // MATCHED CallStmt
         ActionStmt->setCallStmt(CallStmt.newCallStmt());
         ActionStmt->inheritPayload(ActionStmt->getCallStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_CS1
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_CS1);

   return ATtrue;
 }

 OFP::BackspaceStmt BackspaceStmt;
 if (ATmatch(term, "ActionStmt_BS(<term>)", &BackspaceStmt.term)) {

      if (ofp_traverse_BackspaceStmt(BackspaceStmt.term, &BackspaceStmt)) {
         // MATCHED BackspaceStmt
         ActionStmt->setBackspaceStmt(BackspaceStmt.newBackspaceStmt());
         ActionStmt->inheritPayload(ActionStmt->getBackspaceStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_BS
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_BS);

   return ATtrue;
 }

   OFP::AssignmentStmt AssignmentStmt;
   if (ofp_traverse_AssignmentStmt(term, &AssignmentStmt)) {
      // MATCHED AssignmentStmt
      ActionStmt->setOptionType(OFP::ActionStmt::AssignmentStmt_ot);
      ActionStmt->setStatement(AssignmentStmt.newAssignmentStmt());
      ActionStmt->setPayload(ActionStmt->getStatement()->getPayload());
      return ATtrue;
   }

 OFP::AllocateStmt AllocateStmt;
 if (ATmatch(term, "ActionStmt_AS1(<term>)", &AllocateStmt.term)) {

      if (ofp_traverse_AllocateStmt(AllocateStmt.term, &AllocateStmt)) {
         // MATCHED AllocateStmt
         ActionStmt->setAllocateStmt(AllocateStmt.newAllocateStmt());
         ActionStmt->inheritPayload(ActionStmt->getAllocateStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_AS1
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_AS1);

   return ATtrue;
 }

 OFP::PauseStmt PauseStmt;
 if (ATmatch(term, "ActionStmt_PS2(<term>)", &PauseStmt.term)) {

      if (ofp_traverse_PauseStmt(PauseStmt.term, &PauseStmt)) {
         // MATCHED PauseStmt
         ActionStmt->setPauseStmt(PauseStmt.newPauseStmt());
         ActionStmt->inheritPayload(ActionStmt->getPauseStmt());
      } else return ATfalse;

   // MATCHED ActionStmt_PS2
   ActionStmt->setOptionType(OFP::ActionStmt::ActionStmt_PS2);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R215 keyword
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Keyword(ATerm term, OFP::Keyword* Keyword)
{
#ifdef DEBUG_PRINT
   printf("Keyword: %s\n", ATwriteToString(term));
#endif

 OFP::Name Name;
 if (ATmatch(term, "Keyword(<term>)", &Name.term)) {

      if (ofp_traverse_Name(Name.term, &Name)) {
         // MATCHED Name
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}


/**                                                                                             
 * Section/Clause 3: Lexical tokens and source form
 */

//========================================================================================
// R304 constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Constant(ATerm term, OFP::Constant* Constant)
{
#ifdef DEBUG_PRINT
   printf("Constant(W): %s\n", ATwriteToString(term));
#endif

   // LiteralConstant                       -> Constant
   // NamedConstant                         -> Constant  {reject}  %% AMBIGUOUS - can't tell name is constant

   OFP::LiteralConstant LiteralConstant;
   if (ofp_traverse_LiteralConstant(term, &LiteralConstant)) {
      // MATCHED LiteralConstant
      Constant->setLiteralConstant(LiteralConstant.newLiteralConstant());
      Constant->inheritPayload(Constant->getLiteralConstant());
      return ATtrue;
   }

   return ATfalse;
}

//========================================================================================
// R305 literal-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LiteralConstant(ATerm term, OFP::LiteralConstant* LiteralConstant)
{
#ifdef DEBUG_PRINT
   printf("LiteralConstant(F): %s\n", ATwriteToString(term));
#endif

   OFP::IntLiteralConstant IntLiteralConstant;
   if (ofp_traverse_IntLiteralConstant(term, &IntLiteralConstant)) {
      // MATCHED IntLiteralConstant
      LiteralConstant->setOptionType(OFP::LiteralConstant::IntLiteralConstant_ot);
      LiteralConstant->setIntLiteralConstant(IntLiteralConstant.newIntLiteralConstant());
      LiteralConstant->inheritPayload(LiteralConstant->getIntLiteralConstant());
      return ATtrue;
   }

 OFP::BozLiteralConstant BozLiteralConstant;
 if (ATmatch(term, "LiteralConstant_BLC(<term>)", &BozLiteralConstant.term)) {

      if (ofp_traverse_BozLiteralConstant(BozLiteralConstant.term, &BozLiteralConstant)) {
         // MATCHED BozLiteralConstant
         LiteralConstant->setBozLiteralConstant(BozLiteralConstant.newBozLiteralConstant());
         LiteralConstant->inheritPayload(LiteralConstant->getBozLiteralConstant());
      } else return ATfalse;

   // MATCHED LiteralConstant_BLC
   LiteralConstant->setOptionType(OFP::LiteralConstant::LiteralConstant_BLC);

   return ATtrue;
 }

 OFP::CharLiteralConstant CharLiteralConstant;
 if (ATmatch(term, "LiteralConstant_CRLC(<term>)", &CharLiteralConstant.term)) {

      if (ofp_traverse_CharLiteralConstant(CharLiteralConstant.term, &CharLiteralConstant)) {
         // MATCHED CharLiteralConstant
         LiteralConstant->setCharLiteralConstant(CharLiteralConstant.newCharLiteralConstant());
         LiteralConstant->inheritPayload(LiteralConstant->getCharLiteralConstant());
      } else return ATfalse;

   // MATCHED LiteralConstant_CRLC
   LiteralConstant->setOptionType(OFP::LiteralConstant::LiteralConstant_CRLC);

   return ATtrue;
 }

 OFP::LogicalLiteralConstant LogicalLiteralConstant;
 if (ATmatch(term, "LiteralConstant_LLC(<term>)", &LogicalLiteralConstant.term)) {

      if (ofp_traverse_LogicalLiteralConstant(LogicalLiteralConstant.term, &LogicalLiteralConstant)) {
         // MATCHED LogicalLiteralConstant
         LiteralConstant->setLogicalLiteralConstant(LogicalLiteralConstant.newLogicalLiteralConstant());
         LiteralConstant->inheritPayload(LiteralConstant->getLogicalLiteralConstant());
      } else return ATfalse;

   // MATCHED LiteralConstant_LLC
   LiteralConstant->setOptionType(OFP::LiteralConstant::LiteralConstant_LLC);

   return ATtrue;
 }

 OFP::ComplexLiteralConstant ComplexLiteralConstant;
 if (ATmatch(term, "LiteralConstant_CXLC(<term>)", &ComplexLiteralConstant.term)) {

      if (ofp_traverse_ComplexLiteralConstant(ComplexLiteralConstant.term, &ComplexLiteralConstant)) {
         // MATCHED ComplexLiteralConstant
         LiteralConstant->setComplexLiteralConstant(ComplexLiteralConstant.newComplexLiteralConstant());
         LiteralConstant->inheritPayload(LiteralConstant->getComplexLiteralConstant());
      } else return ATfalse;

   // MATCHED LiteralConstant_CXLC
   LiteralConstant->setOptionType(OFP::LiteralConstant::LiteralConstant_CXLC);

   return ATtrue;
 }

 OFP::RealLiteralConstant RealLiteralConstant;
 if (ATmatch(term, "LiteralConstant_RLC(<term>)", &RealLiteralConstant.term)) {

      if (ofp_traverse_RealLiteralConstant(RealLiteralConstant.term, &RealLiteralConstant)) {
         // MATCHED RealLiteralConstant
         LiteralConstant->setRealLiteralConstant(RealLiteralConstant.newRealLiteralConstant());
         LiteralConstant->inheritPayload(LiteralConstant->getRealLiteralConstant());
      } else return ATfalse;

   // MATCHED LiteralConstant_RLC
   LiteralConstant->setOptionType(OFP::LiteralConstant::LiteralConstant_RLC);

   return ATtrue;
 }

   return ATfalse;
}

//========================================================================================
// R306 named-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_NamedConstant(ATerm term, OFP::NamedConstant* NamedConstant)
{
#ifdef DEBUG_PRINT
   printf("NamedConstant: %s\n", ATwriteToString(term));
#endif

 OFP::Name Name;
 if (ATmatch(term, "NamedConstant(<term>)", &Name.term)) {

      if (ofp_traverse_Name(Name.term, &Name)) {
         // MATCHED Name
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}


/**                                                                                             
 * Section/Clause 4: Types                                                                      
 */

//========================================================================================
// R401 type-param-value
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeParamValue(ATerm term, OFP::TypeParamValue* TypeParamValue)
{
#ifdef DEBUG_PRINT
   printf("TypeParamValue: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "TypeParamValue_COLON")) {

   // MATCHED TypeParamValue_COLON

   return ATtrue;
 }

 if (ATmatch(term, "TypeParamValue_STAR")) {

   // MATCHED TypeParamValue_STAR

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "TypeParamValue_E(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED TypeParamValue_E

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R402 type-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeSpec(ATerm term, OFP::TypeSpec* TypeSpec)
{
#ifdef DEBUG_PRINT
   printf("TypeSpec: %s\n", ATwriteToString(term));
#endif

 OFP::DerivedTypeSpec DerivedTypeSpec;
 if (ATmatch(term, "TypeSpec_DTS(<term>)", &DerivedTypeSpec.term)) {

      if (ofp_traverse_DerivedTypeSpec(DerivedTypeSpec.term, &DerivedTypeSpec)) {
         // MATCHED DerivedTypeSpec
      } else return ATfalse;

   // MATCHED TypeSpec_DTS

   return ATtrue;
 }

 OFP::IntrinsicTypeSpec IntrinsicTypeSpec;
 if (ATmatch(term, "TypeSpec_ITS(<term>)", &IntrinsicTypeSpec.term)) {

      if (ofp_traverse_IntrinsicTypeSpec(IntrinsicTypeSpec.term, &IntrinsicTypeSpec)) {
         // MATCHED IntrinsicTypeSpec
      } else return ATfalse;

   // MATCHED TypeSpec_ITS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R403 declaration-type-spec                                                                   
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DeclarationTypeSpec(ATerm term, OFP::DeclarationTypeSpec* DeclarationTypeSpec)
{
#ifdef DEBUG_PRINT
   printf("DeclarationTypeSpec: %s\n", ATwriteToString(term));
#endif

#ifdef TODO_ROSE
  IntrinsicTypeSpec                       -> DeclarationTypeSpec  {cons("IntrinsicType")}
  'TYPE'  '(' IntrinsicTypeSpec ')'       -> DeclarationTypeSpec  {cons("IntrinsicType")}
  'TYPE'  '(' DerivedTypeSpec   ')'       -> DeclarationTypeSpec  {cons("derived-type-spec")}  %% -> DerivedType
  'CLASS' '(' DerivedTypeSpec   ')'       -> DeclarationTypeSpec  {cons("class-type-spec")}    %% -> Class
  'CLASS' '(' '*'               ')'       -> DeclarationTypeSpec  {cons("AssumedClass")}
%%TR29113
  'TYPE'  '(' '*'               ')'       -> DeclarationTypeSpec  {cons("AssumedType")}
#endif

#ifdef TODO_ROSE
 if (ATmatch(term, "DeclarationTypeSpec_T_STAR")) {

   // MATCHED DeclarationTypeSpec_T_STAR
   DeclarationTypeSpec->setOptionType(OFP::DeclarationTypeSpec::DeclarationTypeSpec_T_STAR);
   ast->build_DeclarationTypeSpec(DeclarationTypeSpec);

   return ATtrue;
 }

 if (ATmatch(term, "DeclarationTypeSpec_C_STAR")) {

   // MATCHED DeclarationTypeSpec_C_STAR
   DeclarationTypeSpec->setOptionType(OFP::DeclarationTypeSpec::DeclarationTypeSpec_C_STAR);
   ast->build_DeclarationTypeSpec(DeclarationTypeSpec);

   return ATtrue;
 }

 OFP::DerivedTypeSpec DerivedTypeSpec;
 if (ATmatch(term, "DeclarationTypeSpec_C_DTS(<term>)", &DerivedTypeSpec.term)) {

      if (ofp_traverse_DerivedTypeSpec(DerivedTypeSpec.term, &DerivedTypeSpec)) {
         // MATCHED DerivedTypeSpec
         DeclarationTypeSpec->setDerivedTypeSpec(DerivedTypeSpec.newDerivedTypeSpec());
         DeclarationTypeSpec->inheritPayload(DeclarationTypeSpec->getDerivedTypeSpec());
      } else return ATfalse;

   // MATCHED DeclarationTypeSpec_C_DTS
   DeclarationTypeSpec->setOptionType(OFP::DeclarationTypeSpec::DeclarationTypeSpec_C_DTS);
   ast->build_DeclarationTypeSpec(DeclarationTypeSpec);

   return ATtrue;
 }

 OFP::DerivedTypeSpec DerivedTypeSpec1;
 if (ATmatch(term, "DeclarationTypeSpec_T_DTS(<term>)", &DerivedTypeSpec.term)) {

      if (ofp_traverse_DerivedTypeSpec(DerivedTypeSpec.term, &DerivedTypeSpec)) {
         // MATCHED DerivedTypeSpec
         DeclarationTypeSpec->setDerivedTypeSpec(DerivedTypeSpec.newDerivedTypeSpec());
         DeclarationTypeSpec->inheritPayload(DeclarationTypeSpec->getDerivedTypeSpec());
      } else return ATfalse;

   // MATCHED DeclarationTypeSpec_T_DTS
   DeclarationTypeSpec->setOptionType(OFP::DeclarationTypeSpec::DeclarationTypeSpec_T_DTS);
   ast->build_DeclarationTypeSpec(DeclarationTypeSpec);

   return ATtrue;
 }
#endif

   OFP::IntrinsicTypeSpec IntrinsicTypeSpec;
   if (ofp_traverse_IntrinsicTypeSpec(term, &IntrinsicTypeSpec)) {
      // MATCHED IntrinsicTypeSpec
      DeclarationTypeSpec->setIntrinsicTypeSpec(IntrinsicTypeSpec.newIntrinsicTypeSpec());
      DeclarationTypeSpec->inheritPayload(DeclarationTypeSpec->getIntrinsicTypeSpec());
      DeclarationTypeSpec->setOptionType(OFP::DeclarationTypeSpec::IntrinsicType);
      ast->build_DeclarationTypeSpec(DeclarationTypeSpec);

      return ATtrue;
   }

 return ATfalse;
}

//========================================================================================
// R404 intrinsic-type-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntrinsicTypeSpec(ATerm term, OFP::IntrinsicTypeSpec* IntrinsicTypeSpec)
{
#ifdef DEBUG_PRINT
   printf("IntrinsicTypeSpec(F): %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "IntrinsicTypeSpec_DBL_CMPLX")) {

   // MATCHED IntrinsicTypeSpec_DBL_CMPLX
   IntrinsicTypeSpec->setOptionType(OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_DBL_CMPLX);

   return ATtrue;
 }

   OFP::KindSelector KindSelector;
   if (ATmatch(term, "IntrinsicType(LOGICAL(<term>))", &KindSelector.term)) {
      // MATCHED LOGICAL
      IntrinsicTypeSpec->setOptionType(OFP::IntrinsicTypeSpec::LOGICAL);

      if (ofp_traverse_KindSelector(KindSelector.term, &KindSelector)) {
         // MATCHED KindSelector
         IntrinsicTypeSpec->setKindSelector(KindSelector.newKindSelector());
         IntrinsicTypeSpec->inheritPayload(IntrinsicTypeSpec->getKindSelector());
      } // Optional

      return ATtrue;
   }

 OFP::CharSelector CharSelector;
 if (ATmatch(term, "IntrinsicTypeSpec_CHAR(<term>)", &CharSelector.term)) {

   if (ATmatch(CharSelector.term, "Some(<term>)", &CharSelector.term)) {
      if (ofp_traverse_CharSelector(CharSelector.term, &CharSelector)) {
         // MATCHED CharSelector
         IntrinsicTypeSpec->setCharSelector(CharSelector.newCharSelector());
         IntrinsicTypeSpec->inheritPayload(IntrinsicTypeSpec->getCharSelector());
      } else return ATfalse;
   }

   // MATCHED IntrinsicTypeSpec_CHAR
   IntrinsicTypeSpec->setOptionType(OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_CHAR);

   return ATtrue;
 }

 OFP::KindSelector KindSelector1;
 if (ATmatch(term, "IntrinsicTypeSpec_CMPLX(<term>)", &KindSelector.term)) {

   if (ATmatch(KindSelector.term, "Some(<term>)", &KindSelector.term)) {
      if (ofp_traverse_KindSelector(KindSelector.term, &KindSelector)) {
         // MATCHED KindSelector
         IntrinsicTypeSpec->setKindSelector(KindSelector.newKindSelector());
         IntrinsicTypeSpec->inheritPayload(IntrinsicTypeSpec->getKindSelector());
      } else return ATfalse;
   }

   // MATCHED IntrinsicTypeSpec_CMPLX
   IntrinsicTypeSpec->setOptionType(OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_CMPLX);

   return ATtrue;
 }

 if (ATmatch(term, "IntrinsicTypeSpec_DBL_PREC")) {

   // MATCHED IntrinsicTypeSpec_DBL_PREC
   IntrinsicTypeSpec->setOptionType(OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_DBL_PREC);

   return ATtrue;
 }

 OFP::KindSelector KindSelector2;
 if (ATmatch(term, "IntrinsicTypeSpec_REAL(<term>)", &KindSelector.term)) {

   if (ATmatch(KindSelector.term, "Some(<term>)", &KindSelector.term)) {
      if (ofp_traverse_KindSelector(KindSelector.term, &KindSelector)) {
         // MATCHED KindSelector
         IntrinsicTypeSpec->setKindSelector(KindSelector.newKindSelector());
         IntrinsicTypeSpec->inheritPayload(IntrinsicTypeSpec->getKindSelector());
      } else return ATfalse;
   }

   // MATCHED IntrinsicTypeSpec_REAL
   IntrinsicTypeSpec->setOptionType(OFP::IntrinsicTypeSpec::IntrinsicTypeSpec_REAL);

   return ATtrue;
 }

   if (ATmatch(term, "IntrinsicType(INTEGER(<term>))", &KindSelector.term)) {
      // MATCHED INTEGER
      IntrinsicTypeSpec->setOptionType(OFP::IntrinsicTypeSpec::INTEGER);

      if (ofp_traverse_KindSelector(KindSelector.term, &KindSelector)) {
         // MATCHED KindSelector
         IntrinsicTypeSpec->setKindSelector(KindSelector.newKindSelector());
         IntrinsicTypeSpec->inheritPayload(IntrinsicTypeSpec->getKindSelector());
      } // Optional

      return ATtrue;
   }

   return ATfalse;
}

//========================================================================================
// R405 kind-selector
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_KindSelector(ATerm term, OFP::KindSelector* KindSelector)
{
#ifdef DEBUG_PRINT
   printf("KindSelector: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "KindSelector_STAR(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
         KindSelector->setIcon(Icon.newIcon());
      } else return ATfalse;

   // MATCHED KindSelector_STAR
   KindSelector->setOptionType(OFP::KindSelector::KindSelector_STAR);

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "KindSelector_E(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
         KindSelector->setExpr(Expr.newExpr());
      } else return ATfalse;

   // MATCHED KindSelector_E
   KindSelector->setOptionType(OFP::KindSelector::KindSelector_E);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R406 signed-int-literal-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SignedIntLiteralConstant(ATerm term, OFP::SignedIntLiteralConstant* SignedIntLiteralConstant)
{
#ifdef DEBUG_PRINT
   printf("SignedIntLiteralConstant: %s\n", ATwriteToString(term));
#endif

 OFP::Sign Sign;
 OFP::IntLiteralConstant IntLiteralConstant;
 if (ATmatch(term, "SignedIntLiteralConstant(<term>,<term>)", &Sign.term, &IntLiteralConstant.term)) {

   if (ATmatch(Sign.term, "Some(<term>)", &Sign.term)) {
      if (ofp_traverse_Sign(Sign.term, &Sign)) {
         // MATCHED Sign
      } else return ATfalse;
   }

      if (ofp_traverse_IntLiteralConstant(IntLiteralConstant.term, &IntLiteralConstant)) {
         // MATCHED IntLiteralConstant
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R407 int-literal-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntLiteralConstant(ATerm term, OFP::IntLiteralConstant* IntLiteralConstant)
{
#ifdef DEBUG_PRINT
   printf("IntLiteralConstant(F): %s\n", ATwriteToString(term));
#endif

   OFP::DigitString DigitString;
   OFP::KindParam KindParam;
   if (ATmatch(term, "IntLiteralConstant(<term>,<term>)", &DigitString.term, &KindParam.term)) {

      if (ofp_traverse_DigitString(DigitString.term, &DigitString)) {
         // MATCHED DigitString
         IntLiteralConstant->setDigitString(DigitString.newDigitString());
         IntLiteralConstant->inheritPayload(IntLiteralConstant->getDigitString());
      } else return ATfalse;

      if (ofp_traverse_KindParam(KindParam.term, &KindParam)) {
         // MATCHED KindParam
         IntLiteralConstant->setKindParam(KindParam.newKindParam());
      } else return ATfalse;

      ast->build_IntLiteralConstant(IntLiteralConstant);

      return ATtrue;
   }

   return ATfalse;
}

//========================================================================================
// R408 kind-param
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_KindParam(ATerm term, OFP::KindParam* KindParam)
{
#ifdef DEBUG_PRINT
   printf("KindParam(W): %s\n", ATwriteToString(term));
#endif

   OFP::DigitString DigitString;

   if (ofp_traverse_DigitString(term, &DigitString)) {
      // MATCHED DigitString
      KindParam->setOptionType(OFP::KindParam::DigitString_ot);
      KindParam->setDigitString(DigitString.newDigitString());
      KindParam->inheritPayload(KindParam->getDigitString());
      return ATtrue;
   }

 OFP::Name ScalarIntConstantName;
 if (ATmatch(term, "KindParam_SICN(<term>)", &ScalarIntConstantName.term)) {

      if (ofp_traverse_Name(ScalarIntConstantName.term, &ScalarIntConstantName)) {
         // MATCHED ScalarIntConstantName                                                                       
         KindParam->setScalarIntConstantName(ScalarIntConstantName.newName());
      } else return ATfalse;

   // MATCHED KindParam_SICN                                                                                    
   KindParam->setOptionType(OFP::KindParam::KindParam_SICN);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R410 digit-string
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DigitString(ATerm term, OFP::DigitString* DigitString)
{
#ifdef DEBUG_PRINT
   printf("DigitString(F): %s\n", ATwriteToString(term));
#endif

   OFP::Icon Icon;

   if (ofp_traverse_Icon(term, &Icon)) {
      // MATCHED Icon
      DigitString->setIcon(Icon.newIcon());
      DigitString->inheritPayload(DigitString->getIcon());
      return ATtrue;
   }

   return ATfalse;
}

//========================================================================================
// R411 sign
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Sign(ATerm term, OFP::Sign* Sign)
{
#ifdef DEBUG_PRINT
   printf("Sign: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "Sign_MINUS")) {

   // MATCHED Sign_MINUS

   return ATtrue;
 }

 if (ATmatch(term, "Sign_PLUS")) {

   // MATCHED Sign_PLUS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R412 signed-real-literal-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SignedRealLiteralConstant(ATerm term, OFP::SignedRealLiteralConstant* SignedRealLiteralConstant)
{
#ifdef DEBUG_PRINT
   printf("SignedRealLiteralConstant: %s\n", ATwriteToString(term));
#endif

 OFP::Sign Sign;
 OFP::RealLiteralConstant RealLiteralConstant;
 if (ATmatch(term, "SignedRealLiteralConstant(<term>,<term>)", &Sign.term, &RealLiteralConstant.term)) {

   if (ATmatch(Sign.term, "Some(<term>)", &Sign.term)) {
      if (ofp_traverse_Sign(Sign.term, &Sign)) {
         // MATCHED Sign
      } else return ATfalse;
   }

      if (ofp_traverse_RealLiteralConstant(RealLiteralConstant.term, &RealLiteralConstant)) {
         // MATCHED RealLiteralConstant
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R413 real-literal-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_RealLiteralConstant(ATerm term, OFP::RealLiteralConstant* RealLiteralConstant)
{
#ifdef DEBUG_PRINT
   printf("RealLiteralConstant: %s\n", ATwriteToString(term));
#endif

 OFP::Rcon Rcon;
 OFP::KindParam KindParam;
 if (ATmatch(term, "RealLiteralConstant(<term>,<term>)", &Rcon.term, &KindParam.term)) {

      if (ofp_traverse_Rcon(Rcon.term, &Rcon)) {
         // MATCHED Rcon
      } else return ATfalse;

   if (ATmatch(KindParam.term, "Some(<term>)", &KindParam.term)) {
   if (ATmatch(KindParam.term, "(<term>)", &KindParam.term)) {
      if (ofp_traverse_KindParam(KindParam.term, &KindParam)) {
         // MATCHED KindParam
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R417 complex-literal-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComplexLiteralConstant(ATerm term, OFP::ComplexLiteralConstant* ComplexLiteralConstant)
{
#ifdef DEBUG_PRINT
   printf("ComplexLiteralConstant: %s\n", ATwriteToString(term));
#endif

 OFP::RealPart RealPart;
 OFP::ImagPart ImagPart;
 if (ATmatch(term, "ComplexLiteralConstant(<term>,<term>)", &RealPart.term, &ImagPart.term)) {

      if (ofp_traverse_RealPart(RealPart.term, &RealPart)) {
         // MATCHED RealPart
      } else return ATfalse;

      if (ofp_traverse_ImagPart(ImagPart.term, &ImagPart)) {
         // MATCHED ImagPart
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R418 real-part
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_RealPart(ATerm term, OFP::RealPart* RealPart)
{
#ifdef DEBUG_PRINT
   printf("RealPart: %s\n", ATwriteToString(term));
#endif

 OFP::NamedConstant NamedConstant;
 if (ATmatch(term, "RealPart_NC(<term>)", &NamedConstant.term)) {

      if (ofp_traverse_NamedConstant(NamedConstant.term, &NamedConstant)) {
         // MATCHED NamedConstant
      } else return ATfalse;

   // MATCHED RealPart_NC

   return ATtrue;
 }

 OFP::SignedRealLiteralConstant SignedRealLiteralConstant;
 if (ATmatch(term, "RealPart_SRLC(<term>)", &SignedRealLiteralConstant.term)) {

      if (ofp_traverse_SignedRealLiteralConstant(SignedRealLiteralConstant.term, &SignedRealLiteralConstant)) {
         // MATCHED SignedRealLiteralConstant
      } else return ATfalse;

   // MATCHED RealPart_SRLC

   return ATtrue;
 }

 OFP::SignedIntLiteralConstant SignedIntLiteralConstant;
 if (ATmatch(term, "RealPart_SILC(<term>)", &SignedIntLiteralConstant.term)) {

      if (ofp_traverse_SignedIntLiteralConstant(SignedIntLiteralConstant.term, &SignedIntLiteralConstant)) {
         // MATCHED SignedIntLiteralConstant
      } else return ATfalse;

   // MATCHED RealPart_SILC

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R419 imag-part
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ImagPart(ATerm term, OFP::ImagPart* ImagPart)
{
#ifdef DEBUG_PRINT
   printf("ImagPart: %s\n", ATwriteToString(term));
#endif

 OFP::NamedConstant NamedConstant;
 if (ATmatch(term, "ImagPart_NC(<term>)", &NamedConstant.term)) {

      if (ofp_traverse_NamedConstant(NamedConstant.term, &NamedConstant)) {
         // MATCHED NamedConstant
      } else return ATfalse;

   // MATCHED ImagPart_NC

   return ATtrue;
 }

 OFP::SignedRealLiteralConstant SignedRealLiteralConstant;
 if (ATmatch(term, "ImagPart_SRLC(<term>)", &SignedRealLiteralConstant.term)) {

      if (ofp_traverse_SignedRealLiteralConstant(SignedRealLiteralConstant.term, &SignedRealLiteralConstant)) {
         // MATCHED SignedRealLiteralConstant
      } else return ATfalse;

   // MATCHED ImagPart_SRLC

   return ATtrue;
 }

 OFP::SignedIntLiteralConstant SignedIntLiteralConstant;
 if (ATmatch(term, "ImagPart_SILC(<term>)", &SignedIntLiteralConstant.term)) {

      if (ofp_traverse_SignedIntLiteralConstant(SignedIntLiteralConstant.term, &SignedIntLiteralConstant)) {
         // MATCHED SignedIntLiteralConstant
      } else return ATfalse;

   // MATCHED ImagPart_SILC

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R420 char-selector
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CharSelector(ATerm term, OFP::CharSelector* CharSelector)
{
#ifdef DEBUG_PRINT
   printf("CharSelector: %s\n", ATwriteToString(term));
#endif

 OFP::Expr Expr;
 if (ATmatch(term, "CharSelector_KIND(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED CharSelector_KIND

   return ATtrue;
 }

 OFP::Expr Expr1;
 OFP::TypeParamValue TypeParamValue;
 if (ATmatch(term, "CharSelector_KIND_LEN(<term>,<term>)", &Expr1.term, &TypeParamValue.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
      } else return ATfalse;

      if (ofp_traverse_TypeParamValue(TypeParamValue.term, &TypeParamValue)) {
         // MATCHED TypeParamValue
      } else return ATfalse;

   // MATCHED CharSelector_KIND_LEN

   return ATtrue;
 }

 OFP::TypeParamValue TypeParamValue1;
 OFP::Expr Expr2;
 if (ATmatch(term, "CharSelector_LEN_KIND(<term>,<term>)", &TypeParamValue1.term, &Expr2.term)) {

      if (ofp_traverse_TypeParamValue(TypeParamValue1.term, &TypeParamValue1)) {
         // MATCHED TypeParamValue
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED CharSelector_LEN_KIND

   return ATtrue;
 }

 OFP::LengthSelector LengthSelector;
 if (ATmatch(term, "CharSelector_LS(<term>)", &LengthSelector.term)) {

      if (ofp_traverse_LengthSelector(LengthSelector.term, &LengthSelector)) {
         // MATCHED LengthSelector
      } else return ATfalse;

   // MATCHED CharSelector_LS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R421 length-selector
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LengthSelector(ATerm term, OFP::LengthSelector* LengthSelector)
{
#ifdef DEBUG_PRINT
   printf("LengthSelector: %s\n", ATwriteToString(term));
#endif

 OFP::CharLength CharLength;
 if (ATmatch(term, "LengthSelector_STAR(<term>)", &CharLength.term)) {

      if (ofp_traverse_CharLength(CharLength.term, &CharLength)) {
         // MATCHED CharLength
      } else return ATfalse;

   // MATCHED LengthSelector_STAR

   return ATtrue;
 }

 OFP::TypeParamValue TypeParamValue;
 if (ATmatch(term, "LengthSelector_LEN(<term>)", &TypeParamValue.term)) {

      if (ofp_traverse_TypeParamValue(TypeParamValue.term, &TypeParamValue)) {
         // MATCHED TypeParamValue
      } else return ATfalse;

   // MATCHED LengthSelector_LEN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R422 char-length
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CharLength(ATerm term, OFP::CharLength* CharLength)
{
#ifdef DEBUG_PRINT
   printf("CharLength: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "CharLength_I(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED CharLength_I

   return ATtrue;
 }

 OFP::TypeParamValue TypeParamValue;
 if (ATmatch(term, "CharLength_TPV(<term>)", &TypeParamValue.term)) {

      if (ofp_traverse_TypeParamValue(TypeParamValue.term, &TypeParamValue)) {
         // MATCHED TypeParamValue
      } else return ATfalse;

   // MATCHED CharLength_TPV

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R423 char-literal-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CharLiteralConstant(ATerm term, OFP::CharLiteralConstant* CharLiteralConstant)
{
#ifdef DEBUG_PRINT
   printf("CharLiteralConstant: %s\n", ATwriteToString(term));
#endif

 OFP::KindParam KindParam;
 OFP::Scon Scon;
 if (ATmatch(term, "CharLiteralConstant(<term>,<term>)", &KindParam.term, &Scon.term)) {

   if (ATmatch(KindParam.term, "Some(<term>)", &KindParam.term)) {
   if (ATmatch(KindParam.term, "(<term>)", &KindParam.term)) {
      if (ofp_traverse_KindParam(KindParam.term, &KindParam)) {
         // MATCHED KindParam
      } else return ATfalse;
   }
   }

      if (ofp_traverse_Scon(Scon.term, &Scon)) {
         // MATCHED Scon
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R424 logical-literal-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LogicalLiteralConstant(ATerm term, OFP::LogicalLiteralConstant* LogicalLiteralConstant)
{
#ifdef DEBUG_PRINT
   printf("LogicalLiteralConstant: %s\n", ATwriteToString(term));
#endif

 OFP::KindParam KindParam;
 if (ATmatch(term, "LogicalLiteralConstant_FALSE(<term>)", &KindParam.term)) {

   if (ATmatch(KindParam.term, "Some(<term>)", &KindParam.term)) {
   if (ATmatch(KindParam.term, "(<term>)", &KindParam.term)) {
      if (ofp_traverse_KindParam(KindParam.term, &KindParam)) {
         // MATCHED KindParam
      } else return ATfalse;
   }
   }

   // MATCHED LogicalLiteralConstant_FALSE

   return ATtrue;
 }

 OFP::KindParam KindParam1;
 if (ATmatch(term, "LogicalLiteralConstant_TRUE(<term>)", &KindParam1.term)) {

   if (ATmatch(KindParam1.term, "Some(<term>)", &KindParam1.term)) {
   if (ATmatch(KindParam1.term, "(<term>)", &KindParam1.term)) {
      if (ofp_traverse_KindParam(KindParam1.term, &KindParam1)) {
         // MATCHED KindParam
      } else return ATfalse;
   }
   }

   // MATCHED LogicalLiteralConstant_TRUE

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R425 derived-type-def
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DerivedTypeDef(ATerm term, OFP::DerivedTypeDef* DerivedTypeDef)
{
#ifdef DEBUG_PRINT
   printf("DerivedTypeDef: %s\n", ATwriteToString(term));
#endif

 OFP::DerivedTypeStmt DerivedTypeStmt;
 OFP::TypeParamDefStmt TypeParamDefStmt;
 OFP::PrivateOrSequence PrivateOrSequence;
 OFP::ComponentPart ComponentPart;
 OFP::TypeBoundProcedurePart TypeBoundProcedurePart;
 OFP::EndTypeStmt EndTypeStmt;
 if (ATmatch(term, "DerivedTypeDef(<term>,<term>,<term>,<term>,<term>,<term>)", &DerivedTypeStmt.term, &TypeParamDefStmt.term, &PrivateOrSequence.term, &ComponentPart.term, &TypeBoundProcedurePart.term, &EndTypeStmt.term)) {

      if (ofp_traverse_DerivedTypeStmt(DerivedTypeStmt.term, &DerivedTypeStmt)) {
         // MATCHED DerivedTypeStmt
      } else return ATfalse;

   ATermList TypeParamDefStmt_tail = (ATermList) ATmake("<term>", TypeParamDefStmt.term);
   while (! ATisEmpty(TypeParamDefStmt_tail)) {
      TypeParamDefStmt.term = ATgetFirst(TypeParamDefStmt_tail);
      TypeParamDefStmt_tail = ATgetNext (TypeParamDefStmt_tail);
      if (ofp_traverse_TypeParamDefStmt(TypeParamDefStmt.term, &TypeParamDefStmt)) {
         // MATCHED TypeParamDefStmt
      } else return ATfalse;
   }

   ATermList PrivateOrSequence_tail = (ATermList) ATmake("<term>", PrivateOrSequence.term);
   while (! ATisEmpty(PrivateOrSequence_tail)) {
      PrivateOrSequence.term = ATgetFirst(PrivateOrSequence_tail);
      PrivateOrSequence_tail = ATgetNext (PrivateOrSequence_tail);
      if (ofp_traverse_PrivateOrSequence(PrivateOrSequence.term, &PrivateOrSequence)) {
         // MATCHED PrivateOrSequence
      } else return ATfalse;
   }

   if (ATmatch(ComponentPart.term, "Some(<term>)", &ComponentPart.term)) {
      if (ofp_traverse_ComponentPart(ComponentPart.term, &ComponentPart)) {
         // MATCHED ComponentPart
      } else return ATfalse;
   }

   if (ATmatch(TypeBoundProcedurePart.term, "Some(<term>)", &TypeBoundProcedurePart.term)) {
      if (ofp_traverse_TypeBoundProcedurePart(TypeBoundProcedurePart.term, &TypeBoundProcedurePart)) {
         // MATCHED TypeBoundProcedurePart
      } else return ATfalse;
   }

      if (ofp_traverse_EndTypeStmt(EndTypeStmt.term, &EndTypeStmt)) {
         // MATCHED EndTypeStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R426 derived-type-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DerivedTypeStmt(ATerm term, OFP::DerivedTypeStmt* DerivedTypeStmt)
{
#ifdef DEBUG_PRINT
   printf("DerivedTypeStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::OptTypeAttrSpecList OptTypeAttrSpecList;
 OFP::Name TypeName;
 OFP::OptTypeParamNameList OptTypeParamNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "DerivedTypeStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &OptTypeAttrSpecList.term, &TypeName.term, &OptTypeParamNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(OptTypeAttrSpecList.term, "Some(<term>)", &OptTypeAttrSpecList.term)) {
      if (ofp_traverse_OptTypeAttrSpecList(OptTypeAttrSpecList.term, &OptTypeAttrSpecList)) {
         // MATCHED OptTypeAttrSpecList
      } else return ATfalse;
   }

      if (ofp_traverse_Name(TypeName.term, &TypeName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(OptTypeParamNameList.term, "Some(<term>)", &OptTypeParamNameList.term)) {
      if (ofp_traverse_OptTypeParamNameList(OptTypeParamNameList.term, &OptTypeParamNameList)) {
         // MATCHED OptTypeParamNameList
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_OptTypeAttrSpecList(ATerm term, OFP::OptTypeAttrSpecList* OptTypeAttrSpecList)
{
#ifdef DEBUG_PRINT
   printf("OptTypeAttrSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::TypeAttrSpecList TypeAttrSpecList;
 if (ATmatch(term, "OptTASL(<term>)", &TypeAttrSpecList.term)) {

   if (ATmatch(TypeAttrSpecList.term, "Some(<term>)", &TypeAttrSpecList.term)) {
   if (ATmatch(TypeAttrSpecList.term, "(<term>)", &TypeAttrSpecList.term)) {
      if (ofp_traverse_TypeAttrSpecList(TypeAttrSpecList.term, &TypeAttrSpecList)) {
         // MATCHED TypeAttrSpecList
      } else return ATfalse;
   }
   }

   // MATCHED OptTASL

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_OptTypeParamNameList(ATerm term, OFP::OptTypeParamNameList* OptTypeParamNameList)
{
#ifdef DEBUG_PRINT
   printf("OptTypeParamNameList: %s\n", ATwriteToString(term));
#endif

 OFP::TypeParamNameList TypeParamNameList;
 if (ATmatch(term, "OptTPNL(<term>)", &TypeParamNameList.term)) {

      if (ofp_traverse_TypeParamNameList(TypeParamNameList.term, &TypeParamNameList)) {
         // MATCHED TypeParamNameList
      } else return ATfalse;

   // MATCHED OptTPNL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R427 type-attr-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeAttrSpec(ATerm term, OFP::TypeAttrSpec* TypeAttrSpec)
{
#ifdef DEBUG_PRINT
   printf("TypeAttrSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Name ParentTypeName;
 if (ATmatch(term, "TypeAttrSpec_EXTENDS(<term>)", &ParentTypeName.term)) {

      if (ofp_traverse_Name(ParentTypeName.term, &ParentTypeName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED TypeAttrSpec_EXTENDS

   return ATtrue;
 }

 if (ATmatch(term, "TypeAttrSpec_BIND")) {

   // MATCHED TypeAttrSpec_BIND

   return ATtrue;
 }

 OFP::AccessSpec AccessSpec;
 if (ATmatch(term, "TypeAttrSpec_AS(<term>)", &AccessSpec.term)) {

      if (ofp_traverse_AccessSpec(AccessSpec.term, &AccessSpec)) {
         // MATCHED AccessSpec
      } else return ATfalse;

   // MATCHED TypeAttrSpec_AS

   return ATtrue;
 }

 if (ATmatch(term, "TypeAttrSpec_ABSTRACT")) {

   // MATCHED TypeAttrSpec_ABSTRACT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_TypeAttrSpecList(ATerm term, OFP::TypeAttrSpecList* TypeAttrSpecList)
{
#ifdef DEBUG_PRINT
   printf("TypeAttrSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::TypeAttrSpec TypeAttrSpec;
 if (ATmatch(term, "TypeAttrSpecList(<term>)", &TypeAttrSpec.term)) {

   ATermList TypeAttrSpec_tail = (ATermList) ATmake("<term>", TypeAttrSpec.term);
   while (! ATisEmpty(TypeAttrSpec_tail)) {
      TypeAttrSpec.term = ATgetFirst(TypeAttrSpec_tail);
      TypeAttrSpec_tail = ATgetNext (TypeAttrSpec_tail);
      if (ofp_traverse_TypeAttrSpec(TypeAttrSpec.term, &TypeAttrSpec)) {
         // MATCHED TypeAttrSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_TypeParamNameList(ATerm term, OFP::TypeParamNameList* TypeParamNameList)
{
#ifdef DEBUG_PRINT
   printf("TypeParamNameList: %s\n", ATwriteToString(term));
#endif

 OFP::Name TypeParamName;
 if (ATmatch(term, "TypeParamNameList(<term>)", &TypeParamName.term)) {

   ATermList TypeParamName_tail = (ATermList) ATmake("<term>", TypeParamName.term);
   while (! ATisEmpty(TypeParamName_tail)) {
      TypeParamName.term = ATgetFirst(TypeParamName_tail);
      TypeParamName_tail = ATgetNext (TypeParamName_tail);
      if (ofp_traverse_Name(TypeParamName.term, &TypeParamName)) {
         // MATCHED Name
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R428 private-or-sequence
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PrivateOrSequence(ATerm term, OFP::PrivateOrSequence* PrivateOrSequence)
{
#ifdef DEBUG_PRINT
   printf("PrivateOrSequence: %s\n", ATwriteToString(term));
#endif

 OFP::SequenceStmt SequenceStmt;
 if (ATmatch(term, "PrivateOrSequence_SS(<term>)", &SequenceStmt.term)) {

      if (ofp_traverse_SequenceStmt(SequenceStmt.term, &SequenceStmt)) {
         // MATCHED SequenceStmt
      } else return ATfalse;

   // MATCHED PrivateOrSequence_SS

   return ATtrue;
 }

 OFP::PrivateComponentsStmt PrivateComponentsStmt;
 if (ATmatch(term, "PrivateOrSequence_PCS(<term>)", &PrivateComponentsStmt.term)) {

      if (ofp_traverse_PrivateComponentsStmt(PrivateComponentsStmt.term, &PrivateComponentsStmt)) {
         // MATCHED PrivateComponentsStmt
      } else return ATfalse;

   // MATCHED PrivateOrSequence_PCS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R429 end-type-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndTypeStmt(ATerm term, OFP::EndTypeStmt* EndTypeStmt)
{
#ifdef DEBUG_PRINT
   printf("EndTypeStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name TypeName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndTypeStmt(<term>,<term>,<term>)", &Label.term, &TypeName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(TypeName.term, "Some(<term>)", &TypeName.term)) {
      if (ofp_traverse_Name(TypeName.term, &TypeName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R430 sequence-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SequenceStmt(ATerm term, OFP::SequenceStmt* SequenceStmt)
{
#ifdef DEBUG_PRINT
   printf("SequenceStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "SequenceStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R431 type-param-def-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeParamDefStmt(ATerm term, OFP::TypeParamDefStmt* TypeParamDefStmt)
{
#ifdef DEBUG_PRINT
   printf("TypeParamDefStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::KindSelector KindSelector;
 OFP::TypeParamAttrSpec TypeParamAttrSpec;
 OFP::TypeParamDeclList TypeParamDeclList;
 OFP::EOS EOS;
 if (ATmatch(term, "TypeParamDefStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &KindSelector.term, &TypeParamAttrSpec.term, &TypeParamDeclList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(KindSelector.term, "Some(<term>)", &KindSelector.term)) {
      if (ofp_traverse_KindSelector(KindSelector.term, &KindSelector)) {
         // MATCHED KindSelector
      } else return ATfalse;
   }

      if (ofp_traverse_TypeParamAttrSpec(TypeParamAttrSpec.term, &TypeParamAttrSpec)) {
         // MATCHED TypeParamAttrSpec
      } else return ATfalse;

      if (ofp_traverse_TypeParamDeclList(TypeParamDeclList.term, &TypeParamDeclList)) {
         // MATCHED TypeParamDeclList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R432 type-param-decl
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeParamDecl(ATerm term, OFP::TypeParamDecl* TypeParamDecl)
{
#ifdef DEBUG_PRINT
   printf("TypeParamDecl: %s\n", ATwriteToString(term));
#endif

 OFP::Name TypeParamName;
 OFP::Expr Expr;
 if (ATmatch(term, "TypeParamDecl(<term>,<term>)", &TypeParamName.term, &Expr.term)) {

      if (ofp_traverse_Name(TypeParamName.term, &TypeParamName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(Expr.term, "Some(<term>)", &Expr.term)) {
   if (ATmatch(Expr.term, "(<term>)", &Expr.term)) {
      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_TypeParamDeclList(ATerm term, OFP::TypeParamDeclList* TypeParamDeclList)
{
#ifdef DEBUG_PRINT
   printf("TypeParamDeclList: %s\n", ATwriteToString(term));
#endif

 OFP::TypeParamDecl TypeParamDecl;
 if (ATmatch(term, "TypeParamDeclList(<term>)", &TypeParamDecl.term)) {

   ATermList TypeParamDecl_tail = (ATermList) ATmake("<term>", TypeParamDecl.term);
   while (! ATisEmpty(TypeParamDecl_tail)) {
      TypeParamDecl.term = ATgetFirst(TypeParamDecl_tail);
      TypeParamDecl_tail = ATgetNext (TypeParamDecl_tail);
      if (ofp_traverse_TypeParamDecl(TypeParamDecl.term, &TypeParamDecl)) {
         // MATCHED TypeParamDecl
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R433 type-param-attr-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeParamAttrSpec(ATerm term, OFP::TypeParamAttrSpec* TypeParamAttrSpec)
{
#ifdef DEBUG_PRINT
   printf("TypeParamAttrSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "TypeParamAttrSpec_LEN")) {

   // MATCHED TypeParamAttrSpec_LEN

   return ATtrue;
 }

 if (ATmatch(term, "TypeParamAttrSpec_KIND")) {

   // MATCHED TypeParamAttrSpec_KIND

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R434 component-part
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComponentPart(ATerm term, OFP::ComponentPart* ComponentPart)
{
#ifdef DEBUG_PRINT
   printf("ComponentPart: %s\n", ATwriteToString(term));
#endif

 OFP::ComponentDefStmt ComponentDefStmt;
 if (ATmatch(term, "ComponentPart(<term>)", &ComponentDefStmt.term)) {

   ATermList ComponentDefStmt_tail = (ATermList) ATmake("<term>", ComponentDefStmt.term);
   while (! ATisEmpty(ComponentDefStmt_tail)) {
      ComponentDefStmt.term = ATgetFirst(ComponentDefStmt_tail);
      ComponentDefStmt_tail = ATgetNext (ComponentDefStmt_tail);
      if (ofp_traverse_ComponentDefStmt(ComponentDefStmt.term, &ComponentDefStmt)) {
         // MATCHED ComponentDefStmt
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R435 component-def-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComponentDefStmt(ATerm term, OFP::ComponentDefStmt* ComponentDefStmt)
{
#ifdef DEBUG_PRINT
   printf("ComponentDefStmt: %s\n", ATwriteToString(term));
#endif

 OFP::ProcComponentDefStmt ProcComponentDefStmt;
 if (ATmatch(term, "ComponentDefStmt_PCDS(<term>)", &ProcComponentDefStmt.term)) {

      if (ofp_traverse_ProcComponentDefStmt(ProcComponentDefStmt.term, &ProcComponentDefStmt)) {
         // MATCHED ProcComponentDefStmt
      } else return ATfalse;

   // MATCHED ComponentDefStmt_PCDS

   return ATtrue;
 }

 OFP::DataComponentDefStmt DataComponentDefStmt;
 if (ATmatch(term, "ComponentDefStmt_DCDS(<term>)", &DataComponentDefStmt.term)) {

      if (ofp_traverse_DataComponentDefStmt(DataComponentDefStmt.term, &DataComponentDefStmt)) {
         // MATCHED DataComponentDefStmt
      } else return ATfalse;

   // MATCHED ComponentDefStmt_DCDS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R436 data-component-def-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataComponentDefStmt(ATerm term, OFP::DataComponentDefStmt* DataComponentDefStmt)
{
#ifdef DEBUG_PRINT
   printf("DataComponentDefStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::DeclarationTypeSpec DeclarationTypeSpec;
 OFP::OptComponentAttrSpecList OptComponentAttrSpecList;
 OFP::ComponentDeclList ComponentDeclList;
 OFP::EOS EOS;
 if (ATmatch(term, "DataComponentDefStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &DeclarationTypeSpec.term, &OptComponentAttrSpecList.term, &ComponentDeclList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_DeclarationTypeSpec(DeclarationTypeSpec.term, &DeclarationTypeSpec)) {
         // MATCHED DeclarationTypeSpec
      } else return ATfalse;

   if (ATmatch(OptComponentAttrSpecList.term, "Some(<term>)", &OptComponentAttrSpecList.term)) {
      if (ofp_traverse_OptComponentAttrSpecList(OptComponentAttrSpecList.term, &OptComponentAttrSpecList)) {
         // MATCHED OptComponentAttrSpecList
      } else return ATfalse;
   }

      if (ofp_traverse_ComponentDeclList(ComponentDeclList.term, &ComponentDeclList)) {
         // MATCHED ComponentDeclList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_OptComponentAttrSpecList(ATerm term, OFP::OptComponentAttrSpecList* OptComponentAttrSpecList)
{
#ifdef DEBUG_PRINT
   printf("OptComponentAttrSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ComponentAttrSpecList ComponentAttrSpecList;
 if (ATmatch(term, "OptCASL(<term>)", &ComponentAttrSpecList.term)) {

   if (ATmatch(ComponentAttrSpecList.term, "Some(<term>)", &ComponentAttrSpecList.term)) {
   if (ATmatch(ComponentAttrSpecList.term, "(<term>)", &ComponentAttrSpecList.term)) {
      if (ofp_traverse_ComponentAttrSpecList(ComponentAttrSpecList.term, &ComponentAttrSpecList)) {
         // MATCHED ComponentAttrSpecList
      } else return ATfalse;
   }
   }

   // MATCHED OptCASL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R437 component-attr-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComponentAttrSpec(ATerm term, OFP::ComponentAttrSpec* ComponentAttrSpec)
{
#ifdef DEBUG_PRINT
   printf("ComponentAttrSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "ComponentAttrSpec_POINTER")) {

   // MATCHED ComponentAttrSpec_POINTER

   return ATtrue;
 }

 OFP::ComponentArraySpec ComponentArraySpec;
 if (ATmatch(term, "ComponentAttrSpec_DIMENSION(<term>)", &ComponentArraySpec.term)) {

      if (ofp_traverse_ComponentArraySpec(ComponentArraySpec.term, &ComponentArraySpec)) {
         // MATCHED ComponentArraySpec
      } else return ATfalse;

   // MATCHED ComponentAttrSpec_DIMENSION

   return ATtrue;
 }

 if (ATmatch(term, "ComponentAttrSpec_CONTIGUOUS")) {

   // MATCHED ComponentAttrSpec_CONTIGUOUS

   return ATtrue;
 }

 OFP::CoarraySpec CoarraySpec;
 if (ATmatch(term, "ComponentAttrSpec_CODIMENSION(<term>)", &CoarraySpec.term)) {

      if (ofp_traverse_CoarraySpec(CoarraySpec.term, &CoarraySpec)) {
         // MATCHED CoarraySpec
      } else return ATfalse;

   // MATCHED ComponentAttrSpec_CODIMENSION

   return ATtrue;
 }

 if (ATmatch(term, "ComponentAttrSpec_ALLOCATABLE")) {

   // MATCHED ComponentAttrSpec_ALLOCATABLE

   return ATtrue;
 }

 OFP::AccessSpec AccessSpec;
 if (ATmatch(term, "ComponentAttrSpec_AS(<term>)", &AccessSpec.term)) {

      if (ofp_traverse_AccessSpec(AccessSpec.term, &AccessSpec)) {
         // MATCHED AccessSpec
      } else return ATfalse;

   // MATCHED ComponentAttrSpec_AS

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ComponentAttrSpecList(ATerm term, OFP::ComponentAttrSpecList* ComponentAttrSpecList)
{
#ifdef DEBUG_PRINT
   printf("ComponentAttrSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ComponentAttrSpec ComponentAttrSpec;
 if (ATmatch(term, "ComponentAttrSpecList(<term>)", &ComponentAttrSpec.term)) {

   ATermList ComponentAttrSpec_tail = (ATermList) ATmake("<term>", ComponentAttrSpec.term);
   while (! ATisEmpty(ComponentAttrSpec_tail)) {
      ComponentAttrSpec.term = ATgetFirst(ComponentAttrSpec_tail);
      ComponentAttrSpec_tail = ATgetNext (ComponentAttrSpec_tail);
      if (ofp_traverse_ComponentAttrSpec(ComponentAttrSpec.term, &ComponentAttrSpec)) {
         // MATCHED ComponentAttrSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R438 component-decl
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComponentDecl(ATerm term, OFP::ComponentDecl* ComponentDecl)
{
#ifdef DEBUG_PRINT
   printf("ComponentDecl: %s\n", ATwriteToString(term));
#endif

 OFP::Name ComponentName;
 OFP::ComponentArraySpec ComponentArraySpec;
 OFP::CoarraySpec CoarraySpec;
 OFP::CharLength CharLength;
 OFP::ComponentInitialization ComponentInitialization;
 if (ATmatch(term, "ComponentDecl(<term>,<term>,<term>,<term>,<term>)", &ComponentName.term, &ComponentArraySpec.term, &CoarraySpec.term, &CharLength.term, &ComponentInitialization.term)) {

      if (ofp_traverse_Name(ComponentName.term, &ComponentName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(ComponentArraySpec.term, "Some(<term>)", &ComponentArraySpec.term)) {
   if (ATmatch(ComponentArraySpec.term, "(<term>)", &ComponentArraySpec.term)) {
      if (ofp_traverse_ComponentArraySpec(ComponentArraySpec.term, &ComponentArraySpec)) {
         // MATCHED ComponentArraySpec
      } else return ATfalse;
   }
   }

   if (ATmatch(CoarraySpec.term, "Some(<term>)", &CoarraySpec.term)) {
   if (ATmatch(CoarraySpec.term, "(<term>)", &CoarraySpec.term)) {
      if (ofp_traverse_CoarraySpec(CoarraySpec.term, &CoarraySpec)) {
         // MATCHED CoarraySpec
      } else return ATfalse;
   }
   }

   if (ATmatch(CharLength.term, "Some(<term>)", &CharLength.term)) {
   if (ATmatch(CharLength.term, "(<term>)", &CharLength.term)) {
      if (ofp_traverse_CharLength(CharLength.term, &CharLength)) {
         // MATCHED CharLength
      } else return ATfalse;
   }
   }

   if (ATmatch(ComponentInitialization.term, "Some(<term>)", &ComponentInitialization.term)) {
      if (ofp_traverse_ComponentInitialization(ComponentInitialization.term, &ComponentInitialization)) {
         // MATCHED ComponentInitialization
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ComponentDeclList(ATerm term, OFP::ComponentDeclList* ComponentDeclList)
{
#ifdef DEBUG_PRINT
   printf("ComponentDeclList: %s\n", ATwriteToString(term));
#endif

 OFP::ComponentDecl ComponentDecl;
 if (ATmatch(term, "ComponentDeclList(<term>)", &ComponentDecl.term)) {

   ATermList ComponentDecl_tail = (ATermList) ATmake("<term>", ComponentDecl.term);
   while (! ATisEmpty(ComponentDecl_tail)) {
      ComponentDecl.term = ATgetFirst(ComponentDecl_tail);
      ComponentDecl_tail = ATgetNext (ComponentDecl_tail);
      if (ofp_traverse_ComponentDecl(ComponentDecl.term, &ComponentDecl)) {
         // MATCHED ComponentDecl
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R439 component-array-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComponentArraySpec(ATerm term, OFP::ComponentArraySpec* ComponentArraySpec)
{
#ifdef DEBUG_PRINT
   printf("ComponentArraySpec: %s\n", ATwriteToString(term));
#endif

 OFP::DeferredShapeSpecList DeferredShapeSpecList;
 if (ATmatch(term, "ComponentArraySpec_DSSL(<term>)", &DeferredShapeSpecList.term)) {

      if (ofp_traverse_DeferredShapeSpecList(DeferredShapeSpecList.term, &DeferredShapeSpecList)) {
         // MATCHED DeferredShapeSpecList
      } else return ATfalse;

   // MATCHED ComponentArraySpec_DSSL

   return ATtrue;
 }

 OFP::ExplicitShapeSpecList ExplicitShapeSpecList;
 if (ATmatch(term, "ComponentArraySpec_ESSL(<term>)", &ExplicitShapeSpecList.term)) {

      if (ofp_traverse_ExplicitShapeSpecList(ExplicitShapeSpecList.term, &ExplicitShapeSpecList)) {
         // MATCHED ExplicitShapeSpecList
      } else return ATfalse;

   // MATCHED ComponentArraySpec_ESSL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R440 proc-component-def-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcComponentDefStmt(ATerm term, OFP::ProcComponentDefStmt* ProcComponentDefStmt)
{
#ifdef DEBUG_PRINT
   printf("ProcComponentDefStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ProcInterface ProcInterface;
 OFP::ProcComponentAttrSpecList ProcComponentAttrSpecList;
 OFP::ProcDeclList ProcDeclList;
 OFP::EOS EOS;
 if (ATmatch(term, "ProcComponentDefStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &ProcInterface.term, &ProcComponentAttrSpecList.term, &ProcDeclList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(ProcInterface.term, "Some(<term>)", &ProcInterface.term)) {
      if (ofp_traverse_ProcInterface(ProcInterface.term, &ProcInterface)) {
         // MATCHED ProcInterface
      } else return ATfalse;
   }

      if (ofp_traverse_ProcComponentAttrSpecList(ProcComponentAttrSpecList.term, &ProcComponentAttrSpecList)) {
         // MATCHED ProcComponentAttrSpecList
      } else return ATfalse;

      if (ofp_traverse_ProcDeclList(ProcDeclList.term, &ProcDeclList)) {
         // MATCHED ProcDeclList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R441 proc-component-attr-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcComponentAttrSpec(ATerm term, OFP::ProcComponentAttrSpec* ProcComponentAttrSpec)
{
#ifdef DEBUG_PRINT
   printf("ProcComponentAttrSpec: %s\n", ATwriteToString(term));
#endif

 OFP::AccessSpec AccessSpec;
 if (ATmatch(term, "ProcComponentAttrSpec_AS(<term>)", &AccessSpec.term)) {

      if (ofp_traverse_AccessSpec(AccessSpec.term, &AccessSpec)) {
         // MATCHED AccessSpec
      } else return ATfalse;

   // MATCHED ProcComponentAttrSpec_AS

   return ATtrue;
 }

 if (ATmatch(term, "ProcComponentAttrSpec_NOPASS")) {

   // MATCHED ProcComponentAttrSpec_NOPASS

   return ATtrue;
 }

 if (ATmatch(term, "ProcComponentAttrSpec_PASS")) {

   // MATCHED ProcComponentAttrSpec_PASS

   return ATtrue;
 }

 if (ATmatch(term, "ProcComponentAttrSpec_POINTER")) {

   // MATCHED ProcComponentAttrSpec_POINTER

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ProcComponentAttrSpecList(ATerm term, OFP::ProcComponentAttrSpecList* ProcComponentAttrSpecList)
{
#ifdef DEBUG_PRINT
   printf("ProcComponentAttrSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ProcComponentAttrSpec ProcComponentAttrSpec;
 if (ATmatch(term, "ProcComponentAttrSpecList(<term>)", &ProcComponentAttrSpec.term)) {

   ATermList ProcComponentAttrSpec_tail = (ATermList) ATmake("<term>", ProcComponentAttrSpec.term);
   while (! ATisEmpty(ProcComponentAttrSpec_tail)) {
      ProcComponentAttrSpec.term = ATgetFirst(ProcComponentAttrSpec_tail);
      ProcComponentAttrSpec_tail = ATgetNext (ProcComponentAttrSpec_tail);
      if (ofp_traverse_ProcComponentAttrSpec(ProcComponentAttrSpec.term, &ProcComponentAttrSpec)) {
         // MATCHED ProcComponentAttrSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R442 component-initialization
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComponentInitialization(ATerm term, OFP::ComponentInitialization* ComponentInitialization)
{
#ifdef DEBUG_PRINT
   printf("ComponentInitialization: %s\n", ATwriteToString(term));
#endif

 OFP::InitialDataTarget InitialDataTarget;
 if (ATmatch(term, "ComponentInitialization_IDT(<term>)", &InitialDataTarget.term)) {

      if (ofp_traverse_InitialDataTarget(InitialDataTarget.term, &InitialDataTarget)) {
         // MATCHED InitialDataTarget
      } else return ATfalse;

   // MATCHED ComponentInitialization_IDT

   return ATtrue;
 }

 OFP::NullInit NullInit;
 if (ATmatch(term, "ComponentInitialization_NI(<term>)", &NullInit.term)) {

      if (ofp_traverse_NullInit(NullInit.term, &NullInit)) {
         // MATCHED NullInit
      } else return ATfalse;

   // MATCHED ComponentInitialization_NI

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "ComponentInitialization_E(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED ComponentInitialization_E

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R443 initial-data-target
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InitialDataTarget(ATerm term, OFP::InitialDataTarget* InitialDataTarget)
{
#ifdef DEBUG_PRINT
   printf("InitialDataTarget: %s\n", ATwriteToString(term));
#endif

 OFP::Designator Designator;
 if (ATmatch(term, "InitialDataTarget(<term>)", &Designator.term)) {

      if (ofp_traverse_Designator(Designator.term, &Designator)) {
         // MATCHED Designator
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R444 private-components-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PrivateComponentsStmt(ATerm term, OFP::PrivateComponentsStmt* PrivateComponentsStmt)
{
#ifdef DEBUG_PRINT
   printf("PrivateComponentsStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "PrivateComponentsStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R445 type-bound-procedure-part
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeBoundProcedurePart(ATerm term, OFP::TypeBoundProcedurePart* TypeBoundProcedurePart)
{
#ifdef DEBUG_PRINT
   printf("TypeBoundProcedurePart: %s\n", ATwriteToString(term));
#endif

 OFP::ContainsStmt ContainsStmt;
 OFP::BindingPrivateStmt BindingPrivateStmt;
 OFP::TypeBoundProcBinding TypeBoundProcBinding;
 if (ATmatch(term, "TypeBoundProcedurePart(<term>,<term>,<term>)", &ContainsStmt.term, &BindingPrivateStmt.term, &TypeBoundProcBinding.term)) {

      if (ofp_traverse_ContainsStmt(ContainsStmt.term, &ContainsStmt)) {
         // MATCHED ContainsStmt
      } else return ATfalse;

   if (ATmatch(BindingPrivateStmt.term, "Some(<term>)", &BindingPrivateStmt.term)) {
      if (ofp_traverse_BindingPrivateStmt(BindingPrivateStmt.term, &BindingPrivateStmt)) {
         // MATCHED BindingPrivateStmt
      } else return ATfalse;
   }

   ATermList TypeBoundProcBinding_tail = (ATermList) ATmake("<term>", TypeBoundProcBinding.term);
   while (! ATisEmpty(TypeBoundProcBinding_tail)) {
      TypeBoundProcBinding.term = ATgetFirst(TypeBoundProcBinding_tail);
      TypeBoundProcBinding_tail = ATgetNext (TypeBoundProcBinding_tail);
      if (ofp_traverse_TypeBoundProcBinding(TypeBoundProcBinding.term, &TypeBoundProcBinding)) {
         // MATCHED TypeBoundProcBinding
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R446 binding-private-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BindingPrivateStmt(ATerm term, OFP::BindingPrivateStmt* BindingPrivateStmt)
{
#ifdef DEBUG_PRINT
   printf("BindingPrivateStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "BindingPrivateStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R447 type-bound-proc-binding
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeBoundProcBinding(ATerm term, OFP::TypeBoundProcBinding* TypeBoundProcBinding)
{
#ifdef DEBUG_PRINT
   printf("TypeBoundProcBinding: %s\n", ATwriteToString(term));
#endif

 OFP::FinalProcedureStmt FinalProcedureStmt;
 if (ATmatch(term, "TypeBoundProcBinding_FPS(<term>)", &FinalProcedureStmt.term)) {

      if (ofp_traverse_FinalProcedureStmt(FinalProcedureStmt.term, &FinalProcedureStmt)) {
         // MATCHED FinalProcedureStmt
      } else return ATfalse;

   // MATCHED TypeBoundProcBinding_FPS

   return ATtrue;
 }

 OFP::TypeBoundGenericStmt TypeBoundGenericStmt;
 if (ATmatch(term, "TypeBoundProcBinding_TBGS(<term>)", &TypeBoundGenericStmt.term)) {

      if (ofp_traverse_TypeBoundGenericStmt(TypeBoundGenericStmt.term, &TypeBoundGenericStmt)) {
         // MATCHED TypeBoundGenericStmt
      } else return ATfalse;

   // MATCHED TypeBoundProcBinding_TBGS

   return ATtrue;
 }

 OFP::TypeBoundProcedureStmt TypeBoundProcedureStmt;
 if (ATmatch(term, "TypeBoundProcBinding_TBPS(<term>)", &TypeBoundProcedureStmt.term)) {

      if (ofp_traverse_TypeBoundProcedureStmt(TypeBoundProcedureStmt.term, &TypeBoundProcedureStmt)) {
         // MATCHED TypeBoundProcedureStmt
      } else return ATfalse;

   // MATCHED TypeBoundProcBinding_TBPS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R448 type-bound-procedure-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeBoundProcedureStmt(ATerm term, OFP::TypeBoundProcedureStmt* TypeBoundProcedureStmt)
{
#ifdef DEBUG_PRINT
   printf("TypeBoundProcedureStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::InterfaceName InterfaceName;
 OFP::BindingAttrList BindingAttrList;
 OFP::BindingNameList BindingNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "TypeBoundProcedureStmt_3(<term>,<term>,<term>,<term>,<term>)", &Label.term, &InterfaceName.term, &BindingAttrList.term, &BindingNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_InterfaceName(InterfaceName.term, &InterfaceName)) {
         // MATCHED InterfaceName
      } else return ATfalse;

      if (ofp_traverse_BindingAttrList(BindingAttrList.term, &BindingAttrList)) {
         // MATCHED BindingAttrList
      } else return ATfalse;

      if (ofp_traverse_BindingNameList(BindingNameList.term, &BindingNameList)) {
         // MATCHED BindingNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED TypeBoundProcedureStmt_3

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::BindingAttrList BindingAttrList1;
 OFP::TypeBoundProcDeclList TypeBoundProcDeclList;
 OFP::EOS EOS1;;
 if (ATmatch(term, "TypeBoundProcedureStmt_2(<term>,<term>,<term>,<term>)", &Label1.term, &BindingAttrList1.term, &TypeBoundProcDeclList.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(BindingAttrList1.term, "Some(<term>)", &BindingAttrList1.term)) {
   if (ATmatch(BindingAttrList1.term, "(<term>)", &BindingAttrList1.term)) {
      if (ofp_traverse_BindingAttrList(BindingAttrList1.term, &BindingAttrList1)) {
         // MATCHED BindingAttrList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_TypeBoundProcDeclList(TypeBoundProcDeclList.term, &TypeBoundProcDeclList)) {
         // MATCHED TypeBoundProcDeclList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED TypeBoundProcedureStmt_2

   return ATtrue;
 }

 OFP::Label Label2;
 OFP::TypeBoundProcDeclList TypeBoundProcDeclList1;
 OFP::EOS EOS2;
 if (ATmatch(term, "TypeBoundProcedureStmt_1(<term>,<term>,<term>)", &Label2.term, &TypeBoundProcDeclList1.term, &EOS2.term)) {

   if (ATmatch(Label2.term, "Some(<term>)", &Label2.term)) {
      if (ofp_traverse_Label(Label2.term, &Label2)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_TypeBoundProcDeclList(TypeBoundProcDeclList1.term, &TypeBoundProcDeclList1)) {
         // MATCHED TypeBoundProcDeclList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS2.term, &EOS2)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED TypeBoundProcedureStmt_1

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_BindingNameList(ATerm term, OFP::BindingNameList* BindingNameList)
{
#ifdef DEBUG_PRINT
   printf("BindingNameList: %s\n", ATwriteToString(term));
#endif

 OFP::Name BindingName;
 if (ATmatch(term, "BindingNameList(<term>)", &BindingName.term)) {

   ATermList BindingName_tail = (ATermList) ATmake("<term>", BindingName.term);
   while (! ATisEmpty(BindingName_tail)) {
      BindingName.term = ATgetFirst(BindingName_tail);
      BindingName_tail = ATgetNext (BindingName_tail);
      if (ofp_traverse_Name(BindingName.term, &BindingName)) {
         // MATCHED Name
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_BindingAttrList(ATerm term, OFP::BindingAttrList* BindingAttrList)
{
#ifdef DEBUG_PRINT
   printf("BindingAttrList: %s\n", ATwriteToString(term));
#endif

 OFP::BindingAttr BindingAttr;
 if (ATmatch(term, "BindingAttrList(<term>)", &BindingAttr.term)) {

   ATermList BindingAttr_tail = (ATermList) ATmake("<term>", BindingAttr.term);
   while (! ATisEmpty(BindingAttr_tail)) {
      BindingAttr.term = ATgetFirst(BindingAttr_tail);
      BindingAttr_tail = ATgetNext (BindingAttr_tail);
      if (ofp_traverse_BindingAttr(BindingAttr.term, &BindingAttr)) {
         // MATCHED BindingAttr
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R449 type-bound-proc-decl
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeBoundProcDecl(ATerm term, OFP::TypeBoundProcDecl* TypeBoundProcDecl)
{
#ifdef DEBUG_PRINT
   printf("TypeBoundProcDecl: %s\n", ATwriteToString(term));
#endif

 OFP::Name BindingName;
 OFP::Name ProcedureName;
 if (ATmatch(term, "TypeBoundProcDecl(<term>,<term>)", &BindingName.term, &ProcedureName.term)) {

      if (ofp_traverse_Name(BindingName.term, &BindingName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(ProcedureName.term, "Some(<term>)", &ProcedureName.term)) {
   if (ATmatch(ProcedureName.term, "(<term>)", &ProcedureName.term)) {
      if (ofp_traverse_Name(ProcedureName.term, &ProcedureName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_TypeBoundProcDeclList(ATerm term, OFP::TypeBoundProcDeclList* TypeBoundProcDeclList)
{
#ifdef DEBUG_PRINT
   printf("TypeBoundProcDeclList: %s\n", ATwriteToString(term));
#endif

 OFP::TypeBoundProcDecl TypeBoundProcDecl;
 if (ATmatch(term, "TypeBoundProcDeclList(<term>)", &TypeBoundProcDecl.term)) {

   ATermList TypeBoundProcDecl_tail = (ATermList) ATmake("<term>", TypeBoundProcDecl.term);
   while (! ATisEmpty(TypeBoundProcDecl_tail)) {
      TypeBoundProcDecl.term = ATgetFirst(TypeBoundProcDecl_tail);
      TypeBoundProcDecl_tail = ATgetNext (TypeBoundProcDecl_tail);
      if (ofp_traverse_TypeBoundProcDecl(TypeBoundProcDecl.term, &TypeBoundProcDecl)) {
         // MATCHED TypeBoundProcDecl
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R450 type-bound-generic-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeBoundGenericStmt(ATerm term, OFP::TypeBoundGenericStmt* TypeBoundGenericStmt)
{
#ifdef DEBUG_PRINT
   printf("TypeBoundGenericStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::AccessSpec AccessSpec;
 OFP::GenericSpec GenericSpec;
 OFP::BindingNameList BindingNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "TypeBoundGenericStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &AccessSpec.term, &GenericSpec.term, &BindingNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(AccessSpec.term, "Some(<term>)", &AccessSpec.term)) {
   if (ATmatch(AccessSpec.term, "(<term>)", &AccessSpec.term)) {
      if (ofp_traverse_AccessSpec(AccessSpec.term, &AccessSpec)) {
         // MATCHED AccessSpec
      } else return ATfalse;
   }
   }

      if (ofp_traverse_GenericSpec(GenericSpec.term, &GenericSpec)) {
         // MATCHED GenericSpec
      } else return ATfalse;

      if (ofp_traverse_BindingNameList(BindingNameList.term, &BindingNameList)) {
         // MATCHED BindingNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R451 binding-attr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BindingAttr(ATerm term, OFP::BindingAttr* BindingAttr)
{
#ifdef DEBUG_PRINT
   printf("BindingAttr: %s\n", ATwriteToString(term));
#endif

 OFP::AccessSpec AccessSpec;
 if (ATmatch(term, "BindingAttr_AS(<term>)", &AccessSpec.term)) {

      if (ofp_traverse_AccessSpec(AccessSpec.term, &AccessSpec)) {
         // MATCHED AccessSpec
      } else return ATfalse;

   // MATCHED BindingAttr_AS

   return ATtrue;
 }

 if (ATmatch(term, "BindingAttr_DEFERRED")) {

   // MATCHED BindingAttr_DEFERRED

   return ATtrue;
 }

 if (ATmatch(term, "BindingAttr_NON_OVERRIDABLE")) {

   // MATCHED BindingAttr_NON_OVERRIDABLE

   return ATtrue;
 }

 if (ATmatch(term, "BindingAttr_NOPASS")) {

   // MATCHED BindingAttr_NOPASS

   return ATtrue;
 }

 OFP::Name ArgName;
 if (ATmatch(term, "BindingAttr_PASS(<term>)", &ArgName.term)) {

   if (ATmatch(ArgName.term, "Some(<term>)", &ArgName.term)) {
   if (ATmatch(ArgName.term, "(<term>)", &ArgName.term)) {
      if (ofp_traverse_Name(ArgName.term, &ArgName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

   // MATCHED BindingAttr_PASS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R452 final-procedure-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FinalProcedureStmt(ATerm term, OFP::FinalProcedureStmt* FinalProcedureStmt)
{
#ifdef DEBUG_PRINT
   printf("FinalProcedureStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::FinalSubroutineNameList FinalSubroutineNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "FinalProcedureStmt(<term>,<term>,<term>)", &Label.term, &FinalSubroutineNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_FinalSubroutineNameList(FinalSubroutineNameList.term, &FinalSubroutineNameList)) {
         // MATCHED FinalSubroutineNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_FinalSubroutineNameList(ATerm term, OFP::FinalSubroutineNameList* FinalSubroutineNameList)
{
#ifdef DEBUG_PRINT
   printf("FinalSubroutineNameList: %s\n", ATwriteToString(term));
#endif

 OFP::Name FinalSubroutineName;
 if (ATmatch(term, "FinalSubroutineNameList(<term>)", &FinalSubroutineName.term)) {

   ATermList FinalSubroutineName_tail = (ATermList) ATmake("<term>", FinalSubroutineName.term);
   while (! ATisEmpty(FinalSubroutineName_tail)) {
      FinalSubroutineName.term = ATgetFirst(FinalSubroutineName_tail);
      FinalSubroutineName_tail = ATgetNext (FinalSubroutineName_tail);
      if (ofp_traverse_Name(FinalSubroutineName.term, &FinalSubroutineName)) {
         // MATCHED Name
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R453 derived-type-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DerivedTypeSpec(ATerm term, OFP::DerivedTypeSpec* DerivedTypeSpec)
{
#ifdef DEBUG_PRINT
   printf("DerivedTypeSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Name TypeName;
 OFP::TypeParamSpecList TypeParamSpecList;
 if (ATmatch(term, "DerivedTypeSpec(<term>,<term>)", &TypeName.term, &TypeParamSpecList.term)) {

      if (ofp_traverse_Name(TypeName.term, &TypeName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(TypeParamSpecList.term, "Some(<term>)", &TypeParamSpecList.term)) {
   if (ATmatch(TypeParamSpecList.term, "(<term>)", &TypeParamSpecList.term)) {
      if (ofp_traverse_TypeParamSpecList(TypeParamSpecList.term, &TypeParamSpecList)) {
         // MATCHED TypeParamSpecList
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R454 type-param-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeParamSpec(ATerm term, OFP::TypeParamSpec* TypeParamSpec)
{
#ifdef DEBUG_PRINT
   printf("TypeParamSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Keyword Keyword;
 OFP::TypeParamValue TypeParamValue;
 if (ATmatch(term, "TypeParamSpec(<term>,<term>)", &Keyword.term, &TypeParamValue.term)) {

   if (ATmatch(Keyword.term, "Some(<term>)", &Keyword.term)) {
   if (ATmatch(Keyword.term, "(<term>)", &Keyword.term)) {
      if (ofp_traverse_Keyword(Keyword.term, &Keyword)) {
         // MATCHED Keyword
      } else return ATfalse;
   }
   }

      if (ofp_traverse_TypeParamValue(TypeParamValue.term, &TypeParamValue)) {
         // MATCHED TypeParamValue
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_TypeParamSpecList(ATerm term, OFP::TypeParamSpecList* TypeParamSpecList)
{
#ifdef DEBUG_PRINT
   printf("TypeParamSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::TypeParamSpec TypeParamSpec;
 if (ATmatch(term, "TypeParamSpecList(<term>)", &TypeParamSpec.term)) {

   ATermList TypeParamSpec_tail = (ATermList) ATmake("<term>", TypeParamSpec.term);
   while (! ATisEmpty(TypeParamSpec_tail)) {
      TypeParamSpec.term = ATgetFirst(TypeParamSpec_tail);
      TypeParamSpec_tail = ATgetNext (TypeParamSpec_tail);
      if (ofp_traverse_TypeParamSpec(TypeParamSpec.term, &TypeParamSpec)) {
         // MATCHED TypeParamSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R455 structure-constructor
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_StructureConstructor(ATerm term, OFP::StructureConstructor* StructureConstructor)
{
#ifdef DEBUG_PRINT
   printf("StructureConstructor: %s\n", ATwriteToString(term));
#endif

 OFP::DerivedTypeSpec DerivedTypeSpec;
 OFP::ComponentSpec ComponentSpec;
 if (ATmatch(term, "StructureConstructor(<term>,<term>)", &DerivedTypeSpec.term, &ComponentSpec.term)) {

      if (ofp_traverse_DerivedTypeSpec(DerivedTypeSpec.term, &DerivedTypeSpec)) {
         // MATCHED DerivedTypeSpec
      } else return ATfalse;

   ATermList ComponentSpec_tail = (ATermList) ATmake("<term>", ComponentSpec.term);
   while (! ATisEmpty(ComponentSpec_tail)) {
      ComponentSpec.term = ATgetFirst(ComponentSpec_tail);
      ComponentSpec_tail = ATgetNext (ComponentSpec_tail);
      if (ofp_traverse_ComponentSpec(ComponentSpec.term, &ComponentSpec)) {
         // MATCHED ComponentSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R456 component-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComponentSpec(ATerm term, OFP::ComponentSpec* ComponentSpec)
{
#ifdef DEBUG_PRINT
   printf("ComponentSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Keyword Keyword;
 OFP::ComponentDataSource ComponentDataSource;
 if (ATmatch(term, "ComponentSpec(<term>,<term>)", &Keyword.term, &ComponentDataSource.term)) {

   if (ATmatch(Keyword.term, "Some(<term>)", &Keyword.term)) {
   if (ATmatch(Keyword.term, "(<term>)", &Keyword.term)) {
      if (ofp_traverse_Keyword(Keyword.term, &Keyword)) {
         // MATCHED Keyword
      } else return ATfalse;
   }
   }

      if (ofp_traverse_ComponentDataSource(ComponentDataSource.term, &ComponentDataSource)) {
         // MATCHED ComponentDataSource
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R457 component-data-source
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComponentDataSource(ATerm term, OFP::ComponentDataSource* ComponentDataSource)
{
#ifdef DEBUG_PRINT
   printf("ComponentDataSource: %s\n", ATwriteToString(term));
#endif

 OFP::ProcTarget ProcTarget;
 if (ATmatch(term, "ComponentDataSource_PT(<term>)", &ProcTarget.term)) {

      if (ofp_traverse_ProcTarget(ProcTarget.term, &ProcTarget)) {
         // MATCHED ProcTarget
      } else return ATfalse;

   // MATCHED ComponentDataSource_PT

   return ATtrue;
 }

 OFP::DataTarget DataTarget;
 if (ATmatch(term, "ComponentDataSource_DT(<term>)", &DataTarget.term)) {

      if (ofp_traverse_DataTarget(DataTarget.term, &DataTarget)) {
         // MATCHED DataTarget
      } else return ATfalse;

   // MATCHED ComponentDataSource_DT

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "ComponentDataSource_E(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED ComponentDataSource_E

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R458 enum-def
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EnumDef(ATerm term, OFP::EnumDef* EnumDef)
{
#ifdef DEBUG_PRINT
   printf("EnumDef: %s\n", ATwriteToString(term));
#endif

 OFP::EnumDefStmt EnumDefStmt;
 OFP::EnumeratorDefStmt EnumeratorDefStmt;
 OFP::EndEnumStmt EndEnumStmt;
 if (ATmatch(term, "EnumDef(<term>,<term>,<term>)", &EnumDefStmt.term, &EnumeratorDefStmt.term, &EndEnumStmt.term)) {

      if (ofp_traverse_EnumDefStmt(EnumDefStmt.term, &EnumDefStmt)) {
         // MATCHED EnumDefStmt
      } else return ATfalse;

   ATermList EnumeratorDefStmt_tail = (ATermList) ATmake("<term>", EnumeratorDefStmt.term);
   while (! ATisEmpty(EnumeratorDefStmt_tail)) {
      EnumeratorDefStmt.term = ATgetFirst(EnumeratorDefStmt_tail);
      EnumeratorDefStmt_tail = ATgetNext (EnumeratorDefStmt_tail);
      if (ofp_traverse_EnumeratorDefStmt(EnumeratorDefStmt.term, &EnumeratorDefStmt)) {
         // MATCHED EnumeratorDefStmt
      } else return ATfalse;
   }

      if (ofp_traverse_EndEnumStmt(EndEnumStmt.term, &EndEnumStmt)) {
         // MATCHED EndEnumStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R459 enum-def-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EnumDefStmt(ATerm term, OFP::EnumDefStmt* EnumDefStmt)
{
#ifdef DEBUG_PRINT
   printf("EnumDefStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "EnumDefStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R460 enumerator-def-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EnumeratorDefStmt(ATerm term, OFP::EnumeratorDefStmt* EnumeratorDefStmt)
{
#ifdef DEBUG_PRINT
   printf("EnumeratorDefStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EnumeratorList EnumeratorList;
 OFP::EOS EOS;
 if (ATmatch(term, "EnumeratorDefStmt(<term>,<term>,<term>)", &Label.term, &EnumeratorList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EnumeratorList(EnumeratorList.term, &EnumeratorList)) {
         // MATCHED EnumeratorList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R461 enumerator
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Enumerator(ATerm term, OFP::Enumerator* Enumerator)
{
#ifdef DEBUG_PRINT
   printf("Enumerator: %s\n", ATwriteToString(term));
#endif

 OFP::NamedConstant NamedConstant;
 OFP::IntConstantExpr IntConstantExpr;
 if (ATmatch(term, "Enumerator(<term>,<term>)", &NamedConstant.term, &IntConstantExpr.term)) {

      if (ofp_traverse_NamedConstant(NamedConstant.term, &NamedConstant)) {
         // MATCHED NamedConstant
      } else return ATfalse;

   if (ATmatch(IntConstantExpr.term, "Some(<term>)", &IntConstantExpr.term)) {
   if (ATmatch(IntConstantExpr.term, "(<term>)", &IntConstantExpr.term)) {
      if (ofp_traverse_IntConstantExpr(IntConstantExpr.term, &IntConstantExpr)) {
         // MATCHED IntConstantExpr
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_EnumeratorList(ATerm term, OFP::EnumeratorList* EnumeratorList)
{
#ifdef DEBUG_PRINT
   printf("EnumeratorList: %s\n", ATwriteToString(term));
#endif

 OFP::Enumerator Enumerator;
 if (ATmatch(term, "EnumeratorList(<term>)", &Enumerator.term)) {

   ATermList Enumerator_tail = (ATermList) ATmake("<term>", Enumerator.term);
   while (! ATisEmpty(Enumerator_tail)) {
      Enumerator.term = ATgetFirst(Enumerator_tail);
      Enumerator_tail = ATgetNext (Enumerator_tail);
      if (ofp_traverse_Enumerator(Enumerator.term, &Enumerator)) {
         // MATCHED Enumerator
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R462 end-enum-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndEnumStmt(ATerm term, OFP::EndEnumStmt* EndEnumStmt)
{
#ifdef DEBUG_PRINT
   printf("EndEnumStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "EndEnumStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R463 boz-literal-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BozLiteralConstant(ATerm term, OFP::BozLiteralConstant* BozLiteralConstant)
{
#ifdef DEBUG_PRINT
   printf("BozLiteralConstant: %s\n", ATwriteToString(term));
#endif

 OFP::HexConstant HexConstant;
 if (ATmatch(term, "BozLiteralConstant_HC(<term>)", &HexConstant.term)) {

      if (ofp_traverse_HexConstant(HexConstant.term, &HexConstant)) {
         // MATCHED HexConstant
      } else return ATfalse;

   // MATCHED BozLiteralConstant_HC

   return ATtrue;
 }

 OFP::OctalConstant OctalConstant;
 if (ATmatch(term, "BozLiteralConstant_OC(<term>)", &OctalConstant.term)) {

      if (ofp_traverse_OctalConstant(OctalConstant.term, &OctalConstant)) {
         // MATCHED OctalConstant
      } else return ATfalse;

   // MATCHED BozLiteralConstant_OC

   return ATtrue;
 }

 OFP::BinaryConstant BinaryConstant;
 if (ATmatch(term, "BozLiteralConstant_BC(<term>)", &BinaryConstant.term)) {

      if (ofp_traverse_BinaryConstant(BinaryConstant.term, &BinaryConstant)) {
         // MATCHED BinaryConstant
      } else return ATfalse;

   // MATCHED BozLiteralConstant_BC

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R468 array-constructor
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ArrayConstructor(ATerm term, OFP::ArrayConstructor* ArrayConstructor)
{
#ifdef DEBUG_PRINT
   printf("ArrayConstructor: %s\n", ATwriteToString(term));
#endif

 OFP::AcSpec AcSpec;
 if (ATmatch(term, "ArrayConstructor(<term>)", &AcSpec.term)) {

      if (ofp_traverse_AcSpec(AcSpec.term, &AcSpec)) {
         // MATCHED AcSpec
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R469 ac-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AcSpec(ATerm term, OFP::AcSpec* AcSpec)
{
#ifdef DEBUG_PRINT
   printf("AcSpec: %s\n", ATwriteToString(term));
#endif

 OFP::TypeSpec TypeSpec;
 OFP::AcValueList AcValueList;
 if (ATmatch(term, "AcSpec_AVL(<term>,<term>)", &TypeSpec.term, &AcValueList.term)) {

   if (ATmatch(TypeSpec.term, "Some(<term>)", &TypeSpec.term)) {
   if (ATmatch(TypeSpec.term, "(<term>)", &TypeSpec.term)) {
      if (ofp_traverse_TypeSpec(TypeSpec.term, &TypeSpec)) {
         // MATCHED TypeSpec
      } else return ATfalse;
   }
   }

      if (ofp_traverse_AcValueList(AcValueList.term, &AcValueList)) {
         // MATCHED AcValueList
      } else return ATfalse;

   // MATCHED AcSpec_AVL

   return ATtrue;
 }

 OFP::TypeSpec TypeSpec1;
 if (ATmatch(term, "AcSpec_TS(<term>)", &TypeSpec1.term)) {

      if (ofp_traverse_TypeSpec(TypeSpec1.term, &TypeSpec1)) {
         // MATCHED TypeSpec
      } else return ATfalse;

   // MATCHED AcSpec_TS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R472 ac-value
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AcValue(ATerm term, OFP::AcValue* AcValue)
{
#ifdef DEBUG_PRINT
   printf("AcValue: %s\n", ATwriteToString(term));
#endif

 OFP::AcImpliedDo AcImpliedDo;
 if (ATmatch(term, "AcValue_AID(<term>)", &AcImpliedDo.term)) {

      if (ofp_traverse_AcImpliedDo(AcImpliedDo.term, &AcImpliedDo)) {
         // MATCHED AcImpliedDo
      } else return ATfalse;

   // MATCHED AcValue_AID

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "AcValue_E(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED AcValue_E

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AcValueList(ATerm term, OFP::AcValueList* AcValueList)
{
#ifdef DEBUG_PRINT
   printf("AcValueList: %s\n", ATwriteToString(term));
#endif

 OFP::AcValue AcValue;
 if (ATmatch(term, "AcValueList(<term>)", &AcValue.term)) {

   ATermList AcValue_tail = (ATermList) ATmake("<term>", AcValue.term);
   while (! ATisEmpty(AcValue_tail)) {
      AcValue.term = ATgetFirst(AcValue_tail);
      AcValue_tail = ATgetNext (AcValue_tail);
      if (ofp_traverse_AcValue(AcValue.term, &AcValue)) {
         // MATCHED AcValue
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R473 ac-implied-do
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AcImpliedDo(ATerm term, OFP::AcImpliedDo* AcImpliedDo)
{
#ifdef DEBUG_PRINT
   printf("AcImpliedDo: %s\n", ATwriteToString(term));
#endif

 OFP::AcValueList AcValueList;
 OFP::AcImpliedDoControl AcImpliedDoControl;
 if (ATmatch(term, "AcImpliedDo(<term>,<term>)", &AcValueList.term, &AcImpliedDoControl.term)) {

      if (ofp_traverse_AcValueList(AcValueList.term, &AcValueList)) {
         // MATCHED AcValueList
      } else return ATfalse;

      if (ofp_traverse_AcImpliedDoControl(AcImpliedDoControl.term, &AcImpliedDoControl)) {
         // MATCHED AcImpliedDoControl
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R474 ac-implied-do-control
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AcImpliedDoControl(ATerm term, OFP::AcImpliedDoControl* AcImpliedDoControl)
{
#ifdef DEBUG_PRINT
   printf("AcImpliedDoControl: %s\n", ATwriteToString(term));
#endif

 OFP::AcDoVariable AcDoVariable;
 OFP::Expr Expr;
 OFP::Expr Expr1;
 OFP::Expr Expr2;
 if (ATmatch(term, "AcImpliedDoControl(<term>,<term>,<term>,<term>)", &AcDoVariable.term, &Expr.term, &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_AcDoVariable(AcDoVariable.term, &AcDoVariable)) {
         // MATCHED AcDoVariable
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
      } else return ATfalse;

   if (ATmatch(Expr2.term, "Some(<term>)", &Expr2.term)) {
   if (ATmatch(Expr2.term, "(<term>)", &Expr2.term)) {
      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R475 ac-do-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AcDoVariable(ATerm term, OFP::AcDoVariable* AcDoVariable)
{
#ifdef DEBUG_PRINT
   printf("AcDoVariable: %s\n", ATwriteToString(term));
#endif

 OFP::DoVariable DoVariable;
 if (ATmatch(term, "AcDoVariable(<term>)", &DoVariable.term)) {

      if (ofp_traverse_DoVariable(DoVariable.term, &DoVariable)) {
         // MATCHED DoVariable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}


/**
 * Section/Clause 5: Attribute declarations and specifications
 */

//========================================================================================
// R501 type-declaration-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeDeclarationStmt(ATerm term, OFP::TypeDeclarationStmt* TypeDeclarationStmt)
{
#ifdef DEBUG_PRINT
   printf("TypeDeclarationStmt(F): %s\n", ATwriteToString(term));
#endif

   OFP::Label Label;
   OFP::DeclarationTypeSpec DeclarationTypeSpec;
   OFP::AttrSpecList AttrSpecList;
   OFP::EntityDeclList EntityDeclList;
   OFP::EOS EOS;
   if (ATmatch(term, "TypeDeclarationStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &DeclarationTypeSpec.term, &AttrSpecList.term, &EntityDeclList.term, &EOS.term)) {

      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         TypeDeclarationStmt->setLabel(Label.newLabel());
      } // Optional

      if (ofp_traverse_DeclarationTypeSpec(DeclarationTypeSpec.term, &DeclarationTypeSpec)) {
         // MATCHED DeclarationTypeSpec
         TypeDeclarationStmt->setDeclarationTypeSpec(DeclarationTypeSpec.newDeclarationTypeSpec());
      } else return ATfalse;

      if (ofp_traverse_AttrSpecList(AttrSpecList.term, &AttrSpecList)) {
         // MATCHED AttrSpecList
         TypeDeclarationStmt->setAttrSpecList(AttrSpecList.newAttrSpecList());
      } else return ATfalse;

      if (ofp_traverse_EntityDeclList(EntityDeclList.term, &EntityDeclList)) {
         // MATCHED EntityDeclList
         TypeDeclarationStmt->setEntityDeclList(EntityDeclList.newEntityDeclList());
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         TypeDeclarationStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

      ast->build_TypeDeclarationStmt(TypeDeclarationStmt);

      return ATtrue;
   }

   return ATfalse;
}

#ifdef OBSOLETE
ATbool ofp_traverse_OptAttrSpecList(ATerm term, OFP::OptAttrSpecList* OptAttrSpecList)
{
#ifdef DEBUG_PRINT
   printf("OptAttrSpecList(F): %s\n", ATwriteToString(term));
#endif

 OFP::AttrSpecList AttrSpecList;
 if (ATmatch(term, "OptAttrSpecList(<term>)", &AttrSpecList.term)) {

   if (ATmatch(AttrSpecList.term, "Some(<term>)", &AttrSpecList.term)) {
   if (ATmatch(AttrSpecList.term, "(<term>)", &AttrSpecList.term)) {
      if (ofp_traverse_AttrSpecList(AttrSpecList.term, &AttrSpecList)) {
         // MATCHED AttrSpecList
         OptAttrSpecList->setAttrSpecList(AttrSpecList.newAttrSpecList());
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}
#endif

//========================================================================================
// R502 attr-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AttrSpec(ATerm term, OFP::AttrSpec* AttrSpec)
{
#ifdef DEBUG_PRINT
   printf("AttrSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "AttrSpec_VOLATILE")) {

   // MATCHED AttrSpec_VOLATILE

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_VALUE")) {

   // MATCHED AttrSpec_VALUE

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_TARGET")) {

   // MATCHED AttrSpec_TARGET

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_SAVE")) {

   // MATCHED AttrSpec_SAVE

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_PROTECTED")) {

   // MATCHED AttrSpec_PROTECTED

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_POINTER")) {

   // MATCHED AttrSpec_POINTER

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_PARAMETER")) {

   // MATCHED AttrSpec_PARAMETER

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_OPTIONAL")) {

   // MATCHED AttrSpec_OPTIONAL

   return ATtrue;
 }

 OFP::LanguageBindingSpec LanguageBindingSpec;
 if (ATmatch(term, "AttrSpec_LBS(<term>)", &LanguageBindingSpec.term)) {

      if (ofp_traverse_LanguageBindingSpec(LanguageBindingSpec.term, &LanguageBindingSpec)) {
         // MATCHED LanguageBindingSpec
      } else return ATfalse;

   // MATCHED AttrSpec_LBS

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_INTRINSIC")) {

   // MATCHED AttrSpec_INTRINSIC

   return ATtrue;
 }

 OFP::IntentSpec IntentSpec;
 if (ATmatch(term, "AttrSpec_INTENT(<term>)", &IntentSpec.term)) {

      if (ofp_traverse_IntentSpec(IntentSpec.term, &IntentSpec)) {
         // MATCHED IntentSpec
      } else return ATfalse;

   // MATCHED AttrSpec_INTENT

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_EXTERNAL")) {

   // MATCHED AttrSpec_EXTERNAL

   return ATtrue;
 }

 OFP::ArraySpec ArraySpec;
 if (ATmatch(term, "AttrSpec_DIMENSION(<term>)", &ArraySpec.term)) {

      if (ofp_traverse_ArraySpec(ArraySpec.term, &ArraySpec)) {
         // MATCHED ArraySpec
      } else return ATfalse;

   // MATCHED AttrSpec_DIMENSION

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_CONTIGUOUS")) {

   // MATCHED AttrSpec_CONTIGUOUS

   return ATtrue;
 }

 OFP::CoarraySpec CoarraySpec;
 if (ATmatch(term, "AttrSpec_CODIMENSION(<term>)", &CoarraySpec.term)) {

      if (ofp_traverse_CoarraySpec(CoarraySpec.term, &CoarraySpec)) {
         // MATCHED CoarraySpec
      } else return ATfalse;

   // MATCHED AttrSpec_CODIMENSION

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_ASYNC")) {

   // MATCHED AttrSpec_ASYNC

   return ATtrue;
 }

 if (ATmatch(term, "AttrSpec_ALLOCATABLE")) {

   // MATCHED AttrSpec_ALLOCATABLE

   return ATtrue;
 }

 OFP::AccessSpec AccessSpec;
 if (ATmatch(term, "AttrSpec_AS(<term>)", &AccessSpec.term)) {

      if (ofp_traverse_AccessSpec(AccessSpec.term, &AccessSpec)) {
         // MATCHED AccessSpec
      } else return ATfalse;

   // MATCHED AttrSpec_AS

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AttrSpecList(ATerm term, OFP::AttrSpecList* AttrSpecList)
{
#ifdef DEBUG_PRINT
   printf("AttrSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::AttrSpec AttrSpec;
 if (ATmatch(term, "<term>", &AttrSpec.term)) {

   ATermList AttrSpec_tail = (ATermList) ATmake("<term>", AttrSpec.term);
   while (! ATisEmpty(AttrSpec_tail)) {
      AttrSpec.term = ATgetFirst(AttrSpec_tail);
      AttrSpec_tail = ATgetNext (AttrSpec_tail);
      if (ofp_traverse_AttrSpec(AttrSpec.term, &AttrSpec)) {
         // MATCHED AttrSpec
         AttrSpecList->appendAttrSpec(AttrSpec.newAttrSpec());
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R503 entity-decl
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EntityDecl(ATerm term, OFP::EntityDecl* EntityDecl)
{
#ifdef DEBUG_PRINT
   printf("EntityDecl(W): %s\n", ATwriteToString(term));
#endif

   OFP::Name ObjectName;
   OFP::ArraySpec ArraySpec;
   OFP::CoarraySpec CoarraySpec;
   OFP::CharLength CharLength;
   OFP::Initialization Initialization;
   if (ATmatch(term, "EntityDecl(<term>,<term>,<term>,<term>,<term>)", &ObjectName.term, &ArraySpec.term, &CoarraySpec.term, &CharLength.term, &Initialization.term)) {

      if (ofp_traverse_Name(ObjectName.term, &ObjectName)) {
         // MATCHED Name
         EntityDecl->setObjectName(ObjectName.newName());
      } else return ATfalse;

#ifdef TODO_ROSE
      if (ofp_traverse_ArraySpec(ArraySpec.term, &ArraySpec)) {
         // MATCHED ArraySpec
      } else return ATfalse;

      if (ofp_traverse_CoarraySpec(CoarraySpec.term, &CoarraySpec)) {
         // MATCHED CoarraySpec
      } else return ATfalse;

      if (ofp_traverse_CharLength(CharLength.term, &CharLength)) {
         // MATCHED CharLength
      } else return ATfalse;

      if (ofp_traverse_Initialization(Initialization.term, &Initialization)) {
         // MATCHED Initialization
      } else return ATfalse;
#endif

   ast->build_EntityDecl(EntityDecl);

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_EntityDeclList(ATerm term, OFP::EntityDeclList* EntityDeclList)
{
#ifdef DEBUG_PRINT
   printf("EntityDeclList(F): %s\n", ATwriteToString(term));
#endif

 OFP::EntityDecl EntityDecl;
 if (ATmatch(term, "<term>", &EntityDecl.term)) {

   ATermList EntityDecl_tail = (ATermList) ATmake("<term>", EntityDecl.term);
   while (! ATisEmpty(EntityDecl_tail)) {
      EntityDecl.term = ATgetFirst(EntityDecl_tail);
      EntityDecl_tail = ATgetNext (EntityDecl_tail);
      if (ofp_traverse_EntityDecl(EntityDecl.term, &EntityDecl)) {
         // MATCHED EntityDecl
         EntityDeclList->appendEntityDecl(EntityDecl.newEntityDecl());
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R505 initialization
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Initialization(ATerm term, OFP::Initialization* Initialization)
{
#ifdef DEBUG_PRINT
   printf("Initialization: %s\n", ATwriteToString(term));
#endif

 OFP::InitialDataTarget InitialDataTarget;
 if (ATmatch(term, "Initialization_IDT(<term>)", &InitialDataTarget.term)) {

      if (ofp_traverse_InitialDataTarget(InitialDataTarget.term, &InitialDataTarget)) {
         // MATCHED InitialDataTarget
      } else return ATfalse;

   // MATCHED Initialization_IDT

   return ATtrue;
 }

 OFP::NullInit NullInit;
 if (ATmatch(term, "Initialization_NI(<term>)", &NullInit.term)) {

      if (ofp_traverse_NullInit(NullInit.term, &NullInit)) {
         // MATCHED NullInit
      } else return ATfalse;

   // MATCHED Initialization_NI

   return ATtrue;
 }

 OFP::ConstantExpr ConstantExpr;
 if (ATmatch(term, "Initialization_CE(<term>)", &ConstantExpr.term)) {

      if (ofp_traverse_ConstantExpr(ConstantExpr.term, &ConstantExpr)) {
         // MATCHED ConstantExpr
      } else return ATfalse;

   // MATCHED Initialization_CE

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R506 null-init
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_NullInit(ATerm term, OFP::NullInit* NullInit)
{
#ifdef DEBUG_PRINT
   printf("NullInit: %s\n", ATwriteToString(term));
#endif

 OFP::FunctionReference FunctionReference;
 if (ATmatch(term, "NullInit(<term>)", &FunctionReference.term)) {

      if (ofp_traverse_FunctionReference(FunctionReference.term, &FunctionReference)) {
         // MATCHED FunctionReference
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R507 access-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AccessSpec(ATerm term, OFP::AccessSpec* AccessSpec)
{
#ifdef DEBUG_PRINT
   printf("AccessSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "AccessSpec_PRIVATE")) {

   // MATCHED AccessSpec_PRIVATE

   return ATtrue;
 }

 if (ATmatch(term, "AccessSpec_PUBLIC")) {

   // MATCHED AccessSpec_PUBLIC

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R508 language-binding-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LanguageBindingSpec(ATerm term, OFP::LanguageBindingSpec* LanguageBindingSpec)
{
#ifdef DEBUG_PRINT
   printf("LanguageBindingSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Scon Scon;
 if (ATmatch(term, "LanguageBindingSpec(<term>)", &Scon.term)) {

   if (ATmatch(Scon.term, "Some(<term>)", &Scon.term)) {
   if (ATmatch(Scon.term, "(<term>)", &Scon.term)) {
      if (ofp_traverse_Scon(Scon.term, &Scon)) {
         // MATCHED Scon
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R509 coarray-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CoarraySpec(ATerm term, OFP::CoarraySpec* CoarraySpec)
{
#ifdef DEBUG_PRINT
   printf("CoarraySpec: %s\n", ATwriteToString(term));
#endif

 OFP::ExplicitCoshapeSpec ExplicitCoshapeSpec;
 if (ATmatch(term, "CoarraySpec_ECS(<term>)", &ExplicitCoshapeSpec.term)) {

      if (ofp_traverse_ExplicitCoshapeSpec(ExplicitCoshapeSpec.term, &ExplicitCoshapeSpec)) {
         // MATCHED ExplicitCoshapeSpec
      } else return ATfalse;

   // MATCHED CoarraySpec_ECS

   return ATtrue;
 }

 OFP::DeferredCoshapeSpecList DeferredCoshapeSpecList;
 if (ATmatch(term, "CoarraySpec_DCSL(<term>)", &DeferredCoshapeSpecList.term)) {

      if (ofp_traverse_DeferredCoshapeSpecList(DeferredCoshapeSpecList.term, &DeferredCoshapeSpecList)) {
         // MATCHED DeferredCoshapeSpecList
      } else return ATfalse;

   // MATCHED CoarraySpec_DCSL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R510 deferred-coshape-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DeferredCoshapeSpec(ATerm term, OFP::DeferredCoshapeSpec* DeferredCoshapeSpec)
{
#ifdef DEBUG_PRINT
   printf("DeferredCoshapeSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "DeferredCoshapeSpec")) {

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DeferredCoshapeSpecList(ATerm term, OFP::DeferredCoshapeSpecList* DeferredCoshapeSpecList)
{
#ifdef DEBUG_PRINT
   printf("DeferredCoshapeSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::DeferredCoshapeSpec DeferredCoshapeSpec;
 if (ATmatch(term, "DeferredCoshapeSpecList(<term>)", &DeferredCoshapeSpec.term)) {

   ATermList DeferredCoshapeSpec_tail = (ATermList) ATmake("<term>", DeferredCoshapeSpec.term);
   while (! ATisEmpty(DeferredCoshapeSpec_tail)) {
      DeferredCoshapeSpec.term = ATgetFirst(DeferredCoshapeSpec_tail);
      DeferredCoshapeSpec_tail = ATgetNext (DeferredCoshapeSpec_tail);
      if (ofp_traverse_DeferredCoshapeSpec(DeferredCoshapeSpec.term, &DeferredCoshapeSpec)) {
         // MATCHED DeferredCoshapeSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R511 explicit-coshape-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ExplicitCoshapeSpec(ATerm term, OFP::ExplicitCoshapeSpec* ExplicitCoshapeSpec)
{
#ifdef DEBUG_PRINT
   printf("ExplicitCoshapeSpec: %s\n", ATwriteToString(term));
#endif

 OFP::CoboundsEntry CoboundsEntry;
 OFP::LowerCobound LowerCobound;
 if (ATmatch(term, "ExplicitCoshapeSpec(<term>,<term>)", &CoboundsEntry.term, &LowerCobound.term)) {

   ATermList CoboundsEntry_tail = (ATermList) ATmake("<term>", CoboundsEntry.term);
   while (! ATisEmpty(CoboundsEntry_tail)) {
      CoboundsEntry.term = ATgetFirst(CoboundsEntry_tail);
      CoboundsEntry_tail = ATgetNext (CoboundsEntry_tail);
      if (ofp_traverse_CoboundsEntry(CoboundsEntry.term, &CoboundsEntry)) {
         // MATCHED CoboundsEntry
      } else return ATfalse;
   }

   if (ATmatch(LowerCobound.term, "Some(<term>)", &LowerCobound.term)) {
   if (ATmatch(LowerCobound.term, "(<term>)", &LowerCobound.term)) {
      if (ofp_traverse_LowerCobound(LowerCobound.term, &LowerCobound)) {
         // MATCHED LowerCobound
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CoboundsEntry(ATerm term, OFP::CoboundsEntry* CoboundsEntry)
{
#ifdef DEBUG_PRINT
   printf("CoboundsEntry: %s\n", ATwriteToString(term));
#endif

 OFP::LowerCobound LowerCobound;
 OFP::UpperCobound UpperCobound;
 if (ATmatch(term, "CoboundsEntry(<term>,<term>)", &LowerCobound.term, &UpperCobound.term)) {

   if (ATmatch(LowerCobound.term, "Some(<term>)", &LowerCobound.term)) {
   if (ATmatch(LowerCobound.term, "(<term>)", &LowerCobound.term)) {
      if (ofp_traverse_LowerCobound(LowerCobound.term, &LowerCobound)) {
         // MATCHED LowerCobound
      } else return ATfalse;
   }
   }

      if (ofp_traverse_UpperCobound(UpperCobound.term, &UpperCobound)) {
         // MATCHED UpperCobound
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R512 lower-cobound
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LowerCobound(ATerm term, OFP::LowerCobound* LowerCobound)
{
#ifdef DEBUG_PRINT
   printf("LowerCobound: %s\n", ATwriteToString(term));
#endif

 OFP::SpecificationExpr SpecificationExpr;
 if (ATmatch(term, "LowerCobound(<term>)", &SpecificationExpr.term)) {

      if (ofp_traverse_SpecificationExpr(SpecificationExpr.term, &SpecificationExpr)) {
         // MATCHED SpecificationExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R513 upper-cobound
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_UpperCobound(ATerm term, OFP::UpperCobound* UpperCobound)
{
#ifdef DEBUG_PRINT
   printf("UpperCobound: %s\n", ATwriteToString(term));
#endif

 OFP::SpecificationExpr SpecificationExpr;
 if (ATmatch(term, "UpperCobound(<term>)", &SpecificationExpr.term)) {

      if (ofp_traverse_SpecificationExpr(SpecificationExpr.term, &SpecificationExpr)) {
         // MATCHED SpecificationExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R515 array-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ArraySpec(ATerm term, OFP::ArraySpec* ArraySpec)
{
#ifdef DEBUG_PRINT
   printf("ArraySpec: %s\n", ATwriteToString(term));
#endif

 OFP::AssumedRankSpec AssumedRankSpec;
 if (ATmatch(term, "ArraySpec_ARS(<term>)", &AssumedRankSpec.term)) {

      if (ofp_traverse_AssumedRankSpec(AssumedRankSpec.term, &AssumedRankSpec)) {
         // MATCHED AssumedRankSpec
      } else return ATfalse;

   // MATCHED ArraySpec_ARS

   return ATtrue;
 }

 OFP::ImpliedShapeSpecList ImpliedShapeSpecList;
 if (ATmatch(term, "ArraySpec_ISSL(<term>)", &ImpliedShapeSpecList.term)) {

      if (ofp_traverse_ImpliedShapeSpecList(ImpliedShapeSpecList.term, &ImpliedShapeSpecList)) {
         // MATCHED ImpliedShapeSpecList
      } else return ATfalse;

   // MATCHED ArraySpec_ISSL

   return ATtrue;
 }

 OFP::AssumedSizeSpec AssumedSizeSpec;
 if (ATmatch(term, "ArraySpec_ASS(<term>)", &AssumedSizeSpec.term)) {

      if (ofp_traverse_AssumedSizeSpec(AssumedSizeSpec.term, &AssumedSizeSpec)) {
         // MATCHED AssumedSizeSpec
      } else return ATfalse;

   // MATCHED ArraySpec_ASS

   return ATtrue;
 }

 OFP::DeferredShapeSpecList DeferredShapeSpecList;
 if (ATmatch(term, "ArraySpec_DSSL(<term>)", &DeferredShapeSpecList.term)) {

      if (ofp_traverse_DeferredShapeSpecList(DeferredShapeSpecList.term, &DeferredShapeSpecList)) {
         // MATCHED DeferredShapeSpecList
      } else return ATfalse;

   // MATCHED ArraySpec_DSSL

   return ATtrue;
 }

 OFP::AssumedShapeSpecList AssumedShapeSpecList;
 if (ATmatch(term, "ArraySpec_ASSL(<term>)", &AssumedShapeSpecList.term)) {

      if (ofp_traverse_AssumedShapeSpecList(AssumedShapeSpecList.term, &AssumedShapeSpecList)) {
         // MATCHED AssumedShapeSpecList
      } else return ATfalse;

   // MATCHED ArraySpec_ASSL

   return ATtrue;
 }

 OFP::ExplicitShapeSpecList ExplicitShapeSpecList;
 if (ATmatch(term, "ArraySpec_ESSL(<term>)", &ExplicitShapeSpecList.term)) {

      if (ofp_traverse_ExplicitShapeSpecList(ExplicitShapeSpecList.term, &ExplicitShapeSpecList)) {
         // MATCHED ExplicitShapeSpecList
      } else return ATfalse;

   // MATCHED ArraySpec_ESSL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R516 explicit-shape-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ExplicitShapeSpec(ATerm term, OFP::ExplicitShapeSpec* ExplicitShapeSpec)
{
#ifdef DEBUG_PRINT
   printf("ExplicitShapeSpec: %s\n", ATwriteToString(term));
#endif

 OFP::LowerBound LowerBound;
 OFP::UpperBound UpperBound;
 if (ATmatch(term, "ExplicitShapeSpec(<term>,<term>)", &LowerBound.term, &UpperBound.term)) {

   if (ATmatch(LowerBound.term, "Some(<term>)", &LowerBound.term)) {
   if (ATmatch(LowerBound.term, "(<term>)", &LowerBound.term)) {
      if (ofp_traverse_LowerBound(LowerBound.term, &LowerBound)) {
         // MATCHED LowerBound
      } else return ATfalse;
   }
   }

      if (ofp_traverse_UpperBound(UpperBound.term, &UpperBound)) {
         // MATCHED UpperBound
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ExplicitShapeSpecList(ATerm term, OFP::ExplicitShapeSpecList* ExplicitShapeSpecList)
{
#ifdef DEBUG_PRINT
   printf("ExplicitShapeSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ExplicitShapeSpec ExplicitShapeSpec;
 if (ATmatch(term, "ExplicitShapeSpecList(<term>)", &ExplicitShapeSpec.term)) {

   ATermList ExplicitShapeSpec_tail = (ATermList) ATmake("<term>", ExplicitShapeSpec.term);
   while (! ATisEmpty(ExplicitShapeSpec_tail)) {
      ExplicitShapeSpec.term = ATgetFirst(ExplicitShapeSpec_tail);
      ExplicitShapeSpec_tail = ATgetNext (ExplicitShapeSpec_tail);
      if (ofp_traverse_ExplicitShapeSpec(ExplicitShapeSpec.term, &ExplicitShapeSpec)) {
         // MATCHED ExplicitShapeSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R517 lower-bound
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LowerBound(ATerm term, OFP::LowerBound* LowerBound)
{
#ifdef DEBUG_PRINT
   printf("LowerBound: %s\n", ATwriteToString(term));
#endif

 OFP::SpecificationExpr SpecificationExpr;
 if (ATmatch(term, "LowerBound(<term>)", &SpecificationExpr.term)) {

      if (ofp_traverse_SpecificationExpr(SpecificationExpr.term, &SpecificationExpr)) {
         // MATCHED SpecificationExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R518 lower-bound
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_UpperBound(ATerm term, OFP::UpperBound* UpperBound)
{
#ifdef DEBUG_PRINT
   printf("UpperBound: %s\n", ATwriteToString(term));
#endif

 OFP::SpecificationExpr SpecificationExpr;
 if (ATmatch(term, "UpperBound(<term>)", &SpecificationExpr.term)) {

      if (ofp_traverse_SpecificationExpr(SpecificationExpr.term, &SpecificationExpr)) {
         // MATCHED SpecificationExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R519 assumed-shape-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AssumedShapeSpec(ATerm term, OFP::AssumedShapeSpec* AssumedShapeSpec)
{
#ifdef DEBUG_PRINT
   printf("AssumedShapeSpec: %s\n", ATwriteToString(term));
#endif

 OFP::LowerBound LowerBound;
 if (ATmatch(term, "AssumedShapeSpec(<term>)", &LowerBound.term)) {

   if (ATmatch(LowerBound.term, "Some(<term>)", &LowerBound.term)) {
      if (ofp_traverse_LowerBound(LowerBound.term, &LowerBound)) {
         // MATCHED LowerBound
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AssumedShapeSpecList(ATerm term, OFP::AssumedShapeSpecList* AssumedShapeSpecList)
{
#ifdef DEBUG_PRINT
   printf("AssumedShapeSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::AssumedShapeSpec AssumedShapeSpec;
 if (ATmatch(term, "AssumedShapeSpecList(<term>)", &AssumedShapeSpec.term)) {

   ATermList AssumedShapeSpec_tail = (ATermList) ATmake("<term>", AssumedShapeSpec.term);
   while (! ATisEmpty(AssumedShapeSpec_tail)) {
      AssumedShapeSpec.term = ATgetFirst(AssumedShapeSpec_tail);
      AssumedShapeSpec_tail = ATgetNext (AssumedShapeSpec_tail);
      if (ofp_traverse_AssumedShapeSpec(AssumedShapeSpec.term, &AssumedShapeSpec)) {
         // MATCHED AssumedShapeSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R520 deferred-shape-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DeferredShapeSpec(ATerm term, OFP::DeferredShapeSpec* DeferredShapeSpec)
{
#ifdef DEBUG_PRINT
   printf("DeferredShapeSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "DeferredShapeSpec")) {

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DeferredShapeSpecList(ATerm term, OFP::DeferredShapeSpecList* DeferredShapeSpecList)
{
#ifdef DEBUG_PRINT
   printf("DeferredShapeSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::DeferredShapeSpec DeferredShapeSpec;
 if (ATmatch(term, "DeferredShapeSpecList(<term>)", &DeferredShapeSpec.term)) {

   ATermList DeferredShapeSpec_tail = (ATermList) ATmake("<term>", DeferredShapeSpec.term);
   while (! ATisEmpty(DeferredShapeSpec_tail)) {
      DeferredShapeSpec.term = ATgetFirst(DeferredShapeSpec_tail);
      DeferredShapeSpec_tail = ATgetNext (DeferredShapeSpec_tail);
      if (ofp_traverse_DeferredShapeSpec(DeferredShapeSpec.term, &DeferredShapeSpec)) {
         // MATCHED DeferredShapeSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R521 assumed-size-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AssumedSizeSpec(ATerm term, OFP::AssumedSizeSpec* AssumedSizeSpec)
{
#ifdef DEBUG_PRINT
   printf("AssumedSizeSpec: %s\n", ATwriteToString(term));
#endif

 OFP::ExplicitShapeSpec ExplicitShapeSpec;
 OFP::LowerBound LowerBound;
 if (ATmatch(term, "AssumedSizeSpec(<term>,<term>)", &ExplicitShapeSpec.term, &LowerBound.term)) {

   ATermList ExplicitShapeSpec_tail = (ATermList) ATmake("<term>", ExplicitShapeSpec.term);
   while (! ATisEmpty(ExplicitShapeSpec_tail)) {
      ExplicitShapeSpec.term = ATgetFirst(ExplicitShapeSpec_tail);
      ExplicitShapeSpec_tail = ATgetNext (ExplicitShapeSpec_tail);
      if (ofp_traverse_ExplicitShapeSpec(ExplicitShapeSpec.term, &ExplicitShapeSpec)) {
         // MATCHED ExplicitShapeSpec
      } else return ATfalse;
   }

   if (ATmatch(LowerBound.term, "Some(<term>)", &LowerBound.term)) {
   if (ATmatch(LowerBound.term, "(<term>)", &LowerBound.term)) {
      if (ofp_traverse_LowerBound(LowerBound.term, &LowerBound)) {
         // MATCHED LowerBound
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R522a assumed-rank-spec (TR 29113)
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AssumedRankSpec(ATerm term, OFP::AssumedRankSpec* AssumedRankSpec)
{
#ifdef DEBUG_PRINT
   printf("AssumedRankSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "AssumedRankSpec")) {

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R522 implied-shape-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ImpliedShapeSpec(ATerm term, OFP::ImpliedShapeSpec* ImpliedShapeSpec)
{
#ifdef DEBUG_PRINT
   printf("ImpliedShapeSpec: %s\n", ATwriteToString(term));
#endif

 OFP::LowerBound LowerBound;
 if (ATmatch(term, "ImpliedShapeSpec(<term>)", &LowerBound.term)) {

   if (ATmatch(LowerBound.term, "Some(<term>)", &LowerBound.term)) {
   if (ATmatch(LowerBound.term, "(<term>)", &LowerBound.term)) {
      if (ofp_traverse_LowerBound(LowerBound.term, &LowerBound)) {
         // MATCHED LowerBound
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ImpliedShapeSpecList(ATerm term, OFP::ImpliedShapeSpecList* ImpliedShapeSpecList)
{
#ifdef DEBUG_PRINT
   printf("ImpliedShapeSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ImpliedShapeSpec ImpliedShapeSpec;
 if (ATmatch(term, "ImpliedShapeSpecList(<term>)", &ImpliedShapeSpec.term)) {

   ATermList ImpliedShapeSpec_tail = (ATermList) ATmake("<term>", ImpliedShapeSpec.term);
   while (! ATisEmpty(ImpliedShapeSpec_tail)) {
      ImpliedShapeSpec.term = ATgetFirst(ImpliedShapeSpec_tail);
      ImpliedShapeSpec_tail = ATgetNext (ImpliedShapeSpec_tail);
      if (ofp_traverse_ImpliedShapeSpec(ImpliedShapeSpec.term, &ImpliedShapeSpec)) {
         // MATCHED ImpliedShapeSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R523 intent-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntentSpec(ATerm term, OFP::IntentSpec* IntentSpec)
{
#ifdef DEBUG_PRINT
   printf("IntentSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "IntentSpec_INOUT")) {

   // MATCHED IntentSpec_INOUT

   return ATtrue;
 }

 if (ATmatch(term, "IntentSpec_OUT")) {

   // MATCHED IntentSpec_OUT

   return ATtrue;
 }

 if (ATmatch(term, "IntentSpec_IN")) {

   // MATCHED IntentSpec_IN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R524 access-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AccessStmt(ATerm term, OFP::AccessStmt* AccessStmt)
{
#ifdef DEBUG_PRINT
   printf("AccessStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::AccessSpec AccessSpec;
 OFP::AccessIdList AccessIdList;
 OFP::EOS EOS;
 if (ATmatch(term, "AccessStmt_AIL(<term>,<term>,<term>,<term>)", &Label.term, &AccessSpec.term, &AccessIdList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_AccessSpec(AccessSpec.term, &AccessSpec)) {
         // MATCHED AccessSpec
      } else return ATfalse;

      if (ofp_traverse_AccessIdList(AccessIdList.term, &AccessIdList)) {
         // MATCHED AccessIdList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED AccessStmt_AIL

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::AccessSpec AccessSpec1;
 OFP::EOS EOS1;
 if (ATmatch(term, "AccessStmt(<term>,<term>,<term>)", &Label1.term, &AccessSpec1.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_AccessSpec(AccessSpec1.term, &AccessSpec1)) {
         // MATCHED AccessSpec
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R525 access-id
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AccessId(ATerm term, OFP::AccessId* AccessId)
{
#ifdef DEBUG_PRINT
   printf("AccessId: %s\n", ATwriteToString(term));
#endif

 OFP::GenericSpec GenericSpec;
 if (ATmatch(term, "AccessId(<term>)", &GenericSpec.term)) {

      if (ofp_traverse_GenericSpec(GenericSpec.term, &GenericSpec)) {
         // MATCHED GenericSpec
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AccessIdList(ATerm term, OFP::AccessIdList* AccessIdList)
{
#ifdef DEBUG_PRINT
   printf("AccessIdList: %s\n", ATwriteToString(term));
#endif

 OFP::AccessId AccessId;
 if (ATmatch(term, "AccessIdList(<term>)", &AccessId.term)) {

   ATermList AccessId_tail = (ATermList) ATmake("<term>", AccessId.term);
   while (! ATisEmpty(AccessId_tail)) {
      AccessId.term = ATgetFirst(AccessId_tail);
      AccessId_tail = ATgetNext (AccessId_tail);
      if (ofp_traverse_AccessId(AccessId.term, &AccessId)) {
         // MATCHED AccessId
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R526 allocatable-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AllocatableStmt(ATerm term, OFP::AllocatableStmt* AllocatableStmt)
{
#ifdef DEBUG_PRINT
   printf("AllocatableStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::AllocatableDeclList AllocatableDeclList;
 OFP::EOS EOS;
 if (ATmatch(term, "AllocatableStmt(<term>,<term>,<term>)", &Label.term, &AllocatableDeclList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_AllocatableDeclList(AllocatableDeclList.term, &AllocatableDeclList)) {
         // MATCHED AllocatableDeclList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R527 allocatable-decl
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AllocatableDecl(ATerm term, OFP::AllocatableDecl* AllocatableDecl)
{
#ifdef DEBUG_PRINT
   printf("AllocatableDecl: %s\n", ATwriteToString(term));
#endif

 OFP::Name ObjectName;
 OFP::ArraySpec ArraySpec;
 OFP::CoarraySpec CoarraySpec;
 if (ATmatch(term, "AllocatableDecl(<term>,<term>,<term>)", &ObjectName.term, &ArraySpec.term, &CoarraySpec.term)) {

      if (ofp_traverse_Name(ObjectName.term, &ObjectName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(ArraySpec.term, "Some(<term>)", &ArraySpec.term)) {
   if (ATmatch(ArraySpec.term, "(<term>)", &ArraySpec.term)) {
      if (ofp_traverse_ArraySpec(ArraySpec.term, &ArraySpec)) {
         // MATCHED ArraySpec
      } else return ATfalse;
   }
   }

   if (ATmatch(CoarraySpec.term, "Some(<term>)", &CoarraySpec.term)) {
   if (ATmatch(CoarraySpec.term, "(<term>)", &CoarraySpec.term)) {
      if (ofp_traverse_CoarraySpec(CoarraySpec.term, &CoarraySpec)) {
         // MATCHED CoarraySpec
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AllocatableDeclList(ATerm term, OFP::AllocatableDeclList* AllocatableDeclList)
{
#ifdef DEBUG_PRINT
   printf("AllocatableDeclList: %s\n", ATwriteToString(term));
#endif

 OFP::AllocatableDecl AllocatableDecl;
 if (ATmatch(term, "AllocatableDeclList(<term>)", &AllocatableDecl.term)) {

   ATermList AllocatableDecl_tail = (ATermList) ATmake("<term>", AllocatableDecl.term);
   while (! ATisEmpty(AllocatableDecl_tail)) {
      AllocatableDecl.term = ATgetFirst(AllocatableDecl_tail);
      AllocatableDecl_tail = ATgetNext (AllocatableDecl_tail);
      if (ofp_traverse_AllocatableDecl(AllocatableDecl.term, &AllocatableDecl)) {
         // MATCHED AllocatableDecl
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R528 asynchronous-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AsynchronousStmt(ATerm term, OFP::AsynchronousStmt* AsynchronousStmt)
{
#ifdef DEBUG_PRINT
   printf("AsynchronousStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ObjectNameList ObjectNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "AsynchronousStmt(<term>,<term>,<term>)", &Label.term, &ObjectNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ObjectNameList(ObjectNameList.term, &ObjectNameList)) {
         // MATCHED ObjectNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R529 bind-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BindStmt(ATerm term, OFP::BindStmt* BindStmt)
{
#ifdef DEBUG_PRINT
   printf("BindStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::LanguageBindingSpec LanguageBindingSpec;
 OFP::BindEntityList BindEntityList;
 OFP::EOS EOS;
 if (ATmatch(term, "BindStmt(<term>,<term>,<term>,<term>)", &Label.term, &LanguageBindingSpec.term, &BindEntityList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_LanguageBindingSpec(LanguageBindingSpec.term, &LanguageBindingSpec)) {
         // MATCHED LanguageBindingSpec
      } else return ATfalse;

      if (ofp_traverse_BindEntityList(BindEntityList.term, &BindEntityList)) {
         // MATCHED BindEntityList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R530 bind-entity
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BindEntity(ATerm term, OFP::BindEntity* BindEntity)
{
#ifdef DEBUG_PRINT
   printf("BindEntity: %s\n", ATwriteToString(term));
#endif

 OFP::Name CommonBlockName;
 if (ATmatch(term, "BindEntity_CBN(<term>)", &CommonBlockName.term)) {

      if (ofp_traverse_Name(CommonBlockName.term, &CommonBlockName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED BindEntity_CBN

   return ATtrue;
 }

 OFP::Name EntityName;
 if (ATmatch(term, "BindEntity_EN(<term>)", &EntityName.term)) {

      if (ofp_traverse_Name(EntityName.term, &EntityName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED BindEntity_EN

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_BindEntityList(ATerm term, OFP::BindEntityList* BindEntityList)
{
#ifdef DEBUG_PRINT
   printf("BindEntityList: %s\n", ATwriteToString(term));
#endif

 OFP::BindEntity BindEntity;
 if (ATmatch(term, "BindEntityList(<term>)", &BindEntity.term)) {

   ATermList BindEntity_tail = (ATermList) ATmake("<term>", BindEntity.term);
   while (! ATisEmpty(BindEntity_tail)) {
      BindEntity.term = ATgetFirst(BindEntity_tail);
      BindEntity_tail = ATgetNext (BindEntity_tail);
      if (ofp_traverse_BindEntity(BindEntity.term, &BindEntity)) {
         // MATCHED BindEntity
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R531 codimension-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CodimensionStmt(ATerm term, OFP::CodimensionStmt* CodimensionStmt)
{
#ifdef DEBUG_PRINT
   printf("CodimensionStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::CodimensionDeclList CodimensionDeclList;
 OFP::EOS EOS;
 if (ATmatch(term, "CodimensionStmt(<term>,<term>,<term>)", &Label.term, &CodimensionDeclList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_CodimensionDeclList(CodimensionDeclList.term, &CodimensionDeclList)) {
         // MATCHED CodimensionDeclList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R532 codimension-decl
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CodimensionDecl(ATerm term, OFP::CodimensionDecl* CodimensionDecl)
{
#ifdef DEBUG_PRINT
   printf("CodimensionDecl: %s\n", ATwriteToString(term));
#endif

 OFP::Name CoarrayName;
 OFP::CoarraySpec CoarraySpec;
 if (ATmatch(term, "CodimensionDecl(<term>,<term>)", &CoarrayName.term, &CoarraySpec.term)) {

      if (ofp_traverse_Name(CoarrayName.term, &CoarrayName)) {
         // MATCHED Name
      } else return ATfalse;

      if (ofp_traverse_CoarraySpec(CoarraySpec.term, &CoarraySpec)) {
         // MATCHED CoarraySpec
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CodimensionDeclList(ATerm term, OFP::CodimensionDeclList* CodimensionDeclList)
{
#ifdef DEBUG_PRINT
   printf("CodimensionDeclList: %s\n", ATwriteToString(term));
#endif

 OFP::CodimensionDecl CodimensionDecl;
 if (ATmatch(term, "CodimensionDeclList(<term>)", &CodimensionDecl.term)) {

   ATermList CodimensionDecl_tail = (ATermList) ATmake("<term>", CodimensionDecl.term);
   while (! ATisEmpty(CodimensionDecl_tail)) {
      CodimensionDecl.term = ATgetFirst(CodimensionDecl_tail);
      CodimensionDecl_tail = ATgetNext (CodimensionDecl_tail);
      if (ofp_traverse_CodimensionDecl(CodimensionDecl.term, &CodimensionDecl)) {
         // MATCHED CodimensionDecl
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R533 contiguous-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ContiguousStmt(ATerm term, OFP::ContiguousStmt* ContiguousStmt)
{
#ifdef DEBUG_PRINT
   printf("ContiguousStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ObjectNameList ObjectNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "ContiguousStmt(<term>,<term>,<term>)", &Label.term, &ObjectNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ObjectNameList(ObjectNameList.term, &ObjectNameList)) {
         // MATCHED ObjectNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ObjectNameList(ATerm term, OFP::ObjectNameList* ObjectNameList)
{
#ifdef DEBUG_PRINT
   printf("ObjectNameList: %s\n", ATwriteToString(term));
#endif

 OFP::Name ObjectName;
 if (ATmatch(term, "ObjectNameList(<term>)", &ObjectName.term)) {

   ATermList ObjectName_tail = (ATermList) ATmake("<term>", ObjectName.term);
   while (! ATisEmpty(ObjectName_tail)) {
      ObjectName.term = ATgetFirst(ObjectName_tail);
      ObjectName_tail = ATgetNext (ObjectName_tail);
      if (ofp_traverse_Name(ObjectName.term, &ObjectName)) {
         // MATCHED Name
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R534 data-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataStmt(ATerm term, OFP::DataStmt* DataStmt)
{
#ifdef DEBUG_PRINT
   printf("DataStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::DataStmtSetList DataStmtSetList;
 OFP::EOS EOS;
 if (ATmatch(term, "DataStmt(<term>,<term>,<term>)", &Label.term, &DataStmtSetList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_DataStmtSetList(DataStmtSetList.term, &DataStmtSetList)) {
         // MATCHED DataStmtSetList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R535 data-stmt-set
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataStmtSet(ATerm term, OFP::DataStmtSet* DataStmtSet)
{
#ifdef DEBUG_PRINT
   printf("DataStmtSet: %s\n", ATwriteToString(term));
#endif

 OFP::DataStmtObjectList DataStmtObjectList;
 OFP::DataStmtValueList DataStmtValueList;
 if (ATmatch(term, "DataStmtSet(<term>,<term>)", &DataStmtObjectList.term, &DataStmtValueList.term)) {

      if (ofp_traverse_DataStmtObjectList(DataStmtObjectList.term, &DataStmtObjectList)) {
         // MATCHED DataStmtObjectList
      } else return ATfalse;

      if (ofp_traverse_DataStmtValueList(DataStmtValueList.term, &DataStmtValueList)) {
         // MATCHED DataStmtValueList
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DataStmtSetList(ATerm term, OFP::DataStmtSetList* DataStmtSetList)
{
#ifdef DEBUG_PRINT
   printf("DataStmtSetList: %s\n", ATwriteToString(term));
#endif

 OFP::DataStmtSetList DataStmtSetList1;
 OFP::DataStmtSet DataStmtSet;
 if (ATmatch(term, "DataStmtSetList_2(<term>,<term>)", &DataStmtSetList1.term, &DataStmtSet.term)) {

      if (ofp_traverse_DataStmtSetList(DataStmtSetList1.term, &DataStmtSetList1)) {
         // MATCHED DataStmtSetList
      } else return ATfalse;

      if (ofp_traverse_DataStmtSet(DataStmtSet.term, &DataStmtSet)) {
         // MATCHED DataStmtSet
      } else return ATfalse;

   // MATCHED DataStmtSetList_2

   return ATtrue;
 }

 OFP::DataStmtSet DataStmtSet1;
 if (ATmatch(term, "DataStmtSetList_1(<term>)", &DataStmtSet1.term)) {

      if (ofp_traverse_DataStmtSet(DataStmtSet1.term, &DataStmtSet1)) {
         // MATCHED DataStmtSet
      } else return ATfalse;

   // MATCHED DataStmtSetList_1

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R536 data-stmt-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataStmtObject(ATerm term, OFP::DataStmtObject* DataStmtObject)
{
#ifdef DEBUG_PRINT
   printf("DataStmtObject: %s\n", ATwriteToString(term));
#endif

 OFP::DataImpliedDo DataImpliedDo;
 if (ATmatch(term, "DataStmtObject_DID(<term>)", &DataImpliedDo.term)) {

      if (ofp_traverse_DataImpliedDo(DataImpliedDo.term, &DataImpliedDo)) {
         // MATCHED DataImpliedDo
      } else return ATfalse;

   // MATCHED DataStmtObject_DID

   return ATtrue;
 }

 OFP::Variable Variable;
 if (ATmatch(term, "DataStmtObject_V(<term>)", &Variable.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

   // MATCHED DataStmtObject_V

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DataStmtObjectList(ATerm term, OFP::DataStmtObjectList* DataStmtObjectList)
{
#ifdef DEBUG_PRINT
   printf("DataStmtObjectList: %s\n", ATwriteToString(term));
#endif

 OFP::DataStmtObject DataStmtObject;
 if (ATmatch(term, "DataStmtObjectList(<term>)", &DataStmtObject.term)) {

   ATermList DataStmtObject_tail = (ATermList) ATmake("<term>", DataStmtObject.term);
   while (! ATisEmpty(DataStmtObject_tail)) {
      DataStmtObject.term = ATgetFirst(DataStmtObject_tail);
      DataStmtObject_tail = ATgetNext (DataStmtObject_tail);
      if (ofp_traverse_DataStmtObject(DataStmtObject.term, &DataStmtObject)) {
         // MATCHED DataStmtObject
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R537 data-implied-do
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataImpliedDo(ATerm term, OFP::DataImpliedDo* DataImpliedDo)
{
#ifdef DEBUG_PRINT
   printf("DataImpliedDo: %s\n", ATwriteToString(term));
#endif

 OFP::DataIDoObjectList DataIDoObjectList;
 OFP::DataIDoVariable DataIDoVariable;
 OFP::Expr Expr;
 OFP::Expr Expr1;
 OFP::Expr Expr2;
 if (ATmatch(term, "DataImpliedDo(<term>,<term>,<term>,<term>,<term>)", &DataIDoObjectList.term, &DataIDoVariable.term, &Expr.term, &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_DataIDoObjectList(DataIDoObjectList.term, &DataIDoObjectList)) {
         // MATCHED DataIDoObjectList
      } else return ATfalse;

      if (ofp_traverse_DataIDoVariable(DataIDoVariable.term, &DataIDoVariable)) {
         // MATCHED DataIDoVariable
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
      } else return ATfalse;

   if (ATmatch(Expr2.term, "Some(<term>)", &Expr2.term)) {
   if (ATmatch(Expr2.term, "(<term>)", &Expr2.term)) {
      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R538 data-i-do-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataIDoObject(ATerm term, OFP::DataIDoObject* DataIDoObject)
{
#ifdef DEBUG_PRINT
   printf("DataIDoObject: %s\n", ATwriteToString(term));
#endif

 OFP::DataImpliedDo DataImpliedDo;
 if (ATmatch(term, "DataIDoObject_DID(<term>)", &DataImpliedDo.term)) {

      if (ofp_traverse_DataImpliedDo(DataImpliedDo.term, &DataImpliedDo)) {
         // MATCHED DataImpliedDo
      } else return ATfalse;

   // MATCHED DataIDoObject_DID

   return ATtrue;
 }

 OFP::StructureComponent StructureComponent;
 if (ATmatch(term, "DataIDoObject_SC(<term>)", &StructureComponent.term)) {

      if (ofp_traverse_StructureComponent(StructureComponent.term, &StructureComponent)) {
         // MATCHED StructureComponent
      } else return ATfalse;

   // MATCHED DataIDoObject_SC

   return ATtrue;
 }

 OFP::ArrayElement ArrayElement;
 if (ATmatch(term, "DataIDoObject_AE(<term>)", &ArrayElement.term)) {

      if (ofp_traverse_ArrayElement(ArrayElement.term, &ArrayElement)) {
         // MATCHED ArrayElement
      } else return ATfalse;

   // MATCHED DataIDoObject_AE

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DataIDoObjectList(ATerm term, OFP::DataIDoObjectList* DataIDoObjectList)
{
#ifdef DEBUG_PRINT
   printf("DataIDoObjectList: %s\n", ATwriteToString(term));
#endif

 OFP::DataIDoObject DataIDoObject;
 if (ATmatch(term, "DataIDoObjectList(<term>)", &DataIDoObject.term)) {

   ATermList DataIDoObject_tail = (ATermList) ATmake("<term>", DataIDoObject.term);
   while (! ATisEmpty(DataIDoObject_tail)) {
      DataIDoObject.term = ATgetFirst(DataIDoObject_tail);
      DataIDoObject_tail = ATgetNext (DataIDoObject_tail);
      if (ofp_traverse_DataIDoObject(DataIDoObject.term, &DataIDoObject)) {
         // MATCHED DataIDoObject
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R539 data-i-do-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataIDoVariable(ATerm term, OFP::DataIDoVariable* DataIDoVariable)
{
#ifdef DEBUG_PRINT
   printf("DataIDoVariable: %s\n", ATwriteToString(term));
#endif

 OFP::DoVariable DoVariable;
 if (ATmatch(term, "DataIDoVariable(<term>)", &DoVariable.term)) {

      if (ofp_traverse_DoVariable(DoVariable.term, &DoVariable)) {
         // MATCHED DoVariable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R540 data-stmt-value
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataStmtValue(ATerm term, OFP::DataStmtValue* DataStmtValue)
{
#ifdef DEBUG_PRINT
   printf("DataStmtValue: %s\n", ATwriteToString(term));
#endif

 OFP::DataStmtRepeat DataStmtRepeat;
 OFP::DataStmtConstant DataStmtConstant;
 if (ATmatch(term, "DataStmtValue(<term>,<term>)", &DataStmtRepeat.term, &DataStmtConstant.term)) {

   if (ATmatch(DataStmtRepeat.term, "Some(<term>)", &DataStmtRepeat.term)) {
   if (ATmatch(DataStmtRepeat.term, "(<term>)", &DataStmtRepeat.term)) {
      if (ofp_traverse_DataStmtRepeat(DataStmtRepeat.term, &DataStmtRepeat)) {
         // MATCHED DataStmtRepeat
      } else return ATfalse;
   }
   }

      if (ofp_traverse_DataStmtConstant(DataStmtConstant.term, &DataStmtConstant)) {
         // MATCHED DataStmtConstant
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DataStmtValueList(ATerm term, OFP::DataStmtValueList* DataStmtValueList)
{
#ifdef DEBUG_PRINT
   printf("DataStmtValueList: %s\n", ATwriteToString(term));
#endif

 OFP::DataStmtValue DataStmtValue;
 if (ATmatch(term, "DataStmtValueList(<term>)", &DataStmtValue.term)) {

   ATermList DataStmtValue_tail = (ATermList) ATmake("<term>", DataStmtValue.term);
   while (! ATisEmpty(DataStmtValue_tail)) {
      DataStmtValue.term = ATgetFirst(DataStmtValue_tail);
      DataStmtValue_tail = ATgetNext (DataStmtValue_tail);
      if (ofp_traverse_DataStmtValue(DataStmtValue.term, &DataStmtValue)) {
         // MATCHED DataStmtValue
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R541 data-stmt-repeat
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataStmtRepeat(ATerm term, OFP::DataStmtRepeat* DataStmtRepeat)
{
#ifdef DEBUG_PRINT
   printf("DataStmtRepeat: %s\n", ATwriteToString(term));
#endif

 OFP::IntConstantSubobject IntConstantSubobject;
 if (ATmatch(term, "DataStmtRepeat_ICS(<term>)", &IntConstantSubobject.term)) {

      if (ofp_traverse_IntConstantSubobject(IntConstantSubobject.term, &IntConstantSubobject)) {
         // MATCHED IntConstantSubobject
      } else return ATfalse;

   // MATCHED DataStmtRepeat_ICS

   return ATtrue;
 }

 OFP::Icon Icon;
 if (ATmatch(term, "DataStmtRepeat_IC(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED DataStmtRepeat_IC

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R542 data-stmt-constant
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataStmtConstant(ATerm term, OFP::DataStmtConstant* DataStmtConstant)
{
#ifdef DEBUG_PRINT
   printf("DataStmtConstant: %s\n", ATwriteToString(term));
#endif

 OFP::StructureConstructor StructureConstructor;
 if (ATmatch(term, "DataStmtConstant_SC(<term>)", &StructureConstructor.term)) {

      if (ofp_traverse_StructureConstructor(StructureConstructor.term, &StructureConstructor)) {
         // MATCHED StructureConstructor
      } else return ATfalse;

   // MATCHED DataStmtConstant_SC

   return ATtrue;
 }

 OFP::InitialDataTarget InitialDataTarget;
 if (ATmatch(term, "DataStmtConstant_IDT(<term>)", &InitialDataTarget.term)) {

      if (ofp_traverse_InitialDataTarget(InitialDataTarget.term, &InitialDataTarget)) {
         // MATCHED InitialDataTarget
      } else return ATfalse;

   // MATCHED DataStmtConstant_IDT

   return ATtrue;
 }

 OFP::NullInit NullInit;
 if (ATmatch(term, "DataStmtConstant_NI(<term>)", &NullInit.term)) {

      if (ofp_traverse_NullInit(NullInit.term, &NullInit)) {
         // MATCHED NullInit
      } else return ATfalse;

   // MATCHED DataStmtConstant_NI

   return ATtrue;
 }

 OFP::SignedRealLiteralConstant SignedRealLiteralConstant;
 if (ATmatch(term, "DataStmtConstant_SRLC(<term>)", &SignedRealLiteralConstant.term)) {

      if (ofp_traverse_SignedRealLiteralConstant(SignedRealLiteralConstant.term, &SignedRealLiteralConstant)) {
         // MATCHED SignedRealLiteralConstant
      } else return ATfalse;

   // MATCHED DataStmtConstant_SRLC

   return ATtrue;
 }

 OFP::SignedIntLiteralConstant SignedIntLiteralConstant;
 if (ATmatch(term, "DataStmtConstant_SILC(<term>)", &SignedIntLiteralConstant.term)) {

      if (ofp_traverse_SignedIntLiteralConstant(SignedIntLiteralConstant.term, &SignedIntLiteralConstant)) {
         // MATCHED SignedIntLiteralConstant
      } else return ATfalse;

   // MATCHED DataStmtConstant_SILC

   return ATtrue;
 }

 OFP::ConstantSubobject ConstantSubobject;
 if (ATmatch(term, "DataStmtConstant_CS(<term>)", &ConstantSubobject.term)) {

      if (ofp_traverse_ConstantSubobject(ConstantSubobject.term, &ConstantSubobject)) {
         // MATCHED ConstantSubobject
      } else return ATfalse;

   // MATCHED DataStmtConstant_CS

   return ATtrue;
 }

 OFP::Constant Constant;
 if (ATmatch(term, "DataStmtConstant_C(<term>)", &Constant.term)) {

      if (ofp_traverse_Constant(Constant.term, &Constant)) {
         // MATCHED Constant
      } else return ATfalse;

   // MATCHED DataStmtConstant_C

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R543 int-constant-subobject
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntConstantSubobject(ATerm term, OFP::IntConstantSubobject* IntConstantSubobject)
{
#ifdef DEBUG_PRINT
   printf("IntConstantSubobject: %s\n", ATwriteToString(term));
#endif

 OFP::ConstantSubobject ConstantSubobject;
 if (ATmatch(term, "IntConstantSubobject(<term>)", &ConstantSubobject.term)) {

      if (ofp_traverse_ConstantSubobject(ConstantSubobject.term, &ConstantSubobject)) {
         // MATCHED ConstantSubobject
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R544 constant-subobject
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ConstantSubobject(ATerm term, OFP::ConstantSubobject* ConstantSubobject)
{
#ifdef DEBUG_PRINT
   printf("ConstantSubobject: %s\n", ATwriteToString(term));
#endif

 OFP::Designator Designator;
 if (ATmatch(term, "ConstantSubobject(<term>)", &Designator.term)) {

      if (ofp_traverse_Designator(Designator.term, &Designator)) {
         // MATCHED Designator
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R545 dimension-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DimensionStmt(ATerm term, OFP::DimensionStmt* DimensionStmt)
{
#ifdef DEBUG_PRINT
   printf("DimensionStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ArrayNameSpecList ArrayNameSpecList;
 OFP::EOS EOS;
 if (ATmatch(term, "DimensionStmt(<term>,<term>,<term>)", &Label.term, &ArrayNameSpecList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ArrayNameSpecList(ArrayNameSpecList.term, &ArrayNameSpecList)) {
         // MATCHED ArrayNameSpecList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ArrayNameSpec(ATerm term, OFP::ArrayNameSpec* ArrayNameSpec)
{
#ifdef DEBUG_PRINT
   printf("ArrayNameSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Name ArrayName;
 OFP::ArraySpec ArraySpec;
 if (ATmatch(term, "ArrayNameSpec(<term>,<term>)", &ArrayName.term, &ArraySpec.term)) {

      if (ofp_traverse_Name(ArrayName.term, &ArrayName)) {
         // MATCHED Name
      } else return ATfalse;

      if (ofp_traverse_ArraySpec(ArraySpec.term, &ArraySpec)) {
         // MATCHED ArraySpec
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ArrayNameSpecList(ATerm term, OFP::ArrayNameSpecList* ArrayNameSpecList)
{
#ifdef DEBUG_PRINT
   printf("ArrayNameSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ArrayNameSpec ArrayNameSpec;
 if (ATmatch(term, "ArrayNameSpecList(<term>)", &ArrayNameSpec.term)) {

   ATermList ArrayNameSpec_tail = (ATermList) ATmake("<term>", ArrayNameSpec.term);
   while (! ATisEmpty(ArrayNameSpec_tail)) {
      ArrayNameSpec.term = ATgetFirst(ArrayNameSpec_tail);
      ArrayNameSpec_tail = ATgetNext (ArrayNameSpec_tail);
      if (ofp_traverse_ArrayNameSpec(ArrayNameSpec.term, &ArrayNameSpec)) {
         // MATCHED ArrayNameSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R546 intent-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntentStmt(ATerm term, OFP::IntentStmt* IntentStmt)
{
#ifdef DEBUG_PRINT
   printf("IntentStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::IntentSpec IntentSpec;
 OFP::DummyArgNameList DummyArgNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "IntentStmt(<term>,<term>,<term>,<term>)", &Label.term, &IntentSpec.term, &DummyArgNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_IntentSpec(IntentSpec.term, &IntentSpec)) {
         // MATCHED IntentSpec
      } else return ATfalse;

      if (ofp_traverse_DummyArgNameList(DummyArgNameList.term, &DummyArgNameList)) {
         // MATCHED DummyArgNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DummyArgNameList(ATerm term, OFP::DummyArgNameList* DummyArgNameList)
{
#ifdef DEBUG_PRINT
   printf("DummyArgNameList: %s\n", ATwriteToString(term));
#endif

 OFP::DummyArgName DummyArgName;
 if (ATmatch(term, "DummyArgNameList(<term>)", &DummyArgName.term)) {

   ATermList DummyArgName_tail = (ATermList) ATmake("<term>", DummyArgName.term);
   while (! ATisEmpty(DummyArgName_tail)) {
      DummyArgName.term = ATgetFirst(DummyArgName_tail);
      DummyArgName_tail = ATgetNext (DummyArgName_tail);
      if (ofp_traverse_DummyArgName(DummyArgName.term, &DummyArgName)) {
         // MATCHED DummyArgName
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R547 optional-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_OptionalStmt(ATerm term, OFP::OptionalStmt* OptionalStmt)
{
#ifdef DEBUG_PRINT
   printf("OptionalStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::DummyArgNameList DummyArgNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "OptionalStmt(<term>,<term>,<term>)", &Label.term, &DummyArgNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_DummyArgNameList(DummyArgNameList.term, &DummyArgNameList)) {
         // MATCHED DummyArgNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R548 parameter-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ParameterStmt(ATerm term, OFP::ParameterStmt* ParameterStmt)
{
#ifdef DEBUG_PRINT
   printf("ParameterStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::NamedConstantDefList NamedConstantDefList;
 OFP::EOS EOS;
 if (ATmatch(term, "ParameterStmt(<term>,<term>,<term>)", &Label.term, &NamedConstantDefList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_NamedConstantDefList(NamedConstantDefList.term, &NamedConstantDefList)) {
         // MATCHED NamedConstantDefList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R549 named-constant-def
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_NamedConstantDef(ATerm term, OFP::NamedConstantDef* NamedConstantDef)
{
#ifdef DEBUG_PRINT
   printf("NamedConstantDef: %s\n", ATwriteToString(term));
#endif

 OFP::NamedConstant NamedConstant;
 OFP::ConstantExpr ConstantExpr;
 if (ATmatch(term, "NamedConstantDef(<term>,<term>)", &NamedConstant.term, &ConstantExpr.term)) {

      if (ofp_traverse_NamedConstant(NamedConstant.term, &NamedConstant)) {
         // MATCHED NamedConstant
      } else return ATfalse;

      if (ofp_traverse_ConstantExpr(ConstantExpr.term, &ConstantExpr)) {
         // MATCHED ConstantExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_NamedConstantDefList(ATerm term, OFP::NamedConstantDefList* NamedConstantDefList)
{
#ifdef DEBUG_PRINT
   printf("NamedConstantDefList: %s\n", ATwriteToString(term));
#endif

 OFP::NamedConstantDef NamedConstantDef;
 if (ATmatch(term, "NamedConstantDefList(<term>)", &NamedConstantDef.term)) {

   ATermList NamedConstantDef_tail = (ATermList) ATmake("<term>", NamedConstantDef.term);
   while (! ATisEmpty(NamedConstantDef_tail)) {
      NamedConstantDef.term = ATgetFirst(NamedConstantDef_tail);
      NamedConstantDef_tail = ATgetNext (NamedConstantDef_tail);
      if (ofp_traverse_NamedConstantDef(NamedConstantDef.term, &NamedConstantDef)) {
         // MATCHED NamedConstantDef
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R550 pointer-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PointerStmt(ATerm term, OFP::PointerStmt* PointerStmt)
{
#ifdef DEBUG_PRINT
   printf("PointerStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::PointerDeclList PointerDeclList;
 OFP::EOS EOS;
 if (ATmatch(term, "PointerStmt(<term>,<term>,<term>)", &Label.term, &PointerDeclList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         PointerStmt->setLabel(Label.newLabel());
      } else return ATfalse;
   }

      if (ofp_traverse_PointerDeclList(PointerDeclList.term, &PointerDeclList)) {
         // MATCHED PointerDeclList
         PointerStmt->setPointerDeclList(PointerDeclList.newPointerDeclList());
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         PointerStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R551 pointer-decl
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PointerDecl(ATerm term, OFP::PointerDecl* PointerDecl)
{
#ifdef DEBUG_PRINT
   printf("PointerDecl: %s\n", ATwriteToString(term));
#endif

 OFP::Name ProcEntityName;
 if (ATmatch(term, "PointerDecl_PEN(<term>)", &ProcEntityName.term)) {

      if (ofp_traverse_Name(ProcEntityName.term, &ProcEntityName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED PointerDecl_PEN

   return ATtrue;
 }

 OFP::Name ObjectName;
 OFP::DeferredShapeSpecList DeferredShapeSpecList;
 if (ATmatch(term, "PointerDecl_ON(<term>,<term>)", &ObjectName.term, &DeferredShapeSpecList.term)) {

      if (ofp_traverse_Name(ObjectName.term, &ObjectName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(DeferredShapeSpecList.term, "Some(<term>)", &DeferredShapeSpecList.term)) {
   if (ATmatch(DeferredShapeSpecList.term, "(<term>)", &DeferredShapeSpecList.term)) {
      if (ofp_traverse_DeferredShapeSpecList(DeferredShapeSpecList.term, &DeferredShapeSpecList)) {
         // MATCHED DeferredShapeSpecList
      } else return ATfalse;
   }
   }

   // MATCHED PointerDecl_ON

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_PointerDeclList(ATerm term, OFP::PointerDeclList* PointerDeclList)
{
#ifdef DEBUG_PRINT
   printf("PointerDeclList: %s\n", ATwriteToString(term));
#endif

 OFP::PointerDecl PointerDecl;
 if (ATmatch(term, "PointerDeclList(<term>)", &PointerDecl.term)) {

   ATermList PointerDecl_tail = (ATermList) ATmake("<term>", PointerDecl.term);
   while (! ATisEmpty(PointerDecl_tail)) {
      PointerDecl.term = ATgetFirst(PointerDecl_tail);
      PointerDecl_tail = ATgetNext (PointerDecl_tail);
      if (ofp_traverse_PointerDecl(PointerDecl.term, &PointerDecl)) {
         // MATCHED PointerDecl
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R552 protected-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProtectedStmt(ATerm term, OFP::ProtectedStmt* ProtectedStmt)
{
#ifdef DEBUG_PRINT
   printf("ProtectedStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EntityNameList EntityNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "ProtectedStmt(<term>,<term>,<term>)", &Label.term, &EntityNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EntityNameList(EntityNameList.term, &EntityNameList)) {
         // MATCHED EntityNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_EntityNameList(ATerm term, OFP::EntityNameList* EntityNameList)
{
#ifdef DEBUG_PRINT
   printf("EntityNameList: %s\n", ATwriteToString(term));
#endif

 OFP::Name EntityName;
 if (ATmatch(term, "EntityNameList(<term>)", &EntityName.term)) {

   ATermList EntityName_tail = (ATermList) ATmake("<term>", EntityName.term);
   while (! ATisEmpty(EntityName_tail)) {
      EntityName.term = ATgetFirst(EntityName_tail);
      EntityName_tail = ATgetNext (EntityName_tail);
      if (ofp_traverse_Name(EntityName.term, &EntityName)) {
         // MATCHED Name
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R553 save-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SaveStmt(ATerm term, OFP::SaveStmt* SaveStmt)
{
#ifdef DEBUG_PRINT
   printf("SaveStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "SaveStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::SavedEntityList SavedEntityList;
 OFP::EOS EOS1;
 if (ATmatch(term, "SaveStmt_SEL(<term>,<term>,<term>)", &Label1.term, &SavedEntityList.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_SavedEntityList(SavedEntityList.term, &SavedEntityList)) {
         // MATCHED SavedEntityList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED SaveStmt_SEL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R554 saved-entity
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SavedEntity(ATerm term, OFP::SavedEntity* SavedEntity)
{
#ifdef DEBUG_PRINT
   printf("SavedEntity: %s\n", ATwriteToString(term));
#endif

 OFP::Name CommonBlockName;
 if (ATmatch(term, "SavedEntity(<term>)", &CommonBlockName.term)) {

      if (ofp_traverse_Name(CommonBlockName.term, &CommonBlockName)) {
         // MATCHED Name
      } else return ATfalse;

   return ATtrue;
 }

 OFP::Name ObjectName;
 if (ATmatch(term, "SavedEntity(<term>)", &ObjectName.term)) {

      if (ofp_traverse_Name(ObjectName.term, &ObjectName)) {
         // MATCHED Name
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_SavedEntityList(ATerm term, OFP::SavedEntityList* SavedEntityList)
{
#ifdef DEBUG_PRINT
   printf("SavedEntityList: %s\n", ATwriteToString(term));
#endif

 OFP::SavedEntity SavedEntity;
 if (ATmatch(term, "SavedEntityList(<term>)", &SavedEntity.term)) {

   ATermList SavedEntity_tail = (ATermList) ATmake("<term>", SavedEntity.term);
   while (! ATisEmpty(SavedEntity_tail)) {
      SavedEntity.term = ATgetFirst(SavedEntity_tail);
      SavedEntity_tail = ATgetNext (SavedEntity_tail);
      if (ofp_traverse_SavedEntity(SavedEntity.term, &SavedEntity)) {
         // MATCHED SavedEntity
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R555 proc-pointer-name
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcPointerName(ATerm term, OFP::ProcPointerName* ProcPointerName)
{
#ifdef DEBUG_PRINT
   printf("ProcPointerName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ProcPointerName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R556 target-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TargetStmt(ATerm term, OFP::TargetStmt* TargetStmt)
{
#ifdef DEBUG_PRINT
   printf("TargetStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::TargetDeclList TargetDeclList;
 OFP::EOS EOS;
 if (ATmatch(term, "TargetStmt(<term>,<term>,<term>)", &Label.term, &TargetDeclList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_TargetDeclList(TargetDeclList.term, &TargetDeclList)) {
         // MATCHED TargetDeclList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R557 target-decl
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TargetDecl(ATerm term, OFP::TargetDecl* TargetDecl)
{
#ifdef DEBUG_PRINT
   printf("TargetDecl: %s\n", ATwriteToString(term));
#endif

 OFP::Name ObjectName;
 OFP::ArraySpec ArraySpec;
 OFP::CoarraySpec CoarraySpec;
 if (ATmatch(term, "TargetDecl(<term>,<term>,<term>)", &ObjectName.term, &ArraySpec.term, &CoarraySpec.term)) {

      if (ofp_traverse_Name(ObjectName.term, &ObjectName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(ArraySpec.term, "Some(<term>)", &ArraySpec.term)) {
   if (ATmatch(ArraySpec.term, "(<term>)", &ArraySpec.term)) {
      if (ofp_traverse_ArraySpec(ArraySpec.term, &ArraySpec)) {
         // MATCHED ArraySpec
      } else return ATfalse;
   }
   }

   if (ATmatch(CoarraySpec.term, "Some(<term>)", &CoarraySpec.term)) {
   if (ATmatch(CoarraySpec.term, "(<term>)", &CoarraySpec.term)) {
      if (ofp_traverse_CoarraySpec(CoarraySpec.term, &CoarraySpec)) {
         // MATCHED CoarraySpec
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_TargetDeclList(ATerm term, OFP::TargetDeclList* TargetDeclList)
{
#ifdef DEBUG_PRINT
   printf("TargetDeclList: %s\n", ATwriteToString(term));
#endif

 OFP::TargetDecl TargetDecl;
 if (ATmatch(term, "TargetDeclList(<term>)", &TargetDecl.term)) {

   ATermList TargetDecl_tail = (ATermList) ATmake("<term>", TargetDecl.term);
   while (! ATisEmpty(TargetDecl_tail)) {
      TargetDecl.term = ATgetFirst(TargetDecl_tail);
      TargetDecl_tail = ATgetNext (TargetDecl_tail);
      if (ofp_traverse_TargetDecl(TargetDecl.term, &TargetDecl)) {
         // MATCHED TargetDecl
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R558 value-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ValueStmt(ATerm term, OFP::ValueStmt* ValueStmt)
{
#ifdef DEBUG_PRINT
   printf("ValueStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::DummyArgNameList DummyArgNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "ValueStmt(<term>,<term>,<term>)", &Label.term, &DummyArgNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_DummyArgNameList(DummyArgNameList.term, &DummyArgNameList)) {
         // MATCHED DummyArgNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R559 volatile-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_VolatileStmt(ATerm term, OFP::VolatileStmt* VolatileStmt)
{
#ifdef DEBUG_PRINT
   printf("VolatileStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ObjectNameList ObjectNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "VolatileStmt(<term>,<term>,<term>)", &Label.term, &ObjectNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ObjectNameList(ObjectNameList.term, &ObjectNameList)) {
         // MATCHED ObjectNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R560 implicit-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ImplicitStmt(ATerm term, OFP::ImplicitStmt* ImplicitStmt)
{
#ifdef DEBUG_PRINT
   printf("ImplicitStmt(W): %s\n", ATwriteToString(term));
#endif

   OFP::Label Label;
   OFP::ImplicitSpecList ImplicitSpecList;
   OFP::EOS EOS;
   if (ATmatch(term, "ImplicitNoneStmt(<term>,<term>)", &Label.term, &EOS.term)) {

      // MATCHED ImplicitNoneStmt
      ImplicitStmt->setOptionType(OFP::ImplicitStmt::ImplicitStmt_NONE);

   }
   else if (ATmatch(term, "ImplicitStmt(<term>,<term>,<term>)", &Label.term, &ImplicitSpecList.term, &EOS.term)) {

      if (ofp_traverse_ImplicitSpecList(ImplicitSpecList.term, &ImplicitSpecList)) {
         // MATCHED ImplicitSpecList
         ImplicitStmt->setImplicitSpecList(ImplicitSpecList.newImplicitSpecList());
      } else return ATfalse;

   } else return ATfalse;

   // Matched either ImplicitNoneStmt or ImplicitStmt so traverse common children

   if (ofp_traverse_Label(Label.term, &Label)) {
      // MATCHED Label
      ImplicitStmt->setLabel(Label.newLabel());
   } // Optional

   if (ofp_traverse_EOS(EOS.term, &EOS)) {
      // MATCHED EOS
      ImplicitStmt->setEOS(EOS.newEOS());
   } else return ATfalse;

   ast->build_ImplicitStmt(ImplicitStmt);

   return ATtrue;
}

//========================================================================================
// R561 implicit-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ImplicitSpec(ATerm term, OFP::ImplicitSpec* ImplicitSpec)
{
#ifdef DEBUG_PRINT
   printf("ImplicitSpec: %s\n", ATwriteToString(term));
#endif

 OFP::DeclarationTypeSpec DeclarationTypeSpec;
 OFP::LetterSpecList LetterSpecList;
 if (ATmatch(term, "ImplicitSpec(<term>,<term>)", &DeclarationTypeSpec.term, &LetterSpecList.term)) {

      if (ofp_traverse_DeclarationTypeSpec(DeclarationTypeSpec.term, &DeclarationTypeSpec)) {
         // MATCHED DeclarationTypeSpec
      } else return ATfalse;

      if (ofp_traverse_LetterSpecList(LetterSpecList.term, &LetterSpecList)) {
         // MATCHED LetterSpecList
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ImplicitSpecList(ATerm term, OFP::ImplicitSpecList* ImplicitSpecList)
{
#ifdef DEBUG_PRINT
   printf("ImplicitSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ImplicitSpec ImplicitSpec;
 if (ATmatch(term, "ImplicitSpecList(<term>)", &ImplicitSpec.term)) {

   ATermList ImplicitSpec_tail = (ATermList) ATmake("<term>", ImplicitSpec.term);
   while (! ATisEmpty(ImplicitSpec_tail)) {
      ImplicitSpec.term = ATgetFirst(ImplicitSpec_tail);
      ImplicitSpec_tail = ATgetNext (ImplicitSpec_tail);
      if (ofp_traverse_ImplicitSpec(ImplicitSpec.term, &ImplicitSpec)) {
         // MATCHED ImplicitSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R562 letter-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LetterSpec(ATerm term, OFP::LetterSpec* LetterSpec)
{
#ifdef DEBUG_PRINT
   printf("LetterSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Letter Letter;
 OFP::Letter Letter1;
 if (ATmatch(term, "LetterSpec(<term>,<term>)", &Letter.term, &Letter1.term)) {

      if (ofp_traverse_Letter(Letter.term, &Letter)) {
         // MATCHED Letter
      } else return ATfalse;

   if (ATmatch(Letter1.term, "Some(<term>)", &Letter1.term)) {
   if (ATmatch(Letter1.term, "(<term>)", &Letter1.term)) {
      if (ofp_traverse_Letter(Letter1.term, &Letter1)) {
         // MATCHED Letter
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_LetterSpecList(ATerm term, OFP::LetterSpecList* LetterSpecList)
{
#ifdef DEBUG_PRINT
   printf("LetterSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::LetterSpec LetterSpec;
 if (ATmatch(term, "LetterSpecList(<term>)", &LetterSpec.term)) {

   ATermList LetterSpec_tail = (ATermList) ATmake("<term>", LetterSpec.term);
   while (! ATisEmpty(LetterSpec_tail)) {
      LetterSpec.term = ATgetFirst(LetterSpec_tail);
      LetterSpec_tail = ATgetNext (LetterSpec_tail);
      if (ofp_traverse_LetterSpec(LetterSpec.term, &LetterSpec)) {
         // MATCHED LetterSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R563 namelist-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_NamelistStmt(ATerm term, OFP::NamelistStmt* NamelistStmt)
{
#ifdef DEBUG_PRINT
   printf("NamelistStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::NamelistEntryList NamelistEntryList;
 OFP::EOS EOS;
 if (ATmatch(term, "NamelistStmt(<term>,<term>,<term>)", &Label.term, &NamelistEntryList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_NamelistEntryList(NamelistEntryList.term, &NamelistEntryList)) {
         // MATCHED NamelistEntryList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_NamelistEntry(ATerm term, OFP::NamelistEntry* NamelistEntry)
{
#ifdef DEBUG_PRINT
   printf("NamelistEntry: %s\n", ATwriteToString(term));
#endif

 OFP::Name NamelistGroupName;
 OFP::NamelistGroupObjectList NamelistGroupObjectList;
 if (ATmatch(term, "NamelistEntry(<term>,<term>)", &NamelistGroupName.term, &NamelistGroupObjectList.term)) {

      if (ofp_traverse_Name(NamelistGroupName.term, &NamelistGroupName)) {
         // MATCHED Name
      } else return ATfalse;

      if (ofp_traverse_NamelistGroupObjectList(NamelistGroupObjectList.term, &NamelistGroupObjectList)) {
         // MATCHED NamelistGroupObjectList
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_NamelistEntryList(ATerm term, OFP::NamelistEntryList* NamelistEntryList)
{
#ifdef DEBUG_PRINT
   printf("NamelistEntryList: %s\n", ATwriteToString(term));
#endif

 OFP::NamelistEntry NamelistEntry;
 if (ATmatch(term, "NamelistEntryList(<term>)", &NamelistEntry.term)) {

      if (ofp_traverse_NamelistEntry(NamelistEntry.term, &NamelistEntry)) {
         // MATCHED NamelistEntry
      } else return ATfalse;

   return ATtrue;
 }

 OFP::NamelistEntryList NamelistEntryList1;
 OFP::NamelistEntry NamelistEntry1;
 if (ATmatch(term, "NamelistEntryList_NEL(<term>,<term>)", &NamelistEntryList1.term, &NamelistEntry1.term)) {

      if (ofp_traverse_NamelistEntryList(NamelistEntryList1.term, &NamelistEntryList1)) {
         // MATCHED NamelistEntryList
      } else return ATfalse;

      if (ofp_traverse_NamelistEntry(NamelistEntry1.term, &NamelistEntry1)) {
         // MATCHED NamelistEntry
      } else return ATfalse;

   // MATCHED NamelistEntryList_NEL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R564 namelist-group-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_NamelistGroupObject(ATerm term, OFP::NamelistGroupObject* NamelistGroupObject)
{
#ifdef DEBUG_PRINT
   printf("NamelistGroupObject: %s\n", ATwriteToString(term));
#endif

 OFP::VariableName VariableName;
 if (ATmatch(term, "NamelistGroupObject(<term>)", &VariableName.term)) {

      if (ofp_traverse_VariableName(VariableName.term, &VariableName)) {
         // MATCHED VariableName
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_NamelistGroupObjectList(ATerm term, OFP::NamelistGroupObjectList* NamelistGroupObjectList)
{
#ifdef DEBUG_PRINT
   printf("NamelistGroupObjectList: %s\n", ATwriteToString(term));
#endif

 OFP::NamelistGroupObject NamelistGroupObject;
 if (ATmatch(term, "NamelistGroupObjectList(<term>)", &NamelistGroupObject.term)) {

   ATermList NamelistGroupObject_tail = (ATermList) ATmake("<term>", NamelistGroupObject.term);
   while (! ATisEmpty(NamelistGroupObject_tail)) {
      NamelistGroupObject.term = ATgetFirst(NamelistGroupObject_tail);
      NamelistGroupObject_tail = ATgetNext (NamelistGroupObject_tail);
      if (ofp_traverse_NamelistGroupObject(NamelistGroupObject.term, &NamelistGroupObject)) {
         // MATCHED NamelistGroupObject
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R565 equivalence-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EquivalenceStmt(ATerm term, OFP::EquivalenceStmt* EquivalenceStmt)
{
#ifdef DEBUG_PRINT
   printf("EquivalenceStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EquivalenceSetList EquivalenceSetList;
 OFP::EOS EOS;
 if (ATmatch(term, "EquivalenceStmt(<term>,<term>,<term>)", &Label.term, &EquivalenceSetList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EquivalenceSetList(EquivalenceSetList.term, &EquivalenceSetList)) {
         // MATCHED EquivalenceSetList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R566 equivalence-set
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EquivalenceSet(ATerm term, OFP::EquivalenceSet* EquivalenceSet)
{
#ifdef DEBUG_PRINT
   printf("EquivalenceSet: %s\n", ATwriteToString(term));
#endif

 OFP::EquivalenceObject EquivalenceObject;
 OFP::EquivalenceObjectList EquivalenceObjectList;
 if (ATmatch(term, "EquivalenceSet(<term>,<term>)", &EquivalenceObject.term, &EquivalenceObjectList.term)) {

      if (ofp_traverse_EquivalenceObject(EquivalenceObject.term, &EquivalenceObject)) {
         // MATCHED EquivalenceObject
      } else return ATfalse;

      if (ofp_traverse_EquivalenceObjectList(EquivalenceObjectList.term, &EquivalenceObjectList)) {
         // MATCHED EquivalenceObjectList
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_EquivalenceSetList(ATerm term, OFP::EquivalenceSetList* EquivalenceSetList)
{
#ifdef DEBUG_PRINT
   printf("EquivalenceSetList: %s\n", ATwriteToString(term));
#endif

 OFP::EquivalenceSet EquivalenceSet;
 if (ATmatch(term, "EquivalenceSetList(<term>)", &EquivalenceSet.term)) {

   ATermList EquivalenceSet_tail = (ATermList) ATmake("<term>", EquivalenceSet.term);
   while (! ATisEmpty(EquivalenceSet_tail)) {
      EquivalenceSet.term = ATgetFirst(EquivalenceSet_tail);
      EquivalenceSet_tail = ATgetNext (EquivalenceSet_tail);
      if (ofp_traverse_EquivalenceSet(EquivalenceSet.term, &EquivalenceSet)) {
         // MATCHED EquivalenceSet
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R567 equivalence-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EquivalenceObject(ATerm term, OFP::EquivalenceObject* EquivalenceObject)
{
#ifdef DEBUG_PRINT
   printf("EquivalenceObject: %s\n", ATwriteToString(term));
#endif

 OFP::Variable Variable;
 if (ATmatch(term, "EquivalenceObject(<term>)", &Variable.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_EquivalenceObjectList(ATerm term, OFP::EquivalenceObjectList* EquivalenceObjectList)
{
#ifdef DEBUG_PRINT
   printf("EquivalenceObjectList: %s\n", ATwriteToString(term));
#endif

 OFP::EquivalenceObject EquivalenceObject;
 if (ATmatch(term, "EquivalenceObjectList(<term>)", &EquivalenceObject.term)) {

   ATermList EquivalenceObject_tail = (ATermList) ATmake("<term>", EquivalenceObject.term);
   while (! ATisEmpty(EquivalenceObject_tail)) {
      EquivalenceObject.term = ATgetFirst(EquivalenceObject_tail);
      EquivalenceObject_tail = ATgetNext (EquivalenceObject_tail);
      if (ofp_traverse_EquivalenceObject(EquivalenceObject.term, &EquivalenceObject)) {
         // MATCHED EquivalenceObject
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R568 common-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CommonStmt(ATerm term, OFP::CommonStmt* CommonStmt)
{
#ifdef DEBUG_PRINT
   printf("CommonStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::CommonBlockEntry1 CommonBlockEntry1;
 OFP::CommonBlockEntry CommonBlockEntry;
 OFP::EOS EOS;
 if (ATmatch(term, "CommonStmt(<term>,<term>,<term>,<term>)", &Label.term, &CommonBlockEntry1.term, &CommonBlockEntry.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_CommonBlockEntry1(CommonBlockEntry1.term, &CommonBlockEntry1)) {
         // MATCHED CommonBlockEntry1
      } else return ATfalse;

   ATermList CommonBlockEntry_tail = (ATermList) ATmake("<term>", CommonBlockEntry.term);
   while (! ATisEmpty(CommonBlockEntry_tail)) {
      CommonBlockEntry.term = ATgetFirst(CommonBlockEntry_tail);
      CommonBlockEntry_tail = ATgetNext (CommonBlockEntry_tail);
      if (ofp_traverse_CommonBlockEntry(CommonBlockEntry.term, &CommonBlockEntry)) {
         // MATCHED CommonBlockEntry
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CommonBlockEntry1(ATerm term, OFP::CommonBlockEntry1* CommonBlockEntry1)
{
#ifdef DEBUG_PRINT
   printf("CommonBlockEntry1: %s\n", ATwriteToString(term));
#endif

 OFP::CommonBlockObjectList CommonBlockObjectList;
 if (ATmatch(term, "CBE1(<term>)", &CommonBlockObjectList.term)) {

      if (ofp_traverse_CommonBlockObjectList(CommonBlockObjectList.term, &CommonBlockObjectList)) {
         // MATCHED CommonBlockObjectList
      } else return ATfalse;

   // MATCHED CBE1

   return ATtrue;
 }

 OFP::Name CommonBlockName;
 OFP::CommonBlockObjectList CommonBlockObjectList1;
 if (ATmatch(term, "CBE1_CBN(<term>,<term>)", &CommonBlockName.term, &CommonBlockObjectList1.term)) {

   if (ATmatch(CommonBlockName.term, "Some(<term>)", &CommonBlockName.term)) {
      if (ofp_traverse_Name(CommonBlockName.term, &CommonBlockName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_CommonBlockObjectList(CommonBlockObjectList1.term, &CommonBlockObjectList1)) {
         // MATCHED CommonBlockObjectList
      } else return ATfalse;

   // MATCHED CBE1_CBN

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CommonBlockEntry(ATerm term, OFP::CommonBlockEntry* CommonBlockEntry)
{
#ifdef DEBUG_PRINT
   printf("CommonBlockEntry: %s\n", ATwriteToString(term));
#endif

 OFP::Name CommonBlockName;
 OFP::CommonBlockObjectList CommonBlockObjectList;
 if (ATmatch(term, "CBE(<term>,<term>)", &CommonBlockName.term, &CommonBlockObjectList.term)) {

   if (ATmatch(CommonBlockName.term, "Some(<term>)", &CommonBlockName.term)) {
      if (ofp_traverse_Name(CommonBlockName.term, &CommonBlockName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_CommonBlockObjectList(CommonBlockObjectList.term, &CommonBlockObjectList)) {
         // MATCHED CommonBlockObjectList
      } else return ATfalse;

   // MATCHED CBE

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R569 common-block-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CommonBlockObject(ATerm term, OFP::CommonBlockObject* CommonBlockObject)
{
#ifdef DEBUG_PRINT
   printf("CommonBlockObject: %s\n", ATwriteToString(term));
#endif

 OFP::VariableName VariableName;
 OFP::ArraySpec ArraySpec;
 if (ATmatch(term, "CommonBlockObject(<term>,<term>)", &VariableName.term, &ArraySpec.term)) {

      if (ofp_traverse_VariableName(VariableName.term, &VariableName)) {
         // MATCHED VariableName
      } else return ATfalse;

   if (ATmatch(ArraySpec.term, "Some(<term>)", &ArraySpec.term)) {
   if (ATmatch(ArraySpec.term, "(<term>)", &ArraySpec.term)) {
      if (ofp_traverse_ArraySpec(ArraySpec.term, &ArraySpec)) {
         // MATCHED ArraySpec
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CommonBlockObjectList(ATerm term, OFP::CommonBlockObjectList* CommonBlockObjectList)
{
#ifdef DEBUG_PRINT
   printf("CommonBlockObjectList: %s\n", ATwriteToString(term));
#endif

 OFP::CommonBlockObject CommonBlockObject;
 if (ATmatch(term, "CommonBlockObjectList(<term>)", &CommonBlockObject.term)) {

   ATermList CommonBlockObject_tail = (ATermList) ATmake("<term>", CommonBlockObject.term);
   while (! ATisEmpty(CommonBlockObject_tail)) {
      CommonBlockObject.term = ATgetFirst(CommonBlockObject_tail);
      CommonBlockObject_tail = ATgetNext (CommonBlockObject_tail);
      if (ofp_traverse_CommonBlockObject(CommonBlockObject.term, &CommonBlockObject)) {
         // MATCHED CommonBlockObject
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R601 designator
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Designator(ATerm term, OFP::Designator* Designator)
{
#ifdef DEBUG_PRINT
   printf("Designator(W): %s\n", ATwriteToString(term));
#endif

   OFP::DataRef DataRef;
   if (ofp_traverse_DataRef(term, &DataRef)) {
      // MATCHED DataRef
      Designator->setOptionType(OFP::Designator::DataRef_ot);
      Designator->setDataRef(DataRef.newDataRef());
      Designator->inheritPayload(Designator->getDataRef());
      return ATtrue;
   }

 OFP::Substring Substring;
 if (ATmatch(term, "Designator_S_AMB(<term>)", &Substring.term)) {

      if (ofp_traverse_Substring(Substring.term, &Substring)) {
         // MATCHED Substring
         Designator->setSubstring(Substring.newSubstring());
         Designator->inheritPayload(Designator->getSubstring());
      } else return ATfalse;

   // MATCHED Designator_S_AMB
   Designator->setOptionType(OFP::Designator::Designator_S_AMB);

   return ATtrue;
 }

 if (ATmatch(term, "Designator_S_AMB(<term>)", &Substring.term)) {

      if (ofp_traverse_Substring(Substring.term, &Substring)) {
         // MATCHED Substring
         Designator->setSubstring(Substring.newSubstring());
         Designator->inheritPayload(Designator->getSubstring());
      } else return ATfalse;

   // MATCHED Designator_S_AMB
   Designator->setOptionType(OFP::Designator::Designator_S_AMB);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R602 variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Variable(ATerm term, OFP::Variable* Variable)
{
#ifdef DEBUG_PRINT
   printf("Variable(F): %s\n", ATwriteToString(term));
#endif

   OFP::Designator Designator;
   if (ofp_traverse_Designator(term, &Designator)) {
      // MATCHED Designator
      Variable->setDesignator(Designator.newDesignator());
      Variable->inheritPayload(Variable->getDesignator());
   } else return ATfalse;

 return ATtrue;
}

//========================================================================================
// R603 variable-name
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_VariableName(ATerm term, OFP::VariableName* VariableName)
{
#ifdef DEBUG_PRINT
   printf("VariableName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "VariableName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R604 logical-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LogicalVariable(ATerm term, OFP::LogicalVariable* LogicalVariable)
{
#ifdef DEBUG_PRINT
   printf("LogicalVariable: %s\n", ATwriteToString(term));
#endif

 OFP::Variable Variable;
 if (ATmatch(term, "LogicalVariable(<term>)", &Variable.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R605 char-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CharVariable(ATerm term, OFP::CharVariable* CharVariable)
{
#ifdef DEBUG_PRINT
   printf("CharVariable: %s\n", ATwriteToString(term));
#endif

 OFP::Variable Variable;
 if (ATmatch(term, "CharVariable(<term>)", &Variable.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R606 default-char-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DefaultCharVariable(ATerm term, OFP::DefaultCharVariable* DefaultCharVariable)
{
#ifdef DEBUG_PRINT
   printf("DefaultCharVariable: %s\n", ATwriteToString(term));
#endif

 OFP::Variable Variable;
 if (ATmatch(term, "DefaultCharVariable(<term>)", &Variable.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R607 int-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntVariable(ATerm term, OFP::IntVariable* IntVariable)
{
#ifdef DEBUG_PRINT
   printf("IntVariable: %s\n", ATwriteToString(term));
#endif

 OFP::Variable Variable;
 if (ATmatch(term, "IntVariable(<term>)", &Variable.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R608 substring
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Substring(ATerm term, OFP::Substring* Substring)
{
#ifdef DEBUG_PRINT
   printf("Substring: %s\n", ATwriteToString(term));
#endif

 OFP::ParentString ParentString;
 OFP::SubstringRange SubstringRange;
 if (ATmatch(term, "Substring(<term>,<term>)", &ParentString.term, &SubstringRange.term)) {

      if (ofp_traverse_ParentString(ParentString.term, &ParentString)) {
         // MATCHED ParentString
      } else return ATfalse;

      if (ofp_traverse_SubstringRange(SubstringRange.term, &SubstringRange)) {
         // MATCHED SubstringRange
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R609 parent-string
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ParentString(ATerm term, OFP::ParentString* ParentString)
{
#ifdef DEBUG_PRINT
   printf("ParentString: %s\n", ATwriteToString(term));
#endif

 OFP::Constant Constant;
 if (ATmatch(term, "ParentString_C(<term>)", &Constant.term)) {

      if (ofp_traverse_Constant(Constant.term, &Constant)) {
         // MATCHED Constant
      } else return ATfalse;

   // MATCHED ParentString_C

   return ATtrue;
 }

 OFP::StructureComponent StructureComponent;
 if (ATmatch(term, "ParentString_SC(<term>)", &StructureComponent.term)) {

      if (ofp_traverse_StructureComponent(StructureComponent.term, &StructureComponent)) {
         // MATCHED StructureComponent
      } else return ATfalse;

   // MATCHED ParentString_SC

   return ATtrue;
 }

 OFP::CoindexedNamedObject CoindexedNamedObject;
 if (ATmatch(term, "ParentString_CNO(<term>)", &CoindexedNamedObject.term)) {

      if (ofp_traverse_CoindexedNamedObject(CoindexedNamedObject.term, &CoindexedNamedObject)) {
         // MATCHED CoindexedNamedObject
      } else return ATfalse;

   // MATCHED ParentString_CNO

   return ATtrue;
 }

 OFP::ArrayElement ArrayElement;
 if (ATmatch(term, "ParentString_AE(<term>)", &ArrayElement.term)) {

      if (ofp_traverse_ArrayElement(ArrayElement.term, &ArrayElement)) {
         // MATCHED ArrayElement
      } else return ATfalse;

   // MATCHED ParentString_AE

   return ATtrue;
 }

 OFP::Name ScalarVariableName;
 if (ATmatch(term, "ParentString_SVN(<term>)", &ScalarVariableName.term)) {

      if (ofp_traverse_Name(ScalarVariableName.term, &ScalarVariableName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED ParentString_SVN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R610 substring-range
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SubstringRange(ATerm term, OFP::SubstringRange* SubstringRange)
{
#ifdef DEBUG_PRINT
   printf("SubstringRange: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 OFP::IntExpr IntExpr1;
 if (ATmatch(term, "SubstringRange(<term>,<term>)", &IntExpr.term, &IntExpr1.term)) {

   if (ATmatch(IntExpr.term, "Some(<term>)", &IntExpr.term)) {
      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;
   }

   if (ATmatch(IntExpr1.term, "Some(<term>)", &IntExpr1.term)) {
      if (ofp_traverse_IntExpr(IntExpr1.term, &IntExpr1)) {
         // MATCHED IntExpr
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R611 data-ref
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataRef(ATerm term, OFP::DataRef* DataRef)
{
#ifdef DEBUG_PRINT
   printf("DataRef(W): %s\n", ATwriteToString(term));
#endif

   OFP::PartRef PartRef;

   // AST transformations allow a rewrite of a DataRef directly to a PartRef
   //    - probably most likely path
   //
   if (ofp_traverse_PartRef(term, &PartRef)) {
      DataRef->appendPartRef(PartRef.newPartRef());
   }

   // TODO - this probably needs to match a list
   else if (ATmatch(term, "DataRef(<term>)", &PartRef.term)) {

      ATermList PartRef_tail = (ATermList) ATmake("<term>", PartRef.term);
      while (! ATisEmpty(PartRef_tail)) {
         PartRef.term = ATgetFirst(PartRef_tail);
         PartRef_tail = ATgetNext (PartRef_tail);
         if (ofp_traverse_PartRef(PartRef.term, &PartRef)) {
            // MATCHED PartRef
            DataRef->appendPartRef(PartRef.newPartRef());
         } else return ATfalse;
      }

   }

   else {
      return ATfalse;
   }

   ast->build_DataRef(DataRef);

   return ATtrue;
}
 
//========================================================================================
// R612 part-ref
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PartRef(ATerm term, OFP::PartRef* PartRef)
{
#ifdef DEBUG_PRINT
   printf("PartRef(W): %s\n", ATwriteToString(term));
#endif

   OFP::Name PartName;
   OFP::SectionSubscriptList SectionSubscriptList;
   OFP::ImageSelector ImageSelector;

   // AST rewriting allows PartRef to be just a PartName so try matching Name
   //
   if (ofp_traverse_Name(term, &PartName)) {
      // MATCHED PartName
      PartRef->setPartName(PartName.newName());
      ast->build_PartRef(PartRef);
      return ATtrue;
   }

 if (ATmatch(term, "PartRef(<term>,<term>,<term>)", &PartName.term, &SectionSubscriptList.term, &ImageSelector.term)) {

      if (ofp_traverse_Name(PartName.term, &PartName)) {
         // MATCHED PartName
         PartRef->setPartName(PartName.newName());
      } else return ATfalse;

   if (ATmatch(SectionSubscriptList.term, "Some(<term>)", &SectionSubscriptList.term)) {
   if (ATmatch(SectionSubscriptList.term, "(<term>)", &SectionSubscriptList.term)) {
      if (ofp_traverse_SectionSubscriptList(SectionSubscriptList.term, &SectionSubscriptList)) {
         // MATCHED SectionSubscriptList
         PartRef->setSectionSubscriptList(SectionSubscriptList.newSectionSubscriptList());
         //TODO PartRef->inheritPayload(PartRef->getSectionSubscriptList());
      } else return ATfalse;
   }
   }

   if (ATmatch(ImageSelector.term, "Some(<term>)", &ImageSelector.term)) {
      if (ofp_traverse_ImageSelector(ImageSelector.term, &ImageSelector)) {
         // MATCHED ImageSelector
         PartRef->setImageSelector(ImageSelector.newImageSelector());
         //TODO PartRef->inheritPayload(PartRef->getImageSelector());
      } else return ATfalse;
   }

   ast->build_PartRef(PartRef);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R613 structure-component
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_StructureComponent(ATerm term, OFP::StructureComponent* StructureComponent)
{
#ifdef DEBUG_PRINT
   printf("StructureComponent: %s\n", ATwriteToString(term));
#endif

 OFP::DataRef DataRef;
 if (ATmatch(term, "StructureComponent(<term>)", &DataRef.term)) {

      if (ofp_traverse_DataRef(DataRef.term, &DataRef)) {
         // MATCHED DataRef
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R614 coindexed-named-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CoindexedNamedObject(ATerm term, OFP::CoindexedNamedObject* CoindexedNamedObject)
{
#ifdef DEBUG_PRINT
   printf("CoindexedNamedObject: %s\n", ATwriteToString(term));
#endif

 OFP::DataRef DataRef;
 if (ATmatch(term, "CoindexedNamedObject(<term>)", &DataRef.term)) {

      if (ofp_traverse_DataRef(DataRef.term, &DataRef)) {
         // MATCHED DataRef
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R616 type-param-inquiry
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeParamInquiry(ATerm term, OFP::TypeParamInquiry* TypeParamInquiry)
{
#ifdef DEBUG_PRINT
   printf("TypeParamInquiry: %s\n", ATwriteToString(term));
#endif

 OFP::Designator Designator;
 OFP::Name TypeParamName;
 if (ATmatch(term, "TypeParamInquiry(<term>,<term>)", &Designator.term, &TypeParamName.term)) {

      if (ofp_traverse_Designator(Designator.term, &Designator)) {
         // MATCHED Designator
      } else return ATfalse;

      if (ofp_traverse_Name(TypeParamName.term, &TypeParamName)) {
         // MATCHED Name
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R617 array-element
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ArrayElement(ATerm term, OFP::ArrayElement* ArrayElement)
{
#ifdef DEBUG_PRINT
   printf("ArrayElement: %s\n", ATwriteToString(term));
#endif

 OFP::DataRef DataRef;
 if (ATmatch(term, "ArrayElement(<term>)", &DataRef.term)) {

      if (ofp_traverse_DataRef(DataRef.term, &DataRef)) {
         // MATCHED DataRef
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R619 subscript
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Subscript(ATerm term, OFP::Subscript* Subscript)
{
#ifdef DEBUG_PRINT
   printf("Subscript: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "Subscript(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R620 section-subscript
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SectionSubscript(ATerm term, OFP::SectionSubscript* SectionSubscript)
{
#ifdef DEBUG_PRINT
   printf("SectionSubscript: %s\n", ATwriteToString(term));
#endif

 OFP::SubscriptTriplet SubscriptTriplet;
 if (ATmatch(term, "SectionSubscript_ST(<term>)", &SubscriptTriplet.term)) {

      if (ofp_traverse_SubscriptTriplet(SubscriptTriplet.term, &SubscriptTriplet)) {
         // MATCHED SubscriptTriplet
      } else return ATfalse;

   // MATCHED SectionSubscript_ST

   return ATtrue;
 }

 OFP::Subscript Subscript;
 if (ATmatch(term, "SectionSubscript_S(<term>)", &Subscript.term)) {

      if (ofp_traverse_Subscript(Subscript.term, &Subscript)) {
         // MATCHED Subscript
      } else return ATfalse;

   // MATCHED SectionSubscript_S

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_SectionSubscriptList(ATerm term, OFP::SectionSubscriptList* SectionSubscriptList)
{
#ifdef DEBUG_PRINT
   printf("SectionSubscriptList: %s\n", ATwriteToString(term));
#endif

 OFP::SectionSubscript SectionSubscript;
 if (ATmatch(term, "SectionSubscriptList(<term>)", &SectionSubscript.term)) {

   ATermList SectionSubscript_tail = (ATermList) ATmake("<term>", SectionSubscript.term);
   while (! ATisEmpty(SectionSubscript_tail)) {
      SectionSubscript.term = ATgetFirst(SectionSubscript_tail);
      SectionSubscript_tail = ATgetNext (SectionSubscript_tail);
      if (ofp_traverse_SectionSubscript(SectionSubscript.term, &SectionSubscript)) {
         // MATCHED SectionSubscript
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R621 subscript-triplet
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SubscriptTriplet(ATerm term, OFP::SubscriptTriplet* SubscriptTriplet)
{
#ifdef DEBUG_PRINT
   printf("SubscriptTriplet: %s\n", ATwriteToString(term));
#endif

 OFP::Subscript Subscript;
 OFP::Subscript Subscript1;
 OFP::Stride Stride;
 if (ATmatch(term, "SubscriptTriplet(<term>,<term>,<term>)", &Subscript.term, &Subscript1.term, &Stride.term)) {

   if (ATmatch(Subscript.term, "Some(<term>)", &Subscript.term)) {
      if (ofp_traverse_Subscript(Subscript.term, &Subscript)) {
         // MATCHED Subscript
      } else return ATfalse;
   }

   if (ATmatch(Subscript1.term, "Some(<term>)", &Subscript1.term)) {
      if (ofp_traverse_Subscript(Subscript1.term, &Subscript1)) {
         // MATCHED Subscript
      } else return ATfalse;
   }

   if (ATmatch(Stride.term, "Some(<term>)", &Stride.term)) {
   if (ATmatch(Stride.term, "(<term>)", &Stride.term)) {
      if (ofp_traverse_Stride(Stride.term, &Stride)) {
         // MATCHED Stride
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R622 stride
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Stride(ATerm term, OFP::Stride* Stride)
{
#ifdef DEBUG_PRINT
   printf("Stride: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "Stride(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R624 image-selector
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ImageSelector(ATerm term, OFP::ImageSelector* ImageSelector)
{
#ifdef DEBUG_PRINT
   printf("ImageSelector: %s\n", ATwriteToString(term));
#endif

 OFP::CosubscriptList CosubscriptList;
 if (ATmatch(term, "ImageSelector(<term>)", &CosubscriptList.term)) {

      if (ofp_traverse_CosubscriptList(CosubscriptList.term, &CosubscriptList)) {
         // MATCHED CosubscriptList
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R625 cosubscript
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Cosubscript(ATerm term, OFP::Cosubscript* Cosubscript)
{
#ifdef DEBUG_PRINT
   printf("Cosubscript: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "Cosubscript(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CosubscriptList(ATerm term, OFP::CosubscriptList* CosubscriptList)
{
#ifdef DEBUG_PRINT
   printf("CosubscriptList: %s\n", ATwriteToString(term));
#endif

 OFP::Cosubscript Cosubscript;
 if (ATmatch(term, "CosubscriptList(<term>)", &Cosubscript.term)) {

   ATermList Cosubscript_tail = (ATermList) ATmake("<term>", Cosubscript.term);
   while (! ATisEmpty(Cosubscript_tail)) {
      Cosubscript.term = ATgetFirst(Cosubscript_tail);
      Cosubscript_tail = ATgetNext (Cosubscript_tail);
      if (ofp_traverse_Cosubscript(Cosubscript.term, &Cosubscript)) {
         // MATCHED Cosubscript
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R626 allocate-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AllocateStmt(ATerm term, OFP::AllocateStmt* AllocateStmt)
{
#ifdef DEBUG_PRINT
   printf("AllocateStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::TypeSpec TypeSpec;
 OFP::AllocationList AllocationList;
 OFP::AllocOptList AllocOptList;
 OFP::EOS EOS;
 if (ATmatch(term, "AllocateStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &TypeSpec.term, &AllocationList.term, &AllocOptList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(TypeSpec.term, "Some(<term>)", &TypeSpec.term)) {
   if (ATmatch(TypeSpec.term, "(<term>)", &TypeSpec.term)) {
      if (ofp_traverse_TypeSpec(TypeSpec.term, &TypeSpec)) {
         // MATCHED TypeSpec
      } else return ATfalse;
   }
   }

      if (ofp_traverse_AllocationList(AllocationList.term, &AllocationList)) {
         // MATCHED AllocationList
      } else return ATfalse;

   if (ATmatch(AllocOptList.term, "Some(<term>)", &AllocOptList.term)) {
   if (ATmatch(AllocOptList.term, "(<term>)", &AllocOptList.term)) {
      if (ofp_traverse_AllocOptList(AllocOptList.term, &AllocOptList)) {
         // MATCHED AllocOptList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R627 alloc-opt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AllocOpt(ATerm term, OFP::AllocOpt* AllocOpt)
{
#ifdef DEBUG_PRINT
   printf("AllocOpt: %s\n", ATwriteToString(term));
#endif

 OFP::StatVariable StatVariable;
 if (ATmatch(term, "AllocOpt_STAT(<term>)", &StatVariable.term)) {

      if (ofp_traverse_StatVariable(StatVariable.term, &StatVariable)) {
         // MATCHED StatVariable
      } else return ATfalse;

   // MATCHED AllocOpt_STAT

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "AllocOpt_SRC(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED AllocOpt_SRC

   return ATtrue;
 }

 OFP::Expr Expr1;
 if (ATmatch(term, "AllocOpt_MOLD(<term>)", &Expr1.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED AllocOpt_MOLD

   return ATtrue;
 }

 OFP::ErrmsgVariable ErrmsgVariable;
 if (ATmatch(term, "AllocOpt_ERR(<term>)", &ErrmsgVariable.term)) {

      if (ofp_traverse_ErrmsgVariable(ErrmsgVariable.term, &ErrmsgVariable)) {
         // MATCHED ErrmsgVariable
      } else return ATfalse;

   // MATCHED AllocOpt_ERR

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AllocOptList(ATerm term, OFP::AllocOptList* AllocOptList)
{
#ifdef DEBUG_PRINT
   printf("AllocOptList: %s\n", ATwriteToString(term));
#endif

 OFP::AllocOpt AllocOpt;
 if (ATmatch(term, "AllocOptList(<term>)", &AllocOpt.term)) {

   ATermList AllocOpt_tail = (ATermList) ATmake("<term>", AllocOpt.term);
   while (! ATisEmpty(AllocOpt_tail)) {
      AllocOpt.term = ATgetFirst(AllocOpt_tail);
      AllocOpt_tail = ATgetNext (AllocOpt_tail);
      if (ofp_traverse_AllocOpt(AllocOpt.term, &AllocOpt)) {
         // MATCHED AllocOpt
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R628 stat-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_StatVariable(ATerm term, OFP::StatVariable* StatVariable)
{
#ifdef DEBUG_PRINT
   printf("StatVariable: %s\n", ATwriteToString(term));
#endif

 OFP::IntVariable IntVariable;
 if (ATmatch(term, "StatVariable(<term>)", &IntVariable.term)) {

      if (ofp_traverse_IntVariable(IntVariable.term, &IntVariable)) {
         // MATCHED IntVariable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R629 errmsg-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ErrmsgVariable(ATerm term, OFP::ErrmsgVariable* ErrmsgVariable)
{
#ifdef DEBUG_PRINT
   printf("ErrmsgVariable: %s\n", ATwriteToString(term));
#endif

 OFP::DefaultCharVariable DefaultCharVariable;
 if (ATmatch(term, "ErrmsgVariable(<term>)", &DefaultCharVariable.term)) {

      if (ofp_traverse_DefaultCharVariable(DefaultCharVariable.term, &DefaultCharVariable)) {
         // MATCHED DefaultCharVariable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R631 allocation
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Allocation(ATerm term, OFP::Allocation* Allocation)
{
#ifdef DEBUG_PRINT
   printf("Allocation: %s\n", ATwriteToString(term));
#endif

 OFP::AllocateObject AllocateObject;
 OFP::AllocateShapeSpecList AllocateShapeSpecList;
 OFP::AllocateCoarraySpec AllocateCoarraySpec;
 if (ATmatch(term, "Allocation(<term>,<term>,<term>)", &AllocateObject.term, &AllocateShapeSpecList.term, &AllocateCoarraySpec.term)) {

      if (ofp_traverse_AllocateObject(AllocateObject.term, &AllocateObject)) {
         // MATCHED AllocateObject
      } else return ATfalse;

   if (ATmatch(AllocateShapeSpecList.term, "Some(<term>)", &AllocateShapeSpecList.term)) {
   if (ATmatch(AllocateShapeSpecList.term, "(<term>)", &AllocateShapeSpecList.term)) {
      if (ofp_traverse_AllocateShapeSpecList(AllocateShapeSpecList.term, &AllocateShapeSpecList)) {
         // MATCHED AllocateShapeSpecList
      } else return ATfalse;
   }
   }

   if (ATmatch(AllocateCoarraySpec.term, "Some(<term>)", &AllocateCoarraySpec.term)) {
   if (ATmatch(AllocateCoarraySpec.term, "(<term>)", &AllocateCoarraySpec.term)) {
      if (ofp_traverse_AllocateCoarraySpec(AllocateCoarraySpec.term, &AllocateCoarraySpec)) {
         // MATCHED AllocateCoarraySpec
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AllocationList(ATerm term, OFP::AllocationList* AllocationList)
{
#ifdef DEBUG_PRINT
   printf("AllocationList: %s\n", ATwriteToString(term));
#endif

 OFP::Allocation Allocation;
 if (ATmatch(term, "AllocationList(<term>)", &Allocation.term)) {

   ATermList Allocation_tail = (ATermList) ATmake("<term>", Allocation.term);
   while (! ATisEmpty(Allocation_tail)) {
      Allocation.term = ATgetFirst(Allocation_tail);
      Allocation_tail = ATgetNext (Allocation_tail);
      if (ofp_traverse_Allocation(Allocation.term, &Allocation)) {
         // MATCHED Allocation
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R632 allocate-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AllocateObject(ATerm term, OFP::AllocateObject* AllocateObject)
{
#ifdef DEBUG_PRINT
   printf("AllocateObject: %s\n", ATwriteToString(term));
#endif

 OFP::StructureComponent StructureComponent;
 if (ATmatch(term, "AllocateObject_AMB(<term>)", &StructureComponent.term)) {

      if (ofp_traverse_StructureComponent(StructureComponent.term, &StructureComponent)) {
         // MATCHED StructureComponent
      } else return ATfalse;

   // MATCHED AllocateObject_AMB

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AllocateObjectList(ATerm term, OFP::AllocateObjectList* AllocateObjectList)
{
#ifdef DEBUG_PRINT
   printf("AllocateObjectList: %s\n", ATwriteToString(term));
#endif

 OFP::AllocateObject AllocateObject;
 if (ATmatch(term, "AllocateObjectList(<term>)", &AllocateObject.term)) {

   ATermList AllocateObject_tail = (ATermList) ATmake("<term>", AllocateObject.term);
   while (! ATisEmpty(AllocateObject_tail)) {
      AllocateObject.term = ATgetFirst(AllocateObject_tail);
      AllocateObject_tail = ATgetNext (AllocateObject_tail);
      if (ofp_traverse_AllocateObject(AllocateObject.term, &AllocateObject)) {
         // MATCHED AllocateObject
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R633 allocate-shape-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AllocateShapeSpec(ATerm term, OFP::AllocateShapeSpec* AllocateShapeSpec)
{
#ifdef DEBUG_PRINT
   printf("AllocateShapeSpec: %s\n", ATwriteToString(term));
#endif

 OFP::LowerBoundExpr LowerBoundExpr;
 OFP::UpperBoundExpr UpperBoundExpr;
 if (ATmatch(term, "AllocateShapeSpec(<term>,<term>)", &LowerBoundExpr.term, &UpperBoundExpr.term)) {

   if (ATmatch(LowerBoundExpr.term, "Some(<term>)", &LowerBoundExpr.term)) {
   if (ATmatch(LowerBoundExpr.term, "(<term>)", &LowerBoundExpr.term)) {
      if (ofp_traverse_LowerBoundExpr(LowerBoundExpr.term, &LowerBoundExpr)) {
         // MATCHED LowerBoundExpr
      } else return ATfalse;
   }
   }

      if (ofp_traverse_UpperBoundExpr(UpperBoundExpr.term, &UpperBoundExpr)) {
         // MATCHED UpperBoundExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AllocateShapeSpecList(ATerm term, OFP::AllocateShapeSpecList* AllocateShapeSpecList)
{
#ifdef DEBUG_PRINT
   printf("AllocateShapeSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::AllocateShapeSpec AllocateShapeSpec;
 if (ATmatch(term, "AllocateShapeSpecList(<term>)", &AllocateShapeSpec.term)) {

   ATermList AllocateShapeSpec_tail = (ATermList) ATmake("<term>", AllocateShapeSpec.term);
   while (! ATisEmpty(AllocateShapeSpec_tail)) {
      AllocateShapeSpec.term = ATgetFirst(AllocateShapeSpec_tail);
      AllocateShapeSpec_tail = ATgetNext (AllocateShapeSpec_tail);
      if (ofp_traverse_AllocateShapeSpec(AllocateShapeSpec.term, &AllocateShapeSpec)) {
         // MATCHED AllocateShapeSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R634 lower-bound-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LowerBoundExpr(ATerm term, OFP::LowerBoundExpr* LowerBoundExpr)
{
#ifdef DEBUG_PRINT
   printf("LowerBoundExpr: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "LowerBoundExpr(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R635 upper-bound-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_UpperBoundExpr(ATerm term, OFP::UpperBoundExpr* UpperBoundExpr)
{
#ifdef DEBUG_PRINT
   printf("UpperBoundExpr: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "UpperBoundExpr(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R636 allocate-coarray-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AllocateCoarraySpec(ATerm term, OFP::AllocateCoarraySpec* AllocateCoarraySpec)
{
#ifdef DEBUG_PRINT
   printf("AllocateCoarraySpec: %s\n", ATwriteToString(term));
#endif

 OFP::AllocateCoshapeSpecList AllocateCoshapeSpecList;
 OFP::LowerBoundExpr LowerBoundExpr;
 if (ATmatch(term, "AllocateCoarraySpec(<term>,<term>)", &AllocateCoshapeSpecList.term, &LowerBoundExpr.term)) {

   if (ATmatch(AllocateCoshapeSpecList.term, "Some(<term>)", &AllocateCoshapeSpecList.term)) {
   if (ATmatch(AllocateCoshapeSpecList.term, "(<term>)", &AllocateCoshapeSpecList.term)) {
      if (ofp_traverse_AllocateCoshapeSpecList(AllocateCoshapeSpecList.term, &AllocateCoshapeSpecList)) {
         // MATCHED AllocateCoshapeSpecList
      } else return ATfalse;
   }
   }

   if (ATmatch(LowerBoundExpr.term, "Some(<term>)", &LowerBoundExpr.term)) {
   if (ATmatch(LowerBoundExpr.term, "(<term>)", &LowerBoundExpr.term)) {
      if (ofp_traverse_LowerBoundExpr(LowerBoundExpr.term, &LowerBoundExpr)) {
         // MATCHED LowerBoundExpr
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R637 allocate-coshape-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AllocateCoshapeSpec(ATerm term, OFP::AllocateCoshapeSpec* AllocateCoshapeSpec)
{
#ifdef DEBUG_PRINT
   printf("AllocateCoshapeSpec: %s\n", ATwriteToString(term));
#endif

 OFP::LowerBoundExpr LowerBoundExpr;
 OFP::UpperBoundExpr UpperBoundExpr;
 if (ATmatch(term, "AllocateCoshapeSpec(<term>,<term>)", &LowerBoundExpr.term, &UpperBoundExpr.term)) {

   if (ATmatch(LowerBoundExpr.term, "Some(<term>)", &LowerBoundExpr.term)) {
   if (ATmatch(LowerBoundExpr.term, "(<term>)", &LowerBoundExpr.term)) {
      if (ofp_traverse_LowerBoundExpr(LowerBoundExpr.term, &LowerBoundExpr)) {
         // MATCHED LowerBoundExpr
      } else return ATfalse;
   }
   }

      if (ofp_traverse_UpperBoundExpr(UpperBoundExpr.term, &UpperBoundExpr)) {
         // MATCHED UpperBoundExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AllocateCoshapeSpecList(ATerm term, OFP::AllocateCoshapeSpecList* AllocateCoshapeSpecList)
{
#ifdef DEBUG_PRINT
   printf("AllocateCoshapeSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::AllocateCoshapeSpec AllocateCoshapeSpec;
 if (ATmatch(term, "AllocateCoshapeSpecList(<term>)", &AllocateCoshapeSpec.term)) {

   ATermList AllocateCoshapeSpec_tail = (ATermList) ATmake("<term>", AllocateCoshapeSpec.term);
   while (! ATisEmpty(AllocateCoshapeSpec_tail)) {
      AllocateCoshapeSpec.term = ATgetFirst(AllocateCoshapeSpec_tail);
      AllocateCoshapeSpec_tail = ATgetNext (AllocateCoshapeSpec_tail);
      if (ofp_traverse_AllocateCoshapeSpec(AllocateCoshapeSpec.term, &AllocateCoshapeSpec)) {
         // MATCHED AllocateCoshapeSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R638 nullify-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_NullifyStmt(ATerm term, OFP::NullifyStmt* NullifyStmt)
{
#ifdef DEBUG_PRINT
   printf("NullifyStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::PointerObjectList PointerObjectList;
 OFP::EOS EOS;
 if (ATmatch(term, "NullifyStmt(<term>,<term>,<term>)", &Label.term, &PointerObjectList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_PointerObjectList(PointerObjectList.term, &PointerObjectList)) {
         // MATCHED PointerObjectList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R639 pointer-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PointerObject(ATerm term, OFP::PointerObject* PointerObject)
{
#ifdef DEBUG_PRINT
   printf("PointerObject: %s\n", ATwriteToString(term));
#endif

 OFP::ProcPointerName ProcPointerName;
 if (ATmatch(term, "PointerObject_PPN(<term>)", &ProcPointerName.term)) {

      if (ofp_traverse_ProcPointerName(ProcPointerName.term, &ProcPointerName)) {
         // MATCHED ProcPointerName
      } else return ATfalse;

   // MATCHED PointerObject_PPN

   return ATtrue;
 }

 OFP::StructureComponent StructureComponent;
 if (ATmatch(term, "PointerObject_SC(<term>)", &StructureComponent.term)) {

      if (ofp_traverse_StructureComponent(StructureComponent.term, &StructureComponent)) {
         // MATCHED StructureComponent
      } else return ATfalse;

   // MATCHED PointerObject_SC

   return ATtrue;
 }

 OFP::VariableName VariableName;
 if (ATmatch(term, "PointerObject_VN(<term>)", &VariableName.term)) {

      if (ofp_traverse_VariableName(VariableName.term, &VariableName)) {
         // MATCHED VariableName
      } else return ATfalse;

   // MATCHED PointerObject_VN

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_PointerObjectList(ATerm term, OFP::PointerObjectList* PointerObjectList)
{
#ifdef DEBUG_PRINT
   printf("PointerObjectList: %s\n", ATwriteToString(term));
#endif

 OFP::PointerObject PointerObject;
 if (ATmatch(term, "PointerObjectList(<term>)", &PointerObject.term)) {

   ATermList PointerObject_tail = (ATermList) ATmake("<term>", PointerObject.term);
   while (! ATisEmpty(PointerObject_tail)) {
      PointerObject.term = ATgetFirst(PointerObject_tail);
      PointerObject_tail = ATgetNext (PointerObject_tail);
      if (ofp_traverse_PointerObject(PointerObject.term, &PointerObject)) {
         // MATCHED PointerObject
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R640 deallocate-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DeallocateStmt(ATerm term, OFP::DeallocateStmt* DeallocateStmt)
{
#ifdef DEBUG_PRINT
   printf("DeallocateStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::AllocateObjectList AllocateObjectList;
 OFP::DeallocOptList DeallocOptList;
 OFP::EOS EOS;
 if (ATmatch(term, "DeallocateStmt(<term>,<term>,<term>,<term>)", &Label.term, &AllocateObjectList.term, &DeallocOptList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_AllocateObjectList(AllocateObjectList.term, &AllocateObjectList)) {
         // MATCHED AllocateObjectList
      } else return ATfalse;

   if (ATmatch(DeallocOptList.term, "Some(<term>)", &DeallocOptList.term)) {
   if (ATmatch(DeallocOptList.term, "(<term>)", &DeallocOptList.term)) {
      if (ofp_traverse_DeallocOptList(DeallocOptList.term, &DeallocOptList)) {
         // MATCHED DeallocOptList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R641 deallocate-opt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DeallocOpt(ATerm term, OFP::DeallocOpt* DeallocOpt)
{
#ifdef DEBUG_PRINT
   printf("DeallocOpt: %s\n", ATwriteToString(term));
#endif

 OFP::ErrmsgVariable ErrmsgVariable;
 if (ATmatch(term, "DeallocOpt_ERR(<term>)", &ErrmsgVariable.term)) {

      if (ofp_traverse_ErrmsgVariable(ErrmsgVariable.term, &ErrmsgVariable)) {
         // MATCHED ErrmsgVariable
      } else return ATfalse;

   // MATCHED DeallocOpt_ERR

   return ATtrue;
 }

 OFP::StatVariable StatVariable;
 if (ATmatch(term, "DeallocOpt_STAT(<term>)", &StatVariable.term)) {

      if (ofp_traverse_StatVariable(StatVariable.term, &StatVariable)) {
         // MATCHED StatVariable
      } else return ATfalse;

   // MATCHED DeallocOpt_STAT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DeallocOptList(ATerm term, OFP::DeallocOptList* DeallocOptList)
{
#ifdef DEBUG_PRINT
   printf("DeallocOptList: %s\n", ATwriteToString(term));
#endif

 OFP::DeallocOpt DeallocOpt;
 if (ATmatch(term, "DeallocOptList(<term>)", &DeallocOpt.term)) {

   ATermList DeallocOpt_tail = (ATermList) ATmake("<term>", DeallocOpt.term);
   while (! ATisEmpty(DeallocOpt_tail)) {
      DeallocOpt.term = ATgetFirst(DeallocOpt_tail);
      DeallocOpt_tail = ATgetNext (DeallocOpt_tail);
      if (ofp_traverse_DeallocOpt(DeallocOpt.term, &DeallocOpt)) {
         // MATCHED DeallocOpt
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

#ifdef MOVED_TO_OFP_EXPR
//========================================================================================
// R309 intrinsic-operator
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntrinsicOperator(ATerm term, OFP::IntrinsicOperator* IntrinsicOperator)
{
#ifdef DEBUG_PRINT
   printf("IntrinsicOperator: %s\n", ATwriteToString(term));
#endif

#ifdef NOT_YET
 OFP::EquivOp EquivOp;
 if (ATmatch(term, "IntrinsicOperator_EO(<term>)", &EquivOp.term)) {

      if (ofp_traverse_EquivOp(EquivOp.term, &EquivOp)) {
         // MATCHED EquivOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_EO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::OrOp OrOp;
 if (ATmatch(term, "IntrinsicOperator_OO(<term>)", &OrOp.term)) {

      if (ofp_traverse_OrOp(OrOp.term, &OrOp)) {
         // MATCHED OrOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_OO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::AndOp AndOp;
 if (ATmatch(term, "IntrinsicOperator_AO2(<term>)", &AndOp.term)) {

      if (ofp_traverse_AndOp(AndOp.term, &AndOp)) {
         // MATCHED AndOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_AO2

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::NotOp NotOp;
 if (ATmatch(term, "IntrinsicOperator_NO(<term>)", &NotOp.term)) {

      if (ofp_traverse_NotOp(NotOp.term, &NotOp)) {
         // MATCHED NotOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_NO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::RelOp RelOp;
 if (ATmatch(term, "IntrinsicOperator_RO(<term>)", &RelOp.term)) {

      if (ofp_traverse_RelOp(RelOp.term, &RelOp)) {
         // MATCHED RelOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_RO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::ConcatOp ConcatOp;
 if (ATmatch(term, "IntrinsicOperator_CO(<term>)", &ConcatOp.term)) {

      if (ofp_traverse_ConcatOp(ConcatOp.term, &ConcatOp)) {
         // MATCHED ConcatOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_CO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::AddOp AddOp;
 if (ATmatch(term, "IntrinsicOperator_AO1(<term>)", &AddOp.term)) {

      if (ofp_traverse_AddOp(AddOp.term, &AddOp)) {
         // MATCHED AddOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_AO1

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::MultOp MultOp;
 if (ATmatch(term, "IntrinsicOperator_MO(<term>)", &MultOp.term)) {

      if (ofp_traverse_MultOp(MultOp.term, &MultOp)) {
         // MATCHED MultOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_MO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::PowerOp PowerOp;
 if (ATmatch(term, "IntrinsicOperator_PO(<term>)", &PowerOp.term)) {

      if (ofp_traverse_PowerOp(PowerOp.term, &PowerOp)) {
         // MATCHED PowerOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_PO

   return ATtrue;
 }
#endif

 return ATfalse;
}
#endif

//========================================================================================
// R310 defined-operator
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DefinedOperator(ATerm term, OFP::DefinedOperator* DefinedOperator)
{
#ifdef DEBUG_PRINT
   printf("DefinedOperator: %s\n", ATwriteToString(term));
#endif

 OFP::ExtendedIntrinsicOp ExtendedIntrinsicOp;
 if (ATmatch(term, "DefinedOperator_EIO(<term>)", &ExtendedIntrinsicOp.term)) {

      if (ofp_traverse_ExtendedIntrinsicOp(ExtendedIntrinsicOp.term, &ExtendedIntrinsicOp)) {
         // MATCHED ExtendedIntrinsicOp
      } else return ATfalse;

   // MATCHED DefinedOperator_EIO

   return ATtrue;
 }

 OFP::DefinedBinaryOp DefinedBinaryOp;
 if (ATmatch(term, "DefinedOperator_DBO(<term>)", &DefinedBinaryOp.term)) {

      if (ofp_traverse_DefinedBinaryOp(DefinedBinaryOp.term, &DefinedBinaryOp)) {
         // MATCHED DefinedBinaryOp
      } else return ATfalse;

   // MATCHED DefinedOperator_DBO

   return ATtrue;
 }

 OFP::DefinedUnaryOp DefinedUnaryOp;
 if (ATmatch(term, "DefinedOperator_DUO(<term>)", &DefinedUnaryOp.term)) {

      if (ofp_traverse_DefinedUnaryOp(DefinedUnaryOp.term, &DefinedUnaryOp)) {
         // MATCHED DefinedUnaryOp
      } else return ATfalse;

   // MATCHED DefinedOperator_DUO

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R311 extended-intrinsic-op
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ExtendedIntrinsicOp(ATerm term, OFP::ExtendedIntrinsicOp* ExtendedIntrinsicOp)
{
#ifdef DEBUG_PRINT
   printf("ExtendedIntrinsicOp: %s\n", ATwriteToString(term));
#endif

 OFP::IntrinsicOperator IntrinsicOperator;
 if (ATmatch(term, "ExtendedIntrinsicOp(<term>)", &IntrinsicOperator.term)) {

      if (ofp_traverse_IntrinsicOperator(IntrinsicOperator.term, &IntrinsicOperator)) {
         // MATCHED IntrinsicOperator
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R701 primary
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Primary(ATerm term, OFP::Primary* Primary)
{
#ifdef DEBUG_PRINT
   printf("Primary(W): %s\n", ATwriteToString(term));
#endif

   OFP::Constant Constant;
   OFP::Designator Designator;

   if (ofp_traverse_Constant(term, &Constant)) {
      // MATCHED Constant
      Primary->setOptionType(OFP::Primary::Constant_ot);
      Primary->setConstant(Constant.newConstant());
      Primary->inheritPayload(Primary->getConstant());
      return ATtrue;
   }

   else if (ofp_traverse_Designator(term, &Designator)) {
      // MATCHED Designator
      Primary->setOptionType(OFP::Primary::Designator_ot);
      Primary->setDesignator(Designator.newDesignator());
      Primary->inheritPayload(Primary->getDesignator());
      return ATtrue;
   }

   return ATfalse;

#ifdef INPROGRESS
#endif

#ifdef OBSOLETE
 // This leads to a cycle, don't think it should be here
 OFP::Expr Expr;
 if (ATmatch(term, "Primary_E_AMB(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
         Primary->setExpr(Expr.newExpr());
         Primary->inheritPayload(Primary->getExpr());
      } else return ATfalse;

   // MATCHED Primary_E_AMB
   Primary->setOptionType(OFP::Primary::Primary_E_AMB);

   return ATtrue;
 }
#endif

 OFP::TypeParamInquiry TypeParamInquiry;
 if (ATmatch(term, "Primary_TPI_AMB(<term>)", &TypeParamInquiry.term)) {

      if (ofp_traverse_TypeParamInquiry(TypeParamInquiry.term, &TypeParamInquiry)) {
         // MATCHED TypeParamInquiry
         Primary->setTypeParamInquiry(TypeParamInquiry.newTypeParamInquiry());
         Primary->inheritPayload(Primary->getTypeParamInquiry());
      } else return ATfalse;

   // MATCHED Primary_TPI_AMB
   Primary->setOptionType(OFP::Primary::Primary_TPI_AMB);

   return ATtrue;
 }

 OFP::StructureConstructor StructureConstructor;
 if (ATmatch(term, "Primary_SC_AMB(<term>)", &StructureConstructor.term)) {

      if (ofp_traverse_StructureConstructor(StructureConstructor.term, &StructureConstructor)) {
         // MATCHED StructureConstructor
         Primary->setStructureConstructor(StructureConstructor.newStructureConstructor());
         Primary->inheritPayload(Primary->getStructureConstructor());
      } else return ATfalse;

   // MATCHED Primary_SC_AMB
   Primary->setOptionType(OFP::Primary::Primary_SC_AMB);

   return ATtrue;
 }

 OFP::ArrayConstructor ArrayConstructor;
 if (ATmatch(term, "Primary_AC_AMB(<term>)", &ArrayConstructor.term)) {

      if (ofp_traverse_ArrayConstructor(ArrayConstructor.term, &ArrayConstructor)) {
         // MATCHED ArrayConstructor
         Primary->setArrayConstructor(ArrayConstructor.newArrayConstructor());
         Primary->inheritPayload(Primary->getArrayConstructor());
      } else return ATfalse;

   // MATCHED Primary_AC_AMB
   Primary->setOptionType(OFP::Primary::Primary_AC_AMB);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R703 defined-unary-op
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DefinedUnaryOp(ATerm term, OFP::DefinedUnaryOp* DefinedUnaryOp)
{
#ifdef DEBUG_PRINT
   printf("DefinedUnaryOp: %s\n", ATwriteToString(term));
#endif

 OFP::Dop Dop;
 if (ATmatch(term, "DefinedUnaryOp(<term>)", &Dop.term)) {

      if (ofp_traverse_Dop(Dop.term, &Dop)) {
         // MATCHED Dop
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R723 defined-binary-op
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DefinedBinaryOp(ATerm term, OFP::DefinedBinaryOp* DefinedBinaryOp)
{
#ifdef DEBUG_PRINT
   printf("DefinedBinaryOp: %s\n", ATwriteToString(term));
#endif

 OFP::Dop Dop;
 if (ATmatch(term, "DefinedBinaryOp(<term>)", &Dop.term)) {

      if (ofp_traverse_Dop(Dop.term, &Dop)) {
         // MATCHED Dop
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R724 logical-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LogicalExpr(ATerm term, OFP::LogicalExpr* LogicalExpr)
{
#ifdef DEBUG_PRINT
   printf("LogicalExpr: %s\n", ATwriteToString(term));
#endif

 OFP::Expr Expr;
 if (ATmatch(term, "LogicalExpr(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R725 default-char-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DefaultCharExpr(ATerm term, OFP::DefaultCharExpr* DefaultCharExpr)
{
#ifdef DEBUG_PRINT
   printf("DefaultCharExpr: %s\n", ATwriteToString(term));
#endif

 OFP::Expr Expr;
 if (ATmatch(term, "DefaultCharExpr(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R726 int-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntExpr(ATerm term, OFP::IntExpr* IntExpr)
{
#ifdef DEBUG_PRINT
   printf("IntExpr: %s\n", ATwriteToString(term));
#endif

 OFP::Expr Expr;
 if (ATmatch(term, "IntExpr(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R728 specification-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SpecificationExpr(ATerm term, OFP::SpecificationExpr* SpecificationExpr)
{
#ifdef DEBUG_PRINT
   printf("SpecificationExpr: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "SpecificationExpr(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R729 constant-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ConstantExpr(ATerm term, OFP::ConstantExpr* ConstantExpr)
{
#ifdef DEBUG_PRINT
   printf("ConstantExpr: %s\n", ATwriteToString(term));
#endif

 OFP::Expr Expr;
 if (ATmatch(term, "ConstantExpr(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R731 int-constant-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntConstantExpr(ATerm term, OFP::IntConstantExpr* IntConstantExpr)
{
#ifdef DEBUG_PRINT
   printf("IntConstantExpr: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "IntConstantExpr(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R732 assignment-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AssignmentStmt(ATerm term, OFP::AssignmentStmt* AssignmentStmt)
{
#ifdef DEBUG_PRINT
   printf("AssignmentStmt: %s\n", ATwriteToString(term));
#endif

   OFP::Label Label;
   OFP::Variable Variable;
   OFP::Expr Expr;
   OFP::EOS EOS;
   if (ATmatch(term, "AssignmentStmt(<term>,<term>,<term>,<term>)", &Label.term, &Variable.term, &Expr.term, &EOS.term)) {

      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         AssignmentStmt->setLabel(Label.newLabel());
      } // Optional

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
         AssignmentStmt->setVariable(Variable.newVariable());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
         AssignmentStmt->setExpr(Expr.newExpr());
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         AssignmentStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

      ast->build_AssignmentStmt(AssignmentStmt);

      return ATtrue;
   }

   return ATfalse;
}

//========================================================================================
// R733 pointer-assignment-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PointerAssignmentStmt(ATerm term, OFP::PointerAssignmentStmt* PointerAssignmentStmt)
{
#ifdef DEBUG_PRINT
   printf("PointerAssignmentStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ProcPointerObject ProcPointerObject;
 OFP::ProcTarget ProcTarget;
 OFP::EOS EOS;
 if (ATmatch(term, "PointerAssignmentStmt_PPO(<term>,<term>,<term>,<term>)", &Label.term, &ProcPointerObject.term, &ProcTarget.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ProcPointerObject(ProcPointerObject.term, &ProcPointerObject)) {
         // MATCHED ProcPointerObject
      } else return ATfalse;

      if (ofp_traverse_ProcTarget(ProcTarget.term, &ProcTarget)) {
         // MATCHED ProcTarget
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED PointerAssignmentStmt_PPO

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::DataPointerObject DataPointerObject;
 OFP::BoundsRemappingList BoundsRemappingList;
 OFP::DataTarget DataTarget;
 OFP::EOS EOS1;
 if (ATmatch(term, "PointerAssignmentStmt_DPO2(<term>,<term>,<term>,<term>,<term>)", &Label1.term, &DataPointerObject.term, &BoundsRemappingList.term, &DataTarget.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_DataPointerObject(DataPointerObject.term, &DataPointerObject)) {
         // MATCHED DataPointerObject
      } else return ATfalse;

      if (ofp_traverse_BoundsRemappingList(BoundsRemappingList.term, &BoundsRemappingList)) {
         // MATCHED BoundsRemappingList
      } else return ATfalse;

      if (ofp_traverse_DataTarget(DataTarget.term, &DataTarget)) {
         // MATCHED DataTarget
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED PointerAssignmentStmt_DPO2

   return ATtrue;
 }

 OFP::Label Label2;
 OFP::DataPointerObject DataPointerObject1;
 OFP::BoundsSpecList BoundsSpecList;
 OFP::DataTarget DataTarget1;
 OFP::EOS EOS2;
 if (ATmatch(term, "PointerAssignmentStmt_DPO1(<term>,<term>,<term>,<term>,<term>)", &Label2.term, &DataPointerObject1.term, &BoundsSpecList.term, &DataTarget1.term, &EOS2.term)) {

   if (ATmatch(Label2.term, "Some(<term>)", &Label2.term)) {
      if (ofp_traverse_Label(Label2.term, &Label2)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_DataPointerObject(DataPointerObject1.term, &DataPointerObject1)) {
         // MATCHED DataPointerObject
      } else return ATfalse;

   if (ATmatch(BoundsSpecList.term, "Some(<term>)", &BoundsSpecList.term)) {
   if (ATmatch(BoundsSpecList.term, "(<term>)", &BoundsSpecList.term)) {
      if (ofp_traverse_BoundsSpecList(BoundsSpecList.term, &BoundsSpecList)) {
         // MATCHED BoundsSpecList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_DataTarget(DataTarget1.term, &DataTarget1)) {
         // MATCHED DataTarget
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS2.term, &EOS2)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED PointerAssignmentStmt_DPO1

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R734 data-pointer-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataPointerObject(ATerm term, OFP::DataPointerObject* DataPointerObject)
{
#ifdef DEBUG_PRINT
   printf("DataPointerObject: %s\n", ATwriteToString(term));
#endif

 OFP::Variable Variable;
 OFP::Name DataPointerComponentName;
 if (ATmatch(term, "DataPointerObject_V(<term>,<term>)", &Variable.term, &DataPointerComponentName.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

      if (ofp_traverse_Name(DataPointerComponentName.term, &DataPointerComponentName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED DataPointerObject_V

   return ATtrue;
 }

 OFP::VariableName VariableName;
 if (ATmatch(term, "DataPointerObject_VN(<term>)", &VariableName.term)) {

      if (ofp_traverse_VariableName(VariableName.term, &VariableName)) {
         // MATCHED VariableName
      } else return ATfalse;

   // MATCHED DataPointerObject_VN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R735 bounds-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BoundsSpec(ATerm term, OFP::BoundsSpec* BoundsSpec)
{
#ifdef DEBUG_PRINT
   printf("BoundsSpec: %s\n", ATwriteToString(term));
#endif

 OFP::LowerBoundExpr LowerBoundExpr;
 if (ATmatch(term, "BoundsSpec(<term>)", &LowerBoundExpr.term)) {

      if (ofp_traverse_LowerBoundExpr(LowerBoundExpr.term, &LowerBoundExpr)) {
         // MATCHED LowerBoundExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_BoundsSpecList(ATerm term, OFP::BoundsSpecList* BoundsSpecList)
{
#ifdef DEBUG_PRINT
   printf("BoundsSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::BoundsSpec BoundsSpec;
 if (ATmatch(term, "BoundsSpecList(<term>)", &BoundsSpec.term)) {

   ATermList BoundsSpec_tail = (ATermList) ATmake("<term>", BoundsSpec.term);
   while (! ATisEmpty(BoundsSpec_tail)) {
      BoundsSpec.term = ATgetFirst(BoundsSpec_tail);
      BoundsSpec_tail = ATgetNext (BoundsSpec_tail);
      if (ofp_traverse_BoundsSpec(BoundsSpec.term, &BoundsSpec)) {
         // MATCHED BoundsSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R736 bounds-remapping
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BoundsRemapping(ATerm term, OFP::BoundsRemapping* BoundsRemapping)
{
#ifdef DEBUG_PRINT
   printf("BoundsRemapping: %s\n", ATwriteToString(term));
#endif

 OFP::LowerBoundExpr LowerBoundExpr;
 OFP::UpperBoundExpr UpperBoundExpr;
 if (ATmatch(term, "BoundsRemapping(<term>,<term>)", &LowerBoundExpr.term, &UpperBoundExpr.term)) {

      if (ofp_traverse_LowerBoundExpr(LowerBoundExpr.term, &LowerBoundExpr)) {
         // MATCHED LowerBoundExpr
      } else return ATfalse;

      if (ofp_traverse_UpperBoundExpr(UpperBoundExpr.term, &UpperBoundExpr)) {
         // MATCHED UpperBoundExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_BoundsRemappingList(ATerm term, OFP::BoundsRemappingList* BoundsRemappingList)
{
#ifdef DEBUG_PRINT
   printf("BoundsRemappingList: %s\n", ATwriteToString(term));
#endif

 OFP::BoundsRemapping BoundsRemapping;
 if (ATmatch(term, "BoundsRemappingList(<term>)", &BoundsRemapping.term)) {

   ATermList BoundsRemapping_tail = (ATermList) ATmake("<term>", BoundsRemapping.term);
   while (! ATisEmpty(BoundsRemapping_tail)) {
      BoundsRemapping.term = ATgetFirst(BoundsRemapping_tail);
      BoundsRemapping_tail = ATgetNext (BoundsRemapping_tail);
      if (ofp_traverse_BoundsRemapping(BoundsRemapping.term, &BoundsRemapping)) {
         // MATCHED BoundsRemapping
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R737 data-target
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataTarget(ATerm term, OFP::DataTarget* DataTarget)
{
#ifdef DEBUG_PRINT
   printf("DataTarget: %s\n", ATwriteToString(term));
#endif

 OFP::Expr Expr;
 if (ATmatch(term, "DataTarget(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R738 proc-pointer-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcPointerObject(ATerm term, OFP::ProcPointerObject* ProcPointerObject)
{
#ifdef DEBUG_PRINT
   printf("ProcPointerObject: %s\n", ATwriteToString(term));
#endif

 OFP::ProcComponentRef ProcComponentRef;
 if (ATmatch(term, "ProcPointerObject_PCR(<term>)", &ProcComponentRef.term)) {

      if (ofp_traverse_ProcComponentRef(ProcComponentRef.term, &ProcComponentRef)) {
         // MATCHED ProcComponentRef
      } else return ATfalse;

   // MATCHED ProcPointerObject_PCR

   return ATtrue;
 }

 OFP::ProcPointerName ProcPointerName;
 if (ATmatch(term, "ProcPointerObject_PPN(<term>)", &ProcPointerName.term)) {

      if (ofp_traverse_ProcPointerName(ProcPointerName.term, &ProcPointerName)) {
         // MATCHED ProcPointerName
      } else return ATfalse;

   // MATCHED ProcPointerObject_PPN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R739 proc-component-ref
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcComponentRef(ATerm term, OFP::ProcComponentRef* ProcComponentRef)
{
#ifdef DEBUG_PRINT
   printf("ProcComponentRef: %s\n", ATwriteToString(term));
#endif

 OFP::Variable Variable;
 OFP::Name ProcedureComponentName;
 if (ATmatch(term, "ProcComponentRef(<term>,<term>)", &Variable.term, &ProcedureComponentName.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

      if (ofp_traverse_Name(ProcedureComponentName.term, &ProcedureComponentName)) {
         // MATCHED Name
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R740 proc-target
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcTarget(ATerm term, OFP::ProcTarget* ProcTarget)
{
#ifdef DEBUG_PRINT
   printf("ProcTarget: %s\n", ATwriteToString(term));
#endif

 OFP::ProcComponentRef ProcComponentRef;
 if (ATmatch(term, "ProcTarget_PCR(<term>)", &ProcComponentRef.term)) {

      if (ofp_traverse_ProcComponentRef(ProcComponentRef.term, &ProcComponentRef)) {
         // MATCHED ProcComponentRef
      } else return ATfalse;

   // MATCHED ProcTarget_PCR

   return ATtrue;
 }

 OFP::Name ProcedureName;
 if (ATmatch(term, "ProcTarget_PN(<term>)", &ProcedureName.term)) {

      if (ofp_traverse_Name(ProcedureName.term, &ProcedureName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED ProcTarget_PN

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "ProcTarget_E(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED ProcTarget_E

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R741 where-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_WhereStmt(ATerm term, OFP::WhereStmt* WhereStmt)
{
#ifdef DEBUG_PRINT
   printf("WhereStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::MaskExpr MaskExpr;
 OFP::WhereAssignmentStmt WhereAssignmentStmt;
 if (ATmatch(term, "WhereStmt(<term>,<term>,<term>)", &Label.term, &MaskExpr.term, &WhereAssignmentStmt.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_MaskExpr(MaskExpr.term, &MaskExpr)) {
         // MATCHED MaskExpr
      } else return ATfalse;

      if (ofp_traverse_WhereAssignmentStmt(WhereAssignmentStmt.term, &WhereAssignmentStmt)) {
         // MATCHED WhereAssignmentStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R742 where-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_WhereConstruct(ATerm term, OFP::WhereConstruct* WhereConstruct)
{
#ifdef DEBUG_PRINT
   printf("WhereConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::WhereConstructStmt WhereConstructStmt;
 OFP::WhereBodyConstruct WhereBodyConstruct;
 OFP::MaskedElsewhereClause MaskedElsewhereClause;
 OFP::ElsewhereClause ElsewhereClause;
 OFP::EndWhereStmt EndWhereStmt;
 if (ATmatch(term, "WhereConstruct(<term>,<term>,<term>,<term>,<term>)", &WhereConstructStmt.term, &WhereBodyConstruct.term, &MaskedElsewhereClause.term, &ElsewhereClause.term, &EndWhereStmt.term)) {

      if (ofp_traverse_WhereConstructStmt(WhereConstructStmt.term, &WhereConstructStmt)) {
         // MATCHED WhereConstructStmt
      } else return ATfalse;

   ATermList WhereBodyConstruct_tail = (ATermList) ATmake("<term>", WhereBodyConstruct.term);
   while (! ATisEmpty(WhereBodyConstruct_tail)) {
      WhereBodyConstruct.term = ATgetFirst(WhereBodyConstruct_tail);
      WhereBodyConstruct_tail = ATgetNext (WhereBodyConstruct_tail);
      if (ofp_traverse_WhereBodyConstruct(WhereBodyConstruct.term, &WhereBodyConstruct)) {
         // MATCHED WhereBodyConstruct
      } else return ATfalse;
   }

   ATermList MaskedElsewhereClause_tail = (ATermList) ATmake("<term>", MaskedElsewhereClause.term);
   while (! ATisEmpty(MaskedElsewhereClause_tail)) {
      MaskedElsewhereClause.term = ATgetFirst(MaskedElsewhereClause_tail);
      MaskedElsewhereClause_tail = ATgetNext (MaskedElsewhereClause_tail);
      if (ofp_traverse_MaskedElsewhereClause(MaskedElsewhereClause.term, &MaskedElsewhereClause)) {
         // MATCHED MaskedElsewhereClause
      } else return ATfalse;
   }

   if (ATmatch(ElsewhereClause.term, "Some(<term>)", &ElsewhereClause.term)) {
      if (ofp_traverse_ElsewhereClause(ElsewhereClause.term, &ElsewhereClause)) {
         // MATCHED ElsewhereClause
      } else return ATfalse;
   }

      if (ofp_traverse_EndWhereStmt(EndWhereStmt.term, &EndWhereStmt)) {
         // MATCHED EndWhereStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_MaskedElsewhereClause(ATerm term, OFP::MaskedElsewhereClause* MaskedElsewhereClause)
{
#ifdef DEBUG_PRINT
   printf("MaskedElsewhereClause: %s\n", ATwriteToString(term));
#endif

 OFP::MaskedElsewhereStmt MaskedElsewhereStmt;
 OFP::WhereBodyConstruct WhereBodyConstruct;
 if (ATmatch(term, "MaskedElsewhereClause(<term>,<term>)", &MaskedElsewhereStmt.term, &WhereBodyConstruct.term)) {

      if (ofp_traverse_MaskedElsewhereStmt(MaskedElsewhereStmt.term, &MaskedElsewhereStmt)) {
         // MATCHED MaskedElsewhereStmt
      } else return ATfalse;

   ATermList WhereBodyConstruct_tail = (ATermList) ATmake("<term>", WhereBodyConstruct.term);
   while (! ATisEmpty(WhereBodyConstruct_tail)) {
      WhereBodyConstruct.term = ATgetFirst(WhereBodyConstruct_tail);
      WhereBodyConstruct_tail = ATgetNext (WhereBodyConstruct_tail);
      if (ofp_traverse_WhereBodyConstruct(WhereBodyConstruct.term, &WhereBodyConstruct)) {
         // MATCHED WhereBodyConstruct
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ElsewhereClause(ATerm term, OFP::ElsewhereClause* ElsewhereClause)
{
#ifdef DEBUG_PRINT
   printf("ElsewhereClause: %s\n", ATwriteToString(term));
#endif

 OFP::ElsewhereStmt ElsewhereStmt;
 OFP::WhereBodyConstruct WhereBodyConstruct;
 if (ATmatch(term, "ElsewhereClause(<term>,<term>)", &ElsewhereStmt.term, &WhereBodyConstruct.term)) {

      if (ofp_traverse_ElsewhereStmt(ElsewhereStmt.term, &ElsewhereStmt)) {
         // MATCHED ElsewhereStmt
      } else return ATfalse;

   ATermList WhereBodyConstruct_tail = (ATermList) ATmake("<term>", WhereBodyConstruct.term);
   while (! ATisEmpty(WhereBodyConstruct_tail)) {
      WhereBodyConstruct.term = ATgetFirst(WhereBodyConstruct_tail);
      WhereBodyConstruct_tail = ATgetNext (WhereBodyConstruct_tail);
      if (ofp_traverse_WhereBodyConstruct(WhereBodyConstruct.term, &WhereBodyConstruct)) {
         // MATCHED WhereBodyConstruct
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R743 where-construct-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_WhereConstructStmt(ATerm term, OFP::WhereConstructStmt* WhereConstructStmt)
{
#ifdef DEBUG_PRINT
   printf("WhereConstructStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Ident Ident;
 OFP::MaskExpr MaskExpr;
 OFP::EOS EOS;
 if (ATmatch(term, "WhereConstructStmt(<term>,<term>,<term>,<term>)", &Label.term, &Ident.term, &MaskExpr.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(Ident.term, "Some(<term>)", &Ident.term)) {
   if (ATmatch(Ident.term, "(<term>)", &Ident.term)) {
      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
      } else return ATfalse;
   }
   }

      if (ofp_traverse_MaskExpr(MaskExpr.term, &MaskExpr)) {
         // MATCHED MaskExpr
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R744 where-body-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_WhereBodyConstruct(ATerm term, OFP::WhereBodyConstruct* WhereBodyConstruct)
{
#ifdef DEBUG_PRINT
   printf("WhereBodyConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::WhereConstruct WhereConstruct;
 if (ATmatch(term, "WhereBodyConstruct_WC(<term>)", &WhereConstruct.term)) {

      if (ofp_traverse_WhereConstruct(WhereConstruct.term, &WhereConstruct)) {
         // MATCHED WhereConstruct
      } else return ATfalse;

   // MATCHED WhereBodyConstruct_WC

   return ATtrue;
 }

 OFP::WhereStmt WhereStmt;
 if (ATmatch(term, "WhereBodyConstruct_WS(<term>)", &WhereStmt.term)) {

      if (ofp_traverse_WhereStmt(WhereStmt.term, &WhereStmt)) {
         // MATCHED WhereStmt
      } else return ATfalse;

   // MATCHED WhereBodyConstruct_WS

   return ATtrue;
 }

 OFP::WhereAssignmentStmt WhereAssignmentStmt;
 if (ATmatch(term, "WhereBodyConstruct_WAS(<term>)", &WhereAssignmentStmt.term)) {

      if (ofp_traverse_WhereAssignmentStmt(WhereAssignmentStmt.term, &WhereAssignmentStmt)) {
         // MATCHED WhereAssignmentStmt
      } else return ATfalse;

   // MATCHED WhereBodyConstruct_WAS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R745 where-assignment-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_WhereAssignmentStmt(ATerm term, OFP::WhereAssignmentStmt* WhereAssignmentStmt)
{
#ifdef DEBUG_PRINT
   printf("WhereAssignmentStmt: %s\n", ATwriteToString(term));
#endif

 OFP::AssignmentStmt AssignmentStmt;
 if (ATmatch(term, "WhereAssignmentStmt(<term>)", &AssignmentStmt.term)) {

      if (ofp_traverse_AssignmentStmt(AssignmentStmt.term, &AssignmentStmt)) {
         // MATCHED AssignmentStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R746 mask-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_MaskExpr(ATerm term, OFP::MaskExpr* MaskExpr)
{
#ifdef DEBUG_PRINT
   printf("MaskExpr: %s\n", ATwriteToString(term));
#endif

 OFP::LogicalExpr LogicalExpr;
 if (ATmatch(term, "MaskExpr(<term>)", &LogicalExpr.term)) {

      if (ofp_traverse_LogicalExpr(LogicalExpr.term, &LogicalExpr)) {
         // MATCHED LogicalExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R747 masked-elsewhere-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_MaskedElsewhereStmt(ATerm term, OFP::MaskedElsewhereStmt* MaskedElsewhereStmt)
{
#ifdef DEBUG_PRINT
   printf("MaskedElsewhereStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::MaskExpr MaskExpr;
 OFP::Ident Ident;
 OFP::EOS EOS;
 if (ATmatch(term, "MaskedElsewhereStmt(<term>,<term>,<term>,<term>)", &Label.term, &MaskExpr.term, &Ident.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_MaskExpr(MaskExpr.term, &MaskExpr)) {
         // MATCHED MaskExpr
      } else return ATfalse;

   if (ATmatch(Ident.term, "Some(<term>)", &Ident.term)) {
      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R748 elsewhere-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ElsewhereStmt(ATerm term, OFP::ElsewhereStmt* ElsewhereStmt)
{
#ifdef DEBUG_PRINT
   printf("ElsewhereStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Ident Ident;
 OFP::EOS EOS;
 if (ATmatch(term, "ElsewhereStmt(<term>,<term>,<term>)", &Label.term, &Ident.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(Ident.term, "Some(<term>)", &Ident.term)) {
      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R749 endwhere-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndWhereStmt(ATerm term, OFP::EndWhereStmt* EndWhereStmt)
{
#ifdef DEBUG_PRINT
   printf("EndWhereStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Ident Ident;
 OFP::EOS EOS;
 if (ATmatch(term, "EndWhereStmt(<term>,<term>,<term>)", &Label.term, &Ident.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(Ident.term, "Some(<term>)", &Ident.term)) {
      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R750 forall-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ForallConstruct(ATerm term, OFP::ForallConstruct* ForallConstruct)
{
#ifdef DEBUG_PRINT
   printf("ForallConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::ForallConstructStmt ForallConstructStmt;
 OFP::ForallBodyConstruct ForallBodyConstruct;
 OFP::EndForallStmt EndForallStmt;
 if (ATmatch(term, "ForallConstruct(<term>,<term>,<term>)", &ForallConstructStmt.term, &ForallBodyConstruct.term, &EndForallStmt.term)) {

      if (ofp_traverse_ForallConstructStmt(ForallConstructStmt.term, &ForallConstructStmt)) {
         // MATCHED ForallConstructStmt
      } else return ATfalse;

   ATermList ForallBodyConstruct_tail = (ATermList) ATmake("<term>", ForallBodyConstruct.term);
   while (! ATisEmpty(ForallBodyConstruct_tail)) {
      ForallBodyConstruct.term = ATgetFirst(ForallBodyConstruct_tail);
      ForallBodyConstruct_tail = ATgetNext (ForallBodyConstruct_tail);
      if (ofp_traverse_ForallBodyConstruct(ForallBodyConstruct.term, &ForallBodyConstruct)) {
         // MATCHED ForallBodyConstruct
      } else return ATfalse;
   }

      if (ofp_traverse_EndForallStmt(EndForallStmt.term, &EndForallStmt)) {
         // MATCHED EndForallStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R751 forall-construct-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ForallConstructStmt(ATerm term, OFP::ForallConstructStmt* ForallConstructStmt)
{
#ifdef DEBUG_PRINT
   printf("ForallConstructStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name ForallConstructName;
 OFP::ForallHeader ForallHeader;
 OFP::EOS EOS;
 if (ATmatch(term, "ForallConstructStmt(<term>,<term>,<term>,<term>)", &Label.term, &ForallConstructName.term, &ForallHeader.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(ForallConstructName.term, "Some(<term>)", &ForallConstructName.term)) {
   if (ATmatch(ForallConstructName.term, "(<term>)", &ForallConstructName.term)) {
      if (ofp_traverse_Name(ForallConstructName.term, &ForallConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

      if (ofp_traverse_ForallHeader(ForallHeader.term, &ForallHeader)) {
         // MATCHED ForallHeader
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R752 forall-header
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ForallHeader(ATerm term, OFP::ForallHeader* ForallHeader)
{
#ifdef DEBUG_PRINT
   printf("ForallHeader: %s\n", ATwriteToString(term));
#endif

 OFP::TypeSpec TypeSpec;
 OFP::ForallTripletSpecList ForallTripletSpecList;
 OFP::MaskExpr MaskExpr;
 if (ATmatch(term, "ForallHeader(<term>,<term>,<term>)", &TypeSpec.term, &ForallTripletSpecList.term, &MaskExpr.term)) {

   if (ATmatch(TypeSpec.term, "Some(<term>)", &TypeSpec.term)) {
   if (ATmatch(TypeSpec.term, "(<term>)", &TypeSpec.term)) {
      if (ofp_traverse_TypeSpec(TypeSpec.term, &TypeSpec)) {
         // MATCHED TypeSpec
      } else return ATfalse;
   }
   }

      if (ofp_traverse_ForallTripletSpecList(ForallTripletSpecList.term, &ForallTripletSpecList)) {
         // MATCHED ForallTripletSpecList
      } else return ATfalse;

   if (ATmatch(MaskExpr.term, "Some(<term>)", &MaskExpr.term)) {
   if (ATmatch(MaskExpr.term, "(<term>)", &MaskExpr.term)) {
      if (ofp_traverse_MaskExpr(MaskExpr.term, &MaskExpr)) {
         // MATCHED MaskExpr
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R753 forall-triplet-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ForallTripletSpec(ATerm term, OFP::ForallTripletSpec* ForallTripletSpec)
{
#ifdef DEBUG_PRINT
   printf("ForallTripletSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Name IndexName;
 OFP::ForallLimit ForallLimit;
 OFP::ForallLimit ForallLimit1;
 OFP::ForallStep ForallStep;
 if (ATmatch(term, "ForallTripletSpec(<term>,<term>,<term>,<term>)", &IndexName.term, &ForallLimit.term, &ForallLimit1.term, &ForallStep.term)) {

      if (ofp_traverse_Name(IndexName.term, &IndexName)) {
         // MATCHED Name
      } else return ATfalse;

      if (ofp_traverse_ForallLimit(ForallLimit.term, &ForallLimit)) {
         // MATCHED ForallLimit
      } else return ATfalse;

      if (ofp_traverse_ForallLimit(ForallLimit1.term, &ForallLimit1)) {
         // MATCHED ForallLimit
      } else return ATfalse;

   if (ATmatch(ForallStep.term, "Some(<term>)", &ForallStep.term)) {
   if (ATmatch(ForallStep.term, "(<term>)", &ForallStep.term)) {
      if (ofp_traverse_ForallStep(ForallStep.term, &ForallStep)) {
         // MATCHED ForallStep
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ForallTripletSpecList(ATerm term, OFP::ForallTripletSpecList* ForallTripletSpecList)
{
#ifdef DEBUG_PRINT
   printf("ForallTripletSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ForallTripletSpec ForallTripletSpec;
 if (ATmatch(term, "ForallTripletSpecList(<term>)", &ForallTripletSpec.term)) {

   ATermList ForallTripletSpec_tail = (ATermList) ATmake("<term>", ForallTripletSpec.term);
   while (! ATisEmpty(ForallTripletSpec_tail)) {
      ForallTripletSpec.term = ATgetFirst(ForallTripletSpec_tail);
      ForallTripletSpec_tail = ATgetNext (ForallTripletSpec_tail);
      if (ofp_traverse_ForallTripletSpec(ForallTripletSpec.term, &ForallTripletSpec)) {
         // MATCHED ForallTripletSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R754 forall-limit
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ForallLimit(ATerm term, OFP::ForallLimit* ForallLimit)
{
#ifdef DEBUG_PRINT
   printf("ForallLimit: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "ForallLimit(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R755 forall-step
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ForallStep(ATerm term, OFP::ForallStep* ForallStep)
{
#ifdef DEBUG_PRINT
   printf("ForallStep: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "ForallStep(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R756 forall-body-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ForallBodyConstruct(ATerm term, OFP::ForallBodyConstruct* ForallBodyConstruct)
{
#ifdef DEBUG_PRINT
   printf("ForallBodyConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::ForallStmt ForallStmt;
 if (ATmatch(term, "ForallBodyConstruct_FS(<term>)", &ForallStmt.term)) {

      if (ofp_traverse_ForallStmt(ForallStmt.term, &ForallStmt)) {
         // MATCHED ForallStmt
      } else return ATfalse;

   // MATCHED ForallBodyConstruct_FS

   return ATtrue;
 }

 OFP::ForallConstruct ForallConstruct;
 if (ATmatch(term, "ForallBodyConstruct_FC(<term>)", &ForallConstruct.term)) {

      if (ofp_traverse_ForallConstruct(ForallConstruct.term, &ForallConstruct)) {
         // MATCHED ForallConstruct
      } else return ATfalse;

   // MATCHED ForallBodyConstruct_FC

   return ATtrue;
 }

 OFP::WhereConstruct WhereConstruct;
 if (ATmatch(term, "ForallBodyConstruct_WC(<term>)", &WhereConstruct.term)) {

      if (ofp_traverse_WhereConstruct(WhereConstruct.term, &WhereConstruct)) {
         // MATCHED WhereConstruct
      } else return ATfalse;

   // MATCHED ForallBodyConstruct_WC

   return ATtrue;
 }

 OFP::WhereStmt WhereStmt;
 if (ATmatch(term, "ForallBodyConstruct_WS(<term>)", &WhereStmt.term)) {

      if (ofp_traverse_WhereStmt(WhereStmt.term, &WhereStmt)) {
         // MATCHED WhereStmt
      } else return ATfalse;

   // MATCHED ForallBodyConstruct_WS

   return ATtrue;
 }

 OFP::ForallAssignmentStmt ForallAssignmentStmt;
 if (ATmatch(term, "ForallBodyConstruct_FAS(<term>)", &ForallAssignmentStmt.term)) {

      if (ofp_traverse_ForallAssignmentStmt(ForallAssignmentStmt.term, &ForallAssignmentStmt)) {
         // MATCHED ForallAssignmentStmt
      } else return ATfalse;

   // MATCHED ForallBodyConstruct_FAS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R757 forall-assignment-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ForallAssignmentStmt(ATerm term, OFP::ForallAssignmentStmt* ForallAssignmentStmt)
{
#ifdef DEBUG_PRINT
   printf("ForallAssignmentStmt: %s\n", ATwriteToString(term));
#endif

 OFP::PointerAssignmentStmt PointerAssignmentStmt;
 if (ATmatch(term, "ForallAssignmentStmt_PAS(<term>)", &PointerAssignmentStmt.term)) {

      if (ofp_traverse_PointerAssignmentStmt(PointerAssignmentStmt.term, &PointerAssignmentStmt)) {
         // MATCHED PointerAssignmentStmt
      } else return ATfalse;

   // MATCHED ForallAssignmentStmt_PAS

   return ATtrue;
 }

 OFP::AssignmentStmt AssignmentStmt;
 if (ATmatch(term, "ForallAssignmentStmt_AS(<term>)", &AssignmentStmt.term)) {

      if (ofp_traverse_AssignmentStmt(AssignmentStmt.term, &AssignmentStmt)) {
         // MATCHED AssignmentStmt
      } else return ATfalse;

   // MATCHED ForallAssignmentStmt_AS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R758 end-forall-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndForallStmt(ATerm term, OFP::EndForallStmt* EndForallStmt)
{
#ifdef DEBUG_PRINT
   printf("EndForallStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name ForallConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndForallStmt(<term>,<term>,<term>)", &Label.term, &ForallConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(ForallConstructName.term, "Some(<term>)", &ForallConstructName.term)) {
      if (ofp_traverse_Name(ForallConstructName.term, &ForallConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R759 forall-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ForallStmt(ATerm term, OFP::ForallStmt* ForallStmt)
{
#ifdef DEBUG_PRINT
   printf("ForallStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ForallHeader ForallHeader;
 OFP::ForallAssignmentStmt ForallAssignmentStmt;
 if (ATmatch(term, "ForallStmt(<term>,<term>,<term>)", &Label.term, &ForallHeader.term, &ForallAssignmentStmt.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ForallHeader(ForallHeader.term, &ForallHeader)) {
         // MATCHED ForallHeader
      } else return ATfalse;

      if (ofp_traverse_ForallAssignmentStmt(ForallAssignmentStmt.term, &ForallAssignmentStmt)) {
         // MATCHED ForallAssignmentStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R801 block
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Block(ATerm term, OFP::Block* Block)
{
#ifdef DEBUG_PRINT
   printf("Block: %s\n", ATwriteToString(term));
#endif

 OFP::ExecutionPartConstruct ExecutionPartConstruct;
 if (ATmatch(term, "Block(<term>)", &ExecutionPartConstruct.term)) {

   ATermList ExecutionPartConstruct_tail = (ATermList) ATmake("<term>", ExecutionPartConstruct.term);
   while (! ATisEmpty(ExecutionPartConstruct_tail)) {
      ExecutionPartConstruct.term = ATgetFirst(ExecutionPartConstruct_tail);
      ExecutionPartConstruct_tail = ATgetNext (ExecutionPartConstruct_tail);
      if (ofp_traverse_ExecutionPartConstruct(ExecutionPartConstruct.term, &ExecutionPartConstruct)) {
         // MATCHED ExecutionPartConstruct
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R802 associate-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AssociateConstruct(ATerm term, OFP::AssociateConstruct* AssociateConstruct)
{
#ifdef DEBUG_PRINT
   printf("AssociateConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::AssociateStmt AssociateStmt;
 OFP::Block Block;
 OFP::EndAssociateStmt EndAssociateStmt;
 if (ATmatch(term, "AssociateConstruct(<term>,<term>,<term>)", &AssociateStmt.term, &Block.term, &EndAssociateStmt.term)) {

      if (ofp_traverse_AssociateStmt(AssociateStmt.term, &AssociateStmt)) {
         // MATCHED AssociateStmt
      } else return ATfalse;

      if (ofp_traverse_Block(Block.term, &Block)) {
         // MATCHED Block
      } else return ATfalse;

      if (ofp_traverse_EndAssociateStmt(EndAssociateStmt.term, &EndAssociateStmt)) {
         // MATCHED EndAssociateStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R803 associate-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AssociateStmt(ATerm term, OFP::AssociateStmt* AssociateStmt)
{
#ifdef DEBUG_PRINT
   printf("AssociateStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name AssociateConstructName;
 OFP::AssociationList AssociationList;
 OFP::EOS EOS;
 if (ATmatch(term, "AssociateStmt(<term>,<term>,<term>,<term>)", &Label.term, &AssociateConstructName.term, &AssociationList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(AssociateConstructName.term, "Some(<term>)", &AssociateConstructName.term)) {
   if (ATmatch(AssociateConstructName.term, "(<term>)", &AssociateConstructName.term)) {
      if (ofp_traverse_Name(AssociateConstructName.term, &AssociateConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

      if (ofp_traverse_AssociationList(AssociationList.term, &AssociationList)) {
         // MATCHED AssociationList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R804 association
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Association(ATerm term, OFP::Association* Association)
{
#ifdef DEBUG_PRINT
   printf("Association: %s\n", ATwriteToString(term));
#endif

 OFP::Name AssociateName;
 OFP::Selector Selector;
 if (ATmatch(term, "Association(<term>,<term>)", &AssociateName.term, &Selector.term)) {

      if (ofp_traverse_Name(AssociateName.term, &AssociateName)) {
         // MATCHED Name
      } else return ATfalse;

      if (ofp_traverse_Selector(Selector.term, &Selector)) {
         // MATCHED Selector
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AssociationList(ATerm term, OFP::AssociationList* AssociationList)
{
#ifdef DEBUG_PRINT
   printf("AssociationList: %s\n", ATwriteToString(term));
#endif

 OFP::Association Association;
 if (ATmatch(term, "AssociationList(<term>)", &Association.term)) {

   ATermList Association_tail = (ATermList) ATmake("<term>", Association.term);
   while (! ATisEmpty(Association_tail)) {
      Association.term = ATgetFirst(Association_tail);
      Association_tail = ATgetNext (Association_tail);
      if (ofp_traverse_Association(Association.term, &Association)) {
         // MATCHED Association
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R805 selector
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Selector(ATerm term, OFP::Selector* Selector)
{
#ifdef DEBUG_PRINT
   printf("Selector: %s\n", ATwriteToString(term));
#endif

 OFP::Variable Variable;
 if (ATmatch(term, "Selector_V(<term>)", &Variable.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

   // MATCHED Selector_V

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "Selector_E(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED Selector_E

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R806 end-associate-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndAssociateStmt(ATerm term, OFP::EndAssociateStmt* EndAssociateStmt)
{
#ifdef DEBUG_PRINT
   printf("EndAssociateStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name AssociateConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndAssociateStmt(<term>,<term>,<term>)", &Label.term, &AssociateConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(AssociateConstructName.term, "Some(<term>)", &AssociateConstructName.term)) {
      if (ofp_traverse_Name(AssociateConstructName.term, &AssociateConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R807 block-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BlockConstruct(ATerm term, OFP::BlockConstruct* BlockConstruct)
{
#ifdef DEBUG_PRINT
   printf("BlockConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::BlockStmt BlockStmt;
 OFP::SpecificationPart SpecificationPart;
 OFP::Block Block;
 OFP::EndBlockStmt EndBlockStmt;
 if (ATmatch(term, "BlockConstruct(<term>,<term>,<term>,<term>)", &BlockStmt.term, &SpecificationPart.term, &Block.term, &EndBlockStmt.term)) {

      if (ofp_traverse_BlockStmt(BlockStmt.term, &BlockStmt)) {
         // MATCHED BlockStmt
      } else return ATfalse;

      if (ofp_traverse_SpecificationPart(SpecificationPart.term, &SpecificationPart)) {
         // MATCHED SpecificationPart
      } else return ATfalse;

      if (ofp_traverse_Block(Block.term, &Block)) {
         // MATCHED Block
      } else return ATfalse;

      if (ofp_traverse_EndBlockStmt(EndBlockStmt.term, &EndBlockStmt)) {
         // MATCHED EndBlockStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R808 block-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BlockStmt(ATerm term, OFP::BlockStmt* BlockStmt)
{
#ifdef DEBUG_PRINT
   printf("BlockStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name BlockConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "BlockStmt(<term>,<term>,<term>)", &Label.term, &BlockConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(BlockConstructName.term, "Some(<term>)", &BlockConstructName.term)) {
   if (ATmatch(BlockConstructName.term, "(<term>)", &BlockConstructName.term)) {
      if (ofp_traverse_Name(BlockConstructName.term, &BlockConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R809 end-block-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndBlockStmt(ATerm term, OFP::EndBlockStmt* EndBlockStmt)
{
#ifdef DEBUG_PRINT
   printf("EndBlockStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name BlockConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndBlockStmt(<term>,<term>,<term>)", &Label.term, &BlockConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(BlockConstructName.term, "Some(<term>)", &BlockConstructName.term)) {
      if (ofp_traverse_Name(BlockConstructName.term, &BlockConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R810 critical-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CriticalConstruct(ATerm term, OFP::CriticalConstruct* CriticalConstruct)
{
#ifdef DEBUG_PRINT
   printf("CriticalConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::CriticalStmt CriticalStmt;
 OFP::Block Block;
 OFP::EndCriticalStmt EndCriticalStmt;
 if (ATmatch(term, "CriticalConstruct(<term>,<term>,<term>)", &CriticalStmt.term, &Block.term, &EndCriticalStmt.term)) {

      if (ofp_traverse_CriticalStmt(CriticalStmt.term, &CriticalStmt)) {
         // MATCHED CriticalStmt
      } else return ATfalse;

      if (ofp_traverse_Block(Block.term, &Block)) {
         // MATCHED Block
      } else return ATfalse;

      if (ofp_traverse_EndCriticalStmt(EndCriticalStmt.term, &EndCriticalStmt)) {
         // MATCHED EndCriticalStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R811 critical-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CriticalStmt(ATerm term, OFP::CriticalStmt* CriticalStmt)
{
#ifdef DEBUG_PRINT
   printf("CriticalStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name CriticalConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "CriticalStmt(<term>,<term>,<term>)", &Label.term, &CriticalConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(CriticalConstructName.term, "Some(<term>)", &CriticalConstructName.term)) {
   if (ATmatch(CriticalConstructName.term, "(<term>)", &CriticalConstructName.term)) {
      if (ofp_traverse_Name(CriticalConstructName.term, &CriticalConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R812 end-critical-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndCriticalStmt(ATerm term, OFP::EndCriticalStmt* EndCriticalStmt)
{
#ifdef DEBUG_PRINT
   printf("EndCriticalStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name CriticalConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndCriticalStmt(<term>,<term>,<term>)", &Label.term, &CriticalConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(CriticalConstructName.term, "Some(<term>)", &CriticalConstructName.term)) {
      if (ofp_traverse_Name(CriticalConstructName.term, &CriticalConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R813 do-construct
//----------------------------------------------------------------------------------------

//========================================================================================
// R814 block-do-construct
//----------------------------------------------------------------------------------------

//========================================================================================
// R815 do-stmt
//----------------------------------------------------------------------------------------

//========================================================================================
// R816 label-do-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LabelDoStmt(ATerm term, OFP::LabelDoStmt* LabelDoStmt)
{
#ifdef DEBUG_PRINT
   printf("LabelDoStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name DoConstructName;
 OFP::LblRef LblRef;
 OFP::LoopControl LoopControl;
 OFP::EOS EOS;
 if (ATmatch(term, "LabelDoStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &DoConstructName.term, &LblRef.term, &LoopControl.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(DoConstructName.term, "Some(<term>)", &DoConstructName.term)) {
   if (ATmatch(DoConstructName.term, "(<term>)", &DoConstructName.term)) {
      if (ofp_traverse_Name(DoConstructName.term, &DoConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

   if (ATmatch(LoopControl.term, "Some(<term>)", &LoopControl.term)) {
      if (ofp_traverse_LoopControl(LoopControl.term, &LoopControl)) {
         // MATCHED LoopControl
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R817 non-label-do-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_NonlabelDoStmt(ATerm term, OFP::NonlabelDoStmt* NonlabelDoStmt)
{
#ifdef DEBUG_PRINT
   printf("NonlabelDoStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name DoConstructName;
 OFP::LoopControl LoopControl;
 OFP::EOS EOS;
 if (ATmatch(term, "NonlabelDoStmt(<term>,<term>,<term>,<term>)", &Label.term, &DoConstructName.term, &LoopControl.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(DoConstructName.term, "Some(<term>)", &DoConstructName.term)) {
   if (ATmatch(DoConstructName.term, "(<term>)", &DoConstructName.term)) {
      if (ofp_traverse_Name(DoConstructName.term, &DoConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

   if (ATmatch(LoopControl.term, "Some(<term>)", &LoopControl.term)) {
      if (ofp_traverse_LoopControl(LoopControl.term, &LoopControl)) {
         // MATCHED LoopControl
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R818 loop-control
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LoopControl(ATerm term, OFP::LoopControl* LoopControl)
{
#ifdef DEBUG_PRINT
   printf("LoopControl: %s\n", ATwriteToString(term));
#endif

 OFP::ForallHeader ForallHeader; 
 if (ATmatch(term, "LoopControl_CONCURRENT(<term>)", &ForallHeader.term)) {

      if (ofp_traverse_ForallHeader(ForallHeader.term, &ForallHeader)) {
         // MATCHED ForallHeader
      } else return ATfalse;

   // MATCHED LoopControl_CONCURRENT

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "LoopControl_WHILE(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED LoopControl_WHILE

   return ATtrue;
 }

 OFP::DoVariable DoVariable;
 OFP::Expr Expr1;
 OFP::Expr Expr2;
 OFP::Expr Expr3;
 if (ATmatch(term, "LoopControl_DV(<term>,<term>,<term>,<term>)", &DoVariable.term, &Expr1.term, &Expr2.term, &Expr3.term)) {

      if (ofp_traverse_DoVariable(DoVariable.term, &DoVariable)) {
         // MATCHED DoVariable
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
      } else return ATfalse;

   if (ATmatch(Expr3.term, "Some(<term>)", &Expr3.term)) {
   if (ATmatch(Expr3.term, "(<term>)", &Expr3.term)) {
      if (ofp_traverse_Expr(Expr3.term, &Expr3)) {
         // MATCHED Expr
      } else return ATfalse;
   }
   }

   // MATCHED LoopControl_DV

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R819 do-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DoVariable(ATerm term, OFP::DoVariable* DoVariable)
{
#ifdef DEBUG_PRINT
   printf("DoVariable: %s\n", ATwriteToString(term));
#endif

 OFP::Name ScalarIntVariableName;
 if (ATmatch(term, "DoVariable(<term>)", &ScalarIntVariableName.term)) {

      if (ofp_traverse_Name(ScalarIntVariableName.term, &ScalarIntVariableName)) {
         // MATCHED Name
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R820 do-block
//----------------------------------------------------------------------------------------

//========================================================================================
// R821 end-do
//----------------------------------------------------------------------------------------

//========================================================================================
// R822 end-do-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndDoStmt(ATerm term, OFP::EndDoStmt* EndDoStmt)
{
#ifdef DEBUG_PRINT
   printf("EndDoStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name DoConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndDoStmt(<term>,<term>,<term>)", &Label.term, &DoConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(DoConstructName.term, "Some(<term>)", &DoConstructName.term)) {
      if (ofp_traverse_Name(DoConstructName.term, &DoConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R823 nonblock-do-construct
//----------------------------------------------------------------------------------------

//========================================================================================
// R824 action-term-do-construct
//----------------------------------------------------------------------------------------

//========================================================================================
// R825 do-body
//----------------------------------------------------------------------------------------

//========================================================================================
// R826 do-term-action-stmt
//----------------------------------------------------------------------------------------

//========================================================================================
// R827 outer-shared-do-construct
//----------------------------------------------------------------------------------------

//========================================================================================
// R828 shared-term-do-construct
//----------------------------------------------------------------------------------------

//========================================================================================
// R829 inner-shared-do-construct
//----------------------------------------------------------------------------------------

//========================================================================================
// R830 do-term-shared-stmt
//----------------------------------------------------------------------------------------

//========================================================================================
// R831 cycle-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CycleStmt(ATerm term, OFP::CycleStmt* CycleStmt)
{
#ifdef DEBUG_PRINT
   printf("CycleStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name DoConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "CycleStmt(<term>,<term>,<term>)", &Label.term, &DoConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(DoConstructName.term, "Some(<term>)", &DoConstructName.term)) {
      if (ofp_traverse_Name(DoConstructName.term, &DoConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R832 if-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IfConstruct(ATerm term, OFP::IfConstruct* IfConstruct)
{
#ifdef DEBUG_PRINT
   printf("IfConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::IfThenStmt IfThenStmt;
 OFP::Block Block;
 OFP::ElseIfStmtAndBlock ElseIfStmtAndBlock;
 OFP::ElseStmtAndBlock ElseStmtAndBlock;
 OFP::EndIfStmt EndIfStmt;
 if (ATmatch(term, "IfConstruct(<term>,<term>,<term>,<term>,<term>)", &IfThenStmt.term, &Block.term, &ElseIfStmtAndBlock.term, &ElseStmtAndBlock.term, &EndIfStmt.term)) {

      if (ofp_traverse_IfThenStmt(IfThenStmt.term, &IfThenStmt)) {
         // MATCHED IfThenStmt
      } else return ATfalse;

      if (ofp_traverse_Block(Block.term, &Block)) {
         // MATCHED Block
      } else return ATfalse;

   ATermList ElseIfStmtAndBlock_tail = (ATermList) ATmake("<term>", ElseIfStmtAndBlock.term);
   while (! ATisEmpty(ElseIfStmtAndBlock_tail)) {
      ElseIfStmtAndBlock.term = ATgetFirst(ElseIfStmtAndBlock_tail);
      ElseIfStmtAndBlock_tail = ATgetNext (ElseIfStmtAndBlock_tail);
      if (ofp_traverse_ElseIfStmtAndBlock(ElseIfStmtAndBlock.term, &ElseIfStmtAndBlock)) {
         // MATCHED ElseIfStmtAndBlock
      } else return ATfalse;
   }

   if (ATmatch(ElseStmtAndBlock.term, "Some(<term>)", &ElseStmtAndBlock.term)) {
      if (ofp_traverse_ElseStmtAndBlock(ElseStmtAndBlock.term, &ElseStmtAndBlock)) {
         // MATCHED ElseStmtAndBlock
      } else return ATfalse;
   }

      if (ofp_traverse_EndIfStmt(EndIfStmt.term, &EndIfStmt)) {
         // MATCHED EndIfStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ElseIfStmtAndBlock(ATerm term, OFP::ElseIfStmtAndBlock* ElseIfStmtAndBlock)
{
#ifdef DEBUG_PRINT
   printf("ElseIfStmtAndBlock: %s\n", ATwriteToString(term));
#endif

 OFP::ElseIfStmt ElseIfStmt;
 OFP::Block Block;
 if (ATmatch(term, "ElseIfStmt_Block(<term>,<term>)", &ElseIfStmt.term, &Block.term)) {

      if (ofp_traverse_ElseIfStmt(ElseIfStmt.term, &ElseIfStmt)) {
         // MATCHED ElseIfStmt
      } else return ATfalse;

      if (ofp_traverse_Block(Block.term, &Block)) {
         // MATCHED Block
      } else return ATfalse;

   // MATCHED ElseIfStmt_Block

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ElseStmtAndBlock(ATerm term, OFP::ElseStmtAndBlock* ElseStmtAndBlock)
{
#ifdef DEBUG_PRINT
   printf("ElseStmtAndBlock: %s\n", ATwriteToString(term));
#endif

 OFP::ElseStmt ElseStmt;
 OFP::Block Block;
 if (ATmatch(term, "ElseStmt_Block(<term>,<term>)", &ElseStmt.term, &Block.term)) {

      if (ofp_traverse_ElseStmt(ElseStmt.term, &ElseStmt)) {
         // MATCHED ElseStmt
      } else return ATfalse;

      if (ofp_traverse_Block(Block.term, &Block)) {
         // MATCHED Block
      } else return ATfalse;

   // MATCHED ElseStmt_Block

   return ATtrue;
 }

 return ATfalse;
}


//========================================================================================
// R833 if-then-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IfThenStmt(ATerm term, OFP::IfThenStmt* IfThenStmt)
{
#ifdef DEBUG_PRINT
   printf("IfThenStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name IfConstructName;
 OFP::LogicalExpr LogicalExpr;
 OFP::EOS EOS;
 if (ATmatch(term, "IfThenStmt(<term>,<term>,<term>,<term>)", &Label.term, &IfConstructName.term, &LogicalExpr.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(IfConstructName.term, "Some(<term>)", &IfConstructName.term)) {
   if (ATmatch(IfConstructName.term, "(<term>)", &IfConstructName.term)) {
      if (ofp_traverse_Name(IfConstructName.term, &IfConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

      if (ofp_traverse_LogicalExpr(LogicalExpr.term, &LogicalExpr)) {
         // MATCHED LogicalExpr
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R834 else-if-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ElseIfStmt(ATerm term, OFP::ElseIfStmt* ElseIfStmt)
{
#ifdef DEBUG_PRINT
   printf("ElseIfStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::LogicalExpr LogicalExpr;
 OFP::Name IfConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "ElseIfStmt(<term>,<term>,<term>,<term>)", &Label.term, &LogicalExpr.term, &IfConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_LogicalExpr(LogicalExpr.term, &LogicalExpr)) {
         // MATCHED LogicalExpr
      } else return ATfalse;

   if (ATmatch(IfConstructName.term, "Some(<term>)", &IfConstructName.term)) {
      if (ofp_traverse_Name(IfConstructName.term, &IfConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R835 else-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ElseStmt(ATerm term, OFP::ElseStmt* ElseStmt)
{
#ifdef DEBUG_PRINT
   printf("ElseStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name IfConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "ElseStmt(<term>,<term>,<term>)", &Label.term, &IfConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(IfConstructName.term, "Some(<term>)", &IfConstructName.term)) {
      if (ofp_traverse_Name(IfConstructName.term, &IfConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R836 end-if-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndIfStmt(ATerm term, OFP::EndIfStmt* EndIfStmt)
{
#ifdef DEBUG_PRINT
   printf("EndIfStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name IfConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndIfStmt(<term>,<term>,<term>)", &Label.term, &IfConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(IfConstructName.term, "Some(<term>)", &IfConstructName.term)) {
      if (ofp_traverse_Name(IfConstructName.term, &IfConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R837 if-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IfStmt(ATerm term, OFP::IfStmt* IfStmt)
{
#ifdef DEBUG_PRINT
   printf("IfStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::LogicalExpr LogicalExpr;
 OFP::ActionStmt ActionStmt;
 if (ATmatch(term, "IfStmt(<term>,<term>,<term>)", &Label.term, &LogicalExpr.term, &ActionStmt.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_LogicalExpr(LogicalExpr.term, &LogicalExpr)) {
         // MATCHED LogicalExpr
      } else return ATfalse;

      if (ofp_traverse_ActionStmt(ActionStmt.term, &ActionStmt)) {
         // MATCHED ActionStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R838 case-construct
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CaseConstruct(ATerm term, OFP::CaseConstruct* CaseConstruct)
{
#ifdef DEBUG_PRINT
   printf("CaseConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::SelectCaseStmt SelectCaseStmt;
 OFP::CaseStmtAndBlock CaseStmtAndBlock;
 OFP::EndSelectStmt EndSelectStmt;
 if (ATmatch(term, "CaseConstruct(<term>,<term>,<term>)", &SelectCaseStmt.term, &CaseStmtAndBlock.term, &EndSelectStmt.term)) {

      if (ofp_traverse_SelectCaseStmt(SelectCaseStmt.term, &SelectCaseStmt)) {
         // MATCHED SelectCaseStmt
      } else return ATfalse;

   ATermList CaseStmtAndBlock_tail = (ATermList) ATmake("<term>", CaseStmtAndBlock.term);
   while (! ATisEmpty(CaseStmtAndBlock_tail)) {
      CaseStmtAndBlock.term = ATgetFirst(CaseStmtAndBlock_tail);
      CaseStmtAndBlock_tail = ATgetNext (CaseStmtAndBlock_tail);
      if (ofp_traverse_CaseStmtAndBlock(CaseStmtAndBlock.term, &CaseStmtAndBlock)) {
         // MATCHED CaseStmtAndBlock
      } else return ATfalse;
   }

      if (ofp_traverse_EndSelectStmt(EndSelectStmt.term, &EndSelectStmt)) {
         // MATCHED EndSelectStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CaseStmtAndBlock(ATerm term, OFP::CaseStmtAndBlock* CaseStmtAndBlock)
{
#ifdef DEBUG_PRINT
   printf("CaseStmtAndBlock: %s\n", ATwriteToString(term));
#endif

 OFP::CaseStmt CaseStmt;
 OFP::Block Block;
 if (ATmatch(term, "CaseStmt_Block(<term>,<term>)", &CaseStmt.term, &Block.term)) {

      if (ofp_traverse_CaseStmt(CaseStmt.term, &CaseStmt)) {
         // MATCHED CaseStmt
      } else return ATfalse;

      if (ofp_traverse_Block(Block.term, &Block)) {
         // MATCHED Block
      } else return ATfalse;

   // MATCHED CaseStmt_Block

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R839 select-case-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SelectCaseStmt(ATerm term, OFP::SelectCaseStmt* SelectCaseStmt)
{
#ifdef DEBUG_PRINT
   printf("SelectCaseStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name CaseConstructName;
 OFP::CaseExpr CaseExpr;
 OFP::EOS EOS;
 if (ATmatch(term, "SelectCaseStmt(<term>,<term>,<term>,<term>)", &Label.term, &CaseConstructName.term, &CaseExpr.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(CaseConstructName.term, "Some(<term>)", &CaseConstructName.term)) {
   if (ATmatch(CaseConstructName.term, "(<term>)", &CaseConstructName.term)) {
      if (ofp_traverse_Name(CaseConstructName.term, &CaseConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

      if (ofp_traverse_CaseExpr(CaseExpr.term, &CaseExpr)) {
         // MATCHED CaseExpr
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R840 case-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CaseStmt(ATerm term, OFP::CaseStmt* CaseStmt)
{
#ifdef DEBUG_PRINT
   printf("CaseStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::CaseSelector CaseSelector;
 OFP::Name CaseConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "CaseStmt(<term>,<term>,<term>,<term>)", &Label.term, &CaseSelector.term, &CaseConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_CaseSelector(CaseSelector.term, &CaseSelector)) {
         // MATCHED CaseSelector
      } else return ATfalse;

   if (ATmatch(CaseConstructName.term, "Some(<term>)", &CaseConstructName.term)) {
      if (ofp_traverse_Name(CaseConstructName.term, &CaseConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R841 end-select-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndSelectStmt(ATerm term, OFP::EndSelectStmt* EndSelectStmt)
{
#ifdef DEBUG_PRINT
   printf("EndSelectStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name CaseConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndSelectStmt(<term>,<term>,<term>)", &Label.term, &CaseConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(CaseConstructName.term, "Some(<term>)", &CaseConstructName.term)) {
      if (ofp_traverse_Name(CaseConstructName.term, &CaseConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R842 case-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CaseExpr(ATerm term, OFP::CaseExpr* CaseExpr)
{
#ifdef DEBUG_PRINT
   printf("CaseExpr: %s\n", ATwriteToString(term));
#endif

 OFP::Expr Expr;
 if (ATmatch(term, "CaseExpr(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R843 case-selector
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CaseSelector(ATerm term, OFP::CaseSelector* CaseSelector)
{
#ifdef DEBUG_PRINT
   printf("CaseSelector: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "CaseSelector_DEFAULT")) {

   // MATCHED CaseSelector_DEFAULT

   return ATtrue;
 }

 OFP::CaseValueRangeList CaseValueRangeList;
 if (ATmatch(term, "CaseSelector_CVRL(<term>)", &CaseValueRangeList.term)) {

      if (ofp_traverse_CaseValueRangeList(CaseValueRangeList.term, &CaseValueRangeList)) {
         // MATCHED CaseValueRangeList
      } else return ATfalse;

   // MATCHED CaseSelector_CVRL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R844 case-value-range
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CaseValueRange(ATerm term, OFP::CaseValueRange* CaseValueRange)
{
#ifdef DEBUG_PRINT
   printf("CaseValueRange: %s\n", ATwriteToString(term));
#endif

 OFP::CaseValue CaseValue;
 OFP::CaseValue CaseValue1;
 if (ATmatch(term, "CaseValueRange_CVCV(<term>,<term>)", &CaseValue.term, &CaseValue1.term)) {

      if (ofp_traverse_CaseValue(CaseValue.term, &CaseValue)) {
         // MATCHED CaseValue
      } else return ATfalse;

      if (ofp_traverse_CaseValue(CaseValue1.term, &CaseValue1)) {
         // MATCHED CaseValue
      } else return ATfalse;

   // MATCHED CaseValueRange_CVCV

   return ATtrue;
 }

 OFP::CaseValue CaseValue2;
 if (ATmatch(term, "CaseValueRange_CV(<term>)", &CaseValue2.term)) {

      if (ofp_traverse_CaseValue(CaseValue2.term, &CaseValue2)) {
         // MATCHED CaseValue
      } else return ATfalse;

   // MATCHED CaseValueRange_CV

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CaseValueRangeList(ATerm term, OFP::CaseValueRangeList* CaseValueRangeList)
{
#ifdef DEBUG_PRINT
   printf("CaseValueRangeList: %s\n", ATwriteToString(term));
#endif

 OFP::CaseValueRange CaseValueRange;
 if (ATmatch(term, "CaseValueRangeList(<term>)", &CaseValueRange.term)) {

   ATermList CaseValueRange_tail = (ATermList) ATmake("<term>", CaseValueRange.term);
   while (! ATisEmpty(CaseValueRange_tail)) {
      CaseValueRange.term = ATgetFirst(CaseValueRange_tail);
      CaseValueRange_tail = ATgetNext (CaseValueRange_tail);
      if (ofp_traverse_CaseValueRange(CaseValueRange.term, &CaseValueRange)) {
         // MATCHED CaseValueRange
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R845 case-value
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CaseValue(ATerm term, OFP::CaseValue* CaseValue)
{
#ifdef DEBUG_PRINT
   printf("CaseValue: %s\n", ATwriteToString(term));
#endif

 OFP::ConstantExpr ConstantExpr;
 if (ATmatch(term, "CaseValue(<term>)", &ConstantExpr.term)) {

      if (ofp_traverse_ConstantExpr(ConstantExpr.term, &ConstantExpr)) {
         // MATCHED ConstantExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R846 select-type-construct
//----------------------------------------------------------------------------------------

ATbool ofp_traverse_SelectTypeConstruct(ATerm term, OFP::SelectTypeConstruct* SelectTypeConstruct)
{
#ifdef DEBUG_PRINT
   printf("SelectTypeConstruct: %s\n", ATwriteToString(term));
#endif

 OFP::SelectTypeStmt SelectTypeStmt;
 OFP::TypeGuardStmtAndBlock TypeGuardStmtAndBlock;
 OFP::EndSelectTypeStmt EndSelectTypeStmt;
 if (ATmatch(term, "SelectTypeConstruct(<term>,<term>,<term>)", &SelectTypeStmt.term, &TypeGuardStmtAndBlock.term, &EndSelectTypeStmt.term)) {

      if (ofp_traverse_SelectTypeStmt(SelectTypeStmt.term, &SelectTypeStmt)) {
         // MATCHED SelectTypeStmt
      } else return ATfalse;

   ATermList TypeGuardStmtAndBlock_tail = (ATermList) ATmake("<term>", TypeGuardStmtAndBlock.term);
   while (! ATisEmpty(TypeGuardStmtAndBlock_tail)) {
      TypeGuardStmtAndBlock.term = ATgetFirst(TypeGuardStmtAndBlock_tail);
      TypeGuardStmtAndBlock_tail = ATgetNext (TypeGuardStmtAndBlock_tail);
      if (ofp_traverse_TypeGuardStmtAndBlock(TypeGuardStmtAndBlock.term, &TypeGuardStmtAndBlock)) {
         // MATCHED TypeGuardStmtAndBlock
      } else return ATfalse;
   }

      if (ofp_traverse_EndSelectTypeStmt(EndSelectTypeStmt.term, &EndSelectTypeStmt)) {
         // MATCHED EndSelectTypeStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_TypeGuardStmtAndBlock(ATerm term, OFP::TypeGuardStmtAndBlock* TypeGuardStmtAndBlock)
{
#ifdef DEBUG_PRINT
   printf("TypeGuardStmtAndBlock: %s\n", ATwriteToString(term));
#endif

 OFP::TypeGuardStmt TypeGuardStmt;
 OFP::Block Block;
 if (ATmatch(term, "TypeGuardStmt_Block(<term>,<term>)", &TypeGuardStmt.term, &Block.term)) {

      if (ofp_traverse_TypeGuardStmt(TypeGuardStmt.term, &TypeGuardStmt)) {
         // MATCHED TypeGuardStmt
      } else return ATfalse;

      if (ofp_traverse_Block(Block.term, &Block)) {
         // MATCHED Block
      } else return ATfalse;

   // MATCHED TypeGuardStmt_Block

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R847 select-type-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SelectTypeStmt(ATerm term, OFP::SelectTypeStmt* SelectTypeStmt)
{
#ifdef DEBUG_PRINT
   printf("SelectTypeStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Name SelectConstructName;
 OFP::Name AssociateName;
 OFP::Selector Selector;
 OFP::EOS EOS;
 if (ATmatch(term, "SelectTypeStmt(<term>,<term>,<term>,<term>)", &SelectConstructName.term, &AssociateName.term, &Selector.term, &EOS.term)) {

   if (ATmatch(SelectConstructName.term, "Some(<term>)", &SelectConstructName.term)) {
   if (ATmatch(SelectConstructName.term, "(<term>)", &SelectConstructName.term)) {
      if (ofp_traverse_Name(SelectConstructName.term, &SelectConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

   if (ATmatch(AssociateName.term, "Some(<term>)", &AssociateName.term)) {
   if (ATmatch(AssociateName.term, "(<term>)", &AssociateName.term)) {
      if (ofp_traverse_Name(AssociateName.term, &AssociateName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

      if (ofp_traverse_Selector(Selector.term, &Selector)) {
         // MATCHED Selector
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R848 type-guard-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_TypeGuardStmt(ATerm term, OFP::TypeGuardStmt* TypeGuardStmt)
{
#ifdef DEBUG_PRINT
   printf("TypeGuardStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Name SelectConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "TypeGuardStmt_CLASS_DEF(<term>,<term>)", &SelectConstructName.term, &EOS.term)) {

   if (ATmatch(SelectConstructName.term, "Some(<term>)", &SelectConstructName.term)) {
      if (ofp_traverse_Name(SelectConstructName.term, &SelectConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED TypeGuardStmt_CLASS_DEF

   return ATtrue;
 }

 OFP::DerivedTypeSpec DerivedTypeSpec;
 OFP::Name SelectConstructName1;
 OFP::EOS EOS1;
 if (ATmatch(term, "TypeGuardStmt_CLASS(<term>,<term>,<term>)", &DerivedTypeSpec.term, &SelectConstructName1.term, &EOS1.term)) {

      if (ofp_traverse_DerivedTypeSpec(DerivedTypeSpec.term, &DerivedTypeSpec)) {
         // MATCHED DerivedTypeSpec
      } else return ATfalse;

   if (ATmatch(SelectConstructName1.term, "Some(<term>)", &SelectConstructName1.term)) {
      if (ofp_traverse_Name(SelectConstructName1.term, &SelectConstructName1)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED TypeGuardStmt_CLASS

   return ATtrue;
 }

 OFP::TypeSpec TypeSpec;
 OFP::Name SelectConstructName2;
 OFP::EOS EOS2;
 if (ATmatch(term, "TypeGuardStmt_TYPE(<term>,<term>,<term>)", &TypeSpec.term, &SelectConstructName2.term, &EOS2.term)) {

      if (ofp_traverse_TypeSpec(TypeSpec.term, &TypeSpec)) {
         // MATCHED TypeSpec
      } else return ATfalse;

   if (ATmatch(SelectConstructName2.term, "Some(<term>)", &SelectConstructName2.term)) {
      if (ofp_traverse_Name(SelectConstructName2.term, &SelectConstructName2)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS2.term, &EOS2)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED TypeGuardStmt_TYPE

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R849 end-select-type-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndSelectTypeStmt(ATerm term, OFP::EndSelectTypeStmt* EndSelectTypeStmt)
{
#ifdef DEBUG_PRINT
   printf("EndSelectTypeStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Name SelectConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndSelectTypeStmt(<term>,<term>)", &SelectConstructName.term, &EOS.term)) {

   if (ATmatch(SelectConstructName.term, "Some(<term>)", &SelectConstructName.term)) {
      if (ofp_traverse_Name(SelectConstructName.term, &SelectConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R850 exit-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ExitStmt(ATerm term, OFP::ExitStmt* ExitStmt)
{
#ifdef DEBUG_PRINT
   printf("ExitStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name ConstructName;
 OFP::EOS EOS;
 if (ATmatch(term, "ExitStmt(<term>,<term>,<term>)", &Label.term, &ConstructName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(ConstructName.term, "Some(<term>)", &ConstructName.term)) {
      if (ofp_traverse_Name(ConstructName.term, &ConstructName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R851 goto-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_GotoStmt(ATerm term, OFP::GotoStmt* GotoStmt)
{
#ifdef DEBUG_PRINT
   printf("GotoStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::LblRef LblRef;
 OFP::EOS EOS;
 if (ATmatch(term, "GotoStmt(<term>,<term>,<term>)", &Label.term, &LblRef.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R852 computed-goto-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ComputedGotoStmt(ATerm term, OFP::ComputedGotoStmt* ComputedGotoStmt)
{
#ifdef DEBUG_PRINT
   printf("ComputedGotoStmt: %s\n", ATwriteToString(term));
#endif

#ifdef OBSOLETE
 OFP::Label Label;
 //OBSOLETE OFP::LabelList LabelList;
 OFP::Expr Expr;
 OFP::EOS EOS;
 if (ATmatch(term, "ComputedGotoStmt(<term>,<term>,<term>,<term>)", &Label.term, &LabelList.term, &Expr.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

//OBSOLETE       if (ofp_traverse_LabelList(LabelList.term, &LabelList)) {
//OBSOLETE          // MATCHED LabelList
//OBSOLETE       } else return ATfalse;

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }
#endif

 return ATfalse;
}

//========================================================================================
// R853 arithmetic-if-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ArithmeticIfStmt(ATerm term, OFP::ArithmeticIfStmt* ArithmeticIfStmt)
{
#ifdef DEBUG_PRINT
   printf("ArithmeticIfStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Expr Expr;
 OFP::LblRef LblRef;
 OFP::LblRef LblRef1;
 OFP::LblRef LblRef2;
 OFP::EOS EOS;
 if (ATmatch(term, "ArithmeticIfStmt(<term>,<term>,<term>,<term>,<term>,<term>)", &Label.term, &Expr.term, &LblRef.term, &LblRef1.term, &LblRef2.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

      if (ofp_traverse_LblRef(LblRef1.term, &LblRef1)) {
         // MATCHED LblRef
      } else return ATfalse;

      if (ofp_traverse_LblRef(LblRef2.term, &LblRef2)) {
         // MATCHED LblRef
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R854 continue-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ContinueStmt(ATerm term, OFP::ContinueStmt* ContinueStmt)
{
#ifdef DEBUG_PRINT
   printf("ContinueStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "ContinueStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R855 stop-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_StopStmt(ATerm term, OFP::StopStmt* StopStmt)
{
#ifdef DEBUG_PRINT
   printf("StopStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::StopCode StopCode;
 OFP::EOS EOS;
 if (ATmatch(term, "StopStmt(<term>,<term>,<term>)", &Label.term, &StopCode.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(StopCode.term, "Some(<term>)", &StopCode.term)) {
      if (ofp_traverse_StopCode(StopCode.term, &StopCode)) {
         // MATCHED StopCode
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R856 error-stop-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ErrorStopStmt(ATerm term, OFP::ErrorStopStmt* ErrorStopStmt)
{
#ifdef DEBUG_PRINT
   printf("ErrorStopStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::StopCode StopCode;
 OFP::EOS EOS;
 if (ATmatch(term, "ErrorStopStmt(<term>,<term>,<term>)", &Label.term, &StopCode.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(StopCode.term, "Some(<term>)", &StopCode.term)) {
      if (ofp_traverse_StopCode(StopCode.term, &StopCode)) {
         // MATCHED StopCode
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R857 stop-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_StopCode(ATerm term, OFP::StopCode* StopCode)
{
#ifdef DEBUG_PRINT
   printf("StopCode: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "StopCode_SI(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED StopCode_SI

   return ATtrue;
 }

 OFP::Scon Scon;
 if (ATmatch(term, "StopCode_SD(<term>)", &Scon.term)) {

      if (ofp_traverse_Scon(Scon.term, &Scon)) {
         // MATCHED Scon
      } else return ATfalse;

   // MATCHED StopCode_SD

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R858 sync-all-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SyncAllStmt(ATerm term, OFP::SyncAllStmt* SyncAllStmt)
{
#ifdef DEBUG_PRINT
   printf("SyncAllStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "SyncAllStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::SyncStatList SyncStatList;
 OFP::EOS EOS1;
 if (ATmatch(term, "SyncAllStmt_SSL(<term>,<term>,<term>)", &Label1.term, &SyncStatList.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(SyncStatList.term, "Some(<term>)", &SyncStatList.term)) {
      if (ofp_traverse_SyncStatList(SyncStatList.term, &SyncStatList)) {
         // MATCHED SyncStatList
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED SyncAllStmt_SSL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R859 sync-stat
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SyncStat(ATerm term, OFP::SyncStat* SyncStat)
{
#ifdef DEBUG_PRINT
   printf("SyncStat: %s\n", ATwriteToString(term));
#endif

 OFP::ErrmsgVariable ErrmsgVariable;
 if (ATmatch(term, "SyncStat_ERRMSG(<term>)", &ErrmsgVariable.term)) {

      if (ofp_traverse_ErrmsgVariable(ErrmsgVariable.term, &ErrmsgVariable)) {
         // MATCHED ErrmsgVariable
      } else return ATfalse;

   // MATCHED SyncStat_ERRMSG

   return ATtrue;
 }

 OFP::StatVariable StatVariable;
 if (ATmatch(term, "SyncStat_STAT(<term>)", &StatVariable.term)) {

      if (ofp_traverse_StatVariable(StatVariable.term, &StatVariable)) {
         // MATCHED StatVariable
      } else return ATfalse;

   // MATCHED SyncStat_STAT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_SyncStatList(ATerm term, OFP::SyncStatList* SyncStatList)
{
#ifdef DEBUG_PRINT
   printf("SyncStatList: %s\n", ATwriteToString(term));
#endif

 OFP::SyncStat SyncStat;
 if (ATmatch(term, "SyncStatList(<term>)", &SyncStat.term)) {

   ATermList SyncStat_tail = (ATermList) ATmake("<term>", SyncStat.term);
   while (! ATisEmpty(SyncStat_tail)) {
      SyncStat.term = ATgetFirst(SyncStat_tail);
      SyncStat_tail = ATgetNext (SyncStat_tail);
      if (ofp_traverse_SyncStat(SyncStat.term, &SyncStat)) {
         // MATCHED SyncStat
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R860 sync-images-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SyncImagesStmt(ATerm term, OFP::SyncImagesStmt* SyncImagesStmt)
{
#ifdef DEBUG_PRINT
   printf("SyncImagesStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ImageSet ImageSet;
 OFP::SyncStatList SyncStatList;
 OFP::EOS EOS;
 if (ATmatch(term, "SyncImagesStmt(<term>,<term>,<term>,<term>)", &Label.term, &ImageSet.term, &SyncStatList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ImageSet(ImageSet.term, &ImageSet)) {
         // MATCHED ImageSet
      } else return ATfalse;

   if (ATmatch(SyncStatList.term, "Some(<term>)", &SyncStatList.term)) {
   if (ATmatch(SyncStatList.term, "(<term>)", &SyncStatList.term)) {
      if (ofp_traverse_SyncStatList(SyncStatList.term, &SyncStatList)) {
         // MATCHED SyncStatList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R861 image-set
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ImageSet(ATerm term, OFP::ImageSet* ImageSet)
{
#ifdef DEBUG_PRINT
   printf("ImageSet: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "ImageSet_STAR")) {

   // MATCHED ImageSet_STAR

   return ATtrue;
 }

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "ImageSet_IE(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   // MATCHED ImageSet_IE

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R862 sync-memory-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SyncMemoryStmt(ATerm term, OFP::SyncMemoryStmt* SyncMemoryStmt)
{
#ifdef DEBUG_PRINT
   printf("SyncMemoryStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "SyncMemoryStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::SyncStatList SyncStatList;
 OFP::EOS EOS1;
 if (ATmatch(term, "SyncMemoryStmt_SSL(<term>,<term>,<term>)", &Label1.term, &SyncStatList.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(SyncStatList.term, "Some(<term>)", &SyncStatList.term)) {
      if (ofp_traverse_SyncStatList(SyncStatList.term, &SyncStatList)) {
         // MATCHED SyncStatList
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED SyncMemoryStmt_SSL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R863 lock-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LockStmt(ATerm term, OFP::LockStmt* LockStmt)
{
#ifdef DEBUG_PRINT
   printf("LockStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::LockVariable LockVariable;
 OFP::LockStatList LockStatList;
 OFP::EOS EOS;
 if (ATmatch(term, "LockStmt(<term>,<term>,<term>,<term>)", &Label.term, &LockVariable.term, &LockStatList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_LockVariable(LockVariable.term, &LockVariable)) {
         // MATCHED LockVariable
      } else return ATfalse;

   if (ATmatch(LockStatList.term, "Some(<term>)", &LockStatList.term)) {
   if (ATmatch(LockStatList.term, "(<term>)", &LockStatList.term)) {
      if (ofp_traverse_LockStatList(LockStatList.term, &LockStatList)) {
         // MATCHED LockStatList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R864 lock-stat
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LockStat(ATerm term, OFP::LockStat* LockStat)
{
#ifdef DEBUG_PRINT
   printf("LockStat: %s\n", ATwriteToString(term));
#endif

 OFP::SyncStat SyncStat;
 if (ATmatch(term, "LockStat_SS(<term>)", &SyncStat.term)) {

      if (ofp_traverse_SyncStat(SyncStat.term, &SyncStat)) {
         // MATCHED SyncStat
      } else return ATfalse;

   // MATCHED LockStat_SS

   return ATtrue;
 }

 OFP::LogicalVariable LogicalVariable;
 if (ATmatch(term, "LockStat_LV(<term>)", &LogicalVariable.term)) {

      if (ofp_traverse_LogicalVariable(LogicalVariable.term, &LogicalVariable)) {
         // MATCHED LogicalVariable
      } else return ATfalse;

   // MATCHED LockStat_LV

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_LockStatList(ATerm term, OFP::LockStatList* LockStatList)
{
#ifdef DEBUG_PRINT
   printf("LockStatList: %s\n", ATwriteToString(term));
#endif

 OFP::LockStat LockStat;
 if (ATmatch(term, "LockStatList(<term>)", &LockStat.term)) {

   ATermList LockStat_tail = (ATermList) ATmake("<term>", LockStat.term);
   while (! ATisEmpty(LockStat_tail)) {
      LockStat.term = ATgetFirst(LockStat_tail);
      LockStat_tail = ATgetNext (LockStat_tail);
      if (ofp_traverse_LockStat(LockStat.term, &LockStat)) {
         // MATCHED LockStat
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R865 unlock-stat
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_UnlockStmt(ATerm term, OFP::UnlockStmt* UnlockStmt)
{
#ifdef DEBUG_PRINT
   printf("UnlockStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::LockVariable LockVariable;
 OFP::SyncStatList SyncStatList;
 OFP::EOS EOS;
 if (ATmatch(term, "UnlockStmt(<term>,<term>,<term>,<term>)", &Label.term, &LockVariable.term, &SyncStatList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_LockVariable(LockVariable.term, &LockVariable)) {
         // MATCHED LockVariable
      } else return ATfalse;

   if (ATmatch(SyncStatList.term, "Some(<term>)", &SyncStatList.term)) {
   if (ATmatch(SyncStatList.term, "(<term>)", &SyncStatList.term)) {
      if (ofp_traverse_SyncStatList(SyncStatList.term, &SyncStatList)) {
         // MATCHED SyncStatList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R866 lock-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LockVariable(ATerm term, OFP::LockVariable* LockVariable)
{
#ifdef DEBUG_PRINT
   printf("LockVariable: %s\n", ATwriteToString(term));
#endif

 OFP::Variable Variable;
 if (ATmatch(term, "LockVariable(<term>)", &Variable.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// OBSOLETE: pause-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PauseStmt(ATerm term, OFP::PauseStmt* PauseStmt)
{
#ifdef DEBUG_PRINT
   printf("PauseStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::StopCode StopCode;
 OFP::EOS EOS;
 if (ATmatch(term, "PauseStmt(<term>,<term>,<term>)", &Label.term, &StopCode.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(StopCode.term, "Some(<term>)", &StopCode.term)) {
      if (ofp_traverse_StopCode(StopCode.term, &StopCode)) {
         // MATCHED StopCode
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R901 io-unit
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IoUnit(ATerm term, OFP::IoUnit* IoUnit)
{
#ifdef DEBUG_PRINT
   printf("IoUnit: %s\n", ATwriteToString(term));
#endif

 OFP::CharVariable CharVariable;
 if (ATmatch(term, "IoUnit_IFV(<term>)", &CharVariable.term)) {

      if (ofp_traverse_CharVariable(CharVariable.term, &CharVariable)) {
         // MATCHED CharVariable
      } else return ATfalse;

   // MATCHED IoUnit_IFV

   return ATtrue;
 }

 if (ATmatch(term, "IoUnit_STAR")) {

   // MATCHED IoUnit_STAR

   return ATtrue;
 }

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "IoUnit_FUN(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   // MATCHED IoUnit_FUN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R902 file-unit-number
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FileUnitNumber(ATerm term, OFP::FileUnitNumber* FileUnitNumber)
{
#ifdef DEBUG_PRINT
   printf("FileUnitNumber: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "FileUnitNumber(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R903 internal-file-variable
//----------------------------------------------------------------------------------------

//========================================================================================
// R904 open-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_OpenStmt(ATerm term, OFP::OpenStmt* OpenStmt)
{
#ifdef DEBUG_PRINT
   printf("OpenStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ConnectSpecList ConnectSpecList;
 OFP::EOS EOS;
 if (ATmatch(term, "OpenStmt(<term>,<term>,<term>)", &Label.term, &ConnectSpecList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ConnectSpecList(ConnectSpecList.term, &ConnectSpecList)) {
         // MATCHED ConnectSpecList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R905 connect-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ConnectSpec(ATerm term, OFP::ConnectSpec* ConnectSpec)
{
#ifdef DEBUG_PRINT
   printf("ConnectSpec: %s\n", ATwriteToString(term));
#endif

 OFP::DefaultCharExpr DefaultCharExpr;
 if (ATmatch(term, "ConnectSpec_STATUS(<term>)", &DefaultCharExpr.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr.term, &DefaultCharExpr)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_STATUS

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr1;
 if (ATmatch(term, "ConnectSpec_SIGN(<term>)", &DefaultCharExpr1.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr1.term, &DefaultCharExpr1)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_SIGN

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr2;
 if (ATmatch(term, "ConnectSpec_ROUND(<term>)", &DefaultCharExpr2.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr2.term, &DefaultCharExpr2)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_ROUND

   return ATtrue;
 }

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "ConnectSpec_RECL(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_RECL

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr3;
 if (ATmatch(term, "ConnectSpec_POSITION(<term>)", &DefaultCharExpr3.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr3.term, &DefaultCharExpr3)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_POSITION

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr4;
 if (ATmatch(term, "ConnectSpec_PAD(<term>)", &DefaultCharExpr4.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr4.term, &DefaultCharExpr4)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_PAD

   return ATtrue;
 }

 OFP::IntVariable IntVariable;
 if (ATmatch(term, "ConnectSpec_NEWUNIT(<term>)", &IntVariable.term)) {

      if (ofp_traverse_IntVariable(IntVariable.term, &IntVariable)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED ConnectSpec_NEWUNIT

   return ATtrue;
 }

 OFP::IntVariable IntVariable1;
 if (ATmatch(term, "ConnectSpec_IOSTAT(<term>)", &IntVariable1.term)) {

      if (ofp_traverse_IntVariable(IntVariable1.term, &IntVariable1)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED ConnectSpec_IOSTAT

   return ATtrue;
 }

 OFP::IomsgVariable IomsgVariable;
 if (ATmatch(term, "ConnectSpec_IOMSG(<term>)", &IomsgVariable.term)) {

      if (ofp_traverse_IomsgVariable(IomsgVariable.term, &IomsgVariable)) {
         // MATCHED IomsgVariable
      } else return ATfalse;

   // MATCHED ConnectSpec_IOMSG

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr5;
 if (ATmatch(term, "ConnectSpec_FORM(<term>)", &DefaultCharExpr5.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr5.term, &DefaultCharExpr5)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_FORM

   return ATtrue;
 }

 OFP::FileNameExpr FileNameExpr;
 if (ATmatch(term, "ConnectSpec_FILE(<term>)", &FileNameExpr.term)) {

      if (ofp_traverse_FileNameExpr(FileNameExpr.term, &FileNameExpr)) {
         // MATCHED FileNameExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_FILE

   return ATtrue;
 }

 OFP::LblRef LblRef;
 if (ATmatch(term, "ConnectSpec_ERR(<term>)", &LblRef.term)) {

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED ConnectSpec_ERR

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr6;
 if (ATmatch(term, "ConnectSpec_ENCODING(<term>)", &DefaultCharExpr6.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr6.term, &DefaultCharExpr6)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_ENCODING

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr7;
 if (ATmatch(term, "ConnectSpec_DELIM(<term>)", &DefaultCharExpr7.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr7.term, &DefaultCharExpr7)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_DELIM

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr8;
 if (ATmatch(term, "ConnectSpec_DECIMAL(<term>)", &DefaultCharExpr8.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr8.term, &DefaultCharExpr8)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_DECIMAL

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr9;
 if (ATmatch(term, "ConnectSpec_BLANK(<term>)", &DefaultCharExpr9.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr9.term, &DefaultCharExpr9)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_BLANK

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr10;
 if (ATmatch(term, "ConnectSpec_ASYNC(<term>)", &DefaultCharExpr10.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr10.term, &DefaultCharExpr10)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_ASYNC

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr11;
 if (ATmatch(term, "ConnectSpec_ACTION(<term>)", &DefaultCharExpr11.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr11.term, &DefaultCharExpr11)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_ACTION

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr12;
 if (ATmatch(term, "ConnectSpec_ACCESS(<term>)", &DefaultCharExpr12.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr12.term, &DefaultCharExpr12)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED ConnectSpec_ACCESS

   return ATtrue;
 }

 OFP::FileUnitNumber FileUnitNumber;
 if (ATmatch(term, "ConnectSpec_UNIT(<term>)", &FileUnitNumber.term)) {

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

   // MATCHED ConnectSpec_UNIT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ConnectSpecList(ATerm term, OFP::ConnectSpecList* ConnectSpecList)
{
#ifdef DEBUG_PRINT
   printf("ConnectSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ConnectSpec ConnectSpec;
 if (ATmatch(term, "ConnectSpecList(<term>)", &ConnectSpec.term)) {

   ATermList ConnectSpec_tail = (ATermList) ATmake("<term>", ConnectSpec.term);
   while (! ATisEmpty(ConnectSpec_tail)) {
      ConnectSpec.term = ATgetFirst(ConnectSpec_tail);
      ConnectSpec_tail = ATgetNext (ConnectSpec_tail);
      if (ofp_traverse_ConnectSpec(ConnectSpec.term, &ConnectSpec)) {
         // MATCHED ConnectSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R906 file-name-expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FileNameExpr(ATerm term, OFP::FileNameExpr* FileNameExpr)
{
#ifdef DEBUG_PRINT
   printf("FileNameExpr: %s\n", ATwriteToString(term));
#endif

 OFP::DefaultCharExpr DefaultCharExpr;
 if (ATmatch(term, "FileNameExpr(<term>)", &DefaultCharExpr.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr.term, &DefaultCharExpr)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R907 io-msg-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IomsgVariable(ATerm term, OFP::IomsgVariable* IomsgVariable)
{
#ifdef DEBUG_PRINT
   printf("IomsgVariable: %s\n", ATwriteToString(term));
#endif

 OFP::DefaultCharExpr DefaultCharExpr;
 if (ATmatch(term, "IomsgVariable(<term>)", &DefaultCharExpr.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr.term, &DefaultCharExpr)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R908 close-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CloseStmt(ATerm term, OFP::CloseStmt* CloseStmt)
{
#ifdef DEBUG_PRINT
   printf("CloseStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::CloseSpecList CloseSpecList;
 OFP::EOS EOS;
 if (ATmatch(term, "CloseStmt(<term>,<term>,<term>)", &Label.term, &CloseSpecList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_CloseSpecList(CloseSpecList.term, &CloseSpecList)) {
         // MATCHED CloseSpecList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R909 close-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CloseSpec(ATerm term, OFP::CloseSpec* CloseSpec)
{
#ifdef DEBUG_PRINT
   printf("CloseSpec: %s\n", ATwriteToString(term));
#endif

 OFP::DefaultCharExpr DefaultCharExpr;
 if (ATmatch(term, "CloseSpec_STATUS(<term>)", &DefaultCharExpr.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr.term, &DefaultCharExpr)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED CloseSpec_STATUS

   return ATtrue;
 }

 OFP::LblRef LblRef;
 if (ATmatch(term, "CloseSpec_ERR(<term>)", &LblRef.term)) {

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED CloseSpec_ERR

   return ATtrue;
 }

 OFP::IomsgVariable IomsgVariable;
 if (ATmatch(term, "CloseSpec_IOMSG(<term>)", &IomsgVariable.term)) {

      if (ofp_traverse_IomsgVariable(IomsgVariable.term, &IomsgVariable)) {
         // MATCHED IomsgVariable
      } else return ATfalse;

   // MATCHED CloseSpec_IOMSG

   return ATtrue;
 }

 OFP::IntVariable IntVariable;
 if (ATmatch(term, "CloseSpec_IOSTAT(<term>)", &IntVariable.term)) {

      if (ofp_traverse_IntVariable(IntVariable.term, &IntVariable)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED CloseSpec_IOSTAT

   return ATtrue;
 }

 OFP::FileUnitNumber FileUnitNumber;
 if (ATmatch(term, "CloseSpec_UNIT(<term>)", &FileUnitNumber.term)) {

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

   // MATCHED CloseSpec_UNIT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CloseSpecList(ATerm term, OFP::CloseSpecList* CloseSpecList)
{
#ifdef DEBUG_PRINT
   printf("CloseSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::CloseSpec CloseSpec;
 if (ATmatch(term, "CloseSpecList(<term>)", &CloseSpec.term)) {

   ATermList CloseSpec_tail = (ATermList) ATmake("<term>", CloseSpec.term);
   while (! ATisEmpty(CloseSpec_tail)) {
      CloseSpec.term = ATgetFirst(CloseSpec_tail);
      CloseSpec_tail = ATgetNext (CloseSpec_tail);
      if (ofp_traverse_CloseSpec(CloseSpec.term, &CloseSpec)) {
         // MATCHED CloseSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R910 read-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ReadStmt(ATerm term, OFP::ReadStmt* ReadStmt)
{
#ifdef DEBUG_PRINT
   printf("ReadStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Format Format;
 OFP::InputItemList InputItemList;
 OFP::EOS EOS;
 if (ATmatch(term, "ReadStmt_F(<term>,<term>,<term>,<term>)", &Label.term, &Format.term, &InputItemList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_Format(Format.term, &Format)) {
         // MATCHED Format
      } else return ATfalse;

   if (ATmatch(InputItemList.term, "Some(<term>)", &InputItemList.term)) {
   if (ATmatch(InputItemList.term, "(<term>)", &InputItemList.term)) {
      if (ofp_traverse_InputItemList(InputItemList.term, &InputItemList)) {
         // MATCHED InputItemList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED ReadStmt_F

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::IoControlSpecList IoControlSpecList;
 OFP::InputItemList InputItemList1;
 OFP::EOS EOS1;
 if (ATmatch(term, "ReadStmt_ICSL(<term>,<term>,<term>,<term>)", &Label1.term, &IoControlSpecList.term, &InputItemList1.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_IoControlSpecList(IoControlSpecList.term, &IoControlSpecList)) {
         // MATCHED IoControlSpecList
      } else return ATfalse;

   if (ATmatch(InputItemList1.term, "Some(<term>)", &InputItemList1.term)) {
      if (ofp_traverse_InputItemList(InputItemList1.term, &InputItemList1)) {
         // MATCHED InputItemList
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED ReadStmt_ICSL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R911 write-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_WriteStmt(ATerm term, OFP::WriteStmt* WriteStmt)
{
#ifdef DEBUG_PRINT
   printf("WriteStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::IoControlSpecList IoControlSpecList;
 OFP::OutputItemList OutputItemList;
 OFP::EOS EOS;
 if (ATmatch(term, "WriteStmt(<term>,<term>,<term>,<term>)", &Label.term, &IoControlSpecList.term, &OutputItemList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_IoControlSpecList(IoControlSpecList.term, &IoControlSpecList)) {
         // MATCHED IoControlSpecList
      } else return ATfalse;

   if (ATmatch(OutputItemList.term, "Some(<term>)", &OutputItemList.term)) {
      if (ofp_traverse_OutputItemList(OutputItemList.term, &OutputItemList)) {
         // MATCHED OutputItemList
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R912 print-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PrintStmt(ATerm term, OFP::PrintStmt* PrintStmt)
{
#ifdef DEBUG_PRINT
   printf("PrintStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Format Format;
 OFP::OutputItemList OutputItemList;
 OFP::EOS EOS;
 if (ATmatch(term, "PrintStmt(<term>,<term>,<term>,<term>)", &Label.term, &Format.term, &OutputItemList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_Format(Format.term, &Format)) {
         // MATCHED Format
      } else return ATfalse;

   if (ATmatch(OutputItemList.term, "Some(<term>)", &OutputItemList.term)) {
   if (ATmatch(OutputItemList.term, "(<term>)", &OutputItemList.term)) {
      if (ofp_traverse_OutputItemList(OutputItemList.term, &OutputItemList)) {
         // MATCHED OutputItemList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R913 io-control-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IoControlSpec(ATerm term, OFP::IoControlSpec* IoControlSpec)
{
#ifdef DEBUG_PRINT
   printf("IoControlSpec: %s\n", ATwriteToString(term));
#endif

 OFP::IntVariable IntVariable;
 if (ATmatch(term, "IoControlSpec_SIZE(<term>)", &IntVariable.term)) {

      if (ofp_traverse_IntVariable(IntVariable.term, &IntVariable)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED IoControlSpec_SIZE

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr;
 if (ATmatch(term, "IoControlSpec_SIGN(<term>)", &DefaultCharExpr.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr.term, &DefaultCharExpr)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED IoControlSpec_SIGN

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr1;
 if (ATmatch(term, "IoControlSpec_ROUND(<term>)", &DefaultCharExpr1.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr1.term, &DefaultCharExpr1)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED IoControlSpec_ROUND

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "IoControlSpec_REC(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED IoControlSpec_REC

   return ATtrue;
 }

 OFP::Expr Expr1;
 if (ATmatch(term, "IoControlSpec_POS(<term>)", &Expr1.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED IoControlSpec_POS

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr2;
 if (ATmatch(term, "IoControlSpec_PAD(<term>)", &DefaultCharExpr2.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr2.term, &DefaultCharExpr2)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED IoControlSpec_PAD

   return ATtrue;
 }

 OFP::IntVariable IntVariable1;
 if (ATmatch(term, "IoControlSpec_IOSTAT(<term>)", &IntVariable1.term)) {

      if (ofp_traverse_IntVariable(IntVariable1.term, &IntVariable1)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED IoControlSpec_IOSTAT

   return ATtrue;
 }

 OFP::IomsgVariable IomsgVariable;
 if (ATmatch(term, "IoControlSpec_IOMSG(<term>)", &IomsgVariable.term)) {

      if (ofp_traverse_IomsgVariable(IomsgVariable.term, &IomsgVariable)) {
         // MATCHED IomsgVariable
      } else return ATfalse;

   // MATCHED IoControlSpec_IOMSG

   return ATtrue;
 }

 OFP::IdVariable IdVariable;
 if (ATmatch(term, "IoControlSpec_ID(<term>)", &IdVariable.term)) {

      if (ofp_traverse_IdVariable(IdVariable.term, &IdVariable)) {
         // MATCHED IdVariable
      } else return ATfalse;

   // MATCHED IoControlSpec_ID

   return ATtrue;
 }

 OFP::LblRef LblRef;
 if (ATmatch(term, "IoControlSpec_ERR(<term>)", &LblRef.term)) {

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED IoControlSpec_ERR

   return ATtrue;
 }

 OFP::LblRef LblRef1;
 if (ATmatch(term, "IoControlSpec_EOR(<term>)", &LblRef1.term)) {

      if (ofp_traverse_LblRef(LblRef1.term, &LblRef1)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED IoControlSpec_EOR

   return ATtrue;
 }

 OFP::LblRef LblRef2;
 if (ATmatch(term, "IoControlSpec_END(<term>)", &LblRef2.term)) {

      if (ofp_traverse_LblRef(LblRef2.term, &LblRef2)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED IoControlSpec_END

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr3;
 if (ATmatch(term, "IoControlSpec_DELIM(<term>)", &DefaultCharExpr3.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr3.term, &DefaultCharExpr3)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED IoControlSpec_DELIM

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr4;
 if (ATmatch(term, "IoControlSpec_DECIMAL(<term>)", &DefaultCharExpr4.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr4.term, &DefaultCharExpr4)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED IoControlSpec_DECIMAL

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr5;
 if (ATmatch(term, "IoControlSpec_BLANK(<term>)", &DefaultCharExpr5.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr5.term, &DefaultCharExpr5)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED IoControlSpec_BLANK

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr6;
 if (ATmatch(term, "IoControlSpec_ASYNC(<term>)", &DefaultCharExpr6.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr6.term, &DefaultCharExpr6)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED IoControlSpec_ASYNC

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr7;
 if (ATmatch(term, "IoControlSpec_ADVANCE(<term>)", &DefaultCharExpr7.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr7.term, &DefaultCharExpr7)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED IoControlSpec_ADVANCE

   return ATtrue;
 }

 OFP::Name NamelistGroupName;
 if (ATmatch(term, "IoControlSpec_NML(<term>)", &NamelistGroupName.term)) {

      if (ofp_traverse_Name(NamelistGroupName.term, &NamelistGroupName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED IoControlSpec_NML

   return ATtrue;
 }

 OFP::Format Format;
 if (ATmatch(term, "IoControlSpec_FMT(<term>)", &Format.term)) {

      if (ofp_traverse_Format(Format.term, &Format)) {
         // MATCHED Format
      } else return ATfalse;

   // MATCHED IoControlSpec_FMT

   return ATtrue;
 }

 OFP::IoUnit IoUnit;
 if (ATmatch(term, "IoControlSpec_UNIT(<term>)", &IoUnit.term)) {

      if (ofp_traverse_IoUnit(IoUnit.term, &IoUnit)) {
         // MATCHED IoUnit
      } else return ATfalse;

   // MATCHED IoControlSpec_UNIT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_IoControlSpecList(ATerm term, OFP::IoControlSpecList* IoControlSpecList)
{
#ifdef DEBUG_PRINT
   printf("IoControlSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::IoControlSpec IoControlSpec;
 if (ATmatch(term, "IoControlSpecList(<term>)", &IoControlSpec.term)) {

   ATermList IoControlSpec_tail = (ATermList) ATmake("<term>", IoControlSpec.term);
   while (! ATisEmpty(IoControlSpec_tail)) {
      IoControlSpec.term = ATgetFirst(IoControlSpec_tail);
      IoControlSpec_tail = ATgetNext (IoControlSpec_tail);
      if (ofp_traverse_IoControlSpec(IoControlSpec.term, &IoControlSpec)) {
         // MATCHED IoControlSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R914 id-variable
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IdVariable(ATerm term, OFP::IdVariable* IdVariable)
{
#ifdef DEBUG_PRINT
   printf("IdVariable: %s\n", ATwriteToString(term));
#endif

 OFP::IntVariable IntVariable;
 if (ATmatch(term, "IdVariable(<term>)", &IntVariable.term)) {

      if (ofp_traverse_IntVariable(IntVariable.term, &IntVariable)) {
         // MATCHED IntVariable
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R915 format
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Format(ATerm term, OFP::Format* Format)
{
#ifdef DEBUG_PRINT
   printf("Format: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "Format_STAR")) {

   // MATCHED Format_STAR

   return ATtrue;
 }

 OFP::Label Label;
 if (ATmatch(term, "Format_L(<term>)", &Label.term)) {

      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;

   // MATCHED Format_L

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr;
 if (ATmatch(term, "Format_DCE(<term>)", &DefaultCharExpr.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr.term, &DefaultCharExpr)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED Format_DCE

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R916 input-item
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InputItem(ATerm term, OFP::InputItem* InputItem)
{
#ifdef DEBUG_PRINT
   printf("InputItem: %s\n", ATwriteToString(term));
#endif

 OFP::IoImpliedDo IoImpliedDo;
 if (ATmatch(term, "InputItem_IID(<term>)", &IoImpliedDo.term)) {

      if (ofp_traverse_IoImpliedDo(IoImpliedDo.term, &IoImpliedDo)) {
         // MATCHED IoImpliedDo
      } else return ATfalse;

   // MATCHED InputItem_IID

   return ATtrue;
 }

 OFP::Variable Variable;
 if (ATmatch(term, "InputItem_V(<term>)", &Variable.term)) {

      if (ofp_traverse_Variable(Variable.term, &Variable)) {
         // MATCHED Variable
      } else return ATfalse;

   // MATCHED InputItem_V

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_InputItemList(ATerm term, OFP::InputItemList* InputItemList)
{
#ifdef DEBUG_PRINT
   printf("InputItemList: %s\n", ATwriteToString(term));
#endif

 OFP::InputItem InputItem;
 if (ATmatch(term, "InputItemList(<term>)", &InputItem.term)) {

   ATermList InputItem_tail = (ATermList) ATmake("<term>", InputItem.term);
   while (! ATisEmpty(InputItem_tail)) {
      InputItem.term = ATgetFirst(InputItem_tail);
      InputItem_tail = ATgetNext (InputItem_tail);
      if (ofp_traverse_InputItem(InputItem.term, &InputItem)) {
         // MATCHED InputItem
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R917 output-item
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_OutputItem(ATerm term, OFP::OutputItem* OutputItem)
{
#ifdef DEBUG_PRINT
   printf("OutputItem: %s\n", ATwriteToString(term));
#endif

 OFP::IoImpliedDo IoImpliedDo;
 if (ATmatch(term, "OutputItem_IID(<term>)", &IoImpliedDo.term)) {

      if (ofp_traverse_IoImpliedDo(IoImpliedDo.term, &IoImpliedDo)) {
         // MATCHED IoImpliedDo
      } else return ATfalse;

   // MATCHED OutputItem_IID

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "OutputItem_E(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED OutputItem_E

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_OutputItemList(ATerm term, OFP::OutputItemList* OutputItemList)
{
#ifdef DEBUG_PRINT
   printf("OutputItemList: %s\n", ATwriteToString(term));
#endif

 OFP::OutputItem OutputItem;
 if (ATmatch(term, "OutputItemList(<term>)", &OutputItem.term)) {

   ATermList OutputItem_tail = (ATermList) ATmake("<term>", OutputItem.term);
   while (! ATisEmpty(OutputItem_tail)) {
      OutputItem.term = ATgetFirst(OutputItem_tail);
      OutputItem_tail = ATgetNext (OutputItem_tail);
      if (ofp_traverse_OutputItem(OutputItem.term, &OutputItem)) {
         // MATCHED OutputItem
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R918 io-implied-do
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IoImpliedDo(ATerm term, OFP::IoImpliedDo* IoImpliedDo)
{
#ifdef DEBUG_PRINT
   printf("IoImpliedDo: %s\n", ATwriteToString(term));
#endif

 OFP::IoImpliedDoObjectList IoImpliedDoObjectList;
 OFP::IoImpliedDoControl IoImpliedDoControl;
 if (ATmatch(term, "IoImpliedDo(<term>,<term>)", &IoImpliedDoObjectList.term, &IoImpliedDoControl.term)) {

      if (ofp_traverse_IoImpliedDoObjectList(IoImpliedDoObjectList.term, &IoImpliedDoObjectList)) {
         // MATCHED IoImpliedDoObjectList
      } else return ATfalse;

      if (ofp_traverse_IoImpliedDoControl(IoImpliedDoControl.term, &IoImpliedDoControl)) {
         // MATCHED IoImpliedDoControl
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R919 io-implied-do-object
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IoImpliedDoObject(ATerm term, OFP::IoImpliedDoObject* IoImpliedDoObject)
{
#ifdef DEBUG_PRINT
   printf("IoImpliedDoObject: %s\n", ATwriteToString(term));
#endif

 OFP::OutputItem OutputItem;
 if (ATmatch(term, "IoImpliedDoObject_OI(<term>)", &OutputItem.term)) {

      if (ofp_traverse_OutputItem(OutputItem.term, &OutputItem)) {
         // MATCHED OutputItem
      } else return ATfalse;

   // MATCHED IoImpliedDoObject_OI

   return ATtrue;
 }

 OFP::InputItem InputItem;
 if (ATmatch(term, "IoImpliedDoObject_II(<term>)", &InputItem.term)) {

      if (ofp_traverse_InputItem(InputItem.term, &InputItem)) {
         // MATCHED InputItem
      } else return ATfalse;

   // MATCHED IoImpliedDoObject_II

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_IoImpliedDoObjectList(ATerm term, OFP::IoImpliedDoObjectList* IoImpliedDoObjectList)
{
#ifdef DEBUG_PRINT
   printf("IoImpliedDoObjectList: %s\n", ATwriteToString(term));
#endif

 OFP::IoImpliedDoObject IoImpliedDoObject;
 if (ATmatch(term, "IoImpliedDoObjectList(<term>)", &IoImpliedDoObject.term)) {

   ATermList IoImpliedDoObject_tail = (ATermList) ATmake("<term>", IoImpliedDoObject.term);
   while (! ATisEmpty(IoImpliedDoObject_tail)) {
      IoImpliedDoObject.term = ATgetFirst(IoImpliedDoObject_tail);
      IoImpliedDoObject_tail = ATgetNext (IoImpliedDoObject_tail);
      if (ofp_traverse_IoImpliedDoObject(IoImpliedDoObject.term, &IoImpliedDoObject)) {
         // MATCHED IoImpliedDoObject
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R920 io-implied-do-control
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IoImpliedDoControl(ATerm term, OFP::IoImpliedDoControl* IoImpliedDoControl)
{
#ifdef DEBUG_PRINT
   printf("IoImpliedDoControl: %s\n", ATwriteToString(term));
#endif

 OFP::DoVariable DoVariable;
 OFP::IntExpr IntExpr;
 OFP::IntExpr IntExpr1;
 OFP::IntExpr IntExpr2;
 if (ATmatch(term, "IoImpliedDoControl(<term>,<term>,<term>,<term>)", &DoVariable.term, &IntExpr.term, &IntExpr1.term, &IntExpr2.term)) {

      if (ofp_traverse_DoVariable(DoVariable.term, &DoVariable)) {
         // MATCHED DoVariable
      } else return ATfalse;

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

      if (ofp_traverse_IntExpr(IntExpr1.term, &IntExpr1)) {
         // MATCHED IntExpr
      } else return ATfalse;

   if (ATmatch(IntExpr2.term, "Some(<term>)", &IntExpr2.term)) {
   if (ATmatch(IntExpr2.term, "(<term>)", &IntExpr2.term)) {
      if (ofp_traverse_IntExpr(IntExpr2.term, &IntExpr2)) {
         // MATCHED IntExpr
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R922 wait-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_WaitStmt(ATerm term, OFP::WaitStmt* WaitStmt)
{
#ifdef DEBUG_PRINT
   printf("WaitStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::WaitSpecList WaitSpecList;
 OFP::EOS EOS;
 if (ATmatch(term, "WaitStmt(<term>,<term>,<term>)", &Label.term, &WaitSpecList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_WaitSpecList(WaitSpecList.term, &WaitSpecList)) {
         // MATCHED WaitSpecList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R923 wait-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_WaitSpec(ATerm term, OFP::WaitSpec* WaitSpec)
{
#ifdef DEBUG_PRINT
   printf("WaitSpec: %s\n", ATwriteToString(term));
#endif

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "WaitSpec_IOSTAT(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   // MATCHED WaitSpec_IOSTAT

   return ATtrue;
 }

 OFP::IomsgVariable IomsgVariable;
 if (ATmatch(term, "WaitSpec_IOMSG(<term>)", &IomsgVariable.term)) {

      if (ofp_traverse_IomsgVariable(IomsgVariable.term, &IomsgVariable)) {
         // MATCHED IomsgVariable
      } else return ATfalse;

   // MATCHED WaitSpec_IOMSG

   return ATtrue;
 }

 OFP::IntExpr IntExpr1;
 if (ATmatch(term, "WaitSpec_ID(<term>)", &IntExpr1.term)) {

      if (ofp_traverse_IntExpr(IntExpr1.term, &IntExpr1)) {
         // MATCHED IntExpr
      } else return ATfalse;

   // MATCHED WaitSpec_ID

   return ATtrue;
 }

 OFP::LblRef LblRef;
 if (ATmatch(term, "WaitSpec_ERR(<term>)", &LblRef.term)) {

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED WaitSpec_ERR

   return ATtrue;
 }

 OFP::LblRef LblRef1;
 if (ATmatch(term, "WaitSpec_EOR(<term>)", &LblRef1.term)) {

      if (ofp_traverse_LblRef(LblRef1.term, &LblRef1)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED WaitSpec_EOR

   return ATtrue;
 }

 OFP::LblRef LblRef2;
 if (ATmatch(term, "WaitSpec_END(<term>)", &LblRef2.term)) {

      if (ofp_traverse_LblRef(LblRef2.term, &LblRef2)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED WaitSpec_END

   return ATtrue;
 }

 OFP::FileUnitNumber FileUnitNumber;
 if (ATmatch(term, "WaitSpec_UNIT(<term>)", &FileUnitNumber.term)) {

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

   // MATCHED WaitSpec_UNIT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_WaitSpecList(ATerm term, OFP::WaitSpecList* WaitSpecList)
{
#ifdef DEBUG_PRINT
   printf("WaitSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::WaitSpec WaitSpec;
 if (ATmatch(term, "WaitSpecList(<term>)", &WaitSpec.term)) {

   ATermList WaitSpec_tail = (ATermList) ATmake("<term>", WaitSpec.term);
   while (! ATisEmpty(WaitSpec_tail)) {
      WaitSpec.term = ATgetFirst(WaitSpec_tail);
      WaitSpec_tail = ATgetNext (WaitSpec_tail);
      if (ofp_traverse_WaitSpec(WaitSpec.term, &WaitSpec)) {
         // MATCHED WaitSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R924 backspace-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BackspaceStmt(ATerm term, OFP::BackspaceStmt* BackspaceStmt)
{
#ifdef DEBUG_PRINT
   printf("BackspaceStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::PositionSpecList PositionSpecList;
 OFP::EOS EOS;
 if (ATmatch(term, "BackspaceStmt_PSL(<term>,<term>,<term>)", &Label.term, &PositionSpecList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_PositionSpecList(PositionSpecList.term, &PositionSpecList)) {
         // MATCHED PositionSpecList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED BackspaceStmt_PSL

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::FileUnitNumber FileUnitNumber;
 OFP::EOS EOS1;
 if (ATmatch(term, "BackspaceStmt_FUN(<term>,<term>,<term>)", &Label1.term, &FileUnitNumber.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED BackspaceStmt_FUN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R925 endfile-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndfileStmt(ATerm term, OFP::EndfileStmt* EndfileStmt)
{
#ifdef DEBUG_PRINT
   printf("EndfileStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::PositionSpecList PositionSpecList;
 OFP::EOS EOS;
 if (ATmatch(term, "EndfileStmt_PSL(<term>,<term>,<term>)", &Label.term, &PositionSpecList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_PositionSpecList(PositionSpecList.term, &PositionSpecList)) {
         // MATCHED PositionSpecList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED EndfileStmt_PSL

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::FileUnitNumber FileUnitNumber;
 OFP::EOS EOS1;
 if (ATmatch(term, "EndfileStmt_FUN(<term>,<term>,<term>)", &Label1.term, &FileUnitNumber.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED EndfileStmt_FUN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R926 endfile-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_RewindStmt(ATerm term, OFP::RewindStmt* RewindStmt)
{
#ifdef DEBUG_PRINT
   printf("RewindStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::PositionSpecList PositionSpecList;
 OFP::EOS EOS;
 if (ATmatch(term, "RewindStmt_PSL(<term>,<term>,<term>)", &Label.term, &PositionSpecList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_PositionSpecList(PositionSpecList.term, &PositionSpecList)) {
         // MATCHED PositionSpecList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED RewindStmt_PSL

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::FileUnitNumber FileUnitNumber;
 OFP::EOS EOS1;
 if (ATmatch(term, "RewindStmt_FUN(<term>,<term>,<term>)", &Label1.term, &FileUnitNumber.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED RewindStmt_FUN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R927 position-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PositionSpec(ATerm term, OFP::PositionSpec* PositionSpec)
{
#ifdef DEBUG_PRINT
   printf("PositionSpec: %s\n", ATwriteToString(term));
#endif

 OFP::LblRef LblRef;
 if (ATmatch(term, "PositionSpec_ERR(<term>)", &LblRef.term)) {

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED PositionSpec_ERR

   return ATtrue;
 }

 OFP::IomsgVariable IomsgVariable;
 if (ATmatch(term, "PositionSpec_IOMSG(<term>)", &IomsgVariable.term)) {

      if (ofp_traverse_IomsgVariable(IomsgVariable.term, &IomsgVariable)) {
         // MATCHED IomsgVariable
      } else return ATfalse;

   // MATCHED PositionSpec_IOMSG

   return ATtrue;
 }

 OFP::IntVariable IntVariable;
 if (ATmatch(term, "PositionSpec_IOSTAT(<term>)", &IntVariable.term)) {

      if (ofp_traverse_IntVariable(IntVariable.term, &IntVariable)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED PositionSpec_IOSTAT

   return ATtrue;
 }

 OFP::FileUnitNumber FileUnitNumber;
 if (ATmatch(term, "PositionSpec_UNIT(<term>)", &FileUnitNumber.term)) {

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

   // MATCHED PositionSpec_UNIT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_PositionSpecList(ATerm term, OFP::PositionSpecList* PositionSpecList)
{
#ifdef DEBUG_PRINT
   printf("PositionSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::PositionSpec PositionSpec;
 if (ATmatch(term, "PositionSpecList(<term>)", &PositionSpec.term)) {

   ATermList PositionSpec_tail = (ATermList) ATmake("<term>", PositionSpec.term);
   while (! ATisEmpty(PositionSpec_tail)) {
      PositionSpec.term = ATgetFirst(PositionSpec_tail);
      PositionSpec_tail = ATgetNext (PositionSpec_tail);
      if (ofp_traverse_PositionSpec(PositionSpec.term, &PositionSpec)) {
         // MATCHED PositionSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R928 flush-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FlushStmt(ATerm term, OFP::FlushStmt* FlushStmt)
{
#ifdef DEBUG_PRINT
   printf("FlushStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::FlushSpecList FlushSpecList;
 OFP::EOS EOS;
 if (ATmatch(term, "FlushStmt_FSL(<term>,<term>,<term>)", &Label.term, &FlushSpecList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_FlushSpecList(FlushSpecList.term, &FlushSpecList)) {
         // MATCHED FlushSpecList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED FlushStmt_FSL

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::FileUnitNumber FileUnitNumber;
 OFP::EOS EOS1;
 if (ATmatch(term, "FlushStmt_FUN(<term>,<term>,<term>)", &Label1.term, &FileUnitNumber.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED FlushStmt_FUN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R929 flush-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FlushSpec(ATerm term, OFP::FlushSpec* FlushSpec)
{
#ifdef DEBUG_PRINT
   printf("FlushSpec: %s\n", ATwriteToString(term));
#endif

 OFP::LblRef LblRef;
 if (ATmatch(term, "FlushSpec_ERR(<term>)", &LblRef.term)) {

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED FlushSpec_ERR

   return ATtrue;
 }

 OFP::IomsgVariable IomsgVariable;
 if (ATmatch(term, "FlushSpec_IOMSG(<term>)", &IomsgVariable.term)) {

      if (ofp_traverse_IomsgVariable(IomsgVariable.term, &IomsgVariable)) {
         // MATCHED IomsgVariable
      } else return ATfalse;

   // MATCHED FlushSpec_IOMSG

   return ATtrue;
 }

 OFP::IntVariable IntVariable;
 if (ATmatch(term, "FlushSpec_IOSTAT(<term>)", &IntVariable.term)) {

      if (ofp_traverse_IntVariable(IntVariable.term, &IntVariable)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED FlushSpec_IOSTAT

   return ATtrue;
 }

 OFP::FileUnitNumber FileUnitNumber;
 if (ATmatch(term, "FlushSpec_UNIT(<term>)", &FileUnitNumber.term)) {

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

   // MATCHED FlushSpec_UNIT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_FlushSpecList(ATerm term, OFP::FlushSpecList* FlushSpecList)
{
#ifdef DEBUG_PRINT
   printf("FlushSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::FlushSpec FlushSpec;
 if (ATmatch(term, "FlushSpecList(<term>)", &FlushSpec.term)) {

   ATermList FlushSpec_tail = (ATermList) ATmake("<term>", FlushSpec.term);
   while (! ATisEmpty(FlushSpec_tail)) {
      FlushSpec.term = ATgetFirst(FlushSpec_tail);
      FlushSpec_tail = ATgetNext (FlushSpec_tail);
      if (ofp_traverse_FlushSpec(FlushSpec.term, &FlushSpec)) {
         // MATCHED FlushSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R930 inquire-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InquireStmt(ATerm term, OFP::InquireStmt* InquireStmt)
{
#ifdef DEBUG_PRINT
   printf("InquireStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::IntVariable IntVariable;
 OFP::OutputItemList OutputItemList;
 OFP::EOS EOS;
 if (ATmatch(term, "InquireStmt_IV(<term>,<term>,<term>,<term>)", &Label.term, &IntVariable.term, &OutputItemList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_IntVariable(IntVariable.term, &IntVariable)) {
         // MATCHED IntVariable
      } else return ATfalse;

      if (ofp_traverse_OutputItemList(OutputItemList.term, &OutputItemList)) {
         // MATCHED OutputItemList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED InquireStmt_IV

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::InquireSpecList InquireSpecList;
 OFP::EOS EOS1;
 if (ATmatch(term, "InquireStmt_ISL(<term>,<term>,<term>)", &Label1.term, &InquireSpecList.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_InquireSpecList(InquireSpecList.term, &InquireSpecList)) {
         // MATCHED InquireSpecList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED InquireStmt_ISL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R931 inquire-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InquireSpec(ATerm term, OFP::InquireSpec* InquireSpec)
{
#ifdef DEBUG_PRINT
   printf("InquireSpec: %s\n", ATwriteToString(term));
#endif

 OFP::DefaultCharExpr DefaultCharExpr;
 if (ATmatch(term, "InquireSpec_WRITE(<term>)", &DefaultCharExpr.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr.term, &DefaultCharExpr)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_WRITE

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr1;
 if (ATmatch(term, "InquireSpec_UNFORMATTED(<term>)", &DefaultCharExpr1.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr1.term, &DefaultCharExpr1)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_UNFORMATTED

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr2;
 if (ATmatch(term, "InquireSpec_STREAM(<term>)", &DefaultCharExpr2.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr2.term, &DefaultCharExpr2)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_STREAM

   return ATtrue;
 }

 OFP::IntVariable IntVariable;
 if (ATmatch(term, "InquireSpec_SIZE(<term>)", &IntVariable.term)) {

      if (ofp_traverse_IntVariable(IntVariable.term, &IntVariable)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED InquireSpec_SIZE

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr3;
 if (ATmatch(term, "InquireSpec_SIGN(<term>)", &DefaultCharExpr3.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr3.term, &DefaultCharExpr3)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_SIGN

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr4;
 if (ATmatch(term, "InquireSpec_SEQUENTIAL(<term>)", &DefaultCharExpr4.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr4.term, &DefaultCharExpr4)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_SEQUENTIAL

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr5;
 if (ATmatch(term, "InquireSpec_ROUND(<term>)", &DefaultCharExpr5.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr5.term, &DefaultCharExpr5)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_ROUND

   return ATtrue;
 }

 OFP::IntVariable IntVariable1;
 if (ATmatch(term, "InquireSpec_RECL(<term>)", &IntVariable1.term)) {

      if (ofp_traverse_IntVariable(IntVariable1.term, &IntVariable1)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED InquireSpec_RECL

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr6;
 if (ATmatch(term, "InquireSpec_READWRITE(<term>)", &DefaultCharExpr6.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr6.term, &DefaultCharExpr6)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_READWRITE

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr7;
 if (ATmatch(term, "InquireSpec_READ(<term>)", &DefaultCharExpr7.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr7.term, &DefaultCharExpr7)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_READ

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr8;
 if (ATmatch(term, "InquireSpec_POSITION(<term>)", &DefaultCharExpr8.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr8.term, &DefaultCharExpr8)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_POSITION

   return ATtrue;
 }

 OFP::IntVariable IntVariable2;
 if (ATmatch(term, "InquireSpec_POS(<term>)", &IntVariable2.term)) {

      if (ofp_traverse_IntVariable(IntVariable2.term, &IntVariable2)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED InquireSpec_POS

   return ATtrue;
 }

 OFP::LogicalVariable LogicalVariable;
 if (ATmatch(term, "InquireSpec_PENDING(<term>)", &LogicalVariable.term)) {

      if (ofp_traverse_LogicalVariable(LogicalVariable.term, &LogicalVariable)) {
         // MATCHED LogicalVariable
      } else return ATfalse;

   // MATCHED InquireSpec_PENDING

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr9;
 if (ATmatch(term, "InquireSpec_PAD(<term>)", &DefaultCharExpr9.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr9.term, &DefaultCharExpr9)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_PAD

   return ATtrue;
 }

 OFP::LogicalVariable LogicalVariable1;
 if (ATmatch(term, "InquireSpec_OPENED(<term>)", &LogicalVariable1.term)) {

      if (ofp_traverse_LogicalVariable(LogicalVariable1.term, &LogicalVariable1)) {
         // MATCHED LogicalVariable
      } else return ATfalse;

   // MATCHED InquireSpec_OPENED

   return ATtrue;
 }

 OFP::IntVariable IntVariable3;
 if (ATmatch(term, "InquireSpec_NUMBER(<term>)", &IntVariable3.term)) {

      if (ofp_traverse_IntVariable(IntVariable3.term, &IntVariable3)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED InquireSpec_NUMBER

   return ATtrue;
 }

 OFP::IntVariable IntVariable4;
 if (ATmatch(term, "InquireSpec_NEXTREC(<term>)", &IntVariable4.term)) {

      if (ofp_traverse_IntVariable(IntVariable4.term, &IntVariable4)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED InquireSpec_NEXTREC

   return ATtrue;
 }

 OFP::LogicalVariable LogicalVariable2;
 if (ATmatch(term, "InquireSpec_NAMED(<term>)", &LogicalVariable2.term)) {

      if (ofp_traverse_LogicalVariable(LogicalVariable2.term, &LogicalVariable2)) {
         // MATCHED LogicalVariable
      } else return ATfalse;

   // MATCHED InquireSpec_NAMED

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr10;
 if (ATmatch(term, "InquireSpec_NAME(<term>)", &DefaultCharExpr10.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr10.term, &DefaultCharExpr10)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_NAME

   return ATtrue;
 }

 OFP::IntVariable IntVariable5;
 if (ATmatch(term, "InquireSpec_IOSTAT(<term>)", &IntVariable5.term)) {

      if (ofp_traverse_IntVariable(IntVariable5.term, &IntVariable5)) {
         // MATCHED IntVariable
      } else return ATfalse;

   // MATCHED InquireSpec_IOSTAT

   return ATtrue;
 }

 OFP::IomsgVariable IomsgVariable;
 if (ATmatch(term, "InquireSpec_IOMSG(<term>)", &IomsgVariable.term)) {

      if (ofp_traverse_IomsgVariable(IomsgVariable.term, &IomsgVariable)) {
         // MATCHED IomsgVariable
      } else return ATfalse;

   // MATCHED InquireSpec_IOMSG

   return ATtrue;
 }

 OFP::IntExpr IntExpr;
 if (ATmatch(term, "InquireSpec_ID(<term>)", &IntExpr.term)) {

      if (ofp_traverse_IntExpr(IntExpr.term, &IntExpr)) {
         // MATCHED IntExpr
      } else return ATfalse;

   // MATCHED InquireSpec_ID

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr11;
 if (ATmatch(term, "InquireSpec_FORMATTED(<term>)", &DefaultCharExpr11.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr11.term, &DefaultCharExpr11)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_FORMATTED

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr12;
 if (ATmatch(term, "InquireSpec_FORM(<term>)", &DefaultCharExpr12.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr12.term, &DefaultCharExpr12)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_FORM

   return ATtrue;
 }

 OFP::LogicalVariable LogicalVariable3;
 if (ATmatch(term, "InquireSpec_EXIST(<term>)", &LogicalVariable3.term)) {

      if (ofp_traverse_LogicalVariable(LogicalVariable3.term, &LogicalVariable3)) {
         // MATCHED LogicalVariable
      } else return ATfalse;

   // MATCHED InquireSpec_EXIST

   return ATtrue;
 }

 OFP::LblRef LblRef;
 if (ATmatch(term, "InquireSpec_ERR(<term>)", &LblRef.term)) {

      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;

   // MATCHED InquireSpec_ERR

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr13;
 if (ATmatch(term, "InquireSpec_ENCODING(<term>)", &DefaultCharExpr13.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr13.term, &DefaultCharExpr13)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_ENCODING

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr14;
 if (ATmatch(term, "InquireSpec_DIRECT(<term>)", &DefaultCharExpr14.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr14.term, &DefaultCharExpr14)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_DIRECT

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr15;
 if (ATmatch(term, "InquireSpec_DELIM(<term>)", &DefaultCharExpr15.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr15.term, &DefaultCharExpr15)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_DELIM

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr16;
 if (ATmatch(term, "InquireSpec_DECIMAL(<term>)", &DefaultCharExpr16.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr16.term, &DefaultCharExpr16)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_DECIMAL

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr17;
 if (ATmatch(term, "InquireSpec_BLANK(<term>)", &DefaultCharExpr17.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr17.term, &DefaultCharExpr17)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_BLANK

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr18;
 if (ATmatch(term, "InquireSpec_ASYNC(<term>)", &DefaultCharExpr18.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr18.term, &DefaultCharExpr18)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_ASYNC

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr19;
 if (ATmatch(term, "InquireSpec_ACTION(<term>)", &DefaultCharExpr19.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr19.term, &DefaultCharExpr19)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_ACTION

   return ATtrue;
 }

 OFP::DefaultCharExpr DefaultCharExpr20;
 if (ATmatch(term, "InquireSpec_ACCESS(<term>)", &DefaultCharExpr20.term)) {

      if (ofp_traverse_DefaultCharExpr(DefaultCharExpr20.term, &DefaultCharExpr20)) {
         // MATCHED DefaultCharExpr
      } else return ATfalse;

   // MATCHED InquireSpec_ACCESS

   return ATtrue;
 }

 OFP::FileNameExpr FileNameExpr;
 if (ATmatch(term, "InquireSpec_FILE(<term>)", &FileNameExpr.term)) {

      if (ofp_traverse_FileNameExpr(FileNameExpr.term, &FileNameExpr)) {
         // MATCHED FileNameExpr
      } else return ATfalse;

   // MATCHED InquireSpec_FILE

   return ATtrue;
 }

 OFP::FileUnitNumber FileUnitNumber;
 if (ATmatch(term, "InquireSpec_UNIT(<term>)", &FileUnitNumber.term)) {

      if (ofp_traverse_FileUnitNumber(FileUnitNumber.term, &FileUnitNumber)) {
         // MATCHED FileUnitNumber
      } else return ATfalse;

   // MATCHED InquireSpec_UNIT

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_InquireSpecList(ATerm term, OFP::InquireSpecList* InquireSpecList)
{
#ifdef DEBUG_PRINT
   printf("InquireSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::InquireSpec InquireSpec;
 if (ATmatch(term, "InquireSpecList(<term>)", &InquireSpec.term)) {

   ATermList InquireSpec_tail = (ATermList) ATmake("<term>", InquireSpec.term);
   while (! ATisEmpty(InquireSpec_tail)) {
      InquireSpec.term = ATgetFirst(InquireSpec_tail);
      InquireSpec_tail = ATgetNext (InquireSpec_tail);
      if (ofp_traverse_InquireSpec(InquireSpec.term, &InquireSpec)) {
         // MATCHED InquireSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1001 format-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FormatStmt(ATerm term, OFP::FormatStmt* FormatStmt)
{
#ifdef DEBUG_PRINT
   printf("FormatStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::FormatSpecification FormatSpecification;
 OFP::EOS EOS;
 if (ATmatch(term, "FormatStmt(<term>,<term>,<term>)", &Label.term, &FormatSpecification.term, &EOS.term)) {

      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;

      if (ofp_traverse_FormatSpecification(FormatSpecification.term, &FormatSpecification)) {
         // MATCHED FormatSpecification
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::FormatSpecification FormatSpecification1;
 OFP::EOS EOS1;
 if (ATmatch(term, "FormatStmt(<term>,<term>,<term>)", &Label1.term, &FormatSpecification1.term, &EOS1.term)) {

      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;

      if (ofp_traverse_FormatSpecification(FormatSpecification1.term, &FormatSpecification1)) {
         // MATCHED FormatSpecification
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1002 format-specification
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FormatSpecification(ATerm term, OFP::FormatSpecification* FormatSpecification)
{
#ifdef DEBUG_PRINT
   printf("FormatSpecification: %s\n", ATwriteToString(term));
#endif

 OFP::FormatItems FormatItems;
 if (ATmatch(term, "FormatSpecification(<term>)", &FormatItems.term)) {

   if (ATmatch(FormatItems.term, "Some(<term>)", &FormatItems.term)) {
      if (ofp_traverse_FormatItems(FormatItems.term, &FormatItems)) {
         // MATCHED FormatItems
      } else return ATfalse;
   }

   return ATtrue;
 }

 OFP::FormatItems FormatItems1;
 OFP::UnlimitedFormatItem UnlimitedFormatItem;
 if (ATmatch(term, "FormatSpecification_UFI(<term>,<term>)", &FormatItems1.term, &UnlimitedFormatItem.term)) {

   if (ATmatch(FormatItems1.term, "Some(<term>)", &FormatItems1.term)) {
   if (ATmatch(FormatItems1.term, "(<term>)", &FormatItems1.term)) {
      if (ofp_traverse_FormatItems(FormatItems1.term, &FormatItems1)) {
         // MATCHED FormatItems
      } else return ATfalse;
   }
   }

      if (ofp_traverse_UnlimitedFormatItem(UnlimitedFormatItem.term, &UnlimitedFormatItem)) {
         // MATCHED UnlimitedFormatItem
      } else return ATfalse;

   // MATCHED FormatSpecification_UFI

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1003 format-items
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FormatItems(ATerm term, OFP::FormatItems* FormatItems)
{
#ifdef DEBUG_PRINT
   printf("FormatItems: %s\n", ATwriteToString(term));
#endif

 OFP::FormatItem FormatItem;
 if (ATmatch(term, "FormatItems(<term>)", &FormatItem.term)) {

      if (ofp_traverse_FormatItem(FormatItem.term, &FormatItem)) {
         // MATCHED FormatItem
      } else return ATfalse;

   return ATtrue;
 }

 OFP::FormatItems FormatItems1;
 OFP::FormatItem FormatItem1;
 if (ATmatch(term, "FormatItems_FIs(<term>,<term>)", &FormatItems1.term, &FormatItem1.term)) {

      if (ofp_traverse_FormatItems(FormatItems1.term, &FormatItems1)) {
         // MATCHED FormatItems
      } else return ATfalse;

      if (ofp_traverse_FormatItem(FormatItem1.term, &FormatItem1)) {
         // MATCHED FormatItem
      } else return ATfalse;

   // MATCHED FormatItems_FIs

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1004 format-item
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FormatItem(ATerm term, OFP::FormatItem* FormatItem)
{
#ifdef DEBUG_PRINT
   printf("FormatItem: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 OFP::FormatItems FormatItems;
 if (ATmatch(term, "FormatItem_FIs(<term>,<term>)", &Icon.term, &FormatItems.term)) {

   if (ATmatch(Icon.term, "Some(<term>)", &Icon.term)) {
      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;
   }

      if (ofp_traverse_FormatItems(FormatItems.term, &FormatItems)) {
         // MATCHED FormatItems
      } else return ATfalse;

   // MATCHED FormatItem_FIs

   return ATtrue;
 }

 OFP::Scon Scon;
 if (ATmatch(term, "FormatItem_CSED(<term>)", &Scon.term)) {

      if (ofp_traverse_Scon(Scon.term, &Scon)) {
         // MATCHED Scon
      } else return ATfalse;

   // MATCHED FormatItem_CSED

   return ATtrue;
 }

 OFP::ControlEditDesc ControlEditDesc;
 if (ATmatch(term, "FormatItem_CED(<term>)", &ControlEditDesc.term)) {

      if (ofp_traverse_ControlEditDesc(ControlEditDesc.term, &ControlEditDesc)) {
         // MATCHED ControlEditDesc
      } else return ATfalse;

   // MATCHED FormatItem_CED

   return ATtrue;
 }

 OFP::Icon Icon1;
 OFP::DataEditDesc DataEditDesc;
 if (ATmatch(term, "FormatItem_RED(<term>,<term>)", &Icon1.term, &DataEditDesc.term)) {

   if (ATmatch(Icon1.term, "Some(<term>)", &Icon1.term)) {
      if (ofp_traverse_Icon(Icon1.term, &Icon1)) {
         // MATCHED Icon
      } else return ATfalse;
   }

      if (ofp_traverse_DataEditDesc(DataEditDesc.term, &DataEditDesc)) {
         // MATCHED DataEditDesc
      } else return ATfalse;

   // MATCHED FormatItem_RED

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1005 unlimited-format-item
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_UnlimitedFormatItem(ATerm term, OFP::UnlimitedFormatItem* UnlimitedFormatItem)
{
#ifdef DEBUG_PRINT
   printf("UnlimitedFormatItem: %s\n", ATwriteToString(term));
#endif

 OFP::FormatItems FormatItems;
 if (ATmatch(term, "UnlimitedFormatItem(<term>)", &FormatItems.term)) {

      if (ofp_traverse_FormatItems(FormatItems.term, &FormatItems)) {
         // MATCHED FormatItems
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1006 r
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_REditDesc(ATerm term, OFP::REditDesc* REditDesc)
{
#ifdef DEBUG_PRINT
   printf("REditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "REditDesc(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1007 data-edit-desc
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DataEditDesc(ATerm term, OFP::DataEditDesc* DataEditDesc)
{
#ifdef DEBUG_PRINT
   printf("DataEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Scon Scon;
 OFP::VEditDescList VEditDescList;
 if (ATmatch(term, "DataEditDesc_DT(<term>,<term>)", &Scon.term, &VEditDescList.term)) {

   if (ATmatch(Scon.term, "Some(<term>)", &Scon.term)) {
      if (ofp_traverse_Scon(Scon.term, &Scon)) {
         // MATCHED Scon
      } else return ATfalse;
   }

   if (ATmatch(VEditDescList.term, "Some(<term>)", &VEditDescList.term)) {
   if (ATmatch(VEditDescList.term, "(<term>)", &VEditDescList.term)) {
      if (ofp_traverse_VEditDescList(VEditDescList.term, &VEditDescList)) {
         // MATCHED VEditDescList
      } else return ATfalse;
   }
   }

   // MATCHED DataEditDesc_DT

   return ATtrue;
 }

 OFP::Icon Icon;
 OFP::Icon Icon1;
 if (ATmatch(term, "DataEditDesc_D(<term>,<term>)", &Icon.term, &Icon1.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

      if (ofp_traverse_Icon(Icon1.term, &Icon1)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED DataEditDesc_D

   return ATtrue;
 }

 OFP::Icon Icon2;
 if (ATmatch(term, "DataEditDesc_A(<term>)", &Icon2.term)) {

   if (ATmatch(Icon2.term, "Some(<term>)", &Icon2.term)) {
      if (ofp_traverse_Icon(Icon2.term, &Icon2)) {
         // MATCHED Icon
      } else return ATfalse;
   }

   // MATCHED DataEditDesc_A

   return ATtrue;
 }

 OFP::Icon Icon3;
 if (ATmatch(term, "DataEditDesc_L(<term>)", &Icon3.term)) {

      if (ofp_traverse_Icon(Icon3.term, &Icon3)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED DataEditDesc_L

   return ATtrue;
 }

 OFP::Icon Icon4;
 if (ATmatch(term, "DataEditDesc_G(<term>)", &Icon4.term)) {

      if (ofp_traverse_Icon(Icon4.term, &Icon4)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED DataEditDesc_G

   return ATtrue;
 }

 OFP::Icon Icon5;
 OFP::Icon Icon6;
 OFP::Icon Icon7;
 if (ATmatch(term, "DataEditDesc_GE(<term>,<term>,<term>)", &Icon5.term, &Icon6.term, &Icon7.term)) {

      if (ofp_traverse_Icon(Icon5.term, &Icon5)) {
         // MATCHED Icon
      } else return ATfalse;

      if (ofp_traverse_Icon(Icon6.term, &Icon6)) {
         // MATCHED Icon
      } else return ATfalse;

   if (ATmatch(Icon7.term, "Some(<term>)", &Icon7.term)) {
   if (ATmatch(Icon7.term, "(<term>)", &Icon7.term)) {
      if (ofp_traverse_Icon(Icon7.term, &Icon7)) {
         // MATCHED Icon
      } else return ATfalse;
   }
   }

   // MATCHED DataEditDesc_GE

   return ATtrue;
 }

 OFP::Icon Icon8;
 OFP::Icon Icon9;
 OFP::Icon Icon10;
 if (ATmatch(term, "DataEditDesc_ES(<term>,<term>,<term>)", &Icon8.term, &Icon9.term, &Icon10.term)) {

      if (ofp_traverse_Icon(Icon8.term, &Icon8)) {
         // MATCHED Icon
      } else return ATfalse;

      if (ofp_traverse_Icon(Icon9.term, &Icon9)) {
         // MATCHED Icon
      } else return ATfalse;

   if (ATmatch(Icon10.term, "Some(<term>)", &Icon10.term)) {
   if (ATmatch(Icon10.term, "(<term>)", &Icon10.term)) {
      if (ofp_traverse_Icon(Icon10.term, &Icon10)) {
         // MATCHED Icon
      } else return ATfalse;
   }
   }

   // MATCHED DataEditDesc_ES

   return ATtrue;
 }

 OFP::Icon Icon11;
 OFP::Icon Icon12;
 OFP::Icon Icon13;
 if (ATmatch(term, "DataEditDesc_EN(<term>,<term>,<term>)", &Icon11.term, &Icon12.term, &Icon13.term)) {

      if (ofp_traverse_Icon(Icon11.term, &Icon11)) {
         // MATCHED Icon
      } else return ATfalse;

      if (ofp_traverse_Icon(Icon12.term, &Icon12)) {
         // MATCHED Icon
      } else return ATfalse;

   if (ATmatch(Icon13.term, "Some(<term>)", &Icon13.term)) {
   if (ATmatch(Icon13.term, "(<term>)", &Icon13.term)) {
      if (ofp_traverse_Icon(Icon13.term, &Icon13)) {
         // MATCHED Icon
      } else return ATfalse;
   }
   }

   // MATCHED DataEditDesc_EN

   return ATtrue;
 }

 OFP::Icon Icon14;
 OFP::Icon Icon15;
 OFP::Icon Icon16;
 if (ATmatch(term, "DataEditDesc_E(<term>,<term>,<term>)", &Icon14.term, &Icon15.term, &Icon16.term)) {

      if (ofp_traverse_Icon(Icon14.term, &Icon14)) {
         // MATCHED Icon
      } else return ATfalse;

      if (ofp_traverse_Icon(Icon15.term, &Icon15)) {
         // MATCHED Icon
      } else return ATfalse;

   if (ATmatch(Icon16.term, "Some(<term>)", &Icon16.term)) {
   if (ATmatch(Icon16.term, "(<term>)", &Icon16.term)) {
      if (ofp_traverse_Icon(Icon16.term, &Icon16)) {
         // MATCHED Icon
      } else return ATfalse;
   }
   }

   // MATCHED DataEditDesc_E

   return ATtrue;
 }

 OFP::Icon Icon17;
 OFP::Icon Icon18;
 if (ATmatch(term, "DataEditDesc_F(<term>,<term>)", &Icon17.term, &Icon18.term)) {

      if (ofp_traverse_Icon(Icon17.term, &Icon17)) {
         // MATCHED Icon
      } else return ATfalse;

      if (ofp_traverse_Icon(Icon18.term, &Icon18)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED DataEditDesc_F

   return ATtrue;
 }

 OFP::Icon Icon19;
 OFP::Icon Icon20;
 if (ATmatch(term, "DataEditDesc_Z(<term>,<term>)", &Icon19.term, &Icon20.term)) {

      if (ofp_traverse_Icon(Icon19.term, &Icon19)) {
         // MATCHED Icon
      } else return ATfalse;

   if (ATmatch(Icon20.term, "Some(<term>)", &Icon20.term)) {
   if (ATmatch(Icon20.term, "(<term>)", &Icon20.term)) {
      if (ofp_traverse_Icon(Icon20.term, &Icon20)) {
         // MATCHED Icon
      } else return ATfalse;
   }
   }

   // MATCHED DataEditDesc_Z

   return ATtrue;
 }

 OFP::Icon Icon21;
 OFP::Icon Icon22;
 if (ATmatch(term, "DataEditDesc_O(<term>,<term>)", &Icon21.term, &Icon22.term)) {

      if (ofp_traverse_Icon(Icon21.term, &Icon21)) {
         // MATCHED Icon
      } else return ATfalse;

   if (ATmatch(Icon22.term, "Some(<term>)", &Icon22.term)) {
   if (ATmatch(Icon22.term, "(<term>)", &Icon22.term)) {
      if (ofp_traverse_Icon(Icon22.term, &Icon22)) {
         // MATCHED Icon
      } else return ATfalse;
   }
   }

   // MATCHED DataEditDesc_O

   return ATtrue;
 }

 OFP::Icon Icon23;
 OFP::Icon Icon24;
 if (ATmatch(term, "DataEditDesc_B(<term>,<term>)", &Icon23.term, &Icon24.term)) {

      if (ofp_traverse_Icon(Icon23.term, &Icon23)) {
         // MATCHED Icon
      } else return ATfalse;

   if (ATmatch(Icon24.term, "Some(<term>)", &Icon24.term)) {
   if (ATmatch(Icon24.term, "(<term>)", &Icon24.term)) {
      if (ofp_traverse_Icon(Icon24.term, &Icon24)) {
         // MATCHED Icon
      } else return ATfalse;
   }
   }

   // MATCHED DataEditDesc_B

   return ATtrue;
 }

 OFP::Icon Icon25;
 OFP::Icon Icon26;
 if (ATmatch(term, "DataEditDesc_I(<term>,<term>)", &Icon25.term, &Icon26.term)) {

      if (ofp_traverse_Icon(Icon25.term, &Icon25)) {
         // MATCHED Icon
      } else return ATfalse;

   if (ATmatch(Icon26.term, "Some(<term>)", &Icon26.term)) {
   if (ATmatch(Icon26.term, "(<term>)", &Icon26.term)) {
      if (ofp_traverse_Icon(Icon26.term, &Icon26)) {
         // MATCHED Icon
      } else return ATfalse;
   }
   }

   // MATCHED DataEditDesc_I

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1008 w
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_WEditDesc(ATerm term, OFP::WEditDesc* WEditDesc)
{
#ifdef DEBUG_PRINT
   printf("WEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "WEditDesc(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1009 m
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_MEditDesc(ATerm term, OFP::MEditDesc* MEditDesc)
{
#ifdef DEBUG_PRINT
   printf("MEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "MEditDesc(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1010 d
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DEditDesc(ATerm term, OFP::DEditDesc* DEditDesc)
{
#ifdef DEBUG_PRINT
   printf("DEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "DEditDesc(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1011 e
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EEditDesc(ATerm term, OFP::EEditDesc* EEditDesc)
{
#ifdef DEBUG_PRINT
   printf("EEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "EEditDesc(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1012 v
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_VEditDesc(ATerm term, OFP::VEditDesc* VEditDesc)
{
#ifdef DEBUG_PRINT
   printf("VEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "VEditDesc(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_VEditDescList(ATerm term, OFP::VEditDescList* VEditDescList)
{
#ifdef DEBUG_PRINT
   printf("VEditDescList: %s\n", ATwriteToString(term));
#endif

 OFP::VEditDesc VEditDesc;
 if (ATmatch(term, "VEditDescList(<term>)", &VEditDesc.term)) {

   ATermList VEditDesc_tail = (ATermList) ATmake("<term>", VEditDesc.term);
   while (! ATisEmpty(VEditDesc_tail)) {
      VEditDesc.term = ATgetFirst(VEditDesc_tail);
      VEditDesc_tail = ATgetNext (VEditDesc_tail);
      if (ofp_traverse_VEditDesc(VEditDesc.term, &VEditDesc)) {
         // MATCHED VEditDesc
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1013 control-edit-desc
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ControlEditDesc(ATerm term, OFP::ControlEditDesc* ControlEditDesc)
{
#ifdef DEBUG_PRINT
   printf("ControlEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::DecimalEditDesc DecimalEditDesc;
 if (ATmatch(term, "ControlEditDesc_DED(<term>)", &DecimalEditDesc.term)) {

      if (ofp_traverse_DecimalEditDesc(DecimalEditDesc.term, &DecimalEditDesc)) {
         // MATCHED DecimalEditDesc
      } else return ATfalse;

   // MATCHED ControlEditDesc_DED

   return ATtrue;
 }

 OFP::RoundEditDesc RoundEditDesc;
 if (ATmatch(term, "ControlEditDesc_RED(<term>)", &RoundEditDesc.term)) {

      if (ofp_traverse_RoundEditDesc(RoundEditDesc.term, &RoundEditDesc)) {
         // MATCHED RoundEditDesc
      } else return ATfalse;

   // MATCHED ControlEditDesc_RED

   return ATtrue;
 }

 OFP::BlankInterpEditDesc BlankInterpEditDesc;
 if (ATmatch(term, "ControlEditDesc_BIED(<term>)", &BlankInterpEditDesc.term)) {

      if (ofp_traverse_BlankInterpEditDesc(BlankInterpEditDesc.term, &BlankInterpEditDesc)) {
         // MATCHED BlankInterpEditDesc
      } else return ATfalse;

   // MATCHED ControlEditDesc_BIED

   return ATtrue;
 }

 OFP::SignedIntLiteralConstant SignedIntLiteralConstant;
 if (ATmatch(term, "ControlEditDesc_P(<term>)", &SignedIntLiteralConstant.term)) {

      if (ofp_traverse_SignedIntLiteralConstant(SignedIntLiteralConstant.term, &SignedIntLiteralConstant)) {
         // MATCHED SignedIntLiteralConstant
      } else return ATfalse;

   // MATCHED ControlEditDesc_P

   return ATtrue;
 }

 OFP::SignEditDesc SignEditDesc;
 if (ATmatch(term, "ControlEditDesc_SED(<term>)", &SignEditDesc.term)) {

      if (ofp_traverse_SignEditDesc(SignEditDesc.term, &SignEditDesc)) {
         // MATCHED SignEditDesc
      } else return ATfalse;

   // MATCHED ControlEditDesc_SED

   return ATtrue;
 }

 if (ATmatch(term, "ControlEditDesc_COLON")) {

   // MATCHED ControlEditDesc_COLON

   return ATtrue;
 }

 OFP::Icon Icon;
 if (ATmatch(term, "ControlEditDesc_SLASH(<term>)", &Icon.term)) {

   if (ATmatch(Icon.term, "Some(<term>)", &Icon.term)) {
      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;
   }

   // MATCHED ControlEditDesc_SLASH

   return ATtrue;
 }

 OFP::PositionEditDesc PositionEditDesc;
 if (ATmatch(term, "ControlEditDesc_PED(<term>)", &PositionEditDesc.term)) {

      if (ofp_traverse_PositionEditDesc(PositionEditDesc.term, &PositionEditDesc)) {
         // MATCHED PositionEditDesc
      } else return ATfalse;

   // MATCHED ControlEditDesc_PED

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1014 k
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_KEditDesc(ATerm term, OFP::KEditDesc* KEditDesc)
{
#ifdef DEBUG_PRINT
   printf("KEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::SignedIntLiteralConstant SignedIntLiteralConstant;
 if (ATmatch(term, "KEditDesc(<term>)", &SignedIntLiteralConstant.term)) {

      if (ofp_traverse_SignedIntLiteralConstant(SignedIntLiteralConstant.term, &SignedIntLiteralConstant)) {
         // MATCHED SignedIntLiteralConstant
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1015 position-edit-desc
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PositionEditDesc(ATerm term, OFP::PositionEditDesc* PositionEditDesc)
{
#ifdef DEBUG_PRINT
   printf("PositionEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "PositionEditDesc_X(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED PositionEditDesc_X

   return ATtrue;
 }

 OFP::Icon Icon1;
 if (ATmatch(term, "PositionEditDesc_TR(<term>)", &Icon1.term)) {

      if (ofp_traverse_Icon(Icon1.term, &Icon1)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED PositionEditDesc_TR

   return ATtrue;
 }

 OFP::Icon Icon2;
 if (ATmatch(term, "PositionEditDesc_TL(<term>)", &Icon2.term)) {

      if (ofp_traverse_Icon(Icon2.term, &Icon2)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED PositionEditDesc_TL

   return ATtrue;
 }

 OFP::Icon Icon3;
 if (ATmatch(term, "PositionEditDesc_T(<term>)", &Icon3.term)) {

      if (ofp_traverse_Icon(Icon3.term, &Icon3)) {
         // MATCHED Icon
      } else return ATfalse;

   // MATCHED PositionEditDesc_T

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1016 n
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_NEditDesc(ATerm term, OFP::NEditDesc* NEditDesc)
{
#ifdef DEBUG_PRINT
   printf("NEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Icon Icon;
 if (ATmatch(term, "NEditDesc(<term>)", &Icon.term)) {

      if (ofp_traverse_Icon(Icon.term, &Icon)) {
         // MATCHED Icon
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1017 sign-edit-desc
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SignEditDesc(ATerm term, OFP::SignEditDesc* SignEditDesc)
{
#ifdef DEBUG_PRINT
   printf("SignEditDesc: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "SignEditDesc_S")) {

   // MATCHED SignEditDesc_S

   return ATtrue;
 }

 if (ATmatch(term, "SignEditDesc_SP")) {

   // MATCHED SignEditDesc_SP

   return ATtrue;
 }

 if (ATmatch(term, "SignEditDesc_SS")) {

   // MATCHED SignEditDesc_SS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1018 blank-interp-edit-desc
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BlankInterpEditDesc(ATerm term, OFP::BlankInterpEditDesc* BlankInterpEditDesc)
{
#ifdef DEBUG_PRINT
   printf("BlankInterpEditDesc: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "BlonkInterpEditDesc_BZ")) {

   // MATCHED BlonkInterpEditDesc_BZ

   return ATtrue;
 }

 if (ATmatch(term, "BlonkInterpEditDesc_BN")) {

   // MATCHED BlonkInterpEditDesc_BN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1019 round-edit-desc
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_RoundEditDesc(ATerm term, OFP::RoundEditDesc* RoundEditDesc)
{
#ifdef DEBUG_PRINT
   printf("RoundEditDesc: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "RoundEditDesc_RP")) {

   // MATCHED RoundEditDesc_RP

   return ATtrue;
 }

 if (ATmatch(term, "RoundEditDesc_RC")) {

   // MATCHED RoundEditDesc_RC

   return ATtrue;
 }

 if (ATmatch(term, "RoundEditDesc_RN")) {

   // MATCHED RoundEditDesc_RN

   return ATtrue;
 }

 if (ATmatch(term, "RoundEditDesc_RZ")) {

   // MATCHED RoundEditDesc_RZ

   return ATtrue;
 }

 if (ATmatch(term, "RoundEditDesc_RD")) {

   // MATCHED RoundEditDesc_RD

   return ATtrue;
 }

 if (ATmatch(term, "RoundEditDesc_RU")) {

   // MATCHED RoundEditDesc_RU

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1020 decimal-edit-desc
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DecimalEditDesc(ATerm term, OFP::DecimalEditDesc* DecimalEditDesc)
{
#ifdef DEBUG_PRINT
   printf("DecimalEditDesc: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "DecimalEditDesc_DP")) {

   // MATCHED DecimalEditDesc_DP

   return ATtrue;
 }

 if (ATmatch(term, "DecimalEditDesc_DC")) {

   // MATCHED DecimalEditDesc_DC

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1021 char-string-edit-desc
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CharStringEditDesc(ATerm term, OFP::CharStringEditDesc* CharStringEditDesc)
{
#ifdef DEBUG_PRINT
   printf("CharStringEditDesc: %s\n", ATwriteToString(term));
#endif

 OFP::Scon Scon;
 if (ATmatch(term, "CharStringEditDesc(<term>)", &Scon.term)) {

      if (ofp_traverse_Scon(Scon.term, &Scon)) {
         // MATCHED Scon
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1101 main-program
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_MainProgram(ATerm term, OFP::MainProgram* MainProgram)
{
#ifdef DEBUG_PRINT
   printf("MainProgram(F): %s\n", ATwriteToString(term));
#endif

 OFP::ProgramStmt ProgramStmt;
 OFP::InitialSpecPart InitialSpecPart;
 OFP::SpecAndExecPart SpecAndExecPart;
 OFP::InternalSubprogramPart InternalSubprogramPart;
 OFP::EndProgramStmt EndProgramStmt;
 if (ATmatch(term, "MainProgram(<term>,<term>,<term>,<term>,<term>)", &ProgramStmt.term, &InitialSpecPart.term, &SpecAndExecPart.term, &InternalSubprogramPart.term, &EndProgramStmt.term)) {

    if (ofp_traverse_ProgramStmt(ProgramStmt.term, &ProgramStmt)) {
       // MATCHED ProgramStmt
       MainProgram->setProgramStmt(ProgramStmt.newProgramStmt());
    } // Optional

    if (ofp_traverse_InitialSpecPart(InitialSpecPart.term, &InitialSpecPart)) {
       // MATCHED InitialSpecPart
       MainProgram->setInitialSpecPart(InitialSpecPart.newInitialSpecPart());
    } else return ATfalse;

    if (ofp_traverse_SpecAndExecPart(SpecAndExecPart.term, &SpecAndExecPart)) {
       // MATCHED SpecAndExecPart
       MainProgram->setSpecAndExecPart(SpecAndExecPart.newSpecAndExecPart());
    } else return ATfalse;

    if (ofp_traverse_InternalSubprogramPart(InternalSubprogramPart.term, &InternalSubprogramPart)) {
       // MATCHED InternalSubprogramPart
       MainProgram->setInternalSubprogramPart(InternalSubprogramPart.newInternalSubprogramPart());
    } // Optional

    if (ofp_traverse_EndProgramStmt(EndProgramStmt.term, &EndProgramStmt)) {
       // MATCHED EndProgramStmt
       MainProgram->setEndProgramStmt(EndProgramStmt.newEndProgramStmt());
    } else return ATfalse;

    ast->build_MainProgram(MainProgram);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1102 program-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProgramStmt(ATerm term, OFP::ProgramStmt* ProgramStmt)
{
#ifdef DEBUG_PRINT
   printf("ProgramStmt(F): %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name ProgramName;
 OFP::EOS EOS;
 if (ATmatch(term, "ProgramStmt(<term>,<term>,<term>)", &Label.term, &ProgramName.term, &EOS.term)) {

    if (ofp_traverse_Label(Label.term, &Label)) {
       // MATCHED Label
       ProgramStmt->setLabel(Label.newLabel());
    } // Optional

   if (ofp_traverse_Name(ProgramName.term, &ProgramName)) {
      // MATCHED ProgramName
      ProgramStmt->setProgramName(ProgramName.newName());
   } else return ATfalse;

   if (ofp_traverse_EOS(EOS.term, &EOS)) {
      // MATCHED EOS
      ProgramStmt->setEOS(EOS.newEOS());
   } else return ATfalse;

   ast->build_ProgramStmt(ProgramStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1103 end-program-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndProgramStmt(ATerm term, OFP::EndProgramStmt* EndProgramStmt)
{
#ifdef DEBUG_PRINT
   printf("EndProgramStmt(F): %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name ProgramName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndProgramStmt(<term>,<term>,<term>)", &Label.term, &ProgramName.term, &EOS.term)) {

    if (ofp_traverse_Label(Label.term, &Label)) {
       // MATCHED Label
       EndProgramStmt->setLabel(Label.newLabel());
    } // Optional

    if (ofp_traverse_Name(ProgramName.term, &ProgramName)) {
       // MATCHED ProgramName
       EndProgramStmt->setProgramName(ProgramName.newName());
    } // Optional

    if (ofp_traverse_EOS(EOS.term, &EOS)) {
       // MATCHED EOS
       EndProgramStmt->setEOS(EOS.newEOS());
    } else return ATfalse;

   ast->build_EndProgramStmt(EndProgramStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1104 module
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Module(ATerm term, OFP::Module* Module)
{
#ifdef DEBUG_PRINT
   printf("Module: %s\n", ATwriteToString(term));
#endif

 OFP::ModuleStmt ModuleStmt;
 OFP::SpecificationPart SpecificationPart;
 OFP::ModuleSubprogramPart ModuleSubprogramPart;
 OFP::EndModuleStmt EndModuleStmt;
 if (ATmatch(term, "Module(<term>,<term>,<term>,<term>)", &ModuleStmt.term, &SpecificationPart.term, &ModuleSubprogramPart.term, &EndModuleStmt.term)) {

      if (ofp_traverse_ModuleStmt(ModuleStmt.term, &ModuleStmt)) {
         // MATCHED ModuleStmt
         Module->setModuleStmt(ModuleStmt.newModuleStmt());
      } else return ATfalse;

      if (ofp_traverse_SpecificationPart(SpecificationPart.term, &SpecificationPart)) {
         // MATCHED SpecificationPart
         Module->setSpecificationPart(SpecificationPart.newSpecificationPart());
      } else return ATfalse;

   if (ATmatch(ModuleSubprogramPart.term, "Some(<term>)", &ModuleSubprogramPart.term)) {
      if (ofp_traverse_ModuleSubprogramPart(ModuleSubprogramPart.term, &ModuleSubprogramPart)) {
         // MATCHED ModuleSubprogramPart
         Module->setModuleSubprogramPart(ModuleSubprogramPart.newModuleSubprogramPart());
      } else return ATfalse;
   }

      if (ofp_traverse_EndModuleStmt(EndModuleStmt.term, &EndModuleStmt)) {
         // MATCHED EndModuleStmt
         Module->setEndModuleStmt(EndModuleStmt.newEndModuleStmt());
      } else return ATfalse;

   ast->build_Module(Module);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1105 module-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ModuleStmt(ATerm term, OFP::ModuleStmt* ModuleStmt)
{
#ifdef DEBUG_PRINT
   printf("ModuleStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name ModuleName;
 OFP::EOS EOS;
 if (ATmatch(term, "ModuleStmt(<term>,<term>,<term>)", &Label.term, &ModuleName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         ModuleStmt->setLabel(Label.newLabel());
      } else return ATfalse;
   }

      if (ofp_traverse_Name(ModuleName.term, &ModuleName)) {
         // MATCHED Name
         ModuleStmt->setModuleName(ModuleName.newName());
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         ModuleStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

   ast->build_ModuleStmt(ModuleStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1106 end-module-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndModuleStmt(ATerm term, OFP::EndModuleStmt* EndModuleStmt)
{
#ifdef DEBUG_PRINT
   printf("EndModuleStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name ModuleName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndModuleStmt(<term>,<term>,<term>)", &Label.term, &ModuleName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         EndModuleStmt->setLabel(Label.newLabel());
      } else return ATfalse;
   }

   if (ATmatch(ModuleName.term, "Some(<term>)", &ModuleName.term)) {
   if (ATmatch(ModuleName.term, "(Some(<term>))", &ModuleName.term)) {
      if (ofp_traverse_Name(ModuleName.term, &ModuleName)) {
         // MATCHED Name
         EndModuleStmt->setModuleName(ModuleName.newName());
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         EndModuleStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

   ast->build_EndModuleStmt(EndModuleStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1107 module-subprogram-part
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ModuleSubprogramPart(ATerm term, OFP::ModuleSubprogramPart* ModuleSubprogramPart)
{
#ifdef DEBUG_PRINT
   printf("ModuleSubprogramPart: %s\n", ATwriteToString(term));
#endif

 OFP::ContainsStmt ContainsStmt;
 OFP::ModuleSubprogram ModuleSubprogram;
 if (ATmatch(term, "ModuleSubprogramPart(<term>,<term>)", &ContainsStmt.term, &ModuleSubprogram.term)) {

      if (ofp_traverse_ContainsStmt(ContainsStmt.term, &ContainsStmt)) {
         // MATCHED ContainsStmt
         ModuleSubprogramPart->setContainsStmt(ContainsStmt.newContainsStmt());
      } else return ATfalse;

   ATermList ModuleSubprogram_tail = (ATermList) ATmake("<term>", ModuleSubprogram.term);
   while (! ATisEmpty(ModuleSubprogram_tail)) {
      ModuleSubprogram.term = ATgetFirst(ModuleSubprogram_tail);
      ModuleSubprogram_tail = ATgetNext (ModuleSubprogram_tail);
      if (ofp_traverse_ModuleSubprogram(ModuleSubprogram.term, &ModuleSubprogram)) {
         // MATCHED ModuleSubprogram
         ModuleSubprogramPart->appendModuleSubprogram(ModuleSubprogram.newModuleSubprogram());
      } else return ATfalse;
   }

   ast->build_ModuleSubprogramPart(ModuleSubprogramPart);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1108 module-subprogram
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ModuleSubprogram(ATerm term, OFP::ModuleSubprogram* ModuleSubprogram)
{
#ifdef DEBUG_PRINT
   printf("ModuleSubprogram: %s\n", ATwriteToString(term));
#endif

 OFP::SeparateModuleSubprogram SeparateModuleSubprogram;
 if (ATmatch(term, "ModuleSubprogram_SMS(<term>)", &SeparateModuleSubprogram.term)) {

      if (ofp_traverse_SeparateModuleSubprogram(SeparateModuleSubprogram.term, &SeparateModuleSubprogram)) {
         // MATCHED SeparateModuleSubprogram
         ModuleSubprogram->setSeparateModuleSubprogram(SeparateModuleSubprogram.newSeparateModuleSubprogram());
         ModuleSubprogram->inheritPayload(ModuleSubprogram->getSeparateModuleSubprogram());
      } else return ATfalse;

   // MATCHED ModuleSubprogram_SMS
   ModuleSubprogram->setOptionType(OFP::ModuleSubprogram::ModuleSubprogram_SMS);

   return ATtrue;
 }

 OFP::SubroutineSubprogram SubroutineSubprogram;
 if (ATmatch(term, "ModuleSubprogram_SS(<term>)", &SubroutineSubprogram.term)) {

      if (ofp_traverse_SubroutineSubprogram(SubroutineSubprogram.term, &SubroutineSubprogram)) {
         // MATCHED SubroutineSubprogram
         ModuleSubprogram->setSubroutineSubprogram(SubroutineSubprogram.newSubroutineSubprogram());
         ModuleSubprogram->inheritPayload(ModuleSubprogram->getSubroutineSubprogram());
      } else return ATfalse;

   // MATCHED ModuleSubprogram_SS
   ModuleSubprogram->setOptionType(OFP::ModuleSubprogram::ModuleSubprogram_SS);

   return ATtrue;
 }

 OFP::FunctionSubprogram FunctionSubprogram;
 if (ATmatch(term, "ModuleSubprogram_FS(<term>)", &FunctionSubprogram.term)) {

      if (ofp_traverse_FunctionSubprogram(FunctionSubprogram.term, &FunctionSubprogram)) {
         // MATCHED FunctionSubprogram
         ModuleSubprogram->setFunctionSubprogram(FunctionSubprogram.newFunctionSubprogram());
         ModuleSubprogram->inheritPayload(ModuleSubprogram->getFunctionSubprogram());
      } else return ATfalse;

   // MATCHED ModuleSubprogram_FS
   ModuleSubprogram->setOptionType(OFP::ModuleSubprogram::ModuleSubprogram_FS);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1109 use-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_UseStmt(ATerm term, OFP::UseStmt* UseStmt)
{
#ifdef DEBUG_PRINT
   printf("UseStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ModuleNature ModuleNature;
 OFP::Name ModuleName;
 OFP::OnlyList OnlyList;
 OFP::EOS EOS;
 if (ATmatch(term, "UseStmt_OL(<term>,<term>,<term>,<term>,<term>)", &Label.term, &ModuleNature.term, &ModuleName.term, &OnlyList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(ModuleNature.term, "Some(<term>)", &ModuleNature.term)) {
   if (ATmatch(ModuleNature.term, "(Some(<term>))", &ModuleNature.term)) {
   if (ATmatch(ModuleNature.term, "(<term>)", &ModuleNature.term)) {
      if (ofp_traverse_ModuleNature(ModuleNature.term, &ModuleNature)) {
         // MATCHED ModuleNature
      } else return ATfalse;
   }
   }
   }

      if (ofp_traverse_Name(ModuleName.term, &ModuleName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(OnlyList.term, "Some(<term>)", &OnlyList.term)) {
      if (ofp_traverse_OnlyList(OnlyList.term, &OnlyList)) {
         // MATCHED OnlyList
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED UseStmt_OL

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::ModuleNature ModuleNature1;
 OFP::Name ModuleName1;
 OFP::RenameList RenameList;
 OFP::EOS EOS1;
 if (ATmatch(term, "UseStmt_RL(<term>,<term>,<term>,<term>,<term>)", &Label1.term, &ModuleNature1.term, &ModuleName1.term, &RenameList.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(ModuleNature1.term, "Some(<term>)", &ModuleNature1.term)) {
   if (ATmatch(ModuleNature1.term, "(Some(<term>))", &ModuleNature1.term)) {
   if (ATmatch(ModuleNature1.term, "(<term>)", &ModuleNature1.term)) {
      if (ofp_traverse_ModuleNature(ModuleNature1.term, &ModuleNature1)) {
         // MATCHED ModuleNature
      } else return ATfalse;
   }
   }
   }

      if (ofp_traverse_Name(ModuleName1.term, &ModuleName1)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(RenameList.term, "Some(<term>)", &RenameList.term)) {
   if (ATmatch(RenameList.term, "(<term>)", &RenameList.term)) {
      if (ofp_traverse_RenameList(RenameList.term, &RenameList)) {
         // MATCHED RenameList
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED UseStmt_RL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1110 module-nature
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ModuleNature(ATerm term, OFP::ModuleNature* ModuleNature)
{
#ifdef DEBUG_PRINT
   printf("ModuleNature: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "ModuleNature_NON_INTRINSIC")) {

   // MATCHED ModuleNature_NON_INTRINSIC

   return ATtrue;
 }

 if (ATmatch(term, "ModuleNature_INTRINSIC")) {

   // MATCHED ModuleNature_INTRINSIC

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1111 rename
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Rename(ATerm term, OFP::Rename* Rename)
{
#ifdef DEBUG_PRINT
   printf("Rename: %s\n", ATwriteToString(term));
#endif

 OFP::LocalDefinedOperator LocalDefinedOperator;
 OFP::UseDefinedOperator UseDefinedOperator;
 if (ATmatch(term, "Rename_OP(<term>,<term>)", &LocalDefinedOperator.term, &UseDefinedOperator.term)) {

      if (ofp_traverse_LocalDefinedOperator(LocalDefinedOperator.term, &LocalDefinedOperator)) {
         // MATCHED LocalDefinedOperator
      } else return ATfalse;

      if (ofp_traverse_UseDefinedOperator(UseDefinedOperator.term, &UseDefinedOperator)) {
         // MATCHED UseDefinedOperator
      } else return ATfalse;

   // MATCHED Rename_OP

   return ATtrue;
 }

 OFP::Name LocalName;
 OFP::Name UseName;
 if (ATmatch(term, "Rename_LN(<term>,<term>)", &LocalName.term, &UseName.term)) {

      if (ofp_traverse_Name(LocalName.term, &LocalName)) {
         // MATCHED Name
      } else return ATfalse;

      if (ofp_traverse_Name(UseName.term, &UseName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED Rename_LN

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_RenameList(ATerm term, OFP::RenameList* RenameList)
{
#ifdef DEBUG_PRINT
   printf("RenameList: %s\n", ATwriteToString(term));
#endif

 OFP::Rename Rename;
 if (ATmatch(term, "RenameList(<term>)", &Rename.term)) {

   ATermList Rename_tail = (ATermList) ATmake("<term>", Rename.term);
   while (! ATisEmpty(Rename_tail)) {
      Rename.term = ATgetFirst(Rename_tail);
      Rename_tail = ATgetNext (Rename_tail);
      if (ofp_traverse_Rename(Rename.term, &Rename)) {
         // MATCHED Rename
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1112 only
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Only(ATerm term, OFP::Only* Only)
{
#ifdef DEBUG_PRINT
   printf("Only: %s\n", ATwriteToString(term));
#endif

 OFP::Rename Rename;
 if (ATmatch(term, "Only_R(<term>)", &Rename.term)) {

      if (ofp_traverse_Rename(Rename.term, &Rename)) {
         // MATCHED Rename
      } else return ATfalse;

   // MATCHED Only_R

   return ATtrue;
 }

 OFP::OnlyUseName OnlyUseName;
 if (ATmatch(term, "Only_OUN(<term>)", &OnlyUseName.term)) {

      if (ofp_traverse_OnlyUseName(OnlyUseName.term, &OnlyUseName)) {
         // MATCHED OnlyUseName
      } else return ATfalse;

   // MATCHED Only_OUN

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_OnlyList(ATerm term, OFP::OnlyList* OnlyList)
{
#ifdef DEBUG_PRINT
   printf("OnlyList: %s\n", ATwriteToString(term));
#endif

 OFP::Only Only;
 if (ATmatch(term, "OnlyList(<term>)", &Only.term)) {

   ATermList Only_tail = (ATermList) ATmake("<term>", Only.term);
   while (! ATisEmpty(Only_tail)) {
      Only.term = ATgetFirst(Only_tail);
      Only_tail = ATgetNext (Only_tail);
      if (ofp_traverse_Only(Only.term, &Only)) {
         // MATCHED Only
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1113 only-use-name
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_OnlyUseName(ATerm term, OFP::OnlyUseName* OnlyUseName)
{
#ifdef DEBUG_PRINT
   printf("OnlyUseName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "OnlyUseName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1114 local-defined-operator
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_LocalDefinedOperator(ATerm term, OFP::LocalDefinedOperator* LocalDefinedOperator)
{
#ifdef DEBUG_PRINT
   printf("LocalDefinedOperator: %s\n", ATwriteToString(term));
#endif

 OFP::DefinedBinaryOp DefinedBinaryOp;
 if (ATmatch(term, "LocalDefinedOperator_DBO(<term>)", &DefinedBinaryOp.term)) {

      if (ofp_traverse_DefinedBinaryOp(DefinedBinaryOp.term, &DefinedBinaryOp)) {
         // MATCHED DefinedBinaryOp
      } else return ATfalse;

   // MATCHED LocalDefinedOperator_DBO

   return ATtrue;
 }

 OFP::DefinedUnaryOp DefinedUnaryOp;
 if (ATmatch(term, "LocalDefinedOperator_DUO(<term>)", &DefinedUnaryOp.term)) {

      if (ofp_traverse_DefinedUnaryOp(DefinedUnaryOp.term, &DefinedUnaryOp)) {
         // MATCHED DefinedUnaryOp
      } else return ATfalse;

   // MATCHED LocalDefinedOperator_DUO

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1115 use-defined-operator
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_UseDefinedOperator(ATerm term, OFP::UseDefinedOperator* UseDefinedOperator)
{
#ifdef DEBUG_PRINT
   printf("UseDefinedOperator: %s\n", ATwriteToString(term));
#endif

 OFP::DefinedBinaryOp DefinedBinaryOp;
 if (ATmatch(term, "UseDefinedOperator_DBO(<term>)", &DefinedBinaryOp.term)) {

      if (ofp_traverse_DefinedBinaryOp(DefinedBinaryOp.term, &DefinedBinaryOp)) {
         // MATCHED DefinedBinaryOp
      } else return ATfalse;

   // MATCHED UseDefinedOperator_DBO

   return ATtrue;
 }

 OFP::DefinedUnaryOp DefinedUnaryOp;
 if (ATmatch(term, "UseDefinedOperator_DUO(<term>)", &DefinedUnaryOp.term)) {

      if (ofp_traverse_DefinedUnaryOp(DefinedUnaryOp.term, &DefinedUnaryOp)) {
         // MATCHED DefinedUnaryOp
      } else return ATfalse;

   // MATCHED UseDefinedOperator_DUO

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1116 submodule
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Submodule(ATerm term, OFP::Submodule* Submodule)
{
#ifdef DEBUG_PRINT
   printf("\nSubmodule: %s\n", ATwriteToString(term));
#endif

 OFP::SubmoduleStmt SubmoduleStmt;
 OFP::SpecificationPart SpecificationPart;
 OFP::ModuleSubprogramPart ModuleSubprogramPart;
 OFP::EndSubmoduleStmt EndSubmoduleStmt;
 if (ATmatch(term, "Submodule(<term>,<term>,<term>,<term>)", &SubmoduleStmt.term, &SpecificationPart.term, &ModuleSubprogramPart.term, &EndSubmoduleStmt.term)) {

      if (ofp_traverse_SubmoduleStmt(SubmoduleStmt.term, &SubmoduleStmt)) {
         // MATCHED SubmoduleStmt
      } else return ATfalse;

      if (ofp_traverse_SpecificationPart(SpecificationPart.term, &SpecificationPart)) {
         // MATCHED SpecificationPart
      } else return ATfalse;

   if (ATmatch(ModuleSubprogramPart.term, "Some(<term>)", &ModuleSubprogramPart.term)) {
      if (ofp_traverse_ModuleSubprogramPart(ModuleSubprogramPart.term, &ModuleSubprogramPart)) {
         // MATCHED ModuleSubprogramPart
      } else return ATfalse;
   }

      if (ofp_traverse_EndSubmoduleStmt(EndSubmoduleStmt.term, &EndSubmoduleStmt)) {
         // MATCHED EndSubmoduleStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1117 submodule-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SubmoduleStmt(ATerm term, OFP::SubmoduleStmt* SubmoduleStmt)
{
#ifdef DEBUG_PRINT
   printf("SubmoduleStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ParentIdentifier ParentIdentifier;
 OFP::Name SubmoduleName;
 OFP::EOS EOS;
 if (ATmatch(term, "SubmoduleStmt(<term>,<term>,<term>,<term>)", &Label.term, &ParentIdentifier.term, &SubmoduleName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ParentIdentifier(ParentIdentifier.term, &ParentIdentifier)) {
         // MATCHED ParentIdentifier
      } else return ATfalse;

      if (ofp_traverse_Name(SubmoduleName.term, &SubmoduleName)) {
         // MATCHED Name
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1118 parent-identifier
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ParentIdentifier(ATerm term, OFP::ParentIdentifier* ParentIdentifier)
{
#ifdef DEBUG_PRINT
   printf("ParentIdentifier: %s\n", ATwriteToString(term));
#endif

 OFP::Name AncestorModuleName;
 OFP::Name ParentSubmoduleName;
 if (ATmatch(term, "ParentIdentifier(<term>,<term>)", &AncestorModuleName.term, &ParentSubmoduleName.term)) {

      if (ofp_traverse_Name(AncestorModuleName.term, &AncestorModuleName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(ParentSubmoduleName.term, "Some(<term>)", &ParentSubmoduleName.term)) {
   if (ATmatch(ParentSubmoduleName.term, "(<term>)", &ParentSubmoduleName.term)) {
      if (ofp_traverse_Name(ParentSubmoduleName.term, &ParentSubmoduleName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1119 end-submodule-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndSubmoduleStmt(ATerm term, OFP::EndSubmoduleStmt* EndSubmoduleStmt)
{
#ifdef DEBUG_PRINT
   printf("EndSubmoduleStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name SubmoduleName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndSubmoduleStmt(<term>,<term>,<term>)", &Label.term, &SubmoduleName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(SubmoduleName.term, "Some(<term>)", &SubmoduleName.term)) {
   if (ATmatch(SubmoduleName.term, "(Some(<term>))", &SubmoduleName.term)) {
      if (ofp_traverse_Name(SubmoduleName.term, &SubmoduleName)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1120 block-data
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BlockData(ATerm term, OFP::BlockData* BlockData)
{
#ifdef DEBUG_PRINT
   printf("\nBlockData: %s\n", ATwriteToString(term));
#endif

 OFP::BlockDataStmt BlockDataStmt;
 OFP::SpecificationPart SpecificationPart;
 OFP::EndBlockDataStmt EndBlockDataStmt;
 if (ATmatch(term, "BlockData(<term>,<term>,<term>)", &BlockDataStmt.term, &SpecificationPart.term, &EndBlockDataStmt.term)) {

      if (ofp_traverse_BlockDataStmt(BlockDataStmt.term, &BlockDataStmt)) {
         // MATCHED BlockDataStmt
      } else return ATfalse;

      if (ofp_traverse_SpecificationPart(SpecificationPart.term, &SpecificationPart)) {
         // MATCHED SpecificationPart
      } else return ATfalse;

      if (ofp_traverse_EndBlockDataStmt(EndBlockDataStmt.term, &EndBlockDataStmt)) {
         // MATCHED EndBlockDataStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1121 block-data-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_BlockDataStmt(ATerm term, OFP::BlockDataStmt* BlockDataStmt)
{
#ifdef DEBUG_PRINT
   printf("BlockDataStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name BlockDataName;
 OFP::EOS EOS;
 if (ATmatch(term, "BlockDataStmt(<term>,<term>,<term>)", &Label.term, &BlockDataName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(BlockDataName.term, "Some(<term>)", &BlockDataName.term)) {
      if (ofp_traverse_Name(BlockDataName.term, &BlockDataName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1122 end-block-data-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndBlockDataStmt(ATerm term, OFP::EndBlockDataStmt* EndBlockDataStmt)
{
#ifdef DEBUG_PRINT
   printf("EndBlockDataStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "EndBlockDataStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::Name BlockDataName;
 OFP::EOS EOS1;
 if (ATmatch(term, "EndBlockDataStmt_BDN(<term>,<term>,<term>)", &Label1.term, &BlockDataName.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(BlockDataName.term, "Some(<term>)", &BlockDataName.term)) {
      if (ofp_traverse_Name(BlockDataName.term, &BlockDataName)) {
         // MATCHED Name
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED EndBlockDataStmt_BDN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1201 interface-block
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InterfaceBlock(ATerm term, OFP::InterfaceBlock* InterfaceBlock)
{
#ifdef DEBUG_PRINT
   printf("InterfaceBlock: %s\n", ATwriteToString(term));
#endif

 OFP::InterfaceStmt InterfaceStmt;
 OFP::InterfaceSpecification InterfaceSpecification;
 OFP::EndInterfaceStmt EndInterfaceStmt;
 if (ATmatch(term, "InterfaceBlock(<term>,<term>,<term>)", &InterfaceStmt.term, &InterfaceSpecification.term, &EndInterfaceStmt.term)) {

      if (ofp_traverse_InterfaceStmt(InterfaceStmt.term, &InterfaceStmt)) {
         // MATCHED InterfaceStmt
      } else return ATfalse;

   ATermList InterfaceSpecification_tail = (ATermList) ATmake("<term>", InterfaceSpecification.term);
   while (! ATisEmpty(InterfaceSpecification_tail)) {
      InterfaceSpecification.term = ATgetFirst(InterfaceSpecification_tail);
      InterfaceSpecification_tail = ATgetNext (InterfaceSpecification_tail);
      if (ofp_traverse_InterfaceSpecification(InterfaceSpecification.term, &InterfaceSpecification)) {
         // MATCHED InterfaceSpecification
      } else return ATfalse;
   }

      if (ofp_traverse_EndInterfaceStmt(EndInterfaceStmt.term, &EndInterfaceStmt)) {
         // MATCHED EndInterfaceStmt
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1202 interface-specification
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InterfaceSpecification(ATerm term, OFP::InterfaceSpecification* InterfaceSpecification)
{
#ifdef DEBUG_PRINT
   printf("InterfaceSpecification: %s\n", ATwriteToString(term));
#endif

 OFP::ProcedureStmt ProcedureStmt;
 if (ATmatch(term, "InterfaceSpecification_PS(<term>)", &ProcedureStmt.term)) {

      if (ofp_traverse_ProcedureStmt(ProcedureStmt.term, &ProcedureStmt)) {
         // MATCHED ProcedureStmt
      } else return ATfalse;

   // MATCHED InterfaceSpecification_PS

   return ATtrue;
 }

 OFP::InterfaceBody InterfaceBody;
 if (ATmatch(term, "InterfaceSpecification_IB(<term>)", &InterfaceBody.term)) {

      if (ofp_traverse_InterfaceBody(InterfaceBody.term, &InterfaceBody)) {
         // MATCHED InterfaceBody
      } else return ATfalse;

   // MATCHED InterfaceSpecification_IB

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1203 interface-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InterfaceStmt(ATerm term, OFP::InterfaceStmt* InterfaceStmt)
{
#ifdef DEBUG_PRINT
   printf("InterfaceStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "InterfaceStmt_AB(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED InterfaceStmt_AB

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::GenericSpec GenericSpec;
 OFP::EOS EOS1;
 if (ATmatch(term, "InterfaceStmt_GS(<term>,<term>,<term>)", &Label1.term, &GenericSpec.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(GenericSpec.term, "Some(<term>)", &GenericSpec.term)) {
      if (ofp_traverse_GenericSpec(GenericSpec.term, &GenericSpec)) {
         // MATCHED GenericSpec
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED InterfaceStmt_GS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1204 end-interface-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndInterfaceStmt(ATerm term, OFP::EndInterfaceStmt* EndInterfaceStmt)
{
#ifdef DEBUG_PRINT
   printf("EndInterfaceStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::GenericSpec GenericSpec;
 OFP::EOS EOS;
 if (ATmatch(term, "EndInterfaceStmt(<term>,<term>,<term>)", &Label.term, &GenericSpec.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(GenericSpec.term, "Some(<term>)", &GenericSpec.term)) {
      if (ofp_traverse_GenericSpec(GenericSpec.term, &GenericSpec)) {
         // MATCHED GenericSpec
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1205 interface-body
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InterfaceBody(ATerm term, OFP::InterfaceBody* InterfaceBody)
{
#ifdef DEBUG_PRINT
   printf("InterfaceBody: %s\n", ATwriteToString(term));
#endif

 OFP::SubroutineStmt SubroutineStmt;
 OFP::SpecificationPart SpecificationPart;
 OFP::EndSubroutineStmt EndSubroutineStmt;
 if (ATmatch(term, "InterfaceBody_SS(<term>,<term>,<term>)", &SubroutineStmt.term, &SpecificationPart.term, &EndSubroutineStmt.term)) {

      if (ofp_traverse_SubroutineStmt(SubroutineStmt.term, &SubroutineStmt)) {
         // MATCHED SubroutineStmt
      } else return ATfalse;

      if (ofp_traverse_SpecificationPart(SpecificationPart.term, &SpecificationPart)) {
         // MATCHED SpecificationPart
      } else return ATfalse;

      if (ofp_traverse_EndSubroutineStmt(EndSubroutineStmt.term, &EndSubroutineStmt)) {
         // MATCHED EndSubroutineStmt
      } else return ATfalse;

   // MATCHED InterfaceBody_SS

   return ATtrue;
 }

 OFP::FunctionStmt FunctionStmt;
 OFP::SpecificationPart SpecificationPart1;
 OFP::EndFunctionStmt EndFunctionStmt;
 if (ATmatch(term, "InterfaceBody_FS(<term>,<term>,<term>)", &FunctionStmt.term, &SpecificationPart1.term, &EndFunctionStmt.term)) {

      if (ofp_traverse_FunctionStmt(FunctionStmt.term, &FunctionStmt)) {
         // MATCHED FunctionStmt
      } else return ATfalse;

      if (ofp_traverse_SpecificationPart(SpecificationPart1.term, &SpecificationPart1)) {
         // MATCHED SpecificationPart
      } else return ATfalse;

      if (ofp_traverse_EndFunctionStmt(EndFunctionStmt.term, &EndFunctionStmt)) {
         // MATCHED EndFunctionStmt
      } else return ATfalse;

   // MATCHED InterfaceBody_FS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1206 procedure-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcedureStmt(ATerm term, OFP::ProcedureStmt* ProcedureStmt)
{
#ifdef DEBUG_PRINT
   printf("ProcedureStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ProcedureNameList ProcedureNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "ProcedureStmt_P(<term>,<term>,<term>)", &Label.term, &ProcedureNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ProcedureNameList(ProcedureNameList.term, &ProcedureNameList)) {
         // MATCHED ProcedureNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED ProcedureStmt_P

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::ProcedureNameList ProcedureNameList1;
 OFP::EOS EOS1;
 if (ATmatch(term, "ProcedureStmt_MP(<term>,<term>,<term>)", &Label1.term, &ProcedureNameList1.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ProcedureNameList(ProcedureNameList1.term, &ProcedureNameList1)) {
         // MATCHED ProcedureNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED ProcedureStmt_MP

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ProcedureNameList(ATerm term, OFP::ProcedureNameList* ProcedureNameList)
{
#ifdef DEBUG_PRINT
   printf("ProcedureNameList: %s\n", ATwriteToString(term));
#endif

 OFP::Name ProcedureName;
 if (ATmatch(term, "ProcedureNameList(<term>)", &ProcedureName.term)) {

   ATermList ProcedureName_tail = (ATermList) ATmake("<term>", ProcedureName.term);
   while (! ATisEmpty(ProcedureName_tail)) {
      ProcedureName.term = ATgetFirst(ProcedureName_tail);
      ProcedureName_tail = ATgetNext (ProcedureName_tail);
      if (ofp_traverse_Name(ProcedureName.term, &ProcedureName)) {
         // MATCHED Name
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1207 generic-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_GenericSpec(ATerm term, OFP::GenericSpec* GenericSpec)
{
#ifdef DEBUG_PRINT
   printf("GenericSpec: %s\n", ATwriteToString(term));
#endif

 OFP::DefinedIoGenericSpec DefinedIoGenericSpec;
 if (ATmatch(term, "GenericSpec_DIGS(<term>)", &DefinedIoGenericSpec.term)) {

      if (ofp_traverse_DefinedIoGenericSpec(DefinedIoGenericSpec.term, &DefinedIoGenericSpec)) {
         // MATCHED DefinedIoGenericSpec
      } else return ATfalse;

   // MATCHED GenericSpec_DIGS

   return ATtrue;
 }

 if (ATmatch(term, "GenericSpec_AS")) {

   // MATCHED GenericSpec_AS

   return ATtrue;
 }

 OFP::DefinedOperator DefinedOperator;
 if (ATmatch(term, "GenericSpec_OP(<term>)", &DefinedOperator.term)) {

      if (ofp_traverse_DefinedOperator(DefinedOperator.term, &DefinedOperator)) {
         // MATCHED DefinedOperator
      } else return ATfalse;

   // MATCHED GenericSpec_OP

   return ATtrue;
 }

 OFP::Name GenericName;
 if (ATmatch(term, "GenericSpec_GN(<term>)", &GenericName.term)) {

      if (ofp_traverse_Name(GenericName.term, &GenericName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED GenericSpec_GN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1208 defined-io-generic-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DefinedIoGenericSpec(ATerm term, OFP::DefinedIoGenericSpec* DefinedIoGenericSpec)
{
#ifdef DEBUG_PRINT
   printf("DefinedIoGenericSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "DefinedIoGenericSpec_WU")) {

   // MATCHED DefinedIoGenericSpec_WU

   return ATtrue;
 }

 if (ATmatch(term, "DefinedIoGenericSpec_WF")) {

   // MATCHED DefinedIoGenericSpec_WF

   return ATtrue;
 }

 if (ATmatch(term, "DefinedIoGenericSpec_RU")) {

   // MATCHED DefinedIoGenericSpec_RU

   return ATtrue;
 }

 if (ATmatch(term, "DefinedIoGenericSpec_RF")) {

   // MATCHED DefinedIoGenericSpec_RF

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1209 import-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ImportStmt(ATerm term, OFP::ImportStmt* ImportStmt)
{
#ifdef DEBUG_PRINT
   printf("ImportStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "ImportStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::ImportNameList ImportNameList;
 OFP::EOS EOS1;
 if (ATmatch(term, "ImportStmt_INL(<term>,<term>,<term>)", &Label1.term, &ImportNameList.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ImportNameList(ImportNameList.term, &ImportNameList)) {
         // MATCHED ImportNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED ImportStmt_INL

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ImportNameList(ATerm term, OFP::ImportNameList* ImportNameList)
{
#ifdef DEBUG_PRINT
   printf("ImportNameList: %s\n", ATwriteToString(term));
#endif

 OFP::Name ImportName;
 if (ATmatch(term, "ImportNameList(<term>)", &ImportName.term)) {

   ATermList ImportName_tail = (ATermList) ATmake("<term>", ImportName.term);
   while (! ATisEmpty(ImportName_tail)) {
      ImportName.term = ATgetFirst(ImportName_tail);
      ImportName_tail = ATgetNext (ImportName_tail);
      if (ofp_traverse_Name(ImportName.term, &ImportName)) {
         // MATCHED Name
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1210 external-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ExternalStmt(ATerm term, OFP::ExternalStmt* ExternalStmt)
{
#ifdef DEBUG_PRINT
   printf("ExternalStmt: %s\n", ATwriteToString(term));
#endif

#ifdef OBSOLETE
 OFP::Label Label;
//OBSOLETE  OFP::ExternalNameList ExternalNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "ExternalStmt(<term>,<term>,<term>)", &Label.term, &ExternalNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_ExternalNameList(ExternalNameList.term, &ExternalNameList)) {
         // MATCHED ExternalNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }
#endif

 return ATfalse;
}

//========================================================================================
// R1211 procedure-declaration-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcedureDeclarationStmt(ATerm term, OFP::ProcedureDeclarationStmt* ProcedureDeclarationStmt)
{
#ifdef DEBUG_PRINT
   printf("ProcedureDeclarationStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ProcInterface ProcInterface;
 OFP::ProcAttrSpecList ProcAttrSpecList;
 OFP::ProcDeclList ProcDeclList;
 OFP::EOS EOS;
 if (ATmatch(term, "ProcedureDeclarationStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &ProcInterface.term, &ProcAttrSpecList.term, &ProcDeclList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(ProcInterface.term, "Some(<term>)", &ProcInterface.term)) {
      if (ofp_traverse_ProcInterface(ProcInterface.term, &ProcInterface)) {
         // MATCHED ProcInterface
      } else return ATfalse;
   }

   if (ATmatch(ProcAttrSpecList.term, "Some(<term>)", &ProcAttrSpecList.term)) {
   if (ATmatch(ProcAttrSpecList.term, "(Some(<term>))", &ProcAttrSpecList.term)) {
   if (ATmatch(ProcAttrSpecList.term, "(<term>)", &ProcAttrSpecList.term)) {
      if (ofp_traverse_ProcAttrSpecList(ProcAttrSpecList.term, &ProcAttrSpecList)) {
         // MATCHED ProcAttrSpecList
      } else return ATfalse;
   }
   }
   }

      if (ofp_traverse_ProcDeclList(ProcDeclList.term, &ProcDeclList)) {
         // MATCHED ProcDeclList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1212 proc-interface
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcInterface(ATerm term, OFP::ProcInterface* ProcInterface)
{
#ifdef DEBUG_PRINT
   printf("ProcInterface: %s\n", ATwriteToString(term));
#endif

 OFP::DeclarationTypeSpec DeclarationTypeSpec;
 if (ATmatch(term, "ProcInterface_DTS(<term>)", &DeclarationTypeSpec.term)) {

      if (ofp_traverse_DeclarationTypeSpec(DeclarationTypeSpec.term, &DeclarationTypeSpec)) {
         // MATCHED DeclarationTypeSpec
      } else return ATfalse;

   // MATCHED ProcInterface_DTS

   return ATtrue;
 }

 OFP::InterfaceName InterfaceName;
 if (ATmatch(term, "ProcInterface_IN(<term>)", &InterfaceName.term)) {

      if (ofp_traverse_InterfaceName(InterfaceName.term, &InterfaceName)) {
         // MATCHED InterfaceName
      } else return ATfalse;

   // MATCHED ProcInterface_IN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1213 proc-attr-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcAttrSpec(ATerm term, OFP::ProcAttrSpec* ProcAttrSpec)
{
#ifdef DEBUG_PRINT
   printf("ProcAttrSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "ProcAttrSpec_SAVE")) {

   // MATCHED ProcAttrSpec_SAVE

   return ATtrue;
 }

 if (ATmatch(term, "ProcAttrSpec_POINTER")) {

   // MATCHED ProcAttrSpec_POINTER

   return ATtrue;
 }

 if (ATmatch(term, "ProcAttrSpec_OPTIONAL")) {

   // MATCHED ProcAttrSpec_OPTIONAL

   return ATtrue;
 }

 OFP::IntentSpec IntentSpec;
 if (ATmatch(term, "ProcAttrSpec_INTENT(<term>)", &IntentSpec.term)) {

      if (ofp_traverse_IntentSpec(IntentSpec.term, &IntentSpec)) {
         // MATCHED IntentSpec
      } else return ATfalse;

   // MATCHED ProcAttrSpec_INTENT

   return ATtrue;
 }

 OFP::ProcLanguageBindingSpec ProcLanguageBindingSpec;
 if (ATmatch(term, "ProcAttrSpec_PLBS(<term>)", &ProcLanguageBindingSpec.term)) {

      if (ofp_traverse_ProcLanguageBindingSpec(ProcLanguageBindingSpec.term, &ProcLanguageBindingSpec)) {
         // MATCHED ProcLanguageBindingSpec
      } else return ATfalse;

   // MATCHED ProcAttrSpec_PLBS

   return ATtrue;
 }

 OFP::AccessSpec AccessSpec;
 if (ATmatch(term, "ProcAttrSpec_AS(<term>)", &AccessSpec.term)) {

      if (ofp_traverse_AccessSpec(AccessSpec.term, &AccessSpec)) {
         // MATCHED AccessSpec
      } else return ATfalse;

   // MATCHED ProcAttrSpec_AS

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ProcAttrSpecList(ATerm term, OFP::ProcAttrSpecList* ProcAttrSpecList)
{
#ifdef DEBUG_PRINT
   printf("ProcAttrSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ProcAttrSpec ProcAttrSpec;
 if (ATmatch(term, "ProcAttrSpecList(<term>)", &ProcAttrSpec.term)) {

   ATermList ProcAttrSpec_tail = (ATermList) ATmake("<term>", ProcAttrSpec.term);
   while (! ATisEmpty(ProcAttrSpec_tail)) {
      ProcAttrSpec.term = ATgetFirst(ProcAttrSpec_tail);
      ProcAttrSpec_tail = ATgetNext (ProcAttrSpec_tail);
      if (ofp_traverse_ProcAttrSpec(ProcAttrSpec.term, &ProcAttrSpec)) {
         // MATCHED ProcAttrSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1214 proc-decl
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcDecl(ATerm term, OFP::ProcDecl* ProcDecl)
{
#ifdef DEBUG_PRINT
   printf("ProcDecl: %s\n", ATwriteToString(term));
#endif

 OFP::Name ProcedureEntityName;
 OFP::ProcPointerInit ProcPointerInit;
 if (ATmatch(term, "ProcDecl(<term>,<term>)", &ProcedureEntityName.term, &ProcPointerInit.term)) {

      if (ofp_traverse_Name(ProcedureEntityName.term, &ProcedureEntityName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(ProcPointerInit.term, "Some(<term>)", &ProcPointerInit.term)) {
   if (ATmatch(ProcPointerInit.term, "(<term>)", &ProcPointerInit.term)) {
      if (ofp_traverse_ProcPointerInit(ProcPointerInit.term, &ProcPointerInit)) {
         // MATCHED ProcPointerInit
      } else return ATfalse;
   }
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ProcDeclList(ATerm term, OFP::ProcDeclList* ProcDeclList)
{
#ifdef DEBUG_PRINT
   printf("ProcDeclList: %s\n", ATwriteToString(term));
#endif

 OFP::ProcDecl ProcDecl;
 if (ATmatch(term, "ProcDeclList(<term>)", &ProcDecl.term)) {

   ATermList ProcDecl_tail = (ATermList) ATmake("<term>", ProcDecl.term);
   while (! ATisEmpty(ProcDecl_tail)) {
      ProcDecl.term = ATgetFirst(ProcDecl_tail);
      ProcDecl_tail = ATgetNext (ProcDecl_tail);
      if (ofp_traverse_ProcDecl(ProcDecl.term, &ProcDecl)) {
         // MATCHED ProcDecl
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1215 interface-name
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InterfaceName(ATerm term, OFP::InterfaceName* InterfaceName)
{
#ifdef DEBUG_PRINT
   printf("InterfaceName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "InterfaceName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1216 proc-pointer-init
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcPointerInit(ATerm term, OFP::ProcPointerInit* ProcPointerInit)
{
#ifdef DEBUG_PRINT
   printf("ProcPointerInit: %s\n", ATwriteToString(term));
#endif

 OFP::InitialProcTarget InitialProcTarget;
 if (ATmatch(term, "ProcPointerInit_IPT(<term>)", &InitialProcTarget.term)) {

      if (ofp_traverse_InitialProcTarget(InitialProcTarget.term, &InitialProcTarget)) {
         // MATCHED InitialProcTarget
      } else return ATfalse;

   // MATCHED ProcPointerInit_IPT

   return ATtrue;
 }

 OFP::NullInit NullInit;
 if (ATmatch(term, "ProcPointerInit_NI(<term>)", &NullInit.term)) {

      if (ofp_traverse_NullInit(NullInit.term, &NullInit)) {
         // MATCHED NullInit
      } else return ATfalse;

   // MATCHED ProcPointerInit_NI

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1217 initial-proc-target
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_InitialProcTarget(ATerm term, OFP::InitialProcTarget* InitialProcTarget)
{
#ifdef DEBUG_PRINT
   printf("InitialProcTarget: %s\n", ATwriteToString(term));
#endif

 OFP::Name ProcedureName;
 if (ATmatch(term, "InitialProcTarget(<term>)", &ProcedureName.term)) {

      if (ofp_traverse_Name(ProcedureName.term, &ProcedureName)) {
         // MATCHED Name
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1218 intrinsic-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntrinsicStmt(ATerm term, OFP::IntrinsicStmt* IntrinsicStmt)
{
#ifdef DEBUG_PRINT
   printf("IntrinsicStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::IntrinsicProcedureNameList IntrinsicProcedureNameList;
 OFP::EOS EOS;
 if (ATmatch(term, "IntrinsicStmt(<term>,<term>,<term>)", &Label.term, &IntrinsicProcedureNameList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_IntrinsicProcedureNameList(IntrinsicProcedureNameList.term, &IntrinsicProcedureNameList)) {
         // MATCHED IntrinsicProcedureNameList
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_IntrinsicProcedureNameList(ATerm term, OFP::IntrinsicProcedureNameList* IntrinsicProcedureNameList)
{
#ifdef DEBUG_PRINT
   printf("IntrinsicProcedureNameList: %s\n", ATwriteToString(term));
#endif

 OFP::Name IntrinsicProcedureName;
 if (ATmatch(term, "IntrinsicProcedureNameList(<term>)", &IntrinsicProcedureName.term)) {

   ATermList IntrinsicProcedureName_tail = (ATermList) ATmake("<term>", IntrinsicProcedureName.term);
   while (! ATisEmpty(IntrinsicProcedureName_tail)) {
      IntrinsicProcedureName.term = ATgetFirst(IntrinsicProcedureName_tail);
      IntrinsicProcedureName_tail = ATgetNext (IntrinsicProcedureName_tail);
      if (ofp_traverse_Name(IntrinsicProcedureName.term, &IntrinsicProcedureName)) {
         // MATCHED Name
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1219 function-reference
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FunctionReference(ATerm term, OFP::FunctionReference* FunctionReference)
{
#ifdef DEBUG_PRINT
   printf("FunctionReference: %s\n", ATwriteToString(term));
#endif

 OFP::ProcedureDesignator ProcedureDesignator;
 OFP::ActualArgSpecList ActualArgSpecList;
 if (ATmatch(term, "FunctionReference(<term>,<term>)", &ProcedureDesignator.term, &ActualArgSpecList.term)) {

      if (ofp_traverse_ProcedureDesignator(ProcedureDesignator.term, &ProcedureDesignator)) {
         // MATCHED ProcedureDesignator
      } else return ATfalse;

   if (ATmatch(ActualArgSpecList.term, "Some(<term>)", &ActualArgSpecList.term)) {
      if (ofp_traverse_ActualArgSpecList(ActualArgSpecList.term, &ActualArgSpecList)) {
         // MATCHED ActualArgSpecList
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1220 call-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_CallStmt(ATerm term, OFP::CallStmt* CallStmt)
{
#ifdef DEBUG_PRINT
   printf("CallStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::ProcedureDesignator ProcedureDesignator;
 OFP::ActualArgSpecList ActualArgSpecList;
 OFP::EOS EOS;
 if (ATmatch(term, "CallStmt(<term>,<term>,<term>,<term>)", &Label.term, &ProcedureDesignator.term, &ActualArgSpecList.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         CallStmt->setLabel(Label.newLabel());
      } else return ATfalse;
   }

      if (ofp_traverse_ProcedureDesignator(ProcedureDesignator.term, &ProcedureDesignator)) {
         // MATCHED ProcedureDesignator
         CallStmt->setProcedureDesignator(ProcedureDesignator.newProcedureDesignator());
      } else return ATfalse;

   if (ATmatch(ActualArgSpecList.term, "Some(<term>)", &ActualArgSpecList.term)) {
   if (ATmatch(ActualArgSpecList.term, "(Some(<term>))", &ActualArgSpecList.term)) {
      if (ofp_traverse_ActualArgSpecList(ActualArgSpecList.term, &ActualArgSpecList)) {
         // MATCHED ActualArgSpecList
         CallStmt->setActualArgSpecList(ActualArgSpecList.newActualArgSpecList());
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         CallStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1221 procedure-designator
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcedureDesignator(ATerm term, OFP::ProcedureDesignator* ProcedureDesignator)
{
#ifdef DEBUG_PRINT
   printf("ProcedureDesignator: %s\n", ATwriteToString(term));
#endif

 OFP::DataRef DataRef;
 OFP::Name BindingName;
 if (ATmatch(term, "ProcedureDesignator_AMB(<term>,<term>)", &DataRef.term, &BindingName.term)) {

      if (ofp_traverse_DataRef(DataRef.term, &DataRef)) {
         // MATCHED DataRef
      } else return ATfalse;

      if (ofp_traverse_Name(BindingName.term, &BindingName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED ProcedureDesignator_AMB

   return ATtrue;
 }

 OFP::Name ProcedureName;
 if (ATmatch(term, "ProcedureDesignator_PN(<term>)", &ProcedureName.term)) {

      if (ofp_traverse_Name(ProcedureName.term, &ProcedureName)) {
         // MATCHED Name
      } else return ATfalse;

   // MATCHED ProcedureDesignator_PN

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1222 actual-arg-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ActualArgSpec(ATerm term, OFP::ActualArgSpec* ActualArgSpec)
{
#ifdef DEBUG_PRINT
   printf("ActualArgSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Keyword Keyword;
 OFP::ActualArg ActualArg;
 if (ATmatch(term, "ActualArgSpec(<term>,<term>)", &Keyword.term, &ActualArg.term)) {

   if (ATmatch(Keyword.term, "Some(<term>)", &Keyword.term)) {
   if (ATmatch(Keyword.term, "(<term>)", &Keyword.term)) {
      if (ofp_traverse_Keyword(Keyword.term, &Keyword)) {
         // MATCHED Keyword
      } else return ATfalse;
   }
   }

      if (ofp_traverse_ActualArg(ActualArg.term, &ActualArg)) {
         // MATCHED ActualArg
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ActualArgSpecList(ATerm term, OFP::ActualArgSpecList* ActualArgSpecList)
{
#ifdef DEBUG_PRINT
   printf("ActualArgSpecList: %s\n", ATwriteToString(term));
#endif

 OFP::ActualArgSpec ActualArgSpec;
 if (ATmatch(term, "ActualArgSpecList(<term>)", &ActualArgSpec.term)) {

   ATermList ActualArgSpec_tail = (ATermList) ATmake("<term>", ActualArgSpec.term);
   while (! ATisEmpty(ActualArgSpec_tail)) {
      ActualArgSpec.term = ATgetFirst(ActualArgSpec_tail);
      ActualArgSpec_tail = ATgetNext (ActualArgSpec_tail);
      if (ofp_traverse_ActualArgSpec(ActualArgSpec.term, &ActualArgSpec)) {
         // MATCHED ActualArgSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1223 actual-arg
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ActualArg(ATerm term, OFP::ActualArg* ActualArg)
{
#ifdef DEBUG_PRINT
   printf("ActualArg: %s\n", ATwriteToString(term));
#endif

 OFP::AltReturnSpec AltReturnSpec;
 if (ATmatch(term, "ActualArg(<term>)", &AltReturnSpec.term)) {

      if (ofp_traverse_AltReturnSpec(AltReturnSpec.term, &AltReturnSpec)) {
         // MATCHED AltReturnSpec
      } else return ATfalse;

   return ATtrue;
 }

 OFP::Expr Expr;
 if (ATmatch(term, "ActualArg_AMB(<term>)", &Expr.term)) {

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

   // MATCHED ActualArg_AMB

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1224 alt-return-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_AltReturnSpec(ATerm term, OFP::AltReturnSpec* AltReturnSpec)
{
#ifdef DEBUG_PRINT
   printf("AltReturnSpec: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 if (ATmatch(term, "AltReturnSpec(<term>)", &Label.term)) {

      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1225 prefix
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Prefix(ATerm term, OFP::Prefix* Prefix)
{
#ifdef DEBUG_PRINT
   printf("Prefix: %s\n", ATwriteToString(term));
#endif

 OFP::PrefixSpec PrefixSpec;
 if (ATmatch(term, "Prefix(<term>)", &PrefixSpec.term)) {

   ATermList PrefixSpec_tail = (ATermList) ATmake("<term>", PrefixSpec.term);
   while (! ATisEmpty(PrefixSpec_tail)) {
      PrefixSpec.term = ATgetFirst(PrefixSpec_tail);
      PrefixSpec_tail = ATgetNext (PrefixSpec_tail);
      if (ofp_traverse_PrefixSpec(PrefixSpec.term, &PrefixSpec)) {
         // MATCHED PrefixSpec
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1226 prefix-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_PrefixSpec(ATerm term, OFP::PrefixSpec* PrefixSpec)
{
#ifdef DEBUG_PRINT
   printf("PrefixSpec: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "PrefixSpec_RECURSIVE")) {

   // MATCHED PrefixSpec_RECURSIVE

   return ATtrue;
 }

 if (ATmatch(term, "PrefixSpec_PURE")) {

   // MATCHED PrefixSpec_PURE

   return ATtrue;
 }

 if (ATmatch(term, "PrefixSpec_MODULE")) {

   // MATCHED PrefixSpec_MODULE

   return ATtrue;
 }

 if (ATmatch(term, "PrefixSpec_IMPURE")) {

   // MATCHED PrefixSpec_IMPURE

   return ATtrue;
 }

 if (ATmatch(term, "PrefixSpec_ELEMENTAL")) {

   // MATCHED PrefixSpec_ELEMENTAL

   return ATtrue;
 }

 OFP::DeclarationTypeSpec DeclarationTypeSpec;
 if (ATmatch(term, "PrefixSpec_DTS(<term>)", &DeclarationTypeSpec.term)) {

      if (ofp_traverse_DeclarationTypeSpec(DeclarationTypeSpec.term, &DeclarationTypeSpec)) {
         // MATCHED DeclarationTypeSpec
      } else return ATfalse;

   // MATCHED PrefixSpec_DTS

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1227 function-subprogram
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FunctionSubprogram(ATerm term, OFP::FunctionSubprogram* FunctionSubprogram)
{
#ifdef DEBUG_PRINT
   printf("FunctionSubprogram: %s\n", ATwriteToString(term));
#endif

 OFP::FunctionStmt FunctionStmt;
 OFP::InitialSpecPart InitialSpecPart;
 OFP::SpecAndExecPart SpecAndExecPart;
 OFP::InternalSubprogramPart InternalSubprogramPart;
 OFP::EndFunctionStmt EndFunctionStmt;
 if (ATmatch(term, "FunctionSubprogram(<term>,<term>,<term>,<term>,<term>)", &FunctionStmt.term, &InitialSpecPart.term, &SpecAndExecPart.term, &InternalSubprogramPart.term, &EndFunctionStmt.term)) {

      if (ofp_traverse_FunctionStmt(FunctionStmt.term, &FunctionStmt)) {
         // MATCHED FunctionStmt
         FunctionSubprogram->setFunctionStmt(FunctionStmt.newFunctionStmt());
      } else return ATfalse;

      if (ofp_traverse_InitialSpecPart(InitialSpecPart.term, &InitialSpecPart)) {
         // MATCHED InitialSpecPart
         FunctionSubprogram->setInitialSpecPart(InitialSpecPart.newInitialSpecPart());
      } else return ATfalse;

      if (ofp_traverse_SpecAndExecPart(SpecAndExecPart.term, &SpecAndExecPart)) {
         // MATCHED SpecAndExecPart
         FunctionSubprogram->setSpecAndExecPart(SpecAndExecPart.newSpecAndExecPart());
      } else return ATfalse;

   if (ATmatch(InternalSubprogramPart.term, "Some(<term>)", &InternalSubprogramPart.term)) {
      if (ofp_traverse_InternalSubprogramPart(InternalSubprogramPart.term, &InternalSubprogramPart)) {
         // MATCHED InternalSubprogramPart
         FunctionSubprogram->setInternalSubprogramPart(InternalSubprogramPart.newInternalSubprogramPart());
      } else return ATfalse;
   }

      if (ofp_traverse_EndFunctionStmt(EndFunctionStmt.term, &EndFunctionStmt)) {
         // MATCHED EndFunctionStmt
         FunctionSubprogram->setEndFunctionStmt(EndFunctionStmt.newEndFunctionStmt());
      } else return ATfalse;

   ast->build_FunctionSubprogram(FunctionSubprogram);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1228 function-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_FunctionStmt(ATerm term, OFP::FunctionStmt* FunctionStmt)
{
#ifdef DEBUG_PRINT
   printf("FunctionStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Prefix Prefix;
 OFP::Name FunctionName;
 OFP::DummyArgNameList DummyArgNameList;
 OFP::Suffix Suffix;
 OFP::EOS EOS;
 if (ATmatch(term, "FunctionStmt(<term>,<term>,<term>,<term>,<term>,<term>)", &Label.term, &Prefix.term, &FunctionName.term, &DummyArgNameList.term, &Suffix.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
   if (ATmatch(Label.term, "(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         FunctionStmt->setLabel(Label.newLabel());
      } else return ATfalse;
   }
   }

   if (ATmatch(Prefix.term, "Some(<term>)", &Prefix.term)) {
      if (ofp_traverse_Prefix(Prefix.term, &Prefix)) {
         // MATCHED Prefix
         FunctionStmt->setPrefix(Prefix.newPrefix());
      } else return ATfalse;
   }

      if (ofp_traverse_Name(FunctionName.term, &FunctionName)) {
         // MATCHED Name
         FunctionStmt->setFunctionName(FunctionName.newName());
      } else return ATfalse;

   if (ATmatch(DummyArgNameList.term, "Some(<term>)", &DummyArgNameList.term)) {
      if (ofp_traverse_DummyArgNameList(DummyArgNameList.term, &DummyArgNameList)) {
         // MATCHED DummyArgNameList
         FunctionStmt->setDummyArgNameList(DummyArgNameList.newDummyArgNameList());
      } else return ATfalse;
   }

   if (ATmatch(Suffix.term, "Some(<term>)", &Suffix.term)) {
      if (ofp_traverse_Suffix(Suffix.term, &Suffix)) {
         // MATCHED Suffix
         FunctionStmt->setSuffix(Suffix.newSuffix());
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         FunctionStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

   ast->build_FunctionStmt(FunctionStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1229 proc-language-binding-spec
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ProcLanguageBindingSpec(ATerm term, OFP::ProcLanguageBindingSpec* ProcLanguageBindingSpec)
{
#ifdef DEBUG_PRINT
   printf("ProcLanguageBindingSpec: %s\n", ATwriteToString(term));
#endif

 OFP::LanguageBindingSpec LanguageBindingSpec;
 if (ATmatch(term, "ProcLanguageBindingSpec(<term>)", &LanguageBindingSpec.term)) {

      if (ofp_traverse_LanguageBindingSpec(LanguageBindingSpec.term, &LanguageBindingSpec)) {
         // MATCHED LanguageBindingSpec
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1230 dummy-arg-name
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DummyArgName(ATerm term, OFP::DummyArgName* DummyArgName)
{
#ifdef DEBUG_PRINT
   printf("DummyArgName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "DummyArgName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1231 suffix
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Suffix(ATerm term, OFP::Suffix* Suffix)
{
#ifdef DEBUG_PRINT
   printf("Suffix: %s\n", ATwriteToString(term));
#endif

 OFP::Name ResultName;
 OFP::ProcLanguageBindingSpec ProcLanguageBindingSpec;
 if (ATmatch(term, "Suffix_RP(<term>,<term>)", &ResultName.term, &ProcLanguageBindingSpec.term)) {

      if (ofp_traverse_Name(ResultName.term, &ResultName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(ProcLanguageBindingSpec.term, "Some(<term>)", &ProcLanguageBindingSpec.term)) {
      if (ofp_traverse_ProcLanguageBindingSpec(ProcLanguageBindingSpec.term, &ProcLanguageBindingSpec)) {
         // MATCHED ProcLanguageBindingSpec
      } else return ATfalse;
   }

   // MATCHED Suffix_RP

   return ATtrue;
 }

 OFP::ProcLanguageBindingSpec ProcLanguageBindingSpec1;
 OFP::Name ResultName1;
 if (ATmatch(term, "Suffix_PR(<term>,<term>)", &ProcLanguageBindingSpec1.term, &ResultName1.term)) {

      if (ofp_traverse_ProcLanguageBindingSpec(ProcLanguageBindingSpec1.term, &ProcLanguageBindingSpec1)) {
         // MATCHED ProcLanguageBindingSpec
      } else return ATfalse;

   if (ATmatch(ResultName1.term, "Some(<term>)", &ResultName1.term)) {
   if (ATmatch(ResultName1.term, "(<term>)", &ResultName1.term)) {
      if (ofp_traverse_Name(ResultName1.term, &ResultName1)) {
         // MATCHED Name
      } else return ATfalse;
   }
   }

   // MATCHED Suffix_PR

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1232 end-function-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndFunctionStmt(ATerm term, OFP::EndFunctionStmt* EndFunctionStmt)
{
#ifdef DEBUG_PRINT
   printf("EndFunctionStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name FunctionName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndFunctionStmt(<term>,<term>,<term>)", &Label.term, &FunctionName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
   if (ATmatch(Label.term, "(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         EndFunctionStmt->setLabel(Label.newLabel());
      } else return ATfalse;
   }
   }

   if (ATmatch(FunctionName.term, "Some(<term>)", &FunctionName.term)) {
   if (ATmatch(FunctionName.term, "(Some(<term>))", &FunctionName.term)) {
      if (ofp_traverse_Name(FunctionName.term, &FunctionName)) {
         // MATCHED Name
         EndFunctionStmt->setFunctionName(FunctionName.newName());
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         EndFunctionStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

   ast->build_EndFunctionStmt(EndFunctionStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1233 subroutine-subprogram
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SubroutineSubprogram(ATerm term, OFP::SubroutineSubprogram* SubroutineSubprogram)
{
#ifdef DEBUG_PRINT
   printf("SubroutineSubprogram: %s\n", ATwriteToString(term));
#endif

 OFP::SubroutineStmt SubroutineStmt;
 OFP::InitialSpecPart InitialSpecPart;
 OFP::SpecAndExecPart SpecAndExecPart;
 OFP::InternalSubprogramPart InternalSubprogramPart;
 OFP::EndSubroutineStmt EndSubroutineStmt;
 if (ATmatch(term, "SubroutineSubprogram(<term>,<term>,<term>,<term>,<term>)", &SubroutineStmt.term, &InitialSpecPart.term, &SpecAndExecPart.term, &InternalSubprogramPart.term, &EndSubroutineStmt.term)) {

      if (ofp_traverse_SubroutineStmt(SubroutineStmt.term, &SubroutineStmt)) {
         // MATCHED SubroutineStmt
         SubroutineSubprogram->setSubroutineStmt(SubroutineStmt.newSubroutineStmt());
      } else return ATfalse;

      if (ofp_traverse_InitialSpecPart(InitialSpecPart.term, &InitialSpecPart)) {
         // MATCHED InitialSpecPart
         SubroutineSubprogram->setInitialSpecPart(InitialSpecPart.newInitialSpecPart());
      } else return ATfalse;

      if (ofp_traverse_SpecAndExecPart(SpecAndExecPart.term, &SpecAndExecPart)) {
         // MATCHED SpecAndExecPart
         SubroutineSubprogram->setSpecAndExecPart(SpecAndExecPart.newSpecAndExecPart());
      } else return ATfalse;

   if (ATmatch(InternalSubprogramPart.term, "Some(<term>)", &InternalSubprogramPart.term)) {
      if (ofp_traverse_InternalSubprogramPart(InternalSubprogramPart.term, &InternalSubprogramPart)) {
         // MATCHED InternalSubprogramPart
         SubroutineSubprogram->setInternalSubprogramPart(InternalSubprogramPart.newInternalSubprogramPart());
      } else return ATfalse;
   }

      if (ofp_traverse_EndSubroutineStmt(EndSubroutineStmt.term, &EndSubroutineStmt)) {
         // MATCHED EndSubroutineStmt
         SubroutineSubprogram->setEndSubroutineStmt(EndSubroutineStmt.newEndSubroutineStmt());
      } else return ATfalse;

   ast->build_SubroutineSubprogram(SubroutineSubprogram);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1234 subroutine-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SubroutineStmt(ATerm term, OFP::SubroutineStmt* SubroutineStmt)
{
#ifdef DEBUG_PRINT
   printf("SubroutineStmt: %s\n", ATwriteToString(term));
#endif

   OFP::Label Label;
   OFP::Prefix Prefix;
   OFP::Name SubroutineName;
   OFP::DummyArgList DummyArgList;
   OFP::ProcLanguageBindingSpec ProcLanguageBindingSpec;
   OFP::EOS EOS;

   if (ATmatch(term, "SubroutineStmt(<term>,<term>,<term>,<term>,<term>,<term>)", &Label.term, &Prefix.term, &SubroutineName.term, &DummyArgList.term, &ProcLanguageBindingSpec.term, &EOS.term)) {

      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         SubroutineStmt->setLabel(Label.newLabel());
      } // Optional

      if (ofp_traverse_Prefix(Prefix.term, &Prefix)) {
         // MATCHED Prefix
         SubroutineStmt->setPrefix(Prefix.newPrefix());
      } // Optional

      if (ofp_traverse_Name(SubroutineName.term, &SubroutineName)) {
         // MATCHED SubroutineName
         SubroutineStmt->setSubroutineName(SubroutineName.newName());
      } else return ATfalse;

      if (ofp_traverse_DummyArgList(DummyArgList.term, &DummyArgList)) {
         // MATCHED DummyArgList
         SubroutineStmt->setDummyArgList(DummyArgList.newDummyArgList());
      } // Optional

      if (ofp_traverse_ProcLanguageBindingSpec(ProcLanguageBindingSpec.term, &ProcLanguageBindingSpec)) {
         // MATCHED ProcLanguageBindingSpec
         SubroutineStmt->setProcLanguageBindingSpec(ProcLanguageBindingSpec.newProcLanguageBindingSpec());
      } // Optional

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         SubroutineStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

      ast->build_SubroutineStmt(SubroutineStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1235 dummy-arg
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_DummyArg(ATerm term, OFP::DummyArg* DummyArg)
{
#ifdef DEBUG_PRINT
   printf("DummyArg: %s\n", ATwriteToString(term));
#endif

 if (ATmatch(term, "DummyArg_STAR")) {

   // MATCHED DummyArg_STAR

   return ATtrue;
 }

 OFP::DummyArgName DummyArgName;
 if (ATmatch(term, "DummyArg_DAN(<term>)", &DummyArgName.term)) {

      if (ofp_traverse_DummyArgName(DummyArgName.term, &DummyArgName)) {
         // MATCHED DummyArgName
      } else return ATfalse;

   // MATCHED DummyArg_DAN

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DummyArgList(ATerm term, OFP::DummyArgList* DummyArgList)
{
#ifdef DEBUG_PRINT
   printf("DummyArgList: %s\n", ATwriteToString(term));
#endif

 OFP::DummyArg DummyArg;
 if (ATmatch(term, "DummyArgList(<term>)", &DummyArg.term)) {

   ATermList DummyArg_tail = (ATermList) ATmake("<term>", DummyArg.term);
   while (! ATisEmpty(DummyArg_tail)) {
      DummyArg.term = ATgetFirst(DummyArg_tail);
      DummyArg_tail = ATgetNext (DummyArg_tail);
      if (ofp_traverse_DummyArg(DummyArg.term, &DummyArg)) {
         // MATCHED DummyArg
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1236 end-subroutine-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndSubroutineStmt(ATerm term, OFP::EndSubroutineStmt* EndSubroutineStmt)
{
#ifdef DEBUG_PRINT
   printf("EndSubroutineStmt: %s\n", ATwriteToString(term));
#endif

   OFP::Label Label;
   OFP::Name SubroutineName;
   OFP::EOS EOS;
   if (ATmatch(term, "EndSubroutineStmt(<term>,<term>,<term>)", &Label.term, &SubroutineName.term, &EOS.term)) {

      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         EndSubroutineStmt->setLabel(Label.newLabel());
      } // Optional

      if (ofp_traverse_Name(SubroutineName.term, &SubroutineName)) {
         // MATCHED SubroutineName
         EndSubroutineStmt->setSubroutineName(SubroutineName.newName());
      } // Optional

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         EndSubroutineStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

      ast->build_EndSubroutineStmt(EndSubroutineStmt);

      return ATtrue;
   }

   return ATfalse;
}

//========================================================================================
// R1237 separate-module-subprogram
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_SeparateModuleSubprogram(ATerm term, OFP::SeparateModuleSubprogram* SeparateModuleSubprogram)
{
#ifdef DEBUG_PRINT
   printf("SeparateModuleSubprogram: %s\n", ATwriteToString(term));
#endif

 OFP::MpSubprogramStmt MpSubprogramStmt;
 OFP::InitialSpecPart InitialSpecPart;
 OFP::SpecAndExecPart SpecAndExecPart;
 OFP::InternalSubprogramPart InternalSubprogramPart;
 OFP::EndMpSubprogramStmt EndMpSubprogramStmt;
 if (ATmatch(term, "SeparateModuleSubprogram(<term>,<term>,<term>,<term>,<term>)", &MpSubprogramStmt.term, &InitialSpecPart.term, &SpecAndExecPart.term, &InternalSubprogramPart.term, &EndMpSubprogramStmt.term)) {

      if (ofp_traverse_MpSubprogramStmt(MpSubprogramStmt.term, &MpSubprogramStmt)) {
         // MATCHED MpSubprogramStmt
         SeparateModuleSubprogram->setMpSubprogramStmt(MpSubprogramStmt.newMpSubprogramStmt());
      } else return ATfalse;

      if (ofp_traverse_InitialSpecPart(InitialSpecPart.term, &InitialSpecPart)) {
         // MATCHED InitialSpecPart
         SeparateModuleSubprogram->setInitialSpecPart(InitialSpecPart.newInitialSpecPart());
      } else return ATfalse;

      if (ofp_traverse_SpecAndExecPart(SpecAndExecPart.term, &SpecAndExecPart)) {
         // MATCHED SpecAndExecPart
         SeparateModuleSubprogram->setSpecAndExecPart(SpecAndExecPart.newSpecAndExecPart());
      } else return ATfalse;

   if (ATmatch(InternalSubprogramPart.term, "Some(<term>)", &InternalSubprogramPart.term)) {
      if (ofp_traverse_InternalSubprogramPart(InternalSubprogramPart.term, &InternalSubprogramPart)) {
         // MATCHED InternalSubprogramPart
         SeparateModuleSubprogram->setInternalSubprogramPart(InternalSubprogramPart.newInternalSubprogramPart());
      } else return ATfalse;
   }

      if (ofp_traverse_EndMpSubprogramStmt(EndMpSubprogramStmt.term, &EndMpSubprogramStmt)) {
         // MATCHED EndMpSubprogramStmt
         SeparateModuleSubprogram->setEndMpSubprogramStmt(EndMpSubprogramStmt.newEndMpSubprogramStmt());
      } else return ATfalse;

   ast->build_SeparateModuleSubprogram(SeparateModuleSubprogram);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1238 mp-subprogram-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_MpSubprogramStmt(ATerm term, OFP::MpSubprogramStmt* MpSubprogramStmt)
{
#ifdef DEBUG_PRINT
   printf("MpSubprogramStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name ProcedureName;
 OFP::EOS EOS;
 if (ATmatch(term, "MpSubprogramStmt(<term>,<term>,<term>)", &Label.term, &ProcedureName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
   if (ATmatch(Label.term, "(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         MpSubprogramStmt->setLabel(Label.newLabel());
      } else return ATfalse;
   }
   }

      if (ofp_traverse_Name(ProcedureName.term, &ProcedureName)) {
         // MATCHED Name
         MpSubprogramStmt->setProcedureName(ProcedureName.newName());
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         MpSubprogramStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

   ast->build_MpSubprogramStmt(MpSubprogramStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1239 end-mp-subprogram-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EndMpSubprogramStmt(ATerm term, OFP::EndMpSubprogramStmt* EndMpSubprogramStmt)
{
#ifdef DEBUG_PRINT
   printf("EndMpSubprogramStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name ProcedureName;
 OFP::EOS EOS;
 if (ATmatch(term, "EndMpSubprogramStmt(<term>,<term>,<term>)", &Label.term, &ProcedureName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
   if (ATmatch(Label.term, "(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         EndMpSubprogramStmt->setLabel(Label.newLabel());
      } else return ATfalse;
   }
   }

   if (ATmatch(ProcedureName.term, "Some(<term>)", &ProcedureName.term)) {
   if (ATmatch(ProcedureName.term, "(Some(<term>))", &ProcedureName.term)) {
      if (ofp_traverse_Name(ProcedureName.term, &ProcedureName)) {
         // MATCHED Name
         EndMpSubprogramStmt->setProcedureName(ProcedureName.newName());
      } else return ATfalse;
   }
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         EndMpSubprogramStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

   ast->build_EndMpSubprogramStmt(EndMpSubprogramStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1240 entry-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_EntryStmt(ATerm term, OFP::EntryStmt* EntryStmt)
{
#ifdef DEBUG_PRINT
   printf("EntryStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name EntryName;
 OFP::EOS EOS;
 if (ATmatch(term, "EntryStmt(<term>,<term>,<term>)", &Label.term, &EntryName.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_Name(EntryName.term, &EntryName)) {
         // MATCHED Name
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 OFP::Label Label1;
 OFP::Name EntryName1;
 OFP::DummyArgList DummyArgList;
 OFP::Suffix Suffix;
 OFP::EOS EOS1;
 if (ATmatch(term, "EntryStmt_DAL(<term>,<term>,<term>,<term>,<term>)", &Label1.term, &EntryName1.term, &DummyArgList.term, &Suffix.term, &EOS1.term)) {

   if (ATmatch(Label1.term, "Some(<term>)", &Label1.term)) {
      if (ofp_traverse_Label(Label1.term, &Label1)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_Name(EntryName1.term, &EntryName1)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(DummyArgList.term, "Some(<term>)", &DummyArgList.term)) {
      if (ofp_traverse_DummyArgList(DummyArgList.term, &DummyArgList)) {
         // MATCHED DummyArgList
      } else return ATfalse;
   }

   if (ATmatch(Suffix.term, "Some(<term>)", &Suffix.term)) {
      if (ofp_traverse_Suffix(Suffix.term, &Suffix)) {
         // MATCHED Suffix
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS1.term, &EOS1)) {
         // MATCHED EOS
      } else return ATfalse;

   // MATCHED EntryStmt_DAL

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1241 return-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ReturnStmt(ATerm term, OFP::ReturnStmt* ReturnStmt)
{
#ifdef DEBUG_PRINT
   printf("ReturnStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Expr Expr;
 OFP::EOS EOS;
 if (ATmatch(term, "ReturnStmt(<term>,<term>,<term>)", &Label.term, &Expr.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

   if (ATmatch(Expr.term, "Some(<term>)", &Expr.term)) {
      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1242 contains-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_ContainsStmt(ATerm term, OFP::ContainsStmt* ContainsStmt)
{
#ifdef DEBUG_PRINT
   printf("ContainsStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::EOS EOS;
 if (ATmatch(term, "ContainsStmt(<term>,<term>)", &Label.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
         ContainsStmt->setLabel(Label.newLabel());
      } else return ATfalse;
   }

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
         ContainsStmt->setEOS(EOS.newEOS());
      } else return ATfalse;

   ast->build_ContainsStmt(ContainsStmt);

   return ATtrue;
 }

 return ATfalse;
}

//========================================================================================
// R1243 stmt-function-stmt
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_StmtFunctionStmt(ATerm term, OFP::StmtFunctionStmt* StmtFunctionStmt)
{
#ifdef DEBUG_PRINT
   printf("StmtFunctionStmt: %s\n", ATwriteToString(term));
#endif

 OFP::Label Label;
 OFP::Name FunctionName;
 OFP::DummyArgNameList DummyArgNameList;
 OFP::Expr Expr;
 OFP::EOS EOS;
 if (ATmatch(term, "StmtFunctionStmt(<term>,<term>,<term>,<term>,<term>)", &Label.term, &FunctionName.term, &DummyArgNameList.term, &Expr.term, &EOS.term)) {

   if (ATmatch(Label.term, "Some(<term>)", &Label.term)) {
      if (ofp_traverse_Label(Label.term, &Label)) {
         // MATCHED Label
      } else return ATfalse;
   }

      if (ofp_traverse_Name(FunctionName.term, &FunctionName)) {
         // MATCHED Name
      } else return ATfalse;

   if (ATmatch(DummyArgNameList.term, "Some(<term>)", &DummyArgNameList.term)) {
      if (ofp_traverse_DummyArgNameList(DummyArgNameList.term, &DummyArgNameList)) {
         // MATCHED DummyArgNameList
      } else return ATfalse;
   }

      if (ofp_traverse_Expr(Expr.term, &Expr)) {
         // MATCHED Expr
      } else return ATfalse;

      if (ofp_traverse_EOS(EOS.term, &EOS)) {
         // MATCHED EOS
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}


/** Identifier aliases
 */

ATbool ofp_traverse_Name(ATerm term, OFP::Name* Name)
{
 OFP::Ident Ident;
 if (ofp_traverse_Ident(term, &Ident)) {
    // MATCHED Ident
    Name->setIdent(Ident.newIdent());
    Name->inheritPayload(Name->getIdent());
 } else return ATfalse;

 return ATtrue;
}

#ifdef OBSOLETE
ATbool ofp_traverse_AncestorModuleName(ATerm term, OFP::AncestorModuleName* AncestorModuleName)
{
#ifdef DEBUG_PRINT
   printf("AncestorModuleName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "AncestorModuleName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         AncestorModuleName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ArgName(ATerm term, OFP::ArgName* ArgName)
{
#ifdef DEBUG_PRINT
   printf("ArgName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ArgName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ArgName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ArrayName(ATerm term, OFP::ArrayName* ArrayName)
{
#ifdef DEBUG_PRINT
   printf("ArrayName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ArrayName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ArrayName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AssociateConstructName(ATerm term, OFP::AssociateConstructName* AssociateConstructName)
{
#ifdef DEBUG_PRINT
   printf("AssociateConstructName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "AssociateConstructName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         AssociateConstructName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_AssociateName(ATerm term, OFP::AssociateName* AssociateName)
{
#ifdef DEBUG_PRINT
   printf("AssociateName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "AssociateName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         AssociateName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_BindingName(ATerm term, OFP::BindingName* BindingName)
{
#ifdef DEBUG_PRINT
   printf("BindingName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "BindingName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         BindingName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_BlockConstructName(ATerm term, OFP::BlockConstructName* BlockConstructName)
{
#ifdef DEBUG_PRINT
   printf("BlockConstructName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "BlockConstructName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         BlockConstructName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_BlockDataName(ATerm term, OFP::BlockDataName* BlockDataName)
{
#ifdef DEBUG_PRINT
   printf("BlockDataName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "BlockDataName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         BlockDataName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CaseConstructName(ATerm term, OFP::CaseConstructName* CaseConstructName)
{
#ifdef DEBUG_PRINT
   printf("CaseConstructName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "CaseConstructName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         CaseConstructName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CoarrayName(ATerm term, OFP::CoarrayName* CoarrayName)
{
#ifdef DEBUG_PRINT
   printf("CoarrayName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "CoarrayName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         CoarrayName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CommonBlockName(ATerm term, OFP::CommonBlockName* CommonBlockName)
{
#ifdef DEBUG_PRINT
   printf("CommonBlockName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "CommonBlockName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         CommonBlockName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ComponentName(ATerm term, OFP::ComponentName* ComponentName)
{
#ifdef DEBUG_PRINT
   printf("ComponentName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ComponentName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ComponentName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ConstructName(ATerm term, OFP::ConstructName* ConstructName)
{
#ifdef DEBUG_PRINT
   printf("ConstructName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ConstructName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ConstructName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_CriticalConstructName(ATerm term, OFP::CriticalConstructName* CriticalConstructName)
{
#ifdef DEBUG_PRINT
   printf("CriticalConstructName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "CriticalConstructName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         CriticalConstructName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DataPointerComponentName(ATerm term, OFP::DataPointerComponentName* DataPointerComponentName)
{
#ifdef DEBUG_PRINT
   printf("DataPointerComponentName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "DataPointerComponentName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         DataPointerComponentName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_DoConstructName(ATerm term, OFP::DoConstructName* DoConstructName)
{
#ifdef DEBUG_PRINT
   printf("DoConstructName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "DoConstructName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         DoConstructName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_EntityName(ATerm term, OFP::EntityName* EntityName)
{
#ifdef DEBUG_PRINT
   printf("EntityName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "EntityName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         EntityName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_EntryName(ATerm term, OFP::EntryName* EntryName)
{
#ifdef DEBUG_PRINT
   printf("EntryName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "EntryName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         EntryName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ExternalName(ATerm term, OFP::ExternalName* ExternalName)
{
#ifdef DEBUG_PRINT
   printf("ExternalName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ExternalName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ExternalName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_FinalSubroutineName(ATerm term, OFP::FinalSubroutineName* FinalSubroutineName)
{
#ifdef DEBUG_PRINT
   printf("FinalSubroutineName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "FinalSubroutineName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         FinalSubroutineName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ForallConstructName(ATerm term, OFP::ForallConstructName* ForallConstructName)
{
#ifdef DEBUG_PRINT
   printf("ForallConstructName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ForallConstructName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ForallConstructName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_FunctionName(ATerm term, OFP::FunctionName* FunctionName)
{
#ifdef DEBUG_PRINT
   printf("FunctionName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "FunctionName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         FunctionName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_GenericName(ATerm term, OFP::GenericName* GenericName)
{
#ifdef DEBUG_PRINT
   printf("GenericName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "GenericName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         GenericName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_IfConstructName(ATerm term, OFP::IfConstructName* IfConstructName)
{
#ifdef DEBUG_PRINT
   printf("IfConstructName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "IfConstructName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         IfConstructName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ImportName(ATerm term, OFP::ImportName* ImportName)
{
#ifdef DEBUG_PRINT
   printf("ImportName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ImportName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ImportName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_IndexName(ATerm term, OFP::IndexName* IndexName)
{
#ifdef DEBUG_PRINT
   printf("IndexName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "IndexName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         IndexName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_IntrinsicProcedureName(ATerm term, OFP::IntrinsicProcedureName* IntrinsicProcedureName)
{
#ifdef DEBUG_PRINT
   printf("IntrinsicProcedureName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "IntrinsicProcedureName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         IntrinsicProcedureName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_LocalName(ATerm term, OFP::LocalName* LocalName)
{
#ifdef DEBUG_PRINT
   printf("LocalName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "LocalName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         LocalName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ModuleName(ATerm term, OFP::ModuleName* ModuleName)
{
#ifdef DEBUG_PRINT
   printf("ModuleName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ModuleName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ModuleName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_NamelistGroupName(ATerm term, OFP::NamelistGroupName* NamelistGroupName)
{
#ifdef DEBUG_PRINT
   printf("NamelistGroupName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "NamelistGroupName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         NamelistGroupName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ObjectName(ATerm term, OFP::Name* ObjectName)
{
#ifdef DEBUG_PRINT
   printf("ObjectName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ObjectName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ObjectName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ObjectName(ATerm term, OFP::ObjectName* ObjectName)
{
#ifdef DEBUG_PRINT
   printf("ObjectName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ObjectName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ObjectName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ParentSubmoduleName(ATerm term, OFP::ParentSubmoduleName* ParentSubmoduleName)
{
#ifdef DEBUG_PRINT
   printf("ParentSubmoduleName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ParentSubmoduleName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ParentSubmoduleName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ParentTypeName(ATerm term, OFP::ParentTypeName* ParentTypeName)
{
#ifdef DEBUG_PRINT
   printf("ParentTypeName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ParentTypeName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ParentTypeName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_PartName(ATerm term, OFP::PartName* PartName)
{
#ifdef DEBUG_PRINT
   printf("PartName(F): %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "PartName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         PartName->setIdent(Ident.newIdent());
         PartName->inheritPayload(PartName->getIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ProcedureComponentName(ATerm term, OFP::ProcedureComponentName* ProcedureComponentName)
{
#ifdef DEBUG_PRINT
   printf("ProcedureComponentName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ProcedureComponentName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ProcedureComponentName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ProcedureEntityName(ATerm term, OFP::ProcedureEntityName* ProcedureEntityName)
{
#ifdef DEBUG_PRINT
   printf("ProcedureEntityName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ProcedureEntityName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ProcedureEntityName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ProcedureName(ATerm term, OFP::ProcedureName* ProcedureName)
{
#ifdef DEBUG_PRINT
   printf("ProcedureName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ProcedureName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ProcedureName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ProcEntityName(ATerm term, OFP::ProcEntityName* ProcEntityName)
{
#ifdef DEBUG_PRINT
   printf("ProcEntityName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ProcEntityName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ProcEntityName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ResultName(ATerm term, OFP::ResultName* ResultName)
{
#ifdef DEBUG_PRINT
   printf("ResultName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ResultName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ResultName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ScalarIntConstantName(ATerm term, OFP::ScalarIntConstantName* ScalarIntConstantName)
{
#ifdef DEBUG_PRINT
   printf("ScalarIntConstantName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ScalarIntConstantName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ScalarIntConstantName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ScalarIntVariableName(ATerm term, OFP::ScalarIntVariableName* ScalarIntVariableName)
{
#ifdef DEBUG_PRINT
   printf("ScalarIntVariableName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ScalarIntVariableName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ScalarIntVariableName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_ScalarVariableName(ATerm term, OFP::ScalarVariableName* ScalarVariableName)
{
#ifdef DEBUG_PRINT
   printf("ScalarVariableName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "ScalarVariableName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         ScalarVariableName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_SelectConstructName(ATerm term, OFP::SelectConstructName* SelectConstructName)
{
#ifdef DEBUG_PRINT
   printf("SelectConstructName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "SelectConstructName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         SelectConstructName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_SubmoduleName(ATerm term, OFP::SubmoduleName* SubmoduleName)
{
#ifdef DEBUG_PRINT
   printf("SubmoduleName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "SubmoduleName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         SubmoduleName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

#ifdef OBSOLETE
ATbool ofp_traverse_SubroutineName(ATerm term, OFP::SubroutineName* SubroutineName)
{
#ifdef DEBUG_PRINT
   printf("SubroutineName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "SubroutineName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         SubroutineName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}
#endif

ATbool ofp_traverse_TypeName(ATerm term, OFP::TypeName* TypeName)
{
#ifdef DEBUG_PRINT
   printf("TypeName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "TypeName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         TypeName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_TypeParamName(ATerm term, OFP::TypeParamName* TypeParamName)
{
#ifdef DEBUG_PRINT
   printf("TypeParamName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "TypeParamName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         TypeParamName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_UseName(ATerm term, OFP::UseName* UseName)
{
#ifdef DEBUG_PRINT
   printf("UseName: %s\n", ATwriteToString(term));
#endif

 OFP::Ident Ident;
 if (ATmatch(term, "UseName(<term>)", &Ident.term)) {

      if (ofp_traverse_Ident(Ident.term, &Ident)) {
         // MATCHED Ident
         UseName->setIdent(Ident.newIdent());
      } else return ATfalse;

   return ATtrue;
 }

 return ATfalse;
}
#endif


/** Lists
 */

#ifdef OBSOLETE
ATbool ofp_traverse_ExternalNameList(ATerm term, OFP::ExternalNameList* ExternalNameList)
{
#ifdef DEBUG_PRINT
   printf("ExternalNameList: %s\n", ATwriteToString(term));
#endif

 OFP::ExternalName ExternalName;
 if (ATmatch(term, "ExternalNameList(<term>)", &ExternalName.term)) {

   ATermList ExternalName_tail = (ATermList) ATmake("<term>", ExternalName.term);
   while (! ATisEmpty(ExternalName_tail)) {
      ExternalName.term = ATgetFirst(ExternalName_tail);
      ExternalName_tail = ATgetNext (ExternalName_tail);
      if (ofp_traverse_ExternalName(ExternalName.term, &ExternalName)) {
         // MATCHED ExternalName
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}

ATbool ofp_traverse_LabelList(ATerm term, OFP::LabelList* LabelList)
{
#ifdef DEBUG_PRINT
   printf("LabelList: %s\n", ATwriteToString(term));
#endif

 OFP::LblRef LblRef;
 if (ATmatch(term, "LabelList(<term>)", &LblRef.term)) {

   ATermList LblRef_tail = (ATermList) ATmake("<term>", LblRef.term);
   while (! ATisEmpty(LblRef_tail)) {
      LblRef.term = ATgetFirst(LblRef_tail);
      LblRef_tail = ATgetNext (LblRef_tail);
      if (ofp_traverse_LblRef(LblRef.term, &LblRef)) {
         // MATCHED LblRef
      } else return ATfalse;
   }

   return ATtrue;
 }

 return ATfalse;
}
#endif

#ifdef __cplusplus
//} /* closing brace for extern "C" */
#endif
