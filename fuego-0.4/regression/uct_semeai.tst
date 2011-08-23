#-----------------------------------------------------------------------------
# Semeai tests.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/CGOS/196322-variation.2.sgf 45
10 reg_genmove b
#? [F3|H3|H5]*
# b can get seki or a favorable ko in this corner and win.
# 20080126 run: new FAIL.

20 sg_compare_float 0.5 uct_value
#? [1]
# B wins but the program does not understand why.

loadsgf sgf/games/2007/CGOS/196322-variation.2.sgf 47
reg_genmove b
30 sg_compare_float 0.3 uct_value
#? [-1]*
# B is lost but does not see it.

loadsgf sgf/games/2007/CGOS/196322-variation.2.sgf 49
reg_genmove b
40 sg_compare_float 0.3 uct_value
#? [-1]*
# B is lost but does not see it.

loadsgf sgf/games/2007/CGOS/196322-variation.1.sgf 33
50 reg_genmove b
#? [H5|G6|J6]
# H5 is simpler but it seems G6 also works - see 196322-variation.2.sgf
# J6 is probably OK but I did not research it.

loadsgf sgf/games/2007/CGOS/196322-variation.1.sgf 35
60 reg_genmove b
#? [J4|F3|G1|G4]
# maybe even more moves work.

loadsgf sgf/games/2007/CGOS/194557.sgf 43
70 reg_genmove b
#? [G5|G1]
# H3 as in the game loses quickly. G5 is much more complicated, 
# and probably good for B. G1 probably works but did not research in detail.

loadsgf sgf/games/2007/CGOS/200109.sgf 27
80 reg_genmove b
#? [G8]*
# does not work against strongest response F8 G9 F7, but best try by far.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 29
90 reg_genmove b
#? [G8]
# strengthened B on left side to make this into a good semeai problem.

100 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 31
110 reg_genmove b
#? [G9]*
# It loves G2 but that is wrong.

120 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 33
130 reg_genmove b
#? [F9]

140 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 35
150 reg_genmove b
#? [D8]

160 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 37
170 reg_genmove b
#? [C8]

180 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 39
190 reg_genmove b
#? [D9]

200 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 41
210 reg_genmove b
#? [B8]

220 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2008/CGOS/249009.sgf 59
230 reg_genmove b
#? [C9]*
# make eye to make seki, win game

240 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2008/CGOS/248559.sgf 47
250 reg_genmove b
#? [G1]
# win semeai - good ko for us

260 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 36
270 reg_genmove w
#? [A7|A8|A9|G9]
# win semeai - 3 vs 3 liberties

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 37
280 reg_genmove b
#? [G9]

290 sg_compare_float 0.6 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 38
reg_genmove w
300 sg_compare_float 0.4 uct_value
#? [-1]
# W lost.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 39
310 reg_genmove b
#? [E7|G7|J6]

320 sg_compare_float 0.6 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 40
reg_genmove w
330 sg_compare_float 0.4 uct_value
#? [-1]
# W lost.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 41
340 reg_genmove b
#? [G7|J6]

350 sg_compare_float 0.6 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 42
reg_genmove w
360 sg_compare_float 0.4 uct_value
#? [-1]
# W lost.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 43
370 reg_genmove b
#? [G7|A8]

380 sg_compare_float 0.6 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 44
reg_genmove w
390 sg_compare_float 0.05 uct_value
#? [-1]
# W lost.

loadsgf sgf/semeai/semeai-eval.1.sgf 1
400 reg_genmove b
#? [A4]*

410 sg_compare_float 0.6 uct_value
#? [1]*
# B won, but current program is unsure about bottom right - score around 0.5.

loadsgf sgf/semeai/semeai-eval.1.sgf 3
420 reg_genmove b
#? [H2]

430 sg_compare_float 0.6 uct_value
#? [1]
# B won, but current program is unsure about bottom right - score around 0.5.

loadsgf sgf/semeai/semeai-eval.1.sgf 5
440 reg_genmove b
#? [E3]

450 sg_compare_float 0.6 uct_value
#? [1]
# B won, but current program is unsure about bottom right - score around 0.5.
# unstable value, sometimes solved

loadsgf sgf/semeai/semeai-eval.1.sgf 7
460 reg_genmove b
#? [J2]

470 sg_compare_float 0.6 uct_value
#? [1]
# B won, but current program is unsure about bottom right - score around 0.5.
# unstable value, sometimes solved

loadsgf sgf/games/2008/KGS/7/27/Yuusen-Fuego9-3.sgf 33
480 reg_genmove b
#? [C1]
# currently unstable - sometimes the program converges on F2, sometimes C1.

loadsgf sgf/games/2008/CGOS/365007-variation.sgf 1
490 reg_genmove w
#? [H2|J1]*
# W can make big seki. Unstable search - why does fuego often not see this???

loadsgf sgf/games/2008/CGOS/595027.sgf 48
500 reg_genmove w
#? [B3]*
# wins semeai and game. Current search is unstable between A7 (loses) and B3.

loadsgf sgf/games/2008/CGOS/595267.sgf 57
510 reg_genmove w
#? [A6|A9|C9]*
# wins semeai and game.

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081109232055.sgf 55
520 reg_genmove b
#? [J8]*
# wins semeai and game.

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081109232055.sgf 57
530 reg_genmove b
#? [J8]*
# wins semeai and game.

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081109232055.sgf 59
540 reg_genmove b
#? [J8]*
# wins semeai and game.

#-----------------------------------------------------------------------------
# Systematic tests with simple semeai. In each case, the right semeai move
# is 1 point better than the obvious capture/escape move, and wins the game.

loadsgf sgf/semeai-exact/semeai-plain-2L.sgf 1
1000 reg_genmove b
#? [C8|C9]

1010 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-2L.sgf 3
1020 reg_genmove b
#? [C9]

1030 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-2L.sgf 5
1040 reg_genmove b
#? [F9]

1050 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-3L.sgf 1
1060 reg_genmove b
#? [C7|C8|C9]

1070 sg_compare_float 0.8 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-3L.sgf 3
1080 reg_genmove b
#? [C8|C9]

1090 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-3L.sgf 5
1100 reg_genmove b
#? [C9]

1110 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-3L.sgf 7
1120 reg_genmove b
#? [F9]

1130 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-4L.sgf 1
1140 reg_genmove b
#? [C6|C7|C8|C9]

1150 sg_compare_float 0.8 uct_value
#? [1]*

loadsgf sgf/semeai-exact/semeai-plain-4L.sgf 3
1160 reg_genmove b
#? [C7|C8|C9]

1170 sg_compare_float 0.8 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-4L.sgf 5
1180 reg_genmove b
#? [C8|C9]

1190 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-4L.sgf 7
1200 reg_genmove b
#? [C9]

1210 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-4L.sgf 9
1220 reg_genmove b
#? [F9]

1230 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-5L.sgf 1
1240 reg_genmove b
#? [C5|C6|C7|C8|C9]

1250 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-5L.sgf 3
1260 reg_genmove b
#? [C6|C7|C8|C9]

1270 sg_compare_float 0.6 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-5L.sgf 5
1280 reg_genmove b
#? [C7|C8|C9]

1290 sg_compare_float 0.8 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-5L.sgf 7
1300 reg_genmove b
#? [C8|C9]

1310 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-5L.sgf 9
1320 reg_genmove b
#? [C9]

1330 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-5L.sgf 11
1340 reg_genmove b
#? [F9]

1350 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-6L.sgf 1
1360 reg_genmove b
#? [C4|C5|C6|C7|C8|C9]

1370 sg_compare_float 0.6 uct_value
#? [1]*

loadsgf sgf/semeai-exact/semeai-plain-6L.sgf 3
1380 reg_genmove b
#? [C5|C6|C7|C8|C9]

1390 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-6L.sgf 5
1400 reg_genmove b
#? [C6|C7|C8|C9]

1410 sg_compare_float 0.6 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-6L.sgf 7
1420 reg_genmove b
#? [C7|C8|C9]

1430 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-6L.sgf 9
1440 reg_genmove b
#? [C8|C9]

1450 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-6L.sgf 11
1460 reg_genmove b
#? [C9]

1470 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-plain-6L.sgf 13
1480 reg_genmove b
#? [F9]

1490 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-1eye-1shared-0ext.sgf 1
1500 reg_genmove b
#? [E9]

1510 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-1eye-1shared-0ext.sgf 3
1520 reg_genmove b
#? [C9]

1530 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-1eye-1shared-0ext.sgf 5
1540 reg_genmove b
#? [G9]

1550 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-1eye-1shared-1+3ext.sgf 1
1560 reg_genmove b
#? [A9|E9]

1570 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-1eye-1shared-1+3ext.sgf 3
1580 reg_genmove b
#? [E9]

1590 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-1eye-1shared-1+3ext.sgf 5
1600 reg_genmove b
#? [C9]

1610 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-1eye-1shared-1+3ext.sgf 7
1620 reg_genmove b
#? [J9]

1630 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-2pteye-1shared-0+3ext.sgf 1
1640 reg_genmove b
#? [B9|C9|E9]

1650 sg_compare_float 0.8 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-2pteye-1shared-0+3ext.sgf 3
1660 reg_genmove b
#? [E9]

1670 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-2pteye-1shared-0+3ext.sgf 5
1680 reg_genmove b
#? [C9]

1690 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/semeai-exact/semeai-2pteye-1shared-0+3ext.sgf 7
1700 reg_genmove b
#? [J9]

1710 sg_compare_float 0.9 uct_value
#? [1]

