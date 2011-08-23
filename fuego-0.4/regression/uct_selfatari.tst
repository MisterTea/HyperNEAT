#-----------------------------------------------------------------------------
# Tests involving self-atari.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/lifeanddeath/good-selfatari.1.sgf
10 reg_genmove w
#? [B9]

20 reg_genmove b
#? [B9]

loadsgf sgf/lifeanddeath/good-selfatari.2.sgf
30 reg_genmove w
#? [G1]

40 reg_genmove b
#? [G1]
