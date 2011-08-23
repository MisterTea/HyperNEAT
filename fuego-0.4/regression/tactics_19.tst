#-----------------------------------------------------------------------------
# Test for necessary tactical moves from Fuego 19x19 games. 
# A control test set with good moves, to make sure not too many tactical
# moves are pruned when working on the  blunder_xxx tests.
# When working on suppressing tactically bad moves, we should keep playing 
# these good ones.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/31423.sgf 59
10 reg_genmove black
#? [E14]

loadsgf sgf/games/2008/CGOS/31423.sgf 79
20 reg_genmove black
#? [N8|N3|O2]*

loadsgf sgf/games/2008/CGOS/31423.sgf 82
30 reg_genmove white
#? [N3]

loadsgf sgf/games/2008/CGOS/31423.sgf 96
40 reg_genmove white
#? [G15|H5]

loadsgf sgf/games/2008/CGOS/31423.sgf 109
50 reg_genmove black
#? [H6]

loadsgf sgf/games/2008/CGOS/31423.sgf 110
60 reg_genmove white
#? [J7]

loadsgf sgf/games/2008/CGOS/31423.sgf 111
70 reg_genmove black
#? [J7]*

loadsgf sgf/games/2008/CGOS/31423.sgf 112
80 reg_genmove white
#? [K6]*

loadsgf sgf/games/2008/CGOS/31423.sgf 114
90 reg_genmove white
#? [N5]

loadsgf sgf/games/2008/CGOS/31423.sgf 116
100 reg_genmove white
#? [O2|M6|M1]*

loadsgf sgf/games/2008/CGOS/31423.sgf 122
110 reg_genmove white
#? [F17|G17]

loadsgf sgf/games/2008/CGOS/31423.sgf 130
120 reg_genmove white
#? [F12]

loadsgf sgf/games/2008/CGOS/31423.sgf 135
130 reg_genmove black
#? [G11]

loadsgf sgf/games/2008/CGOS/31423.sgf 144
140 reg_genmove white
#? [B5]


