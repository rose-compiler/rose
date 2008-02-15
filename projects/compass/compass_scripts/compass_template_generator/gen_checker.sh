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

SCRIPT_DIRECTORY=`dirname $0`;

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
#Set variable ROSE_INSTALL to your ROSE_INSTALL installation like this.
#ROSE_INSTALL = /home/`whoami`/rose-install

CHECKER_NAME = ${FILE_NAME_PREFIX}

LINKER_FLAGS = -L\$(ROSE_INSTALL)/lib -Wl,-rpath \$(ROSE_INSTALL)/lib -lrose -lrt -L\`pwd\` -Wl,-rpath \`pwd\` -lcompass

all: ${FILE_NAME_PREFIX}Test

libcompass.so: compass.h compass.C
	g++ -Wall -shared -o libcompass.so compass.C -I\$(ROSE_INSTALL)/include

${FILE_NAME_PREFIX}Test: ${FILE_NAME_PREFIX}.C ${FILE_NAME_PREFIX}Main.C libcompass.so
	g++ -Wall -o ${FILE_NAME_PREFIX}Test ${FILE_NAME_PREFIX}.C ${FILE_NAME_PREFIX}Main.C -I\$(ROSE_INSTALL)/include \$(LINKER_FLAGS)

test: ${FILE_NAME_PREFIX}Test ${FILE_NAME_PREFIX}Test1.C
	./${FILE_NAME_PREFIX}Test ${FILE_NAME_PREFIX}Test1.C

clean:
	-rm -f libcompass.so "${FILE_NAME_PREFIX}Test"

END
echo "Generated ${SOURCE_DIRECTORY_NAME}/Makefile"

#Generate Makefile.am
cat >./${SOURCE_DIRECTORY_NAME}/Makefile.am <<END
include \$(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -I\$(srcdir)/../compassSupport \$(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = lib${CLASS_NAME_PREFIX}.la
else
noinst_LTLIBRARIES = lib${CLASS_NAME_PREFIX}.la
endif

lib${CLASS_NAME_PREFIX}_la_SOURCES = ${FILE_NAME_PREFIX}.C

bin_PROGRAMS = ${FILE_NAME_PREFIX}Main

LDADD = \$(LIBS_WITH_RPATH) \$(ROSE_LIBS) ../compassSupport/libCompass.la lib${CLASS_NAME_PREFIX}.la

${FILE_NAME_PREFIX}Main_SOURCES = lib${CLASS_NAME_PREFIX}.la ${FILE_NAME_PREFIX}Main.C
${FILE_NAME_PREFIX}Main_DEPENDENCIES = \$(ROSE_LIBS_WITH_PATH) lib${CLASS_NAME_PREFIX}.la

test: lib${CLASS_NAME_PREFIX}.la ${FILE_NAME_PREFIX}Main 
	cp -f \$(srcdir)/compass_parameters compass_parameters
	./${FILE_NAME_PREFIX}Main \$(srcdir)/../tests/${FILE_NAME_PREFIX}Test.C

check-local:
	@\$(MAKE) test
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/projects/compass/${FILE_NAME_PREFIX}: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************************"

EXTRA_DIST = compass_parameters ${FILE_NAME_PREFIX}.h 

clean-local:
	rm -f compass_parameters *.ti ${FILE_NAME_PREFIX}Test

END
echo "Generated ${SOURCE_DIRECTORY_NAME}/Makefile.am"

###################################################################
#Generate the sources and headers.
###################################################################
cat >./"${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}.C" <<END
// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// ${PROJECT_DESCRIPTION_PREFIX} Analysis
// Author: ${USER_NAME}
// Date: ${DATE}

#include "compass.h"
#include "${FILE_NAME_PREFIX}.h"

namespace CompassAnalyses
   { 
     namespace ${CLASS_NAME_PREFIX}
        { 
          const std::string checkerName      = "${CLASS_NAME_PREFIX}";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Short description not written yet!";
          const std::string longDescription  = "Long description not written yet!";
        } //End of namespace ${CLASS_NAME_PREFIX}.
   } //End of namespace CompassAnalyses.

CompassAnalyses::${CLASS_NAME_PREFIX}::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::${CLASS_NAME_PREFIX}::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
   
END

#Generate the header file.
cat >"${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}.h" <<END
// ${PROJECT_DESCRIPTION_PREFIX}
// Author: ${USER_NAME}
// Date: ${DATE}

#include "compass.h"

#ifndef COMPASS_${IFDEF_PREFIX}H
#define COMPASS_${IFDEF_PREFIX}H

namespace CompassAnalyses
   { 
     namespace ${CLASS_NAME_PREFIX}
        { 
        /*! \\brief ${PROJECT_DESCRIPTION_PREFIX}: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_${IFDEF_PREFIX}H
#endif 

END
echo "Generated ${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}.h"

#Generate the header for the factory typedef.
cat >./"${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}Main.C" <<END
//
// Do not modify this file
//

#include "${FILE_NAME_PREFIX}.h"
typedef CompassAnalyses::${CLASS_NAME_PREFIX}::Traversal Checker;

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
%General Suggestion: Ideally, the documentation of a style checker should 
%be around one page.
\\section{${PROJECT_DESCRIPTION_PREFIX}}

\\label{${CLASS_NAME_PREFIX}::overview}

\\quote{Namespace: \\texttt{${CLASS_NAME_PREFIX}}}

\\subsection{Introduction}

%Write your introduction here.
   No introduction yet!

%Give an exact reference to the pattern in the literature.
%Here is the example:
%The reference for this checker is: H. Sutter, A. Alexandrescu:
%\`\`C++ Coding Standards'', Item 28: \`\`Prefer the canonical form ++ and --. 
%Prefer calling the prefix forms''.

No reference to literature as yet!

\\subsection{Parameter Requirements}

%Write the Parameter specification here.
   No Parameter specifications yet!

\\subsection{Implementation}

%Details of the implementation go here.
   No implementation yet!

\\subsection{Example of Failing Output Code}

%Examples of the patterns go here.
     See example: ${FILE_NAME_PREFIX}Test1.C

%The following lines are for references to the examples in the
%documentation.
\\begin{latexonly}
{\\codeFontSize
\\lstinputlisting{\\ExampleDirectory/../${FILE_NAME_PREFIX}/${FILE_NAME_PREFIX}Test1.C}
}
\\end{latexonly}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%If there is strange known behaviour, you can write a 
%subsection that describes that problem.

END
echo "Generated ${SOURCE_DIRECTORY_NAME}/${FILE_NAME_PREFIX}Docs.tex"

cat <<END
******************************************************************************
All files have been generated successfully. You should modify at least the
variable \${ROSE_INSTALL} in ${SOURCE_DIRECTORY_NAME}/Makefile to get a
working system. Good luck with your style-checker!
END

