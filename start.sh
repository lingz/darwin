#!/bin/bash

if [[ $1 = "A" ]]; then
  MODE=1
else
  MODE=0
fi

INFILE=$2
OUTFILE=$3

echo $MODE | cat - $INFILE | ./darwinSolver > $OUTFILE
