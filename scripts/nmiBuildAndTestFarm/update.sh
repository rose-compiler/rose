#!/bin/sh

CWD="${HOME}/rose_nmi"

cd ${CWD}

# output current date, for logging informations
date

# do a update of the test scripts
svn update

#update cronjobs
crontab cronjobs
