#-----------------------------------------------------------------------------
# Short ladder blunders from Fuego 19x19 games. 
# Bad moves that can be captured in a short ladder.
# Capture or escape blunders that could be avoided by reading a short
# (1-5 moves) ladder.
# todo: control test set with good such moves. (Are there any?)
#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/31410.sgf 135
10 reg_genmove black
#? [!L19]

loadsgf sgf/games/2008/CGOS/31410.sgf 181
20 reg_genmove black
#? [!R5]

loadsgf sgf/games/2008/CGOS/31423.sgf 101
30 reg_genmove black
#? [!M5]

loadsgf sgf/games/2008/CGOS/31423.sgf 121
40 reg_genmove black
#? [!F16]

loadsgf sgf/games/2008/CGOS/31423.sgf 125
50 reg_genmove black
#? [!A14]

loadsgf sgf/games/2008/CGOS/31423.sgf 183
60 reg_genmove black
#? [!Q8]

loadsgf sgf/games/2008/CGOS/31423.sgf 185
70 reg_genmove black
#? [!Q7]

