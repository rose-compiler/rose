
# Check command line for option file
if [[ "$1" == "" || !( -f $1 ) ]]
then
	echo "Usage: $0 <option_file>"
	exit 1
fi

. $1

# Check if all necessary options are set

if [[ "$TITLE" == "" ]]
then
	echo "please set TITLE in your option file"
	exit 1
fi

if [[ "$DESCRIPTION" == "" ]]
then
	echo "please set DESCRIPTION in your option file"
	exit 1
fi

if [[ "$PLATFORMS" == "" ]]
then
	echo "please set PLATFORMS in yout option file"
	exit 1
fi

# optional options, however most likely these need to be set
# a warning is emitted
if [[ "$PREREQS" == "" ]]
then
	echo "WARNING: PREREQS is not set"
fi

if [[ "$CONFIGURE_OPTIONS" == "" ]]
then
	echo "WARNING: CONFIGURE_OPTIONS is not set"
fi

# Determine HEAD revision number
REVISION=`svn -r HEAD log https://outreach.scidac.gov/svn/rose/trunk/ | grep '^Load rose' | cut -d'-' -f 3 | cut -d' ' -f 1`

# exporting options, to be recognized in the submit and input scripts
export _NMI_TITLE=${TITLE}
export _NMI_DESCRIPTION=${DESCRIPTION}
export _NMI_PLATFORMS=${PLATFORMS}
export _NMI_PREREQS=${PREREQS}
export _NMI_CONFIGURE_OPTIONS=${CONFIGURE_OPTIONS}

export _NMI_REVISION=${REVISION}

export _NMI_HOME=$HOME
export _NMI_HOSTNAME=$HOSTNAME
export _NMI_SUBMITDIR=$PWD


# submit the build job
echo "Submitting the build job ..."
nmi_submit rose-build.submit > .build_submit.log
RID=`grep "Run ID:" .build_submit.log | cut -f 3`
GID=`grep "Global ID:" .build_submit.log | cut -f 2`
cat .build_submit.log

# export Run ID and Global to be recognized by the test input file
export _NMI_RID=${RID}
export _NMI_GID=${GID}

# submit the test job
echo -e "\nSubmitting the test job ..." 
nmi_submit rose-test.submit 

exit


#3echo $_NMI_RID

# next two lines are needed so that the cronjob works
# and we can define our jobs to look like 
# ./submit.sh submit/<submit_file> in the cronjob file
# CWD="/home/heller/rose"
# cd ${CWD}

#nmi_submit $1 > submit_out.tmp

#echo -e "Run ID:"
#grep "Run ID:" submit_out.tmp | cut -f 3
#echo -e "Global ID:"
#grep "Global ID:" submit_out.tmp | cut -f 2

#echo -e "Whole output"
#cat submit_out.tmp
