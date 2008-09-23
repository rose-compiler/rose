#!/usr/bin/env bash
################################################################################
#
# Date: August 2008
# File: gen_prerequisite.sh
# Author: 
# Brickbats go to Gary Yuan, Jeremiah Willcock, Thomas Panas, and Dan Quinlan
#
################################################################################

if [[ "$*" == "" ]] ;
then
  echo "Usage: $0 <project description>" 1>&2
  echo "Example: $0 auxiliary info" 1>&2
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

CLASS_NAME_PREFIX="${CAMEL_CASE_BEGINNING_WITH_UPPER_CASE}"
FILE_NAME_PREFIX="${CAMEL_CASE_BEGINNING_WITH_LOWER_CASE}"
IFDEF_PREFIX=${ALL_CAPITALS_WITH_UNDERSCORE_DELIMITERS}
PROJECT_DESCRIPTION_PREFIX="${CAPITALIZED_WORDS_WITH_SPACE_DELIMITERS}"
USER_NAME=`perl -e '@X = getpwuid($<); print $X[6]' |  sed 's/[0-9]\+//'`
DATE=`date +"%d-%B-%Y"`
SOURCE_DIRECTORY_NAME=${CAMEL_CASE_BEGINNING_WITH_LOWER_CASE}

cat > ./${CLASS_NAME_PREFIX}Prerequisite.h <<END
class ${CLASS_NAME_PREFIX}Prerequisite: public Prerequisite
{
  private:
    //Your Prerequisite class data members go here
  public:
    ${CLASS_NAME_PREFIX}Prerequisite() : Prerequisite("${CLASS_NAME_PREFIX}")
    {
    } //${CLASS_NAME_PREFIX}Prerequisite()

    void run(/*Your data member pointer goes here*/)
    {
      if(done) return;

      //Your prerequisite initialization code
      done = true;
    } //run()

    PrerequisiteList getPrerequisites() const 
    {
      return PrerequisiteList();
    } //getPrerequisites()
}; //class ${CLASS_NAME_PREFIX}Prerequisite

extern ${CLASS_NAME_PREFIX}Prerequisite ${FILE_NAME_PREFIX}Prerequisite;
END

echo "Generated ${CLASS_NAME_PREFIX}Prerequisite.h"
exit 0
