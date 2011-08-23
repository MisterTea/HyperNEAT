#-----------------------------------------------------------------------------
# Tests for bug fixes in playouts of GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000
uct_stat_player_clear

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/CGOS/70767-test1.sgf 27

10 reg_genmove b
#? [B5]
# defending at H5 or J5 loses by 0.5. After B5 it is still complicated

loadsgf sgf/games/2007/CGOS/70767-test1.sgf 35

20 reg_genmove b
#? [J4]
# J4 is a safe win
# new PASS 20081106

21 sg_compare_float 0.6 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/70767-test1.sgf 40

reg_genmove w
30 sg_compare_float 0.45 uct_value
#? [-1]
# 30 eval is negative - W lost.

loadsgf sgf/games/2007/CGOS/70767-test1.sgf 50

reg_genmove w
40 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/games/2007/CGOS/70767-test1.sgf 52

reg_genmove w
50 sg_compare_float 0.2 uct_value
#? [-1]

loadsgf sgf/games/2007/CGOS/70767-test1.sgf 54

reg_genmove w
60 sg_compare_float 0.1 uct_value
#? [-1]

loadsgf sgf/games/2007/CGOS/70767-test2.sgf 38

reg_genmove w
70 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/games/2007/CGOS/70767-test2.sgf 39

reg_genmove b
80 sg_compare_float 0.5 uct_value
#? [1]

# todo: test for the whole sequence from 38.., that the eval is
# positive for Black and negative for White. Right now, the
# program is mostly pessimistic for whoever's turn it is.

loadsgf sgf/games/2007/CGOS/70767-test2.sgf 67
500 reg_genmove b
#? [A2]*
# E4 as played in the test works too, but A2 is much safer.
# new FAIL 20081106

loadsgf sgf/games/2007/CGOS/70767-test2.sgf 71
510 reg_genmove b
#? [D1]
# C9 as played in the test works too, but D1 is much safer.

loadsgf sgf/games/2007/CGOS/70767-test2.sgf 73
520 reg_genmove b
#? [A9]
# B must live.

loadsgf sgf/games/2007/CGOS/70767-test3.sgf 75
# todo: B is lost here. But program does not know it.
# move 79: tree starts out low, 0.30, but quickly rises to strongly positive
# move 81: starts at 0.61, rises to .84 in 50000 simulations, but B still lost.
# move 83: only now it sees it, value 0.01.

loadsgf sgf/games/2007/CGOS/71448.sgf 43
1000 reg_genmove b
#? [A4]
# B must live here. Then he wins the semeai about the big groups.

loadsgf sgf/games/2007/CGOS/71448.sgf 44
1010 reg_genmove b
#? [A4]*
# W must atari here. Then he kills the corner, and wins the semeai.

loadsgf sgf/games/2007/CGOS/71469-test.sgf
1520 reg_genmove b
#? [A4]
# Older versions of GoUctGlobalSearchPlayer did not explore A4, because it got
# a bad RAVE value, because A4 (as an atari defend) has no priority in the
# playout strategy, but capturing moves have. After W A4, Black played A4
# later, but these games were mostly lost due to the capture. This problem
# should be fixed with the new RAVE update-abort policy SG_RAVEABORT_BOTH,
# which also does not update moves, if the opponent played it first

loadsgf sgf/semeai/filllib-test.1.sgf
1530 reg_genmove b
#? [C9]*

loadsgf sgf/eyes/1.5/make-nakade.1.sgf 1
1540 reg_genmove b
#? [C8]*

1550 reg_genmove w
#? [C8]

loadsgf sgf/eyes/1.5/make-nakade.1.sgf 3
1560 reg_genmove b
#? [B8]

1570 reg_genmove w
#? [B8]

loadsgf sgf/eyes/1.5/make-nakade.1.sgf 5
1580 reg_genmove b
#? [B7]

1581 sg_compare_float 0.5 uct_value
#? [1]
# plays the killing move but value = 0.47

1590 reg_genmove w
#? [B7]

loadsgf sgf/eyes/1.5/make-nakade.2.sgf 1
1600 reg_genmove b
#? [B9|D8]
# new PASS 2008-12-05

1610 reg_genmove w
#? [B9|D8]*
# new FAIL 2008-02-19

loadsgf sgf/eyes/1.5/make-nakade.2.sgf 3
1620 reg_genmove b
#? [C8]

1621 sg_compare_float 0.5 uct_value
#? [1]

1630 reg_genmove w
#? [C8]

loadsgf sgf/uct/capture.1.sgf 14
reg_genmove b
1640 sg_compare_float 0.2 uct_value
#? [-1]

loadsgf sgf/uct/capture.1.sgf 16
reg_genmove b
1650 sg_compare_float 0.2 uct_value
#? [-1]

loadsgf sgf/uct/capture.1.sgf 17
1660 reg_genmove w
#? [G5]
# G5 is an obvious capture but GoUctGlobalSearchPlayer had trouble finding it.

1661 sg_compare_float 0.6 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/107433.sgf 44
1680 reg_genmove w
#? [B8|B9|E9|J4]
# must make living group here and fight lower right ko 
# to make it harder for B to win. 
# @todo I added J4 but not sure if it is good.

loadsgf sgf/games/2007/CGOS/107439.sgf 33
1690 reg_genmove b
#? [E3|H7]
# connect, fight is good for B. H7 atari first seems OK too.

loadsgf sgf/games/2007/CGOS/107439.sgf 27
1700 reg_genmove b
#? [C3|D3|G2]
# play more solidly, avoid bad aji as in game

loadsgf sgf/games/2007/CGOS/107439.sgf 29
1710 reg_genmove b
#? [D3|G2]*
# avoid ko here, it is dangerous

loadsgf sgf/games/2007/CGOS/107439.sgf 39
1720 reg_genmove b
#? [C2]
 
loadsgf sgf/games/2007/CGOS/107457.sgf 11
1730 reg_genmove b
#? [E2|E3]*
# must live on bottom edge now.
# new FAIL 20081106

loadsgf sgf/eyes/1/nakade.1.sgf 1
reg_genmove b
1740 sg_compare_float 0.5 uct_value
#? [1]
# it kills the big white group but thinks it loses.

loadsgf sgf/eyes/1/nakade.1.sgf 2
reg_genmove w
1750 sg_compare_float 0.5 uct_value
#? [-1]
# now it can see it - when it is atari.

loadsgf sgf/games/2007/simulation-bug-107407.sgf 58
reg_genmove w
1760 sg_compare_float 0.4 uct_value
#? [-1]*
# W lost but program thinks it's close.

loadsgf sgf/games/2007/simulation-bug-107407.sgf 60
reg_genmove w
1770 sg_compare_float 0.4 uct_value
#? [-1]
# W lost. 
# Too many targets to try to capture but nothing works.
# new PASS 20081106

loadsgf sgf/games/2007/simulation-bug-107407.sgf 62
reg_genmove w
1780 sg_compare_float 0.4 uct_value
#? [-1]*
# W lost.

loadsgf sgf/games/2007/simulation-bug-107407.2.sgf 66
reg_genmove w
1790 sg_compare_float 0.4 uct_value
#? [-1]
# W is lost. 
# new PASS 20081106

loadsgf sgf/games/2007/simulation-bug-107407.2.sgf 67
1800 reg_genmove b
#? [A4]
# Continuation of 1770.
# new PASS 20081106

1810 sg_compare_float 0.8 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/189721.sgf 35
1820 reg_genmove b
#? [B1|A8]*
# must live here, then game is a win.

1830 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/189715.sgf 71
1840 reg_genmove b
#? [B3]*
# safest way to live + win

1850 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/189715.sgf 81
1900 reg_genmove b
#? [E3]*
# final chance

1910 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/189715.sgf 82
1920 reg_genmove w
#? [E3]
# w can win the fight now.

1930 sg_compare_float 0.5 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/189715.sgf 83
reg_genmove b
1940 sg_compare_float 0.3 uct_value
#? [-1]
# now one group dies in the ko fight

loadsgf sgf/games/2007/CGOS/189676.sgf 46
1950 reg_genmove w
#? [B9|J4|J1|F9|G7]
# must attack corner
# B7 may be playable but really risky - not added
# new PASS 20081106

loadsgf sgf/games/2007/CGOS/189582.sgf 48
1960 reg_genmove w
#? [G7]
# must make eye here, otherwise loses lots of points in corner
# new PASS 20081106

loadsgf sgf/games/2007/CGOS/188524-variation-filling-bug.sgf 57
1970 reg_genmove b
#? [A4|B2]
# the capture heuristic is to blame here. In playouts
# both players capture at J4 and A4 respectively, before
# anything else. That exchange makes white's attack 
# at D2 go away.

loadsgf sgf/games/2007/CGOS/189930-variation.sgf 58
reg_genmove w
1980 sg_compare_float 0.3 uct_value
#? [-1]*
# W is lost but does not see it. Problem with filling move J5???

loadsgf sgf/games/2007/CGOS/189930-variation.sgf 60
reg_genmove w
1990 sg_compare_float 0.3 uct_value
#? [-1]*
# W is lost but does not see it. Problem with filling move J5???

loadsgf sgf/games/2007/CGOS/189930-variation.sgf 62
reg_genmove w
2000 sg_compare_float 0.3 uct_value
#? [-1]*
# W is lost but does not see it, even after J5 is played.

loadsgf sgf/games/2007/move88-selfatari-eval-bug.sgf 88
reg_genmove w
2010 sg_compare_float 0.9 uct_value
#? [1]*
# new FAIL 20081106

loadsgf sgf/games/2007/bookwhite-4.sgf
2020 reg_genmove w
#? [G9]
# W resigned, but can simply capture all.

loadsgf sgf/games/2007/uct160K-mogo10K-2.sgf 87
2030 reg_genmove b
#? [B2]
# B must capture to win by half a point.
# it misevaluates the seki and passes instead.

2040 sg_compare_float 0.8 uct_value
#? [1]*
# new FAIL 20081106

loadsgf sgf/games/2007/uct160K-mogo10K-2.sgf 89
2050 reg_genmove b
#? [PASS]
# passing wins by half a point. Destroying the seki loses.
# The UCT player used to not explore pass, if RAVE was enabled, because pass
# got a bad RAVE value. This test should succeed now since pass moves were
# excluded from RAVE updates.

2060 sg_compare_float 0.9 uct_value
#? [1]*
# new FAIL 20081106

loadsgf sgf/games/2007/CGOS/199550.sgf 27
2070 reg_genmove b
#? [B3]

loadsgf sgf/games/2007/CGOS/200109.sgf 9
2080 reg_genmove b
#? [G5]

loadsgf sgf/games/2007/CGOS/200163.sgf 11
2090 reg_genmove b
#? [C5]

loadsgf sgf/games/2007/CGOS/200163.sgf 12
2100 reg_genmove w
#? [C5]

loadsgf sgf/games/2007/CGOS/200368.sgf 15
2110 reg_genmove b
#? [F6]

loadsgf sgf/games/2007/CGOS/200368.sgf 49
2120 reg_genmove b
#? [C2]*
# defending at J4 loses.

loadsgf sgf/games/2007/CGOS/201395.sgf 18
2130 reg_genmove w
#? [B7]

#-----------------------------------------------------------------------------

uct_stat_player
# print player statistics to log file
