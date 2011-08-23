#-----------------------------------------------------------------------------
# Blunders playing near dead stones from Fuego 19x19 games. 
# Plays that do not make sense because nearby own stones are dead, 
# e.g. a cut where the other cutting stone is dead.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/31410.sgf 139
10 reg_genmove black
#? [!(K8|K5)]

loadsgf sgf/games/2008/CGOS/31410.sgf 141
20 reg_genmove black
#? [!K5]

loadsgf sgf/games/2008/CGOS/31423.sgf 129
30 reg_genmove black
#? [!G13]*

loadsgf sgf/games/2008/CGOS/31423.sgf 133
40 reg_genmove black
#? [!H12]
