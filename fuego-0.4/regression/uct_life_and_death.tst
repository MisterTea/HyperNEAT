#-----------------------------------------------------------------------------
# Life and Death tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/248520.sgf 25
10 reg_genmove b
#? [J4|H5]*
# keeping w separated is easiest way to keep both sides dead

loadsgf sgf/games/2008/CGOS/248520.sgf 31
30 reg_genmove b
#? [J4|H5]*
# keeping w separated, kill top, is enough to win game

loadsgf sgf/games/2008/CGOS/248520.sgf 43
50 reg_genmove b
#? [J4]*
# keeping w separated, kill one side, is enough to win game

loadsgf sgf/games/2008/mogo-ko+ld-problem.sgf
90 reg_genmove w
#? [J4]
# apparently Mogo cannot solve this one.

loadsgf sgf/lifeanddeath/eye-tricky.sgf 1
110 reg_genmove b
#? [D3]
# only way to live.

130 reg_genmove w
#? [D3|F1|F3]
# three ways to kill.

loadsgf sgf/lifeanddeath/eye-tricky.sgf 2
150 reg_genmove w
#? [D3]

loadsgf sgf/lifeanddeath/eye-tricky.sgf 4
170 reg_genmove w
#? [E2]

loadsgf sgf/lifeanddeath/eye-tricky.sgf 6
190 reg_genmove w
#? [F3]

loadsgf sgf/games/2008/gnu-uct3K-28.sgf 40
210 reg_genmove w
#? [G8|H9]

loadsgf sgf/games/2008/KGS/7/26/MatroidX-4.sgf 36
220 reg_genmove w
#? [D2|E2]*

loadsgf sgf/games/2008/KGS/7/26/MatroidX-4.sgf 37
230 reg_genmove b
#? [D2]

loadsgf sgf/games/2008/KGS/7/26/MatroidX-4.sgf 39
240 reg_genmove b
#? [A1]*
# play ko for kill
# new FAIL 20081106

loadsgf sgf/games/2008/KGS/7/26/MatroidX-5.sgf 43
250 reg_genmove b
#? [C8]
# defend territory, don't try to kill

loadsgf sgf/games/2008/CGOS/428168.sgf 36
260 reg_genmove w
#? [B9|D9]

loadsgf sgf/games/2008/CGOS/428168.sgf 37
270 reg_genmove b
#? [B9]

loadsgf sgf/games/2008/CGOS/428168.sgf 41
280 reg_genmove b
#? [A6]

loadsgf sgf/games/2008/CGOS/428168.sgf 42
290 reg_genmove w
#? [A6]*
# new FAIL 20081106

loadsgf sgf/games/2008/CGOS/428168.sgf 73
300 reg_genmove b
#? [B1]
# making 2 eyes is safest way, even though B wins the semeai

loadsgf sgf/games/2008/CGOS/397919.sgf 44
310 reg_genmove w
#? [A1]

loadsgf sgf/games/2008/CGOS/397919-variation.sgf 1
320 reg_genmove b
#? [A4]

loadsgf sgf/games/2008/CGOS/397739.sgf 44
330 reg_genmove w
#? [B2]*
# B2 seems 0.5 pt win. C4 leads to damezumari

loadsgf sgf/games/2008/CGOS/397651.sgf 60
340 reg_genmove w
#? [B2|C2|C1]*
# must defend bottom or lose at least 4 stones and game.
# interesting case for Niu?
# new FAIL 20081106

loadsgf sgf/games/2008/CGOS/397651.sgf 61
350 reg_genmove b
#? [B3]*

loadsgf sgf/games/2008/CGOS/397651.sgf 63
360 reg_genmove b
#? [C2]

loadsgf sgf/games/2008/CGOS/397651.sgf 65
370 reg_genmove b
#? [B2]

loadsgf sgf/games/2008/CGOS/397651.sgf 67
380 reg_genmove b
#? [A1]

loadsgf sgf/games/2008/CGOS/365007.sgf 39
390 reg_genmove b
#? [J7]
# nakade

loadsgf sgf/games/2008/CGOS/365007.sgf 52
400 reg_genmove w
#? [H8]

loadsgf sgf/games/2008/CGOS/543088.sgf 22
420 reg_genmove w
#? [B1]

loadsgf sgf/games/2008/CGOS/543088.sgf 23
430 reg_genmove b
#? [B1]*

loadsgf sgf/games/2008/CGOS/594998.sgf 33
440 reg_genmove b
#? [D8]*
# better aji to capture, leaves group small and obviously dead. 
# Also leaves us strong on outside.

loadsgf sgf/games/2008/CGOS/594998.sgf 39
450 reg_genmove b
#? [A9]*
# must kill. Current search apparently does not consider A9 for White.

loadsgf sgf/games/2008/CGOS/595383.sgf 40
460 reg_genmove w
#? [H9]
# new PASS 20081106

loadsgf sgf/games/2008/CGOS/595383.sgf 41
470 reg_genmove b
#? [H9]*

loadsgf sgf/games/2008/CGOS/595383.sgf 52
480 reg_genmove w
#? [J9]*
# ko, we can win.

loadsgf sgf/games/2008/CGOS/Fuego-519-2c-20080920080149.sgf 40
490 reg_genmove w
#? [F9|G9|H9]*
# simple mistake. Worth looking into. Problem with selfatari/throwin?

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081113151939.sgf 39
500 reg_genmove b
#? [B1]
# kill, avoid seki

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081113151939.sgf 40
510 reg_genmove w
#? [A2]
# make seki

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081113151939.sgf 41
520 reg_genmove b
#? [B1]

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081113151939.sgf 42
530 reg_genmove w
#? [A2]

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081113151939.sgf 43
540 reg_genmove b
#? [B1]

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081113151939.sgf 45
550 reg_genmove b
#? [B1]

loadsgf sgf/games/2008/CGOS/Fuego-671-2c-20081125070320.sgf 40
560 reg_genmove w
#? [J9]*
# alive with J9 and it seems a safe 0.5 point win.
# J6 also lives but not sure if it works for points.

loadsgf sgf/games/2008/CGOS/Fuego-671-2c-20081125113323.sgf 16
570 reg_genmove w
#? [D2|B6]*

loadsgf sgf/games/2008/CGOS/Fuego-671-2c-20081125113323.sgf 24
580 reg_genmove w
#? [B6]*

loadsgf sgf/games/2009/CGOS/702960.sgf 25
590 reg_genmove b
#? [A2]*
# Fuego likes only D1 and A3 which are ko.

loadsgf sgf/games/2009/CGOS/702960.sgf 35 
600 reg_genmove b
#? [D2]
# keep corner ko

loadsgf sgf/games/2009/CGOS/702960.sgf 37
610 reg_genmove b
#? [D2]
# keep corner ko
