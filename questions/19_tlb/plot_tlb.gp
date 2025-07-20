set title "TLB Size Estimation"
set xlabel "Number of Pages"
set ylabel "cycles per access"
set logscale x 2
set grid
set xtics (1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096)

plot datafile using 1:3 with linespoints title "TLB Access Time"

