#-----------------------------------------------------------------------------
# Tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

loadsgf sgf/uct/vital-1.sgf

# Unfortunately with RAVE, it can take (much) more than 500 games until
# B1 in sgf/uct/vital-1.sgf is explored first. Whether this test fails with
# RAVE, depends on the random seed. Currently B1 also gets no prior knowledge
# initialization. Therefor, we run the test w/o RAVE for now.
uct_param_player max_games 500
uct_param_search rave 0

10 reg_genmove b
#? [B1]

11 reg_genmove w
#? [B1]

#-----------------------------------------------------------------------------
# Test MoGo like patterns in GoUctPolicy
#-----------------------------------------------------------------------------

loadsgf sgf/uct/patterns-cut2.sgf

20 uct_patterns
#? [D10 L10 C16 K16 Q16 R17]

loadsgf sgf/uct/mogo-slides-2006.sgf 6

30 uct_patterns
#? [G2 H2]

loadsgf sgf/uct/mogo-slides-2006.sgf 7

40 uct_patterns
#? [G2]

loadsgf sgf/uct/mogo-slides-2006.sgf 8

50 uct_patterns
#? [G3 H3]

loadsgf sgf/uct/mogo-slides-2006.sgf 11

60 uct_patterns
#? [F4 H4]

loadsgf sgf/uct/mogo-slides-2006.sgf 12

70 uct_patterns
#? [F3 H4 F5 G5]

loadsgf sgf/uct/mogo-slides-2006.sgf 31

80 uct_patterns
#? [E1 F1 E3 D4 J4 C5 J5 C7 H7 F8]

loadsgf sgf/uct/patterns-edge4.sgf

90 uct_patterns
#? [Q1 R1 Q4 R4]

loadsgf sgf/uct/mogo-tr6062-fig4-right.sgf 5

100 uct_patterns
#? []

loadsgf sgf/uct/mogo-tr6062-fig4-right.sgf 16

110 uct_patterns
#? [F3 E4 J4 E5 J5 F6 G6]

loadsgf sgf/uct/patterns-edge1.sgf

120 uct_patterns
#? [E1 G1 E2 G2]

#-----------------------------------------------------------------------------

# Test that pass move is played if it immediately wins under Tromp-Taylor
# rules (GoUctGlobalSearchPlayer::ComputeMove should check with
# GoBoardUtil::PassWins)

loadsgf sgf/games/2007/CGOS/189376.sgf 40
komi 7.5
go_rules cgos

200 reg_genmove w
#? [PASS]

#-----------------------------------------------------------------------------
