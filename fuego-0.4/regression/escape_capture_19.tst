#-----------------------------------------------------------------------------
# Escape and capture performance tests from Fuego 19x19 games.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/31410.sgf 67
10 reg_genmove black
#? [L8]*
# W threatens a loose ladder - prevent it.

loadsgf sgf/games/2008/CGOS/31410.sgf 68
20 reg_genmove white
#? [L8]*
# capture by loose ladder

loadsgf sgf/games/2008/CGOS/31410.sgf 69
30 reg_genmove black
#? [!M7]*

loadsgf sgf/games/2008/CGOS/31410.sgf 70
40 reg_genmove white
#? [L8]*

loadsgf sgf/games/2008/CGOS/31410.sgf 95
50 reg_genmove black
#? [!O7]
# the loose ladder again

loadsgf sgf/games/2008/CGOS/31410.sgf 96
60 reg_genmove white
#? [O8]

loadsgf sgf/games/2008/CGOS/31410.sgf 97
70 reg_genmove black
#? [!P7]

loadsgf sgf/games/2008/CGOS/31410.sgf 98
80 reg_genmove white
#? [P8|P6]

loadsgf sgf/games/2008/CGOS/31410.sgf 123
90 reg_genmove black
#? [!S11]

loadsgf sgf/games/2008/CGOS/31410.sgf 124
100 reg_genmove white
#? [S12]

loadsgf sgf/games/2008/CGOS/31410.sgf 143
110 reg_genmove black
#? [!P6]

loadsgf sgf/games/2008/CGOS/31423.sgf 61
120 reg_genmove black
#? [F14|E15]*
# Save the cutting stone, prevent geta

loadsgf sgf/games/2008/CGOS/31423.sgf 62
130 reg_genmove white
#? [F14|F15|E15]

loadsgf sgf/games/2008/CGOS/31423.sgf 63
140 reg_genmove black
#? [!F14]*

loadsgf sgf/games/2008/CGOS/31423.sgf 64
150 reg_genmove white
#? [G14]

loadsgf sgf/games/2008/CGOS/31423.sgf 65
160 reg_genmove black
#? [!F15]*

loadsgf sgf/games/2008/CGOS/31423.sgf 66
170 reg_genmove white
#? [G16]*

loadsgf sgf/games/2008/CGOS/31423.sgf 77
180 reg_genmove black
#? [!M2]

loadsgf sgf/games/2008/CGOS/31423.sgf 78
190 reg_genmove white
#? [N2]

loadsgf sgf/games/2008/CGOS/31423.sgf 94
200 reg_genmove white
#? [H2|G2|G3|H5]

loadsgf sgf/games/2008/CGOS/31423.sgf 97
210 reg_genmove black
#? [H2]

loadsgf sgf/games/2008/CGOS/31423.sgf 99
220 reg_genmove black
#? [J1|K1]*

loadsgf sgf/games/2008/CGOS/31423.sgf 117
230 reg_genmove black
#? [!B14]

loadsgf sgf/games/2008/CGOS/31423.sgf 118
240 reg_genmove white
#? [B13]*

loadsgf sgf/games/2008/CGOS/31423.sgf 120
250 reg_genmove white
#? [B15]

loadsgf sgf/games/2008/CGOS/31423.sgf 123
260 reg_genmove black
#? [!E2]*

loadsgf sgf/games/2008/CGOS/31423.sgf 181
270 reg_genmove black
#? [P9]*

loadsgf sgf/games/2008/CGOS/31425.sgf 34
280 reg_genmove white
#? [C1]
# A5 was semeai losing blunder.

