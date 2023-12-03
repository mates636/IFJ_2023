#!/bin/bash

cd ..
make
cd scanner_testcases
for tcase_in in *.in; do
tcase=`echo $tcase_in | cut -f1 -d.`
echo "========== ${tcase} =========="
../test_scanner $tcase_in > ${tcase}.actual
# ../main < $tcase_in
# ERROR=$?
diff ${tcase}.actual ${tcase}.expected
if [ $? -eq 0 ]; then
echo "SUCCESS"
else
echo "MISMATCH"
fi
done