#-----------------------------------------------------------------------------
# Tests from games played at ICGC 2008
#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/ICGC2008/9/02-Fuego-Steenvreter.sgf 36
10 reg_genmove w
#? [D8]
# the corner is complicated but I think this is better

loadsgf sgf/games/2008/ICGC2008/9/02-Fuego-Steenvreter.sgf 38
20 reg_genmove w
#? [E9]*
# the corner is complicated but I think this is better

loadsgf sgf/games/2008/ICGC2008/9/02-Fuego-Steenvreter.sgf 48
30 reg_genmove w
#? [C9]*

loadsgf sgf/games/2008/ICGC2008/9/02-Fuego-Steenvreter.sgf 51
40 reg_genmove b
#? [A8]*
# takes W eyes. Best chance for B

loadsgf sgf/games/2008/ICGC2008/9/02-Fuego-Steenvreter.sgf 56
50 reg_genmove w
#? [A3]*
# W can win this fight.
# The move in the game is a blunder, not sente.

loadsgf sgf/games/2008/ICGC2008/9/02-Fuego-Steenvreter.sgf 75
60 reg_genmove b
#? [!G7]

# loadsgf sgf/games/2008/ICGC2008/9/02-Steenvreter-Fuego.sgf 5
# @todo maybe this opening is bad already. But not sure what else to do.

loadsgf sgf/games/2008/ICGC2008/9/02-Steenvreter-Fuego.sgf 19
70 reg_genmove b
#? [E9]

loadsgf sgf/games/2008/ICGC2008/9/02-Steenvreter-Fuego.sgf 27
80 reg_genmove b
#? [G3]*
# B may be behind but this keeps the game close

# Round 3 vs Go Intellect: no test cases

# loadsgf sgf/games/2008/ICGC2008/9/04-Fuego-ManyFaces.sgf 8
# seems heavy. maybe D4,D5,G4 instead?

loadsgf sgf/games/2008/ICGC2008/9/04-Fuego-ManyFaces.sgf 12
90 reg_genmove w
#? [F8]

loadsgf sgf/games/2008/ICGC2008/9/04-Fuego-ManyFaces.sgf 13
100 reg_genmove b
#? [E7]
# good move.

loadsgf sgf/games/2008/ICGC2008/9/04-Fuego-ManyFaces.sgf 20
110 reg_genmove w
#? [B4|C7]

loadsgf sgf/games/2008/ICGC2008/9/04-Fuego-ManyFaces.sgf 44
120 reg_genmove w
#? [A2]*

# loadsgf sgf/games/2008/ICGC2008/9/04-ManyFaces-Fuego.sgf 9
# is there something stronger here? B6 or H6 or D2?

loadsgf sgf/games/2008/ICGC2008/9/04-ManyFaces-Fuego.sgf 17
130 reg_genmove b
#? [C5]*
# I think the reason for the loss is that B is too optimistic about
# killing either the top right corner or the bottom left.

loadsgf sgf/games/2008/ICGC2008/9/05-Fuego-Yogo.sgf 12
140 reg_genmove w
#? [E7]*
# good move according to Yu Ping 6 Dan professional

loadsgf sgf/games/2008/ICGC2008/9/05-Fuego-Yogo.sgf 22
150 reg_genmove w
#? [A4]*
# A3 is a typical mistake of MCTS programs according to Yu Ping.
# His book is full of such traps.

loadsgf sgf/games/2008/ICGC2008/9/05-Fuego-Yogo.sgf 32
160 reg_genmove w
#? [F8]*
# the correct move according to Yu Ping.

loadsgf sgf/games/2008/ICGC2008/9/05-Fuego-Yogo.sgf 38
170 reg_genmove w
#? [C7]*
# the only move according to Yu Ping.

loadsgf sgf/games/2008/ICGC2008/9/05-Fuego-Yogo.sgf 59
180 reg_genmove b
#? [H1]*
# according to Yu Ping A8 is the losing move, and B might have won
# otherwise. I am not sure what B's best move is, though.
# I tried B8 but it seems to lose by 0.5. After G9, G1 seems too good for
# White. So maybe H1 is the best try. But to me it seems W has already won.

loadsgf sgf/games/2008/ICGC2008/9/05-Yogo-Fuego.sgf 17
190 reg_genmove b
#? [D9]
# try to kill this corner. Otherwise game is too hard.

loadsgf sgf/games/2008/ICGC2008/9/05-Yogo-Fuego.sgf 19
200 reg_genmove b
#? [C3]*
# This seems better. Fuego is quite optimistic after this move. May still
# be bad though - these positions are misevaluated. See the file
# 05-Yogo-Fuego-variation-miseval.sgf.

# Round 6 vs Golois: no test cases

loadsgf sgf/games/2008/ICGC2008/9/07-Fuego-MoGo.sgf 8
210 reg_genmove w
#? [F2|G3]
# E3 cannot be right. Not sure what's right.

loadsgf sgf/games/2008/ICGC2008/9/07-Fuego-MoGo.sgf 14
220 reg_genmove w
#? [F7]*

loadsgf sgf/games/2008/ICGC2008/9/07-MoGo-Fuego.sgf 7
230 reg_genmove b
#? [F6]
# I think this is better. @todo add to opening book?

loadsgf sgf/games/2008/ICGC2008/9/07-MoGo-Fuego.sgf 18
240 reg_genmove w
#? [C5]*
# I think this is better than what MoGo played.

loadsgf sgf/games/2008/ICGC2008/9/07-MoGo-Fuego.sgf 22
250 reg_genmove w
#? [C3]
# I think this is better than what MoGo played.

loadsgf sgf/games/2008/ICGC2008/9/07-MoGo-Fuego.sgf 36
260 reg_genmove w
#? [H3]
# Yu Ping says this may be better than what MoGo played. Fuego agrees.

loadsgf sgf/games/2008/ICGC2008/9/07-MoGo-Fuego.sgf 53
270 reg_genmove b
#? [C8]*
# Throw-in is typical bad MCTS move.

loadsgf sgf/games/2008/ICGC2008/9/07-MoGo-Fuego.sgf 59
280 reg_genmove b
#? [A4]*
# This move was praised by Yu Ping. He said B wins by 0.5 now.

loadsgf sgf/games/2008/ICGC2008/9/08-Fuego-Leela.sgf 26
290 reg_genmove w
#? [J5|J6]*
# it seems safer to not give Black the option of living in the corner

loadsgf sgf/games/2008/ICGC2008/9/08-Fuego-Leela.sgf 28
300 reg_genmove w
#? [J5|J6|E4]
# H3 in the game looks weird

loadsgf sgf/games/2008/ICGC2008/9/08-Fuego-Leela.sgf 29
310 reg_genmove b
#? [G2]*
# I think this is stronger for B

loadsgf sgf/games/2008/ICGC2008/9/08-Fuego-Leela.sgf 35
320 reg_genmove b
#? [E2]*
# B must live here and fight on the other side. but it is hard for B now.

loadsgf sgf/games/2008/ICGC2008/9/08-Fuego-Leela.sgf 42
330 reg_genmove w
#? [F1]
# The immediate way to win. Program can not see this kind of approach move.
# similar to Yogo game (case 150 above)

loadsgf sgf/games/2008/ICGC2008/9/08-Fuego-Leela.sgf 46
340 reg_genmove w
#? [F1]

loadsgf sgf/games/2008/ICGC2008/9/08-Fuego-Leela.sgf 48
350 reg_genmove w
#? [D2]*

# loadsgf sgf/games/2008/ICGC2008/9/08-Leela-Fuego.sgf 
# maybe this early opening is already bad for Black.

loadsgf sgf/games/2008/ICGC2008/9/08-Leela-Fuego.sgf 15
360 reg_genmove b
#? [D8|C8]

loadsgf sgf/games/2008/ICGC2008/9/08-Leela-Fuego.sgf 19
370 reg_genmove b
#? [C3]
# This opening is already bad. B must fight. C3 looks like a possible start.

loadsgf sgf/games/2008/ICGC2008/9/09-Aya-Fuego.sgf 19
380 reg_genmove b
#? [E8]*
# I don't like the G6 push. Maybe E8, or attack the bottom left first?

loadsgf sgf/games/2008/ICGC2008/9/09-Aya-Fuego.sgf  29
390 reg_genmove b
#? [H6|E1]
# H6 is certainly bigger than B7. But maybe attack the bottom corner first.
# is E1 best?

loadsgf sgf/games/2008/ICGC2008/9/09-Aya-Fuego.sgf  30
400 reg_genmove w
#? [H6]

# loadsgf sgf/games/2008/ICGC2008/9/09-Aya-Fuego.sgf 32
# this way W just dies. Is there another way?

loadsgf sgf/games/2008/ICGC2008/9/09-Aya-Fuego.sgf  33
410 reg_genmove b
#? [E1]

loadsgf sgf/games/2008/ICGC2008/9/09-Aya-Fuego.sgf  39
420 reg_genmove b
#? [D4]

# loadsgf sgf/games/2008/ICGC2008/9/09-Fuego-Aya.sgf 20
# D2 may be better. Not sure.

loadsgf sgf/games/2008/ICGC2008/9/09-Fuego-Aya.sgf 22
430 reg_genmove w
#? [G8]*
# I think this is the reason W lost. But W is still very optimistic here.
# May be a good test case to look deeper into.
# Only at move 32 the evaluation goes negative.
