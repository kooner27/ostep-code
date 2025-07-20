#!/usr/bin/env bash

# This script runs the TLB test just once with 1024 pages
# Make sure test2 is compiled

prog=./test2
pages=1024
trials=$((100000000 / pages)) # ensures roughly 100 million total accesses

echo "Running with pages=$pages, trials=$trials"

# Run and capture both stdout and stderr
res=$($prog $pages $trials 2>&1)

# Show result
if [[ $? -eq 0 ]]; then
  echo "Success:"
  echo -e "$res"
else
  echo "Failed:"
  echo -e "$res"
fi
