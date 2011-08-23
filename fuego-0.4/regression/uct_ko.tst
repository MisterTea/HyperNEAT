#-----------------------------------------------------------------------------
# Ko fighting tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/CGOS/199550.sgf 29
10 reg_genmove b
#? [D4]*
# B can play this ko fight to kill W

loadsgf sgf/games/2008/CGOS/251434-variation.sgf 68
20 reg_genmove w
#? [E1]*
# komi changed to 5.5 to make this an interesting problem.
# W can play this ko fight.

loadsgf sgf/games/2008/CGOS/251434-variation.sgf 69
30 reg_genmove b
#? [G7]*
# only threat.

loadsgf sgf/games/2008/CGOS/251434-variation.sgf 71
40 reg_genmove b
#? [F1]*

loadsgf sgf/games/2008/CGOS/251434-variation.sgf 72
50 reg_genmove w
#? [B3|A4|A3]
# W has threats here.

loadsgf sgf/games/2008/CGOS/251434-variation.sgf 73
60 reg_genmove b
#? [E1]*
# if B answers there are no more threats

loadsgf sgf/games/2008/CGOS/251434-variation.sgf 74
70 reg_genmove w
#? [A4]
# W has threats here.

loadsgf sgf/games/2007/CGOS/107439.sgf 29
80 reg_genmove b
#? [D3|G2]*
# avoid ko here, it is dangerous

loadsgf sgf/games/2007/CGOS/189715.sgf 73
90 reg_genmove b
#? [A3]
# safer, keeps ko simple

100 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/189715.sgf 77
110 reg_genmove b
#? [B4|E3]*
# B4 is best, win by double ko. E3 also wins game.

120 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2008/ko-bug.sgf 2
reg_genmove b

130 sg_compare_float 0.3 uct_value
#? [-1]*
# B is lost.

loadsgf sgf/games/2008/ko-bug.sgf 4
140 reg_genmove w
#? [A9]*

150 sg_compare_float 0.9 uct_value
#? [1]*

loadsgf sgf/games/2008/CGOS/386193-mogo-mannenko-bug.sgf 88
160 reg_genmove w
#? [H9]
# Mogo destroyed the mannen-ko, which is seki in this case.

170 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/games/2008/CGOS/397892.sgf 48
180 reg_genmove w
#? [C8]*
# C8 wins the ko and the game. It looks like fuego does not generate E9
# for black in the search.

loadsgf sgf/games/2009/MM-Fuego20090124-variation.sgf 54
190 reg_genmove w
#? [J7]*
# Ko threat, often not generated because of prior knowledge

loadsgf sgf/ko/remove-ko-threat.1.sgf 2
200 reg_genmove w
#? [H7]
# Eliminate ko threat.

