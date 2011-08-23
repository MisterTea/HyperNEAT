#-----------------------------------------------------------------------------
# Check the direction of play, usually
# in the opening and early middle game.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000
komi 7.5

loadsgf sgf/games/2008/gnu-uct3K-30.sgf 12
10 reg_genmove w
#? [F3]
# blocking here gives a strong position. Saving H4 is wrong.

