#-----------------------------------------------------------------------------
# Test cases from full board semeai solutions for static safety solver
#-----------------------------------------------------------------------------

loadsgf sgf/semeai-exact/FS1-solution-white.sgf 42

10 go_safe static black
#? [78 .*]

20 go_safe static white
#? [136 .*]

loadsgf sgf/semeai-exact/FS2-solution-white.sgf 6

30 go_safe static black
#? [81 .*]

40 go_safe static white
#? [105 .*]

loadsgf sgf/semeai-exact/FS2-solution-white.sgf 21

50 go_safe static black
#? [109 .*]

60 go_safe static white
#? [112 .*]

loadsgf sgf/semeai-exact/FS2-solution-white.sgf 78

70 go_safe static black
#? [133 .*]

80 go_safe static white
#? [182 .*]*

loadsgf sgf/semeai-exact/FS2-solution-black.sgf 23

90 go_safe static black
#? [89 .*]

100 go_safe static white
#? [105 .*]
