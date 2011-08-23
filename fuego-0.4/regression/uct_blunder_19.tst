#-----------------------------------------------------------------------------
# Blunder avoidance tests on 19x19 for GoUctGlobalSearchPlayer
#-----------------------------------------------------------------------------

uct_param_player max_games 10000

loadsgf sgf/games/2008/KGS/8/21/Hellyta-Fuego.sgf 225

10 reg_genmove b
#? [A2|C6|E5]
# White threatens to play G2. The blunder occured, because W G2 was not
# explored at all during the search. It works since SVN 464, when a positive
# initialization was introduced in GoUctDefaultPriorKnowledge for moves
# that set a block into atari.

