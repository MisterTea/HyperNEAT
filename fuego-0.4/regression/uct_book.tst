#-----------------------------------------------------------------------------
# Tests for opening book of GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000
book_load ../book/book.dat

#-----------------------------------------------------------------------------
loadsgf sgf/games/2007/CGOS/200368.sgf 9
10 reg_genmove b
#? [D2]
# E2 seems OK too.

loadsgf sgf/games/2007/CGOS/193991.sgf 2
20 reg_genmove w
#? [E4]

loadsgf sgf/games/2008/CGOS/595283.sgf 5
30 reg_genmove b
#? [E3]
# book move was C4, but this seems refuted by this game.
# See followup tests below.

loadsgf sgf/games/2008/CGOS/595283.sgf 6
40 reg_genmove w
#? [D4]

loadsgf sgf/games/2008/CGOS/595283.sgf 8
50 reg_genmove w
#? [C3]

loadsgf sgf/games/2008/CGOS/595283.sgf 10
60 reg_genmove w
#? [C6]

loadsgf sgf/games/2008/CGOS/595283.sgf 12
70 reg_genmove w
#? [E4]

loadsgf sgf/games/2008/CGOS/595283.sgf 14
80 reg_genmove w
#? [E3]

loadsgf sgf/games/2008/CGOS/595283.sgf 16
90 reg_genmove w
#? [F5]

loadsgf sgf/games/2008/CGOS/595283.sgf 18
100 reg_genmove w
#? [G6]

loadsgf sgf/games/2008/CGOS/595042.sgf 10
110 reg_genmove w
#? [C4]
# This whole line is a big headache. This game seems to refute D8.
# The bottom group is too weak after G3.
# not convinced that C4 is enough, either. Maybe W needs to deviate earlier.
