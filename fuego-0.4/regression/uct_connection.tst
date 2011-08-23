#-----------------------------------------------------------------------------
# Tests for generating connecting moves for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000
book_load ../book/book.dat

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/CGOS/193991.sgf 14
10 reg_genmove w
#? [C2]
# only move to connect

loadsgf sgf/games/2007/CGOS/200190.sgf 12
20 reg_genmove w
#? [F4|G3]
