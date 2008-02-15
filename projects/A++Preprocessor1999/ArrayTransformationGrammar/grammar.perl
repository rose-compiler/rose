#!/bin/perl

# perl program to generate the class declarations for the implementation of the
# A++/P++ grammar

# This list is not used but represents the overall structure of the 
# design of the class hierarchy

# These are removed from the list of classes we will define because
# they will be represented by a typedef from an STL list.
# typedef ROSE_IndexArgumentList list<ROSE_IndexExpression>;
# typedef ROSE_ArgumentList      list<ROSE_ArrayExpression>;

# We don't have to be as specific in this higher level grammar!
# Notice that we extend the grammar to provide the error recovery 
# within the parser (I always wanted to do this trick, it's in the Dragon Book)
@ClassList = (
ROSE_TransformableNode,
     ROSE_TransformableStatement,
          ROSE_NonTransformableStatement,
          ROSE_TransformableStatementBlock,
     ROSE_TransformableExpression,
          ROSE_TransformableOperatorExpression,
               ROSE_TransformableUnaryOperatorExpression,
               ROSE_TransformableBinaryOperatorExpression,
                    ROSE_TransformableBinaryOperatorEquals,
                    ROSE_TransformableBinaryOperatorNonAssignment,
          ROSE_TransformableOperandExpression,
          ROSE_NonTransformableExpression
             );

# The indentation reflects the hierarchy (the less indented name is a base class)
@ClassListRoot = (
ROSE_TransformableNode );

# We can use a list of lists but I'm not clear if we require that!
@ClassList_DerivedFrom_Node = ( 
  ClassList_TransformableNode );

@ClassList_TransformableNode = (
     ROSE_TransformableStatement,
     ROSE_TransformableExpression );

@ClassList_TransformableStatement = (
     ROSE_NonTransformableStatement,
     ROSE_TransformableStatementBlock );

@ClassList_TransformableExpression = (
          ROSE_TransformableOperatorExpression,
          ROSE_TransformableOperandExpression,
          ROSE_NonTransformableExpression );

@ClassList_TransformableOperator = (
          ROSE_TransformableUnaryOperatorExpression,
          ROSE_TransformableBinaryOperatorExpression );

@ClassList_TransformableBinaryOperator = (
               ROSE_TransformableBinaryOperatorEquals,
               ROSE_TransformableBinaryOperatorNonAssignment );

@ListOfLists = (
     ClassListRoot,
     ClassList_TransformableNode,
     ClassList_TransformableStatement,
     ClassList_TransformableExpression,
     ClassList_TransformableOperator,
     ClassList_TransformableBinaryOperator );

@BaseClassList = (
     ArrayAssignmentUsingTransformationGrammar,
     ROSE_TransformableNode,
     ROSE_TransformableStatement,
     ROSE_TransformableExpression,
     ROSE_TransformableOperatorExpression,
     ROSE_TransformableBinaryOperatorExpression );

###############################
# Start of Function Definitions
###############################

sub buildMemberFunctionDeclarations
   {
   # Since we handle ALL the cases we could make this function much simpler!
     local($returnString);
     $returnString = "NULL";
     if ($_[0] eq "ROSE_TransformableNode")
        { $returnString = ""; }
     elsif ($_[0] eq "ROSE_TransformableStatement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableOperatorExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableUnaryOperatorExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableBinaryOperatorExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableBinaryOperatorEquals")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableBinaryOperatorNonAssignment")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableOperandExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_NonTransformableStatement")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_NonTransformableExpression")
        { $returnString = &readClassDeclarationBody($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableStatementBlock")
        { $returnString = &readClassDeclarationBody($_[0]); }
     else
        { die "can not find in class declaration list $_[0]" }

     $returnString;
   }

sub buildMemberFunctionDefinitions
   {
   # Since we handle ALL the cases we could make this function much simpler!
     local($returnString);
     $returnString = "NULL";
     if ($_[0] eq "ROSE_TransformableNode")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableStatement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableOperatorExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableUnaryOperatorExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableBinaryOperatorExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableBinaryOperatorEquals")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableBinaryOperatorNonAssignment")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableOperandExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_NonTransformableStatement")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_NonTransformableExpression")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     elsif ($_[0] eq "ROSE_TransformableStatementBlock")
        { $returnString = &readMemberFunctionDefinition($_[0]); }
     else
        { die "can not find in member function definition list $_[0]" }

     $returnString;
   }

######################################
# Start of Simple Function Definitions
######################################

sub copyNodeClassDeclaration
   {
     $file = "node.macro";
     open(FILE,"$file") || die "cannot open file $file!" ;

     while( <FILE> )
        {
          $line = $_;
          print OUTFILE $line;
        # print $line;
        }
   }

sub readClassDeclarationBody
   {
     &readInFileToString ("classDeclarationBody_$_[0].macro",$_[0]);
   }

sub readMemberFunctionDefinition
   {
     &readInFileToString ("memberFunctionDefinition_$_[0].macro",$_[0]);
   }

sub readInFileToString
   {
   # print "Inside of readInFileToString($_[0]) \n";
     local($readInFileToString_file);
     $readInFileToString_file = $_[0];
     open(readInFileToString_FILE,"$readInFileToString_file") || die "cannot open file $readInFileToString_file!" ;

     local($line);
     local($returnStr);
     while( <readInFileToString_FILE> )
        {
          $line = $_;
          $line =~ s/\bCLASSNAME\b/$_[1]/g;
          $line =~ s/\bCLASSNAME_TAG\b/$_[1]Tag/g;
          $returnStr = $returnStr . $line;
        # print $line;
        }

     close(readInFileToString_FILE);
     $returnStr;
   }

sub classDeclaration
   {
     $file = "grammarClassDeclatationMacros.macro";
     open(FILE,"$file") || die "cannot open file $file!";

   # Compute this once
     $memberFunctionDeclarations = &buildMemberFunctionDeclarations($_[0]);

   # print "Member function data = $memberFunctionDeclarations \n";

     while( <FILE> )
        {
          $line = $_;
          $line =~ s/\bCLASSNAME\b/$_[0]/g;
          $line =~ s/\bCLASSNAME_TAG\b/$_[0]Tag/g;
          $line =~ s/\bBASECLASSNAME\b/$_[1]/g;
          $line =~ s/\bMEMBER_FUNCTION_DECLARATIONS\b/$memberFunctionDeclarations/g;
          print OUTFILE $line;
        # print $line;
        }
   }

sub BuildClassDeclatations
   {
     foreach $class ( @{$_[0]} )
        {
        # print "Generating $class derived from $_[1] \n";
          classDeclaration($class,": public $_[1]");
	}
   }

sub BuildGrammarDeclarations
   {
   # Copy the node.h file into the output file
     copyNodeClassDeclaration;

     local($list);
     local($baseClass);
     local($i);
     for ($i=1; $i <= @ListOfLists; $i++)
        {
          $list = @ListOfLists[$i];
          $baseClass = @BaseClassList[$i];
          BuildClassDeclatations ($list,$baseClass);
        }
   }

sub classDefinition
   {
   # Compute this once
     $memberFunctionDefinitions  = &buildMemberFunctionDefinitions ($_[0]);
     $memberFunctionDefinitions =~ s/\bBASECLASSNAME\b/$_[1]/g;
     print OUTFILE_SOURCE $memberFunctionDefinitions;

     if ( $_[0] ne "ROSE_TransformableNode")
        {
          $file = "grammarClassDefinitionMacros.macro";
          open(DEFINITION_FILE,"$file") || die "cannot open file $file!";

          while( <DEFINITION_FILE> )
             {
               $line = $_;
               $line =~ s/\bCLASSNAME\b/$_[0]/g;
               $line =~ s/\bCLASSNAME_TAG\b/$_[0]Tag/g;
               print OUTFILE_SOURCE $line;
             # print $line;
             }
        }
   }

sub BuildClassDefinitions
   {
     foreach $class ( @{$_[0]} )
        {
        # print "Generating $class member functions derived from $_[1] \n";
          classDefinition($class,$_[1]);
	}
   }

sub BuildGrammarDefinitions
   {
     local($list);
     local($baseClass);
     local($i);
     for ($i=0; $i <= @ListOfLists; $i++)
        {
          $list = @ListOfLists[$i];
          $baseClass = @BaseClassList[$i];
          BuildClassDefinitions ($list,$baseClass);
        }
   }

sub BuildVariantsEnum
   {
     print OUTFILE "\n\n";
#    print OUTFILE "extern arrayGrammarTerminalNames; \n";
#    print OUTFILE "extern arrayGrammarTerminalNames; \n";
#    print OUTFILE "\n\n";

     print OUTFILE "enum ROSE_TransformableGrammarVariants \n";
     print OUTFILE "   {\n";
     print OUTFILE "// Variants used to identify the grammar used in ROSE \n";
     print OUTFILE "\n";
     foreach $class ( @ClassList )
        {
          print OUTFILE "     ${class}Tag, \n";
	}

     print OUTFILE "     ROSE_TRANSFORMABLE_LAST_TAG \n";
     print OUTFILE "   };\n";
     print OUTFILE "\n\n";
   }

sub BuildVariantsStringDataBase
   {
     $listLength = 0;
     foreach $class ( @ClassList )
        {
          $listLength++;
	}

     $listLength++;
     print OUTFILE_SOURCE "\n\n";

     print OUTFILE_SOURCE "struct \n";
     print OUTFILE_SOURCE "   \{ \n";
     print OUTFILE_SOURCE "     ROSE_TransformableGrammarVariants variant; \n";
     print OUTFILE_SOURCE "     char *name;                   \n";
     print OUTFILE_SOURCE "   } transformableGrammarTerminalNames[$listLength] = {  \n";

     foreach $class ( @ClassList )
        {
          print OUTFILE_SOURCE "          {${class}Tag, \"${class}\"}, \n";
	}

     print OUTFILE_SOURCE "          {ROSE_TRANSFORMABLE_LAST_TAG, \"last tag\" } \n";
     print OUTFILE_SOURCE "  \}; \n";
   }

sub BuildForwardDeclarations
   {
     print OUTFILE "\n\n";
     print OUTFILE "// Forward Declarations used to represent the grammar used in ROSE \n";
     foreach $class ( @ClassList )
        {
          print OUTFILE "class ${class};\n";
	}

     print OUTFILE "\n\n";
   }

############################
# Start of MAIN Perl program
############################

$outfile="grammar_3.h";
open(OUTFILE,">$outfile") || die "cannot open file $outfile!" ;

$line = "// MACHINE GENERATED HEADER FILE --- DO NOT MODIFY! \n\n";
print OUTFILE $line;
$line = "\#ifndef TRANSFORMABLE_GRAMMAR_3_H\n";
print OUTFILE $line;
$line = "\#define TRANSFORMABLE_GRAMMAR_3_H\n";
print OUTFILE $line;

# We want the lists to be objects that have ROSE_Node as a base class just as the 
# SgExprListExp is an object in Sage and also a list of SgExpression objects too.
# $typedefStatement = "typedef ROSE_IndexArgumentList list<ROSE_IndexExpression>;\n";
# print OUTFILE $typedefStatement;
# $typedefStatement = "typedef ROSE_ArgumentList      list<ROSE_ArrayExpression>;\n";
# print OUTFILE $typedefStatement;

# Build enum for variants
BuildVariantsEnum;

#Build the forward declarations for all the classes which implement the grammar
BuildForwardDeclarations;

# Now build all the class declarations for the rest of the grammar
BuildGrammarDeclarations;

$line = "\#endif // ifndef TRANSFORMABLE_GRAMMAR_3_H \n";
print OUTFILE $line;

close(FILE);
close(OUTFILE);

# Generate the Class Member Function Definitions
$outfileSource="grammar_3.C";
open(OUTFILE_SOURCE,">$outfileSource") || die "cannot open file $outfileSource!" ;

$line = "// MACHINE GENERATED SOURCE FILE --- DO NOT MODIFY! \n\n";
print OUTFILE_SOURCE $line;

# New version using autoconf and automake configuration tools requires all C++ source to have this line
$line = "#ifdef HAVE_CONFIG_H \n#include \"config.h\" \n#endif \n\n";
print OUTFILE_SOURCE $line;

$line = "#include \"rose.h\" \n";
print OUTFILE_SOURCE $line;

$line = "#include \"ROSE_Transformations.h\" \n\n";
print OUTFILE_SOURCE $line;

$line = "extern struct { ROSE_GrammarVariants variant; char *name; } arrayGrammarTerminalNames[143];\n";
print OUTFILE_SOURCE $line;

BuildVariantsStringDataBase;

# Now build all the class declarations for the rest of the grammar
BuildGrammarDefinitions;

close(OUTFILE_SOURCE);

exit;








