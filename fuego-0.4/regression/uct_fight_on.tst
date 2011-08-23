#-----------------------------------------------------------------------------
# Fight on when behind - create chances - tests for GoUctGlobalSearchPlayer.
# Also test cases where only one move avoids immediate disaster, e.g.
# connect against a big atari in unfavorable position.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/CGOS/193893.sgf 26
10 reg_genmove w
#? [G3]*
# best chance to live somehow

loadsgf sgf/games/2007/CGOS/200058.sgf 18
20 reg_genmove w
#? [D4]
# connect now, look for chances later.
