set ylabel 'time(sec)'
set xlabel 'size of list'
set title 'performance comparison'
set term png enhanced font 'Verdana,10'
set output 'sort_perf.png'
plot "linux_sort.dat" title "linux list sort" w lp, "merge_sort.dat" title "merge sort" w lp

