#!/bin/bash

# Script for playing the AverageLib simple player on 9x9 CGOS

AVERAGELIB="../../build/autotools/release/fuegotest/fuego_test -player average -config config-average.gtp"
NAME=AverageLib

echo "Enter CGOS password for $NAME:"
read PASSWORD

# Append 2>/dev/stderr to invocation, otherwise cgos3.tcl will not pass
# through stderr of the Go program
./cgos3.tcl "$NAME" "$PASSWORD" \
  "$AVERAGELIB 2>/dev/stderr" \
  gracefully_exit_server-average-9
