#-----------------------------------------------------------------------------
# Tests from games played by professionals
#-----------------------------------------------------------------------------

loadsgf sgf/games/pro-9x9/mz1.sgf 21
10 reg_genmove b
#? [E9]

loadsgf sgf/games/pro-9x9/mz1.sgf 22
20 reg_genmove w
#? [F9]

loadsgf sgf/games/pro-9x9/zhou9a.sgf 
loadsgf sgf/games/pro-9x9/zhou9a.sgf
 
loadsgf sgf/games/pro-9x9/zhou9a.sgf 20
30 reg_genmove w
#? [B7|H7]
# either of those seem better than D3.

loadsgf sgf/games/pro-9x9/zhou9a.sgf 60
reg_genmove w

40
#? [C9]*

loadsgf sgf/games/2008/ICGC2008/9/02-Fuego-Steenvreter.sgf 51
40 reg_genmove b
#? [A8]*
# takes W eyes. Best chance for B
