#!/bin/bash
##################################################################################################
#Credits go to the first writers of "Compass: A pragmatic-direction-indicator in the jungle of coding standards".
#Brickbats go to Ramakrishna Upadrasta, Dan Quinlan, Gary Yuan, Jeremiah Willcock, and others (in that order)!
##################################################################################################

##################################################################################################
if [[ "$*" == "" ]] ; 
then
  echo "Usage: gen_checker.sh <project description>" 1>&2
  echo "Example: gen_checker.sh multiple cases on same line" 1>&2
  exit 1
fi

COMMAND_LINE_ARGS=$*
SCRIPTDIR=`dirname $0`
CMDROOT=`sh -c "cd $SCRIPTDIR; pwd"`

##################################################################################################
#A couple of perl hacks that generate the #directory-names/filenames/ifdef-names/class-namespace-names.
#Use temporary variable names that describe what we are getting from the perl hacks.
CAMEL_CASE_BEGINNING_WITH_UPPER_CASE=`perl -e 'foreach $i (@ARGV){print(ucfirst(lc($i)), "" );}' ${COMMAND_LINE_ARGS} | sed 's/[^a-zA-Z0-9_]//'`
CAMEL_CASE_BEGINNING_WITH_LOWER_CASE=`perl -e 'print(lcfirst("@ARGV"));' ${CAMEL_CASE_BEGINNING_WITH_UPPER_CASE} | sed 's/[^a-zA-Z0-9_]//'`
ALL_CAPITALS_WITH_UNDERSCORE_DELIMITERS=`perl -e 'foreach $i (@ARGV){print(uc($i), "_" );}' ${COMMAND_LINE_ARGS} | sed 's/[^a-zA-Z0-9_]//'`
CAPITALIZED_WORDS_WITH_SPACE_DELIMITERS=`perl -e 'foreach $i (@ARGV){print(ucfirst(lc($i)), " " );}' ${COMMAND_LINE_ARGS} | sed 's/ $//'`
##################################################################################################
#Here is an example: If user types in 
# $gen_checker.sh multiple cases on same line
# on command line, the variables are set as follows:
#
#      CAMEL_CASE_BEGINNING_WITH_UPPER_CASE         is set to    MultipleCasesOnSameLine
#      CAMEL_CASE_BEGINNING_WITH_LOWER_CASE         is set to    multipleCasesOnSameLine
#      ALL_CAPITALS_WITH_UNDERSCORE_DELIMITERS      is set to    MULTIPLE_CASES_ON_SAME_LINE_
#      CAPITALIZED_WORDS_WITH_SPACE_DELIMITERS      is set to    Multiple Cases On Same Line
#
#If the maintainer of this script has further doubts. Please uncomment the next 5 files
#echo "The camel case beginning with lower case letter is \"${CAMEL_CASE_BEGINNING_WITH_LOWER_CASE}\""
#echo "The camel case beginning with upper case letter is \"${CAMEL_CASE_BEGINNING_WITH_UPPER_CASE}\""
#echo "The name in caps using underscores as delimiters is \"${ALL_CAPITALS_WITH_UNDERSCORE_DELIMITERS}\""
#echo "The capitalized words with spaces as delimiters is \"${CAPITALIZED_WORDS_WITH_SPACE_DELIMITERS}\""
#exit
##################################################################################################
#What is in a name?
#
#It turns out that 
#            file                    names are camel case beginning with lower case,
# while      class/namespace         names are camel case beginning with upper case
# while      ifdef                   names are all capitalized words with underscores as delimiters
# while      the project-description names are actual-English words.
#Now, can we call ROSE by any other name Dan?
##################################################################################################

#Use the above temporary variable names to assign to the "proper variable names".
CLASS_NAME_PREFIX="${CAMEL_CASE_BEGINNING_WITH_UPPER_CASE}"
FILE_NAME_PREFIX="${CAMEL_CASE_BEGINNING_WITH_LOWER_CASE}"
IFDEF_PREFIX=${ALL_CAPITALS_WITH_UNDERSCORE_DELIMITERS}
PROJECT_DESCRIPTION_PREFIX="${CAPITALIZED_WORDS_WITH_SPACE_DELIMITERS}"
USER_NAME=`perl -e '@X = getpwuid($<); print $X[6]' |  sed 's/[0-9]\+//'`
DATE=`date +"%d-%B-%Y"`
SOURCE_DIRECTORY_NAME=${CAMEL_CASE_BEGINNING_WITH_LOWER_CASE}

###################################################################
#Check if the directory exists. If so, print an error and exit.
###################################################################
#If the directory is present, Exit.
if [  -e "${SOURCE_DIRECTORY_NAME}" ]
then 
    echo "Stopping further progress as the directory ${SOURCE_DIRECTORY_NAME} already exists."
    exit 1
fi

###################################################################
#Create the source directory.
###################################################################
echo "******************************************************************************"
mkdir -p ${SOURCE_DIRECTORY_NAME}
echo "Created the directory ${SOURCE_DIRECTORY_NAME}."

SCRIPT_DIRECTORY=${CMDROOT}/compass_template_generator
PREREQ_DIRECTORY=${CMDROOT}/../../extensions/prerequisites/
#`sh -c "cd ${SCRIPT_DIRECTORY}/../../extensions/prerequisites; pwd"`

###################################################################
#Copy the auxiliary source/header/parameter files to the directory.
###################################################################
cp -f ${SCRIPT_DIRECTORY}/compass.C ${SCRIPT_DIRECTORY}/compass.h ${SCRIPT_DIRECTORY}/compassTestMain.C ${SCRIPT_DIRECTORY}/compass_parameters ${SOURCE_DIRECTORY_NAME}
echo "Copied the files \"compass.C compass.h compassTestMain.C compass_parameters\""
echo "into the directory ${SOURCE_DIRECTORY_NAME}."
echo ""

###################################################################
#Generate the files Makefile and Makefile.am
###################################################################
#Generate Makefile
cat >./${SOURCE_DIRECTORY_NAME}/Makefile <<END
# ************************************************************************************
# *******  Users SHOULD modify the makefile variables at the top of this file ********
# ************************************************************************************

# Set variable ROSE_INSTALL to your ROSE_INSTALL installation like this.
# ROSE_INSTALL = /home/`whoami`/ROSE/ROSE_CompileTree/svn_LINUX-64bit-4.2.2_install

# If ROSE is configured with Dwarf support then we need libdwarf.a
# ROSE_OPTIONAL_LIBS = /home/`whoami`/ROSE/DWARF/dwarf-20081013/libdwarf/libdwarf.a -lelf

# A version of libcompass.so will be built locally to allow the checker to run
LOCAL_COMPASS_LIB = -L\`pwd\` -Wl,-rpath \`pwd\` -lcompass 

# Link syntax required for ROSE (using instaled version of ROSE)
ROSE_LIB = -L\$(ROSE_INSTALL)/lib -Wl,-rpath \$(ROSE_INSTALL)/lib -lrose 

# Specify specific path to find libstdc++.so (required when not using the default system compiler)
# This is required at least on the LLNL computers using non-default versions of GNU compilers.
# PATH_TO_SPECIFIC_LIBSTDCXX = -Wl,--rpath -Wl,/usr/apps/gcc/4.2.2-64bit/lib64

# Boost is required in ROSE, so we need the path.
BOOST_INCLUDE_PATH = /home/`whoami`/local/boost_1_37_0_installTree-gxx-4.2.2/include/boost-1_37

# If ROSE is configured with optional libs then we need the associated include paths.
# ROSE_OPTIONAL_LIBS_INCLUDE = -I/home/`whoami`/ROSE/DWARF/dwarf-20081013/libdwarf

# ************************************************************************************
# **********  Users should NOT have to modify the makefile below this point **********
# ************************************************************************************

CHECKER_NAME = testChecker

CXX_FLAGS = -I\$(ROSE_INSTALL)/include \$(ROSE_OPTIONAL_LIBS_INCLUDE) -I\$(BOOST_INCLUDE_PATH)

LINKER_FLAGS = \$(ROSE_LIB) \$(ROSE_OPTIONAL_LIBS) \$(LOCAL_COMPASS_LIB) \$(PATH_TO_SPECIFIC_LIBSTDCXX)

all: ${FILE_NAME_PREFIX}Test

libcompass.so: compass.h compass.C
	g++ -fPIC -Wall -shared -o libcompass.so compass.C \$(CXX_FLAGS)

${FILE_NAME_PREFIX}Test: ${FILE_NAME_PREFIX}.C ${FILE_NAME_PREFIX}Main.C libcompass.so
	g++ -fPIC -Wall -o ${FILE_NAME_PREFIX}Test ${FILE_NAME_PREFIX}Main.C ${FILE_NAME_PREFIX}.C \$(CXX_FLAGS) \$(LINKER_FLAGS)

test: ${FILE_NAME_PREFIX}Test ${FILE_NAME_PREFIX}Test1.C
	./${FILE_NAME_PREFIX}Test ${FILE_NAME_PREFIX}Test1.C

clean:
	-rm -f libcompass.so "${FILE_NAME_PREFIX}Test"

END
echo "Generated ${SOURCE_DIRECTORY_NAME}/Makefile"

#Generate Makefile.am
#cat >./${SOURCE_DIRECTORY_NAME}/Makefile.am <<END
#include \$(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = -I\$(srcdir)/../compassSupport \$(ROSE_INCLUDES)
#
#noinst_LTLIBRARIES = lib${CLASS_NAME_PREFIX}.la
#
#lib${CLASS_NAME_PREFIX}_la_SOURCES = ${FILE_NAME_PREFIX}.C
#
#bin_PROGRAMS = ${FILE_NAME_PREFIX}Main
#
#LDADD = \$(LIBS_WITH_RPATH) \$(ROSE_LIBS) ../compassSupport/libCompass.la lib${CLASS_NAME_PREFIX}.la
#
#${FILE_NAME_PREFIX}Main_SOURCES = lib${CLASS_NAME_PREFIX}.la ${FILE_NAME_PREFIX}Main.C
#${FILE_NAME_PREFIX}Main_DEPENDENCIES = \$(ROSE_LIBS_WITH_PATH) lib${CLASS_NAME_PREFIX}.la
#
#test: lib${CLASS_NAME_PREFIX}.la ${FILE_NAME_PREFIX}Main 
#	cp -f \$(srcdir)/compass_parameters compass_parameters
#	./${FILE_NAME_PREFIX}Main \$(srcdir)/${FILE_NAME_PREFIX}/${FILE_NAME_PREFIX}Test1.C
#
#check-local:
#	@\$(MAKE) test
#	@echo "*********************************************************************************************************************"
#	@echo "*** ROSE/projects/compass/${FILE_NAME_PREFIX}: make check rule complete (terminated normally) ***"
#	@echo "*********************************************************************************************************************"
#
#EXTRA_DIST = compass_parameters
#
#clean-local:
#	rm -f compass_parameters *.ti ${FILE_NAME_PREFIX}Test
#
#END
#echo "Generated ${SOURCE_DIRECTORY_NAME}/Makefile.am"
#
################################################################################
#
# Generate the include checker include (.inc) file
#
cat > ./"${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}.inc" <<END
# This is an automatically generated file
# This example shows how to automake checkers that use boost
# if ROSE_USE_BOOST_WAVE
#
#  LDADD = \$(LIBS_WITH_RPATH) \$(ROSE_LIBS) compassSupport/libCompass.la
#  \$(compass_detector_libs) -lboost_regex-@BOOST_LIB_SUFFIX@ \$(LDADD)
#
# endif
END

###################################################################
#Generate the sources and headers.
###################################################################
cat >./"${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}.C" <<END
// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// ${PROJECT_DESCRIPTION_PREFIX} Analysis
// Author: ${USER_NAME}
// Date: ${DATE}

#include "rose.h"
#include "compass.h"

extern const Compass::Checker* const ${FILE_NAME_PREFIX}Checker;

// DQ (1/17/2009): Added declaration to match external defined in file:
// rose/projects/compass/extensions/prerequisites/ProjectPrerequisite.h
// I can't tell that it is defined anywhere in compass except the extern 
// declaration in ProjectPrerequisite.h
Compass::ProjectPrerequisite Compass::projectPrerequisite;

namespace CompassAnalyses
   { 
     namespace ${CLASS_NAME_PREFIX}
        { 
        /*! \\brief ${PROJECT_DESCRIPTION_PREFIX}: Add your description here 
         */

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction 
             {
                    Compass::OutputObject* output;
            // Checker specific parameters should be allocated here.

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

CompassAnalyses::${CLASS_NAME_PREFIX}::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,::${FILE_NAME_PREFIX}Checker->checkerName,::${FILE_NAME_PREFIX}Checker->shortDescription)
   {}

CompassAnalyses::${CLASS_NAME_PREFIX}::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["${CLASS_NAME_PREFIX}.YourParameter"]);


   }

void
CompassAnalyses::${CLASS_NAME_PREFIX}::Traversal::
visit(SgNode* node)
   { 
  // Implement your traversal here.  

   } //End of the visit function.

// Checker main run function and metadata

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::${CLASS_NAME_PREFIX}::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::${CLASS_NAME_PREFIX}::Traversal(params, output);
}

extern const Compass::Checker* const ${FILE_NAME_PREFIX}Checker =
  new Compass::CheckerUsingAstSimpleProcessing(
        "${CLASS_NAME_PREFIX}",
     // Descriptions should not include the newline character "\n".
        "Short description not written yet!",
        "Long description not written yet!",
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
   
END

#Generate the header for the factory typedef.
cat >./"${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}Main.C" <<END
//
// Do not modify this file
//

#include "compass.h"
extern const Compass::Checker* const ${FILE_NAME_PREFIX}Checker;
extern const Compass::Checker* const myChecker = ${FILE_NAME_PREFIX}Checker;

#include "compass.C"
#include "compassTestMain.C"
END
chmod a-w "${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}Main.C"
echo "Generated ${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}Main.C"

#Generate the empty test file.
cat >./"${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}Test1.C" <<END

//Your test file code goes here.

END
echo "Generated ${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}Test1.C"

###################################################################
#Generate the LaTeX file for documentation.
###################################################################
cat >./"${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}Docs.tex" <<END
% 9.5.07
% This is a sample documentation for Compass in the tex format.
% We restrict the use of tex to the following subset of commands:
%
% \\section, \\subsection, \\subsubsection, \\paragraph
% \\begin{enumerate} (no-nesting), \\begin{quote}, \\item
% {\\tt ... }, {\\bf ...}, {\\it ... }
% \htmladdnormallink{}{}
% \\begin{verbatim}...\\end{verbatim} is reserved for code segments
% ``...''
%

\\section{${PROJECT_DESCRIPTION_PREFIX}}
\\label{${CLASS_NAME_PREFIX}::overview}

% write your introduction

\\subsection{Parameter Requirements}

%Write the Parameter specification here.
   No Parameter specifications yet!

\\subsection{Implementation}

%Details of the implementation go here.
   No implementation yet!

\\subsection{Non-Compliant Code Example}

% write your non-compliant code subsection

\\begin{verbatim}

% write your non-compliant code example

\\end{verbatim}

\\subsection{Compliant Solution}

% write your compliant code subsection

\\begin{verbatim}

% write your compliant code example

\\end{verbatim}

\\subsection{Mitigation Strategies}
\\subsubsection{Static Analysis} 

Compliance with this rule can be checked using structural static analysis checkers using the following algorithm:

\\begin{enumerate}
\\item Write your checker algorithm
\\end{enumerate}

\\subsection{References}

% Write some references
% ex. \\htmladdnormallink{ISO/IEC 9899-1999:TC2}{https://www.securecoding.cert.org/confluence/display/seccode/AA.+C+References} Forward, Section 6.9.1, ``Function definitions''
END
echo "Generated ${SOURCE_DIRECTORY_NAME}/../${FILE_NAME_PREFIX}/${FILE_NAME_PREFIX}Docs.tex"
cat <<END
******************************************************************************
All files have been generated successfully. You should modify at least the
variable \${ROSE_INSTALL} in ${SOURCE_DIRECTORY_NAME}/Makefile to get a
working system. Good luck with your style-checker!
END

echo $PREREQ_DIRECTORY
#exit 0
find $PREREQ_DIRECTORY -name "*.h" | awk -F/ '{print "#include \"" $NF "\""}' > ${SOURCE_DIRECTORY_NAME}/prerequisites.h

# DQ (9/28/2010): Suggested change by Matt (Portland Fortran Adventure).
# tail -1 $PREREQ_DIRECTORY/*.h | grep "extern" | sed -e 's@extern[\t\ ]*@Compass::@g' | awk '{print $1 " Compass::" $2}' > ${SOURCE_DIRECTORY_NAME}/instantiate_prerequisites.h
tail -n 1 $PREREQ_DIRECTORY/*.h | grep "extern" | sed -e 's@extern[\t\ ]*@Compass::@g' | awk '{print $1 " Compass::" $2}' > ${SOURCE_DIRECTORY_NAME}/instantiate_prerequisites.h
