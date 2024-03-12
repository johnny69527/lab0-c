set ylabel 'time(sec)'
set xlabel 'size of list'
set title 'performance comparison (sorted list)'
set term png enhanced font 'Verdana,10'
set output 'sort_perf_qwc.png'
plot "linux_sort_qwc.dat" title "linux list sort" w lp, "merge_sort_qwc.dat" title "merge sort" w lp, "quick_sort_qwc.dat" title "quick sort" w lp, "tim_sort_qwc.dat" title "tim sort" w lp

