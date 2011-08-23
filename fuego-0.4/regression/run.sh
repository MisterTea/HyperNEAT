#!/bin/bash
#-----------------------------------------------------------------------------
# Run regression tests
#-----------------------------------------------------------------------------

RESULT_DIR="html"
TESTSUITE_DEFAULT="basics.suite"
# You can invoke this script with "env FUEGO_BUILD_ROOT=... run.sh" or it
# will automatically detect the build root, if the build was done in the
# Fuego root directory or in build/autotools/debug
if [[ "$FUEGO_BUILD_ROOT" == "" ]]; then
    if test -x ../fuegomain/fuego; then
        FUEGO_BUILD_ROOT=".."
    elif test -x ../build/autotools/debug/fuegomain/fuego; then
        FUEGO_BUILD_ROOT="../build/autotools/debug"
    fi
fi
FUEGO="$FUEGO_BUILD_ROOT/fuegomain/fuego --nobook --srand 1"
FUEGOTEST="$FUEGO_BUILD_ROOT/fuegotest/fuego_test --srand 1"

#-----------------------------------------------------------------------------
# Functions
#-----------------------------------------------------------------------------

usage() {
    cat >&2 <<EOF
Usage: $0 [options] [testsuite]

Options:
  -d Append date to name of ouput directory
  -h Print help and exit
  -l Long output
  -p Program, using full command or a program abbreviation
     (fuego,fuego_test). Default is fuego.
  -t Single test file to run (without extension .tst or .list)

The test files are expected to have the file endings .tst or .list.
Lists can contain other lists (file names prepended with @, see
gogui-regress documentation).

The test and program to run can be either specified with the -p and -t
options or by giving a test suite argument to allow runs with different
programs. A test suite is a file containing rows with a test file name and
a program name (only program abbreviations are allowed). If neither is
specified, the test suite $TESTSUITE_DEFAULT is used.

This script returns 0, if no test fails.
EOF
}

# Set variables PROGRAM_CMD and RESULT_EXT depending on PROGRAM
setprogram() {
    case "$PROGRAM" in
	fuego)
	    PROGRAM_CMD="$FUEGO"
	    RESULT_EXT="fuego"
	    ;;
	fuego_test)
	    PROGRAM_CMD="$FUEGOTEST"
	    RESULT_EXT="fuego_test"
	    ;;
	*)
	    PROGRAM_CMD="$PROGRAM"
	    RESULT_EXT=""
	    ;;
    esac
}

# Set TESTARG from TESTFILE.
# Adds extension .tst or .list, if a file with that name exists. Prepends
# the argument with @ for lists as required by gogui-regress
settestarg() {
    FILE_EXT=${TESTFILE##*.}
    if [[ "$FILE_EXT" == "$TESTFILE" && -f "$TESTFILE.list" ]]; then
	FILE_EXT="list"
	TESTFILE="$TESTFILE.list"
    fi
    
    if [[ "$FILE_EXT" == "list" ]]; then
	TESTARG="@$TESTFILE"
    elif [[ "$FILE_EXT" == "tst" ]]; then
	TESTARG="$TESTFILE"
    else
	echo >&2 "Invalid test name '$TESTFILE'"
	exit 1
    fi
}

runtest() {
    PROGRAM="$1"
    TESTFILE="$2"

    setprogram
    settestarg

    TEST_SUBDIR=$TESTFILE
    # If the test case name is the name of a single test file, use only the
    # file name for the name of the subdirectory in RESULT_DIR, not the full
    # path (allows to specify test files in parent or sibling directories
    # and the output will still be in RESULT_DIR)
    if [[ -f "$TESTFILE" ]]; then
	TEST_SUBDIR=$(basename $TESTFILE)
    fi
    TEST_DIR="$RESULT_DIR/$TEST_SUBDIR"
    if [[ "$RESULT_EXT" != "" ]]; then
	TEST_DIR="$TEST_DIR-$RESULT_EXT"
    fi
    if (( $APPEND_DATE != 0 )); then
        DATE=$(date +'%Y%m%d')
	TEST_DIR="$TEST_DIR-$DATE"
    fi
    mkdir -p "$TEST_DIR"
    
    if (( $LONG_OUTPUT != 0 )); then
	OPTIONS="-long"
    fi
    cat >&2 <<EOF
Program: '$PROGRAM_CMD'
Test: $TESTARG
Output:  $TEST_DIR/index.html
EOF
    gogui-regress $OPTIONS -output "$TEST_DIR" "$PROGRAM_CMD" "$TESTARG"
    if [[ "$?" != 0 ]]; then
	RESULT=1
    else
	echo "OK"
    fi
    echo
}

#-----------------------------------------------------------------------------
# Parse options and arguments
#-----------------------------------------------------------------------------

LONG_OUTPUT=0
APPEND_DATE=0
PROGRAM=""
TESTFILE=""
while getopts "dhlp:t:" OPT; do
case "$OPT" in
    d)   APPEND_DATE=1;;
    h)   usage; exit 0;;
    l)   LONG_OUTPUT=1;;
    p)   PROGRAM="$OPTARG";;
    t)   TESTFILE="$OPTARG";;
    [?]) usage; exit 1;;
esac
done

TESTSUITE=""
shift $(($OPTIND - 1))
if (( $# > 1 )); then
    usage
    exit 1
fi
if (( $# == 1 )); then
    if [[ "$TESTFILE" != "" || "$PROGRAM" != "" ]]; then
	echo >&2 "Cannot use both a test suite and a test file/program"
	exit 1
    fi
    TESTSUITE="$1"
else
    if [[ "$TESTFILE" == "" && "$PROGRAM" == "" ]]; then
	TESTSUITE="$TESTSUITE_DEFAULT"
    elif [[ "$TESTFILE" == "" || "$PROGRAM" == "" ]]; then
	echo >&2 "Need either test suite or test file/program"
	exit 1
    fi
fi

#-----------------------------------------------------------------------------
# Run tests
#-----------------------------------------------------------------------------

RESULT=0
if [[ "$TESTSUITE" != "" ]]; then
    if [[ ! -f "$TESTSUITE" ]]; then
	echo >&2 "File '$TESTSUITE' not found"
	exit 1
    fi
    LINENO=0
    while read LINE; do
	LINENO=$(( LINENO + 1 ))
	shopt -s extglob
	if [[ "${LINE%%*( )#*}" == "" || "${LINE%%*( )}" == "" ]]; then
	    continue # Skip comment and empty lines
	fi
	runtest $LINE
    done < "$TESTSUITE"
else
    runtest "$PROGRAM" "$TESTFILE"
fi
if [[ $RESULT == 0 ]]; then
    echo "*** ALL OK ***"
else
    echo "*** TEST FAILS OCCURRED ***"
fi

exit $RESULT

#-----------------------------------------------------------------------------
