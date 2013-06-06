#!/bin/sh
find . -name expanded-\* | while read dir; do echo $dir; rawname=`cut -f 2 < ${dir}/modules.tsv`; mv $dir `dirname $dir`/${rawname}-tsv; done
