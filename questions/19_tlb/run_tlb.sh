#!/usr/bin/env bash

if [ $# -lt 2 ]; then
  echo "Usage: $0 <program_name> <output_datafile>" >&2
  exit 1
fi

prog=./$1
out=$2
plot_body=plot_tlb.gp

echo "# pages  trials  ns_per_access" >"$out"

for pages in 1 2 4 8 16 32 64 128 256 512 1024 2048; do
  trials=$((100000000 / pages))
  trials=$((trials < 1 ? 1 : trials))

  res=$($prog $pages $trials)
  echo "$res" >>"$out"
done

# Generate plots
for fmt in png pdf svg; do
  case $fmt in
  png) term="pngcairo size 800,600" ;;
  pdf) term="pdfcairo size 8,6 enhanced font 'Helvetica,10'" ;;
  svg) term="svg size 800,600" ;;
  esac

  gnuplot -e "datafile='${out}'; set terminal $term; set output 'tlb_plot.${fmt}';" "$plot_body"
done
