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
COMPASS_PROJECT=$1

declare -a SUBDIRS
declare -a SUBDIR_COUNT=0

################################################################################

genCertDocs()
{
  local file=$1
  local href=`cat $file`
  local code=`basename $href | awk -F. '{print $1}'`
  local tex=`ls ${CMDROOT}/seccode/*$code*.tex`
  local tempfile=`mktemp -t compassXXXXXX`

  if [[ -f $tex ]]; then
    cat $tex
  else
    wget $href
    mv -f `basename $href | sed -e 's/%28/(/g' | sed -e 's/%29/)/g' | sed -e 's/%20/ /g'` $tempfile.html
    ${CMDROOT}/wiki2tex/bin/wiki2tex.sh $tempfile.html
    cat `basename ${tempfile}.tex`
  fi

  rm -f ${tempfile}.html ${tempfile}* `basename $href` wiki2tex.xsl
} #genCertDoc


highlightDocs()
{
  local file=$1
  local file2=`mktemp -t compassXXXXXX`
  local tempfile=`mktemp -t compassXXXXXX`
  local -a sblocks
  local -a eblocks
  local -i count=0
  local -i count2=0
  local -i sline=0
  local -i eline=0
  local buffer=""

  cp "$file" "$file2"
  sed -e '/^\%/ d' "$file2" > $tempfile
  mv -f $tempfile "$file2"

  count=0
  for sl in `grep -n '\\\begin{verbatim}' "$file2" | awk -F: '{print $1}'`
  do
    sblocks[$((count++))]=$sl
  done

  count=0
  for el in `grep -n '\\end{verbatim}' "$file2" | awk -F: '{print $1}'`
  do
    eblocks[$((count++))]=$el
  done

  local -i numblocks=${#sblocks[@]}

  if(($numblocks != ${#eblocks[*]})); then
    echo "Error: Mismatch in code blocks for $file2"
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

    head -$((eline-1)) $file2 | tail -$((eline-sline-1)) > ${tempfile}_$((count))

    highlight -f -Ll -Sc ${tempfile}_$((count)) -o ${tempfile}_$((count)).tex

    sed -e "$((sline)),$((eline)) c\ ${buffer}@ENDINSERTCODE$((count))@" "$file2" > $tempfile
    mv -f $tempfile "$file2"
    ((count++))
  done #while((count<numblocks))

  count=0
  while((count<numblocks))
  do
    sed -e "/@BEGININSERTCODE$((count))@/r ${tempfile}_$((count)).tex" "$file2" > $tempfile
    mv -f $tempfile "$file2"

    sed -e "s/@BEGININSERTCODE$((count))@/\\\\code{ \n/g" "$file2" > $tempfile
    mv -f $tempfile "$file2"

    sed -e "s/@ENDINSERTCODE$((count))@/}\n/g" "$file2" > $tempfile
    mv -f $tempfile "$file2"

    ((count++))
  done

  cat ${file2}
  rm -f ${tempfile}* ${file2}
}



###
#
# The makeDocs() function generates the makefile information for all checker
# documentation.
#
###
makeDocs()
{
  local tempfile=`mktemp -t compassXXXXXX`
  local -i highstat=`which highlight >& /dev/null; echo $?`

#  echo "Updating checker documentation (running highlight if available) ..."

  while read dir
  do
  # This runs from the source tree
  # echo -e "Current directory = "`pwd`
  # echo -e "Relevant file = "`ls -l ${COMPASS_PROJECT}/${dir}/${dir}Docs.new.tex`

  # DQ (1/24/2008): Fixed bug specific to running with "make distcheck"
  # Old code: This was a problem for make distcheck
  # num=`wc -l ${COMPASS_PROJECT}/${dir}/${dir}Docs.new.tex | awk '{print $1}'`
  # if(( $num == 1 )); then
  #   genCertDocs ${COMPASS_PROJECT}/${dir}/${dir}Docs.new.tex
  # elif(( $highstat==0 )); then
  #   highlightDocs "${COMPASS_PROJECT}/${dir}/${dir}Docs.new.tex" ${makefile}
  # fi

    num=`wc -l ${COMPASS_PROJECT}/${dir}/${dir}Docs.tex | awk '{print $1}'`

    echo '\begin{NOINDENT}'
    echo '\newpage'

    if(( $num == 1 )); then
         genCertDocs ${COMPASS_PROJECT}/${dir}/${dir}Docs.tex
    elif(( $highstat==0 )); then
         highlightDocs "${COMPASS_PROJECT}/${dir}/${dir}Docs.tex"
    else
         cat "${COMPASS_PROJECT}/${dir}/${dir}Docs.tex"
    fi

    echo '\end{NOINDENT}'

  done

  rm -f ${tempfile}*
} #makeDocs()

################################################################################
################################################################################
# stuff needs to be done

unset GREP_OPTIONS

if(( $# < 1 )); then
  echo "Usage: `basename $0` <COMPASS PROJECT DIR>
	Summary of options:
	<COMPASS PROJECT DIR> -- compass directory under ROSE source tree"
  exit 1
fi

if [[ ! -d $COMPASS_PROJECT ]]; then
  echo "Error: $COMPASS_PROJECT is not a valid directory"
  exit 1
fi

cat <<'END'
\newenvironment{NOINDENT}%
{\noindent\ignorespaces}%
{\par\noindent%
\ignorespacesafterend}
END

makeDocs
