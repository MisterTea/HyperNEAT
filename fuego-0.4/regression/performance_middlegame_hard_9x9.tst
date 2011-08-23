#-----------------------------------------------------------------------------
# Middle game test cases from real games that do not fit a more specific test.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2010/CGOS/1072936-variation.sgf 12
10 reg_genmove w
#? [D2]*
# move recommended by Yu Ping


loadsgf sgf/games/2010/CGOS/1072936-variation.sgf 14
20 reg_genmove w
#? [E7]*
# move recommended by Yu Ping


loadsgf sgf/games/2010/CGOS/1072936-variation.sgf 16
30 reg_genmove w
#? [G7]
# move recommended by Yu Ping

#-----------------------------------------------------------------------------

loadsgf sgf/games/2010/CGOS/1072936-variation2.sgf 22
40 reg_genmove w
#? [B4]*
# move recommended by Yu Ping

loadsgf sgf/games/2010/CGOS/1072936-variation2.sgf 24
50 reg_genmove w
#? [C4]*
# move recommended by Yu Ping

loadsgf sgf/games/2010/CGOS/1072936-variation2.sgf 26
60 reg_genmove w
#? [B5]

loadsgf sgf/games/2010/CGOS/1072936-variation2.sgf 28
70 reg_genmove w
#? [C8]*

loadsgf sgf/games/2010/CGOS/1072936-variation2.sgf 30
80 reg_genmove w
#? [G5]

loadsgf sgf/games/2010/CGOS/1072936-variation2.sgf 32
90 reg_genmove w
#? [F4]

loadsgf sgf/games/2010/CGOS/1072936-variation2.sgf 34
100 reg_genmove w
#? [G2]

loadsgf sgf/games/2010/CGOS/1072936-variation2.sgf 36
110 reg_genmove w
#? [H4|H6]

loadsgf sgf/games/2010/CGOS/1072936-variation2.sgf 38
120 reg_genmove w
#? [H6]


#-----------------------------------------------------------------------------

uct_stat_player
# print player statistics to log file
