set title "Tiempo para que se cargue un 10% de un video de 5 minutos frente a la tasa del canal simulando una caida en el enlace entre el sw1 y el sw3 del POD1"
set xlabel "Tasa del canal (Mbps)"
set ylabel "Tiempo (ms)"
plot "-"  title " " with yerrorlines
10 1754.73 0.389716
17 1271.47 0.285973
25 1068.2 0.229289
32 1004.2 1.37573
40 1005 1.62132
47 1005.73 1.64101
55 1002.87 1.21809
62 1004.07 1.39576
70 1004 1.52381
77 1004.4 1.33697
85 1003.8 1.87447
92 1003.73 1.75705
100 1004.53 1.64634
107 1003.13 1.41756
115 1004.2 1.45317
122 1004.6 1.68492
130 1004.33 1.75954
137 1004.07 1.64101
145 1004.73 1.57285
152 1004 1.4041
e
pause -1
