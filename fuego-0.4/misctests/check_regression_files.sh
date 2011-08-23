#!/bin/sh
# Check that all SGF files referenced by a test in regression/ exist and that
# regression/Makefile.am contains all tests and SGF files

cd `dirname $0`/../regression

echo -n "Checking that regression/Makefile.am contains all tests... "
ALL_TSTFILES=`find . -name "*.tst" -o -name "*.list" -o -name "*.suite" | sed s,^./,,`
for TSTFILE in $ALL_TSTFILES; do
   if ! grep -q "$TSTFILE" Makefile.am; then
       echo "fail"
       echo "regression/Makefile.am does not contain $TSTFILE"
       exit 1;
   fi
done
echo "ok"

echo -n "Checking that all SGF files referenced by a test exist and are in regression/Makefile.am... "
for SGFFILE in `cat *.tst|awk '{if ($1=="loadsgf") print $2}'`; do
   if ! test -f $SGFFILE; then
       echo "fail"
       echo "Non-existing file $SGFFILE is referenced by a regression test"
       exit 1;
   fi
   if ! grep -q "$SGFFILE" Makefile.am; then
       echo "fail"
       echo "regression/Makefile.am does not contain $SGFFILE"
       exit 1;
   fi
done
echo "ok"
