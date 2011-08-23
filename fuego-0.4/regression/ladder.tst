#-----------------------------------------------------------------------------
# Tests for GoLadder and GoStaticLadder
#-----------------------------------------------------------------------------

loadsgf sgf/ladder/long-ladder.sgf

10 go_ladder C4
#? [unsettled]

loadsgf sgf/ladder/ladder-breaker.sgf

20 go_ladder C4
#? [escaped]

loadsgf sgf/ladder/ladder-no-breaker.sgf

30 go_ladder C17
#? [unsettled]

loadsgf sgf/ladder/ladder-edge.sgf

40 go_ladder C1
#? [unsettled]

45 go_static_ladder C1
#? [unsettled]

loadsgf sgf/ladder/ladder-edge-breaker.sgf

50 go_ladder C1
#? [escaped]

55 go_static_ladder C1
#? [escaped]

loadsgf sgf/ladder/long-ladder-captured.sgf

60 go_ladder C4
#? [captured]

loadsgf sgf/ladder/ladder-breaker-2.sgf

70 go_ladder D16
#? [unsettled]

loadsgf sgf/ladder/ladder-parallel.sgf

80 go_ladder D16
#? [captured]

loadsgf sgf/ladder/adjacent-blocks.sgf

90 go_ladder T15
#? [captured]

play b R19

100 go_ladder T15
#? [unsettled]

loadsgf sgf/ladder/ladder-triple-ko.sgf 28

# The following test needs the rules to be simple ko.
go_param_rules ko_rule simple

# This ladder causes a triple ko if the global ko rule is simple ko
# (see the continuation after 27 B H3 in the SGF file)
# Test for a bug that did not check if GO_MAX_NUM_MOVES was exceeded
110 go_ladder J3
#? [unsettled]
