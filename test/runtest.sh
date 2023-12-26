#!/bin/zsh

CWD=$(pwd)

echo "Working dir : ${CWD}"

for f in $CWD/test/*.test; do ${f} --verbose=1; done;
