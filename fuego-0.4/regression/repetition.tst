#-----------------------------------------------------------------------------
# Test for superko moves.
#-----------------------------------------------------------------------------

loadsgf sgf/repetition/simpleko.sgf

go_param_rules ko_rule simple

10 is_legal w b1
#? [0]

go_param_rules ko_rule superko

11 is_legal w b1
#? [0]

go_param_rules ko_rule pos_superko

12 is_legal w b1
#? [0]

#-----------------------------------------------------------------------------

loadsgf sgf/repetition/long-life.sgf 4

go_param_rules ko_rule simple

20 is_legal w a4
#? [1]

go_param_rules ko_rule superko

21 is_legal w a4
#? [0]

go_param_rules ko_rule pos_superko

22 is_legal w a4
#? [0]

#-----------------------------------------------------------------------------

# Last move 105 B C9 repeats 99
loadsgf sgf/repetition/repetition-1.sgf 105

go_param_rules ko_rule simple

30 is_legal b c9
#? [1]

go_param_rules ko_rule superko

31 is_legal b c9
#? [0]

go_param_rules ko_rule pos_superko

32 is_legal b c9
#? [0]

#-----------------------------------------------------------------------------

loadsgf sgf/repetition/triple-ko.sgf 6

go_param_rules ko_rule simple

40 is_legal w e7
#? [1]

go_param_rules ko_rule superko

41 is_legal w e7
#? [0]

go_param_rules ko_rule pos_superko

42 is_legal w e7
#? [0]

#-----------------------------------------------------------------------------

loadsgf sgf/repetition/positional-superko.sgf 3

go_param_rules ko_rule simple

50 is_legal w b9
#? [1]

go_param_rules ko_rule superko

51 is_legal w b9
#? [1]

go_param_rules ko_rule pos_superko

52 is_legal w b9
#? [0]

#-----------------------------------------------------------------------------
