#!/bin/sh -v
set -e

cd /export/0/tmp.saebjornsen1/DATA/system32
date
( \
find . -name "expanded-*" -print |
while read filename; do
  echo "FILE: $filename"
  /home/willcock2/clone-detection/BinaryCloneDetection/createVectorsBinary --openFile $filename --database /export/0/tmp.saebjornsen1/DATA/system32/new-windowsXP-500-1.sql --windowSize 500 --stride 1
  echo "FILEFINISHED: $filename $?"
done \
) >& new-windowsXP-500-1-log.txt 
date
( \
find . -name "expanded-*" -print |
while read filename; do
  echo "FILE: $filename"
  /home/willcock2/clone-detection/BinaryCloneDetection/createVectorsBinary --openFile $filename --database /export/0/tmp.saebjornsen1/DATA/system32/new-windowsXP-200-1.sql --windowSize 200 --stride 1
  echo "FILEFINISHED: $filename $?"
done \
) >& new-windowsXP-200-1-log.txt 
date
# ( \
# find . -name "expanded-*" -print |
# while read filename; do
#   echo "FILE: $filename"
#   /home/willcock2/clone-detection/BinaryCloneDetection/createVectorsBinary --openFile $filename --database /export/0/tmp.saebjornsen1/DATA/system32/new-windowsXP-120-1.sql --windowSize 120 --stride 1
#   echo "FILEFINISHED: $filename $?"
# done \
# ) >& new-windowsXP-120-1-log.txt 
# date
# ( \
# find . -name "expanded-*" -print |
# while read filename; do
#   echo "FILE: $filename"
#   /home/willcock2/clone-detection/BinaryCloneDetection/createVectorsBinary --openFile $filename --database /export/0/tmp.saebjornsen1/DATA/system32/new-windowsXP-80-1.sql --windowSize 80 --stride 1
#   echo "FILEFINISHED: $filename $?"
# done \
# ) >& new-windowsXP-80-1-log.txt 
# date
# ( \
# find . -name "expanded-*" -print |
# while read filename; do
#   echo "FILE: $filename"
#   /home/willcock2/clone-detection/BinaryCloneDetection/createVectorsBinary --openFile $filename --database /export/0/tmp.saebjornsen1/DATA/system32/new-windowsXP-40-1.sql --windowSize 40 --stride 1
#   echo "FILEFINISHED: $filename $?"
# done \
# ) >& new-windowsXP-40-1-log.txt 
# date
# find . -name "expanded-*" -exec /home/willcock2/clone-detection/BinaryCloneDetection/createVectorsBinary --openFile {} --database /export/0/tmp.saebjornsen1/DATA/system32/new-windowsXP-80-1.sql --windowSize 80 --stride 1 \; >& new-windowsXP-80-1-log.txt 
# date
# find . -name "expanded-*" -exec /home/willcock2/clone-detection/BinaryCloneDetection/createVectorsBinary --openFile {} --database /export/0/tmp.saebjornsen1/DATA/system32/new-windowsXP-40-1.sql --windowSize 40 --stride 1 \; >& new-windowsXP-40-1-log.txt 
# date
# find . -name "expanded-*" -exec /home/willcock2/clone-detection/BinaryCloneDetection/createVectorsBinary --openFile {} --database /export/0/tmp.saebjornsen1/DATA/system32/new-windowsXP-20-1.sql --windowSize 20 --stride 1 \; >& new-windowsXP-20-1-log.txt 
# date


