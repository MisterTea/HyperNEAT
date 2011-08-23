#-----------------------------------------------------------------------------
# Endgame tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/194557.variation.sgf 47
10 reg_genmove b
#? [H1|G1]*
# H1 is good style. G1 seems to work here, too, but messy.

20 sg_compare_float 0.5 uct_value
#? [1]*
# new FAIL 2008-02-19

loadsgf sgf/games/2007/194557.variation.sgf 49
30 reg_genmove b
#? [G1|F1]
# F1 is very odd but also works.

40 sg_compare_float 0.5 uct_value
#? [1]
# it's a win because B gets J1 after filling at F1.

loadsgf sgf/games/2007/194557.variation.sgf 51
50 reg_genmove b
#? [F1]

60 sg_compare_float 0.8 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 17
70 reg_genmove b
#? [H4]
# the simple connection here wins by 0.5. But B has cutting points
# which affect the winning probability for a long time.

80 sg_compare_float 0.5 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 19
90 reg_genmove b
#? [A6|J6]

100 sg_compare_float 0.5 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 21
110 reg_genmove b
#? [J5]

120 sg_compare_float 0.5 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 23
130 reg_genmove b
#? [D4|F3|D3]

140 sg_compare_float 0.5 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 25
150 reg_genmove b
#? [E4]

160 sg_compare_float 0.5 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 27
170 reg_genmove b
#? [C3|C2]

180 sg_compare_float 0.5 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 29
190 reg_genmove b
#? [F4]

200 sg_compare_float 0.5 uct_value
#? [1]*
# new FAIL 20080219 - value 0.47 now

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 31
210 reg_genmove b
#? [B2]

220 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 33
230 reg_genmove b
#? [C1]
# maybe F2 is OK too but C1 is clearest

240 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 35
250 reg_genmove b
#? [D2|B1]

260 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 37
270 reg_genmove b
#? [F2]
# just killing these stones is enough, good aji

280 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 39
290 reg_genmove b
#? [F1|B1]

300 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 41
310 reg_genmove b
#? [E1]

320 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 43
330 reg_genmove b
#? [B3|D2|H7|A5]

340 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 45
350 reg_genmove b
#? [D2]

360 sg_compare_float 0.7 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 47
370 reg_genmove b
#? [G8|A6]*

380 sg_compare_float 0.7 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/200163-variation.sgf 49
390 reg_genmove b
#? [F9|A6]

400 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081112153751.sgf 54
410 reg_genmove w
#? [G4|G5|J1]*
# W has many ways to win but seems to misevaluate this corner
# and lose by 0.5

