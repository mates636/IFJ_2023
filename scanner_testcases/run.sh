#!/bin/bash

for tcase_in in *.in; do
tcase=`echo $tcase_in | cut -f1 -d.`
echo "========== ${tcase} =========="
../test_scanner $tcase_in > ${tcase}.actual
diff ${tcase}.actual ${tcase}.expected
if [ $? -eq 0 ]; then
echo "SUCCESS"
else
echo "MISMATCH"
fi
done