#!/bin/bash
################################################################################
#
# Author: Gary Yuan
# File: compass_submission_setup.sh
# Date: 7/19/2007
# Updated: 1/24/2008
# Purpose: Automatically build compass from checker submit directory 
#
################################################################################

### GLOBAL VARIABLES

BACK=`pwd`
SCRIPTDIR=`dirname $0`
CMDROOT=`sh -c "cd $SCRIPTDIR; pwd"`
CHECKIN_DIRECTORY=$1
COMPASS_PROJECT=$2
COMPASS_BUILD_DIR=$3
READ_ONLY=$4
SKIP_COMPASS_MAKEFILE_INC=$5

NEW_CHECKERS=`find ${CHECKIN_DIRECTORY} -maxdepth 1 -type f`

COMPASS_LIST=${COMPASS_PROJECT}/CHECKER_LIST
COMPASS_MAKEINC=${COMPASS_PROJECT}/compass_makefile.inc
COMPASS_HEADER=${COMPASS_BUILD_DIR}/checkers.h
COMPASS_SOURCE=${COMPASS_BUILD_DIR}/buildCheckers.C
COMPASS_DOCS=${COMPASS_BUILD_DIR}/compassCheckerDocs.tex
#COMPASS_DOCS_MAKEFILE=${COMPASS_PROJECT}/docs.makefile # DEPRECATED
CUSTOM_CHECKERS=""

declare -a SUBDIRS
declare -a USUBDIRS
declare -a SUBDIR_COUNT=0

################################################################################

###
#
# The printCustomCheckers function prints a diagnostic of checkers with custom
# include files
#
###
printCustomCheckers()
{
  echo ""
  for checker in $@
  do
    echo "Warning: $checker has a customized include file in $COMPASS_MAKEINC"
  done

  echo""
} #printCustomCheckers

###
#
# The copySupportFiles function copies the compass.[Ch] file from the common
# version in from ${CHECKIN_DIRECTORY}/compass_template_generator to the compass
# project directory
#
###
ROSE_BUILD_DIRs()
{
   echo "Copying compass.[Ch] from ${CHECKIN_DIRECTORY}/bin/compass_template_generator to ${COMPASS_PROJECT}/compassSupport/"

   cp -f ${CHECKIN_DIRECTORY}/bin/compass_fixes/compassTestMain.C ${CHECKIN_DIRECTORY}/bin/compass_template_generator/compass.[Ch] ${COMPASS_PROJECT}/compassSupport/

   echo "Copying Compass Fixes to ${COMPASS_PROJECT}..."
   cp -f ${CHECKIN_DIRECTORY}/bin/compass_fixes/compass.tex.in ${CHECKIN_DIRECTORY}/bin/compass_fixes/compassMain.C ${COMPASS_PROJECT}

} #copyFixes()

###
#
# The concatCompassParameters() function concats the individual checker
# compass_parameters file into a global compass_parameters located in
# <COMPASS_PROJECT>/projects/compass
#
###
concatCompassParameters()
{
  local paramfile=${COMPASS_PROJECT}/compass_parameters
  local line=""
  declare -i linenum=0
  declare -i count=0

  echo "# this is an automatically generated file " > $paramfile
  echo "Compass.RuleSelection=RULE_SELECTION" > $paramfile

  for dir in ${SUBDIRS[@]}
  do
   if [[ -f ${COMPASS_PROJECT}/${dir}/compass_parameters ]]; then
      grep -v '^#' ${COMPASS_PROJECT}/${dir}/compass_parameters >> $paramfile 
   fi
  done

  echo "Generated new compass_parameters file"
} #concatCompassParameters()

###
#
# The updateCheckerList() function updates the file 
# projects/compass/CHECKER_LIST that contains a list of all checkers 'seen' by
# this script
#
###
updateCheckerList()
{
  local checker=$1
  local checker_list=$2
  local -i count=0

  local checker_exist=`grep "${checker}" $checker_list`

  if [[ $checker_exist == '' ]]; then
    echo "Updating ${checker} to $checker_list"
    echo "${checker}" >> $checker_list
  fi # We've got new checker!

} #updateCheckerList() <checker name> <checker list path>


genCertDocs()
{
  local file=$1
  local href=`cat $file`
  local code=`basename $href | gawk -F. '{print $1}'`
  local tex=`ls ${CMDROOT}/seccode/*$code*.tex`
  local tempfile=`mktemp`

  if [[ -f $tex ]]; then
    cp $tex $file
  else
    wget $href
    mv -f `basename $href | sed -e 's/%28/(/g' | sed -e 's/%29/)/g' | sed -e 's/%20/ /g'` $tempfile.html
    ${CMDROOT}/wiki2tex/bin/wiki2tex.sh $tempfile.html
    mv -f `basename ${tempfile}.tex` $file
  fi

  rm -f ${tempfile}.html ${tempfile}* `basename $href` wiki2tex.xsl
} #genCertDoc


highlightDocs()
{
  local file=$1
  local makefile=$2
  local tempfile=`mktemp`
  local -a sblocks
  local -a eblocks
  local -i count=0
  local -i count2=0
  local -i sline=0
  local -i eline=0
  local buffer=""

  sed -e '/^\%/ d' "$file" > $tempfile
  mv -f $tempfile "$file"

  count=0
  for sl in `grep -n '\\\begin{verbatim}' "$file" | gawk -F: '{print $1}'`
  do
    sblocks[$((count++))]=$sl
  done

  count=0
  for el in `grep -n '\\end{verbatim}' "$file" | gawk -F: '{print $1}'`
  do
    eblocks[$((count++))]=$el
  done

  local -i numblocks=${#sblocks[@]}

  if(($numblocks != ${#eblocks[*]})); then
    echo "Error: Mismatch in code blocks for $file"
  fi

  count=0
  while((count<numblocks))
  do
    buffer="@BEGININSERTCODE$((count))@"
    sline=${sblocks[$((count))]}
    eline=${eblocks[$((count))]}

    length=$((eline-sline))
    count2=0
    while((count2<length))
    do
      buffer="${buffer}\\n"
      ((count2++))
    done

    head -$((eline-1)) $file | tail -$((eline-sline-1)) > ${tempfile}_$((count))

    highlight -f -Ll -Sc ${tempfile}_$((count)) -o ${tempfile}_$((count)).tex

    sed -e "$((sline)),$((eline)) c\ ${buffer}@ENDINSERTCODE$((count))@" "$file" > $tempfile
    mv -f $tempfile "$file"
    ((count++))
  done #while((count<numblocks))

  count=0
  while((count<numblocks))
  do
    sed -e "/@BEGININSERTCODE$((count))@/r ${tempfile}_$((count)).tex" "$file" > $tempfile
    mv -f $tempfile "$file"

    sed -e "s/@BEGININSERTCODE$((count))@/\\\\code{ \n/g" "$file" > $tempfile
    mv -f $tempfile "$file"

    sed -e "s/@ENDINSERTCODE$((count))@/}\n/g" "$file" > $tempfile
    mv -f $tempfile "$file"

    ((count++))
  done

  rm -f ${tempfile}*
}



###
#
# The makeDocs() function generates the makefile information for all checker
# documentation.
#
###
makeDocs()
{
  local makefile=$1
  local tempfile=`mktemp`
  local -i highstat=`which highlight >& /dev/null; echo $?`

  echo "Updating checker documentation (running highlight if available) ..."

  echo '
docs: compass.tex
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty
	test -f acknowledgments.tex || ln -s $(srcdir)/acknowledgments.tex acknowledgments.tex
	test -f introduction.tex || ln -s $(srcdir)/introduction.tex introduction.tex
	test -f usingCompass.tex || ln -s $(srcdir)/usingCompass.tex usingCompass.tex
	test -f emacs_screenshot.jpg || ln -s $(srcdir)/emacs_screenshot.jpg emacs_screenshot.jpg
	test -f CompassScreenshot.pdf || ln -s $(srcdir)/CompassScreenshot.pdf CompassScreenshot.pdf
	test -f ToolGear_gui_compass_01.pdf || ln -s $(srcdir)/ToolGear_gui_compass_01.pdf ToolGear_gui_compass_01.pdf
	test -f appendix.tex || ln -s $(srcdir)/appendix.tex appendix.tex' >> ${makefile}

  for dir in ${SUBDIRS[@]} 
  do
  # This runs from the source tree
  # echo -e "Current directory = "`pwd`
  # echo -e "Relevant file = "`ls -l ${COMPASS_PROJECT}/${dir}/${dir}Docs.new.tex`

  # DQ (1/24/2008): Fixed bug specific to running with "make distcheck"
  # Old code: This was a problem for make distcheck
  # num=`wc -l ${COMPASS_PROJECT}/${dir}/${dir}Docs.new.tex | gawk '{print $1}'`
  # if(( $num==1 )); then
  #   genCertDocs ${COMPASS_PROJECT}/${dir}/${dir}Docs.new.tex
  # elif(( $highstat==0 )); then
  #   highlightDocs "${COMPASS_PROJECT}/${dir}/${dir}Docs.new.tex" ${makefile}
  # fi

    num=`wc -l ${dir}/${dir}Docs.new.tex | gawk '{print $1}'`

    if(( $num==1 )); then
         genCertDocs ${dir}/${dir}Docs.new.tex
    elif(( $highstat==0 )); then
         highlightDocs "${dir}/${dir}Docs.new.tex" ${makefile}
    fi

    echo -e "\ttest -f ${dir}Docs.new.tex || ln -s \$(srcdir)/${dir}/${dir}Docs.new.tex ${dir}Docs.new.tex" >> ${makefile}
  done

  echo -e '\ttest -f compassDocs.tex || ln -s $(srcdir)/compassSupport/compassDocs.tex compassDocs.tex
	pdflatex compass.tex
	pdflatex compass.tex' >> ${makefile}

  rm -f ${tempfile}*
} #makeDocs()

###
#
# The generateCompassDocsMakefile() function automatically generates a makefile
# that build the compass documentation without having to run build & configure
#
###
generateCompassDocsMakefile()
{
  local makefile=$1

  echo '# this is an automatically generated file' > ${makefile}
  echo "top_srcdir=${COMPASS_PROJECT}/../../" >> ${makefile}
  echo "srcdir=${COMPASS_PROJECT}/" >> ${makefile}
  echo 'all: docs' >> ${makefile}
  makeDocs ${makefile}
  echo '
clean:
	find . -maxdepth 1 -type l -exec rm {} \;
	rm -f compass.aux compass.toc compass.log compass.pdf compass.out' >> ${makefile}

  for dir in ${SUBDIRS[@]}
  do
    echo -e "\trm -f ${dir}Test1.tex" >> ${makefile}
    echo -e "\trm -f ${dir}Docs.tex" >> ${makefile}
  done
} #generateCompassDocsMakefile <checker docs makefile>

###
#
# The generateCompassDocs() function automatically generates the file
# ${COMPASS_DOCS} that is to be included in projects/compass/compass.tex.in
# instead of the individual Compass Checker documentation
#
###
generateCompassDocs()
{
  local docs=$1

  if [[ -f $docs ]]; then
    echo "Updating $docs..."
    rm -f $docs
  fi

  echo '
\newenvironment{NOINDENT}%
{\noindent\ignorespaces}%
{\par\noindent%
\ignorespacesafterend}
' >> $docs

  for dir in ${SUBDIRS[@]}
  do
    echo "" >> $docs
    echo '\begin{NOINDENT}' >> $docs
    echo -e "\\\newpage\n\\input{${dir}Docs.new}" >> $docs
    echo '\end{NOINDENT}' >> $docs
  done

} #generateCompassDocs() <path to compass.tex.in>

###
#
# The generateCompassSource() function generates the .C file containing the
# function call to generate all Compass Checker Traversals. This is compiled
# with compassMain.C
#
###
generateCompassSource()
{
  local source=$1
  local -i num=0
  local -i i=0

  if [[ -f $source ]]; then
    echo "Updating ${source}..."
    rm -f $source
  fi

echo -e '// This is an automatically generated file
#include <iostream>
#include <rose.h>
#include <vector>
#include "compass.h"
#include "checkers.h"

//AS(1/18/08) Read in the selecting of rules
std::map<std::string, bool > 
readFile( std::string filename){
  std::map<std::string, bool > checkersToUse;
  std::ifstream* file_op = Compass::openFile( filename );

  std::string current_word;
  bool is_first_word = true;
  //read file
  char dummyString[2000];

  while((*file_op) >> current_word){
	//First word denotes what the regular expression should operate
	//upon. Second word denotes the regular expression

	if(  current_word.substr(0,1) == std::string("#") ){
	  //Skip rest of the line if a "#" character is found. This denotes a 
	  //comment
	  file_op->getline(dummyString,2000);

	}else{
	  if(current_word.substr(0,2) == "+:" ){
		checkersToUse[current_word.substr(2)] = true;
	  }else if(current_word.substr(0,2) == "-:" ){
		checkersToUse[current_word.substr(2)] = false;
	  }
	}
  }

  return checkersToUse;
}



void
buildCheckers( std::vector<Compass::TraversalBase*> &retVal, Compass::Parameters &params, Compass::OutputObject &output )
{
' >> $source

for dir in ${USUBDIRS[@]}
do
  echo -e "
    try {
        CompassAnalyses::${dir}::Traversal *traversal;
        traversal = new CompassAnalyses::${dir}::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << \"error initializing checker ${dir}: \" << e.what() << std::endl;
    }" >> $source
done

echo -e '

  //AS(1/18/2008) Remove the rules that has been deselected from the retVal
  std::string ruleFile = Compass::parseString(params["Compass.RuleSelection"]);

  std::map<std::string, bool > ruleSelection = readFile(ruleFile);

  std::vector<Compass::TraversalBase*> ruleDeselected;
  for( std::vector<Compass::TraversalBase*>::reverse_iterator iItr = retVal.rbegin();
	  iItr != retVal.rend(); iItr++ )
  {
	std::map<std::string, bool >::iterator isRuleSelected = ruleSelection.find((*iItr)->getName() );   
      
	if( isRuleSelected == ruleSelection.end() ){
         std::cerr << "Error: It has not been selected if " + (*iItr)->getName() + " should be run." 
		           << std::endl;
		 exit(1);
	}
	if( isRuleSelected->second == false  )
	  ruleDeselected.push_back(*iItr);
  }

   for( std::vector<Compass::TraversalBase*>::iterator iItr = ruleDeselected.begin();
	  iItr != ruleDeselected.end(); iItr++ )
  {
	retVal.erase(std::find(retVal.begin(),retVal.end(),*iItr));
  }



  return;
} //buildCheckers()
' >> $source

} #generateCompassSource() <path to compass main source>

###
#
# The generateCompassHeader() function generates the header file
# ${COMPASS_HEADER} that contains #include directives to the individual
# Compass checker header files
#
###
generateCompassHeader()
{
  local header=$1

  if [[ -f $header ]]; then
    echo "Updating ${header}..."
    rm -f ${header}
  fi

  echo "// This is an automatically generated file" >> $header
  for dir in ${SUBDIRS[@]}
  do
    echo "#include \"${dir}.h\"" >> $header
  done
} #generateCompassHeader() <path to compass main header>

###
#
# The generateMakefile() function generates the file ${COMPASS_MAKEFILE}
# this is included into the Makefile.am of the projects/compass directory
#
###
generateMakefile()
{
  local makefile_am=$1
  local -i subbound=$((${#SUBDIRS[@]}-1))
  local libs=""
  local tests=""
  local incfile=""
  local back=`pwd`

  cd ${COMPASS_PROJECT}

  if [[ -f ${makefile_am} ]]; then
    echo "Updating ${makefile_am}..."
    rm -f ${makefile_am}
  fi # if [[ -f ${makefile_am} ]]

  ## Comment block
  echo -e '# This is a generated file\n' >> ${makefile_am}

  ## COMPASS_CHECKER_DIRECTORIES
  echo -e 'COMPASS_CHECKER_DIRECTORIES = \' >> ${makefile_am}

  count=0
  for dir in ${SUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\t${dir} \\" >> ${makefile_am}
    else
      echo -e "\t${dir}" >> ${makefile_am}
    fi
    ((count++))
  done

  ## COMPASS_INCLUDES
  echo -e '\nCOMPASS_INCLUDES = \' >> ${makefile_am}

  count=0
  for dir in ${SUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\t-I\$(srcdir)/${dir} \\" >> ${makefile_am}
    else
      echo -e "\t-I\$(srcdir)/${dir}" >> ${makefile_am}
    fi
    ((count++))
  done

  ## Checker sources
  echo "" >> ${makefile_am}
  echo 'COMPASS_CHECKER_SRCS = \' >> ${makefile_am}
  count=0
  for dir in ${SUBDIRS[@]}
  do
#    incfile=${dir}/${dir}.inc
#
#    if [[ -f $incfile && `grep -v "^#" $incfile` != '' ]]; then
#      ((count++))
#      continue
#    fi #if [[ -f $incfile && `grep -v "^#" $incfile` != '' ]]

    lib=${USUBDIRS[$((count))]}

    if(( count < subbound )); then
      echo -e "\t${dir}/${dir}.C \\" >> ${makefile_am}
    else
      echo -e "\t${dir}/${dir}.C" >> ${makefile_am}
    fi

    ((count++))
  done

  ## COMPASS_CHECKER_TESTS
  echo -e '\nCOMPASS_CHECKER_TESTS = \' >> ${makefile_am}
  count=0
  for dir in ${SUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\t ${dir}/${dir}Test1.C \\" >> ${makefile_am}
    else
      echo -e "\t ${dir}/${dir}Test1.C" >> ${makefile_am}
    fi
    ((count++))
  done

  ## COMPASS_CHECKER_TEST_LABELS
  echo -e '\nCOMPASS_CHECKER_TEST_LABELS = \' >> ${makefile_am}

  count=0
  for dir in ${USUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\ttest${dir} \\" >> ${makefile_am}
    else
      echo -e "\ttest${dir}" >> ${makefile_am}
    fi
    ((count++))
  done

  ## COMPASS_CHECKER_TEX_FILES
  echo -e '\nCOMPASS_CHECKER_TEX_FILES = \' >> ${makefile_am}

  count=0
  for dir in ${SUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\t${dir}Docs.tex \\" >> ${makefile_am}
    else
      echo -e "\t${dir}Docs.tex" >> ${makefile_am}
    fi
    ((count++))
  done

  makeDocs ${makefile_am}
  echo "" >> ${makefile_am}

  echo -e 'TGUI=\n' >> ${makefile_am}

#  ## test{CHECKER} label
#  count=0
#  for dir in ${SUBDIRS[@]}
#  do
#    echo -e "test${USUBDIRS[$((count))]}: compassMain" >> ${makefile_am}
#    echo -e "\t cp \$(srcdir)/compass_parameters compass_parameters" >> ${makefile_am}
#    echo -e "\t ./compassMain \$(srcdir)/${dir}/${dir}Test1.C \$(TGUI)\n" >> ${makefile_am}
#    ((count++))
#  done

  # test{CHECKER} label changed 12/14/07
  count=0
  for dir in ${SUBDIRS[@]}
  do
    echo -e "test${USUBDIRS[$((count))]}: compassMain" >> ${makefile_am}
    echo -e "\t@echo -e \"\\\\nRunning test${USUBDIRS[$((count))]}\"" >> ${makefile_am}
    echo -e "\tcp -f \$(srcdir)/compass_parameters compass_parameters" >> ${makefile_am}
    
    echo -e "\t@echo -e \"./compassMain \$(srcdir)/${dir}/${dir}Test1.C \$(TGUI)\"" >> ${makefile_am}
    echo -e "\t@if ! test -f \$(srcdir)/${dir}/${dir}TestOutput.txt; then \\
\t\techo \"creating \$(srcdir)/${dir}/${dir}TestOutput.txt\" ;\\
\t\t./compassMain \$(srcdir)/${dir}/${dir}Test1.C \$(TGUI) &> \$(srcdir)/${dir}/${dir}TestOutput.txt ;\\
\t\tif((\$\$?!=0)) ; then \\
\t\t\techo -e \"\\\\nError: compassMain EXIT FAILURE, see \$(srcdir)/${dir}/${dir}TestOutput.txt\" ;\\
\t\t\texit 1 ;\\
\t\tfi ;\\
\t\tcp -f \$(srcdir)/${dir}/${dir}TestOutput.txt \$(top_builddir)/projects/compass/${dir}TestOutput.txt.00 ;\\
\t\tgrep \"${USUBDIRS[$((count))]}:\" \$(top_builddir)/projects/compass/${dir}TestOutput.txt.00 | sed -e "s@\\/.*\\/@@g" > \$(srcdir)/${dir}/${dir}TestOutput.txt ;\\
\t\trm -f \$(top_builddir)/projects/compass/${dir}TestOutput.txt.00 ;\\
\t\tif ! test -e \$(srcdir)/${dir}/${dir}TestOutput.txt; then  \\
\t\t\techo \"error in compass tests: failed to create \$(srcdir)/${dir}/${dir}TestOutput.txt\" ;\\
\t\t\texit 1 ;\\
\t\telif ! test -s \$(srcdir)/${dir}/${dir}TestOutput.txt; then \\
\t\t\tif test -e \$(srcdir)/${dir}/regressionException; then \\
\t\t\t\techo \"Warning this checker uses \`cat \$(srcdir)/${dir}/regressionException\`\" ;\\
\t\t\telse \\
\t\t\t\techo \"error in compass tests: Created empty file \$(srcdir)/${dir}/${dir}TestOutput.txt\" ;\\
\t\t\t\texit 1 ;\\
\t\t\tfi ;\\
\t\tfi ;\\
\telse \\
\t\t./compassMain \$(srcdir)/${dir}/${dir}Test1.C \$(TGUI) &> \$(top_builddir)/projects/compass/${dir}TestOutput.txt ;\\
\t\tif((\$\$?!=0)) ; then \\
\t\t\techo -e \"\\\\nError: compassMain EXIT FAILURE, see \$(top_builddir)/projects/compass/${dir}TestOutput.txt\" ;\\
\t\t\texit 1 ;\\
\t\tfi ;\\
\t\tcp -f \$(top_builddir)/projects/compass/${dir}TestOutput.txt \$(top_builddir)/projects/compass/${dir}TestOutput.txt.00 ;\\
\t\tgrep \"${USUBDIRS[$((count))]}:\" \$(top_builddir)/projects/compass/${dir}TestOutput.txt.00 | sed -e "s@\\/.*\\/@@g" > \$(top_builddir)/projects/compass/${dir}TestOutput.txt ;\\
\t\trm -f \$(top_builddir)/projects/compass/${dir}TestOutput.txt.00 ;\\
\t\tif ! test -s \$(top_builddir)/projects/compass/${dir}TestOutput.txt || ! test -s \$(srcdir)/${dir}/${dir}TestOutput.txt ; then \\
\t\t\tif test -e \$(srcdir)/${dir}/regressionException ; then \\
\t\t\t\techo \"Warning this checker uses \`cat \$(srcdir)/${dir}/regressionException\`\" ;\\
\t\t\tfi ;\\
\t\t\techo \"warning in compass tests: Comparison between empty file \$(top_builddir)/projects/compass/${dir}TestOutput.txt or \$(srcdir)/${dir}/${dir}TestOutput.txt\" ;\\
\t\tfi ;\\
\t\tif ! diff -q \$(top_builddir)/projects/compass/${dir}TestOutput.txt \$(srcdir)/${dir}/${dir}TestOutput.txt ; then \\
\t\t\techo \"error in compass tests: \$(top_builddir)/projects/compass/${dir}TestOutput.txt and \$(srcdir)/${dir}/${dir}TestOutput.txt differ\" ;\\
\t\t\texit 1 ;\\
\t\tfi ;\\
\t\trm \$(top_builddir)/projects/compass/${dir}TestOutput.txt ;\\
\tfi
\t@echo \"done test${dir}\"\n" >> ${makefile_am}

    ((count++))
  done

  ## testAllCheckersSeparately label
  echo -e "\ntestAllCheckersSeparately: \$(COMPASS_CHECKER_TEST_LABELS)\n\t@echo \"\$@ done\"" >> ${makefile_am}

  echo -e "\n#COMPASS CHECKER INCLUDES:\n" >> ${makefile_am}
  ## COMPASS CHECKER INCLUDES
  for dir in ${SUBDIRS[@]}
  do
    incfile=${dir}/${dir}.inc

    if [[ -f $incfile && `grep -v "^#" $incfile` != '' ]]; then
      echo -e "include \$(srcdir)/${dir}/${dir}.inc" >> ${makefile_am}
      CUSTOM_CHECKERS="${CUSTOM_CHECKERS} ${dir}"
    fi #if [[ -f $incfile && `grep -v "^#" $incfile` != '' ]]
  done

  ### 11/7/2007 have compass archive back its own checkers

  echo -e "\nCOMPASS_CHECKER_ARCHIVES = \\" >> ${makefile_am}
  count=0
  for dir in ${USUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\tarchive${dir} \\" >> ${makefile_am}
    else
      echo -e "\tarchive${dir}" >> ${makefile_am}
    fi
    ((count++))
  done

  count=0
  for dir in ${SUBDIRS[@]}
  do
    echo -e "\narchive${USUBDIRS[$((count++))]}:" >> ${makefile_am}
    echo -e "\trm -f \$(srcdir)/compassRepository/${dir}.*" >> ${makefile_am}
    echo -e "\tcd \$(srcdir); tar -zcf compassRepository/${dir}.tar.gz ${dir}" >> ${makefile_am}
    echo -e "\t@echo \"updated ${dir} archive\"" >> ${makefile_am}
  done

  echo -e "\narchiveCheckers: \$(COMPASS_CHECKER_ARCHIVES)" >> ${makefile_am}

  ## 11/7/2007

  cd $back
} #generateMakefileAM()


###############################################################################
extraDistMakefile()
{
  dir=$1

  cat > ${COMPASS_PROJECT}/${dir}/${dir}.makefile <<END
# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL= ROSE-INSTALL-DIRECTORY
#COMPASS_PROJECT = COMPASS-PROJECT-DIRECTORY 
#COMPASS_SUPPORT = \$\{COMPASS_PROJECT\}/compassSupport

CHECKER_NAME = ${dir}Test

LINKER_FLAGS = -L\$(ROSE_INSTALL)/lib -Wl,-rpath \$(ROSE_INSTALL)/lib -lrose -lrt -lfl -L\`pwd\` -Wl,-rpath \`pwd\` -lcompass

all: ${dir}Test

libcompass.so: \$(COMPASS_SUPPORT)/compass.h \$(COMPASS_SUPPORT)/compass.C
	g++ -fPIC -Wall -shared -o \$@ \$(COMPASS_SUPPORT)/compass.C -I\$(ROSE_INSTALL)/include -I\$(COMPASS_SUPPORT)

${dir}Test: ${dir}.C ${dir}Main.C libcompass.so \$(COMPASS_SUPPORT)/compassTestMain.C
	g++ -fPIC -Wall -o \$@ ${dir}.C ${dir}Main.C -I\$(COMPASS_SUPPORT) -I\$(ROSE_INSTALL)/include \$(LINKER_FLAGS)

test: ${dir}Test ${dir}Test1.C
	./${dir}Test ${dir}Test1.C

clean:
	rm -f libcompass.so "${dir}Test"

END

}

extraDist()
{
  for dir in ${SUBDIRS[@]}
  do
    Dir=${COMPASS_PROJECT}/${dir}
    rm -f ${Dir}/Makefile.am ${Dir}/compass.[Ch] ${Dir}/Makefile ${Dir}/compassTestMain.C
    extraDistMakefile ${dir}
  done
} # extraDist

################################################################################
################################################################################
# stuff needs to be done

unset GREP_OPTIONS

if(( $# < 2 )); then
  echo "Usage: `basename $0` <COMPASS SUBMIT DIR> <COMPASS PROJECT DIR> <COMPASS BUILD DIR> <read>
	Summary of options:
	<COMPASS SUBMIT DIR> -- Directory where checker tarballs are located
	<COMPASS PROJECT DIR> -- compass directory under ROSE source tree
	<regenerate> -- optional, specify to regenerate current checkers from file CHECKER_LIST"
  exit 1
fi

if [[ ! -d $CHECKIN_DIRECTORY ]]; then
  echo "Error: $CHECKIN_DIRECTORY is not a valid directory"
  exit 1
fi

if [[ ! -d $COMPASS_PROJECT ]]; then
  echo "Error: $COMPASS_PROJECT is not a valid directory"
  exit 1
fi

if [[ ! -f ${COMPASS_LIST} ]]; then
  touch ${COMPASS_LIST}
elif [[ ${READ_ONLY} == "new" ]]; then
  rm -f ${COMPASS_LIST}
fi # if [[ ! -f ${COMPASS_LIST} ]]

if [[ ${READ_ONLY} == "read" ]]; then
  echo 'Please note, the "read" option is now "regenerate"'
  exit 1
fi

################################################################################

if [[ ${READ_ONLY} != "regenerate" ]]; then
  for checker in $NEW_CHECKERS
  do
      checker_directory=`tar ztf ${checker} | head -1`
      tar -C ${COMPASS_PROJECT} -zkxf ${checker} >& /dev/null
  
#      if [[ -d ${COMPASS_PROJECT}/${checker_directory} ]]; then
#          echo "Updating ${COMPASS_PROJECT}/${checker_directory}..."
#          rm -rf ${COMPASS_PROJECT}/${checker_directory}
#      fi
  
#      echo "Moving ${checker_directory} to ${COMPASS_PROJECT}..."
#      mv -f /tmp/${checker_directory} ${COMPASS_PROJECT}
  
      updateCheckerList `basename ${checker_directory:0:((${#checker_directory}-1))}` ${COMPASS_LIST}
  done
fi # if [[ ${READ_ONLY} != "regenerate" ]]

echo "Sorting ${COMPASS_LIST}..."
sort ${COMPASS_LIST} -o ${COMPASS_LIST}

# for checker in $NEW_CHECKERS do; ...; done
# Updates the compass project directory specified by ${COMPASS_PROJECT} with
# the submitted checkers in ${CHECKIN_DIRECTORY}.
# The list of all checkers ${COMPASS_LIST} is also updated.
  
for checker in `cat ${COMPASS_LIST}`
do
  if [[ `echo $checker | grep -v "^#"` != '' ]]; then
    USUBDIRS[$((SUBDIR_COUNT))]=`echo ${checker:0:1} | gawk '{print toupper($0)}'`${checker:1}
    SUBDIRS[$((SUBDIR_COUNT++))]=$checker
  fi #if [[ `echo $checker | grep -v "^#"` != '' ]], # is comment symbol
done

for dir in ${SUBDIRS[@]}
do
  cp ${COMPASS_PROJECT}/${dir}/${dir}Docs.tex ${COMPASS_PROJECT}/${dir}/${dir}Docs.new.tex
done


# DQ (1/24/2008): Modified to support optional build of compass_makefile.inc
# so that its regeneration would not force Makefile.in to be rebuilt. Without 
# this support, automake is called again which is a problem for "make distcheck".
# generateMakefile ${COMPASS_MAKEINC}
if [[ ${SKIP_COMPASS_MAKEFILE_INC} == "skip_compass_makefile" ]]; then
   echo 'Skipping regeneration of compass_makefile.inc to avoid makefile regeneration of Makefile.in'
else
   generateMakefile ${COMPASS_MAKEINC}
fi

generateCompassHeader ${COMPASS_HEADER}
generateCompassSource ${COMPASS_SOURCE}
generateCompassDocs ${COMPASS_DOCS}
#generateCompassDocsMakefile ${COMPASS_DOCS_MAKEFILE}
concatCompassParameters
#copySupportFiles
printCustomCheckers ${CUSTOM_CHECKERS}
extraDist

################################################################################
