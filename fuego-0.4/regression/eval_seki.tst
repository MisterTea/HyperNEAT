#-----------------------------------------------------------------------------
# Seki evaluation tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/seki-evaluation-problem.sgf 53
reg_genmove b
10 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/games/2007/seki-evaluation-problem.sgf 54
reg_genmove w
20 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/193838.sgf 80
reg_genmove w
40 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/193838.sgf 81
reg_genmove b
60 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/193838.sgf 82
reg_genmove w
80 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/games/2008/leela-seki-variation.sgf 45
reg_genmove b
100 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/games/2008/leela-seki-variation.sgf 46
reg_genmove w
120 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 48
reg_genmove w
140 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 58
reg_genmove w
150 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 64
reg_genmove w
160 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 72
reg_genmove w
170 sg_compare_float 0.5 uct_value
#? [1]
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 76
reg_genmove w
180 sg_compare_float 0.5 uct_value
#? [1]
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 78
reg_genmove w
190 sg_compare_float 0.5 uct_value
#? [1]
# finally!

loadsgf sgf/games/2008/seki-miseval.sgf 37
reg_genmove b
200 sg_compare_float 0.5 uct_value
#? [-1]
# B is totally lost because the top corner is seki.
# in playouts, the B selfataries get moved to the other liberty at J7
# but the W selfatari is eventually played.

loadsgf sgf/games/2008/seki-miseval.sgf 47
reg_genmove b
210 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/games/2008/seki-miseval.sgf 57
reg_genmove b
220 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/games/2008/seki-miseval.sgf 59
reg_genmove b
230 sg_compare_float 0.5 uct_value
#? [-1]
# finally!

loadsgf sgf/games/2008/KGS/7/26/Newou-Fuego9.sgf 41
reg_genmove b
260 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/KGS/7/26/Newou-Fuego9.sgf 53
reg_genmove b
270 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/KGS/7/25/PaperTiger-Fuego.sgf 235
reg_genmove b
290 sg_compare_float 0.5 uct_value
#? [-1]
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/KGS/7/25/PaperTiger-Fuego.sgf 345
reg_genmove b
300 sg_compare_float 0.5 uct_value
#? [-1]
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/KGS/7/28/dege-Fuego9.sgf 37
reg_genmove b
320 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 2
reg_genmove b
400 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 4
reg_genmove b
410 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 6
reg_genmove b
420 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 8
reg_genmove b
430 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 10
reg_genmove b
440 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.


loadsgf sgf/seki/sheppard-seki.sgf
reg_genmove b
460 sg_compare_float 0.3 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/seki/sheppard-seki-2.sgf
reg_genmove b
470 sg_compare_float 0.5 uct_value
#? [1]*
# B is winning.

loadsgf sgf/seki/sheppard-seki-3.sgf
reg_genmove w
480 sg_compare_float 0.5 uct_value
#? [-1]
# W is losing.
