#-----------------------------------------------------------------------------
# Regression tests related to bugs with free handicap setup.
#-----------------------------------------------------------------------------

boardsize 19
clear_board
set_free_handicap c16 e16 p16 r16 c10 d4 k4 q4 f3
reg_genmove w

10 sg_compare_float 0.3 uct_value
#? [-1]
# W should have less that 0.3 chance of winning with 9 handicap.
