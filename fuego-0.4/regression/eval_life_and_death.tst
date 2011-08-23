#-----------------------------------------------------------------------------
# Life and Death tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
loadsgf sgf/games/pro-9x9/zhou9a.sgf 60
reg_genmove w
10 sg_compare_float 0.4 uct_value
#? [-1]*
# W is dead and completely lost.

loadsgf sgf/games/2008/CGOS/248520.sgf 25
reg_genmove b
20 sg_compare_float 0.5 uct_value
#? [1]
# B wins.

loadsgf sgf/games/2008/CGOS/248520.sgf 31
reg_genmove b
40 sg_compare_float 0.5 uct_value
#? [1]
# B wins.

loadsgf sgf/games/2008/CGOS/248520.sgf 43
reg_genmove b
60 sg_compare_float 0.5 uct_value
#? [1]
# B wins.

loadsgf sgf/games/2008/mogo-ko+ld-problem-variation.sgf 35
reg_genmove w
70 sg_compare_float 0.5 uct_value
#? [-1]
# W lost, but program does not see it
# Only after Black E2 the value drops
# new PASS 20081106

loadsgf sgf/games/2008/mogo-ko+ld-problem-variation.sgf 37
reg_genmove w
80 sg_compare_float 0.2 uct_value
#? [-1]
# After Black E2 the program sees the loss

loadsgf sgf/lifeanddeath/4x4nakade.sgf
reg_genmove w
100 sg_compare_float 0.8 uct_value
#? [1]
# apparently Mogo has some trouble with this one, thinks the b group is alive.

loadsgf sgf/lifeanddeath/eye-tricky.sgf 1
reg_genmove b
#? [D3]
120 sg_compare_float 0.9 uct_value
#? [1]

reg_genmove w
140 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/lifeanddeath/eye-tricky.sgf 2
reg_genmove w
160 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/lifeanddeath/eye-tricky.sgf 4
reg_genmove w
180 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/lifeanddeath/eye-tricky.sgf 6
reg_genmove w
200 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/games/2008/CGOS/365007.sgf 52
reg_genmove w
410 sg_compare_float 0.7 uct_value
#? [1]*
# cannot solve the Life&Death problem - value too low.

