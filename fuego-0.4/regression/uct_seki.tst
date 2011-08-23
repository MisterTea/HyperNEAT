#-----------------------------------------------------------------------------
# Seki tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------


loadsgf sgf/games/2007/CGOS/193838.sgf 80
30 reg_genmove w
#? [J1]
# kill seki by ko fight to win this game.

loadsgf sgf/games/2007/CGOS/193838.sgf 81
50 reg_genmove b
#? [J6]
# make seki. Only chance to win this game.

loadsgf sgf/games/2007/CGOS/193838.sgf 82
70 reg_genmove w
#? [J1]
# kill seki by ko fight to win this game.

loadsgf sgf/games/2008/leela-seki-variation.sgf 45
90 reg_genmove b
#? [A8]
# kill, avoid seki.

loadsgf sgf/games/2008/leela-seki-variation.sgf 46
110 reg_genmove w
#? [A8]
# make seki.

loadsgf sgf/games/2008/leela-seki-variation.sgf 48
130 reg_genmove w
#? [A7]*
# make seki.

loadsgf sgf/games/2008/KGS/7/26/Newou-Fuego9.sgf 31
240 reg_genmove b
#? [F4]
# kill, avoid seki.

loadsgf sgf/games/2008/KGS/7/26/Newou-Fuego9.sgf 35
250 reg_genmove b
#? [D4]
# already is seki, F4 is bad attack and loses the game.

loadsgf sgf/games/2008/KGS/7/25/PaperTiger-Fuego.sgf 163
280 reg_genmove b
#? [Q13]*
# Q13 is the only way to attack. O12 leads to seki.
# new FAIL 2009-12-01

loadsgf sgf/games/2008/KGS/7/28/dege-Fuego9.sgf 31
310 reg_genmove b
#? [F1]*
# F1 kills, prevents seki.

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 1
330 reg_genmove b
#? [G9]*
# prevents seki.
# This position was analyzed by "bitti" in the German Go Bund forum
# http://www.dgob.de/yabbse/index.php?topic=3314.msg153446#msg153446

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 3
340 reg_genmove b
#? [J8]*

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 5
350 reg_genmove b
#? [E9|B9|A8]*
# E9 is good style, but others work too.

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 7
360 reg_genmove b
#? [J7|B9|A8]*

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 9
370 reg_genmove b
#? [B9|A8]*

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 11
380 reg_genmove b
#? [A8]

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 13
390 reg_genmove b
#? [A5]

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081113151939.sgf 59
450 reg_genmove b
#? [E9]
# E9 wins the endgame. Because of seki misevaluation, B plays 'safe'
# and loses this game.

loadsgf sgf/seki/sheppard-seki-2.sgf
470 reg_genmove b
#? [J3]
# program played A1 which is silly but not fatal.
