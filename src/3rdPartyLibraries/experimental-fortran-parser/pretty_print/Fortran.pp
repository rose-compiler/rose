[
   Comment                       -- _1,

   eos                           -- _1,

   no-label                      -- ,
   no-name                       -- ,

   append-colon                  -- _1 KW[": "],

   opt-list                      -- KW[", "] _1,
   opt-list.1:iter-sep           -- _1 ", ",

   CONCURRENT                    -- "CONCURRENT",
   Halo                          -- ", " "HALO" "(" _1 ")",
   HaloAlloc                     -- ", " "HALO" "=" _1,
   HaloExplicitShapeSpec         -- _1 ":*:" _2,
   AssumedShape                  -- ":",
   ConcurrentExecControl         -- " " "CONCURRENT" _1 _2,
   ImageExecStmt                 -- H hs=1[_1 "CALL" H hs=0[_2 "("_3")"] _4 _5],
   ImageExecStmt.3:iter-sep      -- _1 ",",
   ImageExecutionSelector        -- " [[" _1 "]]",

   CoAllocateStmt                -- H hs=1[_1 H hs=0[KW["ALLOCATE"] "("_2 _3 _4")" _5 _6]],
   CoAllocateStmt.3:iter-sep     -- _1 ",",
   CoAllocateStmt.4:iter-sep     -- _1 ", ",
   CoDeallocateStmt              -- H hs=0[_1 KW["DEALLOCATE"] "("_2 _3")" _4 _5],
   CoDeallocateStmt.2:iter-sep   -- _1 ",",

   clMemObject                   -- "clMemObject(" _1 ")",

   INTRINSIC                     -- "INTRINSIC" " :: ",
   NON_INTRINSIC                 -- "NON_INTRINSIC" " :: ",
   PARAMETER                     -- ", " "PARAMETER",

   #R201 --,
   Program                       -- V vs=0 is=0 [_1],
   Program.1:opt                 -- ,

   #R204 --,
   SpecPart                      -- V vs=0 is=0[_1],
   ExecPart                      -- V vs=0 is=0[_1],
   FuncPart                      -- V ["\nCONTAINS\n" _1],

   #R303 --,
   OfpName                       -- H hs=1[_1],

   #R305 --,
   IntLiteralConstant            -- _1 _2,

   #R312 --,
   Label                         -- _1,

   #R401 --,
   COLON                         -- ":",

   #R403 --,
   IntrinsicType                 -- _1,

   #R404 --,
   Type                          -- H hs=1[_1 _2],
   INTEGER                       -- "INTEGER",
   INTEGER                       -- "INTEGER" _1,
   REAL                          -- "REAL",
   REAL                          -- "REAL" _1,
   DOUBLEPRECISION               -- "DOUBLE PRECISION",
   LOGICAL                       -- "LOGICAL",
   LOGICAL                       -- "LOGICAL" _1,
   COMPLEX                       -- "COMPLEX",
   COMPLEX                       -- "COMPLEX" _1,
   DOUBLECOMPLEX                 -- "DOUBLE COMPLEX",

   CHARACTER                     -- "CHARACTER",
   CHARACTER                     -- "CHARACTER" "(" _1 ")",
   CHARACTER                     -- "CHARACTER" "(" _1 "," _2 ")",
   ppCharNoLen                   -- "CHARACTER" "(" "KIND=" _1 ")",
   no-length-selector            --,

   ppArrayType                   -- "("_1")",
   ppArrayType                   -- "("_1")" "["_2"]",
   ppArrayType.1:iter-sep        -- _1 ",",
   ppArrayType.2:iter-sep        -- _1 ",",

   ppCoarrayType                 -- "("_1")",
   ppCoarrayType.1:iter-sep      -- _1 ",",

   ppRange                       -- _1,
   Range                         -- _1 ":" _2,
   no-lower-bound                -- ,

   #R402 --,
   no-type-spec                  -- ,

   Class                         -- _1 "CLASS" "(" _2" )",

   KIND                          -- "KIND",
   LEN                           -- "LEN",

   Kind                          -- H hs=0 ["(" "KIND=" _1 ")"],
   ppLiteralKind                 -- H hs=0 ["_" _1],
   no-kind                       -- ,
   
   IntVal                        -- _1,
   IntVal                        -- H hs=0[_1 _2],
   RealVal                       -- _1,
   RealVal                       -- H hs=0[_1 _2],

   #R421 --,
   LengthSelector                -- "(" KW["LEN="] _1")",

   #R423 --,
   CharLiteralConstant           -- _1 _2,

   #R424 --,
   TRUE                          -- ".TRUE.",
   FALSE                         -- ".FALSE.",
   TRUE                          -- H hs=0[".TRUE."  "_" _1],
   FALSE                         -- H hs=0[".FALSE." "_" _1],

   #R425 --,
   DerivedTypeDef                -- V vs=0 is=2[_1 _2 _3 _4] _5 _6,

   DerivedTypeStmt               -- H hs=1[_1 "TYPE," H hs=0[_2 " :: " _3 "(" _4 ")"]],
   DerivedTypeStmt.2:iter-sep    -- _1 ", ",
   DerivedTypeStmt.4:iter-sep    -- _1 ",",

   ppDerivedTypeStmt             -- H hs=1[_1 "TYPE" _2],

   ppDerivedTypeStmt_attrs              -- H hs=1[_1 "TYPE," H hs=0[_2 " :: " _3]],
   ppDerivedTypeStmt_attrs.2:iter-sep   -- _1 ", ",

   ppDerivedTypeStmt_params             -- H hs=1[_1 "TYPE" H hs=0[":: " _2 "(" _3 ")"]],
   ppDerivedTypeStmt_params.3:iter-sep  -- _1 ",",


   EndTypeStmt                   -- H hs=1[_1 H hs=0["END TYPE"] _2],

   #R427 --,
   ABSTRACT                      -- "ABSTRACT",
   Extends                       -- "EXTENDS" "(" _1 ")",
   BIND                          -- "BIND(C)",

   #R431 --,
   TypeParamDefStmt              -- _1 H hs=0 ["INTEGER" H hs=0 [_2 ", " _3 " :: " _4]],
   TypeParamDefStmt.4:iter-sep   -- _1 ", ",

   TypeParamDecl                 -- _1 " = " _2,

   DataComponentDefStmt          -- H hs=1[_1 H hs=0[_2 _3 " :: " _4]],
   DataComponentDefStmt.4:iter-sep  -- _1 ", ",
   no-type-bound-procedure-part     --,

   #R437 --,
   Codimension                   -- ", " KW["CODIMENSION"] "["_1"]",
   Codimension.1:iter-sep        -- _1 ",",

   #R438 --,
   ComponentDecl                 -- _1 _2 _3 _4 _5,

   #R440 --,
   ProcComponentDefStmt          -- H hs=1[_1 KW["PROCEDURE"] H hs=0["("_2")" KW[", "] _3 KW[" :: "] _4 _5]],
   ProcComponentDefStmt.3:iter-sep   -- _1 ", ",

   #R441 --,
   POINTER                       -- "POINTER",
   PASS                          -- "PASS",

   #R442 --,
   ComponentInit                 -- _1,

   #R445 --,
   TypeBoundProcPart             -- "CONTAINS" _1 _2,

   BindingPrivateStmt            -- _1 "PRIVATE",
   no-binding-private-stmt       --,

   TypeBoundProcedureStmt             -- H hs=1[H hs=0[_1 "PROCEDURE" _2 _3 _4]],
   TypeBoundProcedureInterfaceStmt    -- H hs=1[H hs=0[_1 "PROCEDURE" "("_2")" _3 _4 _5]],
   TypeBoundProcedureInterfaceStmt.3:iter-sep  -- _1 ", ",
   TypeBoundProcedureInterfaceStmt.4:iter-sep  -- _1 ", ",

   TypeBoundProcDecl             -- _1 " => " _2,

   #R451 --,
   Pass                          -- KW["PASS"] _1,
   NON_OVERRIDABLE               -- KW["NON_OVERRIDABLE"],
   DEFERRED                      -- KW["DEFERRED"],

   #R452 --,
   FinalProcedureStmt            -- H hs=1[_1 "FINAL ::" H hs=0[_2]],
   FinalProcedureStmt.2:iter-sep -- _1 ", ",

   #R453 --,
   DerivedTypeSpec               -- _1 _2,
   DerivedType                   -- "TYPE" "("_1")" _2,

   #R454 --,
   TypeParamSpec                 -- _1 KW["="] "("_2")",

   #R455 --,
   StructureConstructor          -- _1 "("_2")",
%%   StructureConstructor.2:iter-sep   -- _1 ", ",

   #R458 --,
   EnumDef                       -- _1 _2 _3,

   #R459 --,
   EnumDefStmt                   -- H hs=1[_1 KW["ENUM, "] H hs=0[KW["BIND(C)"] _2]],

   #R460 --,
   EnumeratorDefStmt             -- H hs=1[_1 KW["ENUMERATOR ::"] H hs=0[_2 _3]],
   EnumeratorDefStmt.2:iter-sep  -- _1 ", ",

   #R461 --,
   Enumerator                    -- _1 KW[" = "] _2,

   #R462 --,
   EndEnumStmt                   -- H hs=1[_1 KW["END ENUM"] _2],

   #R468 --,
   ArrayConstructor              -- H hs=0["[" _1 "]"],

   #R469 --,
   AcSpec                        -- _1 _2,
   AcSpec.2:iter-sep             -- _1 ",",

   #R473 --,
   AcImpliedDo                   -- "("_1 KW[", "] _2")",

   #R474 --,
   AcImpliedDoControl            -- _1 KW["="] _2 KW[", "] _3 _4,

   #R501 --,
   TypeDeclarationStmt             -- H hs=0 [_1 H hs=0[_2] H hs=0[_3 " :: "] H hs=0[_4]],
   TypeDeclarationStmt.4:iter-sep  -- _1 ", ",

   #R502 --,
   ALLOCATABLE                   -- ", " KW["ALLOCATABLE"],
   Intent                        -- ", " KW["INTENT"]    "("_1")",
   Dimension                     -- ", " KW["DIMENSION"] "("_1")",
   Dimension.1:iter-sep          -- _1 ",",

   IN                            -- KW["IN"],
   OUT                           -- KW["OUT"],
   INOUT                         -- KW["INOUT"],

   #R503 --,
   ppVar                         -- _1 _2,
   ppArrayVar                    -- _1 _2 _3,

   #R505 --,
   Initialization                -- KW[" = "] _1,
   Init                          -- KW[" = "] _1,
   no-init                       -- ,

   #R507 --,
   PRIVATE                       -- KW["PRIVATE"],
   PUBLIC                        -- KW["PUBLIC"],
   
   #R508 --, 
   LanguageBinding               -- " BIND(C" _1 ")",
   BindingName                   -- ",NAME=" _1,
   no-language-binding           -- ,
   no-binding-name               -- ,
   
   #R511 --,
   OfpExplicitCoshape            -- _1,

   #R519 --,
   OfpAssumedOrDeferredShape     -- _1,

   #R523 --,
   IN                            -- KW["IN"],

   #R524 --,
   AccessStmt                    -- H hs=1[_1 _2 H hs=0[_3]],
   AccessStmt.3:iter-sep         -- _1 ", ",

   #R526 --,
   AllocatableStmt               -- H hs=1[_1 "ALLOCATABLE ::" H hs=0[_2]],
   AllocatableStmt.2:iter-sep    -- _1 ", ",

   #R529 --,
   BindStmt                      -- H hs=1[_1 H hs=0[_2 " :: " _3 _4]],
   BindStmt.3:iter-sep           -- _1 ", ",

   #R531 --,
   CodimensionStmt               -- H hs=1[_1 "CODIMENSION ::" H hs=0[_2 _3]],
   CodimensionStmt.2:iter-sep    -- _1 ", ",

   #R532 --,
   CodimensionDecl               -- _1 "["_2"]",
   CodimensionDecl.2:iter-sep    -- _1 ", ",

   #R534 --,
   DataStmt                      -- H hs=1[_1 KW["DATA"] H hs=0[_2 _3]],

   #R535 --,
   DataStmtSet                   -- H hs=1[H hs=0[_1] H hs=0["/"_2"/"]],
   DataStmtSet.1:iter-sep        -- _1 ", ",
   DataStmtSet.2:iter-sep        -- _1 ",",

   #R537 --,
   DataImpliedDo                 -- "("_1 KW[", "] _2 KW["="] _3 KW[","] _4 _5")",
   DataImpliedDo.1:iter-sep      -- _1 ", ",

   #R539 --,
   DataIDoVariable               -- _1,

   #R540 --,
   DataStmtValue                 -- _1 KW["*"] _2,

   #R553 --,
   SaveStmt                      -- H hs=1[_1 KW["SAVE ::"] H hs=0[_2 _3]],
   SaveStmt.2:iter-sep           -- _1 ", ",

   #R554 --,
   SavedEntity                   -- _1,
   SavedEntity_CBN               -- _1,

   #R556 --,
   TargetStmt                    -- H hs=1[_1 KW["TARGET ::"] H hs=0[_2 _3]],
   TargetStmt.2:iter-sep         -- _1 ", ",

   #R557 --,
   TargetDecl                    -- _1 "("_2")" "["_3"]",
   TargetDecl.2:iter-sep         -- _1 ", ",
   TargetDecl.3:iter-sep         -- _1 ", ",

   #R559 --,
   VolatileStmt                  -- H hs=1[_1 KW["VOLATILE ::"] H hs=0[_2]],
   VolatileStmt.2:iter-sep       -- _1 ", ",

   #R560 --,
   ppImplicitStmt                -- H hs=1[_1 KW["IMPLICIT NONE"]],
   ImplicitStmt                  -- H hs=1[_1 KW["IMPLICIT"] H hs=0[_2 _3]],
   ImplicitStmt.2:iter-sep       -- _1 ", ",

   #R561 --,
   ImplicitSpec                  -- _1 "("_2")",
   ImplicitSpec.2:iter-sep       -- _1 ", ",

   #R562 --,
   LetterSpec                    -- _1 _2,
   ppLetterSpec                  -- _1,
   ppLetterSpec                  -- _1 KW["-"] _2,

   #R563 --,
   NamelistStmt                  -- H hs=1[_1 KW["NAMELIST"] H hs=0[_2] _3],
   NamelistEntry                 -- "/"_1"/ " _2,
   NamelistEntry.2:iter-sep      -- _1 ", ",

   #R565 --,
   EquivalenceStmt               -- H hs=1[_1 KW["EQUIVALENCE"] H hs=0[_2 _3]],
   EquivalenceStmt.2:iter-sep    -- _1 ", ",

   #R566 --, 
   EquivalenceSet                -- "("_1")",
   EquivalenceSet.1:iter-sep     -- _1 ", ",

   #R568 --,
   CommonStmt                    -- H hs=1[_1 KW["COMMON"] H hs=0[ _2 _3]],
   CommonBlockEntry              -- _1 _2,
   CommonBlockName               -- "/"_1"/",

   #R569 --,
   CommonBlockObject             -- _1 _2,

   #R611 --,
   DataRef                       -- H hs=0 [_1],
   DataRef.1:iter-sep            -- _1 "%",

   #R612 --,
   VarDef                           -- _1,
   VarRef                           -- _1,
   ppPartRef                        -- H hs=0[_1 _2 _3],
   ppSectionSubscripts              -- "(" _1 ")",
   ppSectionSubscripts.1:iter-sep   -- _1 ",",
   ppImageSelector                  -- "[" _1 "]",
   ppImageSelector.1:iter-sep       -- _1 ",",
   no-section-subscripts            -- ,
   no-image-selector                -- ,

   ppTriplet                        -- _1 ":" _2 _3 _4,
   no-subscript                     -- ,
   no-stride                        -- ,

   #R626 --,
   ppAllocateStmt                -- H hs=1 [_1 H hs=0["ALLOCATE" "("_2 _3         ")"]],
   ppAllocateStmt                -- H hs=1 [_1 H hs=0["ALLOCATE" "("_2 _3 ", " _4 ")"]],
   ppAllocateStmt.3:iter-sep     -- _1 ",",
   ppAllocateStmt.4:iter-sep     -- _1 ", ",

   ppTypeSpec                    -- _1 ":: ",

   ERRMSG                        -- "ERRMSG=" _1,
   MOLD                          --           _1,
   SOURCE                        -- "SOURCE=" _1,
   STAT                          -- "STAT="   _1,

   ppDeallocateStmt              -- H hs=1 [_1 H hs=0["DEALLOCATE" "("_2         ")"]],
   ppDeallocateStmt              -- H hs=1 [_1 H hs=0["DEALLOCATE" "("_2 ", " _3 ")"]],
   ppDeallocateStmt.2:iter-sep   -- _1 ",",
   ppDeallocateStmt.3:iter-sep   -- _1 ", ",

   Allocation                    -- _1 "("_2")" "["_3"]",
   Allocation.2:iter-sep         -- _1 ",",
   Allocation.3:iter-sep         -- _1 ",",
 ppAllocation                    -- _1,
 ppAllocationShape               -- _1 "("_2")",
 ppAllocationShape.2:iter-sep    -- _1 ",",
 ppAllocationCoshape             -- _1 "["_2"]",
 ppAllocationCoshape.2:iter-sep  -- _1 ",",
   Triplet                       -- _1 ":" _2 _3,

   #R638 --,
   NullifyStmt                   -- H hs=1[_1 KW["NULLIFY"] H hs=0["("_2")" _3]],
   NullifyStmt.2:iter-sep        -- _1 ", ",

   #R640 --,
   DeallocateStmt                -- H hs=0[_1 KW["DEALLOCATE"] "("_2 _3")" _4],
   DeallocateStmt.2:iter-sep     -- _1 ",",

   #Operators --,
   UnaryMinus                    -- H hs=0["-" _1],
   UnaryPlus                     -- H hs=0["+" _1],
   Minus                         -- H hs=0[_1 "-" _2],
   NOT                           -- H hs=0[".NOT. " _1],
   Plus                          -- H hs=0[_1 "+" _2],
   GT                            -- H hs=0[_1 " > " _2],
   LT                            -- H hs=0[_1 " < " _2],
   NE                            -- H hs=0[_1 " /= " _2],
   EQ                            -- H hs=0[_1 " == " _2],
   LE                            -- H hs=0[_1 " <= " _2],
   GE                            -- H hs=0[_1 " >= " _2],
   AND                           -- H hs=0[_1 " .AND. " _2],
   OR                            -- H hs=0[_1 " .OR. " _2],
   EQV                           -- H hs=0[_1 " .EQV. " _2],
   NEQV                          -- H hs=0[_1 " .NEQV. " _2],
   DefUnaryExpr                  -- H hs=1[_1 _2],
   DefBinExpr                    -- H hs=0[_1 " " _2 " " _3],
   Power                         -- H hs=1[H hs=0[_1] "**" _2],
   Mult                          -- H hs=0[_1 "*" _2],
   Div                           -- H hs=0[_1 "/" _2],
   Concat                        -- H hs=1[_1 "//" _2],

   Parens                        -- H hs=0["(" _1 ")"],

   #R709 --,
   PLUS                          -- "+",

   #R732 --,
   AssignmentStmt                -- H [_1 _2 "=" _3],

   ppPointerAssignmentStmt       -- H hs=1[_1 H hs=0[_2          " => " _3]],
   ppPointerAssignmentStmt       -- H hs=1[_1 H hs=0[_2 "("_3")" " => " _4]],
   ppPointerAssignmentStmt.3:iter-sep  -- _1 ",",

   ppDataPointerObject           -- _1,
     DataPointerObject           -- _1 "%" _2,

   #R741 --,
   WhereStmt                     -- H hs=1[_1 KW["WHERE"] H hs=0["("_2")"] H hs=0[_3]],

   #R750 --,
   ForallConstruct               -- _1 _2 _3,

   ForallConstructStmt           -- H hs=1[_1 _2 "FORALL" H hs=0[_3]],

   ForallHeader                  -- "("_1 _2 _3")",
   ForallHeader.2:iter-sep       -- _1 ", ",
   no-mask-expr                  -- ,

   ForallTripletSpec             -- _1 "=" _2 ":" _3 _4,
   no-forall-step                -- ,

   EndForallStmt                 -- H hs=1[_1 "END FORALL" H hs=0[_2]],

   #R801 --,
   AssociateConstruct            -- _1 _2 _3,

   AssociateStmt                 -- H hs=1[_1 H hs=0[_2 "ASSOCIATE" "("_3")"]],
   AssociateStmt.3:iter-sep      -- _1 ", ",
   EndAssociateStmt              -- H hs=1 [_1 "END ASSOCIATE" H hs=0[_2]],

   Association                   -- _1 "=>" _2,

   Block                         -- H [_1 _2 ":" "BLOCK"] _3 _4,
   ppBlock                       -- H [_1 "BLOCK"] _2 _3,
   EndBlockStmt                  -- H [_1 "END BLOCK" _2],

   #R814 --,
   BlockDoConstruct              -- V is=2 [_1 _2] _3,

   LabelDoStmt                   -- H hs=1[_1 _2 "DO" H hs=0[_3] H hs=0[_4 _5]],

   NonlabelDoStmt                -- H hs=1[_1 H hs=0[_2 ": DO" _3]],
   ppNonlabelDoStmt              -- H hs=1[_1 H hs=0["DO" _2]],

   #R818 --,
   LoopControl                   -- H hs=0[" " _1 " = " H hs=0[_2 ", " _3 _4]],
   LoopWhileControl              -- KW["WHILE"] "("_1")",
   LoopConcurrentControl         -- " " KW["CONCURRENT"] _1,

   #R822 --,
   EndDoStmt                     -- H hs=1 [_1 "END DO" H hs=0[_2]],

   #R831 --,
   CycleStmt                     -- H hs=1 [_1 "CYCLE" H hs=0[_2]],

   #R832 --,
   IfConstruct                   -- V vs=0 is=2 [_1 _2] _3 _4 _5,
   ElseIfStmtBlock               -- V vs=0 is=2 [_1 _2],
   ElseStmtBlock                 -- V vs=0 is=2 [_1 _2],

   ppIfThenStmt                  -- H hs=1 [_1    "IF" H hs=0["("_2")"] "THEN"],
   ppIfThenStmt                  -- H hs=1 [_1 _2 "IF" H hs=0["("_3")"] "THEN"],
   ppIfConstructName             -- H hs=0 [_1 ":"],

   ElseIfStmt                    -- H hs=1 [_1 "ELSE IF" H hs=0["("_2")"] "THEN" _3],

   ElseStmt                      -- H hs=1 [_1 "ELSE" H hs=0[_2]],
 ppElseStmt                      -- H hs=1 [_1 "ELSE"           ],

   EndIfStmt                     -- H hs=1 [_1 "END IF" H hs=0[_2]],
 ppEndIfStmt                     -- H hs=1 [_1 "END IF"           ],

   #R837 --,
   IfStmt                        -- H hs=1 [_1 KW["IF"] H hs=0["("_2")"] _3],

   no-else-stmt                  -- ,

   #R838 --,
   CaseConstruct                 -- _1 _2 _3,
   CaseStmtBlock                 -- _1 _2,

   #R839 --,
   ppSelectCaseStmt                -- H hs=1[_1 H hs=0 [_2 KW["SELECT CASE"] "("_3")" _4]],

   #R840 --,
   CaseStmt                      -- H hs=1[_1 KW["CASE"] H hs=0["("_2")"] H hs=0[_3 _4]],
   CaseStmt.2:iter-sep           -- _1 ", ",

   #R841 --,
   EndSelectStmt                 -- H hs=1[_1 KW["END SELECT"] H hs=0[_2 _3]],

   #R843 --,
   DEFAULT                       -- KW["DEFAULT"],

   #R844 --,
   CaseValueRange                -- _1 KW[":"] _2,

   #R845 --,
   CaseValue                     -- _1,

   #R846 --,
   SelectTypeConstruct           -- _1 _2 _3,
   TypeGuardStmtBlock            -- _1 _2,

   #R847 --,
   SelectTypeStmt              -- H hs=1[H hs=0[_2 _1 KW["SELECT TYPE"]] H hs=0[_3 "("_4")" _5]],

   #R848 --,
   ClassGuardStmt                -- H hs=1[_1 KW["CLASS IS"] "("_2")" H hs=0[_3 _4]],
   TypeGuardStmt                 -- H hs=1[_1 KW["TYPE IS"] H hs=0["("_2")"] H hs=0[_3 _4]],
   DefaultGuardStmt              -- H hs=1[_1 KW["CLASS DEFAULT"] H hs=0[_2 _3]],

   #R849 --,
   EndSelectTypeStmt             -- H hs=1[_1 "END SELECT" H hs=0[_2 _3]],

   #R850 --,
   ExitStmt                      -- H hs=1 [_1 "EXIT" H hs=0[_2]],

   GotoStmt                      -- H hs=1 [_1 "GO TO" _2],

   ComputedGotoStmt              -- H hs=1 [_1 "GO TO" H hs=0["("_2")"] H hs=0[_3]],
   ComputedGotoStmt.2:iter-sep   -- _1 ", ",

   ArithmeticIfStmt              -- H hs=1 [H hs=0[_1 "IF" "("_2")"] H hs=0 [_3 ", " _4 ", " _5]],
     
   ContinueStmt                  -- H hs=1 [_1 "CONTINUE"],

   StopStmt                      -- H hs=1 [_1 "STOP" _2],
   no-stop-code                  -- ,

   ErrorStopStmt                 -- H hs=1 [_1 "ERROR STOP" _2],

   PauseStmt                     -- H hs=1 [_1 "PAUSE" H hs=0 [_2]],

   SyncAllStmt                   -- H hs=1 [_1 "SYNC ALL" H hs=0 ["(" _2 ")"]],
   SyncAllStmt.2:iter-sep        -- _1 ", ",

   #R860 --,
   SyncImagesStmt                -- H hs=1 [_1 "SYNC IMAGES" H hs=0 ["(" _2 _3 ")"]],
   SyncImagesStmt.3:iter-sep     -- _1 ", ",
 
   SyncMemoryStmt                -- H hs=1 [_1 "SYNC MEMORY" H hs=0 ["("_2")"]],
   SyncMemoryStmt.2:iter-sep     -- _1 ", ",

   LockStmt                      -- H hs=1 [_1 "LOCK" H hs=0 ["("_2 _3")"]],
   LockStmt.3:iter-sep           -- _1 ", ",

   ACQUIRED_LOCK                 -- "ACQUIRED_LOCK=" _1,

   UnlockStmt                    -- H hs=1 [_1 "UNLOCK" H hs=0 ["("_2 _3")"]],

   #R900 --,
   IOMSG                         -- "IOMSG="  _1,
   UNIT                          -- "UNIT="   _1,
   ERR                           -- "ERR="    _1,
   IOSTAT                        -- "IOSTAT=" _1,
   END                           -- "END="    _1,
   EOR                           -- "EOR="    _1,

   OpenStmt                      -- H hs=1 [_1 H hs=0["OPEN"  "(" _2 ")"]],
   OpenStmt.2:iter-sep           -- _1 ", ",

   ACCESS                        -- "ACCESS="       _1,
   ACTION                        -- "ACTION="       _1,
   ASYNCHRONOUS                  -- "ASYNCHRONOUS=" _1,
   BLANK                         -- "BLANK="        _1,
   DECIMAL                       -- "DECIMAL="      _1,
   DELIM                         -- "DELIM="        _1,
   ENCODING                      -- "ENCODING="     _1,
   FILE                          -- "FILE="         _1,
   FORM                          -- "FORM="         _1,
   NEWUNIT                       -- "NEWUNIT="      _1,
   PAD                           -- "PAD="          _1,
   POSITION                      -- "POSITION="     _1,
   RECL                          -- "RECL="         _1,
   ROUND                         -- "ROUND="        _1,
   SIGN                          -- "SIGN="         _1,
   STATUS                        -- "STATUS="       _1,
   TEAM                          -- "TEAM="         _1,

   CloseStmt                     -- H hs=1 [_1 H hs=0["CLOSE" "(" _2 ")"]],
   CloseStmt.2:iter-sep          -- _1 ", ",

   #R910 --,
   ReadStmt                      -- H hs=1 [_1 H hs=0["READ" "("_2")" ", " _3]],
   ReadStmt.3:iter-sep           -- _1 ", ",

   WriteStmt                     -- H hs=1 [_1 H hs=0["WRITE" "("_2") " _3]],
   WriteStmt.2:iter-sep          -- _1 ",",
   WriteStmt.3:iter-sep          -- _1 ", ",

   PrintStmt                     -- H hs=1 [_1 "PRINT" H hs=0[_2 ", " _3]],
   PrintStmt.3:iter-sep          -- _1 ",",

   FMT                           -- "FMT="     _1,
   SIZE                          -- "SIZE="    _1,
   ADVANCE                       -- "ADVANCE=" _1,
   REC                           -- "REC="     _1,

   Format_STAR                   -- "*",
   Format                        -- _1,

   #R917 --,
   OutputItem                    -- _1,

   #R918 --,
   IoImpliedDo                   -- "("_1 KW[", "] _2")",
   IoImpliedDo.1:iter-sep        -- _1 ", ",

   #R919 --,
   IoImpliedDoObjectList         -- _1,

   #R920 --,
   IoImpliedDoControl            -- _1 KW["="] _2 KW[","] _3 KW[","] _4,

   #R922 --,
   WaitStmt                      -- H hs=1 [_1 "WAIT" H hs=0["("_2")"]],
   WaitStmt.2:iter-sep           -- _1 ", ",

   #R924 --,
   BackspaceStmt                 -- H hs=1 [_1 "BACKSPACE" H hs=0 ["("_2")"]],
   BackspaceStmt.2:iter-sep      -- _1 ", ",

   #R926 --,
   RewindStmt                    -- H hs=1 [_1 "REWIND" H hs=0["("_2")"]],
   RewindStmt.2:iter-sep         -- _1 ", ",

   #R928 --,
   FlushStmt                     -- H hs=1 [_1 "FLUSH" H hs=0["("_2")"]],
   FlushStmt.2:iter-sep          -- _1 ", ",

   #R930 --,
   InquireStmt                   -- H hs=1 [_1 "INQUIRE" H hs=0["("_2")"]],
   InquireStmt.2:iter-sep        -- _1 ", ",
   InquireLengthStmt             -- H hs=1 [_1 "INQUIRE" H hs=0["(" "IOLENGTH=" _2 ")"] H hs=0[_3]],
   InquireLengthStmt.3:iter-sep  -- _1 ", ",

   #R1001 --,
   FormatStmt                    -- H hs=1[_1 H hs=0 ["FORMAT" "("_2")"]],
   FormatStmt.2:iter-sep         -- _1 ",",

   FormatSpec                    -- _1 _2,
   FormatSpec.1:iter-sep         -- _1 ",",

   R                             -- _1 "("_2")",
   R.2:iter-sep                  -- _1 ",",
   no-r                          -- ,
   no-m                          -- ,

   I                             -- _1 "I" _2 _3,
   E                             -- _1 "E" _2 "." _3 _4,
   F                             -- _1 "F" _2 "." _3,
   no-e                          -- ,

   P                             -- _1 "P",
   SLASH                         -- "/" _1,
   COLON_ED                      -- ":",

   X                             -- _1 "X",

   CS                            -- _1,

   #R1101 --,
   MainProgram                   -- V [_1 _2 _3],
   MainProgram.1:opt             -- ,

   ProgramStmt                   -- H [_1 KW["PROGRAM"] _2],
   EndProgramStmt                -- H [_1 "END PROGRAM" _2],
   no-program-stmt               -- ,
   no-program-name               -- ,

   #R1104 --,
   Module                        -- V vs=0 [_1 _2 _3],

   ModuleStmt                    -- H hs=1 [_1 KW["MODULE"] _2],
   EndModuleStmt                 -- H hs=1 [_1 "END MODULE" _2],
   no-module-name                -- ,

   #R1109 --,
   UseStmt                       -- H hs=1 [_1 "USE" H hs=0[_2 _3 _4]],
   UseOnlyStmt                   -- H hs=1 [_1 "USE" H hs=0[_2 _3 ", ONLY: " _4]],
   OnlyList                      -- _1,

   #R1110 --,
   ModuleNature                  -- ", " _1,
   no-module-nature              --,

   #R1111 --,
   Rename                        -- ", " H hs=0[_1 "=>" _2],

   #R1116 --,
   Submodule                     -- V vs=0 [_1 _2 _3],

   SubmoduleStmt                 -- H hs=1 [_1 "SUBMODULE" H hs=0["("_2")"] _3],
   EndSubmoduleStmt              -- H hs=1 [_1 "END SUBMODULE" H hs=0[_2]],
   no-parent-submodule-name      -- ,

   ParentIdentifier              -- _1 _2,

   #R1120 --,
   BlockData                     -- _1 _2 _3,

   BlockDataStmt                 -- _1 "BLOCK DATA" _2,
   EndBlockDataStmt              -- _1 "END BLOCK DATA" _2,
   no-block-data-name            -- ,

   #R1201 --,
   InterfaceBlock             -- V vs=0 [_1 _2 _3],
   InterfaceStmt              -- H hs=1 [_1 "INTERFACE" _2],
   AbstractInterfaceStmt      -- H hs=1 [_1 "ABSTRACT INTERFACE"],
   EndInterfaceStmt           -- H hs=1 [_1 "END INTERFACE" _2],
   InterfaceBody              -- _1 _2 _3,
   no-generic-spec            --,

   #R1205 --,
   ppScope                       -- _1 _2,
   ppScope                       -- _1 _2 _3,

   OfpPrefix                           -- _1,
   OfpPrefix.1:iter-star               -- _1,

   ELEMENTAL                           -- "ELEMENTAL",
   IMPURE                              -- "IMPURE",
   MODULE                              -- "MODULE",
   PURE                                -- "PURE",
   RECURSIVE                           -- "RECURSIVE",

   OfpResultName                       -- "RESULT" _1,
   OfpSuffix                           -- _1,
   OfpSuffix.1:iter-star               -- _1,

   #R1206 --,
   ProcedureStmt                       -- H hs=1[_1 "PROCEDURE ::" H hs=0[_2]],
   ProcedureStmt.2:iter-sep            -- _1 ", ",
   ModuleProcedureStmt                 -- H hs=1[_1 "MODULE PROCEDURE ::" H hs=0[_2]],
   ModuleProcedureStmt.2:iter-sep      -- _1 ", ",

   #R1207 --,
   OfpGenericSpec                      -- _1,
   ASSIGNMENT                          -- "ASSIGNMENT(=)",
   OPERATOR                            -- H hs=0[ "OPERATOR" "(" _1 ")" ],

   #R1209 --,
   ImportStmt                          -- H hs=1[_1 "IMPORT" H hs=0[_2]],
   ImportStmt.2:iter-sep               -- _1 ", ",

   #R1210 --,
   ExternalStmt                        -- H hs=1[_1 "EXTERNAL ::" H hs=0[_2]],
   ExternalStmt.2:iter-sep             -- _1 ", ",

   #R1211 --,
   ProcedureDeclarationStmt            --H hs=1[_1 "PROCEDURE" _2 H hs=0["("_3")"] H hs=0[_4]],
   ProcedureDeclarationStmt.4:iter-sep -- _1 ", ",
   no-proc-interface                   -- ,
  
   #R1214 --,
   ProcDecl                            -- _1 _2,

   #R1217 --,
   Function                            -- V vs=0 [_1 _2 _3],
   FunctionStmt                        -- H hs=1[_1 _2 "FUNCTION" H hs=0[_3 "("_4")" _5 _6]],
   FunctionStmt.4:iter-sep             -- _1 ",",
   EndFunctionStmt                     -- H hs=1[_1 "END FUNCTION" _2],
   no-function-name                    -- ,
   Result                              -- " RESULT(" _1 ")",
   no-result                           -- ,

   #R1218 --,
   IntrinsicStmt                       -- H hs=1[_1 "INTRINSIC ::" H hs=0[_2]],
   IntrinsicStmt.2:iter-sep            -- _1 ", ",

   #R1219 --,
   FunctionReference                   -- H hs=0[_1 "("_2")"],
   FunctionReference.2:iter-sep        -- _1 ",",

   #R1220 --,
   CallStmt                            -- H hs=1[_1 "CALL" H hs=0[_2 "("_3")"]],
   CallStmt.3:iter-sep                 -- _1 ",",

   Arg                                 -- _1,
   Arg                                 -- _1 "=" _2,
   ppArg                               -- _1,
   AltReturn                           -- "*" _1,
   no-keyword                          -- ,

   #R1233 --,
   Subroutine                          -- V [_1 _2 _3],
   SubroutineStmt                      -- H [_1 _2 "SUBROUTINE" H hs=0[_3 "("_4")" _5]],
   SubroutineStmt.4:iter-sep           -- _1 ",",
   EndSubroutineStmt                   -- H hs=0 [H hs=1[_1 "END SUBROUTINE" _2] "\n"],

   STAR                                -- "*",

   no-prefix                           -- ,
   no-subroutine-name                  -- ,

   #R1237 --,
   MpSubprogram                        -- _1 _2 _3,
   MpSubprogramStmt                    -- H hs=1 [_1 "MODULE PROCEDURE" _2],
   EndMpSubprogramStmt                 -- H hs=1 [_1 "END PROCEDURE" _2],
   no-procedure-name                   -- ,

   ReturnStmt                          -- H hs=1 [_1 "RETURN" _2],
   no-expr                             --

]
